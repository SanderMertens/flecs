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

EcsEntity* ecs_entity_new(
    EcsWorld *world,
    const char *id,
    EcsEntity *base);

EcsResult ecs_delete(
    EcsEntity *entity);

void ecs_entity_edit(
    EcsEntity *entity);

void ecs_entity_finalize(
    EcsEntity *entity);

void ecs_entity_add_component(
    EcsEntity *entity,
    EcsEntity *component);

void* ecs_entity_get_component(
    EcsEntity *entity,
    EcsEntity *component);

void ecs_entity_remove_component(
    EcsEntity *entity,
    EcsEntity *component);

#ifdef __cplusplus
}
#endif

#endif
