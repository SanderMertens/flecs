#include <api.h>

static
void OnSet(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    Velocity *v = NULL;
    if (rows->column_count >= 2) {
        v = ecs_column(rows, Velocity, 2);
    }

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x ++;

        if (v) {
            v[i].x = p[i].x;
            v[i].y = p[i].y;
        }
    }
}

void SystemOnSet_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);
}

void SystemOnSet_set_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);
}

void SystemOnSet_set_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ecs_set_ptr(world, e, Position, p);

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    test_int(p->y, 20);
}

void SystemOnSet_clone() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ctx = (SysTestData){0};

   ecs_clone(world, e_1, false);

    /* OnSet function should not have been called, because value has not been 
     * copied */
    test_int(ctx.count, 0);
}

void SystemOnSet_clone_w_value() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ctx = (SysTestData){0};

    ecs_entity_t e_2 = ecs_clone(world, e_1, true);

    /* OnSet function should not have been called, because value has not been 
     * copied */
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    test_int(p->y, 20);
}

void SystemOnSet_set_w_optional() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* This is cool- it lets reactive systems access data from the component
     * array directly, without having to do an ecs_get */
    ECS_SYSTEM(world, OnSet, EcsOnSet, Position, ?Velocity);


    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e_1 = ecs_new(world, Position);
    ecs_entity_t e_2 = ecs_new(world, Position);
    ecs_add(world, e_2, Velocity);
    test_int(ctx.count, 0);

    ecs_set(world, e_1, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], 0);


    ctx = (SysTestData){0};
    ecs_set(world, e_2, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_entity(Velocity));
    test_int(ctx.s[0][1], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    Velocity *v = ecs_get_ptr(world, e_2, Velocity);
    test_assert(v != NULL);
    test_assert((void*)p != (void*)v); /* a little bit paranoid.. */
    test_int(v->x, 11);
    test_int(v->y, 20);
}

static bool add_called;
static bool set_called;

static
void OnAdd_check_order(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    test_assert(!add_called);
    test_assert(!set_called);

    ProbeSystem(rows);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 1;
        p[i].y = 2;    
    }

    add_called = true;
}

static
void OnSet_check_order(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    ProbeSystem(rows);

    test_assert(add_called);
    test_assert(!set_called);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x ++;
    }

    set_called = true;
}

void SystemOnSet_set_and_add_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet_check_order, EcsOnSet, Position);
    ECS_SYSTEM(world, OnAdd_check_order, EcsOnAdd, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    test_assert(set_called);
}

static
void OnAdd(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    int i;
    for (i = 0; i < rows->count; i ++) {
        p[i].x = 1;
        p[i].y = 3;
    }
}

void SystemOnSet_on_set_after_on_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnAdd, EcsOnAdd, Position);
    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_add(world, e, Position);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0); 

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);  
}

void SystemOnSet_on_set_after_on_add_w_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnAdd, EcsOnAdd, Position);
    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Position);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0); 

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3); 
}

void SystemOnSet_on_set_after_on_add_w_new_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnAdd, EcsOnAdd, Position);
    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_count(world, Position, 3);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0); 

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e + 1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e + 2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3); 
}

static bool on_set_velocity = false;

static
void OnSetVelocity(ecs_rows_t *rows) {
    on_set_velocity = true;
}

void SystemOnSet_on_set_after_on_add_1_of_2_matched() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, OnAdd, EcsOnAdd, Position);
    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);
    ECS_SYSTEM(world, OnSetVelocity, EcsOnSet, Velocity);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, Type);

    test_int(on_set_velocity, false);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0); 

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3); 
}

static
void OnSetShared(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);

    Velocity *v = NULL;
    if (rows->column_count >= 2) {
        v = ecs_column(rows, Velocity, 2);
    }

    ProbeSystem(rows);

    int i;

    if (!ecs_is_shared(rows, 1)) {
        for (i = 0; i < rows->count; i ++) {
            p[i].x ++;

            if (v) {
                v[i].x = p[i].x;
                v[i].y = p[i].y;
            }
        }
    } else {
        for (i = 0; i < rows->count; i ++) {
            if (v) {
                v[i].x = p->x;
                v[i].y = p->y;
            }
        }        
    }
}

void SystemOnSet_on_set_after_override() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_SYSTEM(world, OnSetShared, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    /* instantiate prefab */

    ecs_entity_t e = ecs_new_instance(world, Prefab, 0);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);

    test_int(ctx.system, OnSetShared);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], Prefab);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_assert(p == ecs_get_ptr(world, Prefab, Position));
    test_int(p->x, 1);
    test_int(p->y, 3);     

    /* override component (doesn't call system) */

    ctx = (SysTestData){0};

    ecs_add(world, e, Position);

    test_int(ctx.count, 0);
    test_int(ctx.invoked, 0);

    Position *p_after = ecs_get_ptr(world, e, Position);
    test_assert(p_after != NULL);
    test_assert(p != p_after);
    test_int(p_after->x, 1);
    test_int(p_after->y, 3);

    /* set component */

    ecs_set(world, e, Position, {2, 4});

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);

    test_int(ctx.system, OnSetShared);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0);    

    p_after = ecs_get_ptr(world, e, Position);
    test_assert(p_after != NULL);
    test_assert(p != p_after);
    test_int(p_after->x, 3);
    test_int(p_after->y, 4);

    ecs_fini(world);
}

void SystemOnSet_on_set_after_override_w_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);
}

void SystemOnSet_on_set_after_override_w_new_w_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new_w_count(world, Type, 3);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, OnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], ecs_entity(Position));
    test_int(ctx.s[0][0], 0); 

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e + 1, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = ecs_get_ptr(world, e + 2, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);
}

void SystemOnSet_on_set_after_override_1_of_2_overridden() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_PREFAB(world, Prefab, Position);
    ecs_set(world, Prefab, Position, {1, 3});

    ECS_TYPE(world, Type, INSTANCEOF | Prefab, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
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

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);
}

static int is_invoked;

static
void IsInvoked(ecs_rows_t *rows) {
    is_invoked ++;
}

void SystemOnSet_disabled_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, IsInvoked, EcsOnSet, Position);

    ecs_enable(world, IsInvoked, false);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_set(world, 0, Position, {10, 20});
    test_assert(e != 0);
    test_assert( ecs_has(world, e, Position));

    test_int(is_invoked, 0);

    ecs_fini(world);
}
