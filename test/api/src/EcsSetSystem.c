/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef int32_t Foo;
typedef int32_t Bar;

static int32_t set_system_called;

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
    ecs_set(world, h, Foo, 10);

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
    ecs_set(world, h, Foo, 10);

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
    ecs_set(world, h, Foo, 10);

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
    ecs_set(world, 0, Foo, 10);

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
    ecs_set(world, h, Foo, 10);

    test_assertint(set_system_called, 1);
    test_assertint(ecs_get(world, h, Foo), 11);

    test_assertint(set_system_called, 1);
    ecs_set(world, h, Foo, 11);

    test_assertint(set_system_called, 2);
    test_assertint(ecs_get(world, h, Foo), 12);

    ecs_fini(world);
}
