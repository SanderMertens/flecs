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
void* _ecs_map_set(
    ecs_map_t *map,
    uint64_t key_hash,
    const void *data,
    uint32_t size);

#define ecs_map_set(map, key, data)\
    _ecs_map_set(map, key, data, sizeof(*data));

FLECS_EXPORT
bool _ecs_map_has(
    ecs_map_t *map,
    uint64_t key_hash,
    void *value_out,
    uint32_t size);

#define ecs_map_has(map, key, data)\
    _ecs_map_has(map, key, data, sizeof(*(data)))

FLECS_EXPORT
void* ecs_map_get_ptr(
    ecs_map_t *map,
    uint64_t key_hash);

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
void* ecs_map_next(
    ecs_map_iter_t *it);

FLECS_EXPORT
void* ecs_map_next_w_size(
    ecs_map_iter_t *it,
    size_t size);

#define ecs_map_next32(it)\
    *(uint32_t*)ecs_map_next_w_size(it, sizeof(uint32_t))

#define ecs_map_next64(it)\
    *(uint64_t*)ecs_map_next_w_size(it, sizeof(uint64_t))

#define ecs_map_nextptr(it)\
    *(void**)ecs_map_next_w_size(it, sizeof(void*))    

FLECS_EXPORT
void* ecs_map_next_w_key(
    ecs_map_iter_t *it,
    uint64_t *key_out);

FLECS_EXPORT
void* ecs_map_next_w_key_w_size(
    ecs_map_iter_t *it,
    uint64_t *key_out,
    size_t size);

#define ecs_map_next32_w_key(it, key_out)\
    *(uint32_t*)ecs_map_next_w_key_w_size(it, key_out, sizeof(uint32_t))

#define ecs_map_next64_w_key(it, key_out)\
    *(uint64_t*)ecs_map_next_w_key_w_size(it, key_out, sizeof(uint64_t))

#define ecs_map_nextptr_w_key(it, key_out)\
    *(void**)ecs_map_next_w_key_w_size(it, key_out, sizeof(void*))   

#ifdef __cplusplus
}
#endif

#endif
