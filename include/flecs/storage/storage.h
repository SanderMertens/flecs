/**
 * @file storage.h
 * @brief Pluggable storage API for custom component storage.
 */

#ifndef FLECS_STORAGE_H
#define FLECS_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations for types used in this header */
typedef struct ecs_world_t ecs_world_t;
typedef uint64_t ecs_entity_t;

/** Storage hooks type that stores callback functions for custom storages. */
typedef struct ecs_storage_hooks_t {
    /** Initialize the storage for a component type. 
     * This function is called when a component with custom storage is registered.
     * 
     * @param world The world.
     * @param component The component entity id.
     * @return A pointer to the storage implementation.
     */
    void* (*init)(
        ecs_world_t *world,
        ecs_entity_t component);
    
    /** Finalize the storage for a component type.
     * This function is called when the world is destroyed.
     * 
     * @param world The world.
     * @param component The component entity id.
     * @param storage The storage object returned by the init function.
     */
    void (*fini)(
        ecs_world_t *world,
        ecs_entity_t component,
        void *storage);
    
    /** Get component data for a specific entity.
     * 
     * @param world The world.
     * @param component The component entity id.
     * @param entity The entity from which to get the component.
     * @param storage The storage object returned by the init function.
     * @return Pointer to the component data.
     */
    void* (*get)(
        const ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        void *storage);
    
    /** Set component data for a specific entity.
     * 
     * @param world The world.
     * @param component The component entity id.
     * @param entity The entity for which to set the component.
     * @param value The component data to set.
     * @param storage The storage object returned by the init function.
     */
    void (*set)(
        ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        const void *value,
        void *storage);
    
    /** Remove component data for a specific entity.
     * 
     * @param world The world.
     * @param component The component entity id.
     * @param entity The entity for which to remove the component.
     * @param storage The storage object returned by the init function.
     */
    void (*remove)(
        ecs_world_t *world,
        ecs_entity_t component,
        ecs_entity_t entity,
        void *storage);

    /** Optional callback to iterate over all entities with this component.
     * 
     * @param world The world.
     * @param component The component entity id.
     * @param storage The storage object returned by the init function.
     * @param callback Callback to invoke for each entity-component pair.
     * @param ctx User data to pass to the callback.
     */
    void (*each)(
        ecs_world_t *world,
        ecs_entity_t component,
        void *storage,
        void (*callback)(ecs_entity_t, void*, void*),
        void *ctx);
    
    /** User context that will be passed to the hooks */
    void *ctx;
    
    /** Function to free the ctx */
    void (*ctx_free)(void *ctx);
} ecs_storage_hooks_t;

/** Register custom storage hooks for a component */
FLECS_API 
void ecs_set_storage_hooks(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_storage_hooks_t *storage_hooks);

/** Get storage object for a component */
FLECS_API
void* ecs_get_storage(
    const ecs_world_t *world,
    ecs_entity_t component);

#ifdef __cplusplus
}
#endif

#endif /* FLECS_STORAGE_H */
