#include "flecs_private.h"

static
void dup_table(
    ecs_table_t *table)
{
    uint32_t c, column_count = ecs_vector_count(table->type);

    /* First create a copy of columns structure */
    table->columns = ecs_os_memdup(
        table->columns, sizeof(ecs_table_column_t) * (column_count + 1));
    
    /* Now copy each column separately */
    for (c = 0; c < column_count + 1; c ++) {
        ecs_table_column_t *column = &table->columns[c];
        ecs_vector_params_t column_params = {.element_size = column->size};
        column->data = ecs_vector_copy(column->data, &column_params);
    }
}

static
ecs_snapshot_t* snapshot_create(
    ecs_world_t *world,
    const ecs_map_t *entity_index,
    const ecs_chunked_t *tables,
    const ecs_filter_t *filter)
{
    ecs_snapshot_t *result = ecs_os_malloc(sizeof(ecs_snapshot_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    /* Copy tables from world */
    result->tables = ecs_chunked_copy(tables);
    
    if (filter || !entity_index) {
        result->filter = filter ? *filter : (ecs_filter_t){0};
        result->entity_index = NULL;
    } else {
        result->filter = (ecs_filter_t){0};
        result->entity_index = ecs_map_copy(entity_index);
    }

    /* We need to dup the table data, because right now the copied tables are
     * still pointing to columns in the main stage. */
    uint32_t i, count = ecs_chunked_count(result->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(result->tables, ecs_table_t, i);

        /* Skip tables with builtin components to avoid dropping critical data
         * like systems or components when restoring the snapshot */
        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        /* Skip tables that are already filtered out */
        if (!table->columns) {
            continue;
        }

        if (!filter || ecs_type_match_w_filter(world, table->type, filter)) {
            dup_table(table);
        } else {
            /* If the table does not match the filter, instead of copying just
             * set the columns to NULL. This way the restore will ignore the
             * table. 
             * Note that this does not cause a memory leak, as at this point the
             * columns member still points to the live data. */
            table->columns = NULL;
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
            world->main_stage.entity_index,
            world->main_stage.tables,
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
            snapshot->entity_index,
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
        ecs_map_free(world->main_stage.entity_index);
        world->main_stage.entity_index = snapshot->entity_index;
    }   

    /* Move snapshot data to table */
    uint32_t i, count = ecs_chunked_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *src = ecs_chunked_get(snapshot->tables, ecs_table_t, i);
        if (src->flags & EcsTableHasBuiltins) {
            continue;
        }

        /* If table has no columns, it was filtered out and should not be
         * restored. */
        if (!src->columns) {
            filter_used = true;
            continue;
        }

        ecs_table_t *dst = ecs_chunked_get(world->main_stage.tables, ecs_table_t, i);
        ecs_table_replace_columns(world, dst, src->columns);

        /* If a filter was used, we need to fix the entity index one by one */
        if (filter_used) {
            ecs_vector_t *entities = dst->columns[0].data;
            ecs_entity_t *array = ecs_vector_first(entities);
            uint32_t j, row_count = ecs_vector_count(entities);
            ecs_map_t *entity_index = world->main_stage.entity_index;
            
            for (j = 0; j < row_count; j ++) {
                ecs_row_t row = {
                    .type = dst->type,
                    .index = j + 1
                };
                ecs_map_set(entity_index, array[j], &row);
            } 
        }
    }

    /* Clear data from remaining tables */
    uint32_t world_count = ecs_chunked_count(world->main_stage.tables);
    for (; i < world_count; i ++) {
        ecs_table_t *table = ecs_chunked_get(world->main_stage.tables, ecs_table_t, i);
        ecs_table_replace_columns(world, table, NULL);
    }

    ecs_chunked_free(snapshot->tables);

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
    if (snapshot->entity_index) {
        ecs_map_free(snapshot->entity_index);
    }

    uint32_t i, count = ecs_chunked_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *src = ecs_chunked_get(snapshot->tables, ecs_table_t, i);
        if (src->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_table_replace_columns(world, src, NULL);
        ecs_os_free(src->columns);
    }    

    ecs_chunked_free(snapshot->tables);
    ecs_os_free(snapshot);
}
