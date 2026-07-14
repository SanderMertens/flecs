/**
 * @file addons/multi_world.c
 * @brief Support for mapping global component indices to world-local ids.
 */

#include "../private_api.h"

#ifdef FLECS_MULTI_WORLD

static int32_t flecs_component_ids_last_index = 0;

void flecs_multi_world_init(
    ecs_world_t *world)
{
    ecs_vec_init_t(
        &world->allocator, &world->component_ids, ecs_entity_t, 0);
}

void flecs_multi_world_fini(
    ecs_world_t *world)
{
    ecs_vec_fini_t(
        &world->allocator, &world->component_ids, ecs_entity_t);
}

int32_t flecs_component_ids_index_get(void) {
    if (ecs_os_api.ainc_) {
        return ecs_os_ainc(&flecs_component_ids_last_index);
    } else {
        return ++ flecs_component_ids_last_index;
    }
}

ecs_entity_t flecs_component_ids_get(
    const ecs_world_t *stage_world,
    int32_t index)
{
    ecs_world_t *world =
        ECS_CONST_CAST(ecs_world_t*, ecs_get_world(stage_world));
    flecs_poly_assert(world, ecs_world_t);

    if (index >= ecs_vec_count(&world->component_ids)) {
        return 0;
    }

    return ecs_vec_get_t(
        &world->component_ids, ecs_entity_t, index)[0];
}

ecs_entity_t flecs_component_ids_get_alive(
    const ecs_world_t *stage_world,
    int32_t index)
{
    ecs_world_t *world =
        ECS_CONST_CAST(ecs_world_t*, ecs_get_world(stage_world));
    flecs_poly_assert(world, ecs_world_t);

    if (index >= ecs_vec_count(&world->component_ids)) {
        return 0;
    }

    ecs_entity_t result = ecs_vec_get_t(
        &world->component_ids, ecs_entity_t, index)[0];
    if (!flecs_entities_is_alive(world, result)) {
        return 0;
    }

    return result;
}

void flecs_component_ids_set(
    ecs_world_t *stage_world,
    int32_t index,
    ecs_entity_t component)
{
    ecs_world_t *world =
        ECS_CONST_CAST(ecs_world_t*, ecs_get_world(stage_world));
    flecs_poly_assert(world, ecs_world_t);

    ecs_vec_set_min_count_zeromem_t(
        &world->allocator, &world->component_ids, ecs_entity_t, index + 1);
    ecs_vec_get_t(&world->component_ids, ecs_entity_t, index)[0] = component;
}

#endif
