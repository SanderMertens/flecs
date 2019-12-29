
#include "flecs_private.h"

ecs_filter_iter_t ecs_filter_iter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    return (ecs_filter_iter_t){
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = world->main_stage.tables,
        .index = 0,
        .rows = {
            .world = world
        }
    };
}

ecs_filter_iter_t ecs_snapshot_filter_iter(
    ecs_world_t *world,
    const ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter)
{
    return (ecs_filter_iter_t){
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = snapshot->tables,
        .index = 0,
        .rows = {
            .world = world
        }
    };
}

bool ecs_filter_next(
    ecs_filter_iter_t *iter)
{
    ecs_chunked_t *tables = iter->tables;
    int32_t count = ecs_chunked_count(tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);

        if (!table->columns) {
            continue;
        }

        if (!ecs_type_match_w_filter(iter->rows.world, table->type, &iter->filter)) {
            continue;
        }

        ecs_rows_t *rows = &iter->rows;
        rows->table = table;
        rows->table_columns = table->columns;
        rows->count = ecs_table_count(table);
        rows->entities = ecs_vector_first(table->columns[0].data);
        iter->index = ++i;
        return true;
    }

    return false;
}
