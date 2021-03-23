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

    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_delete_1_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_delete_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_exists(world, e));
    test_assert(!ecs_is_alive(world, e));
    
    ecs_fini(world);
}

void Delete_delete_nonexist() {
    ecs_world_t *world = ecs_init();

    ecs_delete(world, 100);
    test_assert(!ecs_exists(world, 100));
    test_assert(!ecs_is_alive(world, 100));
    
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
    test_assert(ecs_exists(world, e1));
    test_assert(!ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(ecs_is_alive(world, e3));

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

    test_assert(ecs_exists(world, e1));
    test_assert(ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(ecs_is_alive(world, e3));
    
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

    test_assert(ecs_exists(world, e1));
    test_assert(ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(!ecs_is_alive(world, e3));
    
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

    test_assert(ecs_exists(world, e1));
    test_assert(ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(!ecs_is_alive(world, e3));
    
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

    test_assert(ecs_exists(world, e1));
    test_assert(!ecs_is_alive(world, e1));

    test_assert(ecs_exists(world, e2));
    test_assert(!ecs_is_alive(world, e2));

    test_assert(ecs_exists(world, e3));
    test_assert(!ecs_is_alive(world, e3));

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

    ECS_SYSTEM(world, CreateEntity, EcsOnUpdate, [out] :Position);
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
    
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));
    
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
    test_assert(ecs_exists(world, e));
    
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
    
    test_assert(ecs_exists(world, e));
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
    test_assert(ecs_exists(world, e));
    
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
    test_assert(ecs_exists(world, e));
    
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
    test_assert(ecs_exists(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(ecs_is_alive(world, e2));
    test_assert(e != e2);
    test_assert((ECS_ENTITY_MASK & e) == (ECS_ENTITY_MASK & e2));
    
    ecs_fini(world);
}

void Delete_delete_recycled() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e != e2);
    test_assert((e2 & ECS_ENTITY_MASK) == (e & ECS_ENTITY_MASK));
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, e2));

    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_is_alive(world, e2));

    ecs_fini(world);
}

void Delete_get_alive_for_alive() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    ecs_entity_t a = ecs_get_alive(world, e);
    test_assert(a != 0);
    test_assert(e == a);

    ecs_fini(world);
}

void Delete_get_alive_for_recycled() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    e = ecs_new(world, 0);
    test_assert(ecs_entity_t_lo(e) != e); // Ensure id is recycled

    ecs_entity_t a = ecs_get_alive(world, ecs_entity_t_lo(e));
    test_assert(a != 0);
    test_assert(e == a);

    ecs_fini(world);
}

void Delete_get_alive_for_not_alive() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    // Ensure id has no generation
    test_assert(ecs_entity_t_lo(e) == e);

    ecs_entity_t a = ecs_get_alive(world, e);
    test_assert(a == 0);

    ecs_fini(world);
}

void Delete_get_alive_w_generation_for_recycled_alive() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    e = ecs_new(world, 0);
    test_assert(ecs_entity_t_lo(e) != e);

    ecs_entity_t a = ecs_get_alive(world, e);
    test_assert(a != 0);
    test_assert(e == a);

    ecs_fini(world);
}

void Delete_get_alive_w_generation_for_recycled_not_alive() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    
    ecs_delete(world, e);
    test_assert(!ecs_is_alive(world, e));
    test_assert(ecs_exists(world, e));

    e = ecs_new(world, 0);
    test_assert(ecs_entity_t_lo(e) != e);

    ecs_delete(world, e);

    test_expect_abort();

    // Will assert, because function is called with generation that is not alive
    ecs_get_alive(world, e);
}

void Delete_get_alive_for_0() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();
    ecs_get_alive(world, 0);
}

void Delete_get_alive_for_nonexistent() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_get_alive(world, 1000);
    test_assert(e == 0);

    ecs_fini(world);
}
