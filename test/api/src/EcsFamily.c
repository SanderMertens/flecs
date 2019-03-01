/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef int Foo;
typedef int Bar;

void test_EcsType_tc_family_nested_1(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, BaseFamily, Foo);
    ECS_TYPE(world, SubFamily, BaseFamily);

    EcsEntity e = ecs_new(world, SubFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_nested_2(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, BaseFamily1, Foo);
    ECS_TYPE(world, BaseFamily2, Bar);
    ECS_TYPE(world, SubFamily, BaseFamily1, BaseFamily2);

    EcsEntity e = ecs_new(world, SubFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_2_lvl_nested_1(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, BaseFamily, Foo);
    ECS_TYPE(world, SubFamily, BaseFamily);
    ECS_TYPE(world, SubSubFamily, SubFamily);

    EcsEntity e = ecs_new(world, SubSubFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_2_lvl_nested_2(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, BaseFamily1, Foo);
    ECS_TYPE(world, BaseFamily2, Bar);
    ECS_TYPE(world, SubFamily, BaseFamily1, BaseFamily2);
    ECS_TYPE(world, SubSubFamily, SubFamily);

    EcsEntity e = ecs_new(world, SubSubFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void TestSystem1(EcsRows *rows) { }
void TestSystem2(EcsRows *rows) { }

void test_EcsType_tc_family_of_systems_1(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_TYPE(world, Family, TestSystem1);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    ecs_enable(world, Family_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_of_systems_2(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_SYSTEM(world, TestSystem2, EcsOnFrame, Foo);
    ECS_TYPE(world, Family, TestSystem1, TestSystem2);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    test_assert(ecs_is_enabled(world, TestSystem2_h));
    ecs_enable(world, Family_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));
    test_assert(!ecs_is_enabled(world, TestSystem2_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_of_systems_1_nested(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_TYPE(world, Family, TestSystem1);
    ECS_TYPE(world, SubFamily, Family);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    ecs_enable(world, SubFamily_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_of_systems_1_nested_2_lvl(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_TYPE(world, Family, TestSystem1);
    ECS_TYPE(world, SubFamily, Family);
    ECS_TYPE(world, SubSubFamily, SubFamily);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    ecs_enable(world, SubSubFamily_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_of_systems_2_nested(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_SYSTEM(world, TestSystem2, EcsOnFrame, Foo);
    ECS_TYPE(world, Family1, TestSystem1);
    ECS_TYPE(world, Family2, TestSystem2);
    ECS_TYPE(world, SubFamily, Family1, Family2);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    test_assert(ecs_is_enabled(world, TestSystem2_h));
    ecs_enable(world, SubFamily_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));
    test_assert(!ecs_is_enabled(world, TestSystem2_h));

    ecs_fini(world);
}

void test_EcsType_tc_family_of_systems_2_nested_2_lvl(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_SYSTEM(world, TestSystem2, EcsOnFrame, Foo);
    ECS_TYPE(world, Family1, TestSystem1);
    ECS_TYPE(world, Family2, TestSystem2);
    ECS_TYPE(world, SubFamily1, Family1);
    ECS_TYPE(world, SubFamily2, Family2);
    ECS_TYPE(world, SubSubFamily, SubFamily1, SubFamily2);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    test_assert(ecs_is_enabled(world, TestSystem2_h));
    ecs_enable(world, SubSubFamily_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));
    test_assert(!ecs_is_enabled(world, TestSystem2_h));

    ecs_fini(world);
}
