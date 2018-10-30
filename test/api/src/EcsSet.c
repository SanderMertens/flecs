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

typedef struct Context {
    EcsHandle handle;
    EcsHandle handle2;
} Context;

static
void SetExisting(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsHandle int8_t_h = ctx->handle;
    EcsWorld *world = rows->world;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        int8_t value = ecs_get(world, ecs_entity(row), int8_t);
        ecs_set(world, ecs_entity(row), int8_t, value + 1);
    }
}

void test_EcsSet_tc_set_existing_in_progress(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int8_t);
    ECS_SYSTEM(world, SetExisting, EcsPeriodic, int8_t);

    EcsHandle e1 = ecs_new(world, int8_t_h);
    EcsHandle e2 = ecs_new(world, int8_t_h);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, int8_t, 32);
    ecs_set(world, e2, int8_t, 64);

    Context ctx = {.handle = int8_t_h};

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ecs_get(world, e1, int8_t), 33);
    test_assertint(ecs_get(world, e2, int8_t), 65);

    ecs_fini(world);
}

static
void SetNew(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsHandle int16_t_h = ctx->handle;
    EcsWorld *world = rows->world;
    int16_t value = 10;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ecs_set(world, ecs_entity(row), int16_t, value);
        value += 10;
    }
}

void test_EcsSet_tc_set_new_in_progress(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int8_t);
    ECS_COMPONENT(world, int16_t);
    ECS_SYSTEM(world, SetNew, EcsPeriodic, int8_t);

    EcsHandle e1 = ecs_new(world, int8_t_h);
    EcsHandle e2 = ecs_new(world, int8_t_h);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, int8_t, 32);
    ecs_set(world, e2, int8_t, 64);

    Context ctx = {.handle = int16_t_h};

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ecs_get(world, e1, int8_t), 32);
    test_assert(ecs_get_ptr(world, e1, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e1, int16_t), 10);
    test_assertint(ecs_get(world, e2, int8_t), 64);
    test_assert(ecs_get_ptr(world, e2, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e2, int16_t), 20);

    ecs_fini(world);
}

static
void SetAdd(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsHandle int16_t_h = ctx->handle;
    EcsWorld *world = rows->world;
    int16_t value = 10;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        ecs_add(world, entity, int16_t_h);
        ecs_commit(world, entity);
        ecs_set(world, entity, int16_t, value);
        value += 10;
    }
}

void test_EcsSet_tc_set_add_in_progress(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int8_t);
    ECS_COMPONENT(world, int16_t);
    ECS_SYSTEM(world, SetAdd, EcsPeriodic, int8_t);

    EcsHandle e1 = ecs_new(world, int8_t_h);
    EcsHandle e2 = ecs_new(world, int8_t_h);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, int8_t, 32);
    ecs_set(world, e2, int8_t, 64);

    Context ctx = {.handle = int16_t_h};

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ecs_get(world, e1, int8_t), 32);
    test_assert(ecs_get_ptr(world, e1, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e1, int16_t), 10);
    test_assertint(ecs_get(world, e2, int8_t), 64);
    test_assert(ecs_get_ptr(world, e2, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e2, int16_t), 20);

    ecs_fini(world);
}

static
void SetNew2(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsHandle int16_t_h = ctx->handle;
    EcsHandle int32_t_h = ctx->handle2;
    EcsWorld *world = rows->world;
    int16_t value1 = 10;
    int32_t value2 = 1;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ecs_set(world, ecs_entity(row), int16_t, value1);
        ecs_set(world, ecs_entity(row), int32_t, value2);
        value1 += 10;
        value2 += 1;
    }
}

void test_EcsSet_tc_set_2_new_in_progress(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int8_t);
    ECS_COMPONENT(world, int16_t);
    ECS_COMPONENT(world, int32_t);
    ECS_SYSTEM(world, SetNew2, EcsPeriodic, int8_t);

    EcsHandle e1 = ecs_new(world, int8_t_h);
    EcsHandle e2 = ecs_new(world, int8_t_h);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, int8_t, 32);
    ecs_set(world, e2, int8_t, 64);

    Context ctx = {.handle = int16_t_h, .handle2 = int32_t_h};

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ecs_get(world, e1, int8_t), 32);
    test_assert(ecs_get_ptr(world, e1, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e1, int16_t), 10);
    test_assert(ecs_get_ptr(world, e1, int32_t_h) != NULL);
    test_assertint(ecs_get(world, e1, int32_t), 1);

    test_assertint(ecs_get(world, e2, int8_t), 64);
    test_assert(ecs_get_ptr(world, e2, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e2, int16_t), 20);
    test_assert(ecs_get_ptr(world, e2, int32_t_h) != NULL);
    test_assertint(ecs_get(world, e2, int32_t), 2);

    ecs_fini(world);
}

static
void SetAdd2(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsHandle int16_t_h = ctx->handle;
    EcsHandle int32_t_h = ctx->handle2;
    EcsWorld *world = rows->world;
    int16_t value = 10;
    int16_t value2 = 1;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        ecs_add(world, entity, int16_t_h);
        ecs_add(world, entity, int32_t_h);
        ecs_commit(world, entity);
        ecs_set(world, entity, int16_t, value);
        ecs_set(world, entity, int32_t, value2);
        value += 10;
        value2 += 1;
    }
}

void test_EcsSet_tc_set_2_add_in_progress(
    test_EcsSet this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, int8_t);
    ECS_COMPONENT(world, int16_t);
    ECS_COMPONENT(world, int32_t);
    ECS_SYSTEM(world, SetAdd2, EcsPeriodic, int8_t);

    EcsHandle e1 = ecs_new(world, int8_t_h);
    EcsHandle e2 = ecs_new(world, int8_t_h);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, int8_t, 32);
    ecs_set(world, e2, int8_t, 64);

    Context ctx = {.handle = int16_t_h, .handle2 = int32_t_h};

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ecs_get(world, e1, int8_t), 32);
    test_assert(ecs_get_ptr(world, e1, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e1, int16_t), 10);
    test_assert(ecs_get_ptr(world, e1, int32_t_h) != NULL);
    test_assertint(ecs_get(world, e1, int32_t), 1);

    test_assertint(ecs_get(world, e2, int8_t), 64);
    test_assert(ecs_get_ptr(world, e2, int16_t_h) != NULL);
    test_assertint(ecs_get(world, e2, int16_t), 20);
    test_assert(ecs_get_ptr(world, e2, int32_t_h) != NULL);
    test_assertint(ecs_get(world, e2, int32_t), 2);

    ecs_fini(world);
}
