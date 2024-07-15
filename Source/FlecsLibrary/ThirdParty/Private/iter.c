/**
 * @file iter.c
 * @brief Iterator API.
 * 
 * The iterator API contains functions that apply to all iterators, such as
 * resource management, or fetching resources for a matched table. The API also
 * contains functions for generic iterators, which make it possible to iterate
 * an iterator without needing to know what created the iterator.
 */

#include "private_api.h"
#include <stddef.h>

/* Utility macros to enforce consistency when initializing iterator fields */

/* If term count is smaller than cache size, initialize with inline array,
 * otherwise allocate. */
#define INIT_CACHE(it, stack, fields, f, T, count)\
    if (!it->f && (fields & flecs_iter_cache_##f) && count) {\
        it->f = flecs_stack_calloc_n(stack, T, count);\
        it->priv_.cache.used |= flecs_iter_cache_##f;\
    }

/* If array is allocated, free it when finalizing the iterator */
#define FINI_CACHE(it, f, T, count)\
    if (it->priv_.cache.used & flecs_iter_cache_##f) {\
        flecs_stack_free_n((void*)it->f, T, count);\
    }

void* flecs_iter_calloc(
    ecs_iter_t *it,
    ecs_size_t size,
    ecs_size_t align)
{
    ecs_world_t *world = it->world;
    ecs_stage_t *stage = flecs_stage_from_world((ecs_world_t**)&world);
    ecs_stack_t *stack = &stage->allocators.iter_stack;
    return flecs_stack_calloc(stack, size, align); 
}

void flecs_iter_free(
    void *ptr,
    ecs_size_t size)
{
    flecs_stack_free(ptr, size);
}

void flecs_iter_init(
    const ecs_world_t *world,
    ecs_iter_t *it,
    ecs_flags8_t fields)
{
    ecs_assert(!ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(
        ECS_CONST_CAST(ecs_world_t**, &world));
    ecs_stack_t *stack = &stage->allocators.iter_stack;

    it->priv_.cache.used = 0;
    it->priv_.cache.allocated = 0;
    it->priv_.cache.stack_cursor = flecs_stack_get_cursor(stack);

    INIT_CACHE(it, stack, fields, ids, ecs_id_t, it->field_count);
    INIT_CACHE(it, stack, fields, sources, ecs_entity_t, it->field_count);
    INIT_CACHE(it, stack, fields, columns, int32_t, it->field_count);
    INIT_CACHE(it, stack, fields, variables, ecs_var_t, it->variable_count);
    INIT_CACHE(it, stack, fields, ptrs, void*, it->field_count);
}

void flecs_iter_validate(
    ecs_iter_t *it)
{
    ECS_BIT_SET(it->flags, EcsIterIsValid);

    /* Make sure multithreaded iterator isn't created for real world */
    ecs_world_t *world = it->real_world;
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(!(world->flags & EcsWorldMultiThreaded) || it->world != it->real_world,
        ECS_INVALID_PARAMETER, 
            "create iterator for stage when world is in multithreaded mode");
    (void)world;
error:
    return;
}

void ecs_iter_fini(
    ecs_iter_t *it)
{
    ECS_BIT_CLEAR(it->flags, EcsIterIsValid);

    if (it->fini) {
        it->fini(it);
    }

    ecs_world_t *world = it->world;
    if (!world) {
        return;
    }

    FINI_CACHE(it, ids, ecs_id_t, it->field_count);
    FINI_CACHE(it, sources, ecs_entity_t, it->field_count);
    FINI_CACHE(it, columns, int32_t, it->field_count);
    FINI_CACHE(it, variables, ecs_var_t, it->variable_count);
    FINI_CACHE(it, ptrs, void*, it->field_count);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    flecs_stack_restore_cursor(&stage->allocators.iter_stack, 
        it->priv_.cache.stack_cursor);
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
            int t, field_count = it->field_count;

            for (t = 0; t < field_count; t ++) {
                ecs_entity_t src = it->sources[t];
                if (!src) {
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

    if (result && !is_instanced && it->count && it->shared_fields) {
        it->count = 1;
    }

    return result;
}

/* --- Public API --- */

void* ecs_field_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);
    ecs_check(!size || ecs_field_size(it, index) == size ||
        (!ecs_field_size(it, index) && (!it->ptrs[index])),
            ECS_INVALID_PARAMETER, "mismatching size for field %d", index);
    (void)size;

    return it->ptrs[index];
error:
    return NULL;
}

bool ecs_field_is_readonly(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(it->query != NULL, ECS_INVALID_PARAMETER,
        "operation only valid for query iterators");
    ecs_check(it->query->terms != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);
    const ecs_term_t *term = &it->query->terms[index];

    if (term->inout == EcsIn) {
        return true;
    } else if (term->inout == EcsInOutDefault) {
        if (!ecs_term_match_this(term)) {
            return true;
        }

        const ecs_term_ref_t *src = &term->src;
        if (!(src->id & EcsSelf)) {
            return true;
        }
    }
error:
    return false;
}

bool ecs_field_is_writeonly(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(it->query != NULL, ECS_INVALID_PARAMETER,
        "operation only valid for query iterators");
    ecs_check(it->query->terms != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    const ecs_term_t *term = &it->query->terms[index];
    return term->inout == EcsOut;
error:
    return false;
}

bool ecs_field_is_set(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    return it->set_fields & (1llu << (index));
error:
    return false;
}

bool ecs_field_is_self(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    return it->sources == NULL || it->sources[index] == 0;
error:
    return false;
}

ecs_id_t ecs_field_id(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    return it->ids[index];
error:
    return 0;
}

int32_t ecs_field_column(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    return it->columns[index];
error:
    return 0;
}

ecs_entity_t ecs_field_src(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    if (it->sources) {
        return it->sources[index];
    } else {
        return 0;
    }
error:
    return 0;
}

size_t ecs_field_size(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    return (size_t)it->sizes[index];
error:
    return 0;
}

char* ecs_iter_str(
    const ecs_iter_t *it)
{
    if (!(it->flags & EcsIterIsValid)) {
        return NULL;
    }

    ecs_world_t *world = it->world;
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int i;

    if (it->field_count) {
        ecs_strbuf_list_push(&buf, "id:  ", ",");
        for (i = 0; i < it->field_count; i ++) {
            ecs_id_t id = ecs_field_id(it, i);
            char *str = ecs_id_str(world, id);
            ecs_strbuf_list_appendstr(&buf, str);
            ecs_os_free(str);
        }
        ecs_strbuf_list_pop(&buf, "\n");

        ecs_strbuf_list_push(&buf, "src: ", ",");
        for (i = 0; i < it->field_count; i ++) {
            ecs_entity_t subj = ecs_field_src(it, i);
            char *str = ecs_get_path(world, subj);
            ecs_strbuf_list_appendstr(&buf, str);
            ecs_os_free(str);
        }
        ecs_strbuf_list_pop(&buf, "\n");

        ecs_strbuf_list_push(&buf, "set: ", ",");
        for (i = 0; i < it->field_count; i ++) {
            if (ecs_field_is_set(it, i)) {
                ecs_strbuf_list_appendlit(&buf, "true");
            } else {
                ecs_strbuf_list_appendlit(&buf, "false");
            }
        }
        ecs_strbuf_list_pop(&buf, "\n");
    }

    if (it->variable_count && it->variable_names) {
        int32_t actual_count = 0;
        for (i = 0; i < it->variable_count; i ++) {
            const char *var_name = it->variable_names[i];
            if (!var_name || var_name[0] == '_' || !strcmp(var_name, "this")) {
                /* Skip anonymous variables */
                continue;
            }

            ecs_var_t var = it->variables[i];
            if (!var.entity) {
                /* Skip table variables */
                continue;
            }

            if (!actual_count) {
                ecs_strbuf_list_push(&buf, "var: ", ",");
            }

            char *str = ecs_get_path(world, var.entity);
            ecs_strbuf_list_append(&buf, "%s=%s", var_name, str);
            ecs_os_free(str);

            actual_count ++;
        }
        if (actual_count) {
            ecs_strbuf_list_pop(&buf, "\n");
        }
    }

    if (it->count) {
        ecs_strbuf_appendlit(&buf, "this:\n");
        for (i = 0; i < it->count; i ++) {
            ecs_entity_t e = it->entities[i];
            char *str = ecs_get_path(world, e);
            ecs_strbuf_appendlit(&buf, "    - ");
            ecs_strbuf_appendstr(&buf, str);
            ecs_strbuf_appendch(&buf, '\n');
            ecs_os_free(str);
        }
    }

    return ecs_strbuf_get(&buf);
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

int32_t ecs_iter_count(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    ECS_BIT_SET(it->flags, EcsIterNoData);
    ECS_BIT_SET(it->flags, EcsIterIsInstanced);

    int32_t count = 0;
    while (ecs_iter_next(it)) {
        count += it->count;
    }
    return count;
error:
    return 0;
}

ecs_entity_t ecs_iter_first(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    ECS_BIT_SET(it->flags, EcsIterNoData);
    ECS_BIT_SET(it->flags, EcsIterIsInstanced);

    ecs_entity_t result = 0;
    if (ecs_iter_next(it)) {
        result = it->entities[0];
        ecs_iter_fini(it);
    }

    return result;
error:
    return 0;
}

bool ecs_iter_is_true(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    ECS_BIT_SET(it->flags, EcsIterNoData);
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
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, 
        "invalid variable index %d", var_id);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER,
        "variable index %d out of bounds", var_id);
    ecs_check(it->variables != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_var_t *var = &it->variables[var_id];
    ecs_entity_t e = var->entity;
    if (!e) {
        ecs_table_t *table = var->range.table;
        if (!table && !var_id) {
            table = it->table;
        }
        if (table) {
            if ((var->range.count == 1) || (ecs_table_count(table) == 1)) {
                ecs_assert(ecs_table_count(table) > var->range.offset,
                    ECS_INTERNAL_ERROR, NULL);
                e = ecs_vec_get_t(&table->data.entities, ecs_entity_t,
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
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, 
        "invalid variable index %d", var_id);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, 
        "variable index %d out of bounds", var_id);
    ecs_check(it->variables != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_var_t *var = &it->variables[var_id];
    ecs_table_t *table = var->range.table;
    if (!table && !var_id) {
        table = it->table;
    }

    if (!table) {
        /* If table is not set, try to get table from entity */
        ecs_entity_t e = var->entity;
        if (e) {
            ecs_record_t *r = flecs_entities_get(it->real_world, e);
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
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, 
        "invalid variable index %d", var_id);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, 
        "variable index %d out of bounds", var_id);
    ecs_check(it->variables != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_range_t result = { 0 };

    ecs_var_t *var = &it->variables[var_id];
    ecs_table_t *table = var->range.table;
    if (!table && !var_id) {
        table = it->table;
    }
   
    if (!table) {
        ecs_entity_t e = var->entity;
        if (e) {
            ecs_record_t *r = flecs_entities_get(it->real_world, e);
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
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, 
        "invalid variable index %d", var_id);
    ecs_check(var_id < FLECS_QUERY_VARIABLE_COUNT_MAX, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, 
        "variable index %d out of bounds", var_id);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_PARAMETER,
        "cannot constrain variable while iterating");
    ecs_check(it->variables != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_var_t *var = &it->variables[var_id];
    var->entity = entity;

    ecs_record_t *r = flecs_entities_get(it->real_world, entity);
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
    ecs_table_range_t range = { .table = ECS_CONST_CAST(ecs_table_t*, table) };
    ecs_iter_set_var_as_range(it, var_id, &range);
}

void ecs_iter_set_var_as_range(
    ecs_iter_t *it,
    int32_t var_id,
    const ecs_table_range_t *range)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, 
        "invalid variable index %d", var_id);
    ecs_check(var_id < it->variable_count, ECS_INVALID_PARAMETER, 
        "variable index %d out of bounds", var_id);
    ecs_check(range != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(range->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!range->offset || range->offset < ecs_table_count(range->table), 
        ECS_INVALID_PARAMETER, NULL);
    ecs_check((range->offset + range->count) <= ecs_table_count(range->table), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_OPERATION, 
        "cannot set query variables while iterating");

    ecs_var_t *var = &it->variables[var_id];
    var->range = *range;

    if (range->count == 1) {
        ecs_table_t *table = range->table;
        var->entity = ecs_vec_get_t(
            &table->data.entities, ecs_entity_t, range->offset)[0];
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

static
void ecs_chained_iter_fini(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_fini(it->chain_it);

    it->chain_it = NULL;
}

ecs_iter_t ecs_page_iter(
    const ecs_iter_t *it,
    int32_t offset,
    int32_t limit)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t result = *it;
    result.priv_.cache.stack_cursor = NULL; /* Don't copy allocator cursor */

    result.priv_.iter.page = (ecs_page_iter_t){
        .offset = offset,
        .limit = limit,
        .remaining = limit
    };
    result.next = ecs_page_next;
    result.fini = ecs_chained_iter_fini;
    result.chain_it = ECS_CONST_CAST(ecs_iter_t*, it);

    return result;
error:
    return (ecs_iter_t){ 0 };
}

static
void flecs_offset_iter(
    ecs_iter_t *it,
    int32_t offset)
{
    it->entities = &it->entities[offset];

    int32_t t, field_count = it->field_count;
    void **it_ptrs = it->ptrs;
    if (it_ptrs) {
        for (t = 0; t < field_count; t ++) {
            void *ptrs = it_ptrs[t];
            if (!ptrs) {
                continue;
            }

            if (it->sources[t]) {
                continue;
            }

            it->ptrs[t] = ECS_OFFSET(ptrs, offset * it->sizes[t]);
        }
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
            goto depleted;
        }

        ecs_page_iter_t *iter = &it->priv_.iter.page;
        
        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv_));

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
                goto depleted;
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
                flecs_offset_iter(it, offset);
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
    /* Cleanup iterator resources if it wasn't yet depleted */
    ecs_iter_fini(chain_it);
depleted:
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
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < count, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t result = *it;
    result.priv_.cache.stack_cursor = NULL; /* Don't copy allocator cursor */
    
    result.priv_.iter.worker = (ecs_worker_iter_t){
        .index = index,
        .count = count
    };
    result.next = ecs_worker_next;
    result.fini = ecs_chained_iter_fini;
    result.chain_it = ECS_CONST_CAST(ecs_iter_t*, it);

    return result;
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
    ecs_worker_iter_t *iter = &it->priv_.iter.worker;
    int32_t res_count = iter->count, res_index = iter->index;
    int32_t per_worker, instances_per_worker, first;

    do {
        if (!ecs_iter_next(chain_it)) {
            return false;
        }

        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv_));

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
                // chained iterator was not yet cleaned up
                // since it returned true from ecs_iter_next, so clean it up here.
                ecs_iter_fini(chain_it);
                return false;
            }
        }
    } while (!per_worker);

    it->instance_count = instances_per_worker;
    it->frame_offset += first;

    flecs_offset_iter(it, it->offset + first);
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
