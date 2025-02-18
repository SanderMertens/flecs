/**
 * @file custom_storage.h
 * @brief Interface for custom storage plugins.
 */

#ifndef FLECS_CUSTOM_STORAGE_H
#define FLECS_CUSTOM_STORAGE_H

#include "flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_custom_storage_t {
    void *(*init)(ecs_world_t *world, ecs_entity_t component);
    void (*fini)(ecs_world_t *world, ecs_entity_t component, void *storage);
    void *(*get)(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity);
    void (*set)(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity, void *value);
    void (*remove)(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity);
} ecs_custom_storage_t;

FLECS_API
void ecs_register_custom_storage(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_custom_storage_t *storage);

#ifdef __cplusplus
}
#endif

#endif
