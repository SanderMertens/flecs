#include <api.h>

static
void OnSet(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    Velocity *v = NULL;
    if (it->column_count >= 2) {
        v = ecs_column(it, Velocity, 2);
    }

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x ++;

        if (v) {
            v[i].x = p[i].x;
            v[i].y = p[i].y;
        }
    }
}

void TriggerOnSet_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnSet_set_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnSet_set_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);
    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ecs_set_ptr(world, e, Position, p);

    p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    test_int(p->y, 20);

    ecs_fini(world);
}

void TriggerOnSet_clone() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ctx = (Probe){0};

   ecs_clone(world, 0, e_1, false);

    /* OnSet function should not have been called, because value has not been 
     * copied */
    test_int(ctx.count, 0);

    ecs_fini(world);
}

void TriggerOnSet_clone_w_value() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ctx = (Probe){0};

    ecs_entity_t e_2 = ecs_clone(world, 0, e_1, true);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    p = ecs_get(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    test_int(p->y, 20);

    ecs_fini(world);
}

static bool add_called;
static bool set_called;

static
void OnAdd_check_order(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    test_assert(!add_called);
    test_assert(!set_called);

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x = 1;
        p[i].y = 2;    
    }

    add_called = true;
}

static
void OnSet_check_order(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    probe_system(it);

    test_assert(add_called);
    test_assert(!set_called);

    int i;
    for (i = 0; i < it->count; i ++) {
        p[i].x ++;
    }

    set_called = true;
}

void TriggerOnSet_set_and_add_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet_check_order, EcsOnSet, Position);
    ECS_TRIGGER(world, OnAdd_check_order, EcsOnAdd, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {10, 20});
    
    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, OnSet_check_order);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.e[1], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[1][0], ecs_entity(Position));
    test_int(ctx.s[1][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    test_assert(set_called);

    ecs_fini(world);
}

static
void OnSetShared(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    Velocity *v = NULL;
    if (it->column_count >= 2) {
        v = ecs_column(it, Velocity, 2);
    }

    probe_system(it);

    int i;
    for (i = 0; i < it->count; i ++) {
        if (v) {
            v[i].x = p->x;
            v[i].y = p->y;
        }
    }        
}

void TriggerOnSet_on_set_after_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_SYSTEM(world, OnSetShared, EcsOnSet, ANY:Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    /* instantiate prefab */
    ecs_entity_t e = ecs_new_w_entity(world, ECS_INSTANCEOF | Prefab);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);

    test_int(ctx.system, OnSetShared);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], Prefab);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get(world, Prefab, Position));
    test_int(p->x, 1);
    test_int(p->y, 3);

    /* override component (doesn't call system) */

    ctx = (Probe){0};

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    const Position *p_after = ecs_get(world, e, Position);
    test_assert(p_after != NULL);
    test_assert(p != p_after);
    test_int(p_after->x, 1);
    test_int(p_after->y, 3);

    /* Set component */

    ecs_set(world, e, Position, {2, 4});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);

    test_int(ctx.system, OnSetShared);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);    

    p_after = ecs_get(world, e, Position);
    test_assert(p_after != NULL);
    test_assert(p != p_after);
    test_int(p_after->x, 2);
    test_int(p_after->y, 4);

    ecs_fini(world);
}

void TriggerOnSet_on_set_after_override_w_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_fini(world);
}

void TriggerOnSet_on_set_after_override_w_new_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_bulk_new(world, Type, 3);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0); 

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e + 1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get(world, e + 2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_fini(world);
}

void TriggerOnSet_on_set_after_override_1_of_2_overridden() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);
    
    ecs_fini(world);
}

static
void SetPosition(ecs_iter_t *it) {
    probe_system(it);
}

void TriggerOnSet_on_set_after_snapshot_restore() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, SetPosition, EcsOnSet, Position);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int32_t i;
    for (i = 0; i < 10; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_snapshot_t *s = ecs_snapshot_take(world);

    test_int(ctx.invoked, 0);

    /* Delete one entity, so we have more confidence we're triggering on the 
     * right entities */
    ecs_delete(world, e);
    
    test_int(ctx.invoked, 0);

    ecs_snapshot_restore(world, s);

    test_int(ctx.count, 10);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, SetPosition);
    test_int(ctx.column_count, 1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_null(ctx.param);
    
    for (i = 0; i < 10; i ++) {
        test_int(ctx.e[i], e + i);
    }   

    ecs_fini(world);
}
