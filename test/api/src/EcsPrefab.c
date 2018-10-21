/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

typedef struct Bar {
    int x;
} Bar;

void test_EcsPrefab_tc_prefab_component(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_PREFAB(world, MyPrefab, Foo);

    EcsHandle e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsHandle e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);
    Foo *ptr1 = ecs_get(world, e1, Foo_h);
    test_assert(ptr1 != NULL);
    Foo *ptr2 = ecs_get(world, e2, Foo_h);
    test_assert(ptr2 != NULL);
    test_assert(ptr1 == ptr2);
    ecs_fini(world);
}


void test_EcsPrefab_tc_prefab_empty(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_PREFAB(world, MyPrefab, 0);

    EcsHandle e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsHandle e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    ecs_fini(world);
}


void test_EcsPrefab_tc_prefab_family(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);

    EcsHandle e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsHandle e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);
    Foo *foo1 = ecs_get(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 == foo2);
    test_assert(bar1 == bar2);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_prefab(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, Prefab1, Foo);
    ECS_PREFAB(world, Prefab2, Prefab1, Bar);

    EcsHandle e1 = ecs_new(world, Prefab1_h);
    test_assert(e1 != 0);

    EcsHandle e2 = ecs_new(world, Prefab2_h);
    test_assert(e2 != 0);

    test_assert(ecs_has(world, e1, Foo_h));
    test_assert(!ecs_has(world, e1, Bar_h));

    test_assert(ecs_has(world, e2, Foo_h));
    test_assert(ecs_has(world, e2, Bar_h));

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_override_1_of_2(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);

    EcsHandle e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);

    ecs_add(world, e1, Foo_h);
    ecs_commit(world, e1);

    EcsHandle e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    ecs_add(world, e2, Foo_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 == bar2);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_override_2_of_2(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);

    EcsHandle e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);

    ecs_add(world, e1, Foo_h);
    ecs_add(world, e1, Bar_h);
    ecs_commit(world, e1);

    EcsHandle e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    ecs_add(world, e2, Foo_h);
    ecs_add(world, e2, Bar_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 != bar2);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_add_component(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Foo);

    EcsHandle e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsHandle e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    Foo *foo1 = ecs_get(world, e1, Foo_h);
    test_assert(foo1 != NULL);

    Foo *foo2 = ecs_get(world, e2, Foo_h);
    test_assert(foo2 != NULL);

    test_assert(foo1 == foo2);

    ecs_add(world, MyPrefab_h, Bar_h);
    ecs_commit(world, MyPrefab_h);

    Bar *bar1 = ecs_get(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Bar *bar2 = ecs_get(world, e2, Bar_h);
    test_assert(bar2 != NULL);
    test_assert(bar1 == bar2);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_add_to_empty_entity(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);

    EcsHandle e1 = ecs_new(world, 0);
    test_assert(e1 != 0);
    EcsHandle e2 = ecs_new(world, 0);
    test_assert(e2 != 0);

    ecs_add(world, e1, MyPrefab_h);
    ecs_commit(world, e1);

    ecs_add(world, e2, MyPrefab_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 == foo2);
    test_assert(bar1 == bar2);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_add_to_entity(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Bar);

    EcsHandle e1 = ecs_new(world, Foo_h);
    test_assert(e1 != 0);
    EcsHandle e2 = ecs_new(world, Foo_h);
    test_assert(e2 != 0);

    ecs_add(world, e1, MyPrefab_h);
    ecs_commit(world, e1);

    ecs_add(world, e2, MyPrefab_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 == bar2);

    ecs_fini(world);
}
