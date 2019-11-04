#include <api.h>

void Set_set_empty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_nonempty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Velocity, {10, 20});
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    Velocity *v = ecs_get_ptr(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);
    
    ecs_fini(world);
}

void Set_set_non_empty_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e, Position, {30, 40});
    test_assert(ecs_has(world, e, Position));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);
    
    ecs_fini(world);
}

void Set_set_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_set(world, e, Velocity, {30, 40});
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);
    
    ecs_fini(world);
}

void Set_add_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_add_other() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p == NULL);
    
    ecs_fini(world);
}

void Set_set_remove_other() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_remove_twice() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_remove(world, e, Position);
    test_assert(!ecs_has(world, e, Position));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p == NULL);

    ecs_set(world, e, Position, {10, 20});
    test_assert(ecs_has(world, e, Position));

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Set_set_and_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Set_set_null() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_set_ptr(world, e, Position, NULL);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 0);

    ecs_fini(world);
}
