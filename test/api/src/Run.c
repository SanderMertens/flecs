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

    int *param = rows->param;

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (param) {
            p[i].x += *param;
            p[i].y += *param;
        }

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }

        if (m) {
            m[i] = 50;
        }
    }
}

void Run_run() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run(world, Iter, 1.0, NULL), 0);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.e[3], e_4);
    test_int(ctx.e[4], e_5);
    test_int(ctx.e[5], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_param() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    int param = 5;
    test_int( ecs_run(world, Iter, 1.0, &param), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 1);
    test_ptr(ctx.param, &param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_int(p->x, 15);
    test_int(p->y, 25);

    ecs_fini(world);
}

void Run_run_w_offset() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 2, 0, 0, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_3);
    test_int(ctx.e[1], e_4);
    test_int(ctx.e[2], e_5);
    test_int(ctx.e[3], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_skip_1_archetype() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 0, 0, NULL), 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_4);
    test_int(ctx.e[1], e_5);
    test_int(ctx.e[2], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_skip_1_archetype_plus_one() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 4, 0, 0, NULL), 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_5);
    test_int(ctx.e[1], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_skip_2_archetypes() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 5, 0, 0, NULL), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_6);

    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_6, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);
    Velocity *v = ecs_get_ptr(world, e_6, Velocity);
    test_int(v->x, 30);
    test_int(v->y, 40);        

    ecs_fini(world);
}

void Run_run_w_limit_skip_1_archetype() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 5, 0, NULL), 0);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.e[3], e_4);
    test_int(ctx.e[4], e_5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_limit_skip_1_archetype_minus_one() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 4, 0, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.e[3], e_4);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_limit_skip_2_archetypes() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 3, 0, NULL), 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_1_limit_max() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 1, 5, 0, NULL), 0);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.e[1], e_3);
    test_int(ctx.e[2], e_4);
    test_int(ctx.e[3], e_5);
    test_int(ctx.e[4], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_1_limit_minus_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 1, 4, 0, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.e[1], e_3);
    test_int(ctx.e[2], e_4);
    test_int(ctx.e[3], e_5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_2_type_limit_max() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 3, 0, NULL), 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_4);
    test_int(ctx.e[1], e_5);
    test_int(ctx.e[2], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_2_type_limit_minus_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 2, 0, NULL), 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_4);
    test_int(ctx.e[1], e_5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_limit_1_all_offsets() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    /* Process entities out of order so we can validate whether it is correct */
    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 1, 0, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 2, 1, 0, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 1, 1, 0, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 1, 0, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 5, 1, 0, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 4, 1, 0, NULL), 0);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 6);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_3);
    test_int(ctx.e[2], e_2);
    test_int(ctx.e[3], e_4);
    test_int(ctx.e[4], e_6);
    test_int(ctx.e[5], e_5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_offset_out_of_bounds() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 6, 1, 0, NULL), 0);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

void Run_run_w_limit_out_of_bounds() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 5, 2, 0, NULL), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_component_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 0, Mass, NULL), 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_4);
    test_int(ctx.e[1], e_5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_type_filter_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type, Mass, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Mass, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 0, Type, NULL), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_entity(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        Velocity *v = ecs_get_ptr(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_container_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type, Mass, Rotation);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity, Mass);
    ECS_ENTITY(world, e_5, Position, Velocity, Mass);
    ECS_ENTITY(world, e_6, Position, Velocity, Mass, Rotation);
    ECS_ENTITY(world, e_7, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Create a parent entity */
    ecs_entity_t parent = ecs_new(world, 0);

    /* Adopt child entities */
    ecs_adopt(world, e_1, parent);
    ecs_adopt(world, e_4, parent);
    ecs_adopt(world, e_6, parent);
    ecs_adopt(world, e_7, parent);

    /* Get type from parent to use as filter */
    ecs_type_t TParent = ecs_type_from_entity(world, parent);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 0, Parent, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 4);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_6);
    test_int(ctx.e[1], e_7);
    test_int(ctx.e[2], e_1);
    test_int(ctx.e[3], e_4);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_entity(Position));
        test_int(ctx.s[i][0], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        Position *p = ecs_get_ptr(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);       
    }

    ecs_fini(world);
}

void Run_run_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e_1, Position);

    ECS_SYSTEM(world, Iter, EcsManual, Position, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run(world, Iter, 1.0, NULL), 0);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}

typedef struct Param {
    ecs_entity_t entity;
    int count;
} Param;

static
void TestSubset(ecs_rows_t *rows) {
    Param *param = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_assert(param->entity != rows->entities[i]);
        param->count ++;
    }    
}

static
void TestAll(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    ecs_entity_t TestSubset = ecs_column_entity(rows, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        Param param = {.entity = rows->entities[i], 0};
        ecs_run_w_filter(rows->world, TestSubset, 1, rows->frame_offset + i + 1, 0, 0, &param);
        p[i].x += param.count;
    }
}

void Run_run_comb_10_entities_1_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, .TestSubset);

    int i, ENTITIES = 10;

    ecs_entity_t start = ecs_new_w_count(world, Position, ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void Run_run_comb_10_entities_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, .TestSubset);

    int i, ENTITIES = 10;

    ecs_entity_t start = ecs_new_w_count(world, Position, ENTITIES / 2);
    ecs_new_w_count(world, Type, ENTITIES / 2);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}
