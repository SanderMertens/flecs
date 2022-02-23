
#include "private_api.h"
#include <ctype.h>

static
void term_error(
    const ecs_world_t *world,
    const ecs_term_t *term,
    const char *name,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);

    char *expr = ecs_term_str(world, term);
    ecs_parser_errorv(name, expr, 0, fmt, args);
    ecs_os_free(expr);

    va_end(args);
}

static
int finalize_term_set(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_id_t *identifier,
    const char *name)
{
    if (identifier->set.mask & EcsParent) {
        identifier->set.mask |= EcsSuperSet;
        identifier->set.relation = EcsChildOf;
    }

    /* Default relation for superset/subset is EcsIsA */
    if (identifier->set.mask & (EcsSuperSet|EcsSubSet)) {
        if (!identifier->set.relation) {
            identifier->set.relation = EcsIsA;
        }

        if (!(identifier->set.mask & EcsSelf)) {
            if (!identifier->set.min_depth) {
                identifier->set.min_depth = 1;
            }
        }
    } else {
        if (identifier->set.min_depth > 0) {
            term_error(world, term, name, 
                "min depth cannnot be non-zero for Self term");
            return -1;
        }
        if (identifier->set.max_depth > 1) {
            term_error(world, term, name, 
                "max depth cannnot be larger than 1 for Self term");
            return -1;
        }

        identifier->set.max_depth = 1;
    }

    if ((identifier->set.mask != EcsNothing) && 
        (identifier->set.mask & EcsNothing)) 
    {
        term_error(world, term, name, "invalid Nothing in set mask");
        return -1;
    }

    return 0;
}

static
int finalize_term_var(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_id_t *identifier,
    const char *name)
{
    if (identifier->var == EcsVarDefault) {
        const char *var = ecs_identifier_is_var(identifier->name);
        if (var) {
            char *var_dup = ecs_os_strdup(var);
            ecs_os_free(identifier->name);
            identifier->name = var_dup;
            identifier->var = EcsVarIsVariable;
        }
    }

    if (identifier->var == EcsVarDefault && identifier->set.mask != EcsNothing){
        identifier->var = EcsVarIsEntity;
    }

    if (!identifier->name) {
        return 0;
    }

    if (identifier->var != EcsVarIsVariable) {
        if (ecs_identifier_is_0(identifier->name)) {
            identifier->entity = 0;
        } else {
            ecs_entity_t e = ecs_lookup_symbol(world, identifier->name, true);
            if (!e) {
                term_error(world, term, name,
                    "unresolved identifier '%s'", identifier->name);
                return -1;
            }

            identifier->entity = e;
        }
    }

    if ((identifier->set.mask == EcsNothing) && 
        (identifier->var != EcsVarDefault)) 
    {
        term_error(world, term, name, "Invalid Nothing with entity");
        return -1;
    }

    if (identifier->var == EcsVarIsEntity) {
        if (identifier->entity && !ecs_is_alive(world, identifier->entity)) {
            term_error(world, term, name, 
                "cannot use not alive entity %u in query",
                (uint32_t)identifier->entity);
            return -1;
        }
    }

    return 0;
}

static
int finalize_term_identifier(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_id_t *identifier,
    const char *name)
{
    if (finalize_term_set(world, term, identifier, name)) {
        return -1;
    }
    if (finalize_term_var(world, term, identifier, name)) {
        return -1;
    }
    return 0;
}

static
bool term_can_inherit(
    ecs_term_t *term) 
{
    /* Hardcoded components that can't be inherited. TODO: replace with
     * relationship property. */
    if (term->pred.entity == EcsChildOf ||
       (term->id == ecs_pair(ecs_id(EcsIdentifier), EcsName)) ||
       (term->id == EcsPrefab) ||
       (term->id == EcsDisabled))
    {
        return false;
    }
    return true;
}

static
ecs_entity_t term_id_entity(
    const ecs_world_t *world,
    ecs_term_id_t *term_id)
{
    if (term_id->entity && term_id->entity != EcsThis && 
        term_id->entity != EcsWildcard && term_id->entity != EcsAny) 
    {
        if (!(term_id->entity & ECS_ROLE_MASK)) {
            return term_id->entity;
        } else {
            return 0;
        }
    } else if (term_id->name) {
        if (term_id->var == EcsVarIsEntity || 
           (term_id->var == EcsVarDefault && 
            !ecs_identifier_is_var(term_id->name))) 
        {
            ecs_entity_t e = ecs_lookup_fullpath(world, term_id->name);
            if (e != EcsWildcard && e != EcsThis && e != EcsAny) {
                return e;
            }
            return 0;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static
int finalize_term_vars(
    const ecs_world_t *world,
    ecs_term_t *term,
    const char *name)
{
    if (finalize_term_var(world, term, &term->pred, name)) {
        return -1;
    }
    if (finalize_term_var(world, term, &term->subj, name)) {
        return -1;
    }
    if (finalize_term_var(world, term, &term->obj, name)) {
        return -1;
    }
    return 0;
}

static
bool entity_is_var(
    ecs_entity_t e)
{
    if (e == EcsThis || e == EcsWildcard || e == EcsAny) {
        return true;
    }
    return false;
}

static
int finalize_term_identifiers(
    const ecs_world_t *world,
    ecs_term_t *term,
    const char *name)
{
    /* By default select subsets for predicates. For example, when the term 
     * matches "Tree", also include "Oak", "Pine", "Elm". */
    if (term->pred.set.mask == EcsDefaultSet) {
        ecs_entity_t e = term_id_entity(world, &term->pred);

        if (e && !ecs_has_id(world, e, EcsFinal)) {
            term->pred.set.mask = EcsSelf|EcsSubSet;
        } else {
            /* If predicate is final, don't search subsets */
            term->pred.set.mask = EcsSelf;
        }
    }

    /* By default select supersets for subjects. For example, when an entity has
     * (IsA, SpaceShip), also search the components of SpaceShip. */
    if (term->subj.set.mask == EcsDefaultSet) {
        ecs_entity_t e = term_id_entity(world, &term->pred);

        /* If the component has the DontInherit tag, use EcsSelf */
        if (!e || !ecs_has_id(world, e, EcsDontInherit)) {
            term->subj.set.mask = EcsSelf|EcsSuperSet;
        } else {
            term->subj.set.mask = EcsSelf;
        }
    }

    /* By default select self for objects. */
    if (term->obj.set.mask == EcsDefaultSet) {
        term->obj.set.mask = EcsSelf;
    }

    if (finalize_term_set(world, term, &term->pred, name)) {
        return -1;
    }
    if (finalize_term_set(world, term, &term->subj, name)) {
        return -1;
    }
    if (finalize_term_set(world, term, &term->obj, name)) {
        return -1;
    }

    if (term->pred.set.mask & EcsNothing) {
        term_error(world, term, name, 
            "invalid Nothing value for predicate set mask");
        return -1;
    }

    if (term->obj.set.mask & EcsNothing) {
        term_error(world, term, name, 
            "invalid Nothing value for object set mask");
        return -1;
    }

    if (!(term->subj.set.mask & EcsNothing) && 
        !term->subj.entity && 
        term->subj.var == EcsVarIsEntity) 
    {
        term->subj.entity = EcsThis;
    }
    
    if (entity_is_var(term->pred.entity)) {
        term->pred.var = EcsVarIsVariable;
    }
    if (entity_is_var(term->subj.entity)) {
        term->subj.var = EcsVarIsVariable;
    }
    if (entity_is_var(term->obj.entity)) {
        term->obj.var = EcsVarIsVariable;
    }

    return 0;
}

static
ecs_entity_t entity_from_identifier(
    const ecs_term_id_t *identifier)
{
    if (identifier->var == EcsVarDefault) {
        return 0;
    } else if (identifier->var == EcsVarIsEntity) {
        return identifier->entity;
    } else if (identifier->var == EcsVarIsVariable) {
        return EcsWildcard;
    } else {
        /* This should've been caught earlier */
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

static
int finalize_term_id(
    const ecs_world_t *world,
    ecs_term_t *term,
    const char *name)
{
    ecs_entity_t pred = entity_from_identifier(&term->pred);
    ecs_entity_t obj = entity_from_identifier(&term->obj);
    ecs_id_t role = term->role;

    if (ECS_HAS_ROLE(pred, PAIR)) {
        if (obj) {
            term_error(world, term, name, 
                "cannot set term.pred to a pair and term.obj at the same time");
            return -1;
        }

        obj = ECS_PAIR_SECOND(pred);
        pred = ECS_PAIR_FIRST(pred);

        term->pred.entity = pred;
        term->obj.entity = obj;

        if (finalize_term_identifier(world, term, &term->obj, name)) {
            return -1;
        }
    }

    if (!obj && role != ECS_PAIR) {
        term->id = pred | role;
    } else {
        if (role) {
            if (role && role != ECS_PAIR && role != ECS_CASE) {
                term_error(world, term, name, "invalid role for pair");
                return -1;
            }

            term->role = role;
        } else {
            term->role = ECS_PAIR;
        }

        term->id = term->role | ecs_entity_t_comb(obj, pred);
    }

    return 0;
}

static
int populate_from_term_id(
    const ecs_world_t *world,
    ecs_term_t *term,
    const char *name)
{
    ecs_entity_t pred = 0;
    ecs_entity_t obj = 0;
    ecs_id_t role = term->id & ECS_ROLE_MASK;

    if (!role && term->role) {
        role = term->role;
        term->id |= role;
    }

    if (term->role && term->role != role) {
        term_error(world, term, name, "mismatch between term.id & term.role");
        return -1;
    }

    term->role = role;

    if (ECS_HAS_ROLE(term->id, PAIR) || ECS_HAS_ROLE(term->id, CASE)) {
        pred = ECS_PAIR_FIRST(term->id);
        obj = ECS_PAIR_SECOND(term->id);

        if (!pred) {
            term_error(world, term, name, "missing predicate in term.id pair");
            return -1;
        }
        if (!obj) {
            if (pred != EcsChildOf) {
                term_error(world, term, name, "missing object in term.id pair");
                return -1;
            }
        }
    } else {
        pred = term->id & ECS_COMPONENT_MASK;
        if (!pred) {
            term_error(world, term, name, "missing predicate in term.id");
            return -1;
        }
    }

    ecs_entity_t term_pred = entity_from_identifier(&term->pred);
    if (term_pred) {
        if (term_pred != pred) {
            term_error(world, term, name, 
                "mismatch between term.id and term.pred");
            return -1;
        }
    } else {
        term->pred.entity = pred;
        if (finalize_term_identifier(world, term, &term->pred, name)) {
            return -1;
        }
    }

    ecs_entity_t term_obj = entity_from_identifier(&term->obj);
    if (term_obj) {
        if (ecs_entity_t_lo(term_obj) != obj) {
            term_error(world, term, name, 
                "mismatch between term.id and term.obj");
            return -1;
        }
    } else {
        term->obj.entity = obj;
        if (finalize_term_identifier(world, term, &term->obj, name)) {
            return -1;
        }
    }

    return 0;
}

static
int verify_term_consistency(
    const ecs_world_t *world,
    const ecs_term_t *term,
    const char *name)
{
    ecs_entity_t pred = entity_from_identifier(&term->pred);
    ecs_entity_t obj = entity_from_identifier(&term->obj);
    ecs_id_t role = term->role;
    ecs_id_t id = term->id;
    bool wildcard = pred == EcsWildcard || obj == EcsWildcard;

    if (obj && (!role || (role != ECS_PAIR && role != ECS_CASE))) {
        term_error(world, term, name, 
            "invalid role for term with pair (expected ECS_PAIR)");
        return -1;
    }

    if (role == ECS_CASE && !obj) {
        term_error(world, term, name, 
            "missing object for term with ECS_CASE role");
        return -1;
    }

    if (!pred) {
        term_error(world, term, name, "missing predicate for term");
        return -1;
    }

    if (role != (id & ECS_ROLE_MASK)) {
        term_error(world, term, name, "mismatch between term.role & term.id");
        return -1;
    }

    if (obj && !ECS_HAS_ROLE(id, PAIR) && !ECS_HAS_ROLE(id, CASE)) {
        term_error(world, term, name, "term has object but id is not a pair");
        return -1;
    }

    if (ECS_HAS_ROLE(id, PAIR) || ECS_HAS_ROLE(id, CASE)) {
        if (!wildcard) {
            role = ECS_ROLE_MASK & id;
            if (id != (role | ecs_entity_t_comb(
                term->obj.entity, term->pred.entity))) 
            {
                char *id_str = ecs_id_str(world, ecs_pair(pred, obj));
                term_error(world, term, name, 
                    "term id does not match pred/obj (%s)", id_str);
                ecs_os_free(id_str);
                return -1;
            }
        }
    } else if (term->pred.entity != (id & ECS_COMPONENT_MASK)) {
        if (!wildcard) {
            char *pred_str = ecs_get_fullpath(world, term->pred.entity);
            term_error(world, term, name, "term id does not match pred '%s'",
                pred_str);
            ecs_os_free(pred_str);
            return -1;
        }
    }

    if (term->pred.var == EcsVarIsEntity) {
        const ecs_term_id_t *tsubj = &term->subj;
        const ecs_term_id_t *tobj = &term->obj;

        if (ecs_term_id_is_set(tsubj) && ecs_term_id_is_set(tobj)) {
            if (tsubj->var == tobj->var) {
                bool is_same = false;

                if (tsubj->var == EcsVarIsEntity) {
                    is_same = tsubj->entity == tobj->entity;
                } else if (tsubj->name && tobj->name) {
                    is_same = !ecs_os_strcmp(tsubj->name, tobj->name);
                }

                if (is_same && ecs_has_id(world, term->pred.entity, EcsAcyclic)
                    && !ecs_has_id(world, term->pred.entity, EcsReflexive)) 
                {
                    char *pred_str = ecs_get_fullpath(world, term->pred.entity);
                    term_error(world, term, name, "term with acyclic relation"
                        " '%s' cannot have same subject and object",
                            pred_str);
                    ecs_os_free(pred_str);
                    return -1;
                }
            }
        }
    }

    if (term->subj.set.relation && !term->subj.set.max_depth) {
        if (!ecs_has_id(world, term->subj.set.relation, EcsAcyclic)) {
            char *r_str = ecs_get_fullpath(world, term->subj.set.relation);
            term_error(world, term, name, 
                "relation '%s' is used with SuperSet/SubSet but is not acyclic", 
                    r_str);
            ecs_os_free(r_str);
            return -1;
        }
    }

    return 0;
}

bool ecs_identifier_is_0(
    const char *id)
{
    return id[0] == '0' && !id[1];
}

const char* ecs_identifier_is_var(
    const char *id)
{
    if (!id) {
        return NULL;
    }

    /* Variable identifiers cannot start with a number */
    if (isdigit(id[0])) {
        return NULL;
    }

    /* Identifiers that start with _ are variables */
    if (id[0] == '_' && id[1] != 0) {
        return &id[1];
    }

    return NULL;
}

bool ecs_id_match(
    ecs_id_t id,
    ecs_id_t pattern)
{
    if (id == pattern) {
        return true;
    }

    if (ECS_HAS_ROLE(pattern, PAIR)) {
        if (!ECS_HAS_ROLE(id, PAIR)) {
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
        } else if (pattern_obj == EcsWildcard) {
            if (pattern_rel == id_rel) {
                return true;
            }
        }
    } else {
        if ((id & ECS_ROLE_MASK) != (pattern & ECS_ROLE_MASK)) {
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
    return ECS_HAS_ROLE(id, PAIR);
}

bool ecs_id_is_wildcard(
    ecs_id_t id)
{
    return
        (id == EcsWildcard) || (ECS_HAS_ROLE(id, PAIR) && (
            (ECS_PAIR_FIRST(id) == EcsWildcard) ||
            (ECS_PAIR_SECOND(id) == EcsWildcard)
        ));
}

bool ecs_term_id_is_set(
    const ecs_term_id_t *id)
{
    return id->entity != 0 || id->name != NULL;
}

bool ecs_term_is_initialized(
    const ecs_term_t *term)
{
    return term->id != 0 || ecs_term_id_is_set(&term->pred);
}

bool ecs_term_is_trivial(
    const ecs_term_t *term)
{
    if (term->inout != EcsInOutDefault) {
        return false;
    }

    if (term->subj.entity != EcsThis) {
        return false;
    }

    if (term->subj.set.mask && (term->subj.set.mask != EcsSelf)) {
        return false;
    }

    if (term->oper != EcsAnd && term->oper != EcsAndFrom) {
        return false;
    }

    if (term->name != NULL) {
        return false;
    }

    return true;
}

int ecs_term_finalize(
    const ecs_world_t *world,
    const char *name,
    ecs_term_t *term)
{
    if (finalize_term_vars(world, term, name)) {
        return -1;
    }

    if (!term->id) {
        if (finalize_term_id(world, term, name)) {
            return -1;
        }
    } else {
        if (populate_from_term_id(world, term, name)) {
            return -1;
        }
    }

    if (finalize_term_identifiers(world, term, name)) {
        return -1;
    }

    if (!term_can_inherit(term)) {
        if (term->subj.set.relation == EcsIsA) {
            term->subj.set.relation = 0;
            term->subj.set.mask = EcsSelf;
        }
    }

    if (term->role == ECS_AND || term->role == ECS_OR || term->role == ECS_NOT){
        /* AND/OR terms match >1 component, which is only valid as filter */
        if (term->inout != EcsInOutDefault && term->inout != EcsInOutFilter) {
            term_error(world, term, name, "AND/OR terms must be filters");
            return -1;
        }

        term->inout = EcsInOutFilter;

        /* Translate role to operator */
        if (term->role == ECS_AND) {
            term->oper = EcsAndFrom;
        } else
        if (term->role == ECS_OR) {
            term->oper = EcsOrFrom;
        } else
        if (term->role == ECS_NOT) {
            term->oper = EcsNotFrom;
        }

        /* Zero out role & strip from id */
        term->id &= ECS_COMPONENT_MASK;
        term->role = 0;
    }

    if (verify_term_consistency(world, term, name)) {
        return -1;
    }

    return 0;
}

ecs_term_t ecs_term_copy(
    const ecs_term_t *src)
{
    ecs_term_t dst = *src;
    dst.name = ecs_os_strdup(src->name);
    dst.pred.name = ecs_os_strdup(src->pred.name);
    dst.subj.name = ecs_os_strdup(src->subj.name);
    dst.obj.name = ecs_os_strdup(src->obj.name);
    return dst;
}

ecs_term_t ecs_term_move(
    ecs_term_t *src)
{
    if (src->move) {
        ecs_term_t dst = *src;
        src->name = NULL;
        src->pred.name = NULL;
        src->subj.name = NULL;
        src->obj.name = NULL;
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
    ecs_os_free(term->pred.name);
    ecs_os_free(term->subj.name);
    ecs_os_free(term->obj.name);
    ecs_os_free(term->name);

    term->pred.name = NULL;
    term->subj.name = NULL;
    term->obj.name = NULL;
    term->name = NULL;
}

int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *f)
{
    int32_t i, term_count = f->term_count, actual_count = 0;
    ecs_term_t *terms = f->terms;
    bool is_or = false, prev_or = false;
    int32_t filter_terms = 0;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (ecs_term_finalize(world, f->name, term)) {
            return -1;
        }

        is_or = term->oper == EcsOr;
        actual_count += !(is_or && prev_or);
        term->index = actual_count - 1;
        prev_or = is_or;

        if (term->subj.entity == EcsThis) {
            f->match_this = true;
            if (term->subj.set.mask != EcsSelf) {
                f->match_only_this = false;
            }
        } else {
            f->match_only_this = false;
        }

        if (term->id == EcsPrefab) {
            f->match_prefab = true;
        }
        if (term->id == EcsDisabled) {
            f->match_disabled = true;
        }

        if (f->filter) {
            term->inout = EcsInOutFilter;
        }

        if (term->inout == EcsInOutFilter) {
            filter_terms ++;
        }

        if (term->oper != EcsNot || term->subj.entity != EcsThis) {
            f->match_anything = false;
        }
    }

    f->term_count_actual = actual_count;

    if (filter_terms == term_count) {
        f->filter = true;
    }

    return 0;
}

/* Implementation for iterable mixin */
static
void filter_iter_init(
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

int ecs_filter_init(
    const ecs_world_t *stage,
    ecs_filter_t *filter_out,
    const ecs_filter_desc_t *desc)    
{
    ecs_filter_t f;
    ecs_poly_init(&f, ecs_filter_t);

    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(filter_out != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    int i, term_count = 0;
    ecs_term_t *terms = desc->terms_buffer;
    const char *name = desc->name;
    const char *expr = desc->expr;

    /* Temporarily set the fields to the values provided in desc, until the
     * filter has been validated. */
    f.name = (char*)name;
    f.expr = (char*)expr;
    f.filter = desc->filter;
    f.instanced = desc->instanced;
    f.match_empty_tables = desc->match_empty_tables;
    f.match_anything = true;

    if (terms) {
        term_count = desc->terms_buffer_count;
    } else {
        terms = (ecs_term_t*)desc->terms;
        for (i = 0; i < ECS_TERM_DESC_CACHE_SIZE; i ++) {
            if (!ecs_term_is_initialized(&terms[i])) {
                break;
            }

            term_count ++;
        }
    }

    /* Temporarily set array from desc to filter, until the filter has been
     * validated. */
    f.terms = terms;
    f.term_count = term_count;

    if (expr) {
#ifdef FLECS_PARSER
        int32_t buffer_count = 0;

        /* If terms have already been set, copy buffer to allocated one */
        if (terms && term_count) {
            terms = ecs_os_memdup(terms, term_count * ECS_SIZEOF(ecs_term_t));
            buffer_count = term_count;
        } else {
            terms = NULL;
        }

        /* Parse expression into array of terms */
        const char *ptr = desc->expr;
        ecs_term_t term = {0};
        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))){
            if (!ecs_term_is_initialized(&term)) {
                break;
            }
            
            if (term_count == buffer_count) {
                buffer_count = buffer_count ? buffer_count * 2 : 8;
                terms = ecs_os_realloc(terms, 
                    buffer_count * ECS_SIZEOF(ecs_term_t));
            }

            /* Check for identifiers that have a name that starts with _. If the
             * variable kind is left to Default, the kind should be set to 
             * variable and the _ prefix should be removed. */
            finalize_term_vars(world, &term, name);

            terms[term_count] = term;
            term_count ++;

            if (ptr[0] == '\n') {
                break;
            }
        }

        f.terms = terms;
        f.term_count = term_count;

        if (!ptr) {
            goto error;
        }
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Copy term resources. */
    if (term_count) {
        ecs_term_t *dst_terms = terms;
        if (!f.expr) {
            if (term_count <= ECS_TERM_CACHE_SIZE) {
                dst_terms = f.term_cache;
                f.term_cache_used = true;
            } else {
                dst_terms = ecs_os_malloc_n(ecs_term_t, term_count);
            }
        }

        for (i = 0; i < term_count; i ++) {
            dst_terms[i] = ecs_term_move(&terms[i]);
        }
        f.terms = dst_terms;
    } else {
        f.terms = NULL;
    }

    /* Ensure all fields are consistent and properly filled out */
    if (ecs_filter_finalize(world, &f)) {
        goto error;
    }

    *filter_out = f;
    if (f.term_cache_used) {
        filter_out->terms = filter_out->term_cache;
    }
    filter_out->name = ecs_os_strdup(desc->name);
    filter_out->expr = ecs_os_strdup(desc->expr);

    ecs_assert(!filter_out->term_cache_used || 
        filter_out->terms == filter_out->term_cache,
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(filter_out->term_count == f.term_count,
        ECS_INTERNAL_ERROR, NULL);

    filter_out->iterable.init = filter_iter_init;

    return 0;
error:
    /* NULL members that point to non-owned resources */
    if (!f.expr) {
        f.terms = NULL;
    }

    f.name = NULL;
    f.expr = NULL;

    ecs_filter_fini(&f);

    return -1;
}

void ecs_filter_copy(
    ecs_filter_t *dst,
    const ecs_filter_t *src)   
{
    if (src) {
        *dst = *src;

        int32_t term_count = src->term_count;

        if (src->term_cache_used) {
            dst->terms = dst->term_cache;
        } else {
            dst->terms = ecs_os_memdup_n(src->terms, ecs_term_t, term_count);
        }

        int i;
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
    if (src) {
        *dst = *src;

        if (src->term_cache_used) {
            dst->terms = dst->term_cache;
        }

        if (dst != src) {
            src->terms = NULL;
            src->term_count = 0;
        }
    } else {
        ecs_os_memset_t(dst, 0, ecs_filter_t);
    }
}

void ecs_filter_fini(
    ecs_filter_t *filter) 
{
    if (filter->terms) {
        int i, count = filter->term_count;
        for (i = 0; i < count; i ++) {
            ecs_term_fini(&filter->terms[i]);
        }

        if (!filter->term_cache_used) {
            ecs_os_free(filter->terms);
        }
    }

    ecs_os_free(filter->name);
    ecs_os_free(filter->expr);

    filter->terms = NULL;
    filter->name = NULL;
    filter->expr = NULL;
}

static
void filter_str_add_id(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_term_id_t *id,
    bool is_subject,
    uint8_t default_set_mask)
{
    if (id->name) {
        ecs_strbuf_appendstr(buf, id->name);
    } else if (id->entity) {
        bool id_added = false;
        if (!is_subject || id->entity != EcsThis) {
            char *path = ecs_get_fullpath(world, id->entity);
            ecs_strbuf_appendstr(buf, path);
            ecs_os_free(path);
            id_added = true;
        }

        if (id->set.mask != default_set_mask) {
            if (id_added) {
                ecs_strbuf_list_push(buf, ":", "|");
            } else {
                ecs_strbuf_list_push(buf, "", "|");
            }
            if (id->set.mask & EcsSelf) {
                ecs_strbuf_list_appendstr(buf, "self");
            }
            if (id->set.mask & EcsSuperSet) {
                ecs_strbuf_list_appendstr(buf, "superset");
            }
            if (id->set.mask & EcsSubSet) {
                ecs_strbuf_list_appendstr(buf, "subset");
            }

            if (id->set.relation != EcsIsA) {
                ecs_strbuf_list_push(buf, "(", "");

                char *rel_path = ecs_get_fullpath(world, id->set.relation);
                ecs_strbuf_appendstr(buf, rel_path);
                ecs_os_free(rel_path);

                ecs_strbuf_list_pop(buf, ")");
            }

            ecs_strbuf_list_pop(buf, "");
        }
    } else {
        ecs_strbuf_appendstr(buf, "0");
    }
}

static
void term_str_w_strbuf(
    const ecs_world_t *world,
    const ecs_term_t *term,
    ecs_strbuf_t *buf)
{
    const ecs_term_id_t *subj = &term->subj;
    const ecs_term_id_t *obj = &term->obj;

    const uint8_t def_pred_mask = EcsSelf|EcsSubSet;
    const uint8_t def_subj_mask = EcsSelf|EcsSuperSet;
    const uint8_t def_obj_mask = EcsSelf;

    bool pred_set = ecs_term_id_is_set(&term->pred);
    bool subj_set = ecs_term_id_is_set(subj);
    bool obj_set = ecs_term_id_is_set(obj);

    if (term->role && term->role != ECS_PAIR) {
        ecs_strbuf_appendstr(buf, ecs_role_str(term->role));
        ecs_strbuf_appendstr(buf, " ");
    }

    if (term->oper == EcsNot) {
        ecs_strbuf_appendstr(buf, "!");
    } else if (term->oper == EcsOptional) {
        ecs_strbuf_appendstr(buf, "?");
    }

    if (!subj_set) {
        filter_str_add_id(world, buf, &term->pred, false, def_pred_mask);
        ecs_strbuf_appendstr(buf, "()");
    } else if (subj_set && subj->entity == EcsThis && subj->set.mask == def_subj_mask)
    {
        if (term->id) {
            char *str = ecs_id_str(world, term->id);
            ecs_strbuf_appendstr(buf, str);
            ecs_os_free(str);
        } else if (pred_set) {
            filter_str_add_id(world, buf, &term->pred, false, def_pred_mask);   
        }
    } else {
        filter_str_add_id(world, buf, &term->pred, false, def_pred_mask);
        ecs_strbuf_appendstr(buf, "(");
        filter_str_add_id(world, buf, &term->subj, true, def_subj_mask);
        if (obj_set) {
            ecs_strbuf_appendstr(buf, ",");
            filter_str_add_id(world, buf, &term->obj, false, def_obj_mask);
        }
        ecs_strbuf_appendstr(buf, ")");
    }
}

char* ecs_term_str(
    const ecs_world_t *world,
    const ecs_term_t *term)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    term_str_w_strbuf(world, term, &buf);
    return ecs_strbuf_get(&buf);
}

char* ecs_filter_str(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    ecs_check(!filter->term_cache_used || filter->terms == filter->term_cache,
        ECS_INVALID_PARAMETER, NULL);

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;
    int32_t or_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];

        if (i) {
            if (terms[i - 1].oper == EcsOr && term->oper == EcsOr) {
                ecs_strbuf_appendstr(&buf, " || ");
            } else {
                ecs_strbuf_appendstr(&buf, ", ");
            }
        }

        if (or_count < 1) {
            if (term->inout == EcsIn) {
                ecs_strbuf_appendstr(&buf, "[in] ");
            } else if (term->inout == EcsInOut) {
                ecs_strbuf_appendstr(&buf, "[inout] ");
            } else if (term->inout == EcsOut) {
                ecs_strbuf_appendstr(&buf, "[out] ");
            } else if (term->inout == EcsInOutFilter) {
                ecs_strbuf_appendstr(&buf, "[filter] ");
            }
        }

        if (term->oper == EcsOr) {
            or_count ++;
        } else {
            or_count = 0;
        }

        term_str_w_strbuf(world, term, &buf);
    }

    return ecs_strbuf_get(&buf);
error:
    return NULL;
}

static
ecs_id_t actual_match_id(
    ecs_id_t id)
{
    /* Table types don't store CASE, so replace it with corresponding SWITCH */
    if (ECS_HAS_ROLE(id, CASE)) {
        return ECS_SWITCH | ECS_PAIR_FIRST(id);
    }

    return id;
}

static
bool flecs_n_term_match_table(
    ecs_world_t *world,
    const ecs_term_t *term,
    const ecs_table_t *table,
    ecs_type_t type,
    ecs_id_t *id_out,
    int32_t *column_out,
    ecs_entity_t *subject_out,
    int32_t *match_index_out,
    bool first)
{
    (void)column_out;
    
    ecs_entity_t type_id = term->id;
    ecs_oper_kind_t oper = term->oper;

    const EcsType *term_type = ecs_get(world, type_id, EcsType);
    ecs_check(term_type != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t *ids = ecs_vector_first(term_type->normalized->type, ecs_id_t);
    int32_t i, count = ecs_vector_count(term_type->normalized->type);
    ecs_term_t temp = *term;
    temp.oper = EcsAnd;

    for (i = 0; i < count; i ++) {
        temp.id = ids[i];
        bool result = flecs_term_match_table(world, &temp, table, type, id_out, 
            0, subject_out, match_index_out, first);
        if (!result && oper == EcsAndFrom) {
            return false;
        } else
        if (result && oper == EcsOrFrom) {
            return true;
        }
    }

    if (oper == EcsAndFrom) {
        return true;
    } else
    if (oper == EcsOrFrom) {
        return false;
    }

error:
    return false;
}

bool flecs_term_match_table(
    ecs_world_t *world,
    const ecs_term_t *term,
    const ecs_table_t *table,
    ecs_type_t type,
    ecs_id_t *id_out,
    int32_t *column_out,
    ecs_entity_t *subject_out,
    int32_t *match_index_out,
    bool first)
{
    const ecs_term_id_t *subj = &term->subj;
    ecs_oper_kind_t oper = term->oper;
    const ecs_table_t *match_table = table;
    ecs_type_t match_type = type;
    ecs_id_t id = term->id;

    ecs_entity_t subj_entity = subj->entity;
    if (!subj_entity) {
        id_out[0] = id; /* no source corresponds with Nothing set mask */
        return true;
    }

    if (oper == EcsAndFrom || oper == EcsOrFrom) {
        return flecs_n_term_match_table(world, term, table, type, id_out, column_out, 
            subject_out, match_index_out, first);
    }

    /* If source is not This, search in table of source */
    if (subj_entity != EcsThis) {
        match_table = ecs_get_table(world, subj_entity);
        if (match_table) {
            match_type = match_table->type;
        } else {
            return false;
        }
    } else {
        /* If filter contains This terms, a table must be provided */
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (!match_type) {
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
    column = ecs_search_relation(world, match_table,
        column, actual_match_id(id), subj->set.relation, subj->set.min_depth, 
        subj->set.max_depth, &source, id_out, &tr);

    if (tr && match_index_out) {
        match_index_out[0] = tr->count;
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
        return false;
    }

    if (subj_entity != EcsThis) {
        if (!source) {
            source = subj_entity;
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
    ecs_entity_t *subjects,
    int32_t *match_indices,
    int32_t *matches_left,
    bool first,
    int32_t skip_term)
{
    ecs_assert(!filter->term_cache_used || filter->terms == filter->term_cache,
        ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = NULL;
    if (table) {
        type = table->type;
    }

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    bool is_or = false;
    bool or_result = false;
    int32_t match_count = 1;
    if (matches_left) {
        match_count = *matches_left;
    }

    for (i = 0; i < count; i ++) {
        if (i == skip_term) {
            continue;
        }

        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->subj;
        ecs_oper_kind_t oper = term->oper;
        const ecs_table_t *match_table = table;
        ecs_type_t match_type = type;
        int32_t t_i = term->index;

        if (!is_or && oper == EcsOr) {
            is_or = true;
            or_result = false;
        } else if (is_or && oper != EcsOr) {
            if (!or_result) {
                return false;
            }

            is_or = false;
        }

        ecs_entity_t subj_entity = subj->entity;
        if (!subj_entity) {
            if (ids) {
                ids[t_i] = term->id;
            }
            continue;
        }

        if (subj_entity != EcsThis) {
            match_table = ecs_get_table(world, subj_entity);
            if (match_table) {
                match_type = match_table->type;
            } else {
                match_type = NULL;
            }
        } else {
            /* If filter contains This terms, table must be provided */
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        }

        int32_t match_index = 0;

        bool result = flecs_term_match_table(world, term, match_table, 
            match_type,
            ids ? &ids[t_i] : NULL, 
            columns ? &columns[t_i] : NULL, 
            subjects ? &subjects[t_i] : NULL, 
            &match_index,
            first);

        if (is_or) {
            or_result |= result;
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
    iter->term = (ecs_term_t){ .index = -1 };
    iter->self_index = NULL;
    iter->index = 0;
}

static
void term_iter_init_wildcard(
    const ecs_world_t *world,
    ecs_term_iter_t *iter)
{
    iter->term = (ecs_term_t){ .index = -1 };
    iter->self_index = flecs_get_id_record(world, EcsWildcard);
    iter->cur = iter->self_index;
    iter->index = 0;
}

static
void term_iter_init(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_iter_t *iter)
{    
    const ecs_term_id_t *subj = &term->subj;

    iter->term = *term;

    if (subj->set.mask == EcsDefaultSet || subj->set.mask & EcsSelf) {
        iter->self_index = flecs_get_id_record(world, 
            actual_match_id(term->id));
    }

    if (subj->set.mask & EcsSuperSet) {
        iter->set_index = flecs_get_id_record(world, 
            ecs_pair(subj->set.relation, EcsWildcard));
    }

    iter->index = 0;
    if (iter->self_index) {
        iter->cur = iter->self_index;
    } else {
        iter->cur = iter->set_index;
    }
}

ecs_iter_t ecs_term_iter(
    const ecs_world_t *stage,
    ecs_term_t *term)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term->id != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_process_pending_tables(world);

    if (ecs_term_finalize(world, NULL, term)) {
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .term_count = 1,
        .next = ecs_term_next
    };

    term_iter_init(world, term, &it.priv.iter.term);

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

    if (ecs_term_finalize(world, NULL, term)) {
        ecs_throw(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = chain_it->world,
        .terms = term,
        .term_count = 1,
        .chain_it = (ecs_iter_t*)chain_it,
        .next = ecs_term_next
    };

    term_iter_init(world, term, &it.priv.iter.term);

    return it;
error:
    return (ecs_iter_t){ 0 };
}

static
const ecs_table_record_t *next_table(
    ecs_term_iter_t *iter)
{
    ecs_id_record_t *idr = iter->cur;
    if (!idr) {
        return NULL;
    }

    ecs_table_iter_t it;
    
    if (iter->empty_tables) {
        if (flecs_idr_empty_iter(idr, &it)) {
            it.cur = it.begin + iter->index;
        }
        if (it.cur >= it.end) {
            iter->empty_tables = false;
            iter->index = 0;
        }
    }

    if (!iter->empty_tables) {
        if (flecs_idr_iter(idr, &it)) {
            it.cur = it.begin + iter->index;
        }
        if (it.cur >= it.end) {
            return NULL;
        }
    }

    iter->index ++;

    return it.cur;
}

static
bool term_iter_next(
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
                    iter->id = ecs_vector_get(
                        table->type, ecs_id_t, iter->last_column)[0];
                }
            }
        }

        if (!table) {
            if (!(tr = next_table(iter))) {
                if (iter->cur != iter->set_index && iter->set_index != NULL) {
                    iter->cur = iter->set_index;
                    iter->index = 0;
                    tr = next_table(iter);
                }

                if (!tr) {
                    return false;
                }
            }

            table = tr->table;

            if (!match_prefab && (table->flags & EcsTableIsPrefab)) {
                continue;
            }

            if (!match_disabled && (table->flags & EcsTableIsDisabled)) {
                continue;
            }

            iter->table = table;
            iter->match_count = tr->count;
            iter->cur_match = 0;
            iter->last_column = tr->column;
            iter->column = tr->column + 1;
            iter->id = ecs_vector_get(table->type, ecs_id_t, tr->column)[0];
        }

        if (iter->cur == iter->set_index) {
            const ecs_term_id_t *subj = &term->subj;

            if (iter->self_index) {
                if (flecs_id_record_table(iter->self_index, table) != NULL) {
                    /* If the table has the id itself and this term matched Self
                     * we already matched it */
                    continue;
                }
            }

            /* Test if following the relation finds the id */
            int32_t index = ecs_search_relation(world, table, 0, 
                term->id, subj->set.relation, subj->set.min_depth, 
                subj->set.max_depth, &source, &iter->id, NULL);

            if (index == -1) {
                source = 0;
                continue;
            }

            ecs_assert(source != 0, ECS_INTERNAL_ERROR, NULL);

            iter->column = (index + 1) * -1;
        }

        break;
    } while (true);

    iter->subject = source;

    return true;
}

bool ecs_term_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_term_next, ECS_INVALID_PARAMETER, NULL);

    ecs_term_iter_t *iter = &it->priv.iter.term;
    ecs_term_t *term = &iter->term;
    ecs_world_t *world = it->real_world;
    ecs_table_t *table;

    it->ids = &iter->id;
    it->subjects = &iter->subject;
    it->columns = &iter->column;
    it->terms = &iter->term;

    if (term->inout != EcsInOutFilter) {
        it->sizes = &iter->size;
        it->ptrs = &iter->ptr;
    } else {
        it->sizes = NULL;
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
            match = flecs_term_match_table(world, term, table, table->type,
                it->ids, it->columns, it->subjects, it->match_indices, true);
        } while (!match);
        goto yield;

    } else {
        if (!term_iter_next(world, iter, false, false)) {
            goto done;
        }

        table = iter->table;

        /* Source must either be 0 (EcsThis) or nonzero in case of substitution */
        ecs_assert(iter->subject || iter->cur != iter->set_index, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(iter->table != NULL, ECS_INTERNAL_ERROR, NULL);
    }

yield:
    flecs_iter_populate_data(world, it, table, 0, 0, it->ptrs, it->sizes);
    it->is_valid = true;
    return true;
done:
error:
    return false;
}

static
const ecs_filter_t* init_filter_iter(
    const ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_filter_t *filter)
{
    ecs_filter_iter_t *iter = &it->priv.iter.filter;

    if (filter) {
        iter->filter = *filter;

        if (filter->term_cache_used) {
            iter->filter.terms = iter->filter.term_cache;
        }

        ecs_filter_finalize(world, &iter->filter);

        ecs_assert(!filter->term_cache_used || 
            filter->terms == filter->term_cache, ECS_INTERNAL_ERROR, NULL);    
    } else {
        ecs_filter_init(world, &iter->filter, &(ecs_filter_desc_t) {
            .terms = {{ .id = EcsWildcard }}
        });

        filter = &iter->filter;
    }

    it->term_count = filter->term_count_actual;

    return filter;
}

int32_t ecs_filter_pivot_term(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(filter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_term_t *terms = filter->terms;
    int32_t i, term_count = filter->term_count;
    int32_t pivot_term = -1, min_count = -1;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_id_t id = term->id;

        if (term->oper != EcsAnd) {
            continue;
        }

        if (term->subj.entity != EcsThis) {
            continue;
        }

        ecs_id_record_t *idr = flecs_get_id_record(world,   
            actual_match_id(id));
        if (!idr) {
            /* If one of the terms does not match with any data, iterator 
             * should not return anything */
            return -2; /* -2 indicates filter doesn't match anything */
        }

        int32_t table_count = ecs_table_cache_count(&idr->cache);
        if (min_count == -1 || table_count < min_count) {
            min_count = table_count;
            pivot_term = i;
        }
    }

    return pivot_term;
error:
    return -2;
}

ecs_iter_t ecs_filter_iter(
    const ecs_world_t *stage,
    const ecs_filter_t *filter)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);
    
    flecs_process_pending_tables(world);

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .terms = filter ? filter->terms : NULL,
        .next = ecs_filter_next,
        .is_instanced = filter ? filter->instanced : false
    };

    ecs_filter_iter_t *iter = &it.priv.iter.filter;

    filter = init_filter_iter(world, &it, filter);

    /* Find term that represents smallest superset */
    if (filter->match_this) {
        ecs_term_t *terms = filter->terms;
        int32_t pivot_term = -1;
        ecs_check(terms != NULL, ECS_INVALID_PARAMETER, NULL);

        iter->kind = EcsIterEvalIndex;

        pivot_term = ecs_filter_pivot_term(world, filter);
        if (pivot_term == -2) {
            /* One or more terms have no matching results */
            term_iter_init_no_data(&iter->term_iter);
            return it;
        } else if (pivot_term == -1) {
            /* No terms meet the criteria to be a pivot term, evaluate filter
             * against all tables */
            term_iter_init_wildcard(world, &iter->term_iter);
        } else {
            ecs_assert(pivot_term >= 0, ECS_INTERNAL_ERROR, NULL);
            term_iter_init(world, &terms[pivot_term], &iter->term_iter);
        }

        iter->term_iter.empty_tables = filter->match_empty_tables;
    } else {
        if (!filter->match_anything) {
            iter->kind = EcsIterEvalCondition;
            term_iter_init_no_data(&iter->term_iter);
        } else {
            iter->kind = EcsIterEvalNone;
        }
    }

    if (filter->terms == filter->term_cache) {
        /* Because we're returning the iterator by value, the address of the
         * term cache changes. The ecs_filter_next function will set the correct
         * address when it detects that terms is set to NULL */
        iter->filter.terms = NULL;
    }

    it.is_filter = filter->filter;

    return it;
error:
    return (ecs_iter_t){ 0 };
}

ecs_iter_t ecs_filter_chain_iter(
    const ecs_iter_t *chain_it,
    const ecs_filter_t *filter)
{
    ecs_iter_t it = {
        .terms = filter->terms,
        .term_count = filter->term_count,
        .world = chain_it->world,
        .real_world = chain_it->real_world,
        .chain_it = (ecs_iter_t*)chain_it,
        .next = ecs_filter_next
    };

    ecs_filter_iter_t *iter = &it.priv.iter.filter;
    init_filter_iter(it.world, &it, filter);

    iter->kind = EcsIterEvalChain;

    if (filter->terms == filter->term_cache) {
        /* See ecs_filter_iter */
        iter->filter.terms = NULL;
    }

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
    ecs_filter_t *filter = &iter->filter;
    ecs_world_t *world = it->real_world;
    ecs_table_t *table = NULL;
    bool match;

    if (!filter->terms) {
        filter->terms = filter->term_cache;
    }

    flecs_iter_init(it);

    ecs_iter_t *chain_it = it->chain_it;
    ecs_iter_kind_t kind = iter->kind;

    if (chain_it) {
        ecs_assert(kind == EcsIterEvalChain, ECS_INVALID_PARAMETER, NULL);
        
        ecs_iter_next_action_t next = chain_it->next;
        do {
            if (!next(chain_it)) {
                goto done;
            }

            table = chain_it->table;
            match = flecs_filter_match_table(world, filter, table,
                it->ids, it->columns, it->subjects, it->match_indices, NULL, 
                true, -1);
        } while (!match);

        goto yield;
    } else if (kind == EcsIterEvalIndex || kind == EcsIterEvalCondition) {
        ecs_term_iter_t *term_iter = &iter->term_iter;
        ecs_term_t *term = &term_iter->term;
        int32_t pivot_term = term->index;
        bool first;

        do {
            first = iter->matches_left == 0;

            if (first) {
                if (kind != EcsIterEvalCondition) {
                    /* Find new match, starting with the leading term */
                    if (!term_iter_next(world, term_iter, 
                        filter->match_prefab, filter->match_disabled)) 
                    {
                        goto done;
                    }

                    ecs_assert(term_iter->match_count != 0, 
                        ECS_INTERNAL_ERROR, NULL);

                    if (pivot_term == -1) {
                        /* Without a pivot term, we're iterating all tables with
                         * a wildcard, so the match count is meaningless. */
                        term_iter->match_count = 1;
                    }

                    iter->matches_left = term_iter->match_count;

                    /* Filter iterator takes control over iterating all the
                     * permutations that match the wildcard. */
                    term_iter->match_count = 1;

                    table = term_iter->table;
                    if (pivot_term != -1) {
                        it->ids[pivot_term] = term_iter->id;
                        it->subjects[pivot_term] = term_iter->subject;
                        it->columns[pivot_term] = term_iter->column;
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
                    it->ids, it->columns, it->subjects,
                    it->match_indices, &iter->matches_left, first, 
                    pivot_term);
                if (!match) {
                    iter->matches_left = 0;
                    continue;
                }
                    
                ecs_assert(iter->matches_left != 0, ECS_INTERNAL_ERROR, NULL);
            }

            /* If this is not the first result for the table, and the table
             * is matched more than once, iterate remaining matches */
            if (!first && (iter->matches_left > 0)) {
                table = it->table;
                
                /* Find first term that still has matches left */
                int32_t i, j, count = it->term_count;
                for (i = count - 1; i >= 0; i --) {
                    int32_t mi = -- it->match_indices[i];
                    if (mi) {
                        break;
                    }
                }

                /* Progress first term to next match (must be at least one) */
                it->columns[i] ++;
                flecs_term_match_table(world, &filter->terms[i], table, 
                    table->type, &it->ids[i], &it->columns[i], &it->subjects[i],
                    &it->match_indices[i], false);

                /* Reset remaining terms (if any) to first match */
                for (j = i + 1; j < count; j ++) {
                    flecs_term_match_table(world, &filter->terms[j], table, 
                        table->type, &it->ids[j], &it->columns[j], 
                        &it->subjects[j], &it->match_indices[j], true);
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
    flecs_iter_populate_data(world, it, table, 0, 0, it->ptrs, it->sizes);
    it->is_valid = true;
    return true;    
}
