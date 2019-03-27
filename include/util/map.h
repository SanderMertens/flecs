#ifndef FLECS_MAP_H
#define FLECS_MAP_H

#include "../flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_map_t ecs_map_t;

typedef struct EcsMapIter {
    uint32_t bucket_index;
    uint32_t node;
} EcsMapIter;

FLECS_EXPORT
ecs_map_t* ecs_map_new(
    uint32_t size);

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
uint32_t ecs_map_bucket_count(
    ecs_map_t *map);

FLECS_EXPORT
void ecs_map_clear(
    ecs_map_t *map);

FLECS_EXPORT
void ecs_map_set64(
    ecs_map_t *map,
    uint64_t key_hash,
    uint64_t data);

#define ecs_map_set(map, key_hash, data) \
    ecs_map_set64(map, key_hash, (uintptr_t)data)

FLECS_EXPORT
uint64_t ecs_map_get64(
    ecs_map_t *map,
    uint64_t key_hash);

#define ecs_map_get(map, key_hash) \
    (void*)(uintptr_t)ecs_map_get64(map, key_hash)

FLECS_EXPORT
bool ecs_map_has(
    ecs_map_t *map,
    uint64_t key_hash,
    uint64_t *value_out);

FLECS_EXPORT
int ecs_map_remove(
    ecs_map_t *map,
    uint64_t key_hash);

FLECS_EXPORT
EcsIter _ecs_map_iter(
    ecs_map_t *map,
    EcsMapIter *iter_data);

#define ecs_map_iter(me)\
    _ecs_map_iter(me, ecs_os_alloca(EcsMapIter))

FLECS_EXPORT
uint64_t ecs_map_next(
    EcsIter *it,
    uint64_t *key_out);

#ifdef __cplusplus
}
#endif

#endif
