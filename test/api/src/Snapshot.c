#include <api.h>

void Snapshot_simple_snapshot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p->x ++;
    p->y ++;

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);    

    ecs_fini(world);
}

void Snapshot_snapshot_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_entity_t e2 = ecs_new(world, Position);
    test_assert(e2 != 0);
    test_assert(ecs_has(world, e2, Position));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e2, Position));
    test_assert(ecs_new(world, 0) == e2);

    ecs_fini(world);
}

void Snapshot_snapshot_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_delete(world, e);
    test_assert(!ecs_has(world, e, Position));

    ecs_snapshot_restore(world, s);    

    test_assert(ecs_has(world, e, Position));
    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Snapshot_snapshot_after_new_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add(world, e2, Velocity);

    ecs_snapshot_restore(world, s);    

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e2, Velocity));

    ecs_fini(world);
}

void Snapshot_snapshot_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Velocity));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Snapshot_snapshot_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    
    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Velocity));

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    ecs_remove(world, e, Velocity);
    test_assert(!ecs_has(world, e, Velocity));

    ecs_snapshot_restore(world, s);

    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}
