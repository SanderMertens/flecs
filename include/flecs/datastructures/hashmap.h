/**
 * @file datastructures/hashmap.h
 * @brief Hashmap data structure.
 */

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H

#include "../private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A bucket in the hashmap, storing parallel key and value vectors. */
typedef struct {
    ecs_vec_t keys; /**< Vector of keys. */
    ecs_vec_t values; /**< Vector of values. */
} ecs_hm_bucket_t;

/** A hashmap that supports variable-sized keys and values. */
typedef struct {
    ecs_hash_value_action_t hash; /**< Hash function for keys. */
    ecs_compare_action_t compare; /**< Compare function for keys. */
    ecs_size_t key_size; /**< Size of key type. */
    ecs_size_t value_size; /**< Size of value type. */
    ecs_map_t impl; /**< Underlying map implementation. */
} ecs_hashmap_t;

/** Iterator for a hashmap. */
typedef struct {
    ecs_map_iter_t it; /**< Underlying map iterator. */
    ecs_hm_bucket_t *bucket; /**< Current bucket. */
    int32_t index; /**< Current index within the bucket. */
} flecs_hashmap_iter_t;

/** Result of a hashmap ensure operation. */
typedef struct {
    void *key; /**< Pointer to the key. */
    void *value; /**< Pointer to the value. */
    uint64_t hash; /**< Hash value of the key. */
} flecs_hashmap_result_t;

/** Initialize a hashmap.
 *
 * @param hm The hashmap to initialize.
 * @param key_size The size of the key type.
 * @param value_size The size of the value type.
 * @param hash The hash function.
 * @param compare The compare function.
 * @param allocator The allocator.
 */
FLECS_DBG_API
void flecs_hashmap_init_(
    ecs_hashmap_t *hm,
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare,
    ecs_allocator_t *allocator);

/** Type-safe hashmap initialization. */
#define flecs_hashmap_init(hm, K, V, hash, compare, allocator)\
    flecs_hashmap_init_(hm, ECS_SIZEOF(K), ECS_SIZEOF(V), hash, compare, allocator)

/** Deinitialize a hashmap.
 *
 * @param map The hashmap to deinitialize.
 */
FLECS_DBG_API
void flecs_hashmap_fini(
    ecs_hashmap_t *map);

/** Get a value from the hashmap.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to look up.
 * @param value_size The size of the value type.
 * @return Pointer to the value, or NULL if not found.
 */
FLECS_DBG_API
void* flecs_hashmap_get_(
    const ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

/** Type-safe hashmap get. */
#define flecs_hashmap_get(map, key, V)\
    (V*)flecs_hashmap_get_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

/** Ensure a key exists in the hashmap, inserting if necessary.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to ensure.
 * @param value_size The size of the value type.
 * @return A result containing pointers to the key, value, and hash.
 */
FLECS_DBG_API
flecs_hashmap_result_t flecs_hashmap_ensure_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

/** Type-safe hashmap ensure. */
#define flecs_hashmap_ensure(map, key, V)\
    flecs_hashmap_ensure_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

/** Set a key-value pair in the hashmap.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key.
 * @param value_size The size of the value type.
 * @param value The value to set.
 */
FLECS_DBG_API
void flecs_hashmap_set_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

/** Type-safe hashmap set. */
#define flecs_hashmap_set(map, key, value)\
    flecs_hashmap_set_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

/** Remove a key from the hashmap.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to remove.
 * @param value_size The size of the value type.
 */
FLECS_DBG_API
void flecs_hashmap_remove_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

/** Type-safe hashmap remove. */
#define flecs_hashmap_remove(map, key, V)\
    flecs_hashmap_remove_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

/** Remove a key from the hashmap using a precomputed hash.
 *
 * @param map The hashmap.
 * @param key_size The size of the key type.
 * @param key The key to remove.
 * @param value_size The size of the value type.
 * @param hash The precomputed hash of the key.
 */
FLECS_DBG_API
void flecs_hashmap_remove_w_hash_(
    ecs_hashmap_t *map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

/** Type-safe hashmap remove with precomputed hash. */
#define flecs_hashmap_remove_w_hash(map, key, V, hash)\
    flecs_hashmap_remove_w_hash_(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

/** Get a bucket from the hashmap by hash value.
 *
 * @param map The hashmap.
 * @param hash The hash value.
 * @return The bucket, or NULL if not found.
 */
FLECS_DBG_API
ecs_hm_bucket_t* flecs_hashmap_get_bucket(
    const ecs_hashmap_t *map,
    uint64_t hash);

/** Remove an entry from a hashmap bucket by index.
 *
 * @param map The hashmap.
 * @param bucket The bucket.
 * @param hash The hash value.
 * @param index The index within the bucket to remove.
 */
FLECS_DBG_API
void flecs_hm_bucket_remove(
    ecs_hashmap_t *map,
    ecs_hm_bucket_t *bucket,
    uint64_t hash,
    int32_t index);

/** Copy a hashmap.
 *
 * @param dst The destination hashmap.
 * @param src The source hashmap.
 */
FLECS_DBG_API
void flecs_hashmap_copy(
    ecs_hashmap_t *dst,
    const ecs_hashmap_t *src);

/** Create an iterator for a hashmap.
 *
 * @param map The hashmap to iterate.
 * @return The iterator.
 */
FLECS_DBG_API
flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t *map);

/** Get the next element from a hashmap iterator.
 *
 * @param it The hashmap iterator.
 * @param key_size The size of the key type.
 * @param key_out Output parameter for the key.
 * @param value_size The size of the value type.
 * @return Pointer to the value, or NULL if no more elements.
 */
FLECS_DBG_API
void* flecs_hashmap_next_(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

/** Type-safe hashmap next (value only). */
#define flecs_hashmap_next(map, V)\
    (V*)flecs_hashmap_next_(map, 0, NULL, ECS_SIZEOF(V))

/** Type-safe hashmap next with key output. */
#define flecs_hashmap_next_w_key(map, K, key, V)\
    (V*)flecs_hashmap_next_(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif
