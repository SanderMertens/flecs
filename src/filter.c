
#include "flecs_private.h"

ecs_iter_t ecs_filter_iter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_filter_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = world->stage.tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = world,
        .iter.filter = iter
    };
}

ecs_iter_t ecs_snapshot_filter_iter(
    ecs_world_t *world,
    const ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter)
{
    ecs_filter_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = snapshot->tables,
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
        
        ecs_data_t *data = ecs_vector_first(table->stage_data, ecs_data_t);

        if (!data) {
            continue;
        }

        if (!ecs_table_match_filter(it->world, table, &iter->filter)) {
            continue;
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = ++i;

        return true;
    }

    return false;
}
