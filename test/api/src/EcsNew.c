/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

typedef struct Bar {
    int y;
} Bar;

void test_EcsNew_tc_new(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);
    ecs_fini(world);
}


void test_EcsNew_tc_new_w_component(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    ecs_fini(world);
}


void test_EcsNew_tc_new_w_family(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, MyFamily, Foo);
    EcsEntity e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    ecs_fini(world);
}

void test_EcsNew_tc_new_family_with_family(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Family1, Foo);
    ECS_TYPE(world, Family2, Family1, Bar);
    EcsEntity e = ecs_new(world, Family2_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}

void test_EcsNew_tc_new_w_family_of_2(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    EcsEntity e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}


void test_EcsNew_tc_new_w_prefab(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_PREFAB(world, MyPrefab, Foo);
    EcsEntity e = ecs_new(world, MyPrefab_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    ecs_fini(world);
}


void test_EcsNew_tc_new_w_prefab_of_2(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);
    EcsEntity e = ecs_new(world, MyPrefab_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}
