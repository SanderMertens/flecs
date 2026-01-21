/**
 * @file query/validator.c
 * @brief Validate and finalize queries.
 */

#include "../private_api.h"

#ifdef FLECS_QUERY_DSL
#include "../addons/query_dsl/query_dsl.h"
#endif

static
void flecs_query_validator_error(
    const ecs_query_validator_ctx_t *ctx,
    const char *fmt,
    ...)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (ctx->desc && ctx->desc->expr) {
        ecs_strbuf_appendlit(&buf, "expr: ");
        ecs_strbuf_appendstr(&buf, ctx->desc->expr);
        ecs_strbuf_appendlit(&buf, "\n");
    }

    if (ctx->query) {
        ecs_query_t *query = ctx->query;
        const ecs_term_t *terms = query->terms;
        int32_t i, count = query->term_count;

        for (i = 0; i < count; i ++) {
            const ecs_term_t *term = &terms[i];
            if (ctx->term_index == i) {
                ecs_strbuf_appendlit(&buf, " > ");
            } else {
                ecs_strbuf_appendlit(&buf, "   ");
            }
            flecs_term_to_buf(ctx->world, term, &buf, i);
            if (term->oper == EcsOr) {
                ecs_strbuf_appendlit(&buf, " ||");
            } else if (i != (count - 1)) {
                ecs_strbuf_appendlit(&buf, ",");
            }
            ecs_strbuf_appendlit(&buf, "\n");
        }
    } else {
        ecs_strbuf_appendlit(&buf, " > ");
        flecs_term_to_buf(ctx->world, ctx->term, &buf, 0);
        ecs_strbuf_appendlit(&buf, "\n");
    }

    char *expr = ecs_strbuf_get(&buf);
    const char *name = NULL;
    if (ctx->query && ctx->query->entity) {
        name = ecs_get_name(ctx->query->world, ctx->query->entity);
    }

    va_list args;
    va_start(args, fmt);
    char *msg = flecs_vasprintf(fmt, args);
    ecs_parser_error(name, NULL, 0, "%s\n%s", msg, expr);
    ecs_os_free(msg);
    ecs_os_free(expr);

    va_end(args);
}

static
int flecs_term_ref_finalize_flags(
    ecs_term_ref_t *ref,
    ecs_query_validator_ctx_t *ctx,
    const char* refname)
{
    (void)refname;

    if ((ref->id & EcsIsEntity) && (ref->id & EcsIsVariable)) {
        flecs_query_validator_error(ctx, 
            "cannot set both EcsIsEntity and EcsIsVariable for term.%s", 
                refname);
        return -1;
    }

    if (ref->name && ref->name[0] == '$') {
        if (!ref->name[1]) {
            if (!(ref->id & EcsIsName)) {
                flecs_query_validator_error(ctx, 
                    "invalid variable name for term.%s"
                    " ('$' syntax support is removed, use new Singleton trait)", 
                        refname);
                return -1;
            }
        } else {
            ref->name = &ref->name[1];
            ref->id |= EcsIsVariable;
        }
    } 

    if (!(ref->id & (EcsIsEntity|EcsIsVariable|EcsIsName))) {
        if (ECS_TERM_REF_ID(ref) || ref->name) {
            if (ECS_TERM_REF_ID(ref) == EcsThis || 
                ECS_TERM_REF_ID(ref) == EcsWildcard || 
                ECS_TERM_REF_ID(ref) == EcsAny || 
                ECS_TERM_REF_ID(ref) == EcsVariable) 
            {
                /* Builtin variable ids default to variable */
                ref->id |= EcsIsVariable;
            } else {
                ref->id |= EcsIsEntity;
            }
        }
    }

    return 0;
}

static
bool flecs_identifier_is_0(
    const char *id)
{
    return id[0] == '#' && id[1] == '0' && !id[2];
}

static
int flecs_term_ref_lookup(
    const ecs_world_t *world,
    ecs_entity_t scope,
    ecs_term_ref_t *ref,
    ecs_query_validator_ctx_t *ctx)
{
    const char *name = ref->name;
    if (!name) {
        return 0;
    }

    if (ref->id & EcsIsVariable) {
        if (!ecs_os_strcmp(name, "this")) {
            ref->id = EcsThis | ECS_TERM_REF_FLAGS(ref);
            ref->name = NULL;
            return 0;
        }
        return 0;
    } else if (ref->id & EcsIsName) {
        if (ref->name == NULL) {
            flecs_query_validator_error(ctx, "IsName flag specified without name");
            return -1;
        }
        return 0;
    }

    ecs_assert(ref->id & EcsIsEntity, ECS_INTERNAL_ERROR, NULL);

    if (flecs_identifier_is_0(name)) {
        if (ECS_TERM_REF_ID(ref)) {
            flecs_query_validator_error(
                ctx, "name '0' does not match entity id");
            return -1;
        }
        ref->name = NULL;
        return 0;
    }

    ecs_entity_t e = 0;
    if (scope) {
        e = ecs_lookup_child(world, scope, name);
    }

    if (!e) {
        e = ecs_lookup(world, name);
    }

    if (!e) {
        if (ctx->query && (ctx->query->flags & EcsQueryAllowUnresolvedByName)) {
            ref->id |= EcsIsName;
            ref->id &= ~EcsIsEntity;
            return 0;
        } else {
            flecs_query_validator_error(ctx, "unresolved identifier '%s'", name);
            return -1;
        }
    }

    ecs_entity_t ref_id = ECS_TERM_REF_ID(ref);
    if (ref_id && ref_id != e) {
        char *e_str = ecs_get_path(world, ref_id);
        flecs_query_validator_error(ctx, "name '%s' does not match term.id '%s'", 
            name, e_str);
        ecs_os_free(e_str);
        return -1;
    }

    ref->id = e | ECS_TERM_REF_FLAGS(ref);
    ref_id = ECS_TERM_REF_ID(ref);

    if (!ecs_os_strcmp(name, "*") || !ecs_os_strcmp(name, "_") || 
        !ecs_os_strcmp(name, "$")) 
    {
        ref->id &= ~EcsIsEntity;
        ref->id |= EcsIsVariable;
    }

    /* Check if looked up id is alive (relevant for numerical ids) */
    if (!(ref->id & EcsIsName) && ref_id) {
        if (!ecs_is_alive(world, ref_id)) {
            flecs_query_validator_error(ctx, "identifier '%s' is not alive", 
                ref->name);
            return -1;
        }

        ref->name = NULL;
        return 0;
    }

    return 0;
}

static
int flecs_term_refs_finalize(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_query_validator_ctx_t *ctx)
{
    ecs_term_ref_t *src = &term->src;
    ecs_term_ref_t *first = &term->first;
    ecs_term_ref_t *second = &term->second;

    /* Include subsets for component by default, to support inheritance */
    if (!(first->id & EcsTraverseFlags)) {
        first->id |= EcsSelf;
    }

    /* Traverse Self by default for pair target */
    if (!(second->id & EcsTraverseFlags)) {
        if (ECS_TERM_REF_ID(second) || second->name || (second->id & EcsIsEntity)) {
            second->id |= EcsSelf;
        }
    }

    /* Source defaults to This */
    if (!ECS_TERM_REF_ID(src) && (src->name == NULL) && !(src->id & EcsIsEntity)) {
        src->id = EcsThis | ECS_TERM_REF_FLAGS(src);
        src->id |= EcsIsVariable;
    }

    /* Initialize term identifier flags */
    if (flecs_term_ref_finalize_flags(src, ctx, "src")) {
        return -1;
    }

    if (flecs_term_ref_finalize_flags(first, ctx, "first")) {
        return -1;
    }

    if (flecs_term_ref_finalize_flags(second, ctx, "second")) {
        return -1;
    }

    /* Lookup term identifiers by name */
    if (flecs_term_ref_lookup(world, 0, src, ctx)) {
        return -1;
    }
    if (flecs_term_ref_lookup(world, 0, first, ctx)) {
        return -1;
    }

    ecs_entity_t first_id = 0;
    ecs_entity_t oneof = 0;
    if (first->id & EcsIsEntity) {
        first_id = ECS_TERM_REF_ID(first);

        if (!first_id) {
            flecs_query_validator_error(ctx, "invalid \"0\" for first.name");
            return -1;
        }

        /* If first element of pair has OneOf property, lookup second element of
         * pair in the value of the OneOf property */
        oneof = flecs_get_oneof(world, first_id);
    }

    if (flecs_term_ref_lookup(world, oneof, &term->second, ctx)) {
        return -1;
    }

    /* If source is 0, reset traversal flags */
    if (ECS_TERM_REF_ID(src) == 0 && src->id & EcsIsEntity) {
        src->id &= ~EcsTraverseFlags;
        term->trav = 0;
    }

    /* If source is wildcard, term won't return any data */
    if ((src->id & EcsIsVariable) && ecs_id_is_wildcard(ECS_TERM_REF_ID(src))) {
        term->inout = EcsInOutNone;
    }

    return 0;
}

static
ecs_entity_t flecs_term_ref_get_entity(
    const ecs_term_ref_t *ref)
{
    if (ref->id & EcsIsEntity) {
        return ECS_TERM_REF_ID(ref); /* Id is known */
    } else if (ref->id & EcsIsVariable) {
        /* Return wildcard for variables, as they aren't known yet */
        if (ECS_TERM_REF_ID(ref) != EcsAny) {
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
    ecs_term_t *term)
{
    ecs_entity_t first = flecs_term_ref_get_entity(&term->first);
    ecs_entity_t second = flecs_term_ref_get_entity(&term->second);
    ecs_id_t flags = term->id & ECS_ID_FLAGS_MASK;

    if (first & ECS_ID_FLAGS_MASK) {
        return -1;
    }
    if (second & ECS_ID_FLAGS_MASK) {
        return -1;
    }

    if ((second || (term->second.id & EcsIsEntity))) {
        flags |= ECS_PAIR;
    }

    if (!second && !ECS_HAS_ID_FLAG(flags, PAIR)) {
        term->id = first | flags;
    } else {
        term->id = ecs_pair(first, second) | flags;
    }

    return 0;
}

static
int flecs_term_populate_from_id(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_query_validator_ctx_t *ctx)
{
    ecs_entity_t first = 0;
    ecs_entity_t second = 0;
    bool pair_w_0_tgt = false;

    if (ECS_HAS_ID_FLAG(term->id, PAIR)) {
        first = ECS_PAIR_FIRST(term->id);
        second = ECS_PAIR_SECOND(term->id);

        if (!first) {
            flecs_query_validator_error(ctx, "missing first element in term.id");
            return -1;
        }
        if (!second) {
            if (first != EcsChildOf) {
                flecs_query_validator_error(ctx, "missing second element in term.id");
                return -1;
            } else {
                /* Exception is made for ChildOf so we can use (ChildOf, 0) to match
                 * all entities in the root */
                pair_w_0_tgt = true;
            }
        }
    } else {
        first = term->id & ECS_COMPONENT_MASK;
        if (!first) {
            flecs_query_validator_error(ctx, "missing first element in term.id");
            return -1;
        }
    }

    ecs_entity_t term_first = flecs_term_ref_get_entity(&term->first);
    if (term_first) {
        if ((uint32_t)term_first != (uint32_t)first) {
            flecs_query_validator_error(ctx, "mismatch between term.id and term.first");
            return -1;
        }
    } else {
        ecs_entity_t first_id = ecs_get_alive(world, first);
        if (!first_id) {
            term->first.id = first | ECS_TERM_REF_FLAGS(&term->first);
        } else {
            term->first.id = first_id | ECS_TERM_REF_FLAGS(&term->first);
        }
    }

    ecs_entity_t term_second = flecs_term_ref_get_entity(&term->second);
    if (term_second) {
        if ((uint32_t)term_second != second) {
            flecs_query_validator_error(ctx, "mismatch between term.id and term.second");
            return -1;
        }
    } else if (second) {
        ecs_entity_t second_id = ecs_get_alive(world, second);
        if (!second_id) {
            term->second.id = second | ECS_TERM_REF_FLAGS(&term->second);
        } else {
            term->second.id = second_id | ECS_TERM_REF_FLAGS(&term->second);
        }
    } else if (pair_w_0_tgt) {
        term->second.id = EcsIsEntity;
    }

    return 0;
}

static
int flecs_term_verify_eq_pred(
    const ecs_term_t *term,
    ecs_query_validator_ctx_t *ctx)
{
    const ecs_term_ref_t *second = &term->second;
    const ecs_term_ref_t *src = &term->src;
    ecs_entity_t first_id = ECS_TERM_REF_ID(&term->first);
    ecs_entity_t second_id = ECS_TERM_REF_ID(&term->second);
    ecs_entity_t src_id = ECS_TERM_REF_ID(&term->src);

    if (term->oper != EcsAnd && term->oper != EcsNot && term->oper != EcsOr) {
        flecs_query_validator_error(ctx, "invalid operator combination");
        goto error;
    }

    if ((src->id & EcsIsName) && (second->id & EcsIsName)) {
        flecs_query_validator_error(ctx, "both sides of operator cannot be a name");
        goto error;
    }

    if ((src->id & EcsIsEntity) && (second->id & EcsIsEntity)) {
        flecs_query_validator_error(ctx, "both sides of operator cannot be an entity");
        goto error;
    }

    if (!(src->id & EcsIsVariable)) {
        flecs_query_validator_error(ctx, "left-hand of operator must be a variable");
        goto error;
    }

    if (first_id == EcsPredMatch && !(second->id & EcsIsName)) {
        flecs_query_validator_error(ctx, "right-hand of match operator must be a string");
        goto error;
    }

    if ((src->id & EcsIsVariable) && (second->id & EcsIsVariable)) {
        if (src_id && src_id == second_id) {
            flecs_query_validator_error(ctx, "both sides of operator are equal");
            goto error;
        }
        if (src->name && second->name && !ecs_os_strcmp(src->name, second->name)) {
            flecs_query_validator_error(ctx, "both sides of operator are equal");
            goto error;
        }
    }

    if (first_id == EcsPredEq) {
        if (second_id == EcsPredEq || second_id == EcsPredMatch) {
            flecs_query_validator_error(ctx, 
                "invalid right-hand side for equality operator");
            goto error;
        }
    }

    return 0;
error:
    return -1;
}

static
int flecs_term_verify(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_query_validator_ctx_t *ctx)
{
    const ecs_term_ref_t *first = &term->first;
    const ecs_term_ref_t *second = &term->second;
    const ecs_term_ref_t *src = &term->src;
    ecs_entity_t first_id = 0, second_id = 0;
    ecs_id_t flags = term->id & ECS_ID_FLAGS_MASK;
    ecs_id_t id = term->id;

    if ((src->id & EcsIsName) && (second->id & EcsIsName)) {
        flecs_query_validator_error(ctx, "mismatch between term.cr_flags & term.id");
        return -1;
    }

    ecs_entity_t src_id = ECS_TERM_REF_ID(src);
    if (first->id & EcsIsEntity) {
        first_id = ECS_TERM_REF_ID(first);
    }

    if (second->id & EcsIsEntity) {
        second_id = ECS_TERM_REF_ID(second);
    }

    if (first_id == EcsPredEq || first_id == EcsPredMatch || first_id == EcsPredLookup) {
        return flecs_term_verify_eq_pred(term, ctx);
    }

    if (ecs_term_ref_is_set(second) && !ECS_HAS_ID_FLAG(flags, PAIR)) {
        flecs_query_validator_error(ctx, "expected PAIR flag for term with pair");
        return -1;
    } else if (!ecs_term_ref_is_set(second) && ECS_HAS_ID_FLAG(flags, PAIR)) {
        if (first_id != EcsChildOf) {
            flecs_query_validator_error(ctx, "unexpected PAIR flag for term without pair");
            return -1;
        } else {
            /* Exception is made for ChildOf so we can use (ChildOf, 0) to match
             * all entities in the root */
        }
    }

    if (!ecs_term_ref_is_set(src)) {
        flecs_query_validator_error(ctx, "term.src is not initialized");
        return -1;
    }

    if (!ecs_term_ref_is_set(first)) {
        flecs_query_validator_error(ctx, "term.first is not initialized");
        return -1;
    }

    if (ECS_HAS_ID_FLAG(flags, PAIR)) {
        if (!ECS_PAIR_FIRST(id)) {
            flecs_query_validator_error(ctx, "invalid 0 for first element in pair id");
            return -1;
        }
        if ((ECS_PAIR_FIRST(id) != EcsChildOf) && !ECS_PAIR_SECOND(id)) {
            flecs_query_validator_error(ctx, "invalid 0 for second element in pair id");
            return -1;
        }

        if ((first->id & EcsIsEntity) && 
            (ecs_entity_t_lo(first_id) != ECS_PAIR_FIRST(id))) 
        {
            flecs_query_validator_error(ctx, "mismatch between term.id and term.first");
            return -1;
        }
        if ((first->id & EcsIsVariable) && 
            !ecs_id_is_wildcard(ECS_PAIR_FIRST(id)))
        {
            char *id_str = ecs_id_str(world, id);
            flecs_query_validator_error(ctx, 
                "expected wildcard for variable term.first (got %s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }

        if ((second->id & EcsIsEntity) && 
            (ecs_entity_t_lo(second_id) != ECS_PAIR_SECOND(id))) 
        {
            flecs_query_validator_error(ctx, "mismatch between term.id and term.second");
            return -1;
        }
        if ((second->id & EcsIsVariable) && 
            !ecs_id_is_wildcard(ECS_PAIR_SECOND(id))) 
        {
            char *id_str = ecs_id_str(world, id);
            flecs_query_validator_error(ctx, 
                "expected wildcard for variable term.second (got %s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }
    } else {
        ecs_entity_t component = id & ECS_COMPONENT_MASK;
        if (!component) {
            flecs_query_validator_error(ctx, "missing component id");
            return -1;
        }
        if ((first->id & EcsIsEntity) && 
            (ecs_entity_t_lo(first_id) != ecs_entity_t_lo(component))) 
        {
            flecs_query_validator_error(ctx, "mismatch between term.id and term.first");
            return -1;
        }
        if ((first->id & EcsIsVariable) && !ecs_id_is_wildcard(component)) {
            char *id_str = ecs_id_str(world, id);
            flecs_query_validator_error(ctx, 
                "expected wildcard for variable term.first (got %s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }
    }

    if (first_id) {
        if (ecs_term_ref_is_set(second)) {
            ecs_flags64_t mask = EcsIsEntity | EcsIsVariable;
            if ((src->id & mask) == (second->id & mask)) {
                bool is_same = false;
                if (src->id & EcsIsEntity) {
                    if (src_id) {
                        is_same = src_id == second_id;
                    }
                } else if (src->name && second->name) {
                    is_same = !ecs_os_strcmp(src->name, second->name);
                }

                if (is_same && ecs_has_id(world, first_id, EcsAcyclic)
                    && !(term->flags_ & EcsTermReflexive)) 
                {
                    flecs_query_validator_error(ctx, "term with acyclic "
                        "relationship cannot have the same source and target");
                    return -1;
                }
            }
        }

        if (second_id && !ecs_id_is_wildcard(second_id)) {
            ecs_entity_t oneof = flecs_get_oneof(world, first_id);
            if (oneof) {
                if (!ecs_has_pair(world, second_id, EcsChildOf, oneof)) {
                    char *second_str = ecs_get_path(world, second_id);
                    char *oneof_str = ecs_get_path(world, oneof);
                    char *id_str = ecs_id_str(world, term->id);
                    flecs_query_validator_error(ctx, 
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

    if (term->trav) {
        if (!ecs_has_id(world, term->trav, EcsTraversable)) {
            char *r_str = ecs_get_path(world, term->trav);
            flecs_query_validator_error(ctx, 
                "cannot traverse non-traversable relationship '%s'", r_str);
            ecs_os_free(r_str);
            return -1;
        }
    }

    return 0;
}

int flecs_term_finalize(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_query_validator_ctx_t *ctx)
{
    ctx->term = term;

    ecs_term_ref_t *src = &term->src;
    ecs_term_ref_t *first = &term->first;
    ecs_term_ref_t *second = &term->second;
    ecs_flags64_t first_flags = ECS_TERM_REF_FLAGS(first);
    ecs_flags64_t second_flags = ECS_TERM_REF_FLAGS(second);

    if (first->name && (first->id & ~EcsTermRefFlags)) {
        flecs_query_validator_error(ctx, 
            "first.name (%s) and first.id have competing values",
                first->name);
        return -1;
    }
    if (src->name && (src->id & ~EcsTermRefFlags)) {
        flecs_query_validator_error(ctx, 
            "src.name (%s) and src.id have competing values",
                src->name);
        return -1;
    }
    if (second->name && (second->id & ~EcsTermRefFlags)) {
        flecs_query_validator_error(ctx, 
            "second.name (%s) and second.id have competing values", 
                second->name);
        return -1;
    }

    if (term->id & ~ECS_ID_FLAGS_MASK) {
        if (flecs_term_populate_from_id(world, term, ctx)) {
            return -1;
        }
    }

    if (flecs_term_refs_finalize(world, term, ctx)) {
        return -1;
    }

    ecs_entity_t first_id = ECS_TERM_REF_ID(first);
    ecs_entity_t second_id = ECS_TERM_REF_ID(second);
    ecs_entity_t src_id = ECS_TERM_REF_ID(src);

    if ((first->id & EcsIsVariable) && (first_id == EcsAny)) {
        term->flags_ |= EcsTermMatchAny;
    }

    if ((second->id & EcsIsVariable) && (second_id == EcsAny)) {
        term->flags_ |= EcsTermMatchAny;
    }

    if ((src->id & EcsIsVariable) && (src_id == EcsAny)) {
        term->flags_ |= EcsTermMatchAnySrc;
    }

    ecs_flags64_t ent_var_mask = EcsIsEntity | EcsIsVariable;

    /* If EcsVariable is used by itself, assign to predicate (singleton) */
    if ((ECS_TERM_REF_ID(src) == EcsVariable) && (src->id & EcsIsVariable)) {
        src->id = first->id | ECS_TERM_REF_FLAGS(src);
        src->id &= ~ent_var_mask;
        src->id |= first->id & ent_var_mask;
        src->name = first->name;
    }

    if ((ECS_TERM_REF_ID(second) == EcsVariable) && (second->id & EcsIsVariable)) {
        second->id = first->id | ECS_TERM_REF_FLAGS(second);
        second->id &= ~ent_var_mask;
        second->id |= first->id & ent_var_mask;
        second->name = first->name;
    }

    if (!(term->id & ~ECS_ID_FLAGS_MASK)) {
        if (flecs_term_populate_id(term)) {
            return -1;
        }
    }

    /* If term queries for !(ChildOf, _), translate it to the builtin 
     * (ChildOf, 0) index which is a cheaper way to find root entities */
    if (term->oper == EcsNot && (
            (term->id == ecs_pair(EcsChildOf, EcsAny)) ||
            (term->id == ecs_pair(EcsChildOf, EcsWildcard))
        ))
    {
        /* Only if the source is not EcsAny */
        if (!(ECS_TERM_REF_ID(&term->src) == EcsAny && (term->src.id & EcsIsVariable))) {
            term->oper = EcsAnd;
            term->id = ecs_pair(EcsChildOf, 0);
            second->id = 0;
            second->id |= EcsSelf|EcsIsEntity;
        }
    }

    if (term->id == ecs_pair(EcsChildOf, 0)) {
        /* Ensure same behavior for (ChildOf, #0) and !(ChildOf, _) terms */
        term->flags_ |= EcsTermMatchAny;
    }

    ecs_entity_t first_entity = 0;
    if ((first->id & EcsIsEntity)) {
        first_entity = first_id;
    }

    ecs_flags32_t cr_flags = 0;
    if (term->id) {
        cr_flags = flecs_component_get_flags(world, term->id);
    }

    if (src_id || src->name) {
        if (!(term->src.id & EcsTraverseFlags)) {
            if (cr_flags & EcsIdOnInstantiateInherit) {
                term->src.id |= EcsSelf|EcsUp;
                if (!term->trav) {
                    term->trav = EcsIsA;
                }
            } else {
                term->src.id |= EcsSelf;

                if (term->trav) {
                    char *idstr = ecs_id_str(world, term->id);
                    flecs_query_validator_error(ctx, 
                        "traversal relationship (term::trav) '%s' specified for "
                        "component '%s' which can't be inherited", 
                            flecs_errstr(ecs_id_str(world, term->trav)),
                            idstr);
                    ecs_os_free(idstr);
                    return -1;
                }
            }
        }

        if (term->first.id & EcsCascade) {
            flecs_query_validator_error(ctx, 
                "cascade modifier invalid for term.first");
            return -1;
        }

        if (term->second.id & EcsCascade) {
            flecs_query_validator_error(ctx, 
                "cascade modifier invalid for term.second");
            return -1;
        }

        if (term->first.id & EcsDesc) {
            flecs_query_validator_error(ctx, 
                "desc modifier invalid for term.first");
            return -1;
        }

        if (term->second.id & EcsDesc) {
            flecs_query_validator_error(ctx, 
                "desc modifier invalid for term.second");
            return -1;
        }

        if (term->src.id & EcsDesc && !(term->src.id & EcsCascade)) {
            flecs_query_validator_error(ctx, 
                "desc modifier invalid without cascade");
            return -1;
        }

        if (term->src.id & EcsCascade) {
            /* Cascade always implies up traversal */
            term->src.id |= EcsUp;
        }

        if ((src->id & EcsUp) && !term->trav) {
            /* When traversal flags are specified but no traversal relationship,
            * default to ChildOf, which is the most common kind of traversal. */
            term->trav = EcsChildOf;
        }
    }

    if (!(cr_flags & EcsIdOnInstantiateInherit) && (term->trav == EcsIsA)) {
        if (src->id & EcsUp) {
            char *idstr = ecs_id_str(world, term->id);
            flecs_query_validator_error(ctx, "IsA traversal not allowed "
                "for '%s' because the component doesn't have the "
                "(OnInstantiate, Inherit) trait", idstr);
            ecs_os_free(idstr);
            return -1;
        }
    }

    if (first_entity && !ecs_term_match_0(term)) {
        bool first_is_self = (first_flags & EcsTraverseFlags) == EcsSelf;
        ecs_record_t *first_record = flecs_entities_get(world, first_entity);
        ecs_table_t *first_table = first_record ? first_record->table : NULL;
        
        bool first_can_isa = false;
        if (first_table) {
            first_can_isa = (first_table->flags & EcsTableHasIsA) != 0;
            if (first_can_isa) {
                first_can_isa = !ecs_table_has_id(world, first_table, EcsFinal);
            }
        }

        /* Only enable inheritance for ids which are inherited from at the time
         * of query creation. To force component inheritance to be evaluated,
         * an application can explicitly set traversal flags. */
        if (flecs_components_get(world, ecs_pair(EcsIsA, first->id)) || 
            (cr_flags & EcsIdInheritable) || first_can_isa)
        {
            if (!first_is_self) {
                term->flags_ |= EcsTermIdInherited;
            }
        } else {
#ifdef FLECS_DEBUG
            if (!first_is_self) {
                ecs_query_impl_t *q = flecs_query_impl(ctx->query);
                if (q) {
                    ECS_TERMSET_SET(q->final_terms, 1u << ctx->term_index);
                }
            }
#endif
        }

        if (first_table) {
            if (ecs_term_ref_is_set(second)) {
                /* Add traversal flags for transitive relationships */
                if (!((second_flags & EcsTraverseFlags) == EcsSelf)) {
                    if (!((src->id & EcsIsVariable) && (src_id == EcsAny))) {
                        if (!((second->id & EcsIsVariable) && (second_id == EcsAny))) {
                            if (!((second_id == EcsWildcard) && (term->oper == EcsNot))) {
                                if (ecs_table_has_id(world, first_table, EcsTransitive)) {
                                    term->flags_ |= EcsTermTransitive;
                                }

                                if (ecs_table_has_id(world, first_table, EcsReflexive)) {
                                    term->flags_ |= EcsTermReflexive;
                                }
                            }
                        }
                    }
                }
            }

            if (ecs_table_has_id(world, first_table, EcsDontFragment)) {
                term->flags_ |= EcsTermDontFragment;
            }
        }

        /* Check if term has toggleable component */
        if (cr_flags & EcsIdCanToggle) {
            /* If the term isn't matched on a #0 source */
            if (term->src.id != EcsIsEntity) {
                term->flags_ |= EcsTermIsToggle;

            }
        }

        /* Check if this is a member query */
#ifdef FLECS_META
        if (ecs_id(EcsMember) != 0) {
            if (first_entity) {
                if (ecs_has(world, first_entity, EcsMember)) {
                    term->flags_ |= EcsTermIsMember;
                }
            }
        }
#endif
    }

    if (ECS_TERM_REF_ID(first) == EcsVariable) {
        flecs_query_validator_error(ctx, "invalid $ for term.first");
        return -1;
    }

    if (term->oper == EcsAndFrom || term->oper == EcsOrFrom || term->oper == EcsNotFrom) {
        if (term->inout != EcsInOutDefault && term->inout != EcsInOutNone) {
            flecs_query_validator_error(ctx, 
                "invalid inout value for AndFrom/OrFrom/NotFrom term");
            return -1;
        }
    }

    /* Is term trivial/cacheable */
    bool cacheable_term = true;
    bool trivial_term = true;
    if (term->oper != EcsAnd || term->flags_ & EcsTermIsOr) {
        trivial_term = false;
    }

    if (ECS_IS_PAIR(term->id) && (ECS_PAIR_FIRST(term->id) == EcsChildOf)) {
        if (term->oper == EcsAnd) {
            term->flags_ |= EcsTermNonFragmentingChildOf;
        }

        if (ECS_PAIR_SECOND(term->id)) {
            trivial_term = false;
        }
    }

    if (ecs_id_is_wildcard(term->id)) {
        if (ECS_PAIR_FIRST(term->id) == EcsWildcard) {
            cacheable_term = false;
            trivial_term = false;
        }

        if (!(cr_flags & EcsIdExclusive)) {
            trivial_term = false;
        }

        if (first->id & EcsIsVariable) {
            if (!ecs_id_is_wildcard(first_id) || first_id == EcsAny) {
                trivial_term = false;
                cacheable_term = false;
            }
        }

        if (second->id & EcsIsVariable) {
            if (!ecs_id_is_wildcard(second_id) || second_id == EcsAny) {
                trivial_term = false;
                if (second_id != EcsAny) {
                    cacheable_term = false;
                }
            }
        }
    }

    if (!ecs_term_match_this(term)) {
        trivial_term = false;
    }

    if (term->flags_ & EcsTermTransitive) {
        trivial_term = false;
        cacheable_term = false; 
    }

    if (term->flags_ & EcsTermIdInherited) {
        trivial_term = false;
        cacheable_term = false;
    }

    if (term->flags_ & EcsTermReflexive) {
        trivial_term = false;
        cacheable_term = false;
    }

    if (term->trav && term->trav != EcsIsA) {
        trivial_term = false;
    }

    if (!(src->id & EcsSelf)) {
        trivial_term = false;
    }

    if (((ECS_TERM_REF_ID(&term->first) == EcsPredEq) || 
         (ECS_TERM_REF_ID(&term->first) == EcsPredMatch) || 
         (ECS_TERM_REF_ID(&term->first) == EcsPredLookup)) && 
        (term->first.id & EcsIsEntity)) 
    {
        trivial_term = false;
        cacheable_term = false;
    }

    if (ECS_TERM_REF_ID(src) != EcsThis) {
        cacheable_term = false;
    }

    if (term->id == ecs_childof(0)) {
        cacheable_term = false;
    }

    if (term->flags_ & EcsTermIsMember) {
        trivial_term = false;
        cacheable_term = false;
    }

    if (term->flags_ & EcsTermIsToggle) {
        trivial_term = false;
    }

    if (term->flags_ & EcsTermDontFragment) {
        trivial_term = false;
        cacheable_term = false;
    }

    ECS_BIT_COND16(term->flags_, EcsTermIsTrivial, trivial_term);
    ECS_BIT_COND16(term->flags_, EcsTermIsCacheable, cacheable_term);

    if (flecs_term_verify(world, term, ctx)) {
        return -1;
    }

    return 0;
}

bool ecs_term_ref_is_set(
    const ecs_term_ref_t *ref)
{
    return ECS_TERM_REF_ID(ref) != 0 || ref->name != NULL || (ref->id & EcsIsEntity) != 0;
}

bool ecs_term_is_initialized(
    const ecs_term_t *term)
{
    return term->id != 0 || ecs_term_ref_is_set(&term->first);
}

bool ecs_term_match_this(
    const ecs_term_t *term)
{
    return (term->src.id & EcsIsVariable) && 
        (ECS_TERM_REF_ID(&term->src) == EcsThis);
}

bool ecs_term_match_0(
    const ecs_term_t *term)
{
    return (!ECS_TERM_REF_ID(&term->src) && (term->src.id & EcsIsEntity));
}

static
ecs_term_t* flecs_query_or_other_type(
    ecs_query_t *q,
    int32_t t)
{
    ecs_term_t *term = &q->terms[t];
    ecs_term_t *first = NULL;
    while (t--) {
        if (q->terms[t].oper != EcsOr) {
            break;
        }
        first = &q->terms[t];
    }

    if (first) {
        ecs_world_t *world = q->world;
        const ecs_type_info_t *first_type = ecs_get_type_info(world, first->id);
        const ecs_type_info_t *term_type = ecs_get_type_info(world, term->id);

        if (first_type == term_type) {
            return NULL;
        }
        return first;
    } else {
        return NULL;
    }
}

static
void flecs_normalize_term_name(
    ecs_term_ref_t *ref) 
{
    if (ref->name && ref->name[0] == '$' && ref->name[1]) {
        ecs_assert(ref->id & EcsIsVariable, ECS_INTERNAL_ERROR, NULL);
        const char *old = ref->name;
        ref->name = &old[1];

        if (!ecs_os_strcmp(ref->name, "this")) {
            ref->name = NULL;
            ref->id |= EcsThis;
        }
    }
}

static
int flecs_query_finalize_terms(
    ecs_world_t *world,
    ecs_query_t *q,
    const ecs_query_desc_t *desc)
{
    int8_t i, term_count = q->term_count, field_count = 0;
    ecs_term_t *terms = q->terms;
    int32_t scope_nesting = 0, cacheable_terms = 0;
    bool cond_set = false;

    ecs_query_validator_ctx_t ctx = {0};
    ctx.world = world;
    ctx.query = q;
    ctx.desc = desc;

    q->flags |= EcsQueryMatchOnlyThis;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (term->oper == EcsOr) {
            term->flags_ |= EcsTermIsOr;
            if (i != (term_count - 1)) {
                term[1].flags_ |= EcsTermIsOr;
            }
        }
    }

    bool cacheable = true;
    bool match_nothing = true;

    /* If a query has ChildOf terms we need to prevent it from being marked as 
     * IsCacheable (meaning the query can be evaluated by just iterating the
     * cache). A ChildOf term must be marked as cacheable, but also needs an
     * instruction to filter tables that use non-fragmenting relationships. */
    bool has_childof = false;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        bool prev_is_or = i && term[-1].oper == EcsOr;
        bool nodata_term = false;
        bool default_src = term->src.id == 0 && term->src.name == NULL;
        ctx.term_index = i;

        if (flecs_term_finalize(world, term, &ctx)) {
            return -1;
        }

        if (term->flags_ & EcsTermNonFragmentingChildOf) {
            if (!i) {
                /* If the first term is a ChildOf pair, the query result should
                 * respect the order of parents with the OrderedChildren trait.
                 * This cannot be cached, so unset the IsCacheable bit. */
                ECS_BIT_CLEAR16(term->flags_, EcsTermIsCacheable);
            }

            if (ECS_PAIR_SECOND(term->id) != EcsAny) {
                has_childof = true;
            }
        }

        if (term->trav == EcsChildOf && (term->oper == EcsAnd || term->oper == EcsOptional)) {
            if (!(term->flags_ & EcsTermIsOr)) {
                has_childof = true;
            }
        }

        if (term->src.id != EcsIsEntity) {
            /* If term doesn't match 0 entity, query doesn't match nothing */
            match_nothing = false;
        }

        if (scope_nesting) {
            /* Terms inside a scope are not cacheable */
            ECS_BIT_CLEAR16(term->flags_, EcsTermIsCacheable);
        }

        /* If one of the terms in an OR chain isn't cacheable, none are */
        if (term->flags_ & EcsTermIsCacheable) {
            /* Current term is marked as cacheable. Check if it is part of an OR
             * chain, and if so, the previous term was also cacheable. */
            if (prev_is_or) {
                if (term[-1].flags_ & EcsTermIsCacheable) {
                    cacheable_terms ++;
                } else {
                    ECS_BIT_CLEAR16(term->flags_, EcsTermIsCacheable);
                }
            } else {
                cacheable_terms ++;
            }

            /* Toggle terms may be cacheable for fetching the initial component, 
             * but require an additional toggle instruction for evaluation. */
            if (term->flags_ & EcsTermIsToggle) {
                cacheable = false;
            }
        } else if (prev_is_or) {
            /* Current term is not cacheable. If it is part of an OR chain, mark
             * previous terms in the chain as also not cacheable. */
            int32_t j;
            for (j = i - 1; j >= 0; j --) {
                if (terms[j].oper != EcsOr) {
                    break;
                }
                if (terms[j].flags_ & EcsTermIsCacheable) {
                    cacheable_terms --;
                    ECS_BIT_CLEAR16(terms[j].flags_, EcsTermIsCacheable);
                }
            }
        }

        if (!prev_is_or) {
            field_count ++;
        }

        term->field_index = flecs_ito(int8_t, field_count - 1);

        if (ecs_id_is_wildcard(term->id)) {
            if (term->oper != EcsNot || ecs_id_is_any(term->id)) {
                q->flags |= EcsQueryMatchWildcards;
            }
        } else if (!(term->flags_ & EcsTermIsOr)) {
            ECS_TERMSET_SET(q->static_id_fields, 1u << term->field_index);
        }

        if (ECS_TERM_REF_ID(term) == EcsPrefab) {
            ECS_BIT_SET(q->flags, EcsQueryMatchPrefab);
        }
        if (ECS_TERM_REF_ID(term) == EcsDisabled && (term->src.id & EcsSelf)) {
            ECS_BIT_SET(q->flags, EcsQueryMatchDisabled);
        }
        
        if (term->oper == EcsNot && term->inout == EcsInOutDefault) {
            term->inout = EcsInOutNone;
        }

        if ((term->id == EcsWildcard) || (term->id == 
            ecs_pair(EcsWildcard, EcsWildcard))) 
        {
            /* If term type is unknown beforehand, default the inout type to
             * none. This prevents accidentally requesting lots of components,
             * which can put stress on serializer code. */
            if (term->inout == EcsInOutDefault) {
                term->inout = EcsInOutNone;
            }
        }

        if (term->src.id == EcsIsEntity) {
            nodata_term = true;
        } else if (term->inout == EcsInOutNone) {
            nodata_term = true;
        } else if (!ecs_get_type_info(world, term->id)) {
            nodata_term = true;
        } else if (term->flags_ & EcsTermIsMember) {
            nodata_term = true;
        } else if (scope_nesting) {
            nodata_term = true;
        } else {
            if (ecs_id_is_tag(world, term->id)) {
                nodata_term = true;
            } else if ((ECS_PAIR_SECOND(term->id) == EcsWildcard) ||
                       (ECS_PAIR_SECOND(term->id) == EcsAny)) 
            {
                /* If the second element of a pair is a wildcard and the first
                 * element is not a type, we can't know in advance what the
                 * type of the term is, so it can't provide data. */
                if (!ecs_get_type_info(world, ecs_pair_first(world, term->id))) {
                    nodata_term = true;
                }
            }
        }

        if (!nodata_term && term->inout != EcsIn && term->inout != EcsInOutNone) {
            /* Non-this terms default to EcsIn */
            if (ecs_term_match_this(term) || term->inout != EcsInOutDefault) {
                q->flags |= EcsQueryHasOutTerms;
            }

            bool match_non_this = !ecs_term_match_this(term) || 
                (term->src.id & EcsUp);
            if (match_non_this && term->inout != EcsInOutDefault) {
                q->flags |= EcsQueryHasNonThisOutTerms;
            }
        }

        if (!nodata_term) {
            /* If terms in an OR chain do not all return the same type, the 
             * field will not provide any data */
            if (term->flags_ & EcsTermIsOr) {
                ecs_term_t *first = flecs_query_or_other_type(q, i);
                if (first) {
                    nodata_term = true;
                }
                q->data_fields &= (ecs_termset_t)~(1llu << term->field_index);
            }
        }

        if (term->flags_ & EcsTermIsMember) {
            nodata_term = false;
        }

        if (!nodata_term && term->oper != EcsNot) {
            ECS_TERMSET_SET(q->data_fields, 1u << term->field_index);

            if (term->inout != EcsIn) {
                ECS_TERMSET_SET(q->write_fields, 1u << term->field_index);
            }
            if (term->inout != EcsOut) {
                ECS_TERMSET_SET(q->read_fields, 1u << term->field_index);
            }
            if (term->inout == EcsInOutDefault) {
                ECS_TERMSET_SET(q->shared_readonly_fields, 
                    1u << term->field_index);
            }
        }

        bool is_sparse = false;

        ecs_component_record_t *cr = flecs_components_get(world, term->id);
        ecs_flags32_t cr_flags = 0;

        if (cr) {
            cr_flags = cr->flags;
        } else if (term->id) {
            cr_flags = flecs_component_get_flags(world, term->id);
        }

        if (cr_flags & EcsIdSparse) {
            is_sparse = true;
        }

        if (cr_flags & EcsIdSingleton) {
            if (default_src) {
                term->src.id = term->first.id|EcsSelf|EcsIsEntity;

                ECS_BIT_CLEAR16(term->flags_, EcsTermIsTrivial);
                if (term->flags_ & EcsTermIsCacheable) {
                    cacheable_terms --;
                    ECS_BIT_CLEAR16(term->flags_, EcsTermIsCacheable);
                }
            }
        }

        /* If this is a static field, we need to assume that we might have 
         * to do change detection. */
        if (term->src.id & EcsIsEntity) {
            if (term->id < FLECS_HI_COMPONENT_ID) {
                world->non_trivial_set[term->id] = true;
            }
        }

        if (ecs_term_match_this(term)) {
            ECS_BIT_SET(q->flags, EcsQueryMatchThis);
        } else {
            ECS_BIT_CLEAR(q->flags, EcsQueryMatchOnlyThis);
        }

        if (ECS_TERM_REF_ID(&term->src) && (term->src.id & EcsIsEntity)) {
            ECS_TERMSET_SET(q->fixed_fields, 1u << term->field_index);
        }

        if ((term->src.id & EcsIsVariable) && 
            (ECS_TERM_REF_ID(&term->src) != EcsThis)) 
        {
            ECS_TERMSET_SET(q->var_fields, 1u << term->field_index);
        }

        if (prev_is_or) {
            if (ECS_TERM_REF_ID(&term[-1].src) != ECS_TERM_REF_ID(&term->src)) {
                flecs_query_validator_error(&ctx, 
                    "mismatching sources in OR expression (all terms in OR "
                    "expression must have the same source)");
                return -1;
            }
            if (term->oper != EcsOr && term->oper != EcsAnd) {
                flecs_query_validator_error(&ctx, 
                    "term after OR expression cannot use operators");
                return -1;
            }
        }

        if (is_sparse) {
            term->flags_ |= EcsTermIsSparse;
            ECS_BIT_CLEAR16(term->flags_, EcsTermIsTrivial);
            if (term->flags_ & EcsTermIsCacheable) {
                cacheable_terms --;
                ECS_BIT_CLEAR16(term->flags_, EcsTermIsCacheable);
            }

            /* Sparse component fields must be accessed with ecs_field_at */
            if (!nodata_term) {
                q->row_fields |= flecs_uto(uint32_t, 1llu << i);
            }
        }

        if (term->oper == EcsOptional || term->oper == EcsNot) {
            cond_set = true;
        }

        ecs_entity_t first_id = ECS_TERM_REF_ID(&term->first);
        if (first_id == EcsPredEq || first_id == EcsPredMatch ||
            first_id == EcsPredLookup)
        {
            q->flags |= EcsQueryHasPred;
            term->src.id = (term->src.id & ~EcsTraverseFlags) | EcsSelf;
            term->inout = EcsInOutNone;
        } else {
            if (!ecs_term_match_0(term) && term->oper != EcsNot && 
                term->oper != EcsNotFrom) 
            {
                ECS_TERMSET_SET(q->set_fields, 1u << term->field_index);
            }
        }

        if (first_id == EcsScopeOpen) {
            q->flags |= EcsQueryHasScopes;
            scope_nesting ++;
        }

        if (scope_nesting) {
            term->flags_ |= EcsTermIsScope;
            ECS_BIT_CLEAR16(term->flags_, EcsTermIsTrivial);
            ECS_BIT_CLEAR16(term->flags_, EcsTermIsCacheable);
            cacheable_terms --;
        }

        if (first_id == EcsScopeClose) {
            if (i && ECS_TERM_REF_ID(&terms[i - 1].first) == EcsScopeOpen) {
                flecs_query_validator_error(&ctx, "invalid empty scope");
                return -1;
            }

            q->flags |= EcsQueryHasScopes;
            scope_nesting --;
        }

        if (scope_nesting < 0) {
            flecs_query_validator_error(&ctx, "'}' without matching '{'");
            return -1;
        }
    }

    if (scope_nesting != 0) {
        flecs_query_validator_error(&ctx, "missing '}'");
        return -1;
    }

    if (term_count && (terms[term_count - 1].oper == EcsOr)) {
        flecs_query_validator_error(&ctx, 
            "last term of query can't have OR operator");
        return -1;
    }

    q->field_count = flecs_ito(int8_t, field_count);

    if (field_count) {
        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &terms[i];
            int32_t field = term->field_index;
            q->ids[field] = term->id;

            if (!ecs_term_match_0(term)) {
                flecs_component_lock(world, term->id);
            }

            if (term->flags_ & EcsTermIsOr) {
                if (flecs_query_or_other_type(q, i)) {
                    q->sizes[field] = 0;
                    q->ids[field] = 0;
                    continue;
                }
            }

            ecs_component_record_t *cr = flecs_components_get(world, term->id);
            if (cr) {
                if (!ECS_IS_PAIR(cr->id) || ECS_PAIR_FIRST(cr->id) != EcsWildcard) {
                    if (cr->type_info) {
                        q->sizes[field] = cr->type_info->size;
                        q->ids[field] = cr->id;
                    }
                }
            } else {
                const ecs_type_info_t *ti = ecs_get_type_info(
                    world, term->id);
                if (ti) {
                    q->sizes[field] = ti->size;
                    q->ids[field] = term->id;
                }
            }
        }
    }

    ECS_BIT_COND(q->flags, EcsQueryHasCondSet, cond_set);

    /* Check if this is a trivial query */
    if ((q->flags & EcsQueryMatchOnlyThis)) {
        if (!(q->flags & 
            (EcsQueryHasPred|EcsQueryMatchDisabled|EcsQueryMatchPrefab))) 
        {
            ECS_BIT_SET(q->flags, EcsQueryMatchOnlySelf);

            bool is_trivial = true;

            for (i = 0; i < term_count; i ++) {
                ecs_term_t *term = &terms[i];
                ecs_term_ref_t *src = &term->src;

                if (src->id & EcsUp) {
                    ECS_BIT_CLEAR(q->flags, EcsQueryMatchOnlySelf);
                }

                if (ECS_TERM_REF_ID(&term->first) == EcsChildOf) {
                    if (ECS_TERM_REF_ID(&term->second) != 0) {
                        is_trivial = false;
                        continue;
                    }
                }

                if (!(term->flags_ & EcsTermIsTrivial)) {
                    is_trivial = false;
                    continue;
                }

                if ((term->src.id & EcsTraverseFlags) == EcsSelf) {
                    if (!ecs_id_is_wildcard(term->id)) {
                        
                        q->bloom_filter = flecs_table_bloom_filter_add(
                            q->bloom_filter, term->id);
                    }
                }
            }

            if (term_count && is_trivial) {
                ECS_BIT_SET(q->flags, EcsQueryIsTrivial);
            }
        }
    }

    /* Set cacheable flags */
    ECS_BIT_COND(q->flags, EcsQueryHasCacheable, 
        cacheable_terms != 0);

    /* Exclude queries with order_by from setting the IsCacheable flag. This 
     * allows the routine that evaluates entirely cached queries to use more
     * optimized logic as it doesn't have to deal with order_by edge cases */
    ECS_BIT_COND(q->flags, EcsQueryIsCacheable, 
        cacheable && (cacheable_terms == term_count) &&
            !desc->order_by_callback &&
            !has_childof);

    ECS_BIT_COND(q->flags, EcsQueryCacheWithFilter, has_childof);

    /* If none of the terms match a source, the query matches nothing */
    ECS_BIT_COND(q->flags, EcsQueryMatchNothing, match_nothing);

    for (i = 0; i < q->term_count; i ++) {
        ecs_term_t *term = &q->terms[i];
        /* Post process term names in case they were used to create variables */
        flecs_normalize_term_name(&term->first);
        flecs_normalize_term_name(&term->second);
        flecs_normalize_term_name(&term->src);
    }

    return 0;
}

static
int flecs_query_query_populate_terms(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_t *q,
    const ecs_query_desc_t *desc)
{
    /* Count number of initialized terms in desc->terms */
    int32_t i, term_count = 0;
    for (i = 0; i < FLECS_TERM_COUNT_MAX; i ++) {
        if (!ecs_term_is_initialized(&desc->terms[i])) {
            break;
        }
        term_count ++;
    }

    /* Copy terms from array to query */
    if (term_count) {
        ecs_os_memcpy_n(q->terms, desc->terms, ecs_term_t, term_count);
    }

    /* Parse query expression if set */
    const char *expr = desc->expr;
    if (expr && expr[0]) {
    #ifdef FLECS_QUERY_DSL
        const char *name = desc->entity ? 
            ecs_get_name(world, desc->entity) : NULL;
        
        /* Allocate buffer that's large enough to tokenize the query string */
        ecs_size_t token_buffer_size = ecs_os_strlen(expr) * 2 + 1;
        char *token_buffer = flecs_alloc(
            &flecs_query_impl(q)->stage->allocator, token_buffer_size);

        if (flecs_terms_parse(world, name, expr, token_buffer, &q->terms[term_count], 
            &term_count))
        {
            flecs_free(&stage->allocator, token_buffer_size, token_buffer);
            goto error;
        }

        /* Store on query object so we can free later */
        flecs_query_impl(q)->tokens = token_buffer;
        flecs_query_impl(q)->tokens_len = flecs_ito(int16_t, token_buffer_size);
    #else
        (void)world;
        (void)stage;
        ecs_err("parsing query expressions requires the FLECS_QUERY_DSL addon");
        goto error;
    #endif
    }

    q->term_count = flecs_ito(int8_t, term_count);

    return 0;
error:  
    return -1;
}

bool flecs_query_finalize_simple(
    ecs_world_t *world,
    ecs_query_t *q,
    const ecs_query_desc_t *desc)
{
    ecs_assert(q->terms != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(q->sizes != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(q->ids != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Filter out queries that aren't simple enough */
    if (desc->expr) {
        return false;
    }

    if (desc->order_by_callback || desc->group_by_callback) {
        return false;
    }

    int8_t i, term_count;
    for (i = 0; i < FLECS_TERM_COUNT_MAX; i ++) {
        const ecs_term_t *term = &desc->terms[i];
        if (!ecs_term_is_initialized(&desc->terms[i])) {
            break;
        }

        ecs_id_t id = term->id;
        if (ecs_id_is_wildcard(id)) {
            return false;
        }

        if (id == EcsThis || ECS_PAIR_FIRST(id) == EcsThis || 
            ECS_PAIR_SECOND(id) == EcsThis) 
        {
            return false;
        }

        if (id == EcsVariable || ECS_PAIR_FIRST(id) == EcsVariable || 
            ECS_PAIR_SECOND(id) == EcsVariable) 
        {
            return false;
        }

        if (id == EcsPrefab || id == EcsDisabled) {
            return false;
        }

        ecs_term_t cmp_term = { 
            .id = id, 
            .flags_ = term->flags_, 
            .field_index = term->field_index
        };

        if (term->src.id == (EcsThis|EcsSelf|EcsIsVariable)) {
            cmp_term.src.id = EcsThis|EcsSelf|EcsIsVariable;
        } else if (term->src.id == EcsSelf) {
            cmp_term.src.id = EcsSelf;
        }

        if (term->first.id == (term->id|EcsSelf|EcsIsEntity)) {
            cmp_term.first.id = term->id|EcsSelf|EcsIsEntity;
        }

        if (ecs_os_memcmp_t(&cmp_term, term, ecs_term_t)) {
            return false;
        }
    }

    if (!i) {
        return false; /* No terms */
    }

    term_count = i;
    ecs_os_memcpy_n(q->terms, desc->terms, ecs_term_t, term_count);

    /* All fields are InOut */
    q->write_fields = (1u << term_count) - 1;

    /* Simple query that only queries for component ids */

    /* Populate terms */
    bool has_this = false, has_only_this = true;
    int8_t cacheable_count = 0, trivial_count = 0, up_count = 0;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &q->terms[i];
        ecs_id_t id = term->id;

        ecs_entity_t first = id;
        if (ECS_IS_PAIR(id)) {
            ecs_entity_t second = flecs_entities_get_alive(world, 
                ECS_PAIR_SECOND(id));
            first = flecs_entities_get_alive(world, ECS_PAIR_FIRST(id));
            term->second.id = second | EcsIsEntity | EcsSelf;
        }

        bool is_self = term->src.id == EcsSelf;
        bool default_src = term->src.id == 0;

        term->field_index = i;
        term->first.id = first | EcsIsEntity | EcsSelf;
        term->src.id = EcsThis | EcsIsVariable | EcsSelf;

        q->ids[i] = id;

        ecs_component_record_t *cr = NULL;
#ifndef FLECS_SANITIZE
        cr = flecs_components_get(world, id);
#else
        /* In sanitized mode, always compute the component flags on the spot
         * instead of using the flags cached on the component record. This 
         * ensures that both code paths get the same kind of test coverage. */
#endif

        const ecs_type_info_t *type_info = NULL;
        ecs_flags32_t cr_flags = 0;
        if (cr) {
            cr_flags = cr->flags;
            type_info = cr->type_info;
        } else {
            cr_flags = flecs_component_get_flags(world, id);
            type_info = flecs_determine_type_info_for_component(world, id);
        }

        bool cacheable = true, trivial = true;

        if (type_info) {
            q->sizes[i] = type_info->size;
            q->flags |= EcsQueryHasOutTerms;
            q->data_fields |= (ecs_termset_t)(1llu << i);
        }

        if (!is_self && cr_flags & EcsIdOnInstantiateInherit) {
            term->src.id |= EcsUp;
            term->trav = EcsIsA;
            up_count ++;
        }

        if (cr_flags & EcsIdCanToggle) {
            term->flags_ |= EcsTermIsToggle;
            trivial = false;
        }

        if (cr_flags & EcsIdDontFragment) {
            term->flags_ |= EcsTermDontFragment;
            trivial = false;
        }

        if (cr_flags & EcsIdSparse) {
            term->flags_ |= EcsTermIsSparse;
            cacheable = false; trivial = false;
            q->row_fields |= flecs_uto(uint32_t, 1llu << i);
        }

        if (cr_flags & EcsIdSingleton) {
            if (default_src) {
                term->src.id = term->first.id|EcsSelf|EcsIsEntity;
                has_only_this = false;
                cacheable = false; trivial = false;
            }
        } else {
            has_this = true;
        }

        if (ECS_IS_PAIR(id)) {
            if (first == EcsChildOf) {
                if (term->oper == EcsAnd) {
                    term->flags_ |= EcsTermNonFragmentingChildOf;
                }

                ecs_entity_t second = ECS_PAIR_SECOND(id);
                if (second) {
                    trivial = false;
                    if (ECS_PAIR_SECOND(id) != EcsAny) {
                        if (!i) {
                            /* If first query term is ChildOf, return children
                             * in order if possible, which can't be cached. */
                            cacheable = false;
                        }
                    }
                }
            }

            if (ecs_has_id(world, first, EcsTransitive)) {
                term->flags_ |= EcsTermTransitive;
                trivial = false;
                cacheable = false; 
            }

            if (ecs_has_id(world, first, EcsReflexive)) {
                term->flags_ |= EcsTermReflexive;
                trivial = false;
                cacheable = false; 
            }
        }

        if (flecs_components_get(world, ecs_pair(EcsIsA, first)) != NULL) {
            term->flags_ |= EcsTermIdInherited;
            cacheable = false; trivial = false;
        }

        if (cacheable) {
            term->flags_ |= EcsTermIsCacheable;
            cacheable_count ++;
        }

        if (trivial) {
            term->flags_ |= EcsTermIsTrivial;
            trivial_count ++;            

            if ((term->src.id & EcsTraverseFlags) == EcsSelf) {
                q->bloom_filter = flecs_table_bloom_filter_add(
                    q->bloom_filter, id);
            }
        }

        flecs_component_lock(world, id);
    }

    /* Initialize static data */
    q->term_count = term_count;
    q->field_count = term_count;
    q->set_fields = (ecs_termset_t)((1llu << i) - 1);
    q->static_id_fields = (ecs_termset_t)((1llu << i) - 1);

    if (has_this) {
        q->flags |= EcsQueryHasTableThisVar|EcsQueryMatchThis;
    }

    if (has_only_this) {
        q->flags |= EcsQueryMatchOnlyThis;
    }

    if (cacheable_count) {
        q->flags |= EcsQueryHasCacheable;
    }

    if (cacheable_count == term_count && trivial_count == term_count) {
        q->flags |= EcsQueryIsCacheable|EcsQueryIsTrivial;
    }

    if (!up_count) {
        q->flags |= EcsQueryMatchOnlySelf;
    }

    return true;
}

static
char* flecs_query_append_token(
    char *dst,
    const char *src)
{
    int32_t len = ecs_os_strlen(src);
    ecs_os_memcpy(dst, src, len + 1);
    return dst + len + 1;
}

static
void flecs_query_populate_tokens(
    ecs_query_impl_t *impl)
{
    ecs_query_t *q = &impl->pub;
    int32_t i, term_count = q->term_count;

    char *old_tokens = impl->tokens;
    int32_t old_tokens_len = impl->tokens_len;
    impl->tokens = NULL;
    impl->tokens_len = 0;

    /* Step 1: determine size of token buffer */
    int32_t len = 0;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &q->terms[i];
        
        if (term->first.name) {
            len += ecs_os_strlen(term->first.name) + 1;
        }
        if (term->second.name) {
            len += ecs_os_strlen(term->second.name) + 1;
        }
        if (term->src.name) {
            len += ecs_os_strlen(term->src.name) + 1;
        }
    }

    /* Step 2: reassign term tokens to buffer */
    if (len) {
        impl->tokens = flecs_alloc(&impl->stage->allocator, len);
        impl->tokens_len = flecs_ito(int16_t, len);
        char *token = impl->tokens, *next;

        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &q->terms[i];
            if (term->first.name) {
                next = flecs_query_append_token(token, term->first.name);
                term->first.name = token;
                token = next;
            }
            if (term->second.name) {
                next = flecs_query_append_token(token, term->second.name);
                term->second.name = token;
                token = next;
            }
            if (term->src.name) {
                next = flecs_query_append_token(token, term->src.name);
                term->src.name = token;
                token = next;
            }
        }
    }

    if (old_tokens) {
        flecs_free(&impl->stage->allocator, old_tokens_len, old_tokens);
    }
}

int flecs_query_finalize_query(
    ecs_world_t *world,
    ecs_query_t *q,
    const ecs_query_desc_t *desc)    
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_query_desc_t was not initialized to zero");
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    q->flags |= desc->flags | world->default_query_flags;

    ecs_term_t terms[FLECS_TERM_COUNT_MAX] = {0};
    ecs_size_t sizes[FLECS_TERM_COUNT_MAX] = {0};
    ecs_id_t ids[FLECS_TERM_COUNT_MAX] = {0};

    q->terms = terms;
    q->sizes = sizes;
    q->ids = ids;

    /* Fast routine that initializes simple queries and skips complex validation 
     * logic if it's not needed. When running in sanitized mode, always take the 
     * slow path. This in combination with the test suite ensures that the
     * result of the fast & slow code is the same. */
    #ifndef FLECS_SANITIZE
    if (flecs_query_finalize_simple(world, q, desc)) {
        goto done;
    }
    #endif

    /* Populate term array from desc terms & DSL expression */
    if (flecs_query_query_populate_terms(world, stage, q, desc)) {
        goto error;
    }

    /* Ensure all fields are consistent and properly filled out */
    if (flecs_query_finalize_terms(world, q, desc)) {
        goto error;
    }

    /* Store remaining string tokens in terms (after entity lookups) in single
     * token buffer which simplifies memory management & reduces allocations. */
    flecs_query_populate_tokens(flecs_query_impl(q));

    #ifndef FLECS_SANITIZE
done:
    #endif

    flecs_query_copy_arrays(q);

    q->flags |= EcsQueryValid;

    return 0;
error:
    flecs_query_copy_arrays(q);
    return -1;
}
