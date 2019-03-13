#include <include/api.h>

void InitVelocity(EcsRows *rows) {
    Velocity *v = ecs_column(rows, Velocity, 1);
    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

void InitMass(EcsRows *rows) {
    Mass *m = ecs_column(rows, Mass, 1);
    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        m[i] = 3;
    }
}

void Iter(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_shared(rows, Velocity, 2);
    Mass *m = ecs_shared(rows, Mass, 3);

    test_assert(v != NULL);

    ProbeSystem(rows);

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        p[i].x += v->x;
        p[i].y += v->y;

        if (m) {
            p[i].x += *m;
            p[i].y += *m;
        }
    }
}

void System_w_FromSystem_2_column_1_from_system() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, SYSTEM.Velocity);

    test_assert( ecs_has(world, Iter, Velocity));
    Velocity *v = ecs_get_ptr(world, Iter, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_set(world, 0, Position, {0, 0});

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], Iter);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_progress(world, 1);

    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromSystem_3_column_2_from_system() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, InitMass, EcsOnAdd, Mass);
    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, SYSTEM.Velocity, SYSTEM.Mass);

    test_assert( ecs_has(world, Iter, Velocity));
    Velocity *v = ecs_get_ptr(world, Iter, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    test_assert( ecs_has(world, Iter, Mass));
    Mass *m = ecs_get_ptr(world, Iter, Mass);
    test_assert(m != NULL);
    test_int(*m, 3);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_set(world, 0, Position, {0, 0});

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], Iter);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], Iter);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    ecs_progress(world, 1);

    test_int(p->x, 26);
    test_int(p->y, 46);

    ecs_fini(world);
}

void Iter_reactive(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_shared(rows, Velocity, 2);
    Mass *m = ecs_shared(rows, Mass, 3);

    test_assert(v != NULL);

    ProbeSystem(rows);

    int i;
    for (i = rows->begin; i < rows->end; i ++) {
        p[i].x = v->x;
        p[i].y = v->y;

        if (m) {
            p[i].x = *m;
            p[i].y = *m;
        }
    }
}

void System_w_FromSystem_2_column_1_from_system_on_add() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, Iter_reactive, EcsOnAdd, Position, SYSTEM.Velocity);

    test_assert( ecs_has(world, Iter_reactive, Velocity));
    Velocity *v = ecs_get_ptr(world, Iter_reactive, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_reactive);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], Iter_reactive);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void System_w_FromSystem_2_column_1_from_system_on_remove() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, Iter_reactive, EcsOnRemove, Position, SYSTEM.Velocity);

    test_assert( ecs_has(world, Iter_reactive, Velocity));
    Velocity *v = ecs_get_ptr(world, Iter_reactive, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_int(ctx.count, 0);

    ecs_remove(world, e, Position);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_reactive);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], Iter_reactive);

    ecs_fini(world);
}

void System_w_FromSystem_2_column_1_from_system_on_set() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, InitVelocity, EcsOnAdd, Velocity);
    ECS_SYSTEM(world, Iter_reactive, EcsOnSet, Position, SYSTEM.Velocity);

    test_assert( ecs_has(world, Iter_reactive, Velocity));
    Velocity *v = ecs_get_ptr(world, Iter_reactive, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 20);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_reactive);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], Iter_reactive);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}
