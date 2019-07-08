#include <api.h>

void Task(ecs_rows_t *rows) {
    ProbeSystem(rows);
}

void Tasks_no_components() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Task, EcsOnUpdate, 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 0);

    ecs_fini(world);
}

void Tasks_one_tag() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Task, EcsOnUpdate, SYSTEM.EcsHidden);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], EEcsHidden);

    ecs_fini(world);
}

void Tasks_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Task, EcsOnUpdate, SYSTEM.Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], ecs_entity(Position));

    ecs_fini(world);
}

void Tasks_on_remove_no_components() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Task, EcsOnRemove, 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 0);
}

void Tasks_on_remove_one_tag() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Task, EcsOnRemove, SYSTEM.EcsHidden);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], EEcsHidden);
}

void Tasks_on_remove_from_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Task, EcsOnRemove, SYSTEM.Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], ecs_entity(Position));
}
