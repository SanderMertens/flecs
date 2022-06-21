#include <addons.h>

void SystemManual_setup() {
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

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
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
    ECS_SYSTEM(world, Iter, 0, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_run(world, Iter, 1, NULL);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

static int normal_count;

static
void NormalSystem(ecs_iter_t *it) {
    normal_count ++;
}

static
void AddVelocity(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add(it->world, it->entities[i], Velocity);
    }
}

void SystemManual_no_automerge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, AddVelocity, 0, Position, Velocity());

    ECS_ENTITY(world, e1, Position);

    ecs_set_automerge(world, false);

    ecs_readonly_begin(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);

    ecs_run(stage, AddVelocity, 1, NULL);

    test_assert(!ecs_has(stage, e1, Velocity));

    ecs_readonly_end(world);

    test_assert(!ecs_has(world, e1, Velocity));

    ecs_merge(world);

    test_assert(ecs_has(world, e1, Velocity));

    ecs_fini(world);
}

static int dummy_ran = 0;

void DummySystem(ecs_iter_t *it) {
    ecs_entity_t Tag = ecs_term_id(it, 1);
    ecs_add_id(it->world, Tag, Tag);
    dummy_ran ++;
}

void SystemManual_dont_run_w_unmatching_entity_query() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, DummySystem, 0, !Tag($));

    ecs_run(world, DummySystem, 0, NULL);
    test_int(dummy_ran, 1);

    dummy_ran = 0;

    ecs_run(world, DummySystem, 0, NULL);
    test_int(dummy_ran, 0);

    ecs_fini(world);
}
