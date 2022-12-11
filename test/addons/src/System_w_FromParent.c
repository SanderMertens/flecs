#include <addons.h>

void System_w_FromParent_setup() {
    ecs_log_set_level(-3);
}

static
void Iter(ecs_iter_t *it) {
    Mass *m_ptr = ecs_field(it, Mass, 1);
    bool shared = false;
    
    if (m_ptr) {
        shared = !ecs_field_is_self(it, 1);
    }

    Position *p = NULL;
    Velocity *v = NULL;

    if (it->field_count >= 2) {
        p = ecs_field(it, Position, 2);
    }

    if (it->field_count >= 3) {
        v = ecs_field(it, Velocity, 3);
    }

    probe_iter(it);

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

void System_w_FromParent_1_column_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_2_column_1_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position, Velocity);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[0][2], ecs_id(Velocity));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    v = ecs_get(world, e2, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    v = ecs_get(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 60);
    test_int(v->y, 80);

    ecs_fini(world);
}

static
void Iter_2_shared(ecs_iter_t *it) {
    Mass *m_ptr = ecs_field(it, Mass, 1);

    Rotation *r_ptr = NULL;
    Position *p = NULL;
    Velocity *v = NULL;

    if (it->field_count >= 2) {
        r_ptr = ecs_field(it, Rotation, 2);
    }

    if (it->field_count >= 3) {
        p = ecs_field(it, Position, 3);
    }

    if (it->field_count >= 4) {
        v = ecs_field(it, Velocity, 4);
    }    

    probe_iter(it);

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

void System_w_FromParent_3_column_2_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter_2_shared, EcsOnUpdate, Mass(parent), Rotation(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter_2_shared,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_set(world, parent, Rotation, {3});

    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e4, EcsChildOf, parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_2_shared);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e4);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Rotation));
    test_int(ctx.s[0][1], parent);
    test_int(ctx.c[0][2], ecs_id(Position));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 23);
    test_int(p->y, 43);

    ecs_fini(world);
}

void System_w_FromParent_2_column_1_from_container_w_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, !Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});

    ecs_add_pair(world, e1, EcsChildOf, parent_2);
    ecs_add_pair(world, e2, EcsChildOf, parent_2);
    ecs_add_pair(world, e3, EcsChildOf, parent_2);
    ecs_add_pair(world, e4, EcsChildOf, parent_1);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e5);
    test_int(ctx.e[1], e1);
    test_int(ctx.e[2], e2);
    test_int(ctx.e[3], e3);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], ecs_id(Mass));
    test_int(ctx.s[1][0], 0);
    test_int(ctx.c[1][1], ecs_id(Position));
    test_int(ctx.s[1][1], 0);

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

    p = ecs_get(world, e5, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void System_w_FromParent_3_column_1_from_comtainer_1_from_container_w_not() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Position);
    ECS_ENTITY(world, e6, Position);

    ECS_SYSTEM(world, Iter_2_shared, EcsOnUpdate, !Mass(parent), Rotation(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter_2_shared,
        .query.filter.instanced = true
    });

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});
    ecs_entity_t parent_3 = ecs_set(world, 0, Mass, {4});
    ecs_set(world, parent_3, Rotation, {5});

    ecs_add_pair(world, e1, EcsChildOf, parent_2);
    ecs_add_pair(world, e2, EcsChildOf, parent_2);
    ecs_add_pair(world, e3, EcsChildOf, parent_2);
    ecs_add_pair(world, e4, EcsChildOf, parent_1);
    ecs_add_pair(world, e5, EcsChildOf, parent_3);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter_2_shared);
    test_int(ctx.term_count, 3);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Rotation));
    test_int(ctx.s[0][1], parent_2);
    test_int(ctx.c[0][2], ecs_id(Position));
    test_int(ctx.s[0][2], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);

    ecs_fini(world);
}

void System_w_FromParent_2_column_1_from_container_w_not_prefab() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_PREFAB(world, Prefab, Rotation);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, !Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_add_pair(world, e1, EcsIsA, Prefab);

    ecs_add_pair(world, e1, EcsChildOf, parent_1);
    ecs_set(world, e1, Position, {1, 2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void System_w_FromParent_2_column_1_from_container_w_or() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Position);
    ECS_ENTITY(world, e6, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent) || Rotation(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent_1 = ecs_set(world, 0, Mass, {2});
    ecs_entity_t parent_2 = ecs_set(world, 0, Rotation, {3});
    ecs_entity_t parent_3 = ecs_set(world, 0, Rotation, {4});
                            ecs_set(world, parent_3, Mass, {5});
    ecs_entity_t parent_4 = ecs_set(world, 0, Velocity, {10, 20});

    ecs_add_pair(world, e1, EcsChildOf, parent_1);
    ecs_add_pair(world, e2, EcsChildOf, parent_2);
    ecs_add_pair(world, e3, EcsChildOf, parent_3);
    ecs_add_pair(world, e4, EcsChildOf, parent_4);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent_1);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);
    test_int(ctx.c[1][0], ecs_id(Rotation));
    test_int(ctx.s[1][0], parent_2);
    test_int(ctx.c[1][1], ecs_id(Position));
    test_int(ctx.s[2][1], 0);
    test_int(ctx.c[2][0], ecs_id(Mass));
    test_int(ctx.s[2][0], parent_3);
    test_int(ctx.c[2][1], ecs_id(Position));
    test_int(ctx.s[2][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 50);
    test_int(p->y, 100);

    ecs_fini(world);
}

static
void Dummy(ecs_iter_t *it) {
    Mass *m_ptr = ecs_field(it, Mass, 1);
    Position *p = ecs_field(it, Position, 2);

    test_assert(!m_ptr || !ecs_field_is_self(it, 1));

    probe_iter(it);

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

void System_w_FromParent_add_component_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_add_component_after_match_and_rematch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* This will rematch tables, but not match Iter with e1 and e2 because the
     * parent does not have Mass yet */
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    ecs_set(world, parent, Mass, {2});

    /* Now a rematch of tables need to happen again, since parent has changed */
    ecs_os_zeromem(&ctx);
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_add_component_after_match_and_rematch_w_entity_type_expr() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, e1, Position, (ChildOf, Parent));
    ECS_ENTITY(world, e2, Position, (ChildOf, Parent));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* This will rematch tables, but not match Iter with e1 and e2 because the
     * Parent does not have Mass yet */
    ecs_progress(world, 1);
    test_int(ctx.count, 0);

    ecs_set(world, Parent, Mass, {2});

    /* Now a rematch of tables need to happen again, since parent has changed */
    ecs_os_zeromem(&ctx);
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], Parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

static
void SetMass(ecs_iter_t *it) {
    ecs_id_t ecs_id(Mass) = ecs_field_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Mass, {2});
    }
}

void System_w_FromParent_add_component_after_match_and_rematch_w_entity_type_expr_in_progress() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);     

    ECS_SYSTEM(world, SetMass, EcsOnUpdate, Velocity, Mass());
    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ECS_ENTITY(world, Parent, Velocity);
    ECS_ENTITY(world, e1, Position, (ChildOf, Parent));
    ECS_ENTITY(world, e2, Position, (ChildOf, Parent));  

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_os_zeromem(&ctx);
    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], Parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_add_component_after_match_unmatch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, !Mass(parent), Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void System_w_FromParent_add_component_after_match_unmatch_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, !Mass(parent), Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_add_component_after_match_2_systems() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Dummy,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, Dummy);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e1);
    test_int(ctx.e[3], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 22);
    test_int(p->y, 42);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 22);
    test_int(p->y, 42);

    ecs_fini(world);
}

static
void AddMass(ecs_iter_t *it) {
    test_assert(it->ctx != NULL);
    ecs_entity_t ecs_id(Mass) = *(ecs_entity_t*)it->ctx;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Mass, {2});
    }
}

void System_w_FromParent_add_component_in_progress_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);
    ECS_ENTITY(world, Tag, 0);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ECS_OBSERVER(world, AddMass, EcsOnAdd, Tag);

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .entity = AddMass,
        .ctx = &ecs_id(Mass)
    });

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_add_id(world, parent, Tag);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_adopt_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_new(world, Mass);
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void System_w_FromParent_new_child_after_match() {
        ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e3, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = Iter,
        .query.filter.instanced = true
    });
    
    ECS_ENTITY(world, parent, Mass);
    ECS_ENTITY(world, e1, (ChildOf, parent), Position);
    ECS_ENTITY(world, e2, (ChildOf, parent), Position);

    ecs_set(world, parent, Mass, {2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    ecs_fini(world);
}

void IterSame(ecs_iter_t *it) {
    Position *p_parent = ecs_field(it, Position, 1);
    Position *p = ecs_field(it, Position, 2);

    test_assert(!ecs_field_is_self(it, 1));

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x += p_parent->x;
        p[i].y += p_parent->y;
    }
}

void System_w_FromParent_select_same_from_container() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, IterSame, EcsOnUpdate, Position(parent), Position);
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = IterSame,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_add_pair(world, e1, EcsChildOf, parent);
    ecs_add_pair(world, e2, EcsChildOf, parent);
    ecs_add_pair(world, e3, EcsChildOf, parent);

    ecs_set(world, e1, Position, {10, 20});
    ecs_set(world, e2, Position, {20, 40});
    ecs_set(world, e3, Position, {30, 60});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, IterSame);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 21);
    test_int(p->y, 42);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 31);
    test_int(p->y, 62);

    ecs_fini(world);
}

void System_w_FromParent_realloc_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ECS_ENTITY(world, e1, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Mass(parent), Position);

    ecs_entity_t parent = ecs_set(world, 0, Mass, {2});
    ecs_add_pair(world, e1, EcsChildOf, parent);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);

    /* Add 1000 entities to table of container, which will trigger realloc */
    for (int i = 0; i < 1000; i ++) {
        ecs_clone(world, 0, parent, false);
    }

    /* Change value of parent Mass. This will update the value in the new table.
     * If the realloc would not be properly handled, the code could either crash
     * or reference freed memory and use the old value. */
    ecs_set(world, parent, Mass, {3});

    ecs_os_zeromem(&ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.c[0][0], ecs_id(Mass));
    test_int(ctx.s[0][0], parent);
    test_int(ctx.c[0][1], ecs_id(Position));
    test_int(ctx.s[0][1], 0);

    p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}
