/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

void test_EcsSet_tc_set_int16(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int16_t);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, int16_t, 256);

    test_assertint(ecs_get(world, e, int16_t), 256);

    ecs_fini(world);
}


void test_EcsSet_tc_set_int32(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int32_t);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, int32_t, 65536);

    test_assertint(ecs_get(world, e, int32_t), 65536);

    ecs_fini(world);
}


void test_EcsSet_tc_set_int64(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int64_t);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, int64_t, INT_MAX);

    test_assertint(ecs_get(world, e, int64_t), INT_MAX);

    ecs_fini(world);
}


void test_EcsSet_tc_set_int8(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int8_t);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, int8_t, 64);

    test_assertint(ecs_get(world, e, int8_t), 64);

    ecs_fini(world);
}

typedef struct Struct64 {
    int32_t x;
    int32_t y;
} Struct64;

void test_EcsSet_tc_set_struct64(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Struct64);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Struct64, {.x = 10, .y = 20});

    Struct64 v = ecs_get(world, e, Struct64);
    test_assertint(v.x, 10);
    test_assertint(v.y, 20);

    ecs_fini(world);
}

typedef struct Struct128 {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t v;
} Struct128;

void test_EcsSet_tc_set_struct128(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Struct128);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Struct128, {.x = 10, .y = 20, .z = 30, .v = 40});

    Struct128 v = ecs_get(world, e, Struct128);
    test_assertint(v.x, 10);
    test_assertint(v.y, 20);
    test_assertint(v.z, 30);
    test_assertint(v.v, 40);

    ecs_fini(world);
}

typedef int64_t Array256[4];

void test_EcsSet_tc_set_array256(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Array256);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_set(world, e, Array256, {10, 20, 30, 40});

    int64_t* v = ecs_get(world, e, Array256);
    test_assertint(v[0], 10);
    test_assertint(v[1], 20);
    test_assertint(v[2], 30);
    test_assertint(v[3], 40);

    ecs_fini(world);
}
