#include <api.h>

static
void Iter(ecs_iter_t *it) {
    ECS_COLUMN(it, Mass, m_ptr, 1);
    bool shared = false;
    
    if (m_ptr) {
        shared = !ecs_is_owned(it, 1);
    }

    Position *p = NULL;
    Velocity *v = NULL;

    if (it->column_count >= 2) {
        p = ecs_column(it, Position, 2);
    }

    if (it->column_count >= 3) {
        v = ecs_column(it, Velocity, 3);
    }

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        Mass m = 1;
        if (m_ptr) {
            if (shared) {
                m = *m_ptr;
            } else {
                m = m_ptr[i];
            }
        }
        
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

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_3, Position);
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

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position, Velocity);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_entity(Velocity));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    const Velocity *v = ecs_get(world, e_1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    v = ecs_get(world, e_2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    v = ecs_get(world, e_3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    ecs_fini(world);
}

static
void Iter_2_shared(ecs_iter_t *it) {
    ECS_COLUMN(it, Mass, m_ptr, 1);

    Rotation *r_ptr = NULL;
    Position *p = NULL;
    Velocity *v = NULL;

    if (it->column_count >= 2) {
        r_ptr = ecs_column(it, Rotation, 2);
    }

    if (it->column_count >= 3) {
        p = ecs_column(it, Position, 3);
    }

    if (it->column_count >= 4) {
        v = ecs_column(it, Velocity, 4);
    }    

    probe_system(it);

    Mass m = 1;
    if (m_ptr) {
        m = *m_ptr;
    }

    Rotation r = 0;
    if (r_ptr) {
        r = *r_ptr;
    }

    int i;
    for (i = 0; i < it->count; i ++) {
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

    ECS_SYSTEM(world, Iter_2_shared, EcsOnUpdate, PARENT:Mass, PARENT:Rotation, Position);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_set(world, parent, Rotation, {3});

    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Rotation));
    test_int(ctx.s[0][1], parent);
    test_int(ctx.c[0][2], ecs_entity(Position));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get(world, e_4, Position);
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

    ECS_SYSTEM(world, Iter_2_shared, EcsOnUpdate, PARENT:Mass, PARENT:Rotation, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});

    ecs_add_entity(world, e_1, ECS_CHILDOF | parent_1);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent_2);

    ecs_add_entity(world, e_2, ECS_CHILDOF | parent_1);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent_2);

    ecs_add_entity(world, e_3, ECS_CHILDOF | parent_1);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent_2);

    /* e_4 and e_5 should not be matched */
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent_1);
    ecs_add_entity(world, e_5, ECS_CHILDOF | parent_2);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent_1);
    test_int(ctx.c[0][1], ecs_entity(Rotation));
    test_int(ctx.s[0][1], parent_2);
    test_int(ctx.c[0][2], ecs_entity(Position));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get(world, e_3, Position);
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

    ECS_SYSTEM(world, Iter, EcsOnUpdate, !PARENT:Mass, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});

    ecs_add_entity(world, e_1, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent_1);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], ecs_entity(Mass));
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], ecs_entity(Position));
    test_int(ctx.s[1][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e_5, Position);
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

    ECS_SYSTEM(world, Iter_2_shared, EcsOnUpdate, !PARENT:Mass, PARENT:Rotation, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});
    ecs_entity_t parent_3 = ecs_set(world, 0, Mass, {4});
    ecs_set(world, parent_3, Rotation, {5});

    ecs_add_entity(world, e_1, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent_1);
    ecs_add_entity(world, e_5, ECS_CHILDOF | parent_3);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Rotation));
    test_int(ctx.s[0][1], parent_2);
    test_int(ctx.c[0][2], ecs_entity(Position));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    ecs_fini(world);
}

void System_w_FromContainer_2_column_1_from_container_w_not_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Rotation);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, !PARENT:Mass, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});

    ecs_add_entity(world, e_1, ECS_INSTANCEOF | Prefab);

    ecs_add_entity(world, e_1, ECS_CHILDOF | parent_1);
    ecs_set(world, e_1, Position, {1, 2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

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

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass || PARENT:Rotation, Position);

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});
    ecs_entity_t parent_3 = ecs_set(world, 0, Rotation, {4});
                            ecs_set(world, parent_3, Mass, {5});
    ecs_entity_t parent_4 = ecs_set(world, 0, Velocity, {10, 20});

    ecs_add_entity(world, e_1, ECS_CHILDOF | parent_1);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent_3);
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent_4);
    ecs_add_entity(world, e_5, ECS_CHILDOF | parent_2);
    ecs_add_entity(world, e_5, ECS_CHILDOF | parent_3);

    Probe ctx = {0};
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
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent_1);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], ecs_entity(Rotation));
    test_int(ctx.s[1][0], parent_2);
    test_int(ctx.c[1][1], ecs_entity(Position));
    test_int(ctx.s[2][1], 0);
    test_int(ctx.c[2][0], ecs_entity(Rotation));
    test_int(ctx.s[2][0], parent_3);
    test_int(ctx.c[2][1], ecs_entity(Position));
    test_int(ctx.s[2][1], 0);
    test_int(ctx.c[3][0], ecs_entity(Rotation));
    test_int(ctx.s[3][0], parent_2);
    test_int(ctx.c[3][1], ecs_entity(Position));
    test_int(ctx.s[3][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 40);
    test_int(p->y, 80);

    p = ecs_get(world, e_5, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}

static
void Dummy(ecs_iter_t *it) {
    ECS_COLUMN(it, Mass, m_ptr, 1);
    ECS_COLUMN(it, Position, p, 2);

    test_assert(!m_ptr || !ecs_is_owned(it, 1));

    probe_system(it);

    Mass m = 1;
    if (m_ptr) {
        m = *m_ptr;
    }

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += m;
        p[i].y += m;
    }
}

void System_w_FromContainer_add_component_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_add_component_after_match_and_rematch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* This will rematch tables, but not match Iter with e_1 and e_2 because the
     * parent does not have Mass yet */
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    ecs_set(world, parent, Mass, {2});

    /* Now a rematch of tables need to happen again, since parent has changed */
    ctx = (Probe){0};
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, e_1, Position, CHILDOF | Parent);
    ECS_ENTITY(world, e_2, Position, CHILDOF | Parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* This will rematch tables, but not match Iter with e_1 and e_2 because the
     * Parent does not have Mass yet */
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    ecs_set(world, Parent, Mass, {2});

    /* Now a rematch of tables need to happen again, since parent has changed */
    ctx = (Probe){0};
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], Parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

static
void SetMass(ecs_iter_t *it) {
    ECS_COLUMN_COMPONENT(it, Mass, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Mass, {2});
    }
}

void System_w_FromContainer_add_component_after_match_and_rematch_w_entity_type_expr_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);     

    ECS_SYSTEM(world, SetMass, EcsOnUpdate, Velocity, :Mass);
    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ECS_ENTITY(world, Parent, Velocity);
    ECS_ENTITY(world, e_1, Position, CHILDOF | Parent);
    ECS_ENTITY(world, e_2, Position, CHILDOF | Parent);  

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* This will rematch tables, but not match Iter with e_1 and e_2 because the
     * Parent does not have Mass yet */
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    /* Now a rematch of tables need to happen again, since parent has changed */
    ctx = (Probe){0};
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], Parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_add_component_after_match_unmatch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, !PARENT:Mass, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_3);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void System_w_FromContainer_add_component_after_match_unmatch_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, !PARENT:Mass, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_add_component_after_match_2_systems() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);
    ECS_SYSTEM(world, Dummy, EcsOnUpdate, PARENT:Mass, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Dummy);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_1);
    test_int(ctx.e[3], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 22);
    test_int(p->y, 42);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 22);
    test_int(p->y, 42);

    ecs_fini(world);
}

static
void AddMass(ecs_iter_t *it) {
    ecs_entity_t ecs_entity(Mass) = *(ecs_entity_t*)it->param;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Mass, {2});
    }
}

void System_w_FromContainer_add_component_in_progress_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, Tag, 0);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);
    ECS_TRIGGER(world, AddMass, EcsOnAdd, Tag);

    ecs_set(world, AddMass, EcsContext, {&ecs_entity(Mass)});

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    ecs_add_entity(world, parent, Tag);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_adopt_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ecs_entity_t parent = ecs_new(world, Mass);
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromContainer_new_child_after_match() {
        ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ECS_ENTITY(world, parent, Mass);
    ECS_ENTITY(world, e_1, CHILDOF | parent, Position);
    ECS_ENTITY(world, e_2, CHILDOF | parent, Position);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void IterSame(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p_parent, 1);
    Position *p = ecs_column(it, Position, 2);

    test_assert(!ecs_is_owned(it, 1));

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += p_parent->x;
        p[i].y += p_parent->y;
    }
}

void System_w_FromContainer_select_same_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, IterSame, EcsOnUpdate, PARENT:Position, Position);

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_2, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_3, ECS_CHILDOF | parent);

    ecs_set(world, e_1, Position, {10, 20});
    ecs_set(world, e_2, Position, {20, 40});
    ecs_set(world, e_3, Position, {30, 60});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, IterSame);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], e_3);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 21);
    test_int(p->y, 42);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 31);
    test_int(p->y, 62);

    ecs_fini(world);
}

void System_w_FromContainer_realloc_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e_1, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, PARENT:Mass, Position);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_add_entity(world, e_1, ECS_CHILDOF | parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    /* Add 1000 entities to table of container, which will trigger realloc */
    ecs_type_t parent_type = ecs_get_type(world, parent);
    test_assert(parent_type != 0);

    ecs_bulk_new_w_type(world, parent_type, 1000);

    /* Change value of parent Mass. This will update the value in the new table.
     * If the realloc would not be properly handled, the code could either crash
     * or reference freed memory and use the old value. */
    ecs_set(world, parent, Mass, {3});

    ctx = (Probe){0};

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_entity(Position));
    test_int(ctx.s[0][1], 0);

    p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}
