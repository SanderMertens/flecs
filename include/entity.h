#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include <stdint.h>
#include <ecs/log.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsWorld EcsWorld;
typedef struct EcsEntity EcsEntity;

EcsWorld* ecs_world_new(
    uint32_t initial_size);

EcsWorld* ecs_world_delete(
    uint32_t initial_size);

EcsEntity* ecs_new(
    EcsWorld *world,
    const char *id);

void ecs_delete(
    EcsEntity *entity);

void ecs_add(
    EcsEntity *entity,
    EcsEntity *component);

void* ecs_get(
    EcsEntity *entity,
    EcsEntity *component);

void ecs_remove(
    EcsEntity *entity,
    EcsEntity *component);

void ecs_commit(
    EcsEntity *entity);

EcsEntity* ecs_lookup(
    EcsWorld *world,
    const char *id);

#ifdef __cplusplus
}
#endif

#endif
