/**
 * @file hashmap.h
 * @brief Hashmap data structure.
 */

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecs_vec_t keys;
    ecs_vec_t values;
} ecs_hm_bucket_t;

typedef struct {
    ecs_hash_value_action_t hash;
    ecs_compare_action_t compare;
    ecs_size_t key_size;
    ecs_size_t value_size;
    ecs_block_allocator_t *hashmap_allocator;
    ecs_block_allocator_t bucket_allocator;
    ecs_map_t impl;
} ecs_hashmap_t;

typedef struct {
    ecs_map_iter_t it;
    ecs_hm_bucket_t *bucket;
    int32_t index;
} flecs_hashmap_iter_t;

typedef struct {
    void *key;
    void *value;
    uint64_t hash;
} flecs_hashmap_result_t;

FLECS_DBG_API
void _flecs_hashmap_init(
    ecs_hashmap_t *hm,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare,
    ecs_allocator_t *allocator);

#define flecs_hashmap_init(hm, K, V, compare, hash, allocator)\
    _flecs_hashmap_init(hm, ECS_SIZEOF(K), ECS_SIZEOF(V), compare, hash, allocator)

FLECS_DBG_API
void flecs_hashmap_fini(
    ecs_hashmap_t *map);

FLECS_DBG_API
void* _flecs_hashmap_get(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_get(map, key, V)\
    (V*)_flecs_hashmap_get(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
flecs_hashmap_result_t _flecs_hashmap_ensure(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_ensure(map, key, V)\
    _flecs_hashmap_ensure(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _flecs_hashmap_set(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

#define flecs_hashmap_set(map, key, value)\
    _flecs_hashmap_set(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

FLECS_DBG_API
void _flecs_hashmap_remove(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_remove(map, key, V)\
    _flecs_hashmap_remove(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _flecs_hashmap_remove_w_hash(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

#define flecs_hashmap_remove_w_hash(map, key, V, hash)\
    _flecs_hashmap_remove_w_hash(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

FLECS_DBG_API
ecs_hm_bucket_t* flecs_hashmap_get_bucket(
    const ecs_hashmap_t *map,
    uint64_t hash);

FLECS_DBG_API
void flecs_hm_bucket_remove(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    uint64_t hash,
    int32_t index);

FLECS_DBG_API
void flecs_hashmap_copy(
    ecs_hashmap_t *dst,
    const ecs_hashmap_t *src);

FLECS_DBG_API
flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map);

FLECS_DBG_API
void* _flecs_hashmap_next(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

#define flecs_hashmap_next(map, V)\
    (V*)_flecs_hashmap_next(map, 0, NULL, ECS_SIZEOF(V))

#define flecs_hashmap_next_w_key(map, K, key, V)\
    (V*)_flecs_hashmap_next(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif
