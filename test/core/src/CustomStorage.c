#include <core.h>
#include "../../../src/storage/custom_storage.h"
#include "../../../src/storage/storage.c"
#include <flecs/datastructures/map.h>
#include "../../../src/private_api.h"
#include <string.h>

typedef struct {
    int value;
} CustomComponent;

typedef struct {
    ecs_map_t storage;
} CustomStorage;

// Correct signature for init
void* custom_storage_init(
    ecs_world_t *world,
    ecs_entity_t component)
{
    CustomStorage *storage = ecs_os_calloc_t(CustomStorage); // Use calloc for zero-init
    ecs_map_init(&storage->storage, &world->allocator);
    return storage;
}

// Correct signature for fini
void custom_storage_fini(
    ecs_world_t *world,
    ecs_entity_t component,
    void *storage)
{
    CustomStorage *custom_storage = storage;

    // Iterate and free stored components before finishing the map
    ecs_map_iter_t it = ecs_map_iter(&custom_storage->storage);
    while (ecs_map_next(&it)) {
        CustomComponent *comp_ptr = (CustomComponent*)(uintptr_t)ecs_map_value(&it);
        if (comp_ptr) {
            ecs_os_free(comp_ptr);
        }
    }

    ecs_map_fini(&custom_storage->storage); // Use ecs_map_fini
    ecs_os_free(custom_storage);
}

// Correct signature for get
void* custom_storage_get(
    const ecs_world_t *world,
    ecs_entity_t component,
    ecs_entity_t entity,
    void *storage)
{
    CustomStorage *custom_storage = storage;
    ecs_map_val_t *val_ptr = ecs_map_get(&custom_storage->storage, entity); // Pass map pointer
    if (!val_ptr) {
        return NULL; // Not found
    }
    // Return the stored pointer (cast from uint64_t)
    return (void*)(uintptr_t)*val_ptr;
}

// Correct signature for set
void custom_storage_set(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_entity_t entity,
    const void *value, // Add const
    void *storage) // Add storage param
{
    CustomStorage *custom_storage = storage; // Use storage param
    ecs_map_val_t* val_ptr = ecs_map_get(&custom_storage->storage, entity);
    CustomComponent* dst_ptr = NULL;

    if (val_ptr && *val_ptr != 0) {
        // Component already exists, reuse allocation
        dst_ptr = (CustomComponent*)(uintptr_t)*val_ptr;
    } else {
        // Component doesn't exist, allocate new
        dst_ptr = ecs_os_malloc(sizeof(CustomComponent));
        // Insert the new pointer into the map
        ecs_map_insert(&custom_storage->storage, entity, (ecs_map_val_t)(uintptr_t)dst_ptr);
    }

    // Copy data from value to the storage location
    memcpy(dst_ptr, value, sizeof(CustomComponent));
}


void custom_storage_remove(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_entity_t entity,
    void *storage)
{
    CustomStorage *custom_storage = storage; // Use storage param
    ecs_map_val_t* val_ptr = ecs_map_get(&custom_storage->storage, entity); // Get pointer to value slot

    if (val_ptr && *val_ptr != 0) {
        // Free the component data if it exists
        CustomComponent* comp_ptr = (CustomComponent*)(uintptr_t)*val_ptr;
        ecs_os_free(comp_ptr);
    }

    // Remove entry from map
    ecs_map_remove(&custom_storage->storage, entity);
}

void CustomStorage_test() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, CustomComponent);

    ecs_custom_storage_t custom_storage_hooks = {
        .init = custom_storage_init,
        .fini = custom_storage_fini,
        .get = custom_storage_get,
        .set = custom_storage_set,
        .remove = custom_storage_remove
    };

    ecs_register_custom_storage(world, ecs_id(CustomComponent), &custom_storage_hooks);

    // Use ecs_new_w_id to add component on creation
    ecs_entity_t e = ecs_new_w_id(world, ecs_id(CustomComponent));
    CustomComponent *c = (CustomComponent*)ecs_get(world, e, CustomComponent);
    test_assert(c != NULL);

    c->value = 10;
    ecs_modified(world, e, CustomComponent); // Notify Flecs of modification

    // Cast result of ecs_get
    CustomComponent *c2 = (CustomComponent*)ecs_get(world, e, CustomComponent);
    test_assert(c2 != NULL);
    test_int(c2->value, 10);

    // Test remove
    ecs_remove(world, e, CustomComponent);
    CustomComponent *c3 = (CustomComponent*)ecs_get(world, e, CustomComponent);
    test_assert(c3 == NULL); // Should be NULL after removal

    ecs_fini(world);
}
