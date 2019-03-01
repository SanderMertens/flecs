/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef int Foo;
typedef int Bar;

void test_EcsType_tc_type_nested_1(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, BaseType, Foo);
    ECS_TYPE(world, SubType, BaseType);

    EcsEntity e = ecs_new(world, SubType_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_nested_2(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, BaseType1, Foo);
    ECS_TYPE(world, BaseType2, Bar);
    ECS_TYPE(world, SubType, BaseType1, BaseType2);

    EcsEntity e = ecs_new(world, SubType_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_2_lvl_nested_1(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, BaseType, Foo);
    ECS_TYPE(world, SubType, BaseType);
    ECS_TYPE(world, SubSubType, SubType);

    EcsEntity e = ecs_new(world, SubSubType_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_2_lvl_nested_2(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, BaseType1, Foo);
    ECS_TYPE(world, BaseType2, Bar);
    ECS_TYPE(world, SubType, BaseType1, BaseType2);
    ECS_TYPE(world, SubSubType, SubType);

    EcsEntity e = ecs_new(world, SubSubType_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void TestSystem1(EcsRows *rows) { }
void TestSystem2(EcsRows *rows) { }

void test_EcsType_tc_type_of_systems_1(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_TYPE(world, Type, TestSystem1);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    ecs_enable(world, Type_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_of_systems_2(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_SYSTEM(world, TestSystem2, EcsOnFrame, Foo);
    ECS_TYPE(world, Type, TestSystem1, TestSystem2);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    test_assert(ecs_is_enabled(world, TestSystem2_h));
    ecs_enable(world, Type_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));
    test_assert(!ecs_is_enabled(world, TestSystem2_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_of_systems_1_nested(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_TYPE(world, Type, TestSystem1);
    ECS_TYPE(world, SubType, Type);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    ecs_enable(world, SubType_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_of_systems_1_nested_2_lvl(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_TYPE(world, Type, TestSystem1);
    ECS_TYPE(world, SubType, Type);
    ECS_TYPE(world, SubSubType, SubType);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    ecs_enable(world, SubSubType_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_of_systems_2_nested(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_SYSTEM(world, TestSystem2, EcsOnFrame, Foo);
    ECS_TYPE(world, Type1, TestSystem1);
    ECS_TYPE(world, Type2, TestSystem2);
    ECS_TYPE(world, SubType, Type1, Type2);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    test_assert(ecs_is_enabled(world, TestSystem2_h));
    ecs_enable(world, SubType_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));
    test_assert(!ecs_is_enabled(world, TestSystem2_h));

    ecs_fini(world);
}

void test_EcsType_tc_type_of_systems_2_nested_2_lvl(
    test_EcsType this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem1, EcsOnFrame, Foo);
    ECS_SYSTEM(world, TestSystem2, EcsOnFrame, Foo);
    ECS_TYPE(world, Type1, TestSystem1);
    ECS_TYPE(world, Type2, TestSystem2);
    ECS_TYPE(world, SubType1, Type1);
    ECS_TYPE(world, SubType2, Type2);
    ECS_TYPE(world, SubSubType, SubType1, SubType2);

    test_assert(ecs_is_enabled(world, TestSystem1_h));
    test_assert(ecs_is_enabled(world, TestSystem2_h));
    ecs_enable(world, SubSubType_h, false);
    test_assert(!ecs_is_enabled(world, TestSystem1_h));
    test_assert(!ecs_is_enabled(world, TestSystem2_h));

    ecs_fini(world);
}
