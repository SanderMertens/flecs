/**
 * @file spawner.h
 * @brief Data structure used to speed up the creation of hierarchies.
 */

#ifndef FLECS_SPAWNER_H
#define FLECS_SPAWNER_H

EcsTreeSpawner* flecs_prefab_spawner_build(
    ecs_world_t *world,
    ecs_entity_t base);

void flecs_spawner_instantiate(
    ecs_world_t *world,
    EcsTreeSpawner *spawner,
    ecs_entity_t instance);

#endif
