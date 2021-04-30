
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

    if (identifier->var_kind == EcsVarDefault) {
        if (ecs_identifier_is_var(identifier->name)) {
            identifier->var_kind = EcsVarIsVariable;
        }
    }

    if (identifier->var_kind != EcsVarIsVariable) {
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

    if (term->args[1].entity) {
        term->id = ecs_pair(term->pred.entity, term->args[1].entity);
    } else {
        term->id = term->pred.entity;
    }

    term->id |= term->role;

    return 0;
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

    if (term->args[0].set != EcsDefaultSet) {
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
    /* If id is set, derive predicate and object */
    if (term->id) {
        if (term->args[0].entity && term->args[0].entity != EcsThis) {
            ecs_parser_error(name, expr, 0, 
                "cannot combine id with subject that is not This");
            return -1;
        }

        if (ECS_HAS_ROLE(term->id, PAIR)) {
            term->pred.entity = ECS_PAIR_RELATION(term->id);
            term->args[1].entity = ECS_PAIR_OBJECT(term->id);
        } else {
            term->pred.entity = term->id & ECS_COMPONENT_MASK;
        }

        term->args[0].entity = EcsThis;
        term->role = term->id & ECS_ROLE_MASK;
    } else {
        if (term_resolve_ids(world, name, expr, term)) {
            /* One or more identifiers could not be resolved */
            return -1;
        }
    }

    return 0;
}

void ecs_term_copy(
    ecs_term_t *dst,
    const ecs_term_t *src)
{
    *dst = *src;
    dst->name = ecs_os_strdup(src->name);
    dst->pred.name = ecs_os_strdup(src->pred.name);
    dst->args[0].name = ecs_os_strdup(src->args[0].name);
    dst->args[1].name = ecs_os_strdup(src->args[1].name);
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

    if (expr) {
#ifdef FLECS_PARSER
        /* Cannot set expression and terms at the same time */
        ecs_assert(terms == NULL, ECS_INVALID_PARAMETER, NULL);
        
        /* Parse expression into array of terms */
        int32_t buffer_count = 0;
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
    } else {
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
    }

    /* Ensure all fields are consistent and properly filled out */
    if (ecs_filter_finalize(world, &f)) {
        goto error;
    }

    /* Copy term resources. If an expression was provided, the resources in the
     * terms are already owned by the filter. */
    if (!f.expr) {
        if (term_count) {
            f.terms = ecs_os_malloc(term_count * ECS_SIZEOF(ecs_term_t));
            for (i = 0; i < term_count; i ++) {
                ecs_term_copy(&f.terms[i], &terms[i]);
            }
        } else {
            f.terms = NULL;
        }
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

        if (subj->entity != EcsThis && subj->set & EcsSelf) {
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
