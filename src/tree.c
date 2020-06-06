#include "flecs_private.h"
#include "flecs/util/strbuf.h"

static
bool path_append(
    ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    ecs_entity_t component,
    char *sep,
    char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_type_t type = ecs_get_type(world, child);
    ecs_entity_t cur = ecs_find_in_type(world, type, component, ECS_CHILDOF);
    
    if (cur) {
        if (cur != parent) {
            path_append(world, parent, cur, component, sep, prefix, buf);
            ecs_strbuf_appendstr(buf, sep);
        }
    } else if (prefix) {
        ecs_strbuf_appendstr(buf, prefix);
    }

    ecs_strbuf_appendstr(buf, ecs_get_name(world, child));

    return cur != 0;
}

char* ecs_get_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    ecs_entity_t component,
    char *sep,
    char *prefix)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (parent != child) {
        path_append(world, parent, child, component, sep, prefix, &buf);
    } else {
        ecs_strbuf_appendstr(&buf, "");
    }

    return ecs_strbuf_get(&buf);
}

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
        view->count = ecs_table_count(table);
        view->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = ++i;

        return true;
    }

    return false;    
}
