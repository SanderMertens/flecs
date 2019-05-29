#include <api.h>

void Delete_delete_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);

    test_assert(ecs_is_empty(world, e));
    
    ecs_fini(world);
}

void Delete_delete_1_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_is_empty(world, e));

    ecs_delete(world, e);
    test_assert(ecs_is_empty(world, e));
    
    ecs_fini(world);
}

void Delete_delete_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_delete(world, e);
    test_assert(ecs_is_empty(world, e));
    
    ecs_fini(world);
}

void Delete_delete_nonexist() {
    ecs_world_t *world = ecs_init();

    ecs_delete(world, 100);
    test_assert(ecs_is_empty(world, 100));
    
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
    test_assert(ecs_is_empty(world, e1));
    test_assert(!ecs_is_empty(world, e2));
    test_assert(!ecs_is_empty(world, e3));
    
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
    test_assert(!ecs_is_empty(world, e1));
    test_assert(ecs_is_empty(world, e2));
    test_assert(!ecs_is_empty(world, e3));
    
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
    test_assert(!ecs_is_empty(world, e1));
    test_assert(!ecs_is_empty(world, e2));
    test_assert(ecs_is_empty(world, e3));
    
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
    test_assert(!ecs_is_empty(world, e1));
    test_assert(ecs_is_empty(world, e2));
    test_assert(ecs_is_empty(world, e3));
    
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
    test_assert(ecs_is_empty(world, e1));
    test_assert(ecs_is_empty(world, e2));
    test_assert(ecs_is_empty(world, e3));
}
