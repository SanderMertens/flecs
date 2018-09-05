#ifndef ECS_MAP_H
#define ECS_MAP_H

#include <ecs/platform.h>
#include <ecs/iter.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsMap EcsMap;

typedef struct EcsMapIter {
    EcsIter bucket_iter;
    uint32_t bucket_index;
} EcsMapIter;

EcsMap* ecs_map_new(
    uint32_t size);

void ecs_map_free(
    EcsMap *map);

void ecs_map_clear(
    EcsMap *map);

void ecs_map_set(
    EcsMap *map,
    uint64_t key_hash,
    const void *data);

void ecs_map_remove(
    EcsMap *map,
    uint64_t key_hash);

const void* ecs_map_lookup(
    EcsMap *map,
    uint64_t key_hash);

EcsIter _ecs_map_iter(
    EcsMap *map,
    EcsMapIter *iter_data);

#define ecs_map_iter(me)\
    _ecs_map_iter(me, alloca(sizeof(EcsMapIter)))


#ifdef __cplusplus
}
#endif

#endif
