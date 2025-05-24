/**
 * @file instantiate.h
 * @brief Functions for instantiating prefabs (IsA relationship).
 */

#ifndef FLECS_INSTANTIATE_H
#define FLECS_INSTANTIATE_H

typedef struct ecs_instantiate_ctx_t {
    ecs_entity_t root_prefab;
    ecs_entity_t root_instance;
} ecs_instantiate_ctx_t;

/* Instantiate prefab for entity. Called when adding (IsA, prefab). */
void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    const ecs_instantiate_ctx_t *ctx);

#endif
