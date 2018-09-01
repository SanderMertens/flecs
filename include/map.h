#ifndef ECS_MAP_H
#define ECS_MAP_H

#include <ecs/platform.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsMap EcsMap;

EcsMap* ecs_map_new(
    uint32_t size);

void ecs_map_free(
    EcsMap *map);

void ecs_map_clear(
    EcsMap *map);

void ecs_map_add(
    EcsMap *map,
    uint64_t keyhash,
    const void *data);

void ecs_map_remove(
    EcsMap *map,
    uint64_t keyhash);

#ifdef __cplusplus
}
#endif

#endif
