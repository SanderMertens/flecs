#include <include/api.h>

static
void Iter(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column_test(rows, Velocity, 2);
    Mass *m = ecs_column_test(rows, Mass, 3);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }

        if (m) {
            m[i] = 50;
        }
    }
}

void SystemOnFrame_1_type_1_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnFrame_1_type_3_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position, Velocity, Mass);
    ECS_ENTITY(world, e_2, Position, Velocity, Mass);
    ECS_ENTITY(world, e_3, Position, Velocity, Mass);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Velocity, Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    
    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);


    Mass *m = ecs_get_ptr(world, e_1, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    m = ecs_get_ptr(world, e_2, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    m = ecs_get_ptr(world, e_3, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    ecs_fini(world);
}

void SystemOnFrame_3_type_1_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Mass);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[1][0], EPosition);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[2][0], EPosition);
    test_int(ctx.s[2][0], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnFrame_2_type_3_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity, Mass);
    ECS_ENTITY(world, e_2, Position, Velocity, Mass);
    ECS_ENTITY(world, e_3, Position, Velocity, Mass, Rotation);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Velocity, Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], 0);
    test_int(ctx.c[1][0], EPosition);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], EVelocity);
    test_int(ctx.s[1][1], 0);
    test_int(ctx.c[1][2], EMass);
    test_int(ctx.s[1][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    
    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);


    Mass *m = ecs_get_ptr(world, e_1, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    m = ecs_get_ptr(world, e_2, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    m = ecs_get_ptr(world, e_3, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    ecs_fini(world);
}

void SystemOnFrame_1_type_1_component_1_tag() {
    EcsWorld *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position, Tag);
    ECS_ENTITY(world, e_2, Position, Tag);
    ECS_ENTITY(world, e_3, Position, Tag);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Tag);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ETag);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnFrame_2_type_1_component_1_tag() {
    EcsWorld *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position, Tag);
    ECS_ENTITY(world, e_2, Position, Tag);
    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Tag);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ETag);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}


void SystemOnFrame_2_type_1_and_1_not() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, !Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnFrame_2_type_2_and_1_not() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity, Mass);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Velocity, !Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], 0);
    
    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

void SystemOnFrame_2_type_2_and_2_not() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_3, Position, Velocity, Mass);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Velocity, !Mass, !Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 4);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], 0);
    test_int(ctx.c[0][3], ERotation);
    test_int(ctx.s[0][3], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

typedef Position Position_1;
typedef Position Position_2;

void SystemOnFrame_4_type_1_and_1_or() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Position_1);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position_1, Velocity);
    ECS_ENTITY(world, e_3, Position, Position_1, Velocity);
    ECS_ENTITY(world, e_4, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position | Position_1, Velocity);

    ecs_set(world, e_3, Position_1, {0, 0});
    ecs_set(world, e_4, Velocity, {0, 0});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);

    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], EPosition_1);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], EVelocity);
    test_int(ctx.s[1][1], 0);
    test_int(ctx.c[2][0], EPosition);
    test_int(ctx.s[2][0], 0);
    test_int(ctx.c[2][1], EVelocity);
    test_int(ctx.s[2][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position_1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position_1);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 0);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_4, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 0);
    test_int(v->y, 0);

    ecs_fini(world);
}

void SystemOnFrame_4_type_1_and_1_or_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Position_1);
    ECS_COMPONENT(world, Position_2);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position_2, Velocity);
    ECS_ENTITY(world, e_3, Position_1, Position_2, Velocity);
    ECS_ENTITY(world, e_4, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position | Position_1 | Position_2, Velocity);

    ecs_set(world, e_3, Position_2, {0, 0});
    ecs_set(world, e_4, Velocity, {0, 0});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);

    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], EPosition_2);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], EVelocity);
    test_int(ctx.s[1][1], 0);
    test_int(ctx.c[2][0], EPosition_1);
    test_int(ctx.s[2][0], 0);
    test_int(ctx.c[2][1], EVelocity);
    test_int(ctx.s[2][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position_2);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position_1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position_2);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 0);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_4, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 0);
    test_int(v->y, 0);

    ecs_fini(world);
}

void SystemOnFrame_1_type_1_and_1_or() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Position_1);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position | Position_1, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    /* Now repeat test, with entities that have Position_1 */
    ctx = (SysTestData){0};

    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    ECS_ENTITY(world, e_3, Position_1, Velocity);
    ECS_ENTITY(world, e_4, Position_1, Velocity);
    
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_3);
    test_int(ctx.e[1], e_4);
    test_int(ctx.c[0][0], EPosition_1);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

void SystemOnFrame_2_type_1_and_1_optional() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, ?Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], EPosition);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], EVelocity);
    test_int(ctx.s[1][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

void SystemOnFrame_2_type_2_and_1_optional() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position, Velocity, Mass);
    ECS_ENTITY(world, e_2, Position, Velocity, Mass);
    ECS_ENTITY(world, e_3, Position, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, Velocity, ?Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], 0);
    test_int(ctx.c[1][0], EPosition);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], EVelocity);
    test_int(ctx.s[1][1], 0);
    test_int(ctx.c[1][2], EMass);
    test_int(ctx.s[1][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    Mass *m = ecs_get_ptr(world, e_1, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    ecs_fini(world);
}

void SystemOnFrame_6_type_1_and_2_optional() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity, Mass);
    ECS_ENTITY(world, e_3, Position, Mass);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Velocity);
    ECS_ENTITY(world, e_6, Mass);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position, ?Velocity, ?Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 4);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.e[3], e_4);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], EMass);
    test_int(ctx.s[0][2], 0);
    test_int(ctx.c[1][0], EPosition);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], EVelocity);
    test_int(ctx.s[1][1], 0);
    test_int(ctx.c[1][2], EMass);
    test_int(ctx.s[1][2], 0);    
    test_int(ctx.c[2][0], EPosition);
    test_int(ctx.s[2][0], 0);
    test_int(ctx.c[2][1], EVelocity);
    test_int(ctx.s[2][1], 0);
    test_int(ctx.c[2][2], EMass);
    test_int(ctx.s[2][2], 0);   
    test_int(ctx.c[3][0], EPosition);
    test_int(ctx.s[3][0], 0);
    test_int(ctx.c[3][1], EVelocity);
    test_int(ctx.s[3][1], 0);
    test_int(ctx.c[3][2], EMass);
    test_int(ctx.s[3][2], 0);   

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 30);
    test_int(v->y, 40);

    Mass *m = ecs_get_ptr(world, e_2, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    m = ecs_get_ptr(world, e_3, Mass);
    test_assert(m != NULL);
    test_int(*m, 50);

    ecs_fini(world);
}

void Use_field(EcsRows *rows) {
    int i;
    for (i = 0; i < rows->count; i ++) {
        Position *p = ecs_field(rows, Position, i, 1);
        Velocity *v = ecs_field(rows, Velocity, i, 2);

        p->x += v->x;
        p->y += v->y;
    }
}

void SystemOnFrame_use_fields_2_owned() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position, Velocity);

    ECS_SYSTEM(world, Use_field, EcsOnFrame, Position, Velocity);

    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_1, Velocity, {10, 20});

    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e_1, Position);
    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_int(p->x, 11);
    test_int(p->y, 22);
    test_int(v->x, 10);
    test_int(v->y, 20);

    ecs_fini(world);
}

void SystemOnFrame_use_fields_1_owned_1_shared() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Use_field, EcsOnFrame, Position, CONTAINER.Velocity);

    EcsEntity e_1 = ecs_set(world, 0, Position, {1, 2});
    EcsEntity parent = ecs_set(world, 0, Velocity, {10, 20});
    ecs_adopt(world, parent, e_1);

    ecs_progress(world, 1);

    Position *p = ecs_get_ptr(world, e_1, Position);
    Velocity *v = ecs_get_ptr(world, parent, Velocity);
    test_int(p->x, 11);
    test_int(p->y, 22);
    test_int(v->x, 10);
    test_int(v->y, 20);

    ecs_fini(world);
}

static void Dummy_1(EcsRows *rows) { ProbeSystem(rows); }
static void Dummy_2(EcsRows *rows) { ProbeSystem(rows); }

void SystemOnFrame_match_2_systems_w_populated_table() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, Dummy_1, EcsOnFrame, Position);
    ECS_SYSTEM(world, Dummy_2, EcsOnFrame, Position);
    
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    test_int(ctx.e[0], e);
    test_int(ctx.e[1], e);

    ecs_fini(world);
}

void TestOptional_w_column(EcsRows *rows) {
    Position *p = ecs_column_test(rows, Position, 1);
    Velocity *v = ecs_column_test(rows, Velocity, 2);

    test_assert(p != NULL);
    test_assert(v == NULL);

    ProbeSystem(rows);
}

void TestOptional_w_shared(EcsRows *rows) {
    Position *p = ecs_column_test(rows, Position, 1);
    Velocity *v = ecs_shared_test(rows, Velocity, 2);

    test_assert(p != NULL);
    test_assert(v == NULL);

    ProbeSystem(rows);
}

void TestOptional_w_field(EcsRows *rows) {
    Position *p = ecs_column_test(rows, Position, 1);

    test_assert(p != NULL);

    ProbeSystem(rows);

    for (int i = 0; i < rows->count; i ++) {
        Velocity *v = ecs_field(rows, Velocity, i, 2);
        test_assert(v == NULL);
    }
}

void SystemOnFrame_ensure_optional_is_null_column() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, TestOptional_w_column, EcsOnFrame, Position, ?Velocity);
    
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    test_int(ctx.e[0], e);

    ecs_fini(world);
}

void SystemOnFrame_ensure_optional_is_null_shared() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, TestOptional_w_shared, EcsOnFrame, Position, ?CONTAINER.Velocity);
    
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    test_int(ctx.e[0], e);

    ecs_fini(world);
}

void SystemOnFrame_ensure_optional_is_null_field_owned() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, TestOptional_w_field, EcsOnFrame, Position, ?Velocity);
    
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    test_int(ctx.e[0], e);

    ecs_fini(world);
}

void SystemOnFrame_ensure_optional_is_null_field_shared() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, TestOptional_w_field, EcsOnFrame, Position, ?CONTAINER.Velocity);
    
    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    test_int(ctx.e[0], e);

    ecs_fini(world);
}