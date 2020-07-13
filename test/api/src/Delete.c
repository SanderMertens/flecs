#include <api.h>

void Delete_delete_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);

    test_assert(!ecs_get_type(world, e));
    
    ecs_fini(world);
}

void Delete_delete_1_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(!ecs_get_type(world, e));

    ecs_delete(world, e);
    test_assert(!ecs_get_type(world, e));
    
    ecs_fini(world);
}

void Delete_delete_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(!ecs_get_type(world, e));
    
    ecs_fini(world);
}

void Delete_delete_nonexist() {
    ecs_world_t *world = ecs_init();

    ecs_delete(world, 100);
    test_assert(!ecs_get_type(world, 100));
    
    ecs_fini(world);
}

void Delete_delete_1st_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e1);
    test_assert(!ecs_get_type(world, e1));
    test_assert(!!ecs_get_type(world, e2));
    test_assert(!!ecs_get_type(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_2nd_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e2);
    test_assert(!!ecs_get_type(world, e1));
    test_assert(!ecs_get_type(world, e2));
    test_assert(!!ecs_get_type(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_3rd_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e3);
    test_assert(!!ecs_get_type(world, e1));
    test_assert(!!ecs_get_type(world, e2));
    test_assert(!ecs_get_type(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_2_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e2);
    ecs_delete(world, e3);
    test_assert(!!ecs_get_type(world, e1));
    test_assert(!ecs_get_type(world, e2));
    test_assert(!ecs_get_type(world, e3));
    
    ecs_fini(world);
}

void Delete_delete_3_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);
    test_assert(!ecs_get_type(world, e1));
    test_assert(!ecs_get_type(world, e2));
    test_assert(!ecs_get_type(world, e3));

    ecs_fini(world);
}

static
void CreateEntity(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Position, 1);

    ecs_entity_t e = ecs_bulk_new(it->world, Position, 10);
    test_assert(e != 0);
    test_assert( ecs_has(it->world, e, Position));
}

static
void DeleteEntity(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_delete(it->world, it->entities[i]);
    }
}

static int on_remove_system_invoked;

static
void OnRemoveSystem(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        on_remove_system_invoked ++;
    }
}

void Delete_delete_w_on_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, CreateEntity, EcsOnUpdate, :Position);
    ECS_SYSTEM(world, DeleteEntity, EcsOnStore, Position);
    ECS_TRIGGER(world, OnRemoveSystem, EcsOnRemove, Position);

    ecs_progress(world, 0);

    test_int(on_remove_system_invoked, 10);
    
    test_int( ecs_count(world, Position), 0);
    
    ecs_fini(world);
}
