#include <include/api.h>

void Deinit(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column(rows, Velocity, 2);

    ProbeSystem(rows);

    /* Write to validate columns point to valid memory */
    int i;
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        p[i].x = 0;
        p[i].y = 0;

        if (v) {
            v[i].x = 0;
            v[i].y = 0;
        }
    }
}

void SystemOnRemove_remove_match_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    ecs_fini(world);
}

void SystemOnRemove_remove_match_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    ecs_fini(world);
}

void SystemOnRemove_remove_match_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);

    ecs_fini(world);
}

void SystemOnRemove_remove_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Type);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);

    ecs_fini(world);
}

void SystemOnRemove_remove_no_match_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Velocity);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnRemove_remove_no_match_2_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Velocity);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnRemove_remove_no_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_remove(world, e, Type);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void SystemOnRemove_delete_match_1_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    ecs_fini(world);
}

void SystemOnRemove_delete_match_1_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);

    ecs_fini(world);
}

void SystemOnRemove_delete_match_2_of_2() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);    

    ecs_fini(world);
}

void SystemOnRemove_delete_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 1);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0], ePosition);
    test_int(ctx.s[0], 0);
    test_int(ctx.c[1], eVelocity);
    test_int(ctx.s[1], 0);    

    ecs_fini(world);
}

void SystemOnRemove_delete_no_match_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 0); 

    ecs_fini(world);
}

void SystemOnRemove_delete_no_match_2_of_1() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Velocity);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 0); 

    ecs_fini(world);
}

void SystemOnRemove_delete_no_match_2_of_3() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);
    ECS_TYPE(world, Type, Position, Velocity, Mass);
    ECS_SYSTEM(world, Deinit, EcsOnRemove, Position, Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);

    test_int(ctx.count, 0);

    ecs_delete(world, e);

    test_int(ctx.count, 0); 

    ecs_fini(world);
}
