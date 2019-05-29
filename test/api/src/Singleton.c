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
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v_shared = ecs_shared(rows, Velocity, 2);

    if (v_shared) {
        int i;
        for (i = 0; i < rows->count; i ++) {
            p[i].x += v_shared->x;
            p[i].y += v_shared->y;
        }
    }
}

void Singleton_system_w_singleton() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Iter_w_singleton, EcsOnUpdate, Position, $Velocity);

    ecs_set_singleton(world, Velocity, {1, 2});

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});

    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    
    ecs_fini(world);
}
