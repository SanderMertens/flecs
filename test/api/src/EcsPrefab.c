/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

typedef struct Bar {
    int y;
} Bar;

void test_EcsPrefab_tc_prefab_component(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_PREFAB(world, MyPrefab, Foo);

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);
    Foo *ptr1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(ptr1 != NULL);
    Foo *ptr2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(ptr2 != NULL);
    test_assert(ptr1 == ptr2);
    ecs_fini(world);
}


void test_EcsPrefab_tc_prefab_empty(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_PREFAB(world, MyPrefab, 0);

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsEntity e2 = ecs_new(world, MyPrefab_h);
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

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);
    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
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

    EcsEntity e1 = ecs_new(world, Prefab1_h);
    test_assert(e1 != 0);

    EcsEntity e2 = ecs_new(world, Prefab2_h);
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

    Foo *foo_ptr = ecs_get_ptr(world, MyPrefab_h, Foo_h);
    foo_ptr->x = 10;

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);

    ecs_stage_add(world, e1, Foo_h);
    ecs_commit(world, e1);

    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    ecs_stage_add(world, e2, Foo_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1->x == 10);
    test_assert(foo2->x == 10);

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

    Foo *foo_ptr = ecs_get_ptr(world, MyPrefab_h, Foo_h);
    foo_ptr->x = 10;
    Bar *bar_ptr = ecs_get_ptr(world, MyPrefab_h, Bar_h);
    bar_ptr->y = 20;

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);

    ecs_stage_add(world, e1, Foo_h);
    ecs_stage_add(world, e1, Bar_h);
    ecs_commit(world, e1);

    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    ecs_stage_add(world, e2, Foo_h);
    ecs_stage_add(world, e2, Bar_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 != bar2);

    test_assertint(foo1->x, 10);
    test_assertint(foo2->x, 10);
    test_assertint(bar1->y, 20);
    test_assertint(bar2->y, 20);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_override_1_after_1(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);

    ecs_set(world, MyPrefab_h, Foo, {.x = 10});
    ecs_set(world, MyPrefab_h, Bar, {.y = 20});

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);

    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    ecs_stage_add(world, e1, Foo_h);
    ecs_commit(world, e1);

    ecs_stage_add(world, e2, Foo_h);
    ecs_commit(world, e2);

    ecs_set(world, e1, Foo, {.x = 15});
    ecs_set(world, e2, Foo, {.x = 25});

    test_assertint(ecs_get(world, e1, Foo).x, 15);
    test_assertint(ecs_get(world, e2, Foo).x, 25);
    test_assertint(ecs_get(world, e1, Bar).y, 20);
    test_assertint(ecs_get(world, e2, Bar).y, 20);

    ecs_stage_add(world, e1, Bar_h);
    ecs_commit(world, e1);

    ecs_stage_add(world, e2, Bar_h);
    ecs_commit(world, e2);

    ecs_set(world, e1, Bar, {.y = 35});

    test_assertint(ecs_get(world, e1, Foo).x, 15);
    test_assertint(ecs_get(world, e2, Foo).x, 25);
    test_assertint(ecs_get(world, e1, Bar).y, 35);
    test_assertint(ecs_get(world, e2, Bar).y, 20);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_override_w_family(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);
    ECS_FAMILY(world, PrefabFamily, MyPrefab, Foo);

    Foo *foo_ptr = ecs_get_ptr(world, MyPrefab_h, Foo_h);
    foo_ptr->x = 10;

    EcsEntity e1 = ecs_new(world, PrefabFamily_h);
    test_assert(e1 != 0);

    EcsEntity e2 = ecs_new(world, PrefabFamily_h);
    test_assert(e2 != 0);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 == bar2);

    test_assert(foo1->x == 10);
    test_assert(foo2->x == 10);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_override_2_w_family(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_PREFAB(world, MyPrefab, MyFamily);
    ECS_FAMILY(world, PrefabFamily, MyPrefab, Foo, Bar);

    Foo *foo_ptr = ecs_get_ptr(world, MyPrefab_h, Foo_h);
    foo_ptr->x = 10;
    Bar *bar_ptr = ecs_get_ptr(world, MyPrefab_h, Bar_h);
    bar_ptr->y = 20;

    EcsEntity e1 = ecs_new(world, PrefabFamily_h);
    test_assert(e1 != 0);

    EcsEntity e2 = ecs_new(world, PrefabFamily_h);
    test_assert(e2 != 0);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 != bar2);

    test_assertint(foo1->x, 10);
    test_assertint(foo2->x, 10);
    test_assertint(bar1->y, 20);
    test_assertint(bar2->y, 20);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_add_component(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Foo);

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e2 != 0);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    test_assert(foo1 == foo2);

    ecs_stage_add(world, MyPrefab_h, Bar_h);
    ecs_commit(world, MyPrefab_h);

    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
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

    EcsEntity e1 = ecs_new(world, 0);
    test_assert(e1 != 0);
    EcsEntity e2 = ecs_new(world, 0);
    test_assert(e2 != 0);

    ecs_stage_add(world, e1, MyPrefab_h);
    ecs_commit(world, e1);

    ecs_stage_add(world, e2, MyPrefab_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
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

    EcsEntity e1 = ecs_new(world, Foo_h);
    test_assert(e1 != 0);
    EcsEntity e2 = ecs_new(world, Foo_h);
    test_assert(e2 != 0);

    ecs_stage_add(world, e1, MyPrefab_h);
    ecs_commit(world, e1);

    ecs_stage_add(world, e2, MyPrefab_h);
    ecs_commit(world, e2);

    Foo *foo1 = ecs_get_ptr(world, e1, Foo_h);
    test_assert(foo1 != NULL);
    Bar *bar1 = ecs_get_ptr(world, e1, Bar_h);
    test_assert(bar1 != NULL);

    Foo *foo2 = ecs_get_ptr(world, e2, Foo_h);
    test_assert(foo2 != NULL);
    Bar *bar2 = ecs_get_ptr(world, e2, Bar_h);
    test_assert(bar2 != NULL);

    test_assert(foo1 != foo2);
    test_assert(bar1 == bar2);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_override_w_set(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyPrefab_h);
    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, MyPrefab_h, Foo, {10});
    ecs_set(world, MyPrefab_h, Bar, {20});
    ecs_set(world, e1, Foo, {30});

    test_assertint(ecs_get(world, e1, Foo).x, 30);
    test_assertint(ecs_get(world, e1, Bar).y, 20);
    test_assertint(ecs_get(world, e2, Foo).x, 10);
    test_assertint(ecs_get(world, e2, Bar).y, 20);

    ecs_fini(world);
}

void test_EcsPrefab_tc_prefab_add_to_entity_w_prefab(
    test_EcsPrefab this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    
    ECS_PREFAB(world, Prefab1, EcsContainer, Foo);
    ECS_ENTITY(world, E1, Prefab1);
    ECS_ENTITY(world, E2, E1);

    test_assert(ecs_has(world, E1_h, Foo_h));
    test_assert(ecs_has(world, E1_h, EcsContainer_h));
    test_assert(!ecs_has(world, E1_h, EcsPrefab_h));
    test_assert(ecs_has(world, E2_h, E1_h));

    ecs_fini(world);
}
