/**
 * @file map.h
 * @brief Map data structure.
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

typedef struct ecs_bucket_t {
    ecs_bucket_entry_t *first;
} ecs_bucket_t;

typedef struct ecs_map_t {
    ecs_bucket_t *buckets;
    ecs_bucket_t *buckets_end;
    int16_t elem_size;
    uint8_t bucket_shift;
    bool shared_allocator;
    int32_t bucket_count;
    int32_t count;
    struct ecs_allocator_t *allocator;
    struct ecs_block_allocator_t *entry_allocator;
} ecs_map_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    ecs_bucket_t *bucket;
    ecs_bucket_entry_t *entry;
} ecs_map_iter_t;

typedef struct ecs_map_params_t {
    ecs_size_t size;
    struct ecs_allocator_t *allocator;
    struct ecs_block_allocator_t entry_allocator;
    int32_t initial_count;
} ecs_map_params_t;

#define ECS_MAP_INIT(T) { .elem_size = ECS_SIZEOF(T) }

FLECS_API
void _ecs_map_params_init(
    ecs_map_params_t *params,
    struct ecs_allocator_t *allocator,
    ecs_size_t elem_size);

#define ecs_map_params_init(params, allocator, T)\
    _ecs_map_params_init(params, allocator, ECS_SIZEOF(T))

FLECS_API
void ecs_map_params_fini(
    ecs_map_params_t *params);

/** Initialize new map. */
FLECS_API
void _ecs_map_init(
    ecs_map_t *map,
    ecs_size_t elem_size,
    struct ecs_allocator_t *allocator,
    int32_t initial_count);

#define ecs_map_init(map, T, allocator, initial_count)\
    _ecs_map_init(map, ECS_SIZEOF(T), allocator, initial_count)

/** Initialize new map. */
FLECS_API
void _ecs_map_init_w_params(
    ecs_map_t *map,
    ecs_map_params_t *params);

#define ecs_map_init_w_params(map, param)\
    _ecs_map_init_w_params(map, param)

/** Initialize new map if uninitialized, leave as is otherwise */
FLECS_API
void _ecs_map_init_if(
    ecs_map_t *map,
    ecs_size_t elem_size,
    struct ecs_allocator_t *allocator,
    int32_t elem_count);

#define ecs_map_init_if(map, T, allocator, elem_count)\
    _ecs_map_init_if(map, ECS_SIZEOF(T), allocator, elem_count)

FLECS_API
void _ecs_map_init_w_params_if(
    ecs_map_t *result,
    ecs_map_params_t *params);

#define ecs_map_init_w_params_if(map, params)\
    _ecs_map_init_w_params_if(map, params)

/** Deinitialize map. */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Create new map. */
FLECS_API
ecs_map_t* _ecs_map_new(
    ecs_size_t elem_size,
    struct ecs_allocator_t *allocator,
    int32_t elem_count);

#define ecs_map_new(T, allocator, elem_count)\
    _ecs_map_new(ECS_SIZEOF(T), allocator, elem_count)

/** Get element for key, returns NULL if they key doesn't exist. */
FLECS_API
void* _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_get(map, T, key)\
    (T*)_ecs_map_get(map, ECS_SIZEOF(T), (ecs_map_key_t)key)

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

/** Get or create element for key. */
FLECS_API
void* _ecs_map_ensure(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_ensure(map, T, key)\
    ((T*)_ecs_map_ensure(map, ECS_SIZEOF(T), (ecs_map_key_t)key))

/** Get or create element for key. */
FLECS_API
void* _ecs_map_insert(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

#define ecs_map_insert(map, T, key)\
    ((T*)_ecs_map_insert(map, ECS_SIZEOF(T), (ecs_map_key_t)key))

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
#define ecs_map_count(map) ((map) ? (map)->count : 0)

/** Is map initialized */
#define ecs_map_is_init(map) ((map) ? (map)->bucket_shift != 0 : false)

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
    (T*)_ecs_map_next(iter, ECS_SIZEOF(T), key)

/** Obtain next pointer element from iterator. See ecs_map_get_ptr. */
FLECS_API
void* _ecs_map_next_ptr(
    ecs_map_iter_t* iter,
    ecs_map_key_t *key);

#define ecs_map_next_ptr(iter, T, key) \
    (T)_ecs_map_next_ptr(iter, key)

/** Copy map. */
FLECS_API
ecs_map_t* ecs_map_copy(
    ecs_map_t *map);

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
