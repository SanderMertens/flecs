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

/* Utility macros to enforce consistency when initializing iterator fields */

/* If term count is smaller than cache size, initialize with inline array,
 * otherwise allocate. */

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
    bool alloc_resources)
{
    ecs_assert(!ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(
        ECS_CONST_CAST(ecs_world_t**, &world));
    ecs_stack_t *stack = &stage->allocators.iter_stack;

    it->priv_.stack_cursor = flecs_stack_get_cursor(stack);

    if (alloc_resources && it->field_count) {
        ecs_assert(it->ids == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(it->sources == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(it->trs == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(it->columns == NULL, ECS_INTERNAL_ERROR, NULL);

        int32_t fc = it->field_count;
        ecs_size_t wide = (ecs_size_t)(sizeof(ecs_id_t) + sizeof(ecs_entity_t) +
            sizeof(ecs_table_record_t*)) * fc;
        ecs_size_t cols = (ecs_size_t)sizeof(int16_t) * fc;
        char *buf = flecs_stack_alloc(stack, wide + cols,
            ECS_ALIGNOF(ecs_id_t));

        it->ids = (ecs_id_t*)(void*)buf;
        it->sources = (ecs_entity_t*)(void*)(buf +
            (ecs_size_t)sizeof(ecs_id_t) * fc);
        it->trs = (const ecs_table_record_t**)(void*)(buf +
            (ecs_size_t)(sizeof(ecs_id_t) + sizeof(ecs_entity_t)) * fc);
        int16_t *columns = (int16_t*)(void*)(buf + wide);

        ecs_os_memset(buf, 0, wide);
        ecs_os_memset(columns, 0xFF, cols);
        it->columns = columns;
    }
}

void ecs_iter_fini(
    ecs_iter_t *it)
{
    if (it->fini) {
        it->fini(it);
    }

    ECS_BIT_CLEAR(it->flags, EcsIterIsValid);

    ecs_world_t *world = it->world;
    if (!world) {
        return;
    }

    /* Make sure arrays are below stack page size, which means they don't have
     * to get freed explicitly. */
    ecs_assert(ECS_SIZEOF(ecs_id_t) * it->field_count < FLECS_STACK_PAGE_SIZE,
        ECS_UNSUPPORTED, NULL);
    ecs_assert(ECS_SIZEOF(ecs_entity_t) * it->field_count < FLECS_STACK_PAGE_SIZE,
        ECS_UNSUPPORTED, NULL);
    ecs_assert(ECS_SIZEOF(ecs_table_record_t*) * it->field_count < FLECS_STACK_PAGE_SIZE,
        ECS_UNSUPPORTED, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    flecs_stack_restore_cursor(&stage->allocators.iter_stack, 
        it->priv_.stack_cursor);
}

/* --- Public API --- */

void* ecs_field_w_size(
    const ecs_iter_t *it,
    size_t size,
    int8_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);
    ecs_check(size != 0, ECS_INVALID_PARAMETER, 
        "missing size for field %d", index);
    ecs_check(ecs_field_size(it, index) == size || 
        !ecs_field_size(it, index),
            ECS_INVALID_PARAMETER, 
            "mismatching size for field %d (expected '%s')", 
            index,
            flecs_errstr(ecs_id_str(it->world, it->ids[index])));
    (void)size;

    if (it->ptrs) {
        return it->ptrs[index];
    }

    int16_t column = it->columns[index];
    if (column >= 0) {
        return ECS_ELEM(it->table->data.columns[column].data,
            (ecs_size_t)size, it->offset);
    }

    return flecs_field_shared(it, size, index);
error:
    return NULL;
}

void* flecs_field_shared(
    const ecs_iter_t *it,
    size_t size,
    int8_t index)
{
    if (!ecs_field_is_set(it, index)) {
        return NULL;
    }

    ecs_entity_t src = it->sources[index];
    ecs_record_t *r = flecs_entities_get(it->real_world, src);
    ecs_table_t *table = r->table;

    ecs_component_record_t *cr = flecs_components_get(
        it->real_world, it->ids[index]);
    const ecs_table_record_t *tr = flecs_component_get_table(cr, table);
    int16_t column = tr->column;

    return ECS_ELEM(table->data.columns[column].data,
        (ecs_size_t)size, ECS_RECORD_TO_ROW(r->row));
}

static ecs_component_record_t* flecs_field_cr(
    const ecs_iter_t *it,
    int8_t index)
{
    ecs_component_record_t *cr = NULL;
    const ecs_table_record_t *tr = it->trs[index];
    if (!tr) {
        const ecs_query_t *q = it->query;
        if (q) {
            ecs_component_record_t **cr_cache = flecs_query_impl(q)->cr_cache;
            cr = cr_cache[index];
            if (!cr || cr->id != it->ids[index]) {
                cr = cr_cache[index] = flecs_components_get(
                    it->real_world, it->ids[index]);
            }
        } else {
            cr = flecs_components_get(it->real_world, it->ids[index]);
        }
        ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    } else {
        cr = tr->hdr.cr;
    }
    return cr;
}

ecs_sparse_t* flecs_field_sparse(
    const ecs_iter_t *it,
    int8_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(index >= 0, ECS_INVALID_PARAMETER,
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER,
        "field index %d out of bounds", index);
    ecs_check((it->row_fields & (1llu << index)), ECS_INVALID_PARAMETER,
        "field %d is not a row field", index);

    if (it->sources[index]) {
        return NULL;
    }

    return flecs_field_cr(it, index)->sparse;
error:
    return NULL;
}

void* ecs_field_at_w_size(
    const ecs_iter_t *it,
    size_t size,
    int8_t index,
    int32_t row)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");
    ecs_check(index >= 0, ECS_INVALID_PARAMETER,
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER,
        "field index %d out of bounds", index);
    ecs_check(!size || ecs_field_size(it, index) == size ||
        !ecs_field_size(it, index),
            ECS_INVALID_PARAMETER, "mismatching size for field %d", index);

    ecs_component_record_t *cr = flecs_field_cr(it, index);

    ecs_assert((cr->flags & EcsIdSparse), ECS_INVALID_OPERATION,
        "use ecs_field to access fields for non-sparse components");
    ecs_assert(it->row_fields & (1ull << index), ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t src = it->sources[index];
    if (!src) {
        src = it->entities[row];
    }

    return flecs_sparse_get(cr->sparse, flecs_uto(int32_t, size), src);
error:
    return NULL;
}

bool ecs_field_is_readonly(
    const ecs_iter_t *it,
    int8_t index)
{
    ecs_check(it->flags & EcsIterIsValid, ECS_INVALID_PARAMETER,
        "operation invalid before calling next()");

    if (!it->query) {
        return false;
    }

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
    int8_t index)
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
    int8_t index)
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
    int8_t index)
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
    int8_t index)
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
    int8_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER,
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER,
        "field index %d out of bounds", index);

    if (!ecs_field_is_set(it, index)) {
        return -1;
    }

    if (it->columns && it->columns[index] >= 0) {
        return ecs_table_column_to_type_index(it->table, it->columns[index]);
    }

    if (it->trs && it->trs[index]) {
        return it->trs[index]->index;
    }

    ecs_entity_t src = it->sources[index];
    ecs_assert(src != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_record_t *r = flecs_entities_get(it->real_world, src);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(r->table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_component_record_t *cr = flecs_components_get(
        it->real_world, it->ids[index]);
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_table_record_t *tr = flecs_component_get_table(cr, r->table);
    ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
    return tr->index;
error:
    return 0;
}

ecs_entity_t ecs_field_src(
    const ecs_iter_t *it,
    int8_t index)
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
    int8_t index)
{
    ecs_check(index >= 0, ECS_INVALID_PARAMETER, 
        "invalid field index %d", index);
    ecs_check(index < it->field_count, ECS_INVALID_PARAMETER, 
        "field index %d out of bounds", index);

    return (size_t)it->sizes[index];
error:
    return 0;
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

    bool result = ecs_iter_next(it);
    if (result) {
        ecs_iter_fini(it);
    }
    return result;
error:
    return false;
}

#ifdef FLECS_QUERY_PLANS

ecs_entity_t ecs_iter_get_var(
    ecs_iter_t *it,
    int32_t var_id)
{
    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER,
        "invalid variable index %d", var_id);
    ecs_check(var_id < ecs_iter_get_var_count(it), ECS_INVALID_PARAMETER,
        "variable index %d out of bounds", var_id);
    ecs_check(ecs_iter_get_vars(it) != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_var_t *var = &ecs_iter_get_vars(it)[var_id];
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
                e = ecs_table_entities(table)[var->range.offset];
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
    ecs_check(var_id < ecs_iter_get_var_count(it), ECS_INVALID_PARAMETER,
        "variable index %d out of bounds", var_id);
    ecs_check(ecs_iter_get_vars(it) != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_var_t *var = &ecs_iter_get_vars(it)[var_id];
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
    ecs_check(var_id < ecs_iter_get_var_count(it), ECS_INVALID_PARAMETER,
        "variable index %d out of bounds", var_id);
    ecs_check(ecs_iter_get_vars(it) != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_table_range_t result = { 0 };

    ecs_var_t *var = &ecs_iter_get_vars(it)[var_id];
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

const char* ecs_iter_get_var_name(
    const ecs_iter_t *it,
    int32_t var_id)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    if (var_id == 0) {
        return "this";
    }

    const ecs_query_t *query = it->query;
    ecs_check(query != NULL, ECS_INVALID_PARAMETER,
        "can only obtain variable name for iterators that iterate query");
    ecs_check(var_id < query->var_count, ECS_INVALID_PARAMETER,
        "variable index out of range for query");

    return query->vars[var_id];
error:
    return NULL;
}

int32_t ecs_iter_get_var_count(
    const ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    if (it->query) {
        return it->query->var_count;
    }

    return 1;
error:
    return 0;
}

ecs_var_t* ecs_iter_get_vars(
    const ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    if (!it->query) {
        return NULL;
    }

    if (it->chain_it) {
        return ecs_iter_get_vars(it->chain_it);
    }

    return it->priv_.iter.query.vars;
error:
    return NULL;
}

bool ecs_iter_var_is_constrained(
    ecs_iter_t *it,
    int32_t var_id)
{
    if (it->chain_it) {
        return ecs_iter_var_is_constrained(it->chain_it, var_id);
    }

    return (it->constrained_vars & (1llu << var_id)) != 0;
}

#endif // FLECS_QUERY_PLANS

void ecs_iter_set_var(
    ecs_iter_t *it,
    int32_t var_id,
    ecs_entity_t entity)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    if (it->chain_it) {
        ecs_iter_set_var(it->chain_it, var_id, entity);
        return;
    }

    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER,
        "invalid variable index %d", var_id);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_PARAMETER,
        "cannot constrain variable while iterating");

#ifdef FLECS_QUERY_PLANS
    ecs_check(var_id < FLECS_QUERY_VARIABLE_COUNT_MAX, ECS_INVALID_PARAMETER, NULL);
    ecs_check(var_id < ecs_iter_get_var_count(it), ECS_INVALID_PARAMETER,
        "variable index %d out of bounds", var_id);
    ecs_check(ecs_iter_get_vars(it) != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_var_t *var = &ecs_iter_get_vars(it)[var_id];
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

    it->constrained_vars |= 1llu << var_id;
#else
    ecs_check(var_id == 0, ECS_UNSUPPORTED,
        "setting variables other than $this requires the FLECS_QUERY_PLANS addon");
    ecs_check(it->query != NULL, ECS_INVALID_PARAMETER,
        "can only constrain query iterators");

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    qit->constrained_this = true;
    qit->entity = entity;

    ecs_record_t *r = flecs_entities_get(it->real_world, entity);
    if (r) {
        it->table = r->table;
        it->offset = ECS_RECORD_TO_ROW(r->row);
        it->count = 1;
        it->entities = &ecs_table_entities(it->table)[it->offset];
    } else {
        it->table = NULL;
        it->offset = 0;
        it->count = 1;
        it->entities = &qit->entity;
    }
#endif

    /* Update iterator for constrained iterator */
    flecs_query_iter_constrain(it);

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

    if (it->chain_it) {
        ecs_iter_set_var_as_range(it->chain_it, var_id, range);
        return;
    }

    ecs_check(var_id >= 0, ECS_INVALID_PARAMETER, 
        "invalid variable index %d", var_id);
#ifdef FLECS_QUERY_PLANS
    ecs_check(var_id < ecs_iter_get_var_count(it), ECS_INVALID_PARAMETER, 
        "variable index %d out of bounds", var_id);
#else
    ecs_check(var_id == 0, ECS_INVALID_PARAMETER,
        "only the $this variable is available without query plan support");
    ecs_check(it->query != NULL, ECS_INVALID_PARAMETER,
        "can only constrain query iterators");
#endif
    ecs_check(range != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(range->table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!range->offset || range->offset < ecs_table_count(range->table), 
        ECS_INVALID_PARAMETER, NULL);
    ecs_check((range->offset + range->count) <= ecs_table_count(range->table), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_OPERATION, 
        "cannot set query variables while iterating");

#ifdef FLECS_QUERY_PLANS
    ecs_var_t *var = &ecs_iter_get_vars(it)[var_id];
    var->range = *range;

    if (range->count == 1) {
        ecs_table_t *table = range->table;
        var->entity = ecs_table_entities(table)[range->offset];
    } else {
        var->entity = 0;
    }

    it->constrained_vars |= 1llu << var_id;
#else
    ecs_query_iter_t *qit = &it->priv_.iter.query;
    qit->constrained_this = true;
    qit->entity = 0;

    it->table = range->table;
    it->offset = range->offset;
    it->count = range->count;
    if (!it->count) {
        it->count = ecs_table_count(it->table);
    }
    it->entities = ecs_table_entities(it->table);
    if (it->entities) {
        it->entities += it->offset;
    }
#endif

    /* Update iterator for constrained iterator */
    flecs_query_iter_constrain(it);

error:
    return;
}

#ifdef FLECS_CACHED_QUERIES

uint64_t ecs_iter_get_group(
    const ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);

    if (it->chain_it) {
        return ecs_iter_get_group(it->chain_it);
    }

    ecs_check(it->query != NULL, ECS_INVALID_PARAMETER, 
        "ecs_iter_get_group must be called on iterator that iterates a query");
    const ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_check(qit->group != NULL, ECS_INVALID_PARAMETER,
        "ecs_iter_get_group must be called on iterator that iterates a cached "
        "query (query is uncached)");

    return qit->group->info.id;
error:
    return 0;
}

#endif // FLECS_CACHED_QUERIES

static void ecs_chained_iter_fini(
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
    result.priv_.stack_cursor = NULL; /* Don't copy allocator cursor */

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

bool ecs_page_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_page_next, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t *chain_it = it->chain_it;

    do {
        if (!ecs_iter_next(chain_it)) {
            goto depleted;
        }

        ecs_page_iter_t *iter = &it->priv_.iter.page;
        
        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv_));

        if (!chain_it->table && !chain_it->count) {
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
                iter->offset = 0;
                it->offset = offset;
                count = it->count -= offset;
                if (it->table) {
                    it->entities =
                        &(ecs_table_entities(it->table)[it->offset]);
                } else {
                    it->entities = &it->entities[offset];
                }
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
    return true;

done:
    /* Cleanup iterator resources if it wasn't yet depleted */
    ecs_iter_fini(chain_it);

depleted:
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
        "invalid worker index %d", index);
    ecs_check(index < count, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t result = *it;
    result.priv_.stack_cursor = NULL; /* Don't copy allocator cursor */
    
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

bool ecs_worker_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->chain_it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_worker_next, ECS_INVALID_PARAMETER, NULL);

    ecs_iter_t *chain_it = it->chain_it;
    ecs_worker_iter_t *iter = &it->priv_.iter.worker;
    int32_t res_count = iter->count, res_index = iter->index;
    int32_t per_worker, first;

    do {
        if (!ecs_iter_next(chain_it)) {
            return false;
        }

        /* Copy everything up to the private iterator data */
        ecs_os_memcpy(it, chain_it, offsetof(ecs_iter_t, priv_));

        int32_t count = it->count;
        per_worker = count / res_count;
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

    it->frame_offset += first;
    it->count = per_worker;
    it->offset += first;

    if (it->table) {
        it->entities = &(ecs_table_entities(it->table)[it->offset]);
    } else {
        it->entities = &it->entities[first];
    }

    return true;
error:
    return false;
}
