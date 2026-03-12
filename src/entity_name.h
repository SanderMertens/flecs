/**
 * @file entity_name.h
 * @brief Utilities for looking up entities by name.
 */

#include "private_api.h"

void flecs_bootstrap_entity_name(
    ecs_world_t *world);

void flecs_reparent_name_index(
    ecs_world_t *world,
    ecs_table_t *dst,
    ecs_table_t *src, 
    int32_t offset,
    int32_t count);

void flecs_unparent_name_index(
    ecs_world_t *world,
    ecs_table_t *src,
    ecs_table_t *dst,
    int32_t offset,
    int32_t count);

void ecs_on_set(EcsIdentifier)(
    ecs_iter_t *it);
