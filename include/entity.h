#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EcsEntity EcsEntity;
typedef void *EcsData;

EcsEntity *ecs_declare(
    EcsEntity *parent,
    EcsData id,
    EcsEntity *prefab);

EcsResult ecs_define(
    EcsEntity *entity);

EcsResult ecs_delete(
    EcsEntity *entity);

EcsEntity* ecs_claim(
    EcsEntity *entity);

int32_t ecs_release(
    EcsEntity *entity);

EcsData ecs_add_component(
    EcsEntity *entity,
    EcsEntity *component);

EcsData ecs_get_component(
    EcsEntity *entity,
    EcsEntity *component);

EcsResult ecs_remove_component(
    EcsEntity *entity,
    EcsEntity *component);

#ifdef __cplusplus
}
#endif

#endif
