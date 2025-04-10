/**
 * @file custom_storage.h
 * @brief Private storage API declarations for custom storage.
 */

#ifndef FLECS_STORAGE_CUSTOM_STORAGE_H
#define FLECS_STORAGE_CUSTOM_STORAGE_H

#include "flecs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Storage record for custom component storage */
typedef struct ecs_storage_record_t {
    ecs_storage_hooks_t hooks;  /* Storage hooks */
    ecs_entity_t component;     /* Component entity */
    void *storage;             /* User storage pointer */
} ecs_storage_record_t;

/* These functions are used for creating tables with custom storage */
void flecs_storage_fini(ecs_world_t *world);
void flecs_storage_table_create(ecs_world_t *world, ecs_table_t *table);

/**
 * @brief Legacy custom storage interface.
 *
 * This is kept for backward compatibility with old code. New code should use
 * ecs_storage_hooks_t instead.
 */
typedef struct ecs_custom_storage_t {
    void* (*init)(
        ecs_world_t *world,
        ecs_entity_t component);
    
    void (*fini)(
        ecs_world_t *world,
        ecs_entity_t component,
        void *storage);
    
    void* (*get)(
        const ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        void *storage);
    
    void (*set)(
        ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        const void *value,
        void *storage);
    
    void (*remove)(
        ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        void *storage);
    
    void (*on_add)(
        ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        void *value,
        void *storage);
        
    void (*on_remove)(
        ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        void *storage);
} ecs_custom_storage_t;

/* Legacy register function - use ecs_set_storage_hooks instead */
void ecs_register_custom_storage(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_custom_storage_t *storage);

#ifdef __cplusplus
}
#endif

#endif /* FLECS_STORAGE_CUSTOM_STORAGE_H */
