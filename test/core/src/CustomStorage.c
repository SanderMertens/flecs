#include <core.h>
#include "storage/custom_storage.h"

typedef struct {
    int value;
} CustomComponent;

typedef struct {
    ecs_map_t *storage;
} CustomStorage;

void* custom_storage_init(ecs_world_t *world, ecs_entity_t component) {
    CustomStorage *storage = ecs_os_malloc(sizeof(CustomStorage));
    storage->storage = ecs_map_new(ecs_entity_t, sizeof(CustomComponent));
    return storage;
}

void custom_storage_fini(ecs_world_t *world, ecs_entity_t component, void *storage) {
    CustomStorage *custom_storage = storage;
    ecs_map_free(custom_storage->storage);
    ecs_os_free(custom_storage);
}

void* custom_storage_get(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity) {
    CustomStorage *custom_storage = ecs_get_storage(world, component);
    return ecs_map_get(custom_storage->storage, CustomComponent, entity);
}

void custom_storage_set(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity, void *value) {
    CustomStorage *custom_storage = ecs_get_storage(world, component);
    ecs_map_set(custom_storage->storage, entity, value);
}

void custom_storage_remove(ecs_world_t *world, ecs_entity_t component, ecs_entity_t entity) {
    CustomStorage *custom_storage = ecs_get_storage(world, component);
    ecs_map_remove(custom_storage->storage, entity);
}

void CustomStorage_test() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, CustomComponent);

    ecs_custom_storage_t custom_storage = {
        .init = custom_storage_init,
        .fini = custom_storage_fini,
        .get = custom_storage_get,
        .set = custom_storage_set,
        .remove = custom_storage_remove
    };

    ecs_register_custom_storage(world, ecs_id(CustomComponent), &custom_storage);

    ecs_entity_t e = ecs_new(world, CustomComponent);
    CustomComponent *c = ecs_get(world, e, CustomComponent);
    test_assert(c != NULL);

    c->value = 10;
    ecs_modified(world, e, CustomComponent);

    CustomComponent *c2 = ecs_get(world, e, CustomComponent);
    test_assert(c2 != NULL);
    test_int(c2->value, 10);

    ecs_fini(world);
}
