#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_map_t ecs_map_t;
typedef struct ecs_bucket_t ecs_bucket_t;
typedef uint64_t ecs_map_key_t;

typedef struct ecs_map_iter_t {
    const ecs_map_t *map;
    ecs_bucket_t *bucket;
    int32_t bucket_index;
    int32_t element_index;
    void *payload;
} ecs_map_iter_t;

FLECS_EXPORT
ecs_map_t * _ecs_map_new(
    size_t t_size, 
    uint32_t element_count);

#define ecs_map_new(T, element_count)\
    _ecs_map_new(sizeof(T), element_count)

FLECS_EXPORT
void * _ecs_map_get(
    const ecs_map_t *map,
    size_t payload_size,
    ecs_map_key_t key);

#define ecs_map_get(map, T, key)\
    (T*)_ecs_map_get(map, sizeof(T), key)

FLECS_EXPORT
bool _ecs_map_has(
    const ecs_map_t *map,
    size_t payload_size,
    ecs_map_key_t key,
    void *payload);

#define ecs_map_has(map, key, payload)\
    _ecs_map_has(map, sizeof(*payload), key, payload)

FLECS_EXPORT
void * _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key);

#define ecs_map_get_ptr(map, T, key)\
    (T)_ecs_map_get_ptr(map, key)

FLECS_EXPORT
void _ecs_map_set(
    ecs_map_t *map,
    size_t payload_size,
    ecs_map_key_t key,
    const void *payload);

#define ecs_map_set(map, key, payload)\
    _ecs_map_set(map, sizeof(*payload), key, payload);

FLECS_EXPORT
void ecs_map_free(
    ecs_map_t *map);

FLECS_EXPORT
void ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key);

FLECS_EXPORT
void ecs_map_clear(
    ecs_map_t *map);

FLECS_EXPORT
uint32_t ecs_map_count(
    const ecs_map_t *map);

FLECS_EXPORT
uint32_t ecs_map_bucket_count(
    const ecs_map_t *map);

FLECS_EXPORT
ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map);

FLECS_EXPORT
void* _ecs_map_next(
    ecs_map_iter_t* iter,
    size_t payload_size,
    ecs_map_key_t *key);

#define ecs_map_next(iter, T, key) \
    (T*)_ecs_map_next(iter, sizeof(T), key)

FLECS_EXPORT
void* _ecs_map_next_ptr(
    ecs_map_iter_t* iter,
    ecs_map_key_t *key);

#define ecs_map_next_ptr(iter, T, key) \
    (T)_ecs_map_next_ptr(iter, key)

FLECS_EXPORT
void ecs_map_grow(
    ecs_map_t *map, 
    uint32_t element_count);

FLECS_EXPORT
void ecs_map_set_size(
    ecs_map_t *map, 
    uint32_t element_count);

FLECS_EXPORT
void ecs_map_memory(
    ecs_map_t *map, 
    uint32_t *allocd,
    uint32_t *used);

FLECS_EXPORT
ecs_map_t* ecs_map_copy(
    const ecs_map_t *map);

#ifdef __cplusplus
}
#endif

#endif