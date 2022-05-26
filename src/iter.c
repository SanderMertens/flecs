#include "private_api.h"
#include <stddef.h>

/* Utility macro's to enforce consistency when initializing iterator fields */

/* If term count is smaller than cache size, initialize with inline array,
 * otherwise allocate. */
#define INIT_CACHE(it, fields, f, count, cache_size)\
    if (!it->f && (fields & flecs_iter_cache_##f) && count) {\
        if (count <= cache_size) {\
            it->f = it->priv.cache.f;\
            it->priv.cache.used |= flecs_iter_cache_##f;\
        } else {\
            it->f = ecs_os_calloc(ECS_SIZEOF(*(it->f)) * count);\
            it->priv.cache.allocated |= flecs_iter_cache_##f;\
        }\
    }

/* If array is using the cache, make sure that its address is correct in case
 * the iterator got moved (typically happens when returned by a function) */
#define VALIDATE_CACHE(it, f)\
    if (it->f) {\
        if (it->priv.cache.used & flecs_iter_cache_##f) {\
            it->f = it->priv.cache.f;\
        }\
    }

/* If array is allocated, free it when finalizing the iterator */
#define FINI_CACHE(it, f)\
    if (it->f) {\
        if (it->priv.cache.allocated & flecs_iter_cache_##f) {\
            ecs_os_free((void*)it->f);\
        }\
    }

void flecs_iter_init(
    ecs_iter_t *it,
    ecs_flags8_t fields)
{
    ecs_assert(!ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INTERNAL_ERROR, NULL);

    it->priv.cache.used = 0;
    it->priv.cache.allocated = 0;

    INIT_CACHE(it, fields, ids, it->term_count, ECS_TERM_CACHE_SIZE);
    INIT_CACHE(it, fields, subjects, it->term_count, ECS_TERM_CACHE_SIZE);
    INIT_CACHE(it, fields, match_indices, it->term_count, ECS_TERM_CACHE_SIZE);
    INIT_CACHE(it, fields, columns, it->term_count, ECS_TERM_CACHE_SIZE);
    INIT_CACHE(it, fields, variables, it->variable_count, 
        ECS_VARIABLE_CACHE_SIZE);
    INIT_CACHE(it, fields, sizes, it->term_count, ECS_TERM_CACHE_SIZE);

    if (!ECS_BIT_IS_SET(it->flags, EcsIterIsFilter)) {
        INIT_CACHE(it, fields, ptrs, it->term_count, ECS_TERM_CACHE_SIZE);
    } else {
        it->ptrs = NULL;
    }
}

static
void iter_validate_cache(
    ecs_iter_t *it)
{
    /* Make sure pointers to cache are up to date in case iter has moved */
    VALIDATE_CACHE(it, ids);
    VALIDATE_CACHE(it, subjects);
    VALIDATE_CACHE(it, match_indices);
    VALIDATE_CACHE(it, columns);
    VALIDATE_CACHE(it, variables);
    VALIDATE_CACHE(it, sizes);
    VALIDATE_CACHE(it, ptrs);
}

void flecs_iter_validate(
    ecs_iter_t *it)
{
    iter_validate_cache(it);
    ECS_BIT_SET(it->flags, EcsIterIsValid);
}

void ecs_iter_fini(
    ecs_iter_t *it)
{
    ECS_BIT_CLEAR(it->flags, EcsIterIsValid);

    if (it->fini) {
        it->fini(it);
    }

    FINI_CACHE(it, ids);
    FINI_CACHE(it, columns);
    FINI_CACHE(it, subjects);
    FINI_CACHE(it, sizes);
    FINI_CACHE(it, ptrs);
    FINI_CACHE(it, match_indices);
    FINI_CACHE(it, variables);
}

static
ecs_size_t iter_get_size_for_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return 0;
    }

    if (idr->flags & EcsIdUnion) {
        return ECS_SIZEOF(ecs_entity_t);
    }

    if (idr->type_info) {
        return idr->type_info->size;
    }

    return 0;
}

static
bool flecs_iter_populate_term_data(
    ecs_world_t *world,
    ecs_iter_t *it,
    int32_t t,
    int32_t column,
    void **ptr_out,
    ecs_size_t *size_out)
{
    bool is_shared = false;
    ecs_table_t *table;
    ecs_vector_t *vec;
    ecs_size_t size = 0;
    ecs_size_t align;
    int32_t row, u_index;

    if (!column) {
        /* Term has no data. This includes terms that have Not operators. */
        goto no_data;
    }

    if (!it->terms) {
        goto no_data;
    }

    /* Filter terms may match with data but don't return it */
    if (it->terms[t].inout == EcsInOutFilter) {
        if (size_out) {
            size = iter_get_size_for_id(world, it->ids[t]);
        }
        goto no_data;
    }

    if (column < 0) {
        is_shared = true;

        /* Data is not from This */
        if (it->references) {
            /* The reference array is used only for components matched on a
             * table (vs. individual entities). Remaining components should be
             * assigned outside of this function */
            if (it->terms[t].subj.entity == EcsThis) {

                /* Iterator provides cached references for non-This terms */
                ecs_ref_t *ref = &it->references[-column - 1];
                if (ptr_out) {
                    if (ref->id) {
                        ptr_out[0] = (void*)ecs_ref_get_id(world, ref, ref->id);
                    } else {
                        ptr_out[0] = NULL;
                    }
                }

                if (!ref->id) {
                    is_shared = false;
                }

                /* If cached references were provided, the code that populated
                * the iterator also had a chance to cache sizes, so size array
                * should already have been assigned. This saves us from having
                * to do additional lookups to find the component size. */
                ecs_assert(size_out == NULL, ECS_INTERNAL_ERROR, NULL);
                return is_shared;
            }

            return true;
        } else {
            ecs_entity_t subj = it->subjects[t];
            ecs_assert(subj != 0, ECS_INTERNAL_ERROR, NULL);

            /* Don't use ecs_get_id directly. Instead, go directly to the
             * storage so that we can get both the pointer and size */
            ecs_record_t *r = ecs_eis_get(world, subj);
            ecs_assert(r != NULL && r->table != NULL, ECS_INTERNAL_ERROR, NULL);

            row = ECS_RECORD_TO_ROW(r->row);
            table = r->table;

            ecs_id_t id = it->ids[t];
            ecs_table_t *s_table = table->storage_table;
            ecs_table_record_t *tr;

            if (!s_table || !(tr = flecs_get_table_record(world, s_table, id))){
                u_index = flecs_table_column_to_union_index(table, -column - 1);
                if (u_index != -1) {
                    goto has_union;
                }
                goto no_data;
            }

            /* We now have row and column, so we can get the storage for the id
             * which gives us the pointer and size */
            column = tr->column;
            ecs_type_info_t *ti = table->type_info[column];
            ecs_column_t *s = &table->data.columns[column];
            size = ti->size;
            align = ti->alignment;
            vec = s->data;
            /* Fallthrough to has_data */
        }
    } else {
        /* Data is from This, use table from iterator */
        table = it->table;
        if (!table) {
            goto no_data;
        }

        row = it->offset;


        int32_t storage_column = ecs_table_type_to_storage_index(
            table, column - 1);
        if (storage_column == -1) {
            u_index = flecs_table_column_to_union_index(table, column - 1);
            if (u_index != -1) {
                goto has_union;
            }
            goto no_data;
        }

        ecs_type_info_t *ti = table->type_info[storage_column];
        ecs_column_t *s = &table->data.columns[storage_column];
        size = ti->size;
        align = ti->alignment;
        vec = s->data;

        if (!table || !ecs_table_count(table)) {
            goto no_data;
        }

        /* Fallthrough to has_data */
    }

has_data:
    if (ptr_out) ptr_out[0] = ecs_vector_get_t(vec, size, align, row);
    if (size_out) size_out[0] = size;
    return is_shared;

has_union: {
        /* Edge case: if column is a switch we should return the vector with case
         * identifiers. Will be replaced in the future with pluggable storage */
        ecs_switch_t *sw = &table->data.sw_columns[u_index].data;
        vec = flecs_switch_values(sw);
        size = ECS_SIZEOF(ecs_entity_t);
        align = ECS_ALIGNOF(ecs_entity_t);
        goto has_data;
    }

no_data:
    if (ptr_out) ptr_out[0] = NULL;
    if (size_out) size_out[0] = size;
    return false;
}

void flecs_iter_populate_data(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    void **ptrs,
    ecs_size_t *sizes)
{
    if (it->table) {
        it->frame_offset += ecs_table_count(it->table);
    }

    it->table = table;
    it->offset = offset;
    it->count = count;

    if (table) {
        if (!count) {
            count = it->count = ecs_table_count(table);
        }
        if (count) {
            it->entities = ecs_vector_get(
                table->data.entities, ecs_entity_t, offset);
        } else {
            it->entities = NULL;
        }
    }

    int t, term_count = it->term_count;

    if (ECS_BIT_IS_SET(it->flags, EcsIterIsFilter)) {
        ECS_BIT_CLEAR(it->flags, EcsIterHasShared);

        if (!sizes) {
            return;
        }

        /* Fetch sizes, skip fetching data */
        for (t = 0; t < term_count; t ++) {
            sizes[t] = iter_get_size_for_id(world, it->ids[t]);
        }
        return;
    }

    bool has_shared = false;

    if (ptrs && sizes) {
        for (t = 0; t < term_count; t ++) {
            int32_t column = it->columns[t];
            has_shared |= flecs_iter_populate_term_data(world, it, t, column,
                &ptrs[t], 
                &sizes[t]);
        }
    } else {
        for (t = 0; t < term_count; t ++) {
            ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);

            int32_t column = it->columns[t];
            void **ptr = NULL;
            if (ptrs) {
                ptr = &ptrs[t];
            }
            ecs_size_t *size = NULL;
            if (sizes) {
                size = &sizes[t];
            }

            has_shared |= flecs_iter_populate_term_data(world, it, t, column,
                ptr, size);
        }
    }

    ECS_BIT_COND(it->flags, EcsIterHasShared, has_shared);
}

bool flecs_iter_next_row(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INTERNAL_ERROR, NULL);

    bool is_instanced = ECS_BIT_IS_SET(it->flags, EcsIterIsInstanced);
    if (!is_instanced) {
        int32_t instance_count = it->instance_count;
        int32_t count = it->count;
        int32_t offset = it->offset;

        if (instance_count > count && offset < (instance_count - 1)) {
            ecs_assert(count == 1, ECS_INTERNAL_ERROR, NULL);
            int t, term_count = it->term_count;

            for (t = 0; t < term_count; t ++) {
                int32_t column = it->columns[t];
                if (column >= 0) {
                    void *ptr = it->ptrs[t];
                    if (ptr) {
                        it->ptrs[t] = ECS_OFFSET(ptr, it->sizes[t]);
                    }
                }
            }

            if (it->entities) {
                it->entities ++;
            }
            it->offset ++;

            return true;
        }
    }

    return false;
}

bool flecs_iter_next_instanced(
    ecs_iter_t *it,
    bool result)
{
    it->instance_count = it->count;
    bool is_instanced = ECS_BIT_IS_SET(it->flags, EcsIterIsInstanced);
    bool has_shared = ECS_BIT_IS_SET(it->flags, EcsIterHasShared);
    if (result && !is_instanced && it->count && has_shared) {
        it->count = 1;
    }
    return result;
}

/* --- Public API --- */

void* ecs_term_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t term)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!size || ecs_term_size(it, term) == size || 
        (!ecs_term_size(it, term) && (!it->ptrs || !it->ptrs[term - 1])), 
        ECS_INVALID_PARAMETER, NULL);

    (void)size;

    if (!term) {
        return it->entities;
    }

    if (!it->ptrs) {
        return NULL;
    }

    return it->ptrs[term - 1];
error:
    return NULL;
}

bool ecs_term_is_readonly(
    const ecs_iter_t *it,
    int32_t term_index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term_index > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_term_t *term = &it->terms[term_index - 1];
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (term->inout == EcsIn) {
        return true;
    } else {
        ecs_term_id_t *subj = &term->subj;

        if (term->inout == EcsInOutDefault) {
            if (subj->entity != EcsThis) {
                return true;
            }

            if (!(subj->set.mask & EcsSelf)) {
                return true;
            }
        }
    }

error:
    return false;
}

bool ecs_term_is_writeonly(
    const ecs_iter_t *it,
    int32_t term_index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(term_index > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_term_t *term = &it->terms[term_index - 1];
    ecs_check(term != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (term->inout == EcsOut) {
        return true;
    }

error:
    return false;
}

int32_t ecs_iter_find_column(
    const ecs_iter_t *it,
    ecs_entity_t component)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_search(it->real_world, it->table, component, 0);
error:
    return -1;
}

bool ecs_term_is_set(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);

    int32_t column = it->columns[index - 1];
    if (!column) {
        return false;
    } else if (column < 0) {
        if (it->references) {
            column = -column - 1;
            ecs_ref_t *ref = &it->references[column];
            return ref->entity != 0;
        } else {
            return true;
        }
    }

    return true;
error:
    return false;
}

void* ecs_iter_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    
    ecs_table_t *table = it->table;
    int32_t storage_index = ecs_table_type_to_storage_index(table, index);
    if (storage_index == -1) {
        return NULL;
    }

    ecs_type_info_t *ti = table->type_info[storage_index];
    ecs_check(!size || (ecs_size_t)size == ti->size, 
        ECS_INVALID_PARAMETER, NULL);

    ecs_column_t *column = &table->data.columns[storage_index];
    int32_t alignment = ti->alignment;
    return ecs_vector_get_t(column->data, flecs_uto(int32_t, size), alignment,
         it->offset);
error:
    return NULL;
}

size_t ecs_iter_column_size(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_table_t *table = it->table;
    int32_t storage_index = ecs_table_type_to_storage_index(table, index);
    if (storage_index == -1) {
        return 0;
    }

    ecs_type_info_t *ti = table->type_info[storage_index];
    return flecs_ito(size_t, ti->size);
error:
    return 0;
}

char* ecs_iter_str(
    const ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int i;

    if (it->term_count) {
        ecs_strbuf_list_push(&buf, "term: ", ",");
        for (i = 0; i < it->term_count; i ++) {
            ecs_id_t id = ecs_term_id(it, i + 1);
            char *str = ecs_id_str(world, id);
            ecs_strbuf_list_appendstr(&buf, str);
            ecs_os_free(str);
        }
        ecs_strbuf_list_pop(&buf, "\n");

        ecs_strbuf_list_push(&buf, "subj: ", ",");
        for (i = 0; i < it->term_count; i ++) {
            ecs_entity_t subj = ecs_term_source(it, i + 1);
            char *str = ecs_get_fullpath(world, subj);
            ecs_strbuf_list_appendstr(&buf, str);
            ecs_os_free(str);
        }
        ecs_strbuf_list_pop(&buf, "\n");
    }

    if (it->variable_count) {
        int32_t actual_count = 0;
        for (i = 0; i < it->variable_count; i ++) {
            const char *var_name = it->variable_names[i];
            if (!var_name || var_name[0] == '_' || var_name[0] == '.') {
                /* Skip anonymous variables */
                continue;
            }

            ecs_var_t var = it->variables[i];
            if (!var.entity) {
                /* Skip table variables */
                continue;
            }

            if (!actual_count) {
                ecs_strbuf_list_push(&buf, "vars: ", ",");
            }

            char *str = ecs_get_fullpath(world, var.entity);
            ecs_strbuf_list_append(&buf, "%s=%s", var_name, str);
            ecs_os_free(str);

            actual_count ++;
        }
        if (actual_count) {
            ecs_strbuf_list_pop(&buf, "\n");
        }
    }

    if (it->count) {
        ecs_strbuf_appendstr(&buf, "this:\n");
        for (i = 0; i < it->count; i ++) {
            ecs_entity_t e = it->entities[i];
            char *str = ecs_get_fullpath(world, e);
            ecs_strbuf_appendstr(&buf, "    - ");
            ecs_strbuf_appendstr(&buf, str);
            ecs_strbuf_appendstr(&buf, "\n");
            ecs_os_free(str);
        }
    }

    return ecs_strbuf_get(&buf);
}

void ecs_iter_poly(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter_out,
    ecs_term_t *filter)
{
    ecs_iterable_t *iterable = ecs_get_iterable(poly);
    iterable->init(world, poly, iter_out, filter);
}

bool ecs_iter_next(
    ecs_iter_t *iter)
{
    ecs_check(iter != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(iter->next != NULL, ECS_INVALID_PARAMETER, NULL);
    return iter->next(iter);
error:
    return false;
}

bool ecs_iter_count(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t count = 0;
    while (ecs_iter_next(it)) {
        count += it->count;
    }
    return count;
error:
    return 0;
}

bool ecs_iter_is_true(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    ECS_BIT_SET(it->flags, EcsIterIsFilter);
    ECS_BIT_SET(it->flags, EcsIterIsInstanced);

    bool result = ecs_iter_next(it);
    if (result) {
        ecs_iter_fini(it);
    }
    return result;
error:
    return false;
}

ecs_entity_t ecs_iter_get_var(
    ecs_iter_t *it,
    int32_t var_id)
{
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->variables != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_var_t *var = &it->variables[var_id];
    ecs_entity_t e = var->entity;
    if (!e) {
        ecs_table_t *table = var->range.table;
        if (table) {
            if ((var->range.count == 1) || (ecs_table_count(table) == 1)) {
                ecs_assert(ecs_table_count(table) > var->range.offset,
                    ECS_INTERNAL_ERROR, NULL);
                e = ecs_vector_get(table->data.entities, ecs_entity_t, 
                    var->range.offset)[0];
            }
        }
    } else {
        ecs_assert(ecs_is_valid(it->real_world, e), ECS_INTERNAL_ERROR, NULL);
    }

    return e;
error:
    return 0;
}

ecs_table_t* ecs_iter_get_var_as_table(
    ecs_iter_t *it,
    int32_t var_id)
{
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->variables != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_var_t *var = &it->variables[var_id];
    ecs_table_t *table = var->range.table;
    if (!table) {
        /* If table is not set, try to get table from entity */
        ecs_entity_t e = var->entity;
        if (e) {
            ecs_record_t *r = ecs_eis_get(it->real_world, e);
            if (r) {
                table = r->table;
                if (ecs_table_count(table) != 1) {
                    /* If table contains more than the entity, make sure not to
                     * return a partial table. */
                    return NULL;
                }
            }
        }
    }

    if (table) {
        if (var->range.offset) {
            /* Don't return whole table if only partial table is matched */
            return NULL;
        }

        if (!var->range.count || ecs_table_count(table) == var->range.count) {
            /* Return table if count matches */
            return table;
        }
    }

error:
    return NULL;
}

ecs_table_range_t ecs_iter_get_var_as_range(
    ecs_iter_t *it,
    int32_t var_id)
{
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->variables != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_range_t result = { 0 };

    ecs_var_t *var = &it->variables[var_id];
    ecs_table_t *table = var->range.table;
    if (!table) {
        ecs_entity_t e = var->entity;
        if (e) {
            ecs_record_t *r = ecs_eis_get(it->real_world, e);
            if (r) {
                result.table = r->table;
                result.offset = ECS_RECORD_TO_ROW(r->row);
                result.count = 1;
            }
        }
    } else {
        result.table = table;
        result.offset = var->range.offset;
        result.count = var->range.count;
        if (!result.count) {
            result.count = ecs_table_count(table);
        }
    }

    return result;
error:
    return (ecs_table_range_t){0};
}

void ecs_iter_set_var(
    ecs_iter_t *it,
    int32_t var_id,
    ecs_entity_t entity)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < ECS_VARIABLE_COUNT_MAX, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    /* Can't set variable while iterating */
    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->variables != NULL, ECS_INTERNAL_ERROR, NULL);

    iter_validate_cache(it);

    ecs_var_t *var = &it->variables[var_id];
    var->entity = entity;

    ecs_record_t *r = ecs_eis_get(it->real_world, entity);
    if (r) {
        var->range.table = r->table;
        var->range.offset = ECS_RECORD_TO_ROW(r->row);
        var->range.count = 1;
    } else {
        var->range.table = NULL;
        var->range.offset = 0;
        var->range.count = 0;
    }

    it->constrained_vars |= flecs_ito(uint64_t, 1 << var_id);

error:
    return;
}

void ecs_iter_set_var_as_table(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_t *table)
{
    ecs_table_range_t range = { .table = (ecs_table_t*)table };
    ecs_iter_set_var_as_range(it, var_id, &range);
}

void ecs_iter_set_var_as_range(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_range_t *range)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < ECS_VARIABLE_COUNT_MAX, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(range != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(range->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!range->offset || range->offset < ecs_table_count(range->table), 
        ECS_INVALID_PARAMETER, NULL);
    ecs_check((range->offset + range->count) <= ecs_table_count(range->table), 
        ECS_INVALID_PARAMETER, NULL);

    /* Can't set variable while iterating */
    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_OPERATION, NULL);

    iter_validate_cache(it);

    ecs_var_t *var = &it->variables[var_id];
    var->range = *range;

    if (range->count == 1) {
        ecs_table_t *table = range->table;
        var->entity = ecs_vector_get(
            table->data.entities, ecs_entity_t, range->offset)[0];
    } else {
        var->entity = 0;
    }

    it->constrained_vars |= flecs_uto(uint64_t, 1 << var_id);

error:
    return;
}

bool ecs_iter_var_is_constrained(
    ecs_iter_t *it,
    int32_t var_id)
{
    return (it->constrained_vars & (flecs_uto(uint64_t, 1 << var_id))) != 0;
}

ecs_iter_t ecs_page_iter(
    const ecs_iter_t *it,
    int32_t offset,
    int32_t limit)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t result = *it;
    result.priv.iter.page = (ecs_page_iter_t){
        .offset = offset,
        .limit = limit,
        .remaining = limit
    };
    result.next = ecs_page_next;
    result.chain_it = (ecs_iter_t*)it;

    return result;
error:
    return (ecs_iter_t){ 0 };
}

static
void offset_iter(
    ecs_iter_t *it,
    int32_t offset)
{
    it->entities = &it->entities[offset];

    int32_t t, term_count = it->term_count;
    for (t = 0; t < term_count; t ++) {
        void *ptrs = it->ptrs[t];
        if (!ptrs) {
            continue;
        }

        if (it->subjects[t]) {
            continue;
        }

        it->ptrs[t] = ECS_OFFSET(ptrs, offset * it->sizes[t]);
    }
}

static
bool ecs_page_next_instanced(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_page_next, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t *chain_it = it->chain_it;
    bool instanced = ECS_BIT_IS_SET(it->flags, EcsIterIsInstanced);

    do {
        if (!ecs_iter_next(chain_it)) {
            goto done;
        }

        ecs_page_iter_t *iter = &it->priv.iter.page;
        
        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv));

        /* Keep instancing setting from original iterator */
        ECS_BIT_COND(it->flags, EcsIterIsInstanced, instanced);

        if (!chain_it->table) {
            goto yield; /* Task query */
        }

        int32_t offset = iter->offset;
        int32_t limit = iter->limit;
        if (!(offset || limit)) {
            if (it->count) {
                goto yield;
            } else {
                goto done;
            }
        }

        int32_t count = it->count;
        int32_t remaining = iter->remaining;

        if (offset) {
            if (offset > count) {
                /* No entities to iterate in current table */
                iter->offset -= count;
                it->count = 0;
                continue;
            } else {
                it->offset += offset;
                count = it->count -= offset;
                iter->offset = 0;
                offset_iter(it, offset);
            }
        }

        if (remaining) {
            if (remaining > count) {
                iter->remaining -= count;
            } else {
                it->count = remaining;
                iter->remaining = 0;
            }
        } else if (limit) {
            /* Limit hit: no more entities left to iterate */
            goto done;
        }
    } while (it->count == 0);

yield:
    if (!ECS_BIT_IS_SET(it->flags, EcsIterIsInstanced)) {
        it->offset = 0;
    }

    return true;
done:
error:
    return false;
}

bool ecs_page_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_page_next, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);

    ECS_BIT_SET(it->chain_it->flags, EcsIterIsInstanced);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_page_next_instanced(it));
error:
    return false;
}

ecs_iter_t ecs_worker_iter(
    const ecs_iter_t *it,
    int32_t index,
    int32_t count)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(count > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index < count, ECS_INVALID_PARAMETER, NULL);

    return (ecs_iter_t){
        .real_world = it->real_world,
        .world = it->world,
        .priv.iter.worker = {
            .index = index,
            .count = count
        },
        .next = ecs_worker_next,
        .chain_it = (ecs_iter_t*)it,
        .flags = it->flags & EcsIterIsInstanced
    };

error:
    return (ecs_iter_t){ 0 };
}

static
bool ecs_worker_next_instanced(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_worker_next, ECS_INVALID_PARAMETER, NULL);

    bool instanced = ECS_BIT_IS_SET(it->flags, EcsIterIsInstanced);

    ecs_iter_t *chain_it = it->chain_it;
    ecs_worker_iter_t *iter = &it->priv.iter.worker;
    int32_t res_count = iter->count, res_index = iter->index;
    int32_t per_worker, instances_per_worker, first;

    do {
        if (!ecs_iter_next(chain_it)) {
            return false;
        }

        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv));

        /* Keep instancing setting from original iterator */
        ECS_BIT_COND(it->flags, EcsIterIsInstanced, instanced);

        int32_t count = it->count;
        int32_t instance_count = it->instance_count;
        per_worker = count / res_count;
        instances_per_worker = instance_count / res_count;
        first = per_worker * res_index;
        count -= per_worker * res_count;

        if (count) {
            if (res_index < count) {
                per_worker ++;
                first += res_index;
            } else {
                first += count;
            }
        }

        if (!per_worker && it->table == NULL) {
            if (res_index == 0) {
                return true;
            } else {
                return false;
            }
        }
    } while (!per_worker);

    it->instance_count = instances_per_worker;
    it->frame_offset += first;

    offset_iter(it, it->offset + first);
    it->count = per_worker;

    if (ECS_BIT_IS_SET(it->flags, EcsIterIsInstanced)) {
        it->offset += first;
    } else {
        it->offset = 0;
    }

    return true;
error:
    return false;
}

bool ecs_worker_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_worker_next, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);

    ECS_BIT_SET(it->chain_it->flags, EcsIterIsInstanced);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_worker_next_instanced(it));
error:
    return false;
}
