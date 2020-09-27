#include <api.h>

void Delete_setup() {
    ecs_tracing_enable(-3);
}

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
    const ecs_entity_t *ids = ecs_bulk_new(it->world, Position, 10);
    test_assert(ids != NULL);
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

    test_int( ecs_count(world, Position), 0);
    test_int(on_remove_system_invoked, 10);
    
    ecs_fini(world);
}

void Delete_clear_1_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_clear(world, e);
    test_assert(!ecs_get_type(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 > e);
    
    ecs_fini(world);
}

void Delete_clear_2_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    ecs_add(world, e, Velocity);
    test_assert(e != 0);

    ecs_clear(world, e);
    test_assert(!ecs_get_type(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 > e);
    
    ecs_fini(world);
}

void Delete_alive_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_delete(world, e);
    
    test_assert(!ecs_get_type(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_alive_after_clear() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_clear(world, e);

    test_assert(!ecs_get_type(world, e));
    test_assert(ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_alive_after_staged_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    ecs_delete(world, e);
    ecs_defer_end(world);
    
    test_assert(!ecs_get_type(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_alive_while_staged() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    test_assert(ecs_is_alive(world, e));
    ecs_defer_end(world);
    
    ecs_fini(world);
}

void Delete_alive_while_staged_w_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    test_assert(ecs_is_alive(world, e));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_alive_while_staged_w_delete_recycled_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_delete(world, e);
    e = ecs_new(world, 0);
    test_assert(ecs_is_alive(world, e));

    ecs_defer_begin(world);
    test_assert(ecs_is_alive(world, e));
    ecs_delete(world, e);
    test_assert(ecs_is_alive(world, e));
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_alive_after_recycle() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(ecs_is_alive(world, e2));
    test_assert(e != e2);
    test_assert((ECS_ENTITY_MASK & e) == (ECS_ENTITY_MASK & e2));
    
    ecs_fini(world);
}
