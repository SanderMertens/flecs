
#include "private_api.h"

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
ecs_entity_t term_id_entity(
    const ecs_world_t *world,
    ecs_term_id_t *term_id)
{
    if (term_id->entity && term_id->entity != EcsThis && 
        term_id->entity != EcsWildcard) 
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
            return ecs_lookup_fullpath(world, term_id->name);
        } else {
            return 0;
        }
    } else {
        return 0;
    }
}

static
void finalize_default_substitution(
    const ecs_world_t *world,
    ecs_term_t *terms,
    int32_t term_count)
{
    int i;
    for (i = 0; i < term_count; i ++) {
        ecs_term_id_t *pred = &terms[i].pred;
        ecs_term_id_t *subj = &terms[i].args[0];
        ecs_term_id_t *obj = &terms[i].args[1];

        bool pred_transitive = false;
        if (pred->set.mask == EcsDefaultSet) {
            ecs_entity_t e = term_id_entity(world, pred);
            if (e) {
                if (ecs_has_id(world, e, EcsFinal)) {
                    /* Final predicates are never substituted */
                    pred->set.mask = EcsSelf;
                } else {
                    pred->set.mask = EcsSelf|EcsSuperSet;
                    pred->set.relation = EcsIsA;
                }
                pred_transitive = ecs_has_id(world, e, EcsTransitive);
            }
        }

        if (subj->set.mask == EcsDefaultSet) {
            /* By default subjects are substituted with their supersets */
            subj->set.mask = EcsSelf|EcsSuperSet;
            subj->set.relation = EcsIsA;
        }

        if (obj->set.mask == EcsDefaultSet) {
            /* By default subjects are substituted with their supersets */
            if (!pred_transitive) {
                obj->set.mask = EcsSelf|EcsSuperSet;
                obj->set.relation = EcsIsA;
            } else {
                /* If query is transitive substitute object. If object is fixed
                 * insert SubSet ("is <subj> <rel> <obj>"). If object is a
                 * variable, insert SuperSet ("find <rel> <obj> for <subj>") */
                ecs_entity_t e = term_id_entity(world, obj);
                obj->set.relation = EcsIsA;
                if (!e) {
                    obj->set.mask = EcsSelf|EcsSuperSet;
                } else {
                    obj->set.mask = EcsSelf|EcsSubSet;
                }
            }
        }
    }
}

static
int finalize_term_identifier(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_id_t *identifier,
    const char *name)
{
    /* Default set is Self */
    if (identifier->set.mask == EcsDefaultSet) {
        identifier->set.mask |= EcsSelf;
    }

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

    if (identifier->var == EcsVarDefault) {
        const char *var = ecs_identifier_is_var(identifier->name);
        if (ecs_identifier_is_var(identifier->name)) {
            char *var_id = ecs_os_strdup(var);
            ecs_os_free(identifier->name);
            identifier->name = var_id;
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

    return 0;
}

static
int finalize_term_identifiers(
    const ecs_world_t *world,
    ecs_term_t *term,
    const char *name)
{
    if (finalize_term_identifier(world, term, &term->pred, name)) {
        return -1;
    }
    if (finalize_term_identifier(world, term, &term->args[0], name)) {
        return -1;
    }
    if (finalize_term_identifier(world, term, &term->args[1], name)) {
        return -1;
    }

    if (term->pred.set.mask & EcsNothing) {
        term_error(world, term, name, 
            "invalid Nothing value for predicate set mask");
        return -1;
    }

    if (term->args[1].set.mask & EcsNothing) {
        term_error(world, term, name, 
            "invalid Nothing value for object set mask");
        return -1;
    }

    if (!(term->args[0].set.mask & EcsNothing) && 
        !term->args[0].entity && 
        term->args[0].var == EcsVarIsEntity) 
    {
        term->args[0].entity = EcsThis;
    }
    
    if (term->pred.entity == EcsThis) {
        term->pred.var = EcsVarIsVariable;
    }
    if (term->args[0].entity == EcsThis) {
        term->args[0].var = EcsVarIsVariable;
    }
    if (term->args[1].entity == EcsThis) {
        term->args[1].var = EcsVarIsVariable;
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
    ecs_entity_t obj = entity_from_identifier(&term->args[1]);
    ecs_id_t role = term->role;

    if (ECS_HAS_ROLE(pred, PAIR)) {
        if (obj) {
            term_error(world, term, name, 
                "cannot set term.pred to a pair and term.obj at the same time");
            return -1;
        }

        obj = ECS_PAIR_OBJECT(pred);
        pred = ECS_PAIR_RELATION(pred);

        term->pred.entity = pred;
        term->args[1].entity = obj;

        finalize_term_identifier(world, term, &term->args[1], name);
    }

    if (!obj && role != ECS_PAIR) {
        term->id = pred | role;
    } else {
        if (role && role != ECS_PAIR) {
            term_error(world, term, name, "invalid role for pair");
            return -1;
        }

        term->id = ecs_pair(pred, obj);
        term->role = ECS_PAIR;
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

    if (ECS_HAS_ROLE(term->id, PAIR)) {
        pred = ECS_PAIR_RELATION(term->id);
        obj = ECS_PAIR_OBJECT(term->id);

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

    ecs_entity_t term_obj = entity_from_identifier(&term->args[1]);
    if (term_obj) {
        if (term_obj != obj) {
            term_error(world, term, name, 
                "mismatch between term.id and term.obj");
            return -1;
        }
    } else {
        term->args[1].entity = obj;
        if (finalize_term_identifier(world, term, &term->args[1], name)) {
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
    ecs_entity_t obj = entity_from_identifier(&term->args[1]);
    ecs_id_t role = term->role;
    ecs_id_t id = term->id;

    if (obj && (!role || (role != ECS_PAIR))) {
        term_error(world, term, name, 
            "invalid role for term with pair (expected ECS_PAIR)");
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

    if (obj && !(ECS_HAS_ROLE(id, PAIR))) {
        term_error(world, term, name, "term has object but id is not a pair");
        return -1;
    }

    if (ECS_HAS_ROLE(id, PAIR)) {
        if (id != ecs_pair(pred, obj)) {
            char *id_str = ecs_id_str(world, ecs_pair(pred, obj));
            term_error(world, term, name, 
                "term id does not match pred/obj (%s)", id_str);
            ecs_os_free(id_str);
            return -1;
        }
    } else if (pred != (id & ECS_COMPONENT_MASK)) {
        char *pred_str = ecs_get_fullpath(world, pred);
        term_error(world, term, name, "term id does not match pred '%s'",
            pred_str);
        ecs_os_free(pred_str);
        return -1;
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
    if (id[0] == '_') {
        return &id[1];
    }

    /* Identifiers that have a single uppercase character are variables */
    if (ecs_os_strlen(id) == 1 && isupper(id[0])) {
        return id;
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

        ecs_entity_t id_rel = ECS_PAIR_RELATION(id);
        ecs_entity_t id_obj = ECS_PAIR_OBJECT(id);
        ecs_entity_t pattern_rel = ECS_PAIR_RELATION(pattern);
        ecs_entity_t pattern_obj = ECS_PAIR_OBJECT(pattern);

        ecs_assert(id_rel != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(id_obj != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_assert(pattern_rel != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(pattern_obj != 0, ECS_INVALID_PARAMETER, NULL);
        
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
    if (id == EcsWildcard) {
        return true;
    } else if (ECS_HAS_ROLE(id, PAIR)) {
        return ECS_PAIR_RELATION(id) == EcsWildcard || 
               ECS_PAIR_OBJECT(id) == EcsWildcard;
    }

    return false;
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

    if (term->args[0].entity != EcsThis) {
        return false;
    }

    if (term->args[0].set.mask && (term->args[0].set.mask != EcsSelf)) {
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
    if (finalize_term_identifiers(world, term, name)) {
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
    dst.args[0].name = ecs_os_strdup(src->args[0].name);
    dst.args[1].name = ecs_os_strdup(src->args[1].name);
    return dst;
}

ecs_term_t ecs_term_move(
    ecs_term_t *src)
{
    if (src->move) {
        ecs_term_t dst = *src;
        src->name = NULL;
        src->pred.name = NULL;
        src->args[0].name = NULL;
        src->args[1].name = NULL;
        return dst;
    } else {
        return ecs_term_copy(src);
    }
}

void ecs_term_fini(
    ecs_term_t *term)
{
    ecs_os_free(term->pred.name);
    ecs_os_free(term->args[0].name);
    ecs_os_free(term->args[1].name);
    ecs_os_free(term->name);

    term->pred.name = NULL;
    term->args[0].name = NULL;
    term->args[1].name = NULL;
    term->name = NULL;
}

int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *f)
{
    int32_t i, term_count = f->term_count, actual_count = 0;
    ecs_term_t *terms = f->terms;
    bool is_or = false, prev_or = false;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (ecs_term_finalize(world, f->name, term)) {
            return -1;
        }

        is_or = term->oper == EcsOr;
        actual_count += !(is_or && prev_or);
        term->index = actual_count - 1;
        prev_or = is_or;

        if (term->args[0].entity == EcsThis) {
            f->match_this = true;
            if (term->args[0].set.mask != EcsSelf) {
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
    }

    f->term_count_actual = actual_count;

    return 0;
}

int ecs_filter_init(
    const ecs_world_t *stage,
    ecs_filter_t *filter_out,
    const ecs_filter_desc_t *desc)    
{
    ecs_assert(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(filter_out != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    int i, term_count = 0;
    ecs_term_t *terms = desc->terms_buffer;
    const char *name = desc->name;
    const char *expr = desc->expr;

    ecs_filter_t f = {
        /* Temporarily set the fields to the values provided in desc, until the
         * filter has been validated. */
        .name = (char*)name,
        .expr = (char*)expr
    };

    if (terms) {
        terms = desc->terms_buffer;
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

    /* If default substitution is enabled, replace DefaultSet with SuperSet */
    if (desc->substitute_default) {
        finalize_default_substitution(world, terms, term_count);
    } else {
        for (i = 0; i < term_count; i ++) {
            if (terms[i].args[0].set.mask == EcsDefaultSet) {
                terms[i].args[0].set.mask = EcsSelf;
            }            
        }
    }

    /* Ensure all fields are consistent and properly filled out */
    if (ecs_filter_finalize(world, &f)) {
        goto error;
    }

    *filter_out = f;

    /* Copy term resources. */
    if (term_count) {
        if (!filter_out->expr) {
            if (term_count < ECS_TERM_CACHE_SIZE) {
                filter_out->terms = filter_out->term_cache;
                filter_out->term_cache_used = true;
            } else {
                filter_out->terms = ecs_os_malloc_n(ecs_term_t, term_count);
            }
        }

        for (i = 0; i < term_count; i ++) {
            filter_out->terms[i] = ecs_term_move(&terms[i]);
        }        
    } else {
        filter_out->terms = NULL;
    }

    filter_out->name = ecs_os_strdup(desc->name);
    filter_out->expr = ecs_os_strdup(desc->expr);

    ecs_assert(!filter_out->term_cache_used || 
        filter_out->terms == filter_out->term_cache,
        ECS_INTERNAL_ERROR, NULL);

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

        src->terms = NULL;
        src->term_count = 0;
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
}

static
void filter_str_add_id(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    const ecs_term_id_t *id,
    bool is_subject)
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

        if (id->set.mask != EcsSelf) {
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
    const ecs_term_id_t *subj = &term->args[0];
    const ecs_term_id_t *obj = &term->args[1];

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
        filter_str_add_id(world, buf, &term->pred, false);
        ecs_strbuf_appendstr(buf, "()");
    } else if (subj_set && subj->entity == EcsThis && subj->set.mask == EcsSelf)
    {
        if (term->id) {
            char *str = ecs_id_str(world, term->id);
            ecs_strbuf_appendstr(buf, str);
            ecs_os_free(str);
        } else if (pred_set) {
            filter_str_add_id(world, buf, &term->pred, false);   
        }
    } else {
        filter_str_add_id(world, buf, &term->pred, false);
        ecs_strbuf_appendstr(buf, "(");
        filter_str_add_id(world, buf, &term->args[0], true);
        if (obj_set) {
            ecs_strbuf_appendstr(buf, ",");
            filter_str_add_id(world, buf, &term->args[1], false);
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

    ecs_assert(!filter->term_cache_used || filter->terms == filter->term_cache,
        ECS_INTERNAL_ERROR, NULL);

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
}

static
bool populate_from_column(
    ecs_world_t *world,
    const ecs_table_t *table,
    int32_t offset,
    ecs_id_t id,
    int32_t column,
    ecs_entity_t source,
    ecs_id_t *id_out,
    ecs_entity_t *subject_out,
    ecs_size_t *size_out,
    void **ptr_out)
{
    bool has_data = false;

    ecs_size_t size = 0;

    if (column != -1) {
        /* If source is not This, find table of source */
        if (source) {
            table = ecs_get_table(world, source);
            ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
            column = tr->column;
        }

        const ecs_data_t *data = &table->storage;
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);

        /* If there is no data, ensure that iterator won't try to get it */
        if (table->column_count > column) {
            ecs_column_t *c = &data->columns[column];
            if (c->size) {
                has_data = true;
                size = c->size;
            }
        }

        id = ids[column];

        if (subject_out) {
            *subject_out = source;
        }

        if (ptr_out) {
            if (has_data) {
                if (source) {
                    *ptr_out = (void*)ecs_get_id(world, source, id);
                } else {
                    ecs_column_t *col = &data->columns[column];
                    *ptr_out = ecs_vector_first_t(
                        col->data, col->size, col->alignment);
                    
                    if (*ptr_out && offset) {
                        *ptr_out = ECS_OFFSET(*ptr_out, col->size * offset);
                    }
                }
            } else {
                *ptr_out = NULL;
            }
        }
    }

    if (id_out) {
        *id_out = id;
    }

    if (size_out) {
        *size_out = size;
    }

    return has_data;
}

static
void populate_from_table(
    ecs_iter_t *it,
    ecs_table_t *table)
{
    it->table = table;
    it->type = table->type;
    it->count = ecs_table_count(table);

    const ecs_data_t *data = &table->storage;
    it->entities = ecs_vector_first(data->entities, ecs_entity_t);
}

bool flecs_term_match_table(
    ecs_world_t *world,
    const ecs_term_t *term,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t *id_out,
    int32_t *column_out,
    ecs_entity_t *subject_out,
    ecs_size_t *size_out,
    void **ptr_out,
    int32_t *match_index_out,
    bool first)
{
    const ecs_term_id_t *subj = &term->args[0];
    ecs_oper_kind_t oper = term->oper;
    const ecs_table_t *match_table = table;
    ecs_type_t match_type = type;

    ecs_entity_t subj_entity = subj->entity;
    if (!subj_entity) {
        id_out[0] = term->id;
        return true;
    }

    if (subj_entity != EcsThis) {
        match_table = ecs_get_table(world, subj_entity);
        if (match_table) {
            match_type = match_table->type;
        } else {
            match_type = NULL;
        }
    }

    ecs_entity_t source;

    int32_t column = 0;
    if (!first && column_out && column_out[0] != 0) {
        column = column_out[0] - 1;
    }

    column = ecs_type_match(world, match_table, match_type,
        column, term->id, subj->set.relation, subj->set.min_depth, 
        subj->set.max_depth, &source, match_index_out);

    bool result = column != -1;

    if (oper == EcsNot) {
        result = !result;
    }

    if (oper == EcsOptional) {
        result = true;
    }

    if (subj_entity != EcsThis) {
        if (!source) {
            source = subj_entity;
        }
    }

    if (column_out && result) {
        ecs_assert(id_out != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(subject_out != NULL, ECS_INTERNAL_ERROR, NULL);

        populate_from_column(world, table, offset, term->id, column, 
            source, id_out, subject_out, size_out, ptr_out);

        if (column != -1) {
            column_out[0] = column + 1;
        } else {
            column_out[0] = 0;
        }
    }

    return result;
}

bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_entity_t *subjects,
    ecs_size_t *sizes,
    void **ptrs,
    int32_t *match_indices,
    int32_t *matches_left,
    bool first,
    int32_t skip_term)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(filter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_assert(!filter->term_cache_used || filter->terms == filter->term_cache,
        ECS_INTERNAL_ERROR, NULL);

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    bool is_or = false;
    bool or_result = false;
    int32_t match_count = 0;

    for (i = 0; i < count; i ++) {
        if (i == skip_term) {
            continue;
        }

        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];
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
        }

        bool result = flecs_term_match_table(world, term, match_table, 
            match_type, offset,
            ids ? &ids[t_i] : NULL, 
            columns ? &columns[t_i] : NULL, 
            subjects ? &subjects[t_i] : NULL, 
            sizes ? &sizes[t_i] : NULL,
            ptrs ? &ptrs[t_i] : NULL,
            match_indices ? &match_indices[t_i] : NULL,
            first);

        if (is_or) {
            or_result |= result;
        } else if (!result) {
            return false;
        }

        /* Match indices is populated with the number of matches for this term.
         * This is used to determine whether to keep iterating this table. */
        if (first && match_indices && match_indices[t_i]) {
            match_indices[t_i] --;
            match_count += match_indices[t_i];
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
    const ecs_term_id_t *subj = &term->args[0];

    iter->term = *term;

    if (subj->set.mask == EcsDefaultSet || subj->set.mask & EcsSelf) {
        iter->self_index = flecs_get_id_record(world, term->id);
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
    ecs_assert(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term->id != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    if (ecs_term_finalize(world, NULL, term)) {
        /* Invalid term */
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .terms = term,
        .term_count = 1,
        .next = ecs_term_next
    };

    term_iter_init(world, term, &it.iter.term);

    return it;
}

static
const ecs_table_record_t *next_table(
    ecs_term_iter_t *iter)
{
    const ecs_table_record_t *tables = flecs_id_record_tables(iter->cur);
    int32_t count = flecs_id_record_count(iter->cur);
    if (iter->index >= count) {
        return NULL;
    }

    return &tables[iter->index ++];
}

static
ecs_table_record_t term_iter_next(
    ecs_world_t *world,
    ecs_term_iter_t *iter,
    ecs_entity_t *source_out,
    bool match_prefab,
    bool match_disabled)
{
    ecs_table_t *table = iter->table;
    ecs_entity_t source = 0;
    const ecs_table_record_t *tr;
    ecs_table_record_t result = { .table = NULL };
    ecs_term_t *term = &iter->term;

    do {
        if (table) {
            iter->cur_match ++;
            if (iter->cur_match >= iter->match_count) {
                table = NULL;
            } else {
                result.table = table;
                result.count = iter->match_count;
                result.column = iter->last_column = ecs_type_index_of(
                    table->type, iter->last_column + 1, term->id);
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
                    return result;
                }
            }

            table = tr->table;

            if (!match_prefab && (table->flags & EcsTableIsPrefab)) {
                continue;
            }

            if (!match_disabled && (table->flags & EcsTableIsDisabled)) {
                continue;
            }

            if (!ecs_table_count(table)) {
                continue;
            }

            iter->table = table;
            iter->match_count = tr->count;
            iter->cur_match = 0;
            iter->last_column = tr->column;

            result = *tr;
        }

        if (iter->cur == iter->set_index) {
            const ecs_term_id_t *subj = &term->args[0];

            if (iter->self_index) {
                if (flecs_id_record_table(iter->self_index, table) != NULL) {
                    /* If the table has the id itself and this term matched Self
                     * we already matched it */
                    continue;
                }
            }

            /* Test if following the relation finds the id */
            int32_t index = ecs_type_match(world, table, table->type, 0, 
                term->id, subj->set.relation, subj->set.min_depth, 
                subj->set.max_depth, &source, NULL);
            if (index == -1) {
                continue;
            }

            ecs_assert(source != 0, ECS_INTERNAL_ERROR, NULL);
        }

        break;
    } while (true);

    if (source_out) {
        *source_out = source;
    }

    return result;
}

bool ecs_term_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_term_next, ECS_INVALID_PARAMETER, NULL);

    ecs_term_iter_t *iter = &it->iter.term;
    ecs_term_t *term = &iter->term;
    ecs_world_t *world = it->real_world;

    ecs_entity_t source;
    ecs_table_record_t tr = term_iter_next(world, iter, &source, false, false);
    ecs_table_t *table = tr.table;
    if (!table) {
        it->is_valid = false;
        return false;
    }

    /* Source must either be 0 (EcsThis) or nonzero in case of substitution */
    ecs_assert(source || iter->cur != iter->set_index, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    it->table = table;
    it->type = table->type;
    it->ids = &iter->id;
    it->columns = &iter->column;
    it->subjects = &iter->subject;
    it->sizes = &iter->size;
    it->ptrs = &iter->ptr;

    it->count = ecs_table_count(table);
    it->entities = ecs_vector_first(table->storage.entities, ecs_entity_t);
    it->is_valid = true;

    bool has_data = populate_from_column(world, table, 0, term->id, tr.column, 
        source, &iter->id, &iter->subject, &iter->size, 
        &iter->ptr);

    if (!source) {
        if (has_data) {
            iter->column = tr.column + 1;
        } else {
            iter->column = 0;
        }
    } else {
        iter->column = -1; /* Point to ref */
    }

    return true;
}

static
const ecs_filter_t* init_filter_iter(
    const ecs_world_t *world,
    ecs_iter_t *it,
    const ecs_filter_t *filter)
{
    ecs_filter_iter_t *iter = &it->iter.filter;

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

ecs_iter_t ecs_filter_iter(
    const ecs_world_t *stage,
    const ecs_filter_t *filter)
{
    ecs_assert(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .terms = filter ? filter->terms : NULL,
        .next = ecs_filter_next
    };

    ecs_filter_iter_t *iter = &it.iter.filter;

    filter = init_filter_iter(world, &it, filter);

    int32_t i, term_count = filter->term_count;
    ecs_term_t *terms = filter->terms;
    int32_t min_count = -1;
    int32_t min_term_index = -1;

    /* Find term that represents smallest superset */
    if (filter->match_this) {
        iter->kind = EcsFilterIterEvalIndex;

        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &terms[i];

            ecs_assert(term != NULL, ECS_INTERNAL_ERROR, NULL);

            if (term->oper != EcsAnd) {
                continue;
            }

            if (term->args[0].entity != EcsThis) {
                continue;
            }

            ecs_id_record_t *idr = flecs_get_id_record(world, term->id);
            if (!idr) {
                /* If one of the terms does not match with any data, iterator 
                 * should not return anything */
                term_iter_init_no_data(&iter->term_iter);
                return it;
            }

            int32_t table_count = flecs_id_record_count(idr);
            if (min_count == -1 || table_count < min_count) {
                min_count = table_count;
                min_term_index = i;
            }
        }

        iter->min_term_index = min_term_index;

        if (min_term_index == -1) {
            term_iter_init_wildcard(world, &iter->term_iter);
        } else {
            term_iter_init(world, &terms[min_term_index], &iter->term_iter);
        }
    } else {
        /* If filter has no this terms, no tables need to be evaluated */
        iter->kind = EcsFilterIterEvalNone;
    }

    if (filter->terms == filter->term_cache) {
        /* Because we're returning the iterator by value, the address of the
         * term cache changes. The ecs_filter_next function will set the correct
         * address when it detects that terms is set to NULL */
        iter->filter.terms = NULL;
    }

    return it;
}

ecs_iter_t ecs_filter_chain_iter(
    ecs_iter_t *chain_it,
    const ecs_filter_t *filter)
{
    ecs_iter_t it = {
        .chain_it = chain_it,
        .next = ecs_filter_next,
        .world = chain_it->world,
        .real_world = chain_it->real_world
    };

    ecs_filter_iter_t *iter = &it.iter.filter;
    init_filter_iter(it.world, &it, filter);

    iter->kind = EcsFilterIterEvalChain;

    if (filter->terms == filter->term_cache) {
        /* See ecs_filter_iter*/
        iter->filter.terms = NULL;
    }

    return it;
}

bool ecs_filter_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->next == ecs_filter_next, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->chain_it != it, ECS_INVALID_PARAMETER, NULL);

    ecs_filter_iter_t *iter = &it->iter.filter;
    ecs_filter_t *filter = &iter->filter;
    ecs_world_t *world = it->real_world;
    ecs_table_t *table;
    bool match;
    int i;

    if (!filter->terms) {
        filter->terms = filter->term_cache;
    }

    flecs_iter_init(it);

    if (iter->kind == EcsFilterIterEvalIndex) {
        ecs_term_iter_t *term_iter = &iter->term_iter;
        ecs_term_t *term = &term_iter->term;
        int32_t term_index = term->index;

        do {
            ecs_assert(iter->matches_left >= 0, ECS_INTERNAL_ERROR, NULL);
            bool first_match = iter->matches_left == 0;

            if (first_match) {
                /* Find new match, starting with the leading term */
                ecs_entity_t source;
                ecs_table_record_t tr = term_iter_next(world, term_iter, 
                    &source, filter->match_prefab, filter->match_disabled);
                table = tr.table;
                if (!table) {
                    goto done;
                }

                /* Populate term data as flecs_filter_match_table skips it */
                populate_from_column(world, table, 0, term->id, tr.column, source, 
                    &it->ids[term_index], 
                    &it->subjects[term_index],
                    &it->sizes[term_index], 
                    &it->ptrs[term_index]);

                it->columns[term_index] = tr.column + 1;
            } else {
                /* Progress iterator to next match for table, if any */
                table = it->table;
                first_match = false;

                for (i = filter->term_count_actual - 1; i >= 0; i --) {
                    if (it->match_indices[i] > 0) {
                        it->match_indices[i] --;
                        it->columns[i] ++;
                        break;
                    }
                }
            }

            /* Match the remainder of the terms */
            match = flecs_filter_match_table(world, filter, table, table->type,
                0, it->ids, it->columns, it->subjects, it->sizes, 
                it->ptrs, it->match_indices, &iter->matches_left, 
                first_match, term->index);

            /* Check if there are any terms which have more matching columns */
            if (!first_match) {
                iter->matches_left = 0;
                for (i = 0; i < filter->term_count_actual; i ++) {
                    if (it->match_indices[i] > 0) {
                        iter->matches_left += it->match_indices[i];
                    }
                }
            }
        } while (!match);

        populate_from_table(it, table);
        goto yield;

    } else if (iter->kind == EcsFilterIterEvalChain) {
        ecs_iter_t *chain_it = it->chain_it;
        ecs_iter_next_action_t next = chain_it->next;

        /* If this is a chain iterator, the input iterator must be set */
        ecs_assert(chain_it != NULL, ECS_INVALID_PARAMETER, NULL);

        do {
            if (!next(chain_it)) {
                goto done;
            }

            table = chain_it->table;

            match = flecs_filter_match_table(world, filter, table, table->type,
                0, it->ids, it->columns, it->subjects, it->sizes, 
                it->ptrs, it->match_indices, NULL, true, -1);
        } while (!match);

        populate_from_table(it, table);
        goto yield;
    }

done:
    flecs_iter_fini(it);
    return false;

yield:
    it->is_valid = true;
    return true;    
}
