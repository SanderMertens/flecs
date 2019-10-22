#include <api.h>

void Singleton_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_singleton(world, Position, {10, 20});

    Position *p = ecs_get_singleton_ptr(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Singleton_set_ptr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    Position p_value = {10, 20};

    ecs_set_singleton_ptr(world, Position, &p_value);

    Position *p = ecs_get_singleton_ptr(world, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    
    ecs_fini(world);
}

void Iter_w_singleton(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);
    test_assert(!v || ecs_is_shared(rows, 2));

    ProbeSystem(rows);

    if (v) {
        int i;
        for (i = 0; i < rows->count; i ++) {
            p[i].x += v->x;
            p[i].y += v->y;
        }
    }
}

void Singleton_system_w_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Iter_w_singleton, EcsOnUpdate, Position, $.Velocity);

    ecs_set_singleton(world, Velocity, {1, 2});
    test_assert( ecs_has(world, ECS_SINGLETON, Velocity));

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert( !ecs_has(world, ECS_SINGLETON, Position));

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    
    ecs_fini(world);
}

void Singleton_system_w_singleton_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Iter_w_singleton, EcsOnUpdate, Position, $.Velocity);

    ecs_set(world, 0, Position, {10, 20});
    test_assert( !ecs_has(world, ECS_SINGLETON, Position));

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    
    ecs_fini(world);
}

void Singleton_system_w_not_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Iter_w_singleton, EcsOnUpdate, Position, !$.Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    
    ecs_fini(world);
}

void Singleton_lookup_singleton() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup(world, "$");
    test_assert(e == ECS_SINGLETON);

    ecs_fini(world);
}

void Singleton_get_singleton_id() {
    ecs_world_t *world = ecs_init();

    test_str( ecs_get_id(world, ECS_SINGLETON), "$");

    ecs_fini(world);
}
