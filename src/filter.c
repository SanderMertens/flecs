
#include "private_api.h"

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
            ecs_entity_t e = ecs_lookup_symbol(world, identifier->name, true);
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

    if (term->args[1].entity || term->role == ECS_PAIR) {
        /* Both the relation and object must be set */
        ecs_assert(term->pred.entity != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(term->args[1].entity != 0, ECS_INVALID_PARAMETER, NULL);
        term->id = ecs_pair(term->pred.entity, term->args[1].entity);
    } else {
        term->id = term->pred.entity;
    }

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

        /* If other fields are set, make sure they are consistent with id */
        if (term->args[1].entity) {
            ecs_assert(term->pred.entity != 0, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(term->id == 
                ecs_pair(term->pred.entity, term->args[1].entity), 
                    ECS_INVALID_PARAMETER, NULL);
        } else if (term->pred.entity) {
            /* If only predicate is set (not object) it must match the id
             * without any roles set. */
            ecs_assert(term->pred.entity == (term->id & ECS_COMPONENT_MASK), 
                ECS_INVALID_PARAMETER, NULL);
        }

        /* If id is set, check for pair and derive predicate and object */
        if (ECS_HAS_ROLE(term->id, PAIR)) {
            term->pred.entity = ECS_PAIR_RELATION(term->id);
            term->args[1].entity = ECS_PAIR_OBJECT(term->id);
        } else {
            term->pred.entity = term->id & ECS_COMPONENT_MASK;
        }

        if (!term->role) {
            term->role = term->id & ECS_ROLE_MASK;
        } else {
            /* If id already has a role set it should be equal to the provided
             * role */
            ecs_assert(!(term->id & ECS_ROLE_MASK) || 
                        (term->id & ECS_ROLE_MASK) == term->role, 
                            ECS_INVALID_PARAMETER, NULL);
        }      
    } else {
        if (term_resolve_ids(world, name, expr, term)) {
            /* One or more identifiers could not be resolved */
            return -1;
        }
    }

    /* role field should only set role bits */
    ecs_assert(term->role == (term->role & ECS_ROLE_MASK), 
        ECS_INVALID_PARAMETER, NULL);    

    term->id |= term->role;

    if (!term->args[0].entity && 
        term->args[0].set.mask != EcsNothing && 
        term->args[0].var != EcsVarIsVariable) 
    {
        term->args[0].entity = EcsThis;
    }

    if (term->args[0].set.mask & (EcsSuperSet | EcsSubSet)) {
        if (!term->args[0].set.relation) {
            term->args[0].set.relation = EcsIsA;
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

int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *f)
{
    int32_t i, term_count = f->term_count, actual_count = 0;
    ecs_term_t *terms = f->terms;
    bool is_or = false, prev_or = false;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (ecs_term_finalize(world, f->name, f->expr, term)) {
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

        if (term->role && term->role != ECS_PAIR) {
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

        if (term->args[0].entity == EcsThis && 
            ecs_term_id_is_set(&term->args[1])) 
        {
            ecs_strbuf_appendstr(&buf, "(");
        }

        if (!ecs_term_id_is_set(&term->args[1]) && 
            (term->pred.entity != term->args[0].entity)) 
        {
            filter_str_add_id(world, &buf, &term->pred);

            if (!ecs_term_id_is_set(&term->args[0])) {
                ecs_strbuf_appendstr(&buf, "()");
            } else if (term->args[0].entity != EcsThis) {
                ecs_strbuf_appendstr(&buf, "(");
                filter_str_add_id(world, &buf, &term->args[0]);
            }

            if (ecs_term_id_is_set(&term->args[1])) {
                ecs_strbuf_appendstr(&buf, ", ");
                filter_str_add_id(world, &buf, &term->args[1]);
                ecs_strbuf_appendstr(&buf, ")");
            }
        } else if (!ecs_term_id_is_set(&term->args[1])) {
            ecs_strbuf_appendstr(&buf, "$");
            filter_str_add_id(world, &buf, &term->pred);
        } else if (ecs_term_id_is_set(&term->args[1])) {
            filter_str_add_id(world, &buf, &term->pred);
            ecs_strbuf_appendstr(&buf, ", ");
            filter_str_add_id(world, &buf, &term->args[1]);
            ecs_strbuf_appendstr(&buf, ")");
        }
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
    ecs_type_t *type_out,
    ecs_entity_t *subject_out,
    ecs_size_t *size_out,
    void **ptr_out)
{
    bool has_data = false;

    if (column != -1) {
        /* If source is not This, find table of source */
        if (source) {
            table = ecs_get_table(world, source);
            ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
            column = tr->column;
        }

        ecs_data_t *data = flecs_table_get_data(table);
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);

        /* If there is no data, ensure that iterator won't try to get it */
        if (table->column_count > column) {
            ecs_column_t *c = &data->columns[column];
            if (c->size) {
                has_data = true;
                *size_out = c->size;
            }
        }

        if (!has_data) {
            *size_out = 0;
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

    *type_out = NULL;
    *id_out = id;

    return has_data;
}

static
void populate_from_table(
    ecs_iter_t *it,
    ecs_table_t *table)
{
    it->table = table;
    it->count = ecs_table_count(table);

    const ecs_data_t *data = flecs_table_get_data(table);
    it->data = (ecs_data_t*)data;

    if (data) {
        it->table_columns = data->columns;
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
    } else {
        it->table_columns = NULL;
        it->entities = NULL;
    }
}

bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_type_t *types,
    ecs_entity_t *subjects,
    ecs_size_t *sizes,
    void **ptrs)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(filter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_assert(!filter->term_cache_used || filter->terms == filter->term_cache,
        ECS_INTERNAL_ERROR, NULL);

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    bool is_or = false;
    bool or_result = false;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];
        ecs_oper_kind_t oper = term->oper;
        const ecs_table_t *match_table = table;
        ecs_type_t match_type = type;

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

        ecs_entity_t source;

        int32_t column = ecs_type_match(world, match_table, match_type,
            0, term->id, subj->set.relation, subj->set.min_depth, 
            subj->set.max_depth, &source);
        bool result = column != -1;

        if (oper == EcsNot) {
            result = !result;
        }

        if (oper == EcsOptional) {
            result = true;
        }

        if (is_or) {
            or_result |= result;
        } else if (!result) {
            return false;
        }

        if (subj_entity != EcsThis) {
            if (!source) {
                source = subj_entity;
            }
        }

        if (columns && result) {
            ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(types != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(subjects != NULL, ECS_INTERNAL_ERROR, NULL);

            int32_t t_i = term->index;

            void **ptr = ptrs ? &ptrs[t_i] : NULL;
            populate_from_column(world, table, offset, term->id, column, 
                source, &ids[t_i], &types[t_i], &subjects[t_i], &sizes[t_i], 
                ptr);

            if (column != -1) {
                columns[t_i] = column + 1;
            } else {
                columns[t_i] = 0;
            }
        }
    }

    return !is_or || or_result;
}

static
void term_iter_init_no_data(
    ecs_term_iter_t *iter)
{
    iter->term = NULL;
    iter->self_index = NULL;
    iter->iter = ecs_map_iter(NULL);
}

static
void term_iter_init_wildcard(
    const ecs_world_t *world,
    ecs_term_iter_t *iter)
{
    iter->term = NULL;
    iter->self_index = flecs_get_id_record(world, EcsWildcard);

    if (iter->self_index) {
        iter->iter = ecs_map_iter(iter->self_index->table_index);
    }
}

static
void term_iter_init(
    const ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_iter_t *iter)
{    
    const ecs_term_id_t *subj = &term->args[0];

    iter->term = term;

    if (subj->set.mask == EcsDefaultSet || subj->set.mask & EcsSelf) {
        iter->self_index = flecs_get_id_record(world, term->id);
    }

    if (subj->set.mask & EcsSuperSet) {
        iter->set_index = flecs_get_id_record(world, 
            ecs_pair(subj->set.relation, EcsWildcard));
    }

    if (iter->self_index) {
        iter->iter = ecs_map_iter(iter->self_index->table_index);
    } else if (iter->set_index) {
        iter->iter = ecs_map_iter(iter->set_index->table_index);
        iter->iter_set = true;
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

    if (ecs_term_finalize(world, NULL, NULL, term)) {
        /* Invalid term */
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage,
        .column_count = 1
    };

    term_iter_init(world, term, &it.iter.term);

    return it;
}

static
ecs_table_record_t* term_iter_next(
    ecs_world_t *world,
    ecs_term_iter_t *iter,
    ecs_entity_t *source_out)
{
    ecs_table_t *table = NULL;
    ecs_entity_t source = 0;
    ecs_table_record_t *tr;

    do {
        tr = ecs_map_next(&iter->iter, ecs_table_record_t, NULL);
        if (!tr) {
            if (!iter->iter_set) {
                if (iter->set_index) {
                    iter->iter = ecs_map_iter(iter->set_index->table_index);
                    tr = ecs_map_next(&iter->iter, ecs_table_record_t, NULL);
                    iter->iter_set = true;
                }
            }

            if (!tr) {
                return NULL;
            }
        }

        table = tr->table;

        if (!ecs_table_count(table)) {
            continue;
        }

        if (iter->iter_set) {
            const ecs_term_t *term = iter->term;
            const ecs_term_id_t *subj = &term->args[0];

            if (iter->self_index) {
                if (ecs_map_has(iter->self_index->table_index, table->id)) {
                    /* If the table has the id itself and this term matched Self
                     * we already matched it */
                    continue;
                }
            }

            /* Test if following the relation finds the id */
            int32_t index = ecs_type_match(world, table, table->type, 0, 
                term->id, subj->set.relation, subj->set.min_depth, 
                subj->set.max_depth, &source);
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

    return tr;
}

bool ecs_term_next(
    ecs_iter_t *it)
{
    ecs_term_iter_t *iter = &it->iter.term;
    ecs_term_t *term = iter->term;
    ecs_world_t *world = it->real_world;

    ecs_entity_t source;
    ecs_table_record_t *tr = term_iter_next(world, iter, &source);
    if (!tr) {
        it->is_valid = false;
        return false;
    }

    ecs_table_t *table = tr->table;

    /* Source must either be 0 (EcsThis) or nonzero in case of substitution */
    ecs_assert(source || !iter->iter_set, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = flecs_table_get_data(table);

    it->table = table;
    it->data = data;
    it->ids = &iter->id;
    it->types = &iter->type;
    it->columns = &iter->column;
    it->subjects = &iter->subject;
    it->sizes = &iter->size;
    it->ptrs = &iter->ptr;

    it->table_columns = data->columns;
    it->count = ecs_table_count(table);
    it->entities = ecs_vector_first(data->entities, ecs_entity_t);
    it->is_valid = true;

    bool has_data = populate_from_column(world, table, 0, term->id, tr->column, 
        source, &iter->id, &iter->type, &iter->subject, &iter->size, 
        &iter->ptr);

    if (!source) {
        if (has_data) {
            iter->column = tr->column + 1;
        } else {
            iter->column = 0;
        }
    } else {
        iter->column = -1; /* Point to ref */
    }

    return true;
}

ecs_iter_t ecs_filter_iter(
    const ecs_world_t *stage,
    const ecs_filter_t *filter)
{
    ecs_assert(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    ecs_iter_t it = {
        .real_world = (ecs_world_t*)world,
        .world = (ecs_world_t*)stage
    };

    ecs_filter_iter_t *iter = &it.iter.filter;
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

            int32_t table_count = ecs_map_count(idr->table_index);
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

    it.column_count = filter->term_count_actual;

    if (filter->terms == filter->term_cache) {
        /* Because we're returning the iterator by value, the address of the
         * term cache changes. The ecs_filter_next function will set the correct
         * address when it detects that terms is set to NULL */
        iter->filter.terms = NULL;
    }

    return it;
}

bool ecs_filter_next(
    ecs_iter_t *it)
{
    ecs_filter_iter_t *iter = &it->iter.filter;
    ecs_filter_t *filter = &iter->filter;
    ecs_world_t *world = it->real_world;

    if (!filter->terms) {
        filter->terms = filter->term_cache;
    }

    ecs_iter_init(it);

    if (iter->kind == EcsFilterIterEvalIndex) {
        ecs_term_iter_t *term_iter = &iter->term_iter;
        ecs_table_t *table;
        bool match;

        do {
            ecs_entity_t source;
            ecs_table_record_t *tr = term_iter_next(world, term_iter, &source);
            if (!tr) {
                goto done;
            }

            table = tr->table;
            match = flecs_filter_match_table(world, filter, table, table->type,
                0, it->ids, it->columns, it->types, it->subjects, it->sizes, 
                it->ptrs);
        } while (!match);

        populate_from_table(it, table);

        goto yield;
    }

done:
    ecs_iter_fini(it);
    return false;

yield:
    it->is_valid = true;
    return true;    
}
