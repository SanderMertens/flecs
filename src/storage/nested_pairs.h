/**
 * @file storage/nested_pairs.h
 * @brief Handles for the targets of nested pairs.
 */

#ifndef FLECS_NESTED_PAIRS_H
#define FLECS_NESTED_PAIRS_H

#define FLECS_NESTED_PAIR_HANDLE_OFFSET ((uint32_t)EcsFirstUserEntityId)

typedef struct ecs_nested_pairs_t {
    ecs_vec_t ids;
    ecs_vec_t free_handles;
    ecs_vec_t pending_free;
    ecs_map_t handles;
    ecs_map_t elements;
} ecs_nested_pairs_t;

void flecs_nested_pairs_init(
    ecs_world_t *world);

void flecs_nested_pairs_fini(
    ecs_world_t *world);

ecs_id_t flecs_nested_pair_get(
    const ecs_world_t *world,
    uint32_t handle);

uint32_t flecs_nested_pair_ensure(
    ecs_world_t *world,
    ecs_id_t id);

bool flecs_nested_pairs_take_element(
    ecs_world_t *world,
    uint64_t element,
    ecs_vec_t *out);

bool flecs_nested_pair_release(
    ecs_world_t *world,
    uint32_t handle);

void flecs_nested_pairs_flush(
    ecs_world_t *world);

#endif
