#include "private_api.h"
#include <stddef.h>

#define INIT_CACHE(it, f, term_count)\
    if (!it->f && term_count) {\
        if (term_count <= ECS_TERM_CACHE_SIZE) {\
            it->f = it->priv.cache.f;\
            it->priv.cache.f##_alloc = false;\
        } else {\
            it->f = ecs_os_calloc(ECS_SIZEOF(*(it->f)) * term_count);\
            it->priv.cache.f##_alloc = true;\
        }\
    }

#define FINI_CACHE(it, f)\
    if (it->f) {\
        if (it->priv.cache.f##_alloc) {\
            ecs_os_free((void*)it->f);\
        }\
    }   

void flecs_iter_init(
    ecs_iter_t *it)
{
    INIT_CACHE(it, ids, it->term_count);
    INIT_CACHE(it, subjects, it->term_count);
    INIT_CACHE(it, match_indices, it->term_count);
    INIT_CACHE(it, columns, it->term_count);
    
    if (!it->is_filter) {
        INIT_CACHE(it, sizes, it->term_count);
        INIT_CACHE(it, ptrs, it->term_count);
    } else {
        it->sizes = NULL;
        it->ptrs = NULL;
    }

    it->is_valid = true;
}

void ecs_iter_fini(
    ecs_iter_t *it)
{
    ecs_check(it->is_valid == true, ECS_INVALID_PARAMETER, NULL);
    it->is_valid = false;

    if (it->fini) {
        it->fini(it);
    }

    FINI_CACHE(it, ids);
    FINI_CACHE(it, columns);
    FINI_CACHE(it, subjects);
    FINI_CACHE(it, sizes);
    FINI_CACHE(it, ptrs);
    FINI_CACHE(it, match_indices);
error:
    return;
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

    if (!column) {
        /* Term has no data. This includes terms that have Not operators. */
        goto no_data;
    }

    ecs_assert(it->terms != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Filter terms may match with data but don't return it */
    if (it->terms[t].inout == EcsInOutFilter) {
        goto no_data;
    }

    ecs_table_t *table;
    ecs_vector_t *vec;
    ecs_size_t size;
    ecs_size_t align;
    int32_t row;

    if (column < 0) {
        is_shared = true;

        /* Data is not from This */
        if (it->references) {
            /* Iterator provides cached references for non-This terms */
            ecs_ref_t *ref = &it->references[-column - 1];
            if (ptr_out) ptr_out[0] = (void*)ecs_get_ref_id(
                world, ref, ref->entity, ref->component);

            /* If cached references were provided, the code that populated
             * the iterator also had a chance to cache sizes, so size array
             * should already have been assigned. This saves us from having
             * to do additional lookups to find the component size. */
            ecs_assert(size_out == NULL, ECS_INTERNAL_ERROR, NULL);
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
                /* The entity has no components or the id is not a component */
                
                ecs_id_t term_id = it->terms[t].id;
                if (ECS_HAS_ROLE(term_id, SWITCH) || ECS_HAS_ROLE(term_id, CASE)) {
                    /* Edge case: if this is a switch. Find switch column in
                     * actual table, as its not in the storage table */
                    tr = flecs_get_table_record(world, table, id);
                    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
                    column = tr->column;
                    goto has_switch;
                } else {
                    goto no_data;
                }
            }

            /* We now have row and column, so we can get the storage for the id
             * which gives us the pointer and size */
            column = tr->column;
            ecs_column_t *s = &table->storage.columns[column];
            size = s->size;
            align = s->alignment;
            vec = s->data;
            /* Fallthrough to has_data */
        }
    } else {
        /* Data is from This, use table from iterator */
        table = it->table;
        if (!table || !ecs_table_count(table)) {
            goto no_data;
        }

        row = it->offset;
        
        int32_t storage_column = ecs_table_type_to_storage_index(
            table, column - 1);
        if (storage_column == -1) {
            ecs_id_t id = it->terms[t].id;
            if (ECS_HAS_ROLE(id, SWITCH) || ECS_HAS_ROLE(id, CASE)) {
                goto has_switch;
            }
            goto no_data;
        }

        ecs_column_t *s = &table->storage.columns[storage_column];
        size = s->size;
        align = s->alignment;
        vec = s->data;
        /* Fallthrough to has_data */
    }

has_data:
    if (ptr_out) ptr_out[0] = ecs_vector_get_t(vec, size, align, row);
    if (size_out) size_out[0] = size;
    return is_shared;

has_switch: {
        /* Edge case: if column is a switch we should return the vector with case
        * identifiers. Will be replaced in the future with pluggable storage */
        ecs_switch_t *sw = table->storage.sw_columns[
            (column - 1) - table->sw_column_offset].data;
        vec = flecs_switch_values(sw);
        size = ECS_SIZEOF(ecs_entity_t);
        align = ECS_ALIGNOF(ecs_entity_t);
        goto has_data;
    }

no_data:
    if (ptr_out) ptr_out[0] = NULL;
    if (size_out) size_out[0] = 0;
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
        it->type = it->table->type;
        if (!count) {
            count = it->count = ecs_table_count(table);
        }
        if (count) {
            it->entities = ecs_vector_get(
                table->storage.entities, ecs_entity_t, offset);
        } else {
            it->entities = NULL;
        }
    }

    if (it->is_filter) {
        it->has_shared = false;
        return;
    }

    int t, term_count = it->term_count;
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

    it->has_shared = has_shared;
}

bool flecs_iter_next_row(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INTERNAL_ERROR, NULL);

    bool is_instanced = it->is_instanced;
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
    if (result && !it->is_instanced && it->count && it->has_shared) {
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
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);
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
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);
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
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);
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
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_search(it->real_world, it->table, component, 0);
error:
    return -1;
}

bool ecs_term_is_set(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);

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
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    
    ecs_table_t *table = it->table;
    int32_t storage_index = ecs_table_type_to_storage_index(table, index);
    if (storage_index == -1) {
        return NULL;
    }

    ecs_column_t *columns = table->storage.columns;
    ecs_column_t *column = &columns[storage_index];
    ecs_check(!size || (ecs_size_t)size == column->size, 
        ECS_INVALID_PARAMETER, NULL);

    void *ptr = ecs_vector_first_t(
        column->data, column->size, column->alignment);

    return ECS_OFFSET(ptr, column->size * it->offset);
error:
    return NULL;
}

size_t ecs_iter_column_size(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_table_t *table = it->table;
    int32_t storage_index = ecs_table_type_to_storage_index(table, index);
    if (storage_index == -1) {
        return 0;
    }

    ecs_column_t *columns = table->storage.columns;
    ecs_column_t *column = &columns[storage_index];
    
    return flecs_ito(size_t, column->size);
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

            ecs_entity_t var = it->variables[i];
            if (!var) {
                /* Skip table variables */
                continue;
            }

            if (!actual_count) {
                ecs_strbuf_list_push(&buf, "vars: ", ",");
            }

            char *str = ecs_get_fullpath(world, var);
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
    bool result = ecs_iter_next(it);
    if (result) {
        ecs_iter_fini(it);
    }
    return result;
error:
    return false;
}

ecs_iter_t ecs_page_iter(
    const ecs_iter_t *it,
    int32_t offset,
    int32_t limit)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next != NULL, ECS_INVALID_PARAMETER, NULL);

    return (ecs_iter_t){
        .real_world = it->real_world,
        .world = it->world,
        .priv.iter.page = {
            .offset = offset,
            .limit = limit,
            .remaining = limit
        },
        .next = ecs_page_next,
        .chain_it = (ecs_iter_t*)it,
        .is_instanced = it->is_instanced
    };
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
    bool instanced = it->is_instanced;

    do {
        if (!ecs_iter_next(chain_it)) {
            goto done;
        }

        ecs_page_iter_t *iter = &it->priv.iter.page;
        
        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv));
        it->is_instanced = instanced;

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
    if (!it->is_instanced) {
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

    it->chain_it->is_instanced = true;

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
        .is_instanced = it->is_instanced
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

    bool instanced = it->is_instanced;

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
        it->is_instanced = instanced;

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

    if (it->is_instanced) {
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

    it->chain_it->is_instanced = true;

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_worker_next_instanced(it));
error:
    return false;
}
