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

    ecs_add(world, e, Foo_h);
    ecs_add(world, e, Hello_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Foo_h));

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

    ecs_add(world, e, Foo_h);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

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

    ecs_add(world, e, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_commit(world, e);
    test_assert(ecs_has(world, e, Foo_h));

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

    ecs_add(world, e, MyFamily_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

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
