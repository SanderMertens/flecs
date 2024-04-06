/**
 * @file query/each.c
 * @brief Simple iterator for a single component id.
 */

#include "private_api.h"

ecs_iter_t ecs_each_id(
    const ecs_world_t *stage,
    ecs_id_t id)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_process_pending_tables(world);

    ecs_iter_t it = {
        .real_world = ECS_CONST_CAST(ecs_world_t*, world),
        .world = ECS_CONST_CAST(ecs_world_t*, stage),
        .field_count = 1,
        .next = ecs_each_next
    };

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return it;
    }

    ecs_each_iter_t *each_iter = &it.priv_.iter.each;
    each_iter->ids = id;
    each_iter->sizes = 0;
    if (idr->type_info) {
        each_iter->sizes = idr->type_info->size;
    }

    each_iter->sources = 0;
    flecs_table_cache_iter((ecs_table_cache_t*)idr, &each_iter->it);

    return it;
error:
    return (ecs_iter_t){0};
}

bool ecs_each_next(
    ecs_iter_t *it)
{
    ecs_each_iter_t *each_iter = &it->priv_.iter.each;
    ecs_table_record_t *next = flecs_table_cache_next(
        &each_iter->it, ecs_table_record_t);
    it->flags |= EcsIterIsValid;
    if (next) {
        ecs_table_t *table = next->hdr.table;
        it->table = table;
        it->count = ecs_table_count(table);
        it->entities = flecs_table_entities_array(table);
        it->ids = &table->type.array[next->index];
        it->columns = &each_iter->columns;
        it->sources = &each_iter->sources;
        it->sizes = &each_iter->sizes;
        it->ptrs = &each_iter->ptrs;
        it->set_fields = 1;
        each_iter->columns = next->index;

        if (next->column != -1) {
            each_iter->ptrs = ecs_vec_first(
                &it->table->data.columns[next->column].data);
        }
        return true;
    } else {
        return false;
    }
}

ecs_iter_t ecs_children(
    const ecs_world_t *stage,
    ecs_entity_t parent)
{
    return ecs_each_id(stage, ecs_childof(parent));
}

bool ecs_children_next(
    ecs_iter_t *it)
{
    return ecs_each_next(it);
}
