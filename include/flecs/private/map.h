/**
 * @file map.h
 * @brief Map datastructure.
 *
 * Key-value datastructure. The map allows for fast retrieval of a payload for
 * a 64-bit key. While it is not as fast as the sparse set, it is better at
 * handling randomly distributed values.
 *
 * Payload is stored in bucket arrays. A bucket is computed from an id by
 * using the (bucket_count - 1) as an AND-mask. The number of buckets is always
 * a power of 2. Multiple keys will be stored in the same bucket. As a result
 * the worst case retrieval performance of the map is O(n), though this is rare.
 * On average lookup performance should equal O(1).
 *
 * The datastructure will automatically grow the number of buckets when the
 * ratio between elements and buckets exceeds a certain threshold (LOAD_FACTOR).
 *
 * Note that while the implementation is a hashmap, it can only compute hashes
 * for the provided 64 bit keys. This means that the provided keys must always
 * be unique. If the provided keys are hashes themselves, it is the
 * responsibility of the user to ensure that collisions are handled.
 *
 * In debug mode the map verifies that the type provided to the map functions
 * matches the one used at creation time.
 */

#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t ecs_map_key_t;

/* Map type */
typedef struct ecs_bucket_entry_t {
    struct ecs_bucket_entry_t *next;
    ecs_map_key_t key;
    /* payload right after key. */
} ecs_bucket_entry_t;

typedef struct ecs_block_allocator_block_t {
    void *memory;
    struct ecs_block_allocator_block_t *next;
} ecs_block_allocator_block_t;

typedef struct ecs_block_allocator_chunk_header_t {
    struct ecs_block_allocator_chunk_header_t *next;
} ecs_block_allocator_chunk_header_t;

typedef struct ecs_block_allocator_t {
    ecs_block_allocator_chunk_header_t *head;
    ecs_block_allocator_block_t *block_head;
    ecs_block_allocator_block_t *block_tail;
    int32_t chunk_size;
    int32_t chunks_per_block;
    int32_t block_size;
} ecs_block_allocator_t;

typedef struct ecs_bucket_t {
    ecs_bucket_entry_t *first;
} ecs_bucket_t;

typedef struct ecs_map_t {
    ecs_bucket_t *buckets;
    ecs_bucket_t *buckets_end;
    int16_t elem_size;
    uint8_t bucket_shift;
    int32_t bucket_count;
    int32_t count;
    ecs_block_allocator_t allocator;
} ecs_map_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    ecs_bucket_t *bucket;
    ecs_bucket_entry_t *entry;
} ecs_map_iter_t;

#define ECS_MAP_INIT(T) { .elem_size = ECS_SIZEOF(T) }

/** Initialize new map. */
FLECS_API
void _ecs_map_init(
    ecs_map_t *map,
    ecs_size_t elem_size,
    int32_t elem_count);

#define ecs_map_init(map, T, elem_count)\
    _ecs_map_init(map, sizeof(T), elem_count)

/** Deinitialize map. */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Create new map. */
FLECS_API
ecs_map_t* _ecs_map_new(
    ecs_size_t elem_size,
    int32_t elem_count);

#define ecs_map_new(T, elem_count)\
    _ecs_map_new(sizeof(T), elem_count)

/** Is map initialized */
bool ecs_map_is_initialized(
    const ecs_map_t *result);

/** Get element for key, returns NULL if they key doesn't exist. */
FLECS_API
void* _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_get(map, T, key)\
    (T*)_ecs_map_get(map, sizeof(T), (ecs_map_key_t)key)

/** Get pointer element. This dereferences the map element as a pointer. This
 * operation returns NULL when either the element does not exist or whether the
 * pointer is NULL, and should therefore only be used when the application knows
 * for sure that a pointer should never be NULL. */
FLECS_API
void* _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key);

#define ecs_map_get_ptr(map, T, key)\
    (T)_ecs_map_get_ptr(map, key)

/** Test if map has key */
FLECS_API
bool ecs_map_has(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get or create element for key. */
FLECS_API
void* _ecs_map_ensure(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_ensure(map, T, key)\
    ((T*)_ecs_map_ensure(map, sizeof(T), (ecs_map_key_t)key))

/** Set element. */
FLECS_API
void* _ecs_map_set(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    const void *payload);

#define ecs_map_set(map, key, payload)\
    _ecs_map_set(map, sizeof(*payload), (ecs_map_key_t)key, payload)

#define ecs_map_set_ptr(map, key, payload)\
    _ecs_map_set(map, sizeof(payload), (ecs_map_key_t)key, &payload)

/** Free map. */
FLECS_API
void ecs_map_free(
    ecs_map_t *map);

/** Remove key from map.
 * Returns number of remaining elements.
 */
FLECS_API
int32_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Remove all elements from map. */
FLECS_API
void ecs_map_clear(
    ecs_map_t *map);

/** Return number of elements in map. */
FLECS_API
int32_t ecs_map_count(
    const ecs_map_t *map);

/** Return number of buckets in map. */
FLECS_API
int32_t ecs_map_bucket_count(
    const ecs_map_t *map);

/** Return iterator to map contents. */
FLECS_API
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

/** Obtain next element in map from iterator. */
FLECS_API
void* _ecs_map_next(
    ecs_map_iter_t* iter,
    ecs_size_t elem_size,
    ecs_map_key_t *key);

#define ecs_map_next(iter, T, key) \
    (T*)_ecs_map_next(iter, sizeof(T), key)

/** Obtain next pointer element from iterator. See ecs_map_get_ptr. */
FLECS_API
void* _ecs_map_next_ptr(
    ecs_map_iter_t* iter,
    ecs_map_key_t *key);

#define ecs_map_next_ptr(iter, T, key) \
    (T)_ecs_map_next_ptr(iter, key)

/** Grow number of buckets in the map for specified number of elements. */
FLECS_API
void ecs_map_grow(
    ecs_map_t *map,
    int32_t elem_count);

/** Set number of buckets in the map for specified number of elements. */
FLECS_API
void ecs_map_set_size(
    ecs_map_t *map,
    int32_t elem_count);

/** Copy map. */
FLECS_API
ecs_map_t* ecs_map_copy(
    ecs_map_t *map);

/** Return memory occupied by map. */
FLECS_API
void ecs_map_memory(
    ecs_map_t *map,
    int32_t *allocd,
    int32_t *used);

#ifndef FLECS_LEGACY
#define ecs_map_each(map, T, key, var, ...)\
    {\
        ecs_map_iter_t it = ecs_map_iter(map);\
        ecs_map_key_t key;\
        T* var;\
        (void)key;\
        (void)var;\
        while ((var = ecs_map_next(&it, T, &key))) {\
            __VA_ARGS__\
        }\
    }
#endif

#ifdef __cplusplus
}
#endif

#endif
