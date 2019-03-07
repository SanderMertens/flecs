#include <include/api.h>

void InitP(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column(rows, Velocity, 2);

    ProbeSystem(rows);

    int i;
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }
    }
}

void SystemOnAdd_new_match_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_new_match_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_new_no_match_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_new_no_match_2_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_new_no_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_match_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_match_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_match_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_no_match_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_no_match_2_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Velocity, Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_no_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_set_match_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void SystemOnAdd_set_no_match_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    EcsEntity e_1 = ecs_new(world, Position);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    EcsEntity e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Velocity);

    EcsEntity e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_2, Velocity);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position, Velocity);

    EcsEntity e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_2, Velocity);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

void SystemOnAdd_add_again_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_set_again_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e, Position, {30, 40});

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_again_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, InitP, EcsOnAdd, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Type);

    test_int(ctx.count, 0);

    ecs_fini(world);
}
