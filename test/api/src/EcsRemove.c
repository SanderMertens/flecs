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

void test_EcsRemove_tc_remove_2_component(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_remove(world, e, Foo_h);
    ecs_remove(world, e, Bar_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    ecs_fini(world);
}

void test_EcsRemove_tc_remove_component(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_FAMILY(world, MyFamily, Foo);
    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    ecs_remove(world, e, Foo_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    ecs_fini(world);
}

void test_EcsRemove_tc_remove_2_component_from_3(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_FAMILY(world, MyFamily, Foo, Bar, Hello);
    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));
    ecs_remove(world, e, Foo_h);
    ecs_remove(world, e, Bar_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));
    ecs_fini(world);
}

void test_EcsRemove_tc_remove_component_from_2(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_remove(world, e, Foo_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}
