#include <addons.h>

void Run_setup(void) {
    ecs_log_set_level(-3);
}

static
void Iter(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    Velocity *v = NULL;
    Mass *m = NULL;

    if (it->field_count >= 2) {
        v = ecs_field(it, Velocity, 1);
    }

    if (it->field_count >= 3) {
        m = ecs_field(it, Mass, 2);
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

void Run_run(void) {
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
    ecs_set_ctx(world, &ctx, NULL);

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

void Run_run_w_param(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);

    ECS_SYSTEM(world, Iter, 0, Position);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

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

void Run_run_w_interrupt(void) {
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

    Position *p = ecs_field(it, Position, 0);
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];

        float x = p[i].x;
        float y = p[i].y;
        
        ecs_set(world, e, Position, {x + 1, y + 2});
        const Position *p_stage = ecs_get(world, e, Position);
        test_int(p_stage->x, x + 1);
        test_int(p_stage->y, y + 2);

        /* Main stage is updated immediately */
        test_int(p[i].x, x + 1);
        test_int(p[i].y, y + 2);

        ecs_set(world, e, Velocity, {1, 2});
        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v == NULL);
    }
}

void Run_run_staging(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {30, 40}));

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

void Run_run_no_match(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e1, Position);

    ECS_SYSTEM(world, Iter, 0, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    /* Ensure system is not run by ecs_progress */
    ecs_progress(world, 1);
    test_int(ctx.invoked, 0);

    test_int( ecs_run(world, Iter, 1.0, NULL), 0);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    ecs_fini(world);
}
