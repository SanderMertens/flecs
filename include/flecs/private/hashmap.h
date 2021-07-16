/**
 * @file hashmap.h
 * @brief Hashmap datastructure.
 *
 * Datastructure that computes a hash to store & retrieve values. Similar to
 * ecs_map_t, but allows for arbitrary keytypes.
 */

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecs_hash_value_action_t hash;
    ecs_compare_action_t compare;
    ecs_size_t key_size;
    ecs_size_t value_size;
    ecs_map_t *impl;
} ecs_hashmap_t;

typedef struct {
    ecs_map_iter_t it;
    struct ecs_hm_bucket_t *bucket;
    int32_t index;
} ecs_hashmap_iter_t;

typedef struct {
    void *key;
    void *value;
    uint64_t hash;
} ecs_hashmap_result_t;

FLECS_DBG_API
ecs_hashmap_t _ecs_hashmap_new(
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare);

#define ecs_hashmap_new(K, V, compare, hash)\
    _ecs_hashmap_new(ECS_SIZEOF(K), ECS_SIZEOF(V), compare, hash)

FLECS_DBG_API
void ecs_hashmap_free(
    ecs_hashmap_t map);

FLECS_DBG_API
void* _ecs_hashmap_get(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define ecs_hashmap_get(map, key, V)\
    (V*)_ecs_hashmap_get(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
ecs_hashmap_result_t _ecs_hashmap_ensure(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size);

#define ecs_hashmap_ensure(map, key, V)\
    _ecs_hashmap_ensure(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _ecs_hashmap_set(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

#define ecs_hashmap_set(map, key, value)\
    _ecs_hashmap_set(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

FLECS_DBG_API
void _ecs_hashmap_remove(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define ecs_hashmap_remove(map, key, V)\
    _ecs_hashmap_remove(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _ecs_hashmap_remove_w_hash(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

#define ecs_hashmap_remove_w_hash(map, key, V, hash)\
    _ecs_hashmap_remove_w_hash(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

FLECS_DBG_API
ecs_hashmap_iter_t ecs_hashmap_iter(
    ecs_hashmap_t map);

FLECS_DBG_API
void* _ecs_hashmap_next(
    ecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

#define ecs_hashmap_next(map, V)\
    (V*)_ecs_hashmap_next(map, 0, NULL, ECS_SIZEOF(V))

#define ecs_hashmap_next_w_key(map, K, key, V)\
    (V*)_ecs_hashmap_next(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif
