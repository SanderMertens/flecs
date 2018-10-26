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

    EcsHandle e = ecs_new(world, Bar_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Bar_h));

    Bar *bar_ptr = ecs_get(world, e, Bar_h);
    test_assert(bar_ptr != NULL);
    bar_ptr->y = 10;

    ecs_add(world, e, Foo_h);
    ecs_add(world, e, Hello_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Foo_h));

    bar_ptr = ecs_get(world, e, Bar_h);
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

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Foo_h);
    ecs_add(world, e, Hello_h);
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

    EcsHandle e = ecs_new(world, Bar_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Bar_h));

    Bar *bar_ptr = ecs_get(world, e, Bar_h);
    test_assert(bar_ptr != NULL);
    bar_ptr->y = 10;

    ecs_add(world, e, Foo_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    bar_ptr = ecs_get(world, e, Bar_h);
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

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Foo_h);
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

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));

    Foo *foo_ptr = ecs_get(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    foo_ptr->x = 10;

    ecs_add(world, e, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));

    foo_ptr = ecs_get(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    test_assertint(foo_ptr->x, 10);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_family(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_add(world, e, MyFamily_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void test_EcsAdd_tc_add_family_with_family(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, Family1, Foo);
    ECS_FAMILY(world, Family2, Family1, Bar);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_add(world, e, Family2_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void test_EcsAdd_tc_add_family_overlapping_w_entity(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    Foo *foo_ptr = ecs_get(world, e, Foo_h);
    test_assert(foo_ptr != NULL);
    foo_ptr->x = 10;

    ecs_add(world, e, MyFamily_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    foo_ptr = ecs_get(world, e, Foo_h);
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
    ECS_FAMILY(world, Family1, Foo, Bar);
    ECS_FAMILY(world, Family2, Bar, Hello);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_add(world, e, Family1_h);
    ecs_add(world, e, Family2_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

typedef struct Context {
    EcsHandle entity;
    EcsHandle component;
    uint32_t count;
} Context;

static
void AddNext(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_column(rows, row, 0)) ++;
        if (ecs_entity(row) - 1 == ctx->entity) {
            ecs_add(rows->world, ctx->entity, ctx->component);
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
        (*(int*)ecs_column(rows, row, 0)) ++;
        if (ecs_entity(row) + 1 == ctx->entity) {
            ecs_add(rows->world, ctx->entity, ctx->component);
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
        (*(int*)ecs_column(rows, row, 0)) ++;
        if (ecs_entity(row) == ctx->entity) {
            ecs_add(rows->world, ctx->entity, ctx->component);
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
        ecs_add(rows->world, ecs_entity(row), ctx->component);
        ecs_commit(rows->world, ecs_entity(row));
        (*(int*)ecs_column(rows, row, 0)) ++;
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
    ECS_SYSTEM(world, AddPrev, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 32);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_cur_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddCurrent, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 32);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_next_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddNext, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 32);

    ecs_fini(world);
}

void test_EcsAdd_tc_add_all_in_progress(
    test_EcsAdd this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddAll, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 21);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    test_assertint(ctx.count, 3);

    ecs_progress(world);
    test_assertint(ctx.count, 6);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 22);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 32);

    ecs_fini(world);
}
