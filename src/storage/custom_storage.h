/**
 * @file custom_storage.h
 * @brief Implementation for pluggable storage API.
 * 
 * This header implements the private functionality needed to support the
 * public pluggable storage API.
 */

#ifndef FLECS_CUSTOM_STORAGE_H
#define FLECS_CUSTOM_STORAGE_H

#include "flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Compatibility layer for old custom_storage.h */
typedef struct ecs_custom_storage_t {
    void *(*init)(ecs_world_t *world, ecs_entity_t component);
    void (*fini)(ecs_world_t *world, ecs_entity_t component, void *storage);
    void *(*get)(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity);
    void (*set)(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity, void *value);
    void (*remove)(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity);
} ecs_custom_storage_t;

/* Cache custom storage info for component */
typedef struct ecs_storage_record_t {
    ecs_header_t hdr;
    ecs_storage_hooks_t hooks;  /* Storage hooks */
    ecs_entity_t component;     /* Component id */
    void *storage;              /* Actual storage returned by init */
} ecs_storage_record_t;

/* Register custom storage for component */
FLECS_API
void ecs_register_custom_storage(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_custom_storage_t *storage);

#ifdef __cplusplus
}
#endif

#endif
