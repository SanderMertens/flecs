/**
 * @file table_data.c
 * @brief Table data implementation.
 */

#include "../private_api.h"

/* Construct components */
static
void flecs_table_data_invoke_ctor(
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_xtor_t ctor = ti->hooks.ctor;
    if (ctor) {
        void *ptr = ecs_vec_get(&column->data, column->size, row);
        ctor(ptr, count, ti);
    }
}

/* Destruct components */
static
void flecs_table_data_invoke_dtor(
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        void *ptr = ecs_vec_get(&column->data, column->size, row);
        dtor(ptr, count, ti);
    }
}

/* Invoke type hook for entities in table */
static
void flecs_table_data_invoke_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_iter_action_t callback,
    ecs_entity_t event,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count)
{
    void *ptr = ecs_vec_get(&column->data, column->size, row);
    flecs_invoke_hook(world, table, count, row, entities, ptr, column->id, 
        column->ti, event, callback);
}

/* Run hooks that get invoked when component is added to entity */
static
void flecs_table_data_invoke_add_hooks(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count,
    bool construct)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    if (construct) {
        flecs_table_data_invoke_ctor(column, row, count);
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (on_add) {
        flecs_table_data_invoke_hook(world, table, on_add, EcsOnAdd, column,
            entities, row, count);
    }
}

/* Run hooks that get invoked when component is removed from entity */
static
void flecs_table_data_invoke_remove_hooks(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count,
    bool dtor)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_iter_action_t on_remove = ti->hooks.on_remove;
    if (on_remove) {
        flecs_table_data_invoke_hook(world, table, on_remove, EcsOnRemove, 
            column, entities, row, count);
    }
    
    if (dtor) {
        flecs_table_data_invoke_dtor(column, row, count);
    }
}

/* Mark table column dirty. This usually happens as the result of a set 
 * operation, or iteration of a query with [out] fields. */
static
void flecs_table_data_mark_table_dirty(
    ecs_table_data_t *data,
    int32_t index)
{
    if (data->dirty_state) {
        data->dirty_state[index] ++;
    }
}

/* Set flags for type hooks so table operations can quickly check whether a
 * fast or complex operation that invokes hooks is required. */
static
ecs_flags32_t flecs_type_info_flags(
    const ecs_type_info_t *ti) 
{
    ecs_flags32_t flags = 0;

    if (ti->hooks.ctor) {
        flags |= EcsTableHasCtors;
    }
    if (ti->hooks.on_add) {
        flags |= EcsTableHasCtors;
    }
    if (ti->hooks.dtor) {
        flags |= EcsTableHasDtors;
    }
    if (ti->hooks.on_remove) {
        flags |= EcsTableHasDtors;
    }
    if (ti->hooks.copy) {
        flags |= EcsTableHasCopy;
    }
    if (ti->hooks.move) {
        flags |= EcsTableHasMove;
    }  

    return flags;  
}

static
void flecs_table_data_init_flags(
    ecs_table_data_t *data,
    ecs_id_t *ids,
    int32_t count)
{
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_HAS_ID_FLAG(id, TOGGLE)) {
            if (!(data->flags & EcsTableHasToggle)) {
                data->bs_offset = flecs_ito(int16_t, i);
            }
            data->flags |= EcsTableHasToggle;
        }
    }
}

static
void flecs_table_data_init_columns(
    ecs_world_t *world,
    ecs_table_data_t *data,
    ecs_id_t *ids,
    int32_t column_count)
{
    if (!column_count) {
        return;
    }

    int32_t i, cur = 0;
    ecs_column_t *columns = flecs_wcalloc_n(world, ecs_column_t, column_count);
    data->columns = columns;

    for (i = 0; i < column_count; i ++) {
        ecs_id_record_t *idr = flecs_id_record_get(world, ids[i]);
        const ecs_type_info_t *ti = idr->type_info;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        columns[cur].ti = ECS_CONST_CAST(ecs_type_info_t*, ti);
        columns[cur].id = ids[i];
        columns[cur].size = ti->size;

#ifdef FLECS_DEBUG
        ecs_vec_init(NULL, &columns[cur].data, ti->size, 0);
#endif

        data->flags |= flecs_type_info_flags(ti);
        cur ++;
    }
}

/* Initialize table storage */
static
ecs_table_data_t* flecs_table_data_init(
    ecs_world_t *world,
    ecs_id_t *ids,
    int32_t column_count)
{
    ecs_table_data_t *result = ecs_os_calloc_t(ecs_table_data_t);
    result->column_count = flecs_ito(int16_t, column_count);
    flecs_table_data_init_flags(result, ids, column_count);
    ecs_vec_init_t(&world->allocator, &result->entities, ecs_entity_t, 0);

    flecs_table_data_init_columns(world, result, ids, column_count);

    if (result->flags & EcsTableHasToggle) {
        int32_t i, bs_count = 0;
        for (i = result->bs_offset; i < column_count; i ++) {
            if (ECS_HAS_ID_FLAG(ids[i], TOGGLE)) {
                bs_count ++;
            }
        }

        ecs_assert(bs_count > 0, ECS_INTERNAL_ERROR, NULL);

        result->bitsets = flecs_wcalloc_n(world, ecs_bitset_column_t, bs_count);
        for (i = 0; i < bs_count; i ++) {
            flecs_bitset_init(&result->bitsets[i].data);
        }

        result->bs_count = flecs_ito(int16_t, bs_count);
    }

    result->refcount = 1;

    return result;
}

ecs_table_data_t* flecs_table_data_for_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_count)
{
    ecs_table_data_t *result = NULL;
    ecs_id_t *ids = NULL;
    ecs_vec_t idsvec = {0};
    bool table_ids = false;

    if (table->type.count == column_count) {
        ids = table->type.array;
        table_ids = true;
    } else {
        ecs_vec_init_t(NULL, &idsvec, ecs_id_t, column_count);

        int32_t i = 0;
        for (i = 0; i < table->type.count; i ++) {
            ecs_id_record_t *idr = flecs_table_idr_for(table, i);
            if (idr->type_info == NULL) {
                continue;
            }

            ecs_id_t *id = ecs_vec_append_t(NULL, &idsvec, ecs_id_t);
            *id = table->type.array[i];
        }

        ecs_assert(ecs_vec_count(&idsvec) == column_count, 
            ECS_INTERNAL_ERROR, NULL);

        ids = ecs_vec_first(&idsvec);
    }

    ecs_type_t type = {
        .count = column_count,
        .array = ids
    };

    flecs_hashmap_result_t hmr = flecs_hashmap_ensure(
        &world->store.table_data_map, &type, ecs_table_data_t*);

    if (!(result = *(ecs_table_data_t**)hmr.value)) {
        result = flecs_table_data_init(world, ids, column_count);
        *(ecs_table_data_t**)hmr.value = result;
        result->hash = hmr.hash;
        if (table_ids) {
            result->ids = flecs_wdup_n(world, ecs_id_t, column_count, ids);
            *(ecs_id_t**)hmr.key = result->ids;
        } else {
            result->ids = ids;
        }
    } else {
        result->refcount ++;
        ecs_vec_fini_t(NULL, &idsvec, ecs_id_t);
    }

    return result;
}

void flecs_table_data_release(
    ecs_world_t *world,
    ecs_table_data_t *data)
{
    ecs_assert(data->refcount > 0, ECS_INTERNAL_ERROR, NULL);
    if (!--data->refcount) {
        ecs_assert(ecs_vec_count(&data->entities) == 0, 
            ECS_INTERNAL_ERROR, NULL);
        ecs_allocator_t *a = &world->allocator;
        ecs_vec_fini_t(a, &data->entities, ecs_entity_t);

        int32_t i, column_count = data->column_count;
        for (i = 0; i < column_count; i ++) {
            ecs_column_t *column = &data->columns[i];
            ecs_vec_fini(a, &column->data, column->size);
        }

        int32_t bs_count = data->bs_count;
        for (i = 0; i < bs_count; i ++) {
            ecs_bitset_column_t *bs = &data->bitsets[i];
            flecs_bitset_fini(&bs->data);
        }

        flecs_wfree_n(world, ecs_column_t, column_count, data->columns);
        flecs_wfree_n(world, ecs_bitset_column_t, bs_count, data->bitsets);
        flecs_wfree_n(world, int32_t, column_count + 1, data->dirty_state);

        ecs_type_t type = {
            .array = data->ids,
            .count = column_count
        };

        flecs_hashmap_remove_w_hash(
            &world->store.table_data_map, &type, ecs_table_data_t*, data->hash);
        flecs_wfree_n(world, ecs_id_t, column_count, data->ids);

        ecs_os_free(data);
    }
}

/* Destruct all components and/or delete all entities in table in range */
static
void flecs_table_data_dtor_all(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    int32_t count,
    bool update_entity_index,
    bool is_delete)
{
    /* Can't delete and not update the entity index */
    ecs_assert(!is_delete || update_entity_index, ECS_INTERNAL_ERROR, NULL);

    ecs_table_data_t *data = flecs_table_data(table);
    int32_t ids_count = data->column_count;
    ecs_entity_t *entities = data->entities.array;
    int32_t i, c, end = row + count;

    if (is_delete && table->_->traversable_count) {
        /* If table contains monitored entities with traversable relationships,
         * make sure to invalidate observer cache */
        flecs_emit_propagate_invalidate(world, table, row, count);
    }

    /* If table has components with destructors, iterate component columns */
    if (table->flags & EcsTableHasDtors) {
        /* Throw up a lock just to be sure */
        table->_->lock = true;

        /* Run on_remove callbacks first before destructing components */
        for (c = 0; c < ids_count; c++) {
            ecs_column_t *column = &data->columns[c];
            ecs_iter_action_t on_remove = column->ti->hooks.on_remove;
            if (on_remove) {
                flecs_table_data_invoke_hook(world, table, on_remove, EcsOnRemove, 
                    column, &entities[row], row, count);
            }
        }

        /* Destruct components */
        for (c = 0; c < ids_count; c++) {
            flecs_table_data_invoke_dtor(&data->columns[c], row, count);
        }

        /* Iterate entities first, then components. This ensures that only one
         * entity is invalidated at a time, which ensures that destructors can
         * safely access other entities. */
        for (i = row; i < end; i ++) {
            /* Update entity index after invoking destructors so that entity can
             * be safely used in destructor callbacks. */
            if (update_entity_index) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), 
                    ECS_INTERNAL_ERROR, NULL);

                if (is_delete) {
                    flecs_entities_remove(world, e);
                    ecs_assert(ecs_is_valid(world, e) == false, 
                        ECS_INTERNAL_ERROR, NULL);
                } else {
                    // If this is not a delete, clear the entity index record
                    ecs_record_t *record = flecs_entities_get(world, e);
                    record->table = NULL;
                    record->row = 0;
                }
            } else {
                /* This should only happen in rare cases, such as when the data
                 * cleaned up is not part of the world (like with snapshots) */
            }
        }

        table->_->lock = false;

    /* If table does not have destructors, just update entity index */
    } else if (update_entity_index) {
        if (is_delete) {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                flecs_entities_remove(world, e);
                ecs_assert(!ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
            } 
        } else {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                ecs_record_t *record = flecs_entities_get(world, e);
                record->table = NULL;
                record->row = record->row & ECS_ROW_FLAGS_MASK;
                (void)e;
            }
        }      
    }
}

/* Free table storage */
void flecs_table_data_fini(
    ecs_world_t *world,
    ecs_table_t *table,
    bool do_on_remove,
    bool update_entity_index,
    bool is_delete,
    bool deactivate)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_data_t *data = table->data;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_table_count(table);
    if (count) {
        if (do_on_remove) {
            flecs_notify_on_remove(world, table, NULL, 0, count, &table->type);
        }

        flecs_table_data_dtor_all(world, table, 0, count, 
            update_entity_index, is_delete);
    }

    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = table->data->column_count;
        for (c = 0; c < column_count; c ++) {
            /* Sanity check */
            ecs_assert(columns[c].data.count == data->entities.count,
                ECS_INTERNAL_ERROR, NULL);
            ecs_vec_fini(&world->allocator,
                &columns[c].data, columns[c].size);
        }
        flecs_wfree_n(world, ecs_column_t, column_count, columns);
        data->columns = NULL;
    }

    ecs_bitset_column_t *bs_columns = data->bitsets;
    if (bs_columns) {
        int32_t c, column_count = data->bs_count;
        for (c = 0; c < column_count; c ++) {
            flecs_bitset_fini(&bs_columns[c].data);
        }
        flecs_wfree_n(world, ecs_bitset_t, column_count, bs_columns);
        data->bitsets = NULL;
    }

    ecs_vec_fini_t(&world->allocator, &data->entities, ecs_entity_t);
}

/* Append operation for tables that don't have any complex logic */
static
void flecs_table_data_fast_append(
    ecs_world_t *world,
    ecs_column_t *columns,
    int32_t count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_vec_append(&world->allocator, &column->data, column->size);
    }
}

/* Grow table column. When a column needs to be reallocated this function takes
 * care of correctly invoking ctor/move/dtor hooks. */
static
void* flecs_table_data_column_append(
    ecs_world_t *world,
    ecs_column_t *column,
    int32_t to_add,
    int32_t dst_size,
    bool construct)
{
    ecs_assert(column != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_info_t *ti = column->ti;
    int32_t size = column->size;
    int32_t count = column->data.count;
    int32_t src_size = column->data.size;
    int32_t dst_count = count + to_add;
    bool can_realloc = dst_size != src_size;
    void *result = NULL;

    ecs_assert(dst_size >= dst_count, ECS_INTERNAL_ERROR, NULL);

    /* If the array could possibly realloc and the component has a move action 
     * defined, move old elements manually */
    ecs_move_t move_ctor;
    if (count && can_realloc && (move_ctor = ti->hooks.ctor_move_dtor)) {
        ecs_xtor_t ctor = ti->hooks.ctor;
        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(move_ctor != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create  vector */
        ecs_vec_t dst;
        ecs_vec_init(&world->allocator, &dst, size, dst_size);
        dst.count = dst_count;

        void *src_buffer = column->data.array;
        void *dst_buffer = dst.array;

        /* Move (and construct) existing elements to new vector */
        move_ctor(dst_buffer, src_buffer, count, ti);

        if (construct) {
            /* Construct new element(s) */
            result = ECS_ELEM(dst_buffer, size, count);
            ctor(result, to_add, ti);
        }

        /* Free old vector */
        ecs_vec_fini(&world->allocator, &column->data, size);

        column->data = dst;
    } else {
        /* If array won't realloc or has no move, simply add new elements */
        if (can_realloc) {
            ecs_vec_set_size(&world->allocator, &column->data, size, dst_size);
        }

        result = ecs_vec_grow(&world->allocator, &column->data, size, to_add);

        ecs_xtor_t ctor;
        if (construct && (ctor = ti->hooks.ctor)) {
            /* If new elements need to be constructed and component has a
             * constructor, construct */
            ctor(result, to_add, ti);
        }
    }

    ecs_assert(column->data.size == dst_size, ECS_INTERNAL_ERROR, NULL);

    return result;
}

/* Append entity to table data */
int32_t flecs_table_data_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    bool construct,
    bool on_add)
{
    ecs_table_data_t *data = table->data;

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    int32_t count = data->entities.count;
    int32_t column_count = data->column_count;
    ecs_column_t *columns = data->columns;

    /* Grow buffer with entity ids, set new element to new entity */
    ecs_entity_t *e = ecs_vec_append_t(&world->allocator, 
        &data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;

    /* If the table is monitored indicate that there has been a change */
    flecs_table_data_mark_table_dirty(data, 0);
    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(data->flags & EcsTableIsComplex)) {
        flecs_table_data_fast_append(world, columns, column_count);
        return count;
    }

    ecs_entity_t *entities = data->entities.array;

    /* Reobtain size to ensure that the columns have the same size as the 
     * entities and record vectors. This keeps reasoning about when allocations
     * occur easier. */
    int32_t size = data->entities.size;

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        flecs_table_data_column_append(world, column, 1, size, construct);

        ecs_iter_action_t on_add_hook;
        if (on_add && (on_add_hook = column->ti->hooks.on_add)) {
            flecs_table_data_invoke_hook(world, table, on_add_hook, EcsOnAdd, 
                column, &entities[count], count, 1);
        }

        ecs_assert(columns[i].data.size == 
            data->entities.size, ECS_INTERNAL_ERROR, NULL); 
        ecs_assert(columns[i].data.count == 
            data->entities.count, ECS_INTERNAL_ERROR, NULL);
    }

    int32_t bs_count = data->bs_count;
    ecs_bitset_column_t *bitsets = data->bitsets;

    /* Add element to each bitset column */
    for (i = 0; i < bs_count; i ++) {
        ecs_assert(bitsets != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_bitset_t *bs = &bitsets[i].data;
        flecs_bitset_addn(bs, 1);
    }

    return count;
}

/* Grow all data structures in a table */
int32_t flecs_table_data_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t to_add,
    const ecs_entity_t *ids)
{
    ecs_table_data_t *data = table->data;

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur_count = data->entities.count;
    int32_t column_count = data->column_count;
    int32_t size = to_add + cur_count;
    ecs_allocator_t *a = &world->allocator;

    /* Add entity to column with entity ids */
    ecs_vec_set_size_t(a, &data->entities, ecs_entity_t, size);
    ecs_entity_t *e = ecs_vec_last_t(&data->entities, ecs_entity_t) + 1;
    data->entities.count += to_add;
    if (data->entities.size > size) {
        size = data->entities.size;
    }

    /* Initialize entity ids and record ptrs */
    int32_t i;
    if (ids) {
        ecs_os_memcpy_n(e, ids, ecs_entity_t, to_add);
    } else {
        ecs_os_memset(e, 0, ECS_SIZEOF(ecs_entity_t) * to_add);
    }

    /* Add elements to each column array */
    ecs_column_t *columns = data->columns;
    for (i = 0; i < column_count; i ++) {
        flecs_table_data_column_append(world, &columns[i], to_add, size, true);
        ecs_assert(columns[i].data.size == size, ECS_INTERNAL_ERROR, NULL);
        flecs_table_data_invoke_add_hooks(world, table, &columns[i], e, 
            cur_count, to_add, false);
    }

    int32_t bs_count = data->bs_count;
    ecs_bitset_column_t *bitsets = data->bitsets;

    /* Add elements to each bitset column */
    for (i = 0; i < bs_count; i ++) {
        ecs_assert(bitsets != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_bitset_t *bs = &bitsets[i].data;
        flecs_bitset_addn(bs, to_add);
    }

    /* If the table is monitored indicate that there has been a change */
    flecs_table_data_mark_table_dirty(data, 0);

    /* Return index of first added entity */
    return cur_count;
}

/* Move operation for tables that don't have any complex logic */
static
void flecs_table_fast_move(
    ecs_table_data_t *dst_data,
    int32_t dst_index,
    ecs_table_data_t *src_data,
    int32_t src_index)
{
    int32_t i_dst = 0, dst_column_count = dst_data->column_count;
    int32_t i_src = 0, src_column_count = src_data->column_count;

    ecs_column_t *dst_columns = dst_data->columns;
    ecs_column_t *src_columns = src_data->columns;

    for (; (i_dst < dst_column_count) && (i_src < src_column_count);) {
        ecs_column_t *dst_column = &dst_columns[i_dst];
        ecs_column_t *src_column = &src_columns[i_src];
        ecs_id_t dst_id = dst_column->id;
        ecs_id_t src_id = src_column->id;

        if (dst_id == src_id) {
            int32_t size = dst_column->size;
            void *dst = ecs_vec_get(&dst_column->data, size, dst_index);
            void *src = ecs_vec_get(&src_column->data, size, src_index);
            ecs_os_memcpy(dst, src, size);
        }

        i_dst += dst_id <= src_id;
        i_src += dst_id >= src_id;
    }
}

/* Table move logic for bitset (toggle component) column */
static
void flecs_table_data_move_bitset_columns(
    ecs_table_t *dst_table, 
    int32_t dst_index,
    ecs_table_t *src_table, 
    int32_t src_index,
    int32_t count,
    bool clear)
{
    ecs_table_data_t *dst_data = dst_table->data;
    ecs_table_data_t *src_data = src_table->data;

    int32_t i_dst = 0, dst_column_count = dst_data->bs_count;
    int32_t i_src = 0, src_column_count = src_data->bs_count;

    if (!src_column_count && !dst_column_count) {
        return;
    }

    ecs_bitset_column_t *src_columns = src_data->bitsets;
    ecs_bitset_column_t *dst_columns = dst_data->bitsets;

    for (; (i_dst < dst_column_count) && (i_src < src_column_count);) {
        ecs_bitset_column_t *dst_column = &dst_columns[i_dst];
        ecs_bitset_column_t *src_column = &src_columns[i_src];
        ecs_id_t dst_id = dst_column->id, src_id = src_column->id;

        if (dst_id == src_id) {
            ecs_bitset_t *src_bs = &src_columns[i_src].data;
            ecs_bitset_t *dst_bs = &dst_columns[i_dst].data;

            flecs_bitset_ensure(dst_bs, dst_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_bitset_get(src_bs, src_index + i);
                flecs_bitset_set(dst_bs, dst_index + i, value);
            }

            if (clear) {
                ecs_assert(count == flecs_bitset_count(src_bs), 
                    ECS_INTERNAL_ERROR, NULL);
                flecs_bitset_fini(src_bs);
            }
        } else if (dst_id > src_id) {
            ecs_bitset_t *src_bs = &src_columns[i_src].data;
            flecs_bitset_fini(src_bs);
        }

        i_dst += dst_id <= src_id;
        i_src += dst_id >= src_id;
    }

    /* Clear remaining columns */
    if (clear) {
        for (; (i_src < src_column_count); i_src ++) {
            ecs_bitset_t *src_bs = &src_columns[i_src].data;
            ecs_assert(count == flecs_bitset_count(src_bs), 
                ECS_INTERNAL_ERROR, NULL);
            flecs_bitset_fini(src_bs);
        }
    }
}

/* Move entity from src to dst table */
void flecs_table_data_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *dst_table,
    int32_t dst_index,
    ecs_table_t *src_table,
    int32_t src_index,
    bool construct)
{
    ecs_table_data_t *dst_data = dst_table->data;
    ecs_table_data_t *src_data = src_table->data;
    ecs_assert(dst_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!((dst_data->flags | src_data->flags) & EcsTableIsComplex)) {
        flecs_table_fast_move(dst_data, dst_index, src_data, src_index);
        return;
    }

    flecs_table_data_move_bitset_columns(
        dst_table, dst_index, src_table, src_index, 1, false);

    /* If the source and destination entities are the same, move component 
     * between tables. If the entities are not the same (like when cloning) use
     * a copy. */
    bool same_entity = dst_entity == src_entity;

    /* Call move_dtor for moved away from storage only if the entity is at the
     * last index in the source table. If it isn't the last entity, the last 
     * entity in the table will be moved to the src storage, which will take
     * care of cleaning up resources. */
    bool use_move_dtor = ecs_table_count(src_table) == (src_index + 1);

    int32_t i_dst = 0, dst_column_count = dst_data->column_count;
    int32_t i_src = 0, src_column_count = src_data->column_count;

    ecs_column_t *dst_columns = dst_data->columns;
    ecs_column_t *src_columns = src_data->columns;

    for (; (i_dst < dst_column_count) && (i_src < src_column_count); ) {
        ecs_column_t *dst_column = &dst_columns[i_dst];
        ecs_column_t *src_column = &src_columns[i_src];
        ecs_id_t dst_id = dst_column->id;
        ecs_id_t src_id = src_column->id;

        if (dst_id == src_id) {
            int32_t size = dst_column->size;

            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
            void *dst = ecs_vec_get(&dst_column->data, size, dst_index);
            void *src = ecs_vec_get(&src_column->data, size, src_index);
            ecs_type_info_t *ti = dst_column->ti;

            if (same_entity) {
                ecs_move_t move = ti->hooks.move_ctor;
                if (use_move_dtor || !move) {
                    /* Also use move_dtor if component doesn't have a move_ctor
                     * registered, to ensure that the dtor gets called to 
                     * cleanup resources. */
                    move = ti->hooks.ctor_move_dtor;
                }

                if (move) {
                    move(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            } else {
                ecs_copy_t copy = ti->hooks.copy_ctor;
                if (copy) {
                    copy(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            }
        } else {
            if (dst_id < src_id) {
                flecs_table_data_invoke_add_hooks(world, dst_table,
                    dst_column, &dst_entity, dst_index, 1, construct);
            } else {
                flecs_table_data_invoke_remove_hooks(world, src_table,
                    src_column, &src_entity, src_index, 1, use_move_dtor);
            }
        }

        i_dst += dst_id <= src_id;
        i_src += dst_id >= src_id;
    }

    for (; (i_dst < dst_column_count); i_dst ++) {
        flecs_table_data_invoke_add_hooks(world, dst_table, &dst_columns[i_dst], 
            &dst_entity, dst_index, 1, construct);
    }

    for (; (i_src < src_column_count); i_src ++) {
        flecs_table_data_invoke_remove_hooks(world, src_table, &src_columns[i_src], 
            &src_entity, src_index, 1, use_move_dtor);
    }
}


/* Delete last operation for tables that don't have any complex logic */
static
void flecs_table_data_fast_delete_last(
    ecs_column_t *columns,
    int32_t column_count) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_vec_remove_last(&columns[i].data);
    }
}

/* Delete operation for tables that don't have any complex logic */
static
void flecs_table_data_fast_delete(
    ecs_column_t *columns,
    int32_t column_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_vec_remove(&column->data, column->size, index);
    }
}

/* Delete entity from table */
int32_t flecs_table_data_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool destruct)
{
    ecs_table_data_t *data = table->data;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t count = data->entities.count;

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    /* Move last entity id to index */
    ecs_entity_t *entities = data->entities.array;
    ecs_entity_t entity_to_move = entities[count];
    ecs_entity_t entity_to_delete = entities[index];
    entities[index] = entity_to_move;
    ecs_vec_remove_last(&data->entities);

    /* Update row of moved entity */
    if (index != count) {
        ecs_record_t *record = flecs_entities_get(world, entity_to_move);
        if (record) {
            int32_t row = record->row & ECS_ROW_MASK;
            ecs_assert(record->table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_vec_get_t(&record->table->rows, int32_t, row)[0] = index;
        }
    }     

    /* If the table is monitored indicate that there has been a change */
    flecs_table_data_mark_table_dirty(data, 0);

    /* Destruct component data */
    ecs_column_t *columns = data->columns;
    int32_t column_count = data->column_count;
    int32_t i;

    /* If this is a table without lifecycle callbacks or special columns, take
     * fast path that just remove an element from the array(s) */
    if (!(data->flags & EcsTableIsComplex)) {
        if (index == count) {
            flecs_table_data_fast_delete_last(columns, column_count);
        } else {
            flecs_table_data_fast_delete(columns, column_count, index);
        }
        return count;
    }

    /* Last element, destruct & remove */
    if (index == count) {
        /* If table has component destructors, invoke */
        if (destruct && (data->flags & EcsTableHasDtors)) {            
            for (i = 0; i < column_count; i ++) {
                flecs_table_data_invoke_remove_hooks(world, table, &columns[i], 
                    &entity_to_delete, index, 1, true);
            }
        }

        flecs_table_data_fast_delete_last(columns, column_count);

    /* Not last element, move last element to deleted element & destruct */
    } else {
        /* If table has component destructors, invoke */
        if ((data->flags & (EcsTableHasDtors | EcsTableHasMove))) {
            for (i = 0; i < column_count; i ++) {
                ecs_column_t *column = &columns[i];
                ecs_type_info_t *ti = column->ti;
                ecs_size_t size = column->size;
                void *dst = ecs_vec_get(&column->data, size, index);
                void *src = ecs_vec_last(&column->data, size);

                ecs_iter_action_t on_remove = ti->hooks.on_remove;
                if (destruct && on_remove) {
                    flecs_table_data_invoke_hook(world, table, on_remove, EcsOnRemove,
                        column, &entity_to_delete, index, 1);
                }

                ecs_move_t move_dtor = ti->hooks.move_dtor;
                if (move_dtor) {
                    move_dtor(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }

                ecs_vec_remove_last(&column->data);
            }
        } else {
            flecs_table_data_fast_delete(columns, column_count, index);
        }
    }

    /* Remove elements from bitset columns */
    ecs_bitset_column_t *bitsets = data->bitsets;
    int32_t bs_count = data->bs_count;
    for (i = 0; i < bs_count; i ++) {
        ecs_assert(bitsets != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_bitset_remove(&bitsets[i].data, index);
    }

    return count;
}

/* Swap operation for bitset (toggle component) columns */
static
void flecs_table_data_swap_bitset_columns(
    ecs_table_data_t *data,
    int32_t row_1,
    int32_t row_2)
{
    int32_t i = 0, column_count = data->bs_count;
    if (!column_count) {
        return;
    }

    ecs_bitset_column_t *columns = data->bitsets;
    for (i = 0; i < column_count; i ++) {
        ecs_bitset_t *bs = &columns[i].data;
        flecs_bitset_swap(bs, row_1, row_2);
    }
}

/* Swap two rows in a table. Used for table sorting. */
void flecs_table_data_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2)
{    
    if (row_1 == row_2) {
        return;
    }

    ecs_table_data_t *data = table->data;

    /* If the table is monitored indicate that there has been a change */
    flecs_table_data_mark_table_dirty(data, 0);    

    ecs_entity_t *entities = data->entities.array;
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t *record_ptr_1 = flecs_entities_get(world, e1);
    ecs_record_t *record_ptr_2 = flecs_entities_get(world, e2);

    ecs_assert(record_ptr_1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record_ptr_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Keep track of whether entity is watched */
    uint32_t flags_1 = ECS_RECORD_TO_ROW_FLAGS(record_ptr_1->row);
    uint32_t flags_2 = ECS_RECORD_TO_ROW_FLAGS(record_ptr_2->row);

    /* Swap entities & records */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = ECS_ROW_TO_RECORD(row_2, flags_1);
    record_ptr_2->row = ECS_ROW_TO_RECORD(row_1, flags_2);

    flecs_table_data_swap_bitset_columns(data, row_1, row_2);

    ecs_column_t *columns = data->columns;

    /* Find the maximum size of column elements
     * and allocate a temporary buffer for swapping */
    int32_t i, temp_buffer_size = ECS_SIZEOF(uint64_t);
    int32_t column_count = table->data->column_count;
    for (i = 0; i < column_count; i++) {
        temp_buffer_size = ECS_MAX(temp_buffer_size, columns[i].size);
    }

    void* tmp = ecs_os_alloca(temp_buffer_size);

    /* Swap columns */
    for (i = 0; i < column_count; i ++) {
        int32_t size = columns[i].size;
        void *ptr = columns[i].data.array;

        void *el_1 = ECS_ELEM(ptr, size, row_1);
        void *el_2 = ECS_ELEM(ptr, size, row_2);

        ecs_os_memcpy(tmp, el_1, size);
        ecs_os_memcpy(el_1, el_2, size);
        ecs_os_memcpy(el_2, tmp, size);
    }
}

/* Merge data from one table column into other table column */
static
void flecs_table_data_merge_column(
    ecs_world_t *world,
    ecs_column_t *dst,
    ecs_column_t *src,
    int32_t column_size)
{
    ecs_size_t size = dst->size;
    int32_t dst_count = dst->data.count;

    if (!dst_count) {
        ecs_vec_fini(&world->allocator, &dst->data, size);
        *dst = *src;
        src->data.array = NULL;
        src->data.count = 0;
        src->data.size = 0;

    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = src->data.count;

        flecs_table_data_column_append(world, dst, src_count, column_size, true);
        void *dst_ptr = ECS_ELEM(dst->data.array, size, dst_count);
        void *src_ptr = src->data.array;

        /* Move values into column */
        ecs_type_info_t *ti = dst->ti;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_move_t move = ti->hooks.move_dtor;
        if (move) {
            move(dst_ptr, src_ptr, src_count, ti);
        } else {
            ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);
        }

        ecs_vec_fini(&world->allocator, &src->data, size);
    }
}

/* Merge storage of two tables. */
static
void flecs_table_data_merge_columns(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table,
    int32_t src_count,
    int32_t dst_count,
    ecs_table_data_t *src_data,
    ecs_table_data_t *dst_data)
{
    int32_t i_new = 0, dst_column_count = dst_table->data->column_count;
    int32_t i_old = 0, src_column_count = src_table->data->column_count;
    ecs_column_t *src_columns = src_data->columns;
    ecs_column_t *dst_columns = dst_data->columns;

    ecs_assert(!dst_column_count || dst_columns, ECS_INTERNAL_ERROR, NULL);

    if (!src_count) {
        return;
    }

    /* Merge entities & records vectors */
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_merge_t(a, &dst_data->entities, &src_data->entities, ecs_entity_t);
    ecs_assert(dst_data->entities.count == src_count + dst_count, 
        ECS_INTERNAL_ERROR, NULL);

    int32_t column_size = dst_data->entities.size;
    for (; (i_new < dst_column_count) && (i_old < src_column_count); ) {
        ecs_column_t *dst_column = &dst_columns[i_new];
        ecs_column_t *src_column = &src_columns[i_old];
        ecs_id_t dst_id = dst_column->id;
        ecs_id_t src_id = src_column->id;
    
        if (dst_id == src_id) {
            flecs_table_data_merge_column(world, dst_column, src_column, column_size);
            flecs_table_data_mark_table_dirty(dst_data, i_new + 1);
            i_new ++;
            i_old ++;
        } else if (dst_id < src_id) {
            /* New column, make sure vector is large enough. */
            ecs_size_t size = dst_column->size;
            ecs_vec_set_size(a, &dst_column->data, size, column_size);
            ecs_vec_set_count(a, &dst_column->data, size, src_count + dst_count);
            flecs_table_data_invoke_ctor(dst_column, dst_count, src_count);
            i_new ++;
        } else if (dst_id > src_id) {
            /* Old column does not occur in new table, destruct */
            flecs_table_data_invoke_dtor(src_column, 0, src_count);
            ecs_vec_fini(a, &src_column->data, src_column->size);
            i_old ++;
        }
    }

    flecs_table_data_move_bitset_columns(
        dst_table, dst_count, src_table, 0, src_count, true);

    /* Initialize remaining columns */
    for (; i_new < dst_column_count; i_new ++) {
        ecs_column_t *column = &dst_columns[i_new];
        int32_t size = column->size;
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_set_size(a, &column->data, size, column_size);
        ecs_vec_set_count(a, &column->data, size, src_count + dst_count);
        flecs_table_data_invoke_ctor(column, dst_count, src_count);
    }

    /* Destruct remaining columns */
    for (; i_old < src_column_count; i_old ++) {
        ecs_column_t *column = &src_columns[i_old];
        flecs_table_data_invoke_dtor(column, 0, src_count);
        ecs_vec_fini(a, &column->data, column->size);
    }    

    /* Mark entity column as dirty */
    flecs_table_data_mark_table_dirty(dst_data, 0); 
}

/* Merge source table into destination table. This typically happens as result
 * of a bulk operation, like when a component is removed from all entities in 
 * the source table (like for the Remove OnDelete policy). */
void flecs_table_data_merge(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table)
{
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!src_table->_->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_table_data_t *dst_data = dst_table->data;
    ecs_table_data_t *src_data = src_table->data;
    ecs_assert(dst_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);

    bool move_data = false;

    ecs_entity_t *src_entities = src_data->entities.array;
    int32_t src_count = src_data->entities.count;
    int32_t dst_count = dst_data->entities.count;

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < src_count; i ++) {
        ecs_record_t *record = flecs_entities_ensure(world, src_entities[i]);
        uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
        record->row = ECS_ROW_TO_RECORD(dst_count + i, flags);
        record->table = dst_table;
    }

    /* Merge table columns */
    if (move_data) {
        *dst_data = *src_data;
    } else {
        flecs_table_data_merge_columns(world, dst_table, src_table, 
            src_count, dst_count, src_data, dst_data);
    }
}

/* Shrink table storage to fit number of entities */
bool flecs_table_data_shrink(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_table_data_t *data = table->data;
    bool has_payload = data->entities.array != NULL;
    ecs_vec_reclaim_t(&world->allocator, &data->entities, ecs_entity_t);

    int32_t i, count = data->column_count;
    for (i = 0; i < count; i ++) {
        ecs_column_t *column = &data->columns[i];
        ecs_vec_reclaim(&world->allocator, &column->data, column->size);
    }

    return has_payload;
}

/* Mark table component dirty */
void flecs_table_data_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_table_data_t *data = table->data;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (data->dirty_state) {
        ecs_id_record_t *idr = flecs_id_record_get(world, component);
        if (!idr) {
            return;
        }

        const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
        if (!tr || tr->column == -1) {
            return;
        }

        data->dirty_state[tr->column + 1] ++;
    }
}

/* Get (or create) dirty state of table. Used by queries for change tracking */
int32_t* flecs_table_data_get_dirty_state(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_table_data_t *data = table->data;
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!data->dirty_state) {
        int32_t column_count = table->data->column_count;
        data->dirty_state = flecs_alloc_n(&world->allocator,
             int32_t, column_count + 1);
        for (int i = 0; i < column_count + 1; i ++) {
            data->dirty_state[i] = 1;
        }
    }

    return data->dirty_state;
}
