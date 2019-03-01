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


void test_EcsNew_tc_new_w_type(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, MyType, Foo);
    EcsEntity e = ecs_new(world, MyType_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    ecs_fini(world);
}

void test_EcsNew_tc_new_type_with_type(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type1, Foo);
    ECS_TYPE(world, Type2, Type1, Bar);
    EcsEntity e = ecs_new(world, Type2_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}

void test_EcsNew_tc_new_w_type_of_2(
    test_EcsNew this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);
    EcsEntity e = ecs_new(world, MyType_h);
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
    ECS_TYPE(world, MyType, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyType);
    EcsEntity e = ecs_new(world, MyPrefab_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}
