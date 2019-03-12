#include <include/api.h>

static
void OnSet(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column(rows, Velocity, 2);

    ProbeSystem(rows);

    int i;
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        p[i].x ++;

        if (v) {
            v[i].x = p[i].x;
            v[i].y = p[i].y;
        }
    }
}

void SystemOnSet_set() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);
}

void SystemOnSet_set_new() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);
}

void SystemOnSet_set_again() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, Position);
    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ecs_set_ptr(world, e, Position, sizeof(Position), p);

    p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    test_int(p->y, 20);
}

void SystemOnSet_clone() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_1 = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], EPosition);
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
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet, EcsOnSet, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_1 = ecs_set(world, 0, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    Position *p = ecs_get_ptr(world, e_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    ctx = (SysTestData){0};

    EcsEntity e_2 = ecs_clone(world, e_1, true);

    /* OnSet function should not have been called, because value has not been 
     * copied */
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);

    p = ecs_get_ptr(world, e_2, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    test_int(p->y, 20);
}

void SystemOnSet_set_w_optional() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* This is cool- it lets reactive systems access data from the component
     * array directly, without having to do an ecs_get */
    ECS_SYSTEM(world, OnSet, EcsOnSet, Position, ?Velocity);


    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e_1 = ecs_new(world, Position);
    EcsEntity e_2 = ecs_new(world, Position);
    ecs_add(world, e_2, Velocity);
    test_int(ctx.count, 0);

    ecs_set(world, e_1, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_1);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
    test_int(ctx.s[0][1], 0);


    ctx = (SysTestData){0};
    ecs_set(world, e_2, Position, {10, 20});
    
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, EOnSet);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e_2);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], EVelocity);
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
void OnAdd_check_order(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);

    test_assert(!add_called);
    test_assert(!set_called);

    ProbeSystem(rows);

    int i;
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        p[i].x = 1;
        p[i].y = 2;    
    }

    add_called = true;
}

static
void OnSet_check_order(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);

    ProbeSystem(rows);

    test_assert(add_called);
    test_assert(!set_called);

    int i;
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        p[i].x ++;
    }

    set_called = true;
}

void SystemOnSet_set_and_add_system() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, OnSet_check_order, EcsOnSet, Position);
    ECS_SYSTEM(world, OnAdd_check_order, EcsOnAdd, Position);

    SysTestData ctx = {0};
    ecs_set_context(world, &ctx);

    EcsEntity e = ecs_new(world, 0);
    test_int(ctx.count, 0);

    ecs_set(world, e, Position, {10, 20});
    
    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, EOnSet_check_order);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e);
    test_int(ctx.e[1], e);
    test_int(ctx.c[0][0], EPosition);
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[1][0], EPosition);
    test_int(ctx.s[1][0], 0);

    Position *p = ecs_get_ptr(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 20);

    test_assert(set_called);
}
