#include <include/api.h>

static
void Iter(ecs_rows_t *rows) {
    Mass *m_ptr = ecs_shared_test(rows, Mass, 1);
    Position *p = ecs_column(rows, Position, 2);
    Velocity *v = ecs_column_test(rows, Velocity, 3);

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

void System_w_FromContainer_1_column_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, CONTAINER.Mass, Position);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_adopt(world, parent, e_1);
    ecs_adopt(world, parent, e_2);

    ecs_adopt(world, parent, e_3);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_to_entity(Position));
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_2_column_1_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position, Velocity);
    ECS_ENTITY(world, e_2, Position, Velocity);
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, CONTAINER.Mass, Position, Velocity);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_adopt(world, parent, e_1);
    ecs_adopt(world, parent, e_2);
    ecs_adopt(world, parent, e_3);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_to_entity(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_to_entity(Velocity));
    test_int(ctx.s[0][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    Velocity *v = ecs_get_ptr(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    v = ecs_get_ptr(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    ecs_fini(world);
}

static
void Iter_2_shared(ecs_rows_t *rows) {
    Mass *m_ptr = ecs_shared_test(rows, Mass, 1);
    Rotation *r_ptr = ecs_shared_test(rows, Rotation, 2);
    Position *p = ecs_column_test(rows, Position, 3);
    Velocity *v = ecs_column_test(rows, Velocity, 4);

    ProbeSystem(rows);

    Mass m = 1;
    if (m_ptr) {
        m = *m_ptr;
    }

    Rotation r = 0;
    if (r_ptr) {
        r = *r_ptr;
    }

    int i;
    for (i = 0; i < rows->count; i ++) {
        if (p) {
            p[i].x = 10 * m + r;
            p[i].y = 20 * m + r;
        }

        if (v) {
            v[i].x = 30 * m + r;
            v[i].y = 40 * m + r;
        }
    }
}

void System_w_FromContainer_3_column_2_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter_2_shared, EcsOnFrame, CONTAINER.Mass, CONTAINER.Rotation, Position);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_set(world, parent, Rotation, {3});

    ecs_adopt(world, parent, e_1);
    ecs_adopt(world, parent, e_2);
    ecs_adopt(world, parent, e_4);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_2_shared);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_4);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_to_entity(Rotation));
    test_int(ctx.s[0][1], parent);
    test_int(ctx.c[0][2], ecs_to_entity(Position));
    test_int(ctx.s[0][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get_ptr(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    ecs_fini(world);
}

void System_w_FromContainer_3_column_2_from_different_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Position);
    ECS_ENTITY(world, e_6, Position);

    ECS_SYSTEM(world, Iter_2_shared, EcsOnFrame, CONTAINER.Mass, CONTAINER.Rotation, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});

    ecs_adopt(world, parent_1, e_1);
    ecs_adopt(world, parent_2, e_1);

    ecs_adopt(world, parent_1, e_2);
    ecs_adopt(world, parent_2, e_2);

    ecs_adopt(world, parent_1, e_3);
    ecs_adopt(world, parent_2, e_3);

    /* e_4 and e_5 should not be matched */
    ecs_adopt(world, parent_1, e_4);
    ecs_adopt(world, parent_2, e_5);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_2_shared);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], parent_1);
    test_int(ctx.c[0][1], ecs_to_entity(Rotation));
    test_int(ctx.s[0][1], parent_2);
    test_int(ctx.c[0][2], ecs_to_entity(Position));
    test_int(ctx.s[0][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    ecs_fini(world);
}

void System_w_FromContainer_2_column_1_from_container_w_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, !CONTAINER.Mass, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});

    ecs_adopt(world, parent_2, e_1);
    ecs_adopt(world, parent_2, e_2);
    ecs_adopt(world, parent_2, e_3);
    ecs_adopt(world, parent_1, e_4);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_5);
    test_int(ctx.e[1], e_1);
    test_int(ctx.e[2], e_2);
    test_int(ctx.e[3], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_to_entity(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], ecs_to_entity(Mass));
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], ecs_to_entity(Position));
    test_int(ctx.s[1][1], 0);

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

    p = ecs_get_ptr(world, e_5, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void System_w_FromContainer_3_column_1_from_comtainer_1_from_container_w_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Position);
    ECS_ENTITY(world, e_6, Position);

    ECS_SYSTEM(world, Iter_2_shared, EcsOnFrame, !CONTAINER.Mass, CONTAINER.Rotation, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});
    ecs_entity_t parent_3 = ecs_set(world, 0, Mass, {4});
    ecs_set(world, parent_3, Rotation, {5});

    ecs_adopt(world, parent_2, e_1);
    ecs_adopt(world, parent_2, e_2);
    ecs_adopt(world, parent_2, e_3);
    ecs_adopt(world, parent_1, e_4);
    ecs_adopt(world, parent_3, e_5);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_2_shared);
    test_int(ctx.column_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_to_entity(Rotation));
    test_int(ctx.s[0][1], parent_2);
    test_int(ctx.c[0][2], ecs_to_entity(Position));
    test_int(ctx.s[0][2], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    ecs_fini(world);
}

void System_w_FromContainer_2_column_1_from_container_w_or() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Position);
    ECS_ENTITY(world, e_6, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, CONTAINER.Mass | CONTAINER.Rotation, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});
    ecs_entity_t parent_3 = ecs_set(world, 0, Rotation, {4});
        ecs_set(world, parent_3, Mass, {5});
    ecs_entity_t parent_4 = ecs_set(world, 0, Velocity, {10, 20});

    ecs_adopt(world, parent_1, e_1);
    ecs_adopt(world, parent_2, e_2);
    ecs_adopt(world, parent_3, e_3);
    ecs_adopt(world, parent_4, e_4);
    ecs_adopt(world, parent_2, e_5); 
    ecs_adopt(world, parent_3, e_5);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 4);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.e[3], e_5);
    test_int(ctx.c[0][0], ecs_to_entity(Mass));
    test_int(ctx.s[0][0], parent_1);
    test_int(ctx.c[0][1], ecs_to_entity(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], ecs_to_entity(Rotation));
    test_int(ctx.s[1][0], parent_2);
    test_int(ctx.c[1][1], ecs_to_entity(Position));
    test_int(ctx.s[2][1], 0);
    test_int(ctx.c[2][0], ecs_to_entity(Rotation));
    test_int(ctx.s[2][0], parent_3);
    test_int(ctx.c[2][1], ecs_to_entity(Position));
    test_int(ctx.s[2][1], 0);
    test_int(ctx.c[3][0], ecs_to_entity(Rotation));
    test_int(ctx.s[3][0], parent_2);
    test_int(ctx.c[3][1], ecs_to_entity(Position));
    test_int(ctx.s[3][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get_ptr(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 40);
    test_int(p->y, 80);

    p = ecs_get_ptr(world, e_5, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}
