/**
 * @file storage.c
 * @brief Implementation for pluggable storage API.
 */

#include "../private_api.h"
#include "flecs.h"

/* Component storage index */
static
ecs_hashmap_t* get_storage_map(
    const ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    
    /* Get or create storage map if it doesn't exist yet */
    world = ecs_get_world(world);
    ecs_hashmap_t *storage_map = ECS_CONST_CAST(ecs_hashmap_t*, world->store.storages);
    
    if (!storage_map) {
        ecs_world_t *mutable_world = ECS_CONST_CAST(ecs_world_t*, world);
        mutable_world->store.storages = storage_map = 
            ecs_os_calloc_t(ecs_hashmap_t);
        flecs_hashmap_init_(storage_map, sizeof(ecs_entity_t), sizeof(ecs_storage_record_t*),
            0, NULL, NULL);
    }
    
    return storage_map;
}

static
void storage_record_free(
    ecs_storage_record_t *sr)
{
    if (sr->hooks.ctx_free && sr->hooks.ctx) {
        sr->hooks.ctx_free(sr->hooks.ctx);
    }

    if (sr->hooks.fini) {
        sr->hooks.fini(NULL, sr->component, sr->storage);
    }

    ecs_os_free(sr);
}

/* Register custom storage for component */
void ecs_set_storage_hooks(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_storage_hooks_t *storage_hooks)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage_hooks != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage_hooks->init != NULL, ECS_INVALID_PARAMETER, 
        "storage hooks must have init function");

    ecs_hashmap_t *storage_map = get_storage_map(world);
    
    /* Create and initialize storage record */
    ecs_storage_record_t *sr = ecs_os_calloc_t(ecs_storage_record_t);
    
    /* Copy hooks */
    sr->hooks = *storage_hooks;
    sr->component = component;
    
    /* Initialize storage */
    if (sr->hooks.init) {
        sr->storage = sr->hooks.init(world, component);
    }
    
    /* Register storage with component */
    flecs_hashmap_set(storage_map, &component, sizeof(ecs_entity_t), &sr, sizeof(ecs_storage_record_t*));
}

/* Legacy function for backward compatibility */
void ecs_register_custom_storage(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_custom_storage_t *storage)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    
    /* Create storage hooks from custom storage */
    ecs_storage_hooks_t hooks = {
        .init = storage->init,
        .fini = storage->fini,
        .get = (void*(*)(const ecs_world_t*, ecs_entity_t, ecs_entity_t, void*))storage->get,
        .set = (void(*)(ecs_world_t*, ecs_entity_t, ecs_entity_t, const void*, void*))storage->set,
        .remove = (void(*)(ecs_world_t*, ecs_entity_t, ecs_entity_t, void*))storage->remove
    };
    
    /* Register with new API */
    ecs_set_storage_hooks(world, component, &hooks);
}

/* Get storage for component */
void* ecs_get_storage(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    
    /* Get storage record */
    ecs_hashmap_t *storage_map = get_storage_map(world);
    ecs_storage_record_t **sr_ptr = flecs_hashmap_get(
        storage_map, &component, ecs_storage_record_t*);
    
    if (!sr_ptr) {
        return NULL;
    }
    
    return (*sr_ptr)->storage;
}

/* Cleanup storage map during world cleanup */
void flecs_storage_fini(
    ecs_world_t *world)
{
    ecs_hashmap_t *storage_map = world->store.storages;
    if (!storage_map) {
        return;
    }

    ecs_map_iter_t it = ecs_map_iter(&storage_map->impl);
    while (ecs_map_next(&it)) {
        ecs_storage_record_t *sr = *(ecs_storage_record_t**)ecs_map_value(&it);
        storage_record_free(sr);
    }

    flecs_hashmap_fini(storage_map);
    ecs_os_free(storage_map);
    world->store.storages = NULL;
}

/* Hook into table creation system to handle custom storage */
void flecs_storage_table_create(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_hashmap_t *storage_map = world->store.storages;
    if (!storage_map) {
        return;
    }
    
    /* Check if table has any components with custom storage */
    ecs_type_t type = table->type;
    for (int i = 0; i < type.count; i++) {
        ecs_id_t id = type.array[i];
        /* Check if component has custom storage */
        ecs_storage_record_t **sr_ptr = flecs_hashmap_get(
            storage_map, &id, ecs_storage_record_t*);
        
        if (sr_ptr) {
            /* Mark table as having custom storage */
            table->flags |= EcsTableHasCustomStorage;
            
            /* Add component to list of components with custom storage */
            if (!table->custom_storage_components) {
                ecs_size_t count = type.count + 1;
                ecs_size_t elem_size = (ecs_size_t)sizeof(ecs_entity_t);
                ecs_size_t alloc_size = elem_size * count;
                table->custom_storage_components = ecs_os_malloc(alloc_size);
                table->custom_storage_count = 0;
            }
            
            /* Store component in table metadata */
            table->custom_storage_components[table->custom_storage_count++] = id;
        }
    }
}
