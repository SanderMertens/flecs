
#include "private_api.h"

static
bool term_id_is_set(
    const ecs_term_id_t *id)
{
    return id->entity != 0 || id->name != NULL;
}

static
int resolve_identifier(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_id_t *identifier)
{
    if (!identifier->name) {
        return 0;
    }

    if (identifier->var == EcsVarDefault) {
        if (ecs_identifier_is_var(identifier->name)) {
            identifier->var = EcsVarIsVariable;
        }
    }

    if (identifier->var != EcsVarIsVariable) {
        if (ecs_identifier_is_0(identifier->name)) {
            identifier->entity = 0;
        } else {
            ecs_entity_t e = ecs_lookup_fullpath(world, identifier->name);
            if (!e) {
                ecs_parser_error(name, expr, 0,
                    "unresolved identifier '%s'", identifier->name);
                return -1;
            }

            /* Use OR, as entity may have already been populated with role */
            identifier->entity = e;
        }
    }

    return 0;
}

bool ecs_identifier_is_0(
    const char *id)
{
    return id[0] == '0' && !id[1];
}

bool ecs_identifier_is_var(
    const char *id)
{
    if (id[0] == '_') {
        return true;
    }

    if (isdigit(id[0])) {
        return false;
    }

    const char *ptr;
    char ch;
    for (ptr = id; (ch = *ptr); ptr ++) {
        if (!isupper(ch) && ch != '_' && !isdigit(ch)) {
            return false;
        }
    }

    return true;
}

static
int term_resolve_ids(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term)
{
    if (resolve_identifier(world, name, expr, &term->pred)) {
        return -1;
    }
    if (resolve_identifier(world, name, expr, &term->args[0])) {
        return -1;
    }
    if (resolve_identifier(world, name, expr, &term->args[1])) {
        return -1;
    }

    /* An explicitly set PAIR role indicates legacy behavior. In the legacy
     * query language a wildcard object means that the entity should have the
     * matched object by itself. This is incompatible with the new query parser.
     * For now, this behavior is mapped to a pair with a 0 object, but in the
     * future this behavior will be replaced with variables. */
    if (term->role == ECS_PAIR) {
        if (term->args[1].entity == EcsWildcard) {
            term->args[1].entity = 0;
            if (term->move) {
                ecs_os_free(term->args[1].name);
            }
            term->args[1].name = NULL;
        } else if (!term->args[1].entity) {
            term->args[1].entity = EcsWildcard;
        }
    }

    if (term->args[1].entity || term->role == ECS_PAIR) {
        term->id = ecs_pair(term->pred.entity, term->args[1].entity);
    } else {
        term->id = term->pred.entity;
    }

    term->id |= term->role;

    return 0;
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
            /* legacy roles that are now relations */
            if (!ECS_HAS_ROLE(id, INSTANCEOF) && !ECS_HAS_ROLE(id, CHILDOF)) {
                return false;
            }
        }

        ecs_entity_t id_rel = ECS_PAIR_RELATION(id);
        ecs_entity_t id_obj = ECS_PAIR_OBJECT(id);
        ecs_entity_t pattern_rel = ECS_PAIR_RELATION(pattern);
        ecs_entity_t pattern_obj = ECS_PAIR_OBJECT(pattern);

        if (pattern_rel == EcsWildcard) {
            if (pattern_obj == EcsWildcard || !pattern_obj || pattern_obj == id_obj) {
                return true;
            }
        } else if (!pattern_obj || pattern_obj == EcsWildcard) {
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

bool ecs_term_is_set(
    const ecs_term_t *term)
{
    return term->id != 0 || term_id_is_set(&term->pred);
}

bool ecs_term_is_trivial(
    ecs_term_t *term)
{
    if (term->inout != EcsInOutDefault) {
        return false;
    }

    if (term->args[0].entity != EcsThis) {
        return false;
    }

    if (term->args[0].set.mask != EcsDefaultSet) {
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
    const char *expr,
    ecs_term_t *term)
{
    if (term->id) {
        /* Allow for combining explicit object with id */
        if (term->args[1].name && !term->args[1].entity) {
            if (resolve_identifier(world, name, expr, &term->args[1])) {
                return -1;
            }
        }

        if (term->args[1].entity) {
            term->id = ecs_pair(term->id, term->args[1].entity);
        }

        /* If id is set, check for pair and derive predicate and object */
        if (ECS_HAS_ROLE(term->id, PAIR)) {
            term->pred.entity = ECS_PAIR_RELATION(term->id);
            term->args[1].entity = ECS_PAIR_OBJECT(term->id);
        } else {
            term->pred.entity = term->id & ECS_COMPONENT_MASK;
        }

        if (!term->args[0].entity) {
            term->args[0].entity = EcsThis;
        }

        if (!term->role) {
            term->role = term->id & ECS_ROLE_MASK;
        } else {
            ecs_assert(!(term->id & ECS_ROLE_MASK), 
                ECS_INVALID_PARAMETER, NULL);
            term->id |= term->role;
        }
    } else {
        if (term_resolve_ids(world, name, expr, term)) {
            /* One or more identifiers could not be resolved */
            return -1;
        }
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
}

int ecs_filter_init(
    const ecs_world_t *world,
    ecs_filter_t *filter_out,
    const ecs_filter_desc_t *desc)    
{
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
        for (i = 0; i < ECS_FILTER_DESC_TERM_ARRAY_MAX; i ++) {
            if (!ecs_term_is_set(&terms[i])) {
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
            if (!ecs_term_is_set(&term)) {
                break;
            }
            
            if (term_count == buffer_count) {
                buffer_count = buffer_count ? buffer_count * 2 : 8;
                terms = ecs_os_realloc(terms, 
                    buffer_count * ECS_SIZEOF(ecs_term_t));
            }

            terms[term_count] = term;
            term_count ++;
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
        for (i = 0; i < term_count; i ++) {
            if (terms[i].args[0].set.mask == EcsDefaultSet) {
                terms[i].args[0].set.mask = EcsSuperSet | EcsSelf;
                terms[i].args[0].set.relation = EcsIsA;
            }            
        }
    }

    /* Ensure all fields are consistent and properly filled out */
    if (ecs_filter_finalize(world, &f)) {
        goto error;
    }

    /* Copy term resources. */
    if (term_count) {
        if (!f.expr) {
            f.terms = ecs_os_malloc(term_count * ECS_SIZEOF(ecs_term_t));
        }

        for (i = 0; i < term_count; i ++) {
            f.terms[i] = ecs_term_move(&terms[i]);
        }        
    } else {
        f.terms = NULL;
    }

    f.name = ecs_os_strdup(desc->name);
    f.expr = ecs_os_strdup(desc->expr);

    *filter_out = f;

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

int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *f)
{
    int32_t i, term_count = f->term_count, index = 0;
    ecs_term_t *terms = f->terms;
    ecs_oper_kind_t prev_oper = 0;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (ecs_term_finalize(world, f->name, f->expr, term)) {
            return -1;
        }

        term->index = index;

        /* Fold OR terms */
        if (prev_oper != EcsOr || term->oper != EcsOr) {
            index ++;
        }

        prev_oper = term->oper;
    }

    f->term_count_actual = index;

    return 0;
}

static
void filter_str_add_id(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_term_id_t *id)
{
    if (id->name) {
        ecs_strbuf_appendstr(buf, id->name);
    } else if (id->entity) {
        char *path = ecs_get_fullpath(world, id->entity);
        ecs_strbuf_appendstr(buf, path);
        ecs_os_free(path);
    } else {
        ecs_strbuf_appendstr(buf, "0");
    }
}

char* ecs_filter_str(
    const ecs_world_t *world,
    ecs_filter_t *filter)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

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
            } else if (term->inout == EcsInOut) {
                ecs_strbuf_appendstr(&buf, "[out] ");
            }
        }

        if (term->role) {
            ecs_strbuf_appendstr(&buf, ecs_role_str(term->role));
            ecs_strbuf_appendstr(&buf, " ");
        }

        if (term->oper == EcsOr) {
            or_count ++;
        } else {
            or_count = 0;
        }

        if (term->oper == EcsNot) {
            ecs_strbuf_appendstr(&buf, "!");
        } else if (term->oper == EcsOptional) {
            ecs_strbuf_appendstr(&buf, "?");
        }

        if (term->args[0].entity == EcsThis && term_id_is_set(&term->args[1])) {
            ecs_strbuf_appendstr(&buf, "(");
        }

        if (!term_id_is_set(&term->args[1]) && 
            (term->pred.entity != term->args[0].entity)) 
        {
            filter_str_add_id(world, &buf, &term->pred);

            if (!term_id_is_set(&term->args[0])) {
                ecs_strbuf_appendstr(&buf, "()");
            } else if (term->args[0].entity != EcsThis) {
                ecs_strbuf_appendstr(&buf, "(");
                filter_str_add_id(world, &buf, &term->args[0]);
            }

            if (term_id_is_set(&term->args[1])) {
                ecs_strbuf_appendstr(&buf, ", ");
                filter_str_add_id(world, &buf, &term->args[1]);
                ecs_strbuf_appendstr(&buf, ")");
            }
        } else {
            ecs_strbuf_appendstr(&buf, "$");
            filter_str_add_id(world, &buf, &term->pred);
        }
    }

    return ecs_strbuf_get(&buf);
}

void ecs_filter_fini(
    ecs_filter_t *filter) 
{
    if (filter->terms) {
        int i, count = filter->term_count;
        for (i = 0; i < count; i ++) {
            ecs_term_fini(&filter->terms[i]);
        }

        ecs_os_free(filter->terms);
    }

    ecs_os_free(filter->name);
    ecs_os_free(filter->expr);
}

bool ecs_filter_match_entity(
    const ecs_world_t *world,
    const ecs_filter_t *filter,
    ecs_entity_t e)
{
    ecs_assert(e == 0, ECS_UNSUPPORTED, NULL);
    (void)e;

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];
        ecs_oper_kind_t oper = term->oper;

        if (subj->entity != EcsThis && subj->set.mask & EcsSelf) {
            ecs_type_t type = ecs_get_type(world, subj->entity);
            if (ecs_type_has_id(world, type, term->id)) {
                if (oper == EcsNot) {
                    return false;
                }
            } else {
                if (oper != EcsNot) {
                    return false;
                }
            }
        }        
    }

    return true;    
}

ecs_iter_t ecs_filter_iter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_filter_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = world->store.tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = world,
        .iter.filter = iter
    };
}

bool ecs_filter_next(
    ecs_iter_t *it)
{
    ecs_filter_iter_t *iter = &it->iter.filter;
    ecs_sparse_t *tables = iter->tables;
    int32_t count = ecs_sparse_count(tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_table_get_data(table);

        if (!data) {
            continue;
        }

        if (!ecs_table_match_filter(it->world, table, &iter->filter)) {
            continue;
        }

        iter->table.table = table;
        it->table = &iter->table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = i + 1;

        return true;
    }

    return false;
}
