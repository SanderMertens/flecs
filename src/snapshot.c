#include "flecs_private.h"

static
void dup_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    /* Store pointer to data in main stage */
    ecs_data_t *main_data = ecs_vector_first(table->stage_data);

    /* Obtain new data for the snapshot table  */
    table->stage_data = NULL;
    ecs_data_t *snapshot_data = ecs_table_get_data(world, table);

    int32_t c, column_count = ecs_vector_count(table->type);

    snapshot_data->columns = ecs_os_memdup(
        main_data->columns, sizeof(ecs_column_t) * column_count);

    /* Copy entities */
    snapshot_data->entities = ecs_vector_copy(main_data->entities, ecs_entity_t);

    /* Copy each column */
    for (c = 0; c < column_count; c ++) {
        ecs_column_t *column = &snapshot_data->columns[c];
        column->data = _ecs_vector_copy(column->data, column->size);
    }
}

static
ecs_snapshot_t* snapshot_create(
    ecs_world_t *world,
    const ecs_ei_t *entity_index,
    const ecs_sparse_t *tables,
    const ecs_filter_t *filter)
{
    ecs_snapshot_t *result = ecs_os_malloc(sizeof(ecs_snapshot_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    /* Copy tables from world */
    result->tables = ecs_sparse_copy(tables);
    
    if (filter || !entity_index) {
        result->filter = filter ? *filter : (ecs_filter_t){0};
        result->entity_index.hi = NULL;
        result->entity_index.lo = NULL;
    } else {
        result->filter = (ecs_filter_t){0};
        result->entity_index = ecs_ei_copy(entity_index);
    }

    /* We need to dup the table data, because right now the copied tables are
     * still pointing to columns in the main stage. */
    int32_t i, count = ecs_sparse_count(result->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(result->tables, ecs_table_t, i);

        /* Skip tables with builtin components to avoid dropping critical data
         * like systems or components when restoring the snapshot */
        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_data_t *data = ecs_table_get_data(world, table);

        /* Skip tables that are already filtered out */
        if (!data) {
            continue;
        }

        if (!filter || ecs_type_match_w_filter(world, table->type, filter)) {
            dup_table(world, table);
        } else {
            /* If the table does not match the filter, instead of copying just
             * set the data to NULL. This way the restore will ignore the
             * table. 
             * Note that this does not cause a memory leak, as at this point the
             * columns member still points to the live data. */
            table->stage_data = NULL;
        }
    }

    return result;
}

/** Create a snapshot */
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_snapshot_t *result = snapshot_create(
            world,
            &world->stage.entity_index,
            world->stage.tables,
            filter);

    result->last_handle = world->last_handle;

    return result;
}

/** Copy a snapshot */
ecs_snapshot_t* ecs_snapshot_copy(
    ecs_world_t *world,
    const ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter)
{
    ecs_snapshot_t *result = snapshot_create(
            world,
            &snapshot->entity_index,
            snapshot->tables,
            filter);

    if (!filter) {
        result->filter = snapshot->filter;
    }

    result->last_handle = snapshot->last_handle;

    return result;
}

/** Restore a snapshot */
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    ecs_filter_t filter = snapshot->filter;
    bool filter_used = false;

    /* If a filter was used, clear all data that matches the filter, except the
     * tables for which the snapshot has data */
    if (filter.include || filter.exclude) {
        ecs_clear_w_filter(world, &filter);
        filter_used = true;
    } else {
        /* If no filter was used, the entity index will be an exact copy of what
         * it was before taking the snapshot */
        ecs_ei_free(&world->stage.entity_index);
        world->stage.entity_index = snapshot->entity_index;
    }   

    /* Move snapshot data to table */
    int32_t i, count = ecs_sparse_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *src = ecs_sparse_get(snapshot->tables, ecs_table_t, i);
        if (src->flags & EcsTableHasBuiltins) {
            continue;
        }

        /* If table has no columns, it was filtered out and should not be
         * restored. */
        ecs_data_t *data = ecs_vector_first(src->stage_data);
        if (!data) {
            filter_used = true;
            continue;
        }

        ecs_table_t *dst = ecs_sparse_get(world->stage.tables, ecs_table_t, i);
        ecs_table_replace_data(world, dst, data);

        ecs_data_t *dst_data = ecs_table_get_data(world, dst);

        /* If a filter was used, we need to fix the entity index one by one */
        if (filter_used) {
            ecs_vector_t *entities = dst_data->entities;
            ecs_entity_t *array = ecs_vector_first(entities);
            int32_t j, row_count = ecs_vector_count(entities);
            ecs_ei_t *entity_index = &world->stage.entity_index;
            
            for (j = 0; j < row_count; j ++) {
                ecs_record_t record = {
                    .table = dst,
                    .row = j + 1
                };
                ecs_ei_set(entity_index, array[j], &record);
            } 
        }
    }

    /* Clear data from remaining tables */
    int32_t world_count = ecs_sparse_count(world->stage.tables);
    for (; i < world_count; i ++) {
        ecs_table_t *table = ecs_sparse_get(world->stage.tables, ecs_table_t, i);
        ecs_table_replace_data(world, table, NULL);
    }

    ecs_sparse_free(snapshot->tables);

    world->should_match = true;
    world->should_resolve = true;

    if (!filter_used) {
        world->last_handle = snapshot->last_handle;
    }

    ecs_os_free(snapshot);    
}

/** Cleanup snapshot */
void ecs_snapshot_free(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    ecs_ei_free(&snapshot->entity_index);

    int32_t i, count = ecs_sparse_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *src = ecs_sparse_get(snapshot->tables, ecs_table_t, i);
        if (src->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_table_replace_data(world, src, NULL);

        ecs_data_t *src_data = ecs_vector_first(src->stage_data);
        if (src_data) {
            ecs_os_free(src_data->columns);
            src_data->columns = NULL;
        }
    }    

    ecs_sparse_free(snapshot->tables);
    ecs_os_free(snapshot);
}
