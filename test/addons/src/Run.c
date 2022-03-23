#include <addons.h>

void Run_setup() {
    ecs_log_set_level(-3);
}

static
void Iter(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = NULL;
    Mass *m = NULL;

    if (it->term_count >= 2) {
        v = ecs_term(it, Velocity, 2);
    }

    if (it->term_count >= 3) {
        m = ecs_term(it, Mass, 3);
    }

    int *param = it->param;

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run(world, Iter, 1.0, NULL), 0);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.e[3], e4);
    test_int(ctx.e[4], e5);
    test_int(ctx.e[5], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_w_param() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);

    ECS_SYSTEM(world, Iter, 0, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    int param = 5;
    test_int( ecs_run(world, Iter, 1.0, &param), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 1);
    test_ptr(ctx.param, &param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 2, 0, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.e[1], e4);
    test_int(ctx.e[2], e5);
    test_int(ctx.e[3], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 0, NULL), 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e5);
    test_int(ctx.e[2], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 4, 0, NULL), 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e5);
    test_int(ctx.e[1], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 5, 0, NULL), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e6);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e6, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);
    const Velocity *v = ecs_get(world, e6, Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 5, NULL), 0);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.e[3], e4);
    test_int(ctx.e[4], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 4, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.e[3], e4);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 3, NULL), 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 1, 5, NULL), 0);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.e[1], e3);
    test_int(ctx.e[2], e4);
    test_int(ctx.e[3], e5);
    test_int(ctx.e[4], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 1, 4, NULL), 0);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.e[1], e3);
    test_int(ctx.e[2], e4);
    test_int(ctx.e[3], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 3, NULL), 0);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e5);
    test_int(ctx.e[2], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 2, NULL), 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    /* Process entities out of order so we can validate whether it is correct */
    test_int( ecs_run_w_filter(world, Iter, 1.0, 0, 1, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 2, 1, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 1, 1, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 3, 1, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 5, 1, NULL), 0);
    test_int( ecs_run_w_filter(world, Iter, 1.0, 4, 1, NULL), 0);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 6);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e3);
    test_int(ctx.e[2], e2);
    test_int(ctx.e[3], e4);
    test_int(ctx.e[4], e6);
    test_int(ctx.e[5], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 6, 1, NULL), 0);

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

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run_w_filter(world, Iter, 1.0, 5, 2, NULL), 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Run_run_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e1, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
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
void TestSubset(ecs_iter_t *it) {
    Param *param = it->param;

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(param->entity != it->entities[i]);
        param->count ++;
    }    
}

static
void TestAll(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    ecs_entity_t TestSubset = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        Param param = {.entity = it->entities[i], 0};
        ecs_run_w_filter(it->world, TestSubset, 1, it->frame_offset + i + 1, 0, &param);
        p[i].x += param.count;
    }
}

void Run_run_comb_10_entities_1_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, 0, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, TestSubset());

    int i, ENTITIES = 10;

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, ids[i], Position, {1, 2});
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        const Position *p = ecs_get(world, ids[i], Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void Run_run_comb_10_entities_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, TestSubset, 0, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, TestSubset());

    int i, ENTITIES = 10;
    ecs_entity_t ids_1[5], ids_2[5];

    for (int i = 0; i < ENTITIES / 2; i ++) {
        ids_1[i] = ecs_set(world, 0, Position, {1, 2});
    }
    for (int i = 0; i < ENTITIES / 2; i ++) {
        ids_2[i] = ecs_set(world, 0, Position, {1, 2});
        ecs_add(world, ids_2[i], Velocity);
    } 

    ecs_progress(world, 0);

    for (i = 0; i < 5; i ++) {
        const Position *p = ecs_get(world, ids_1[i], Position);
        test_int(p->x, ENTITIES - i);

        p = ecs_get(world, ids_2[i], Position);
        test_int(p->x, ENTITIES - (i + 5));
    }

    ecs_fini(world);
}

static
void Interrupt(ecs_iter_t *it) {
    int i;
    for (i = 0; i < it->count; i ++) {
        if (i == 2) {
            it->interrupted_by = it->entities[i];
            break;
        }
    }
}

void Run_run_w_interrupt() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Mass, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_SYSTEM(world, Interrupt, 0, Position);

    ecs_entity_t e = ecs_run(world, Interrupt, 0, NULL);
    test_int(e, e3);
 
    ecs_fini(world);
}

static
void AddVelocity(ecs_iter_t *it) {
    ecs_world_t *world = it->world;

    Position *p = ecs_term(it, Position, 1);
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        float x = p[i].x;
        float y = p[i].y;
        
        ecs_set(world, e, Position, {x + 1, y + 2});
        const Position *p_stage = ecs_get(world, e, Position);
        test_int(p_stage->x, x);
        test_int(p_stage->y, y);

        /* Main stage isn't updated until after merge */
        test_int(p[i].x, x);
        test_int(p[i].y, y);

        ecs_set(world, e, Velocity, {1, 2});
        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v == NULL);
    }
}

void Run_run_staging() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {30, 40});

    ECS_SYSTEM(world, AddVelocity, 0, Position, Velocity());

    ecs_run(world, AddVelocity, 0, NULL);

    test_assert( ecs_has(world, e1, Position));
    test_assert( ecs_has(world, e1, Velocity));

    const Position *p = ecs_get(world, e1, Position);
    test_int(p->x, 11);
    test_int(p->y, 22);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    p = ecs_get(world, e2, Position);
    test_int(p->x, 31);
    test_int(p->y, 42);

    v = ecs_get(world, e2, Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);
 
    ecs_fini(world);
}
