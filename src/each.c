/**
 * @file query/each.c
 * @brief Simple iterator for a single component id.
 */

#include "private_api.h"

static
bool flecs_each_component_record(
    ecs_iter_t *it,
    ecs_component_record_t *cr,
    ecs_id_t id)
{
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(id == cr->id, ECS_INTERNAL_ERROR, NULL);

    ecs_each_iter_t *each_iter = &it->priv_.iter.each;
    each_iter->ids = id;
    each_iter->sizes = 0;
    if (cr->type_info) {
        each_iter->sizes = cr->type_info->size;
    }

    each_iter->sources = 0;
    each_iter->trs = NULL;
    flecs_table_cache_iter((ecs_table_cache_t*)cr, &each_iter->it);

    return true;
error:
    return false;
}

ecs_iter_t ecs_each_id(
    const ecs_world_t *stage,
    ecs_id_t id)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_check_exclusive_world_access_write(world);

    ecs_iter_t it = {
        .real_world = ECS_CONST_CAST(ecs_world_t*, world),
        .world = ECS_CONST_CAST(ecs_world_t*, stage),
        .field_count = 1,
        .next = ecs_each_next
    };

    ecs_component_record_t *cr = flecs_components_get(world, id);
    if (!cr) {
        return it;
    }

    if (!flecs_each_component_record(&it, cr, id)) {
        return (ecs_iter_t){0};
    }

    return it;
error:
    return (ecs_iter_t){0};
}

bool ecs_each_next(
    ecs_iter_t *it)
{
    ecs_each_iter_t *each_iter = &it->priv_.iter.each;
    const ecs_table_record_t *next = flecs_table_cache_next(
        &each_iter->it, ecs_table_record_t);
    it->flags |= EcsIterIsValid;
    if (next) {
        each_iter->trs = next;
        ecs_table_t *table = next->hdr.table;
        it->table = table;
        it->count = ecs_table_count(table);
        it->entities = ecs_table_entities(table);
        if (next->index != -1) {
            it->ids = &table->type.array[next->index];
        } else {
            it->ids = NULL;
        }
        it->trs = &each_iter->trs;
        it->sources = &each_iter->sources;
        it->sizes = &each_iter->sizes;
        it->set_fields = 1;

        return true;
    } else {
        return false;
    }
}

static
bool flecs_children_next_ordered(
    ecs_iter_t *it)
{
    return ecs_children_next(it);
}

ecs_iter_t ecs_children_w_rel(
    const ecs_world_t *stage,
    ecs_entity_t relationship,
    ecs_entity_t parent)
{
    ecs_check(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    const ecs_world_t *world = ecs_get_world(stage);

    flecs_check_exclusive_world_access_read(world);

    ecs_iter_t it = {
        .real_world = ECS_CONST_CAST(ecs_world_t*, world),
        .world = ECS_CONST_CAST(ecs_world_t*, stage),
        .field_count = 1,
        .next = ecs_children_next
    };

    ecs_component_record_t *cr = flecs_components_get(
        world, ecs_pair(relationship, parent));
    if (!cr) {
        return (ecs_iter_t){0};
    }

    if (cr->flags & EcsIdOrderedChildren) {
        ecs_vec_t *v = &cr->pair->ordered_children;
        it.entities = ecs_vec_first_t(v, ecs_entity_t);
        it.count = ecs_vec_count(v);
        it.next = flecs_children_next_ordered;
        return it;
    } else if (cr->flags & EcsIdSparse) {
        it.entities = flecs_sparse_ids(cr->sparse);
        it.count = flecs_sparse_count(cr->sparse);
        it.next = flecs_children_next_ordered;
        return it;
    }

    return ecs_each_id(stage, ecs_pair(relationship, parent));
error:
    return (ecs_iter_t){0};
}

ecs_iter_t ecs_children(
    const ecs_world_t *stage,
    ecs_entity_t parent)
{
    return ecs_children_w_rel(stage, EcsChildOf, parent);
}

bool ecs_children_next(
    ecs_iter_t *it)
{
    if (it->next == NULL) {
        return false;
    }

    if (it->next == flecs_children_next_ordered) {
        if (!it->count) {
            return false;
        }

        it->next = NULL; /* Only return once with ordered children vector */

        return true;
    }

    return ecs_each_next(it);
}

int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_entity_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!id) {
        return 0;
    }

    int32_t count = 0;
    ecs_iter_t it = ecs_each_id(world, id);
    while (ecs_each_next(&it)) {
        count += it.count * it.trs[0]->count;
    }

    return count;
error:
    return 0;
}
