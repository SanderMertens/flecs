/**
 * @file datastructures/name_index.h
 * @brief Data structure for resolving 64bit keys by string (name).
 */

#ifndef FLECS_NAME_INDEX_H
#define FLECS_NAME_INDEX_H

void flecs_name_index_init(
    ecs_hashmap_t *hm,
    ecs_allocator_t *allocator);

ecs_hashmap_t* flecs_name_index_new(
    ecs_world_t *world,
    ecs_allocator_t *allocator);

void flecs_name_index_fini(
    ecs_hashmap_t *map);

void flecs_name_index_free(
    ecs_hashmap_t *map);

ecs_hashmap_t* flecs_name_index_copy(
    ecs_hashmap_t *dst);

ecs_hashed_string_t flecs_get_hashed_string(
    const char *name,
    ecs_size_t length,
    uint64_t hash);

const uint64_t* flecs_name_index_find_ptr(
    const ecs_hashmap_t *map,
    const char *name,
    ecs_size_t length,
    uint64_t hash);

uint64_t flecs_name_index_find(
    const ecs_hashmap_t *map,
    const char *name,
    ecs_size_t length,
    uint64_t hash);

void flecs_name_index_ensure(
    ecs_hashmap_t *map,
    uint64_t id,
    const char *name,
    ecs_size_t length,
    uint64_t hash);

void flecs_name_index_remove(
    ecs_hashmap_t *map,
    uint64_t id,
    uint64_t hash);

void flecs_name_index_update_name(
    ecs_hashmap_t *map,
    uint64_t e,
    uint64_t hash,
    const char *name);

#endif
