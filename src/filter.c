/**
 * @file filter.c
 * @brief Uncached query implementation.
 * 
 * Uncached queries (filters) are stateless objects that do not cache their 
 * results. This file contains the creation and validation of uncached queries
 * and code for query iteration.
 * 
 * There file contains the implementation for term queries and filters. Term 
 * queries are uncached queries that only apply to a single term. Filters are
 * uncached queries that support multiple terms. Filters are built on top of
 * term queries: before iteration a filter will first find a "pivot" term (the
 * term with the smallest number of elements), and create a term iterator for
 * it. The output of that term iterator is then evaluated against the rest of
 * the terms of the filter.
 * 
 * Cached queries and observers are built using filters.
 */

#include "private_api.h"
#include <ctype.h>

ecs_filter_t ECS_FILTER_INIT = { .hdr = { .magic = ecs_filter_t_magic }};

/* Helper type for passing around context required for error messages */
typedef struct {
    const ecs_world_t *world;
    ecs_filter_t *filter;
    ecs_term_t *term;
    int32_t term_index;
} ecs_filter_finalize_ctx_t;

static
char* flecs_filter_str(
    const ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_filter_finalize_ctx_t *ctx,
    int32_t *term_start_out);

static
void flecs_filter_error(
    const ecs_filter_finalize_ctx_t *ctx,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);

    int32_t term_start = 0;

    char *expr = NULL;
    if (ctx->filter) {
        expr = flecs_filter_str(ctx->world, ctx->filter, ctx, &term_start);
    } else {
        expr = ecs_term_str(ctx->world, ctx->term);
    }
    const char *name = NULL;
    if (ctx->filter && ctx->filter->entity) {
        name = ecs_get_name(ctx->filter->world, ctx->filter->entity);
    }
    ecs_parser_errorv(name, expr, term_start, fmt, args);
    ecs_os_free(expr);

    va_end(args);
}

static
int flecs_term_id_finalize_flags(
    ecs_term_id_t *term_id,
    ecs_filter_finalize_ctx_t *ctx)
{
    if ((term_id->flags & EcsIsEntity) && (term_id->flags & EcsIsVariable)) {
        flecs_filter_error(ctx, "cannot set both IsEntity and IsVariable");
        return -1;
    }

    if (!(term_id->flags & EcsIsEntity) && !(term_id->flags & EcsIsVariable)) {
        if (term_id->id || term_id->name) {
            if (term_id->id == EcsThis || 
                term_id->id == EcsWildcard || 
                term_id->id == EcsAny || 
                term_id->id == EcsVariable) 
            {
                /* Builtin variable ids default to variable */
                term_id->flags |= EcsIsVariable;
            } else {
                term_id->flags |= EcsIsEntity;
            }
        }
    }

    if (term_id->flags & EcsParent) {
        term_id->flags |= EcsUp;
        term_id->trav = EcsChildOf;
    }

    if ((term_id->flags & EcsCascade) && !(term_id->flags & (EcsUp|EcsDown))) {
        term_id->flags |= EcsUp;
    }

    if ((term_id->flags & (EcsUp|EcsDown)) && !term_id->trav) {
        term_id->trav = EcsIsA;
    }

    if (term_id->trav && !(term_id->flags & EcsTraverseFlags)) {
        term_id->flags |= EcsUp;
    }

    return 0;
}

static
int flecs_term_id_lookup(
    const ecs_world_t *world,
    ecs_entity_t scope,
    ecs_term_id_t *term_id,
    bool free_name,
    ecs_filter_finalize_ctx_t *ctx)
{
    char *name = term_id->name;
    if (!name) {
        return 0;
    }

    if (term_id->flags & EcsIsVariable) {
        if (!ecs_os_strcmp(name, "This")) {
            term_id->id = EcsThis;
            if (free_name) {
                ecs_os_free(term_id->name);
            }
            term_id->name = NULL;
        }
        return 0;
    }

    ecs_assert(term_id->flags & EcsIsEntity, ECS_INTERNAL_ERROR, NULL);

    if (ecs_identifier_is_0(name)) {
        if (term_id->id) {
            flecs_filter_error(ctx, "name '0' does not match entity id");
            return -1;
        }
        return 0;
    }

    ecs_entity_t e = ecs_lookup_symbol(world, name, true);
    if (scope && !e) {
        e = ecs_lookup_child(world, scope, name);
    }

    if (!e) {
        flecs_filter_error(ctx, "unresolved identifier '%s'", name);
        return -1;
    }

    if (term_id->id && term_id->id != e) {
        char *e_str = ecs_get_fullpath(world, term_id->id);
        flecs_filter_error(ctx, "name '%s' does not match term.id '%s'", 
            name, e_str);
        ecs_os_free(e_str);
        return -1;
    }

    term_id->id = e;

    if (!ecs_os_strcmp(name, "*") || !ecs_os_strcmp(name, "_") || 
        !ecs_os_strcmp(name, "$") || !ecs_os_strcmp(name, ".")) 
    {
        term_id->flags &= ~EcsIsEntity;
        term_id->flags |= EcsIsVariable;
    }

    /* Check if looked up id is alive (relevant for numerical ids) */
    if (!ecs_is_alive(world, term_id->id)) {
        flecs_filter_error(ctx, "identifier '%s' is not alive", term_id->name);
        return -1;
    }

    if (free_name) {
        ecs_os_free(name);
    }

    term_id->name = NULL;

    return 0;
}

static
int flecs_term_ids_finalize(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_filter_finalize_ctx_t *ctx)
{
    ecs_term_id_t *src = &term->src;
    ecs_term_id_t *first = &term->first;
    ecs_term_id_t *second = &term->second;

    /* Include inherited components (like from prefabs) by default for src */
    if (!(src->flags & EcsTraverseFlags)) {
        src->flags |= EcsSelf | EcsUp;
    }

    /* Include subsets for component by default, to support inheritance */
    if (!(first->flags & EcsTraverseFlags)) {
        first->flags |= EcsSelf | EcsDown;
    }

    /* Traverse Self by default for pair target */
    if (!(second->flags & EcsTraverseFlags)) {
        second->flags |= EcsSelf;
    }

    /* Source defaults to This */
    if ((src->id == 0) && (src->name == NULL) && !(src->flags & EcsIsEntity)) {
        src->id = EcsThis;
        src->flags |= EcsIsVariable;
    }

    /* Initialize term identifier flags */
    if (flecs_term_id_finalize_flags(src, ctx)) {
        return -1;
    }
    if (flecs_term_id_finalize_flags(first, ctx)) {
        return -1;
    }

    if (flecs_term_id_finalize_flags(second, ctx)) {
        return -1;
    }

    /* Lookup term identifiers by name */
    if (flecs_term_id_lookup(world, 0, src, term->move, ctx)) {
        return -1;
    }
    if (flecs_term_id_lookup(world, 0, first, term->move, ctx)) {
        return -1;
    }

    ecs_entity_t first_id = 0;
    ecs_entity_t oneof = 0;
    if (first->flags & EcsIsEntity) {
        first_id = first->id;

        /* If first element of pair has OneOf property, lookup second element of
         * pair in the value of the OneOf property */
        oneof = flecs_get_oneof(world, first_id);
    }

    if (flecs_term_id_lookup(world, oneof, &term->second, term->move, ctx)) {
        return -1;
    }

    /* If source is 0, reset traversal flags */
    if (src->id == 0 && src->flags & EcsIsEntity) {
        src->flags &= ~EcsTraverseFlags;
        src->trav = 0;
    }
    /* If second is 0, reset traversal flags */
    if (second->id == 0 && second->flags & EcsIsEntity) {
        second->flags &= ~EcsTraverseFlags;
        second->trav = 0;
    }

    return 0;
}

static
ecs_entity_t flecs_term_id_get_entity(
    const ecs_term_id_t *term_id)
{
    if (term_id->flags & EcsIsEntity) {
        return term_id->id; /* Id is known */
    } else if (term_id->flags & EcsIsVariable) {
        /* Return wildcard for variables, as they aren't known yet */
        if (term_id->id != EcsAny) {
            /* Any variable should not use wildcard, as this would return all
             * ids matching a wildcard, whereas Any returns the first match */
            return EcsWildcard;
        } else {
            return EcsAny;
        }
    } else {
        return 0; /* Term id is uninitialized */
    }
}

static
int flecs_term_populate_id(
    ecs_term_t *term,
    ecs_filter_finalize_ctx_t *ctx)
{
    ecs_entity_t first = flecs_term_id_get_entity(&term->first);
    ecs_entity_t second = flecs_term_id_get_entity(&term->second);
    ecs_id_t role = term->id_flags;

    if (first & ECS_ID_FLAGS_MASK) {
        return -1;
    }
    if (second & ECS_ID_FLAGS_MASK) {
        return -1;
    }

    if ((second || term->second.flags == EcsIsEntity) && !role) {
        role = term->id_flags = ECS_PAIR;
    }

    if (!second && !ECS_HAS_ID_FLAG(role, PAIR)) {
        term->id = first | role;
    } else {
        if (!ECS_HAS_ID_FLAG(role, PAIR)) {
            flecs_filter_error(ctx, "invalid role for pair");
            return -1;
        }

        term->id = ecs_pair(first, second);
    }

    return 0;
}

static
int flecs_term_populate_from_id(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_filter_finalize_ctx_t *ctx)
{
    ecs_entity_t first = 0;
    ecs_entity_t second = 0;
    ecs_id_t role = term->id & ECS_ID_FLAGS_MASK;

    if (!role && term->id_flags) {
        role = term->id_flags;
        term->id |= role;
    }

    if (term->id_flags && term->id_flags != role) {
        flecs_filter_error(ctx, "mismatch between term.id & term.id_flags");
        return -1;
    }

    term->id_flags = role;

    if (ECS_HAS_ID_FLAG(term->id, PAIR)) {
        first = ECS_PAIR_FIRST(term->id);
        second = ECS_PAIR_SECOND(term->id);

        if (!first) {
            flecs_filter_error(ctx, "missing first element in term.id");
            return -1;
        }
        if (!second) {
            if (first != EcsChildOf) {
                flecs_filter_error(ctx, "missing second element in term.id");
                return -1;
            } else {
                /* (ChildOf, 0) is allowed so filter can be used to efficiently
                 * query for root entities */
            }
        }
    } else {
        first = term->id & ECS_COMPONENT_MASK;
        if (!first) {
            flecs_filter_error(ctx, "missing first element in term.id");
            return -1;
        }
    }

    ecs_entity_t term_first = flecs_term_id_get_entity(&term->first);
    if (term_first) {
        if (term_first != first) {
            flecs_filter_error(ctx, "mismatch between term.id and term.first");
            return -1;
        }
    } else {
        if (!(term->first.id = ecs_get_alive(world, first))) {
            term->first.id = first;
        }
    }

    ecs_entity_t term_second = flecs_term_id_get_entity(&term->second);
    if (term_second) {
        if (ecs_entity_t_lo(term_second) != second) {
            flecs_filter_error(ctx, "mismatch between term.id and term.second");
            return -1;
        }
    } else if (second) {
        if (!(term->second.id = ecs_get_alive(world, second))) {
            term->second.id = second;
        }
    }

    return 0;
}

static
int flecs_term_verify(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_filter_finalize_ctx_t *ctx)
{
    const ecs_term_id_t *first = &term->first;
    const ecs_term_id_t *second = &term->second;
    const ecs_term_id_t *src = &term->src;
    ecs_entity_t first_id = 0, second_id = 0;
    ecs_id_t role = term->id_flags;
    ecs_id_t id = term->id;

    if (first->flags & EcsIsEntity) {
        first_id = first->id;
    }
    if (second->flags & EcsIsEntity) {
        second_id = second->id;
    }

    if (role != (id & ECS_ID_FLAGS_MASK)) {
        flecs_filter_error(ctx, "mismatch between term.id_flags & term.id");
        return -1;
    }

    if (ecs_term_id_is_set(second) && !ECS_HAS_ID_FLAG(role, PAIR)) {
        flecs_filter_error(ctx, "expected PAIR flag for term with pair");
        return -1;
    } else if (!ecs_term_id_is_set(second) && ECS_HAS_ID_FLAG(role, PAIR)) {
        if (first_id != EcsChildOf) {
            flecs_filter_error(ctx, "unexpected PAIR flag for term without pair");
            return -1;
        } else {
            /* Exception is made for ChildOf so we can use (ChildOf, 0) to match
             * all entities in the root */
        }
    }

    if (!ecs_term_id_is_set(src)) {
        flecs_filter_error(ctx, "term.src is not initialized");
        return -1;
    }

    if (!ecs_term_id_is_set(first)) {
        flecs_filter_error(ctx, "term.first is not initialized");
        return -1;
    }

    if (ECS_HAS_ID_FLAG(role, PAIR)) {
        if (!ECS_PAIR_FIRST(id)) {
            flecs_filter_error(ctx, "invalid 0 for first element in pair id");
            return -1;
        }
        if ((ECS_PAIR_FIRST(id) != EcsChildOf) && !ECS_PAIR_SECOND(id)) {
            flecs_filter_error(ctx, "invalid 0 for second element in pair id");
            return -1;
        }

        if ((first->flags & EcsIsEntity) && 
            (ecs_entity_t_lo(first_id) != ECS_PAIR_FIRST(id))) 
        {
            flecs_filter_error(ctx, "mismatch between term.id and term.first");
            return -1;
        }
        if ((first->flags & EcsIsVariable) && 
            !ecs_id_is_wildcard(ECS_PAIR_FIRST(id)))
        {
            char *id_str = ecs_id_str(world, id);
            flecs_filter_error(ctx, 
                "expected wildcard for variable term.first (got %s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }

        if ((second->flags & EcsIsEntity) && 
            (ecs_entity_t_lo(second_id) != ECS_PAIR_SECOND(id))) 
        {
            flecs_filter_error(ctx, "mismatch between term.id and term.second");
            return -1;
        }
        if ((second->flags & EcsIsVariable) && 
            !ecs_id_is_wildcard(ECS_PAIR_SECOND(id))) 
        {
            char *id_str = ecs_id_str(world, id);
            flecs_filter_error(ctx, 
                "expected wildcard for variable term.second (got %s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }
    } else {
        ecs_entity_t component = id & ECS_COMPONENT_MASK;
        if (!component) {
            flecs_filter_error(ctx, "missing component id");
            return -1;
        }
        if ((first->flags & EcsIsEntity) && 
            (ecs_entity_t_lo(first_id) != ecs_entity_t_lo(component))) 
        {
            flecs_filter_error(ctx, "mismatch between term.id and term.first");
            return -1;
        }
        if ((first->flags & EcsIsVariable) && !ecs_id_is_wildcard(component)) {
            char *id_str = ecs_id_str(world, id);
            flecs_filter_error(ctx, 
                "expected wildcard for variable term.first (got %s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }
    }

    if (first_id) {
        if (ecs_term_id_is_set(second)) {
            ecs_flags32_t mask = EcsIsEntity | EcsIsVariable;
            if ((src->flags & mask) == (second->flags & mask)) {
                bool is_same = false;
                if (src->flags & EcsIsEntity) {
                    is_same = src->id == second->id;
                } else if (src->name && second->name) {
                    is_same = !ecs_os_strcmp(src->name, second->name);
                }

                if (is_same && ecs_has_id(world, first_id, EcsAcyclic)
                    && !ecs_has_id(world, first_id, EcsReflexive)) 
                {
                    char *pred_str = ecs_get_fullpath(world, term->first.id);
                    flecs_filter_error(ctx, "term with acyclic relationship"
                        " '%s' cannot have same subject and object",
                            pred_str);
                    ecs_os_free(pred_str);
                    return -1;
                }
            }
        }

        if (second_id && !ecs_id_is_wildcard(second_id)) {
            ecs_entity_t oneof = flecs_get_oneof(world, first_id);
            if (oneof) {
                if (!ecs_has_pair(world, second_id, EcsChildOf, oneof)) {
                    char *second_str = ecs_get_fullpath(world, second_id);
                    char *oneof_str = ecs_get_fullpath(world, oneof);
                    char *id_str = ecs_id_str(world, term->id);
                    flecs_filter_error(ctx, 
                        "invalid target '%s' for %s: must be child of '%s'",
                            second_str, id_str, oneof_str);
                    ecs_os_free(second_str);
                    ecs_os_free(oneof_str);
                    ecs_os_free(id_str);
                    return -1;
                }
            }
        }
    }

    if (term->src.trav) {
        if (!ecs_has_id(world, term->src.trav, EcsAcyclic)) {
            char *r_str = ecs_get_fullpath(world, term->src.trav);
            flecs_filter_error(ctx, 
                "cannot traverse non-Acyclic relationship '%s'", r_str);
            ecs_os_free(r_str);
            return -1;
        }
    }

    return 0;
}

static
int flecs_term_finalize(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_filter_finalize_ctx_t *ctx)
{
    ctx->term = term;

    ecs_term_id_t *src = &term->src;
    ecs_term_id_t *first = &term->first;
    ecs_term_id_t *second = &term->second;
    ecs_flags32_t first_flags = first->flags;
    ecs_flags32_t src_flags = src->flags;
    ecs_flags32_t second_flags = second->flags;

    if (term->id) {
        if (flecs_term_populate_from_id(world, term, ctx)) {
            return -1;
        }
    }

    if (flecs_term_ids_finalize(world, term, ctx)) {
        return -1;
    }

    /* If EcsVariable is used by itself, assign to predicate (singleton) */
    if ((src->id == EcsVariable) && (src->flags & EcsIsVariable)) {
        src->id = first->id;
        src->flags &= ~(EcsIsVariable | EcsIsEntity);
        src->flags |= first->flags & (EcsIsVariable | EcsIsEntity);
    }
    if ((second->id == EcsVariable) && (second->flags & EcsIsVariable)) {
        second->id = first->id;
        second->flags &= ~(EcsIsVariable | EcsIsEntity);
        second->flags |= first->flags & (EcsIsVariable | EcsIsEntity);
    }

    if (!term->id) {
        if (flecs_term_populate_id(term, ctx)) {
            return -1;
        }
    }

    ecs_entity_t first_id = 0;
    if (term->first.flags & EcsIsEntity) {
        first_id = term->first.id;
    }

    if (first_id) {
        /* If component id is final, don't attempt component inheritance */
        if (ecs_has_id(world, first_id, EcsFinal)) {
            if (first_flags & EcsDown) {
                flecs_filter_error(ctx, "final id cannot be traversed down");
                return -1;
            }
            first->flags &= ~EcsDown;
            first->trav = 0;
        }
        /* Don't traverse ids that cannot be inherited */
        if (ecs_has_id(world, first_id, EcsDontInherit)) {
            if (src_flags & (EcsUp | EcsDown)) {
                flecs_filter_error(ctx, 
                    "traversing not allowed for id that can't be inherited");
                return -1;
            }
            src->flags &= ~(EcsUp | EcsDown);
            src->trav = 0;
        }
        /* Add traversal flags for transitive relationships */
        if (!(second_flags & EcsTraverseFlags)) {
            if (ecs_has_id(world, first_id, EcsTransitive)) {
                second->flags |= EcsSelf|EcsUp|EcsTraverseAll;
                second->trav = first_id;
            }
        }
    }

    if (first->id == EcsVariable) {
        flecs_filter_error(ctx, "invalid $ for term.first");
        return -1;
    }

    if (term->id_flags & ECS_AND) {
        term->oper = EcsAndFrom;
        term->id &= ECS_COMPONENT_MASK;
        term->id_flags = 0;
    }

    if (term->oper == EcsAndFrom || term->oper == EcsOrFrom || term->oper == EcsNotFrom) {
        if (term->inout != EcsInOutDefault && term->inout != EcsInOutNone) {
            flecs_filter_error(ctx, 
                "invalid inout value for AndFrom/OrFrom/NotFrom term");
            return -1;
        }
    }

    if (flecs_term_verify(world, term, ctx)) {
        return -1;
    }

    term->idr = flecs_query_id_record_get(world, term->id);

    return 0;
}

ecs_id_t flecs_to_public_id(
    ecs_id_t id)
{
    if (ECS_PAIR_FIRST(id) == EcsUnion) {
        return ecs_pair(ECS_PAIR_SECOND(id), EcsWildcard);
    } else {
        return id;
    }
}

ecs_id_t flecs_from_public_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        ecs_entity_t first = ECS_PAIR_FIRST(id);
        ecs_id_record_t *idr = flecs_id_record_ensure(world, 
            ecs_pair(first, EcsWildcard));
        if (idr->flags & EcsIdUnion) {
            return ecs_pair(EcsUnion, first);
        }
    }

    return id;
}

bool ecs_identifier_is_0(
    const char *id)
{
    return id[0] == '0' && !id[1];
}

bool ecs_id_match(
    ecs_id_t id,
    ecs_id_t pattern)
{
    if (id == pattern) {
        return true;
    }

    if (ECS_HAS_ID_FLAG(pattern, PAIR)) {
        if (!ECS_HAS_ID_FLAG(id, PAIR)) {
            return false;
        }

        ecs_entity_t id_rel = ECS_PAIR_FIRST(id);
        ecs_entity_t id_obj = ECS_PAIR_SECOND(id);
        ecs_entity_t pattern_rel = ECS_PAIR_FIRST(pattern);
        ecs_entity_t pattern_obj = ECS_PAIR_SECOND(pattern);

        ecs_check(id_rel != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_check(id_obj != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_check(pattern_rel != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_check(pattern_obj != 0, ECS_INVALID_PARAMETER, NULL);
        
        if (pattern_rel == EcsWildcard) {
            if (pattern_obj == EcsWildcard || pattern_obj == id_obj) {
                return true;
            }
        } else if (pattern_rel == EcsFlag) {
            /* Used for internals, helps to keep track of which ids are used in
             * pairs that have additional flags (like OVERRIDE and TOGGLE) */
            if (ECS_HAS_ID_FLAG(id, PAIR) && !ECS_IS_PAIR(id)) {
                if (ECS_PAIR_FIRST(id) == pattern_obj) {
                    return true;
                }
                if (ECS_PAIR_SECOND(id) == pattern_obj) {
                    return true;
                }
            }
        } else if (pattern_obj == EcsWildcard) {
            if (pattern_rel == id_rel) {
                return true;
            }
        }
    } else {
        if ((id & ECS_ID_FLAGS_MASK) != (pattern & ECS_ID_FLAGS_MASK)) {
            return false;
        }

        if ((ECS_COMPONENT_MASK & pattern) == EcsWildcard) {
            return true;
        }
    }

error:
    return false;
}

bool ecs_id_is_pair(
    ecs_id_t id)
{
    return ECS_HAS_ID_FLAG(id, PAIR);
}

bool ecs_id_is_wildcard(
    ecs_id_t id)
{
    if ((id == EcsWildcard) || (id == EcsAny)) {
        return true;
    }

    bool is_pair = ECS_IS_PAIR(id);
    if (!is_pair) {
        return false;
    }

    ecs_entity_t first = ECS_PAIR_FIRST(id);
    ecs_entity_t second = ECS_PAIR_SECOND(id);

    return (first == EcsWildcard) || (second == EcsWildcard) ||
           (first == EcsAny) || (second == EcsAny);
}

bool ecs_id_is_valid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    if (!id) {
        return false;
    }
    if (ecs_id_is_wildcard(id)) {
        return false;
    }

    world = ecs_get_world(world);
    const ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (idr && idr->flags & EcsIdMarkedForDelete) {
        return false;
    }

    if (ECS_HAS_ID_FLAG(id, PAIR)) {
        if (!ECS_PAIR_FIRST(id)) {
            return false;
        }
        if (!ECS_PAIR_SECOND(id)) {
            return false;
        }
    } else if (id & ECS_ID_FLAGS_MASK) {
        if (!ecs_is_valid(world, id & ECS_COMPONENT_MASK)) {
            return false;
        }
    }

    return true;
}

ecs_flags32_t ecs_id_get_flags(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (idr) {
        return idr->flags;
    } else {
        return 0;
    }
}

bool ecs_term_id_is_set(
    const ecs_term_id_t *id)
{
    return id->id != 0 || id->name != NULL || id->flags & EcsIsEntity;
}

bool ecs_term_is_initialized(
    const ecs_term_t *term)
{
    return term->id != 0 || ecs_term_id_is_set(&term->first);
}

bool ecs_term_match_this(
    const ecs_term_t *term)
{
    return (term->src.flags & EcsIsVariable) && (term->src.id == EcsThis);
}

bool ecs_term_match_0(
    const ecs_term_t *term)
{
    return (term->src.id == 0) && (term->src.flags & EcsIsEntity);
}

int ecs_term_finalize(
    const ecs_world_t *world,
    ecs_term_t *term)
{
    ecs_filter_finalize_ctx_t ctx = {0};
    ctx.world = world;
    ctx.term = term;
    return flecs_term_finalize(world, term, &ctx);
}

ecs_term_t ecs_term_copy(
    const ecs_term_t *src)
{
    ecs_term_t dst = *src;
    dst.name = ecs_os_strdup(src->name);
    dst.first.name = ecs_os_strdup(src->first.name);
    dst.src.name = ecs_os_strdup(src->src.name);
    dst.second.name = ecs_os_strdup(src->second.name);
    return dst;
}

ecs_term_t ecs_term_move(
    ecs_term_t *src)
{
    if (src->move) {
        ecs_term_t dst = *src;
        src->name = NULL;
        src->first.name = NULL;
        src->src.name = NULL;
        src->second.name = NULL;
        dst.move = false;
        return dst;
    } else {
        ecs_term_t dst = ecs_term_copy(src);
        dst.move = false;
        return dst;
    }
}

void ecs_term_fini(
    ecs_term_t *term)
{
    ecs_os_free(term->first.name);
    ecs_os_free(term->src.name);
    ecs_os_free(term->second.name);
    ecs_os_free(term->name);

    term->first.name = NULL;
    term->src.name = NULL;
    term->second.name = NULL;
    term->name = NULL;
}

int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *f)
{
    int32_t i, term_count = f->term_count, field_count = 0;
    ecs_term_t *terms = f->terms;
    bool is_or = false, prev_or = false;
    ecs_entity_t prev_src_id = 0;
    int32_t filter_terms = 0;

    ecs_filter_finalize_ctx_t ctx = {0};
    ctx.world = world;
    ctx.filter = f;

    f->flags |= EcsFilterMatchOnlyThis;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        ctx.term_index = i;
        if (flecs_term_finalize(world, term, &ctx)) {
            return -1;
        }

        is_or = term->oper == EcsOr;
        field_count += !(is_or && prev_or);
        term->field_index = field_count - 1;

        if (prev_or && is_or) {
            if (prev_src_id != term->src.id) {
                flecs_filter_error(&ctx, "mismatching src.id for OR terms");
                return -1;
            }
        }

        prev_src_id = term->src.id;
        prev_or = is_or;

        if (ecs_term_match_this(term)) {
            ECS_BIT_SET(f->flags, EcsFilterMatchThis);
        } else {
            ECS_BIT_CLEAR(f->flags, EcsFilterMatchOnlyThis);
        }

        if (term->id == EcsPrefab) {
            ECS_BIT_SET(f->flags, EcsFilterMatchPrefab);
        }
        if (term->id == EcsDisabled && (term->src.flags & EcsSelf)) {
            ECS_BIT_SET(f->flags, EcsFilterMatchDisabled);
        }

        if (ECS_BIT_IS_SET(f->flags, EcsFilterNoData)) {
            term->inout = EcsInOutNone;
        }

        if (term->inout == EcsInOutNone) {
            filter_terms ++;
        }

        if (term->oper != EcsNot || !ecs_term_match_this(term)) {
            ECS_BIT_CLEAR(f->flags, EcsFilterMatchAnything);
        }

        if (term->idr) {
            if (ecs_os_has_threading()) {
                ecs_os_ainc(&term->idr->keep_alive);
            } else {
                term->idr->keep_alive ++;
            }
        }
    }

    f->field_count = field_count;

    if (filter_terms == term_count) {
        ECS_BIT_SET(f->flags, EcsFilterNoData);
    }

    return 0;
}

/* Implementation for iterable mixin */
static
void flecs_filter_iter_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter,
    ecs_term_t *filter)
{
    ecs_poly_assert(poly, ecs_filter_t);

    if (filter) {
        iter[1] = ecs_filter_iter(world, (ecs_filter_t*)poly);
        iter[0] = ecs_term_chain_iter(&iter[1], filter);
    } else {
        iter[0] = ecs_filter_iter(world, (ecs_filter_t*)poly);
    }
}

/* Implementation for dtor mixin */
static
void flecs_filter_fini(
    ecs_filter_t *filter)
{
    if (filter->terms) {
        int i, count = filter->term_count;
        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &filter->terms[i];
            if (term->idr) {
                if (ecs_os_has_threading()) {
                    ecs_os_adec(&term->idr->keep_alive);
                } else {
                    term->idr->keep_alive --;
                }
            }
            ecs_term_fini(&filter->terms[i]);
        }

        if (filter->terms_owned) {
            ecs_os_free(filter->terms);
        }
    }

    filter->terms = NULL;

    if (filter->owned) {
        ecs_os_free(filter);
    }
}

void ecs_filter_fini(
    ecs_filter_t *filter) 
{
    if (filter->owned && filter->entity) {
        /* If filter is associated with entity, use poly dtor path */
        ecs_delete(filter->world, filter->entity);
    } else {
        flecs_filter_fini(filter);
    }
}

ecs_filter_t* ecs_filter_init(
    ecs_world_t *world,
    const ecs_filter_desc_t *desc)    
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);

    ecs_filter_t *f = desc->storage;
    int32_t i, term_count = desc->terms_buffer_count, storage_count = 0, expr_count = 0;
    const ecs_term_t *terms = desc->terms_buffer;
    ecs_term_t *storage_terms = NULL, *expr_terms = NULL;

    if (f) {
        ecs_check(f->hdr.magic == ecs_filter_t_magic, 
            ECS_INVALID_PARAMETER, NULL);
        storage_count = f->term_count;
        storage_terms = f->terms;
        ecs_poly_init(f, ecs_filter_t);
    } else {
        f = ecs_poly_new(ecs_filter_t);
        f->owned = true;
    }
    if (!storage_terms) {
        f->terms_owned = true;
    }

    ECS_BIT_COND(f->flags, EcsFilterIsInstanced, desc->instanced);
    ECS_BIT_SET(f->flags, EcsFilterMatchAnything);
    f->flags |= desc->flags;

    /* If terms_buffer was not set, count number of initialized terms in
     * static desc::terms array */
    if (!terms) {
        ecs_check(term_count == 0, ECS_INVALID_PARAMETER, NULL);
        terms = desc->terms;
        for (i = 0; i < ECS_TERM_DESC_CACHE_SIZE; i ++) {
            if (!ecs_term_is_initialized(&terms[i])) {
                break;
            }
            term_count ++;
        }
    } else {
        ecs_check(term_count != 0, ECS_INVALID_PARAMETER, NULL);
    }

    /* If expr is set, parse query expression */
    const char *expr = desc->expr;
    ecs_entity_t entity = desc->entity;
    if (expr) {
#ifdef FLECS_PARSER
        const char *name = NULL;
        const char *ptr = desc->expr;
        ecs_term_t term = {0};
        int32_t expr_size = 0;

        if (entity) {
            name = ecs_get_name(world, entity);
        }

        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))){
            if (!ecs_term_is_initialized(&term)) {
                break;
            }

            if (expr_count == expr_size) {
                expr_size = expr_size ? expr_size * 2 : 8;
                expr_terms = ecs_os_realloc_n(expr_terms, ecs_term_t, expr_size);
            }

            expr_terms[expr_count ++] = term;
            if (ptr[0] == '\n') {
                break;
            }
        }

        if (!ptr) {
            /* Set terms in filter object to make sur they get cleaned up */
            f->terms = expr_terms;
            f->term_count = expr_count;
            f->terms_owned = true;
            goto error;
        }
#else
        (void)expr;
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* If storage is provided, make sure it's large enough */
    ecs_check(!storage_terms || storage_count >= (term_count + expr_count),
        ECS_INVALID_PARAMETER, NULL);

    if (term_count || expr_count) {
        /* If no storage is provided, create it */
        if (!storage_terms) {
            f->terms = ecs_os_calloc_n(ecs_term_t, term_count + expr_count);
            f->term_count = term_count + expr_count;
            ecs_assert(f->terms_owned == true, ECS_INTERNAL_ERROR, NULL);
        } else {
            f->terms = storage_terms;
            f->term_count = storage_count;
        }

        /* Copy terms to filter storage */
        for (i = 0; i < term_count; i ++) {
            f->terms[i] = ecs_term_copy(&terms[i]);
            /* Allow freeing resources from expr parser during finalization */
            f->terms[i].move = true;
        }

        /* Move expr terms to filter storage */
        for (i = 0; i < expr_count; i ++) {
            f->terms[i + term_count] = ecs_term_move(&expr_terms[i]);
            /* Allow freeing resources from expr parser during finalization */
            f->terms[i + term_count].move = true;
        }
        ecs_os_free(expr_terms);
    }

    /* Ensure all fields are consistent and properly filled out */
    if (ecs_filter_finalize(world, f)) {
        goto error;
    }

    /* Any allocated resources remaining in terms are now owned by filter */
    for (i = 0; i < f->term_count; i ++) {
        f->terms[i].move = false;
    }

    f->variable_names[0] = (char*)".";
    f->iterable.init = flecs_filter_iter_init;

    f->dtor = (ecs_poly_dtor_t)flecs_filter_fini;
    f->world = world;
    f->entity = entity;

    if (entity && f->owned) {
        EcsPoly *poly = ecs_poly_bind(world, entity, ecs_filter_t);
        poly->poly = f;
        ecs_poly_modified(world, entity, ecs_filter_t);
    }

    return f;
error:
    ecs_filter_fini(f);
    return NULL;
}

void ecs_filter_copy(
    ecs_filter_t *dst,
    const ecs_filter_t *src)   
{
    if (src == dst) {
        return;
    }

    if (src) {
        *dst = *src;

        int32_t i, term_count = src->term_count;
        dst->terms = ecs_os_malloc_n(ecs_term_t, term_count);
        dst->terms_owned = true;

        for (i = 0; i < term_count; i ++) {
            dst->terms[i] = ecs_term_copy(&src->terms[i]);
        }
    } else {
        ecs_os_memset_t(dst, 0, ecs_filter_t);
    }
}

void ecs_filter_move(
    ecs_filter_t *dst,
    ecs_filter_t *src)   
{
    if (src == dst) {
        return;
    }

    if (src) {
        *dst = *src;
        if (src->terms_owned) {
            dst->terms = src->terms;
            dst->terms_owned = true;
        } else {
            ecs_filter_copy(dst, src);
        }
        src->terms = NULL;
        src->term_count = 0;
    } else {
        ecs_os_memset_t(dst, 0, ecs_filter_t);
    }
}

static
void flecs_filter_str_add_id(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_term_id_t *id,
    bool is_subject,
    ecs_flags32_t default_traverse_flags)
{
    bool is_added = false;
    if (!is_subject || id->id != EcsThis) {
        if (id->flags & EcsIsVariable) {
            ecs_strbuf_appendlit(buf, "$");
        }
        if (id->id) {
            char *path = ecs_get_fullpath(world, id->id);
            ecs_strbuf_appendstr(buf, path);
            ecs_os_free(path);
        } else if (id->name) {
            ecs_strbuf_appendstr(buf, id->name);
        } else {
            ecs_strbuf_appendlit(buf, "0");
        }
        is_added = true;
    }

    ecs_flags32_t flags = id->flags;
    if (!(flags & EcsTraverseFlags)) {
        /* If flags haven't been set yet, initialize with defaults. This can
         * happen if an error is thrown while the term is being finalized */
        flags |= default_traverse_flags;
    }

    if ((flags & EcsTraverseFlags) != default_traverse_flags) {
        if (is_added) {
            ecs_strbuf_list_push(buf, ":", "|");
        } else {
            ecs_strbuf_list_push(buf, "", "|");
        }
        if (id->flags & EcsSelf) {
            ecs_strbuf_list_appendstr(buf, "self");
        }
        if (id->flags & EcsUp) {
            ecs_strbuf_list_appendstr(buf, "up");
        }
        if (id->flags & EcsDown) {
            ecs_strbuf_list_appendstr(buf, "down");
        }

        if (id->trav && (id->trav != EcsIsA)) {
            ecs_strbuf_list_push(buf, "(", "");

            char *rel_path = ecs_get_fullpath(world, id->trav);
            ecs_strbuf_appendstr(buf, rel_path);
            ecs_os_free(rel_path);

            ecs_strbuf_list_pop(buf, ")");
        }

        ecs_strbuf_list_pop(buf, "");
    }
}

static
void flecs_term_str_w_strbuf(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_strbuf_t *buf)
{
    const ecs_term_id_t *src = &term->src;
    const ecs_term_id_t *second = &term->second;

    uint8_t def_src_mask = EcsSelf|EcsUp;
    uint8_t def_first_mask = EcsSelf|EcsDown;
    uint8_t def_second_mask = EcsSelf;

    bool pred_set = ecs_term_id_is_set(&term->first);
    bool subj_set = !ecs_term_match_0(term);
    bool obj_set = ecs_term_id_is_set(second);

    if (term->first.flags & EcsIsEntity && term->first.id != 0) {
        if (ecs_has_id(world, term->first.id, EcsFinal)) {
            def_first_mask = EcsSelf;
        }
        if (ecs_has_id(world, term->first.id, EcsDontInherit)) {
            def_src_mask = EcsSelf;
        }
    }

    if (term->oper == EcsNot) {
        ecs_strbuf_appendlit(buf, "!");
    } else if (term->oper == EcsOptional) {
        ecs_strbuf_appendlit(buf, "?");
    }

    if (!subj_set) {
        flecs_filter_str_add_id(world, buf, &term->first, false, 
            def_first_mask);
        if (!obj_set) {
            ecs_strbuf_appendlit(buf, "()");
        } else {
            ecs_strbuf_appendlit(buf, "(0,");
            flecs_filter_str_add_id(world, buf, &term->second, false, 
                def_second_mask);
            ecs_strbuf_appendlit(buf, ")");
        }
    } else if (ecs_term_match_this(term) && 
        (src->flags & EcsTraverseFlags) == def_src_mask)
    {
        if (pred_set) {
            if (obj_set) {
                ecs_strbuf_appendlit(buf, "(");
            }
            flecs_filter_str_add_id(world, buf, &term->first, false, def_first_mask);   
            if (obj_set) {
                ecs_strbuf_appendlit(buf, ",");
                flecs_filter_str_add_id(
                    world, buf, &term->second, false, def_second_mask);
                ecs_strbuf_appendlit(buf, ")");
            }
        } else if (term->id) {
            char *str = ecs_id_str(world, term->id);
            ecs_strbuf_appendstr(buf, str);
            ecs_os_free(str);
        }
    } else {
        if (term->id_flags && !ECS_HAS_ID_FLAG(term->id_flags, PAIR)) {
            ecs_strbuf_appendstr(buf, ecs_id_flag_str(term->id_flags));
            ecs_strbuf_appendch(buf, '|');
        }

        flecs_filter_str_add_id(world, buf, &term->first, false, def_first_mask);
        ecs_strbuf_appendlit(buf, "(");
        if (term->src.flags & EcsIsEntity && term->src.id == term->first.id) {
            ecs_strbuf_appendlit(buf, "$");
        } else {
            flecs_filter_str_add_id(world, buf, &term->src, true, def_src_mask);
        }
        if (obj_set) {
            ecs_strbuf_appendlit(buf, ",");
            flecs_filter_str_add_id(world, buf, &term->second, false, def_second_mask);
        }
        ecs_strbuf_appendlit(buf, ")");
    }
}

char* ecs_term_str(
    const ecs_world_t *world,
    const ecs_term_t *term)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    flecs_term_str_w_strbuf(world, term, &buf);
    return ecs_strbuf_get(&buf);
}

static
char* flecs_filter_str(
    const ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_filter_finalize_ctx_t *ctx,
    int32_t *term_start_out)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(filter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;
    int32_t or_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];

        if (term_start_out && ctx) {
            if (ctx->term_index == i) {
                term_start_out[0] = ecs_strbuf_written(&buf);
                if (i) {
                    term_start_out[0] += 2; /* whitespace  + , */
                }
            }
        }

        if (i) {
            if (terms[i - 1].oper == EcsOr && term->oper == EcsOr) {
                ecs_strbuf_appendlit(&buf, " || ");
            } else {
                ecs_strbuf_appendlit(&buf, ", ");
            }
        }

        if (term->oper != EcsOr) {
            or_count = 0;
        }

        if (or_count < 1) {
            if (term->inout == EcsIn) {
                ecs_strbuf_appendlit(&buf, "[in] ");
            } else if (term->inout == EcsInOut) {
                ecs_strbuf_appendlit(&buf, "[inout] ");
            } else if (term->inout == EcsOut) {
                ecs_strbuf_appendlit(&buf, "[out] ");
            } else if (term->inout == EcsInOutNone) {
                ecs_strbuf_appendlit(&buf, "[none] ");
            }
        }

        if (term->oper == EcsOr) {
            or_count ++;
        }

        flecs_term_str_w_strbuf(world, term, &buf);
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

char* ecs_filter_str(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    return flecs_filter_str(world, filter, NULL, NULL);
}

int32_t ecs_filter_find_this_var(
    const ecs_filter_t *filter)
{
    ecs_check(filter != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (ECS_BIT_IS_SET(filter->flags, EcsFilterMatchThis)) {
        /* Filters currently only support the This variable at index 0. Only
         * return 0 if filter actually has terms for the This variable. */
        return 0;
    }

error:
    return -1;
}

/* Check if the id is a pair that has Any as first or second element. Any 
 * pairs behave just like Wildcard pairs and reuses the same data structures,
 * with as only difference that the number of results returned for an Any pair
 * is never more than one. This function is used to tell the difference. */
static
bool is_any_pair(
    ecs_id_t id)
{
    if (!ECS_HAS_ID_FLAG(id, PAIR)) {
        return false;
    }

    if (ECS_PAIR_FIRST(id) == EcsAny) {
        return true;
    }
    if (ECS_PAIR_SECOND(id) == EcsAny) {
        return true;
    }

    return false;
}

static
bool flecs_n_term_match_table(
    ecs_world_t *world,
    const ecs_term_t *term,
    const ecs_table_t *table,
    ecs_entity_t type_id,
    ecs_oper_kind_t oper,
    ecs_id_t *id_out,
    int32_t *column_out,
    ecs_entity_t *subject_out,
    int32_t *match_index_out,
    bool first,
    ecs_flags32_t iter_flags)
{
    (void)column_out;

    const ecs_type_t *type = ecs_get_type(world, type_id);
    ecs_assert(type != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t *ids = type->array;
    int32_t i, count = type->count;
    ecs_term_t temp = *term;
    temp.oper = EcsAnd;

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ecs_id_get_flags(world, id) & EcsIdDontInherit) {
            continue;
        }
        bool result;
        if (ECS_HAS_ID_FLAG(id, AND)) {
            ecs_oper_kind_t id_oper = EcsAndFrom;
            result = flecs_n_term_match_table(world, term, table, 
                id & ECS_COMPONENT_MASK, id_oper, id_out, column_out, 
                subject_out, match_index_out, first, iter_flags);
        } else {
            temp.id = id;
            result = flecs_term_match_table(world, &temp, table, id_out, 
                0, subject_out, match_index_out, first, iter_flags);
        }
        if (!result && oper == EcsAndFrom) {
            return false;
        } else
        if (result && oper == EcsOrFrom) {
            return true;
        }
    }

    if (oper == EcsAndFrom) {
        if (id_out) {
            id_out[0] = type_id;
        }
        return true;
    } else
    if (oper == EcsOrFrom) {
        return false;
    }

    return false;
}

bool flecs_term_match_table(
    ecs_world_t *world,
    const ecs_term_t *term,
    const ecs_table_t *table,
    ecs_id_t *id_out,
    int32_t *column_out,
    ecs_entity_t *subject_out,
    int32_t *match_index_out,
    bool first,
    ecs_flags32_t iter_flags)
{
    const ecs_term_id_t *src = &term->src;
    ecs_oper_kind_t oper = term->oper;
    const ecs_table_t *match_table = table;
    ecs_id_t id = term->id;

    ecs_entity_t src_id = src->id;
    if (ecs_term_match_0(term)) {
        if (id_out) {
            id_out[0] = id; /* If no entity is matched, just set id */
        }
        return true;
    }

    if (oper == EcsAndFrom || oper == EcsOrFrom) {
        return flecs_n_term_match_table(world, term, table, term->id, 
            term->oper, id_out, column_out, subject_out, match_index_out, first, 
            iter_flags);
    }

    /* If source is not This, search in table of source */
    if (!ecs_term_match_this(term)) {
        if (iter_flags & EcsIterEntityOptional) {
            /* Treat entity terms as optional */
            oper = EcsOptional;
        }

        match_table = ecs_get_table(world, src_id);
        if (match_table) {
        } else if (oper != EcsOptional) {
            return false;
        }
    } else {
        /* If filter contains This terms, a table must be provided */
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (!match_table) {
        return false;
    }

    ecs_entity_t source = 0;

    /* If first = false, we're searching from an offset. This supports returning
     * multiple results when using wildcard filters. */
    int32_t column = 0;
    if (!first && column_out && column_out[0] != 0) {
        column = column_out[0];
        if (column < 0) {
            /* In case column is not from This, flip sign */
            column = -column;
        }

        /* Remove base 1 offset */
        column --;
    }

    /* Find location, source and id of match in table type */
    ecs_table_record_t *tr = 0;
    bool is_any = is_any_pair(id);

    column = flecs_search_relation_w_idr(world, match_table,
        column, id, src->trav, src->flags, &source, id_out, &tr, term->idr);

    if (tr && match_index_out) {
        if (!is_any) {
            match_index_out[0] = tr->count;
        } else {
            match_index_out[0] = 1;
        }
    }

    bool result = column != -1;

    if (oper == EcsNot) {
        if (match_index_out) {
            match_index_out[0] = 1;
        }
        result = !result;
    }

    if (oper == EcsOptional) {
        result = true;
    }

    if (!result) {
        if (iter_flags & EcsFilterPopulate) {
            column = 0;
        } else {
            return false;
        }
    }

    if (!ecs_term_match_this(term)) {
        if (!source) {
            source = src_id;
        }
    }

    if (id_out && column < 0) {
        id_out[0] = id;
    }

    if (column_out) {
        if (column >= 0) {
            column ++;
            if (source != 0) {
                column *= -1; 
            }
            column_out[0] = column;
        } else {
            column_out[0] = 0;
        }
    }

    if (subject_out) {
        subject_out[0] = source;
    }

    return result;
}

bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_entity_t *sources,
    int32_t *match_indices,
    int32_t *matches_left,
    bool first,
    int32_t skip_term,
    ecs_flags32_t iter_flags)
{
    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    bool is_or = false;
    bool or_result = false;
    int32_t match_count = 1;
    if (matches_left) {
        match_count = *matches_left;
    }

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_oper_kind_t oper = term->oper;
        if (i == skip_term) {
            if (oper == EcsOr) {
                is_or = true;
                or_result = true;
            }
            if (oper != EcsAndFrom && oper != EcsOrFrom && oper != EcsNotFrom) {
                continue;
            }
        }

        ecs_term_id_t *src = &term->src;
        const ecs_table_t *match_table = table;
        int32_t t_i = term->field_index;

        if (!is_or && oper == EcsOr) {
            is_or = true;
            or_result = false;
        } else if (is_or && oper != EcsOr) {
            if (!or_result) {
                return false;
            }

            is_or = false;
        }

        ecs_entity_t src_id = src->id;
        if (!src_id) {
            if (ids) {
                ids[t_i] = term->id;
            }
            continue;
        }

        if (!ecs_term_match_this(term)) {
            match_table = ecs_get_table(world, src_id);
        } else {
            if (ECS_BIT_IS_SET(iter_flags, EcsIterIgnoreThis)) {
                or_result = true;
                continue;
            }
            
            /* If filter contains This terms, table must be provided */
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        }

        int32_t match_index = 0;

        bool result = flecs_term_match_table(world, term, match_table,
            ids ? &ids[t_i] : NULL, 
            columns ? &columns[t_i] : NULL, 
            sources ? &sources[t_i] : NULL, 
            &match_index,
            first,
            iter_flags);

        if (is_or) {
            or_result |= result;
            if (result) {
                /* If Or term matched, skip following Or terms */
                for (; i < count && terms[i].oper == EcsOr; i ++) { }
                i -- ;
            }
        } else if (!result) {
            return false;
        }

        if (first && match_index) {
            match_count *= match_index;
        }
        if (match_indices) {
            match_indices[t_i] = match_index;
        }
    }

    if (matches_left) {
        *matches_left = match_count;
    }

    return !is_or || or_result;
}

static
void term_iter_init_no_data(
    ecs_term_iter_t *iter)
{
    iter->term = (ecs_term_t){ .field_index = -1 };
    iter->self_index = NULL;
    iter->index = 0;
}

static
void term_iter_init_w_idr(
    ecs_term_iter_t *iter, 
    ecs_id_record_t *idr, 
    bool empty_tables)
{
    if (idr) {
        if (empty_tables) {
            flecs_table_cache_all_iter(&idr->cache, &iter->it);
        } else {
            flecs_table_cache_iter(&idr->cache, &iter->it);
        }
    } else {
        term_iter_init_no_data(iter);
    }

    iter->index = 0;
    iter->empty_tables = empty_tables;
}

static
void term_iter_init_wildcard(
    const ecs_world_t *world,
    ecs_term_iter_t *iter,
    bool empty_tables)
{
    iter->term = (ecs_term_t){ .field_index = -1 };
    iter->self_index = flecs_id_record_get(world, EcsAny);
    ecs_id_record_t *idr = iter->cur = iter->self_index;
    term_iter_init_w_idr(iter, idr, empty_tables);
}

static
void term_iter_init(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_iter_t *iter,
    bool empty_tables)
{    
    const ecs_term_id_t *src = &term->src;

    iter->term = *term;

    if (src->flags & EcsSelf) {
        iter->self_index = term->idr;
        if (!iter->self_index) {
            iter->self_index = flecs_query_id_record_get(world, term->id);
        }
    }

    if (src->flags & EcsUp) {
        iter->set_index = flecs_id_record_get(world, 
            ecs_pair(src->trav, EcsWildcard));
    }

    ecs_id_record_t *idr;
    if (iter->self_index) {
        idr = iter->cur = iter->self_index;
    } else {
        idr = iter->cur = iter->set_index;
    }

    term_iter_init_w_idr(iter, idr, empty_tables);
}

ecs_iter_t ecs_term_iter(
    const ecs_world_t *stage,
    ecs_term_t *term)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_process_pending_tables(world);

    if (ecs_term_finalize(world, term)) {
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .field_count = 1,
        .next = ecs_term_next
    };

    /* Term iter populates the iterator with arrays from its own cache, ensure 
     * they don't get overwritten by flecs_iter_validate.
     *
     * Note: the reason the term iterator doesn't use the iterator cache itself
     * (which could easily accomodate a single term) is that the filter iterator
     * is built on top of the term iterator. The private cache of the term
     * iterator keeps the filter iterator code simple, as it doesn't need to
     * worry about the term iter overwriting the iterator fields. */
    flecs_iter_init(stage, &it, 0);

    term_iter_init(world, term, &it.priv.iter.term, false);

    return it;
error:
    return (ecs_iter_t){ 0 };
}

ecs_iter_t ecs_term_chain_iter(
    const ecs_iter_t *chain_it,
    ecs_term_t *term)
{
    ecs_check(chain_it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_world_t *world = chain_it->real_world;
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ecs_term_finalize(world, term)) {
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = chain_it->world,
        .terms = term,
        .field_count = 1,
        .chain_it = (ecs_iter_t*)chain_it,
        .next = ecs_term_next
    };

    flecs_iter_init(chain_it->world, &it, flecs_iter_cache_all);

    term_iter_init(world, term, &it.priv.iter.term, false);

    return it;
error:
    return (ecs_iter_t){ 0 };
}

ecs_iter_t ecs_children(
    const ecs_world_t *world,
    ecs_entity_t parent)
{
    return ecs_term_iter(world,  &(ecs_term_t){ .id = ecs_childof(parent) });
}

bool ecs_children_next(
    ecs_iter_t *it)
{
    return ecs_term_next(it);
}

static
const ecs_table_record_t *flecs_term_iter_next_table(
    ecs_term_iter_t *iter)
{
    ecs_id_record_t *idr = iter->cur;
    if (!idr) {
        return NULL;
    }

    return flecs_table_cache_next(&iter->it, ecs_table_record_t);
}

static
bool flecs_term_iter_find_superset(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_term_t *term, 
    ecs_entity_t *source, 
    ecs_id_t *id, 
    int32_t *column)
{
    ecs_term_id_t *src = &term->src;

    /* Test if following the relationship finds the id */
    int32_t index = flecs_search_relation_w_idr(world, table, 0, 
        term->id, src->trav, src->flags, source, id, 0, term->idr);

    if (index == -1) {
        *source = 0;
        return false;
    }

    ecs_assert(*source != 0, ECS_INTERNAL_ERROR, NULL);

    *column = (index + 1) * -1;

    return true;
}

static
bool flecs_term_iter_next(
    ecs_world_t *world,
    ecs_term_iter_t *iter,
    bool match_prefab,
    bool match_disabled)
{
    ecs_table_t *table = iter->table;
    ecs_entity_t source = 0;
    const ecs_table_record_t *tr;
    ecs_term_t *term = &iter->term;

    do {
        if (table) {
            iter->cur_match ++;
            if (iter->cur_match >= iter->match_count) {
                table = NULL;
            } else {
                iter->last_column = ecs_search_offset(
                    world, table, iter->last_column + 1, term->id, 0);
                iter->column = iter->last_column + 1;
                if (iter->last_column >= 0) {
                    iter->id = table->type.array[iter->last_column];
                }
            }
        }

        if (!table) {
            if (!(tr = flecs_term_iter_next_table(iter))) {
                if (iter->cur != iter->set_index && iter->set_index != NULL) {
                    if (iter->observed_table_count != 0) {
                        iter->cur = iter->set_index;
                        if (iter->empty_tables) {
                            flecs_table_cache_all_iter(
                                &iter->set_index->cache, &iter->it);
                        } else {
                            flecs_table_cache_iter(
                                &iter->set_index->cache, &iter->it);
                        }
                        iter->index = 0;
                        tr = flecs_term_iter_next_table(iter);
                    }
                }

                if (!tr) {
                    return false;
                }
            }

            table = tr->hdr.table;
            if (table->observed_count) {
                iter->observed_table_count ++;
            }

            if (!match_prefab && (table->flags & EcsTableIsPrefab)) {
                continue;
            }

            if (!match_disabled && (table->flags & EcsTableIsDisabled)) {
                continue;
            }

            iter->table = table;
            iter->match_count = tr->count;
            if (is_any_pair(term->id)) {
                iter->match_count = 1;
            }

            iter->cur_match = 0;
            iter->last_column = tr->column;
            iter->column = tr->column + 1;
            iter->id = flecs_to_public_id(table->type.array[tr->column]);
        }

        if (iter->cur == iter->set_index) {
            if (iter->self_index) {
                if (flecs_id_record_get_table(iter->self_index, table) != NULL) {
                    /* If the table has the id itself and this term matched Self
                     * we already matched it */
                    continue;
                }
            }

            if (!flecs_term_iter_find_superset(
                world, table, term, &source, &iter->id, &iter->column)) 
            {
                continue;
            }

            /* The tr->count field refers to the number of relationship instances,
             * not to the number of matches. Superset terms can only yield a
             * single match. */
            iter->match_count = 1;
        }

        break;
    } while (true);

    iter->subject = source;

    return true;
}

static
bool flecs_term_iter_set_table(
    ecs_world_t *world,
    ecs_term_iter_t *iter,
    ecs_table_t *table)
{
    const ecs_table_record_t *tr = NULL;
    const ecs_id_record_t *idr = iter->self_index;
    if (idr) {
        tr = ecs_table_cache_get(&idr->cache, table);
        if (tr) {
            iter->match_count = tr->count;
            iter->last_column = tr->column;
            iter->column = tr->column + 1;
            iter->id = flecs_to_public_id(table->type.array[tr->column]);
        }
    }

    if (!tr) {
        idr = iter->set_index;
        if (idr) {
            tr = ecs_table_cache_get(&idr->cache, table);
            if (!flecs_term_iter_find_superset(world, table, &iter->term, 
                &iter->subject, &iter->id, &iter->column)) 
            {
                return false;
            }
            iter->match_count = 1;
        }
    }

    if (!tr) {
        return false;
    }

    /* Populate fields as usual */
    iter->table = table;
    iter->cur_match = 0;

    return true;
}

bool ecs_term_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_term_next, ECS_INVALID_PARAMETER, NULL);

    flecs_iter_validate(it);

    ecs_term_iter_t *iter = &it->priv.iter.term;
    ecs_term_t *term = &iter->term;
    ecs_world_t *world = it->real_world;
    ecs_table_t *table;

    it->ids = &iter->id;
    it->sources = &iter->subject;
    it->columns = &iter->column;
    it->terms = &iter->term;
    it->sizes = &iter->size;

    if (term->inout != EcsInOutNone) {
        it->ptrs = &iter->ptr;
    } else {
        it->ptrs = NULL;
    }

    ecs_iter_t *chain_it = it->chain_it;
    if (chain_it) {
        ecs_iter_next_action_t next = chain_it->next;
        bool match;

        do {
            if (!next(chain_it)) {
                goto done;
            }

            table = chain_it->table;
            match = flecs_term_match_table(world, term, table,
                it->ids, it->columns, it->sources, it->match_indices, true,
                it->flags);
        } while (!match);
        goto yield;

    } else {
        if (!flecs_term_iter_next(world, iter, false, false)) {
            goto done;
        }

        table = iter->table;

        /* Source must either be 0 (EcsThis) or nonzero in case of substitution */
        ecs_assert(iter->subject || iter->cur != iter->set_index, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(iter->table != NULL, ECS_INTERNAL_ERROR, NULL);
    }

yield:
    flecs_iter_populate_data(world, it, table, 0, ecs_table_count(table), 
        it->ptrs, it->sizes);
    ECS_BIT_SET(it->flags, EcsIterIsValid);
    return true;
done:
    ecs_iter_fini(it);
error:
    return false;
}

static
void flecs_init_filter_iter(
    ecs_iter_t *it,
    const ecs_filter_t *filter)
{
    ecs_assert(filter != NULL, ECS_INTERNAL_ERROR, NULL);
    it->priv.iter.filter.filter = filter;
    it->field_count = filter->field_count;
}

int32_t ecs_filter_pivot_term(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(filter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_term_t *terms = filter->terms;
    int32_t i, term_count = filter->term_count;
    int32_t pivot_term = -1, min_count = -1, self_pivot_term = -1;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_id_t id = term->id;

        if (term->oper != EcsAnd) {
            continue;
        }

        if (!ecs_term_match_this(term)) {
            continue;
        }

        ecs_id_record_t *idr = flecs_query_id_record_get(world, id);
        if (!idr) {
            /* If one of the terms does not match with any data, iterator 
             * should not return anything */
            return -2; /* -2 indicates filter doesn't match anything */
        }

        int32_t table_count = flecs_table_cache_count(&idr->cache);
        if (min_count == -1 || table_count < min_count) {
            min_count = table_count;
            pivot_term = i;
            if ((term->src.flags & EcsTraverseFlags) == EcsSelf) {
                self_pivot_term = i;
            }
        }
    }

    if (self_pivot_term != -1) {
        pivot_term = self_pivot_term;
    }

    return pivot_term;
error:
    return -2;
}

ecs_iter_t flecs_filter_iter_w_flags(
    const ecs_world_t *stage,
    const ecs_filter_t *filter,
    ecs_flags32_t flags)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(filter != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_world_t *world = ecs_get_world(stage);
    
    if (!(flags & EcsIterMatchVar)) {
        flecs_process_pending_tables(world);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .terms = filter ? filter->terms : NULL,
        .next = ecs_filter_next,
        .flags = flags
    };

    ecs_filter_iter_t *iter = &it.priv.iter.filter;
    iter->pivot_term = -1;

    flecs_init_filter_iter(&it, filter);
    ECS_BIT_COND(it.flags, EcsIterIsInstanced, 
        ECS_BIT_IS_SET(filter->flags, EcsFilterIsInstanced));

    /* Find term that represents smallest superset */
    if (ECS_BIT_IS_SET(flags, EcsIterIgnoreThis)) {
        term_iter_init_no_data(&iter->term_iter);
    } else if (ECS_BIT_IS_SET(filter->flags, EcsFilterMatchThis)) {
        ecs_term_t *terms = filter->terms;
        int32_t pivot_term = -1;
        ecs_check(terms != NULL, ECS_INVALID_PARAMETER, NULL);

        pivot_term = ecs_filter_pivot_term(world, filter);
        iter->kind = EcsIterEvalTables;
        iter->pivot_term = pivot_term;

        if (pivot_term == -2) {
            /* One or more terms have no matching results */
            term_iter_init_no_data(&iter->term_iter);
        } else if (pivot_term == -1) {
            /* No terms meet the criteria to be a pivot term, evaluate filter
             * against all tables */
            term_iter_init_wildcard(world, &iter->term_iter, 
                ECS_BIT_IS_SET(filter->flags, EcsFilterMatchEmptyTables));
        } else {
            ecs_assert(pivot_term >= 0, ECS_INTERNAL_ERROR, NULL);
            term_iter_init(world, &terms[pivot_term], &iter->term_iter,
                ECS_BIT_IS_SET(filter->flags, EcsFilterMatchEmptyTables));
        }
    } else {
        if (!ECS_BIT_IS_SET(filter->flags, EcsFilterMatchAnything)) {
            term_iter_init_no_data(&iter->term_iter);
        } else {
            iter->kind = EcsIterEvalNone;
        }
    }

    ECS_BIT_COND(it.flags, EcsIterIsFilter, 
        ECS_BIT_IS_SET(filter->flags, EcsFilterNoData));

    if (ECS_BIT_IS_SET(filter->flags, EcsFilterMatchThis)) {
        /* Make space for one variable if the filter has terms for This var */ 
        it.variable_count = 1;

        /* Set variable name array */
        it.variable_names = (char**)filter->variable_names;
    }

    flecs_iter_init(stage, &it, flecs_iter_cache_all);

    return it;
error:
    return (ecs_iter_t){ 0 };
}

ecs_iter_t ecs_filter_iter(
    const ecs_world_t *stage,
    const ecs_filter_t *filter)
{
    return flecs_filter_iter_w_flags(stage, filter, 0);
}

ecs_iter_t ecs_filter_chain_iter(
    const ecs_iter_t *chain_it,
    const ecs_filter_t *filter)
{
    ecs_iter_t it = {
        .terms = filter->terms,
        .field_count = filter->field_count,
        .world = chain_it->world,
        .real_world = chain_it->real_world,
        .chain_it = (ecs_iter_t*)chain_it,
        .next = ecs_filter_next
    };

    flecs_iter_init(chain_it->world, &it, flecs_iter_cache_all);
    ecs_filter_iter_t *iter = &it.priv.iter.filter;
    flecs_init_filter_iter(&it, filter);

    iter->kind = EcsIterEvalChain;

    return it;
}

bool ecs_filter_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_filter_next, ECS_INVALID_PARAMETER, NULL);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_filter_next_instanced(it));
error:
    return false;
}

bool ecs_filter_next_instanced(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_filter_next, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != it, ECS_INVALID_PARAMETER, NULL);

    ecs_filter_iter_t *iter = &it->priv.iter.filter;
    const ecs_filter_t *filter = iter->filter;
    ecs_world_t *world = it->real_world;
    ecs_table_t *table = NULL;
    bool match;

    flecs_iter_validate(it);

    ecs_iter_t *chain_it = it->chain_it;
    ecs_iter_kind_t kind = iter->kind;

    if (chain_it) {
        ecs_assert(kind == EcsIterEvalChain, ECS_INVALID_PARAMETER, NULL);
        
        ecs_iter_next_action_t next = chain_it->next;
        do {
            if (!next(chain_it)) {
                ecs_iter_fini(it);
                goto done;
            }

            table = chain_it->table;
            match = flecs_filter_match_table(world, filter, table,
                it->ids, it->columns, it->sources, it->match_indices, NULL, 
                true, -1, it->flags);
        } while (!match);

        goto yield;
    } else if (kind == EcsIterEvalTables || kind == EcsIterEvalCondition) {
        ecs_term_iter_t *term_iter = &iter->term_iter;
        ecs_term_t *term = &term_iter->term;
        int32_t pivot_term = iter->pivot_term;
        bool first;

        /* Check if the This variable has been set on the iterator. If set,
         * the filter should only be applied to the variable value */
        ecs_var_t *this_var = NULL;
        ecs_table_t *this_table = NULL;
        if (it->variable_count) {
            if (ecs_iter_var_is_constrained(it, 0)) {
                this_var = it->variables;
                this_table = this_var->range.table;

                /* If variable is constrained, make sure it's a value that's
                 * pointing to a table, as a filter can't iterate single
                 * entities (yet) */
                ecs_assert(this_table != NULL, ECS_INVALID_OPERATION, NULL);

                /* Can't set variable for filter that does not iterate tables */
                ecs_assert(kind == EcsIterEvalTables, 
                    ECS_INVALID_OPERATION, NULL);
            }
        }

        do {
            /* If there are no matches left for the previous table, this is the
             * first match of the next table. */
            first = iter->matches_left == 0;

            if (first) {
                if (kind != EcsIterEvalCondition) {
                    /* Check if this variable was constrained */
                    if (this_table != NULL) {                        
                        /* If this is the first match of a new result and the
                         * previous result was equal to the value of a 
                         * constrained var, there's nothing left to iterate */
                        if (it->table == this_table) {
                            goto done;
                        }

                        /* If table doesn't match term iterator, it doesn't
                         * match filter. */
                        if (!flecs_term_iter_set_table(
                            world, term_iter, this_table))
                        {
                            goto done;
                        }

                        /* But if it does, forward it to filter matching */
                        ecs_assert(term_iter->table == this_table,
                            ECS_INTERNAL_ERROR, NULL);

                    /* If This variable is not constrained, iterate as usual */
                    } else {
                        /* Find new match, starting with the leading term */
                        if (!flecs_term_iter_next(world, term_iter, 
                            ECS_BIT_IS_SET(filter->flags, 
                                EcsFilterMatchPrefab), 
                            ECS_BIT_IS_SET(filter->flags, 
                                EcsFilterMatchDisabled))) 
                        {
                            goto done;
                        }
                    }

                    ecs_assert(term_iter->match_count != 0, 
                        ECS_INTERNAL_ERROR, NULL);

                    if (pivot_term == -1) {
                        /* Without a pivot term, we're iterating all tables with
                         * a wildcard, so the match count is meaningless. */
                        term_iter->match_count = 1;
                    } else {
                        it->match_indices[pivot_term] = term_iter->match_count;
                    }

                    iter->matches_left = term_iter->match_count;

                    /* Filter iterator takes control over iterating all the
                     * permutations that match the wildcard. */
                    term_iter->match_count = 1;

                    table = term_iter->table;

                    if (pivot_term != -1) {
                        int32_t index = term->field_index;
                        it->ids[index] = term_iter->id;
                        it->sources[index] = term_iter->subject;
                        it->columns[index] = term_iter->column;
                    }
                } else {
                    /* Progress iterator to next match for table, if any */
                    table = it->table;
                    if (term_iter->index == 0) {
                        iter->matches_left = 1;
                        term_iter->index = 1; /* prevents looping again */
                    } else {
                        goto done;
                    }
                }

                /* Match the remainder of the terms */
                match = flecs_filter_match_table(world, filter, table,
                    it->ids, it->columns, it->sources,
                    it->match_indices, &iter->matches_left, first, 
                    pivot_term, it->flags);
                if (!match) {
                    it->table = table;
                    iter->matches_left = 0;
                    continue;
                }

                /* Table got matched, set This variable */
                if (table) {
                    ecs_assert(it->variable_count == 1, ECS_INTERNAL_ERROR, NULL);
                    ecs_assert(it->variables != NULL, ECS_INTERNAL_ERROR, NULL);
                    it->variables[0].range.table = table;
                }

                ecs_assert(iter->matches_left != 0, ECS_INTERNAL_ERROR, NULL);
            }

            /* If this is not the first result for the table, and the table
             * is matched more than once, iterate remaining matches */
            if (!first && (iter->matches_left > 0)) {
                table = it->table;

                /* Find first term that still has matches left */
                int32_t i, j, count = it->field_count;
                for (i = count - 1; i >= 0; i --) {
                    int32_t mi = -- it->match_indices[i];
                    if (mi) {
                        if (mi < 0) {
                            continue;
                        }
                        break;
                    }
                }

                /* If matches_left > 0 we should've found at least one match */
                ecs_assert(i >= 0, ECS_INTERNAL_ERROR, NULL);

                /* Progress first term to next match (must be at least one) */
                int32_t column = it->columns[i];
                if (column < 0) {
                    /* If this term was matched on a non-This entity, reconvert
                     * the column back to a positive value */
                    column = -column;
                }

                it->columns[i] = column + 1;
                flecs_term_match_table(world, &filter->terms[i], table, 
                    &it->ids[i], &it->columns[i], &it->sources[i],
                    &it->match_indices[i], false, it->flags);

                /* Reset remaining terms (if any) to first match */
                for (j = i + 1; j < count; j ++) {
                    flecs_term_match_table(world, &filter->terms[j], table, 
                        &it->ids[j], &it->columns[j], &it->sources[j], 
                        &it->match_indices[j], true, it->flags);
                }
            }

            match = iter->matches_left != 0;
            iter->matches_left --;

            ecs_assert(iter->matches_left >= 0, ECS_INTERNAL_ERROR, NULL);
        } while (!match);

        goto yield;
    }

done:
error:
    ecs_iter_fini(it);
    return false;

yield:
    it->offset = 0;
    flecs_iter_populate_data(world, it, table, 0, 
        table ? ecs_table_count(table) : 0, it->ptrs, it->sizes);
    ECS_BIT_SET(it->flags, EcsIterIsValid);
    return true;    
}
