#include <addons.h>

static
void Iter(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Position *p_parent = ecs_term(it, Position, 2);

    test_assert(!p_parent || !ecs_term_is_owned(it, 2));

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
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

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, ?Position(parent|cascade));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = Iter,
        .query.filter.instanced = true
    });

    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});

    ecs_add_pair(world, e3, EcsChildOf, e1);
    ecs_add_pair(world, e4, EcsChildOf, e2);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    probe_has_entity(&ctx, e1);
    probe_has_entity(&ctx, e2);
    probe_has_entity(&ctx, e3);
    probe_has_entity(&ctx, e4);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    ecs_fini(world);
}

void SystemCascade_cascade_depth_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Position);
    ECS_ENTITY(world, e6, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, ?Position(parent|cascade));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = Iter,
        .query.filter.instanced = true
    });

    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});
    ecs_set(world, e5, Position, {1, 2});
    ecs_set(world, e6, Position, {1, 2});

    ecs_add_pair(world, e3, EcsChildOf, e1); /* depth 1 */
    ecs_add_pair(world, e4, EcsChildOf, e2); /* depth 1 */
    ecs_add_pair(world, e5, EcsChildOf, e3); /* depth 2 */
    ecs_add_pair(world, e6, EcsChildOf, e4); /* depth 2 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 5);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_assert((ctx.e[0] == e1 && ctx.e[1] == e2) || (ctx.e[0] == e2 && ctx.e[1] == e1));
    test_assert((ctx.e[2] == e3 && ctx.e[3] == e4) || (ctx.e[2] == e4 && ctx.e[3] == e3));
    test_assert((ctx.e[4] == e5 && ctx.e[5] == e6) || (ctx.e[4] == e6 && ctx.e[5] == e5));

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e5, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    p = ecs_get(world, e6, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    ecs_fini(world);
}

void SystemCascade_cascade_depth_2_new_syntax() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Position);
    ECS_ENTITY(world, e6, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, ?Position(cascade(ChildOf)));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = Iter,
        .query.filter.instanced = true
    });

    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});
    ecs_set(world, e5, Position, {1, 2});
    ecs_set(world, e6, Position, {1, 2});

    ecs_add_pair(world, e3, EcsChildOf, e1); /* depth 1 */
    ecs_add_pair(world, e4, EcsChildOf, e2); /* depth 1 */
    ecs_add_pair(world, e5, EcsChildOf, e3); /* depth 2 */
    ecs_add_pair(world, e6, EcsChildOf, e4); /* depth 2 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 5);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_assert((ctx.e[0] == e1 && ctx.e[1] == e2) || (ctx.e[0] == e2 && ctx.e[1] == e1));
    test_assert((ctx.e[2] == e3 && ctx.e[3] == e4) || (ctx.e[2] == e4 && ctx.e[3] == e3));
    test_assert((ctx.e[4] == e5 && ctx.e[5] == e6) || (ctx.e[4] == e6 && ctx.e[5] == e5));

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e5, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    p = ecs_get(world, e6, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    ecs_fini(world);
}

static
void AddParent(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Position *p_parent = ecs_term(it, Position, 2);

    test_assert(!p_parent || !ecs_term_is_owned(it, 2));

    probe_iter(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (p_parent) {
            p[i].x += p_parent->x;
            p[i].y += p_parent->y;
        }
    }
}

void SystemCascade_add_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, AddParent, EcsOnUpdate, Position, ?Position(parent|cascade));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = AddParent,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});

    ecs_add_pair(world, e3, EcsChildOf, parent); /* depth 1 */
    ecs_add_pair(world, e4, EcsChildOf, parent); /* depth 1 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    /* Before adding Position to parent, it wasn't being considered for the
     * column(parent|cascade), so tables could have been ordered randomly. Make sure
     * that queries can handle changes to depth after all tables are matched */
    ecs_set(world, parent, Position, {1, 2});

    ecs_os_zeromem(&ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, AddParent);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_assert(ctx.e[0] == e1 || ctx.e[1] == e1 || ctx.e[2] == e1);
    test_assert(ctx.e[0] == e2 || ctx.e[1] == e2 || ctx.e[2] == e2);
    test_assert(ctx.e[0] == parent || ctx.e[1] == parent || ctx.e[2] == parent);
    test_assert(ctx.e[3] == e3 || ctx.e[4] == e3);
    test_assert(ctx.e[3] == e4 || ctx.e[4] == e4);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}

void SystemCascade_adopt_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, AddParent, EcsOnUpdate, Position, ?Position(parent|cascade));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = AddParent,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    ecs_add_pair(world, e3, EcsChildOf, parent);
    ecs_add_pair(world, e4, EcsChildOf, parent);

    ecs_os_zeromem(&ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, AddParent);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], parent);
    test_int(ctx.e[3], e3);
    test_int(ctx.e[4], e4);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}

void SystemCascade_custom_relation_cascade_depth_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Acyclic);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, ?Position(cascade(Rel)));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = Iter,
        .query.filter.instanced = true
    });

    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});

    ecs_add_pair(world, e3, Rel, e1);
    ecs_add_pair(world, e4, Rel, e2);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    probe_has_entity(&ctx, e1);
    probe_has_entity(&ctx, e2);
    probe_has_entity(&ctx, e3);
    probe_has_entity(&ctx, e4);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    ecs_fini(world);
}

void SystemCascade_custom_relation_cascade_depth_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Acyclic);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);
    ECS_ENTITY(world, e5, Position);
    ECS_ENTITY(world, e6, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, ?Position(cascade(Rel)));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = Iter,
        .query.filter.instanced = true
    });

    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});
    ecs_set(world, e5, Position, {1, 2});
    ecs_set(world, e6, Position, {1, 2});

    ecs_add_pair(world, e3, Rel, e1); /* depth 1 */
    ecs_add_pair(world, e4, Rel, e2); /* depth 1 */
    ecs_add_pair(world, e5, Rel, e3); /* depth 2 */
    ecs_add_pair(world, e6, Rel, e4); /* depth 2 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 5);
    test_int(ctx.system, Iter);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_assert((ctx.e[0] == e1 && ctx.e[1] == e2) || (ctx.e[0] == e2 && ctx.e[1] == e1));
    test_assert((ctx.e[2] == e3 && ctx.e[3] == e4) || (ctx.e[2] == e4 && ctx.e[3] == e3));
    test_assert((ctx.e[4] == e5 && ctx.e[5] == e6) || (ctx.e[4] == e6 && ctx.e[5] == e5));

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e5, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    p = ecs_get(world, e6, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    ecs_fini(world);
}

void SystemCascade_custom_relation_add_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Acyclic);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, AddParent, EcsOnUpdate, Position, ?Position(cascade(Rel)));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = AddParent,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});

    ecs_add_pair(world, e3, Rel, parent); /* depth 1 */
    ecs_add_pair(world, e4, Rel, parent); /* depth 1 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    /* Before adding Position to parent, it wasn't being considered for the
     * column(parent|cascade), so tables could have been ordered randomly. Make sure
     * that queries can handle changes to depth after all tables are matched */
    ecs_set(world, parent, Position, {1, 2});

    ecs_os_zeromem(&ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, AddParent);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_assert(ctx.e[0] == e1 || ctx.e[1] == e1 || ctx.e[2] == e1);
    test_assert(ctx.e[0] == e2 || ctx.e[1] == e2 || ctx.e[2] == e2);
    test_assert(ctx.e[0] == parent || ctx.e[1] == parent || ctx.e[2] == parent);
    test_assert(ctx.e[3] == e3 || ctx.e[4] == e3);
    test_assert(ctx.e[3] == e4 || ctx.e[4] == e4);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}

void SystemCascade_custom_relation_adopt_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Acyclic);

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);
    ECS_ENTITY(world, e4, Position);

    ECS_SYSTEM(world, AddParent, EcsOnUpdate, Position, ?Position(cascade(Rel)));
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = AddParent,
        .query.filter.instanced = true
    });

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, e1, Position, {1, 2});
    ecs_set(world, e2, Position, {1, 2});
    ecs_set(world, e3, Position, {1, 2});
    ecs_set(world, e4, Position, {1, 2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    ecs_add_pair(world, e3, Rel, parent);
    ecs_add_pair(world, e4, Rel, parent);

    ecs_os_zeromem(&ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, AddParent);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], parent);
    test_int(ctx.e[3], e3);
    test_int(ctx.e[4], e4);
    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    p = ecs_get(world, e4, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}
