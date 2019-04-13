#include <include/api.h>

static
void Iter(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_SHARED_TEST(rows, Position, p_parent, 2);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x ++;
        p[i].y ++;

        if (p_parent) {
            p[i].x += p_parent->x;
            p[i].y += p_parent->y;
        }
    }
}

void SystemCascade_cascade_depth_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, CASCADE.Position);

    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_2, Position, {1, 2});
    ecs_set(world, e_3, Position, {1, 2});
    ecs_set(world, e_4, Position, {1, 2});

    ecs_adopt(world, e_3, e_1);
    ecs_adopt(world, e_4, e_2);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_4);
    test_int(ctx.e[3], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get_ptr(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    ecs_fini(world);
}

void SystemCascade_cascade_depth_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Position);
    ECS_ENTITY(world, e_6, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, CASCADE.Position);

    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_2, Position, {1, 2});
    ecs_set(world, e_3, Position, {1, 2});
    ecs_set(world, e_4, Position, {1, 2});
    ecs_set(world, e_5, Position, {1, 2});
    ecs_set(world, e_6, Position, {1, 2});

    ecs_adopt(world, e_3, e_1);
    ecs_adopt(world, e_4, e_2);
    ecs_adopt(world, e_5, e_3);
    ecs_adopt(world, e_6, e_4);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 5);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.e[3], e_4);
    test_int(ctx.e[4], e_6);
    test_int(ctx.e[5], e_5);    
    test_int(ctx.c[0][0], ecs_to_entity(Position));
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get_ptr(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get_ptr(world, e_5, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    p = ecs_get_ptr(world, e_6, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    ecs_fini(world);
}
