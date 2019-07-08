#include <api.h>

static
void Iter(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    Velocity *v = NULL;
    Mass *m = NULL;
        
    if (rows->column_count >= 2) {
        v = ecs_column(rows, Velocity, 2);
    }

    if (rows->column_count >= 3) {
        m = ecs_column(rows, Mass, 3);
    }

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

void SystemManual_1_type_1_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Iter, EcsManual, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_run(world, Iter, 1, NULL);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_entity(Position));
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

static int normal_count;

static
void NormalSystem(ecs_rows_t *rows) {
    normal_count ++;
}

void SystemManual_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e, Position);

    ECS_SYSTEM(world, NormalSystem, EcsManual, Position);

    ecs_run(world, NormalSystem, 0, NULL);

    test_int(normal_count, 1);

    ecs_enable(world, NormalSystem, false);

    ecs_run(world, NormalSystem, 0, NULL);

    test_int(normal_count, 1);

    ecs_enable(world, NormalSystem, true);

    ecs_run(world, NormalSystem, 0, NULL);

    test_int(normal_count, 2);

    ecs_fini(world);
}
