#include <include/api.h>

void Init(ecs_rows_t *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column_test(rows, Velocity, 2);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }
    }
}

void SystemOnAdd_new_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_new_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_new_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_new_no_match_2_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_new_no_match_2_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_match_2_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_match_2_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_add_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_no_match_2_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_SYSTEM(world, Init, EcsOnAdd, Velocity, Mass);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_no_match_2_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_add(world, e, Type);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_set_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void SystemOnAdd_set_no_match_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {1, 2});

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    ecs_entity_t e_1 = ecs_new(world, Position);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_1_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_2_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Velocity);

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_2, Velocity);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

void SystemOnAdd_clone_match_2_of_3() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position, Velocity);

    ecs_entity_t e_1 = ecs_new(world, Type);
    test_assert(e_1 != 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_2 = ecs_clone(world, e_1, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_2, Velocity);
    test_int(v->x, 30);
    test_int(v->y, 40);

    ecs_fini(world);
}

void SystemOnAdd_add_again_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_set_again_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Position, {10, 20});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_set(world, e, Position, {30, 40});

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_add_again_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_add(world, e, Type);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnAdd_new_w_count_match_1_of_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Init, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_count(world, Position, 3, NULL);
    test_assert(e != 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Init);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.e[1], e + 1);
    test_int(ctx.e[2], e + 2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}
