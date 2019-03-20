#include <include/api.h>

void Task(EcsRows *rows) {
    Position *p = ecs_shared_test(rows, Position, 1);
    
    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (p) {
            p->x = 0;
            p->y = 0;
        }
    }
}

void Tasks_no_components() {
    EcsWorld *world = ecs_init();

    ECS_SYSTEM(world, Task, EcsOnFrame, 0);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 0);

    ecs_fini(world);
}

void Tasks_one_tag() {
    EcsWorld *world = ecs_init();

    ECS_SYSTEM(world, Task, EcsOnFrame, SYSTEM.EcsHidden);

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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Task, EcsOnFrame, SYSTEM.Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 1);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], EPosition);

    ecs_fini(world);
}

void Tasks_on_remove_no_components() {
    EcsWorld *world = ecs_init();

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
    EcsWorld *world = ecs_init();

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
    EcsWorld *world = ecs_init();

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
    test_int(ctx.c[0][0], EPosition);
}
