#include <api.h>

static
void Iter(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    Position *p_parent = ecs_column(it, Position, 2);

    test_assert(!p_parent || !ecs_is_owned(it, 2));

    probe_system(it);

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

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, CASCADE:Position);

    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_2, Position, {1, 2});
    ecs_set(world, e_3, Position, {1, 2});
    ecs_set(world, e_4, Position, {1, 2});

    ecs_add_entity(world, e_3, ECS_CHILDOF | e_1);
    ecs_add_entity(world, e_4, ECS_CHILDOF | e_2);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    probe_has_entity(&ctx, e_1);
    probe_has_entity(&ctx, e_2);
    probe_has_entity(&ctx, e_3);
    probe_has_entity(&ctx, e_4);

    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    ecs_fini(world);
}

void SystemCascade_cascade_depth_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);
    ECS_ENTITY(world, e_5, Position);
    ECS_ENTITY(world, e_6, Position);

    ECS_SYSTEM(world, Iter, EcsOnUpdate, Position, CASCADE:Position);

    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_2, Position, {1, 2});
    ecs_set(world, e_3, Position, {1, 2});
    ecs_set(world, e_4, Position, {1, 2});
    ecs_set(world, e_5, Position, {1, 2});
    ecs_set(world, e_6, Position, {1, 2});

    ecs_add_entity(world, e_3, ECS_CHILDOF | e_1); /* depth 1 */
    ecs_add_entity(world, e_4, ECS_CHILDOF | e_2); /* depth 1 */
    ecs_add_entity(world, e_5, ECS_CHILDOF | e_3); /* depth 2 */
    ecs_add_entity(world, e_6, ECS_CHILDOF | e_4); /* depth 2 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 6);
    test_int(ctx.invoked, 5);
    test_int(ctx.system, Iter);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_assert((ctx.e[0] == e_1 && ctx.e[1] == e_2) || (ctx.e[0] == e_2 && ctx.e[1] == e_1));
    test_assert((ctx.e[2] == e_3 && ctx.e[3] == e_4) || (ctx.e[2] == e_4 && ctx.e[3] == e_3));
    test_assert((ctx.e[4] == e_5 && ctx.e[5] == e_6) || (ctx.e[4] == e_6 && ctx.e[5] == e_5));

    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);
    test_int(p->y, 6);

    p = ecs_get(world, e_5, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    p = ecs_get(world, e_6, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 9);

    ecs_fini(world);
}

static
void AddParent(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);
    Position *p_parent = ecs_column(it, Position, 2);

    test_assert(!p_parent || !ecs_is_owned(it, 2));

    probe_system(it);

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

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, AddParent, EcsOnUpdate, Position, CASCADE:Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_2, Position, {1, 2});
    ecs_set(world, e_3, Position, {1, 2});
    ecs_set(world, e_4, Position, {1, 2});

    ecs_add_entity(world, e_3, ECS_CHILDOF | parent); /* depth 1 */
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent); /* depth 1 */

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    /* Before adding Position to parent, it wasn't being considered for the
     * CASCADE column, so tables could have been ordered randomly. Make sure
     * that queries can handle changes to depth after all tables are matched */
    ecs_set(world, parent, Position, {1, 2});

    ctx = (Probe){0};

    ecs_progress(world, 1);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, AddParent);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_assert(ctx.e[0] == e_1 || ctx.e[1] == e_1 || ctx.e[2] == e_1);
    test_assert(ctx.e[0] == e_2 || ctx.e[1] == e_2 || ctx.e[2] == e_2);
    test_assert(ctx.e[0] == parent || ctx.e[1] == parent || ctx.e[2] == parent);
    test_assert(ctx.e[3] == e_3 || ctx.e[4] == e_3);
    test_assert(ctx.e[3] == e_4 || ctx.e[4] == e_4);

    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    p = ecs_get(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}

void SystemCascade_adopt_after_match() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    ECS_SYSTEM(world, AddParent, EcsOnUpdate, Position, CASCADE:Position);

    ecs_entity_t parent = ecs_set(world, 0, Position, {1, 2});
    ecs_set(world, e_1, Position, {1, 2});
    ecs_set(world, e_2, Position, {1, 2});
    ecs_set(world, e_3, Position, {1, 2});
    ecs_set(world, e_4, Position, {1, 2});

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    ecs_add_entity(world, e_3, ECS_CHILDOF | parent);
    ecs_add_entity(world, e_4, ECS_CHILDOF | parent);

    ctx = (Probe){0};

    ecs_progress(world, 1);

    test_int(ctx.count, 5);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, AddParent);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.e[1], e_2);
    test_int(ctx.e[2], parent);
    test_int(ctx.e[3], e_3);
    test_int(ctx.e[4], e_4);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, e_3, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    p = ecs_get(world, e_4, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 4);

    ecs_fini(world);
}
