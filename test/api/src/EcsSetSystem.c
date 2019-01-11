/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef int32_t Foo;
typedef int32_t Bar;

static int32_t set_system_called;

static
void Set(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Foo *v = ecs_column(rows, row, 0);
        (*v) ++;
        set_system_called ++;
    }
}

void test_EcsSetSystem_tc_set_after_new_empty(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Set, EcsOnSet, Foo);

    test_assertint(set_system_called, 0);
    EcsHandle h = ecs_new(world, 0);

    test_assertint(set_system_called, 0);
    ecs_set(world, h, Foo, {10});

    test_assertint(set_system_called, 1);
    test_assertint(ecs_get(world, h, Foo), 11);

    ecs_fini(world);
}

void test_EcsSetSystem_tc_set_after_new_nonempty(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, Set, EcsOnSet, Foo);

    test_assertint(set_system_called, 0);
    EcsHandle h = ecs_new(world, Bar_h);

    test_assertint(set_system_called, 0);
    ecs_set(world, h, Foo, {10});

    test_assertint(set_system_called, 1);
    test_assertint(ecs_get(world, h, Foo), 11);

    ecs_fini(world);
}

void test_EcsSetSystem_tc_set_existing_components(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Set, EcsOnSet, Foo);

    test_assertint(set_system_called, 0);
    EcsHandle h = ecs_new(world, Foo_h);

    test_assertint(set_system_called, 0);
    ecs_set(world, h, Foo, {10});

    test_assertint(set_system_called, 1);
    test_assertint(ecs_get(world, h, Foo), 11);

    ecs_fini(world);
}

void test_EcsSetSystem_tc_set_implicit_new(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Set, EcsOnSet, Foo);

    test_assertint(set_system_called, 0);
    ecs_set(world, 0, Foo, {10});

    test_assertint(set_system_called, 1);

    ecs_fini(world);
}

void test_EcsSetSystem_tc_set_twice(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Set, EcsOnSet, Foo);

    test_assertint(set_system_called, 0);
    EcsHandle h = ecs_new(world, 0);

    test_assertint(set_system_called, 0);
    ecs_set(world, h, Foo, {10});

    test_assertint(set_system_called, 1);
    test_assertint(ecs_get(world, h, Foo), 11);

    test_assertint(set_system_called, 1);
    ecs_set(world, h, Foo, {11});

    test_assertint(set_system_called, 2);
    test_assertint(ecs_get(world, h, Foo), 12);

    ecs_fini(world);
}

typedef struct SetFooCtx {
    EcsHandle Bar;
} SetFooCtx;

static
void SetBar(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Bar *v = ecs_column(rows, row, 0);
        (*v) ++;
    }
}

static
void SetFoo(EcsRows *rows) {
    EcsWorld *world = rows->world;
    SetFooCtx *ctx = ecs_get_system_context(world, rows->system);
    EcsHandle Bar_h = ctx->Bar;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        Foo *v = ecs_column(rows, row, 0);
        (*v) ++;

        ecs_set(world, entity, Bar, {20});
    }
}

void test_EcsSetSystem_tc_set_in_on_set(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, SetFooCtx);
    ECS_SYSTEM(world, SetFoo, EcsOnSet, Foo);
    ECS_SYSTEM(world, SetBar, EcsOnSet, Bar);

    ecs_set_system_context(world, SetFoo_h, SetFooCtx, {
        .Bar = Bar_h
    });

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo), 11);
    test_assertint(ecs_get(world, e, Bar), 21);

    ecs_fini(world);
}

static
void InitBar(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Bar *v = ecs_column(rows, row, 0);
        *v = 20;
    }
}

static
void SetFooAdd(EcsRows *rows) {
    EcsWorld *world = rows->world;
    SetFooCtx *ctx = ecs_get_system_context(world, rows->system);
    EcsHandle Bar_h = ctx->Bar;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        Foo *v = ecs_column(rows, row, 0);
        (*v) ++;

        ecs_add(world, entity, Bar_h);
        ecs_commit(world, entity);
    }
}

void test_EcsSetSystem_tc_on_add_in_on_set(
    test_EcsSetSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, SetFooCtx);
    ECS_SYSTEM(world, SetFooAdd, EcsOnSet, Foo);
    ECS_SYSTEM(world, InitBar, EcsOnAdd, Bar);

    ecs_set_system_context(world, SetFooAdd_h, SetFooCtx, {
        .Bar = Bar_h
    });

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo), 11);
    test_assertint(ecs_get(world, e, Bar), 20);

    ecs_fini(world);
}
