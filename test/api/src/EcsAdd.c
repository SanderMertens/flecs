/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

typedef struct Bar {
    int y;
} Bar;

typedef struct Hello {
    int a;
} Hello;

typedef struct World {
    int b;
} World;

void test_EcsAdd_tc_add_2_component(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);

    EcsEntity e = ecs_new(world, Bar_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Bar_h));

    Bar *bar_ptr = ecs_get_ptr(world, e, Bar_h);
    test_assert(bar_ptr != NULL);
    bar_ptr->y = 10;

    ecs_stage_add(world, e, Foo_h);
    ecs_stage_add(world, e, Hello_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Foo_h));

    bar_ptr = ecs_get_ptr(world, e, Bar_h);
    test_assert(bar_ptr != NULL);
    test_assertint(bar_ptr->y, 10);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_2_component_to_empty(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Hello);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_stage_add(world, e, Foo_h);
    ecs_stage_add(world, e, Hello_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Foo_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Foo_h));

    ecs_fini(world);
}

void test_EcsAdd_tc_add_component(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);

    EcsEntity e = ecs_new(world, Bar_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Bar_h));

    Bar *bar_ptr = ecs_get_ptr(world, e, Bar_h);
    test_assert(bar_ptr != NULL);
    bar_ptr->y = 10;

    ecs_stage_add(world, e, Foo_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    bar_ptr = ecs_get_ptr(world, e, Bar_h);
    test_assert(bar_ptr != NULL);
    test_assertint(bar_ptr->y, 10);

    ecs_fini(world);
}


void test_EcsAdd_tc_add_component_to_empty(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_stage_add(world, e, Foo_h);
    test_assert(!ecs_has(world, e, Foo_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_fini(world);
}

void test_EcsAdd_tc_add_existing_component(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));

    Foo *foo_ptr = ecs_get_ptr(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    foo_ptr->x = 10;

    ecs_stage_add(world, e, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));

    foo_ptr = ecs_get_ptr(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    test_assertint(foo_ptr->x, 10);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_type(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_stage_add(world, e, MyType_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void test_EcsAdd_tc_add_type_with_type(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type1, Foo);
    ECS_TYPE(world, Type2, Type1, Bar);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_stage_add(world, e, Type2_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void test_EcsAdd_tc_add_type_overlapping_w_entity(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    Foo *foo_ptr = ecs_get_ptr(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    foo_ptr->x = 10;

    ecs_stage_add(world, e, MyType_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    foo_ptr = ecs_get_ptr(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    test_assertint(foo_ptr->x, 10);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_overlapping_families(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, Type1, Foo, Bar);
    ECS_TYPE(world, Type2, Bar, Hello);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_stage_add(world, e, Type1_h);
    ecs_stage_add(world, e, Type2_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

typedef struct Context {
    EcsEntity entity;
    EcsEntity component;
    uint32_t count;
} Context;

static
void AddNext(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_data(rows, row, 0)) ++;
        if (ecs_entity(rows, row, ECS_ROW_ENTITY) - 1 == ctx->entity) {
            ecs_stage_add(rows->world, ctx->entity, ctx->component);
            ecs_commit(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

static
void AddPrev(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_data(rows, row, 0)) ++;
        if (ecs_entity(rows, row, ECS_ROW_ENTITY) + 1 == ctx->entity) {
            ecs_stage_add(rows->world, ctx->entity, ctx->component);
            ecs_commit(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

static
void AddCurrent(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_data(rows, row, 0)) ++;
        if (ecs_entity(rows, row, ECS_ROW_ENTITY) == ctx->entity) {
            ecs_stage_add(rows->world, ctx->entity, ctx->component);
            ecs_commit(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

static
void AddAll(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ecs_stage_add(rows->world, ecs_entity(rows, row, ECS_ROW_ENTITY), ctx->component);
        ecs_commit(rows->world, ecs_entity(rows, row, ECS_ROW_ENTITY));
        (*(int*)ecs_data(rows, row, 0)) ++;
        ctx->count ++;
    }
}

void test_EcsAdd_tc_add_prev_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddPrev, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 32);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_cur_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddCurrent, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 32);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_next_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddNext, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 32);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_all_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddAll, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 32);

    ecs_fini(world);
}

static
void AddNewContainerInSystem(EcsRows *rows) {
    for (void *row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity parent = ecs_new(rows->world, EcsContainer_h);
        ecs_add(rows->world, ecs_entity(rows, row, 0), parent);
    }
}

void test_EcsAdd_tc_add_new_container_in_system(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddNewContainerInSystem, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    test_assert(e1 != 0);

    /* If this doesn't crash, test passes */
    ecs_progress(world, 0);

    ecs_fini(world);
}

void DoFoo(EcsRows *rows) {
    for (void *row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
    }
}

void AddBar(EcsRows *rows) {
    for (void *row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity e = ecs_entity(rows, row, 0);
        EcsEntity Bar_h = ecs_component(rows, 1);
        ecs_add(rows->world, e, Bar_h);
    }
}


void test_EcsAdd_tc_add_in_progress_before_2nd_system(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, Prefab, EcsContainer);
    
    ECS_ENTITY(world, E1, Prefab, Foo);
    ECS_ENTITY(world, E2, E1, Foo);

    ECS_SYSTEM(world, AddBar, EcsOnFrame, Foo, ID.Bar);
    ECS_SYSTEM(world, DoFoo, EcsOnFrame, Foo, ?EcsContainer, !CONTAINER.Bar);

    ecs_set_target_fps(world, 1);

    ecs_progress(world, 0);

    /* If test doesn't crash, it succeeds */
    test_assert(true);

    ecs_fini(world);
}
