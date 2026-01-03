/**
 * @file spawner.h
 * @brief TOOD
 */

#ifndef FLECS_SPAWNER_H
#define FLECS_SPAWNER_H

const EcsTreeSpawner* flecs_build_prefab_spawner(
    ecs_world_t *world,
    ecs_entity_t base);

void flecs_spawner_instantiate(
    ecs_world_t *world,
    const EcsTreeSpawner *spawner,
    ecs_entity_t instance);

#endif
