#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_map_t ecs_map_t;

typedef struct ecs_map_iter_t {
    ecs_map_t *map;
    uint32_t bucket_index;
    uint32_t node;
} ecs_map_iter_t;

FLECS_EXPORT
ecs_map_t* ecs_map_new(
    uint32_t size,
    uint32_t elem_size);

FLECS_EXPORT
void ecs_map_free(
    ecs_map_t *map);

FLECS_EXPORT
void ecs_map_memory(
    ecs_map_t *map,
    uint32_t *total,
    uint32_t *used);

FLECS_EXPORT
uint32_t ecs_map_count(
    ecs_map_t *map);

FLECS_EXPORT
uint32_t ecs_map_set_size(
    ecs_map_t *map,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_map_data_size(
    ecs_map_t *map);

FLECS_EXPORT
uint32_t ecs_map_grow(
    ecs_map_t *map,
    uint32_t size);

FLECS_EXPORT
uint32_t ecs_map_bucket_count(
    ecs_map_t *map);

FLECS_EXPORT
void ecs_map_clear(
    ecs_map_t *map);

FLECS_EXPORT
void* ecs_map_set(
    ecs_map_t *map,
    uint64_t key_hash,
    const void *data);

#define ecs_map_set32(map, key, data)\
    ecs_assert(ecs_map_data_size(map) == sizeof(uint32_t), ECS_INVALID_PARAMETERS, NULL);\
    ecs_map_set(map, key, &data)

#define ecs_map_set64(map, key, data)\
    ecs_assert(ecs_map_data_size(map) == sizeof(uint64_t), ECS_INVALID_PARAMETERS, NULL);\
    ecs_map_set(map, key, &data)

#define ecs_map_setptr(map, key, data)\
    ecs_assert(ecs_map_data_size(map) == sizeof(void*), ECS_INVALID_PARAMETERS, NULL);\
    ecs_map_set(map, key, &data)

FLECS_EXPORT
void* ecs_map_get(
    ecs_map_t *map,
    uint64_t key_hash);

FLECS_EXPORT
void* ecs_map_get_w_size(
    ecs_map_t *map,
    uint64_t key_hash,
    uint32_t size);

#define ecs_map_get32(map, key)\
    *(uint32_t*)ecs_map_get_w_size(map, key, sizeof(uint32_t))

#define ecs_map_get64(map, key)\
    *(uint64_t*)ecs_map_get_w_size(map, key, sizeof(uint64_t))

#define ecs_map_getptr(map, key)\
    *(void**)ecs_map_get_w_size(map, key, sizeof(void*))

FLECS_EXPORT
bool ecs_map_has(
    ecs_map_t *map,
    uint64_t key_hash,
    void *value_out);

FLECS_EXPORT
int ecs_map_remove(
    ecs_map_t *map,
    uint64_t key_hash);

FLECS_EXPORT
ecs_map_iter_t ecs_map_iter(
    ecs_map_t *map);

FLECS_EXPORT
bool ecs_map_hasnext(
    ecs_map_iter_t *it);

FLECS_EXPORT
uint64_t ecs_map_next(
    ecs_map_iter_t *it);

FLECS_EXPORT
void* ecs_map_next_ptr(
    ecs_map_iter_t *iter_data);

FLECS_EXPORT
uint64_t ecs_map_next_w_key(
    ecs_map_iter_t *it,
    uint64_t *key_out);

FLECS_EXPORT
void* ecs_map_next_ptr_w_key(
    ecs_map_iter_t *it,
    uint64_t *key_out);

#ifdef __cplusplus
}
#endif

#endif
