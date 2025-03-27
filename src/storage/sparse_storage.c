#include "../private_api.h"

bool flecs_component_sparse_has(
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t id = cdr->id;
    if (ecs_id_is_wildcard(id)) {
        if (ECS_IS_PAIR(id)) {
            if ((ECS_PAIR_SECOND(id) == EcsWildcard) && 
                (cdr->flags & EcsIdDontFragment)) 
            {
                ecs_component_record_t *cur = cdr;
                while ((cur = flecs_component_first_next(cur))) {
                    if (!cur->sparse) {
                        continue;
                    }

                    if (flecs_sparse_has_any(cur->sparse, entity)) {
                        return true;
                    }
                }
            }

            if ((ECS_PAIR_FIRST(id) == EcsWildcard) && 
                (cdr->flags & EcsIdMatchDontFragment)) 
            {
                ecs_component_record_t *cur = cdr;
                while ((cur = flecs_component_second_next(cur))) {
                    if (!cur->sparse) {
                        continue;
                    }

                    if (flecs_sparse_has_any(cur->sparse, entity)) {
                        return true;
                    }
                }
            }

            return false;
        }

        return false;
    } else {
        ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cdr->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
        return flecs_sparse_has_any(cdr->sparse, entity);
    }
}

void* flecs_component_sparse_get(
    ecs_component_record_t *cdr,
    ecs_entity_t entity)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    return flecs_sparse_get_any(cdr->sparse, 0, entity);
}

void* flecs_component_sparse_insert(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_insert(cdr->sparse, 0, entity);
    if (!ptr) {
        return NULL;
    }

    const ecs_type_info_t *ti = cdr->type_info;
    if (!ti) {
        return ptr;
    }

    ecs_xtor_t ctor = ti->hooks.ctor;
    if (ctor) {
        ctor(ptr, 1, ti);
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (!on_add) {
        return ptr;
    }

    const ecs_table_record_t *tr = NULL;
    if (!(cdr->flags & EcsIdDontFragment)) {
        tr = flecs_component_get_table(cdr, table);
    }

    flecs_invoke_hook(world, table, cdr, tr, 1, row, 
        entity, cdr->id, ti, EcsOnAdd, on_add);

    return ptr;
}

void* flecs_component_sparse_emplace(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    void *ptr = flecs_sparse_insert(cdr->sparse, 0, entity);
    if (!ptr) {
        return NULL;
    }

    const ecs_type_info_t *ti = cdr->type_info;
    if (!ti) {
        return ptr;
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (!on_add) {
        return ptr;
    }

    const ecs_table_record_t *tr = NULL;
    if (!(cdr->flags & EcsIdDontFragment)) {
        tr = flecs_component_get_table(cdr, table);
    }

    flecs_invoke_hook(world, table, cdr, tr, 1, row, 
        entity, cdr->id, ti, EcsOnAdd, on_add);

    return ptr;
}

void flecs_component_sparse_remove(
    ecs_world_t *world,
    ecs_component_record_t *cdr,
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(cdr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cdr->flags & EcsIdIsSparse, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t entity = ecs_table_entities(table)[row];
    const ecs_type_info_t *ti = cdr->type_info;

    if (!ti) {
        flecs_sparse_remove_fast(cdr->sparse, 0, entity);
        return;
    }

    void *ptr = flecs_sparse_get_any(cdr->sparse, 0, entity);
    if (!ptr) {
        return;
    }

    ecs_iter_action_t on_remove = ti->hooks.on_remove;
    if (on_remove) {
        const ecs_table_record_t *tr = NULL;
        if (!(cdr->flags & EcsIdDontFragment)) {
            tr = flecs_component_get_table(cdr, table);
        }

        flecs_invoke_hook(world, table, cdr, tr, 1, row, 
            entity, cdr->id, ti, EcsOnRemove, on_remove);
    }

    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        dtor(ptr, 1, ti);
    }

    flecs_sparse_remove_fast(cdr->sparse, 0, entity);
}
