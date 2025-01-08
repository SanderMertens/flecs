#include <addons.h>

static
void Iter(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
    }
}

void System_w_Empty_2_column_1_from_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, Velocity());

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    ecs_fini(world);
}

void System_w_Empty_3_column_2_from_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, Velocity(), Rotation());

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_entity_t e = ecs_new_w(world, Position);

    ecs_progress(world, 1);
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Rotation));
    test_int(ctx.s[0][2], 0);   

    ecs_fini(world); 
}

static
void CheckColumnType(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 1);

    test_assert(ecs_id(Position) == ecs_field_id(it, 0));

    probe_iter(it);
}

void System_w_Empty_column_type(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);

    ECS_SYSTEM(world, CheckColumnType, EcsOnUpdate, Position, Position());

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    ecs_new_w(world, Position);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);

    ecs_fini(world);
}
