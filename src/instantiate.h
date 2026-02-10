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

void flecs_instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance,
    const ecs_instantiate_ctx_t *ctx);

void flecs_instantiate_dont_fragment(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t instance);

void flecs_instantiate_sparse(
    ecs_world_t *world,
    const ecs_table_range_t *base_child_range,
    const ecs_entity_t *base_children,
    ecs_table_t *instance_table,
    const ecs_entity_t *instance_children,
    int32_t row_offset);

#endif
