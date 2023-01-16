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

typedef uint64_t ecs_map_data_t;
typedef ecs_map_data_t ecs_map_key_t;
typedef ecs_map_data_t ecs_map_val_t;

/* Map type */
typedef struct ecs_bucket_entry_t {
    ecs_map_key_t key;
    ecs_map_val_t value;
    struct ecs_bucket_entry_t *next;
} ecs_bucket_entry_t;

typedef struct ecs_bucket_t {
    ecs_bucket_entry_t *first;
} ecs_bucket_t;

typedef struct ecs_map_t {
    uint8_t bucket_shift;
    bool shared_allocator;
    ecs_bucket_t *buckets;
    int32_t bucket_count;
    int32_t count;
    struct ecs_block_allocator_t *entry_allocator;
    struct ecs_allocator_t *allocator;
} ecs_map_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    ecs_bucket_t *bucket;
    ecs_bucket_entry_t *entry;
    ecs_map_data_t *res;
} ecs_map_iter_t;

typedef struct ecs_map_params_t {
    struct ecs_allocator_t *allocator;
    struct ecs_block_allocator_t entry_allocator;
} ecs_map_params_t;

/* Function/macro postfixes meaning:
 *   _ptr:    access ecs_map_val_t as void*
 *   _ref:    access ecs_map_val_t* as T**
 *   _deref:  dereferences a _ref
 *   _alloc:  if _ptr is NULL, alloc
 *   _free:   if _ptr is not NULL, free
 */

FLECS_API
void ecs_map_params_init(
    ecs_map_params_t *params,
    struct ecs_allocator_t *allocator);

FLECS_API
void ecs_map_params_fini(
    ecs_map_params_t *params);

/** Initialize new map. */
FLECS_API
void ecs_map_init(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

/** Initialize new map. */
FLECS_API
void ecs_map_init_w_params(
    ecs_map_t *map,
    ecs_map_params_t *params);

/** Initialize new map if uninitialized, leave as is otherwise */
FLECS_API
void ecs_map_init_if(
    ecs_map_t *map,
    struct ecs_allocator_t *allocator);

FLECS_API
void ecs_map_init_w_params_if(
    ecs_map_t *result,
    ecs_map_params_t *params);

/** Deinitialize map. */
FLECS_API
void ecs_map_fini(
    ecs_map_t *map);

/** Get element for key, returns NULL if they key doesn't exist. */
FLECS_API
ecs_map_val_t* ecs_map_get(
    const ecs_map_t *map,
    ecs_map_key_t key);

/* Get element as pointer (auto-dereferences _ptr) */
FLECS_API
void* _ecs_map_get_deref(
    const ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert element for key. */
FLECS_API
ecs_map_val_t* ecs_map_ensure(
    ecs_map_t *map,
    ecs_map_key_t key);

/** Get or insert pointer element for key, allocate if the pointer is NULL */
FLECS_API
void* ecs_map_ensure_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Insert element for key. */
FLECS_API
void ecs_map_insert(
    ecs_map_t *map,
    ecs_map_key_t key,
    ecs_map_val_t value);

/** Insert pointer element for key, populate with new allocation. */
FLECS_API
void* ecs_map_insert_alloc(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key);

/** Remove key from map. */
FLECS_API
ecs_map_val_t ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

/* Remove pointer element, free if not NULL */
FLECS_API
void ecs_map_remove_free(
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
bool ecs_map_next(
    ecs_map_iter_t *iter);

/** Copy map. */
FLECS_API
void ecs_map_copy(
    ecs_map_t *dst,
    const ecs_map_t *src);

#define ecs_map_get_ref(m, T, k) ECS_CAST(T**, ecs_map_get(m, k))
#define ecs_map_get_deref(m, T, k) ECS_CAST(T*, _ecs_map_get_deref(m, k))
#define ecs_map_ensure_ref(m, T, k) ECS_CAST(T**, ecs_map_ensure(m, k))

#define ecs_map_insert_ptr(m, k, v) ecs_map_insert(m, k, ECS_CAST(ecs_map_val_t, v))
#define ecs_map_insert_alloc_t(m, T, k) ECS_CAST(T*, ecs_map_insert_alloc(m, ECS_SIZEOF(T), k))
#define ecs_map_ensure_alloc_t(m, T, k) ECS_CAST(T*, ecs_map_ensure_alloc(m, ECS_SIZEOF(T), k))
#define ecs_map_remove_ptr(m, k) ((void*)(ecs_map_remove(m, k)))

#define ecs_map_key(it) ((it)->res[0])
#define ecs_map_value(it) ((it)->res[1])
#define ecs_map_ptr(it) ECS_CAST(void*, ecs_map_value(it))
#define ecs_map_ref(it, T) (ECS_CAST(T**, &((it)->res[1])))

#ifdef __cplusplus
}
#endif

#endif
