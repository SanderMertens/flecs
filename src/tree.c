#include "flecs_private.h"
#include "flecs/util/strbuf.h"

ecs_view_t ecs_tree_iter(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_tree_iter_t iter = {
        .tables = ecs_map_get_ptr(world->child_tables, ecs_vector_t*, parent),
        .index = 0
    };

    return (ecs_view_t) {
        .world = world,
        .iter.parent = iter
    };
}

bool ecs_tree_next(
    ecs_view_t *view)
{
    ecs_tree_iter_t *iter = &view->iter.parent;
    ecs_vector_t *tables = iter->tables;
    int32_t count = ecs_vector_count(tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = *ecs_vector_get(tables, ecs_table_t*, i);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_vector_first(table->stage_data, ecs_data_t);
        if (!data) {
            continue;
        }
        
        view->count = ecs_table_count(table);
        if (!view->count) {
            continue;
        }

        view->table = table;
        view->table_columns = data->columns;
        view->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = ++i;

        return true;
    }

    return false;    
}
