#include <api.h>

static
void Iter(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Mass, m_ptr, 1);

    Position *p = NULL;
    Velocity *v = NULL;

    if (rows->column_count >= 2) {
        p = ecs_column(rows, Position, 2);
    }

    if (rows->column_count >= 3) {
        v = ecs_column(rows, Velocity, 3);
    }

    test_assert(!m_ptr || ecs_is_shared(rows, 1));

    ProbeSystem(rows);

    Mass m = 1;
    if (m_ptr) {
        m = *m_ptr;
    }

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 10 * m;
        p[i].y = 20 * m;

        if (v) {
            v[i].x = 30 * m;
            v[i].y = 40 * m;
        }
    }
}

void System_w_FromEntity_2_column_1_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Mass);
    ECS_ENTITY(world, e_2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, e_1.Mass, Position);

    ecs_set(world, e_1, Mass, {5});

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], e_1);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 50);
    test_int(p->y, 100);

    ecs_fini(world);
}

static bool dummy_invoked = 0;
static ecs_entity_t dummy_component = 0;
static ecs_entity_t dummy_source = 0;

static
void dummy_reset() {
    dummy_invoked = false;
    dummy_component = 0;
    dummy_source = 0;
}

static
void Dummy(ecs_rows_t *rows) {
    dummy_invoked = 1;
    dummy_component = ecs_column_entity(rows, 1);
    dummy_source = ecs_column_source(rows, 1);
}

void System_w_FromEntity_task_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, e_1.Position);

    ecs_progress(world, 1);

    test_bool(dummy_invoked, true);
    test_assert(dummy_component == ecs_entity(Position));
    test_assert(dummy_source == e_1);

    dummy_reset();
    ecs_remove(world, e_1, Position);

    ecs_progress(world, 1);
    test_bool(dummy_invoked, false);

    ecs_fini(world);
}

void System_w_FromEntity_task_not_from_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, !e_1.Position);

    ecs_progress(world, 1);

    test_bool(dummy_invoked, false);

    ecs_remove(world, e_1, Position);

    ecs_progress(world, 1);

    test_bool(dummy_invoked, true);
    test_assert(dummy_component == ecs_entity(Position));
    test_assert(dummy_source == e_1);

    ecs_fini(world);
}
