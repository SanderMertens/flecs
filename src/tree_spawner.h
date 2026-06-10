/**
 * @file tree_spawner.h
 * @brief Data structure used to speed up the creation of hierarchies.
 */

#ifndef FLECS_TREE_SPAWNER_H
#define FLECS_TREE_SPAWNER_H

/* Called during bootstrap to register spawner entities with the world. */
void flecs_bootstrap_spawner(
    ecs_world_t *world);

EcsTreeSpawner* flecs_prefab_spawner_build(
    ecs_world_t *world,
    ecs_entity_t base);

#ifdef FLECS_DEBUG
void flecs_tree_spawner_assert_not_instantiated(
    ecs_world_t *world,
    ecs_entity_t parent);
#else
#define flecs_tree_spawner_assert_not_instantiated(world, parent)
#endif

void flecs_spawner_instantiate(
    ecs_world_t *world,
    EcsTreeSpawner *spawner,
    ecs_entity_t base,
    ecs_entity_t instance,
    const ecs_instantiate_ctx_t *ctx);

void flecs_fini_tree_spawners(
    ecs_world_t *world);

#endif
