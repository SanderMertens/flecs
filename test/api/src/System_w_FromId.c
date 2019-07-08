#include <api.h>

static
void Iter(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
    }
}

void System_w_FromId_2_column_1_from_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, .Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], 0);
}

void System_w_FromId_3_column_2_from_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, .Velocity, .Rotation);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_entity(Rotation));
    test_int(ctx.s[0][2], 0);    
}

static
void CheckColumnType(ecs_rows_t *rows) {
    ECS_COLUMN_COMPONENT(rows, Position, 2);

    test_assert(ecs_type(Position) == ecs_column_type(rows, 1));

    ProbeSystem(rows);
}

void System_w_FromId_column_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);

    ECS_SYSTEM(world, CheckColumnType, EcsOnUpdate, Position, .Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_new(world, Position);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
}
