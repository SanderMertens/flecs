/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

#define COLUMN_COUNT (8)
#define COUNT (8)

typedef int Foo;
typedef int Bar;
typedef int Hello;
typedef int World;
typedef int Bleep;
typedef int Bloop;

typedef struct Context {
    int count;
    EcsEntity entities[COUNT];
    int column[COLUMN_COUNT][COUNT];
    EcsEntity component[COLUMN_COUNT][COUNT];
    EcsEntity entity[COLUMN_COUNT][COUNT];
    int column_count;
} Context;

static
void TestSystem(EcsRows *rows) {
    void *row;
    Context *ctx = ecs_get_context(rows->world);
    int column;

    ctx->column_count = rows->column_count;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ctx->entities[ctx->count] = ecs_entity(rows, row, ECS_ROW_ENTITY);

        for (column = 0; column < rows->column_count; column ++) {
            int* ptr = ecs_data(rows, row, column);

            if (ptr) {
                ctx->column[column][ctx->count] = *ptr;
            } else {
                ctx->column[column][ctx->count] = 0;
            }

            ctx->component[column][ctx->count] = rows->components[column];
            ctx->entity[column][ctx->count] = ecs_entity(rows, row, column);
        }

        ctx->count ++;
    }
}

void test_EcsOnFrameSystem_tc_system_component(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *bar_3 = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 40);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_component(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar, Hello);
    ECS_TYPE(world, Type3, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Hello);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);
    EcsEntity e5 = ecs_new(world, Type3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *hello_4 = ecs_get_ptr(world, e4, Hello_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *bar_5 = ecs_get_ptr(world, e5, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *hello_4 = 80;
    *foo_5 = 90;
    *bar_5 = 100;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 40);
    test_assertint(ctx.column[1][0], 50);
    test_assertint(ctx.column[0][1], 60);
    test_assertint(ctx.column[1][1], 80);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Hello_h);

    ecs_fini(world);
}


void test_EcsOnFrameSystem_tc_system_prefab(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Foo);
    ECS_TYPE(world, MyType, MyPrefab, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, MyType_h);
    EcsEntity e4 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_prefab = ecs_get_ptr(world, MyPrefab_h, Foo_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *bar_3 = 50;
    *bar_4 = 60;
    *foo_prefab = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e3);
    test_assert(ctx.entities[2] == e4);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 70);
    test_assertint(ctx.column[0][2], 70);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}


void test_EcsOnFrameSystem_tc_system_prefab_component(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_PREFAB(world, MyPrefab, Foo);
    ECS_TYPE(world, MyType, MyPrefab, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, MyType_h);
    EcsEntity e4 = ecs_new(world, MyType_h);
    EcsEntity e5 = ecs_new(world, Hello_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_prefab = ecs_get_ptr(world, MyPrefab_h, Foo_h);
    int *hello_5 = ecs_get_ptr(world, e5, Hello_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *bar_3 = 40;
    *bar_4 = 60;
    *foo_prefab = 70;
    *hello_5 = 80;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 70);
    test_assertint(ctx.column[1][0], 40);
    test_assertint(ctx.column[0][1], 70);
    test_assertint(ctx.column[1][1], 60);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_component_1_not(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, !Hello);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 60);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Hello_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_component_2_not(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_TYPE(world, Type3, Foo, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, !Hello, !Bar);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);
    EcsEntity e5 = ecs_new(world, Type3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *world_5 = ecs_get_ptr(world, e5, World_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *foo_5 = 80;
    *world_5 = 90;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 80);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Hello_h);
    test_assertint(ctx.component[2][0], Bar_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_component_1_not(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_TYPE(world, Type3, Foo, Bar, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bar, !Hello);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);
    EcsEntity e5 = ecs_new(world, Type3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *bar_5 = ecs_get_ptr(world, e5, Bar_h);
    int *world_5 = ecs_get_ptr(world, e5, World_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *foo_5 = 80;
    *bar_5 = 90;
    *world_5 = 100;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 60);
    test_assertint(ctx.column[1][0], 70);
    test_assertint(ctx.column[0][1], 80);
    test_assertint(ctx.column[1][1], 90);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[2][0], Hello_h);

    ecs_fini(world);
}


void test_EcsOnFrameSystem_tc_system_2_component_2_not(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_TYPE(world, Type3, Foo, Bar, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bar, !Hello, !World);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);
    EcsEntity e5 = ecs_new(world, Type3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *bar_5 = ecs_get_ptr(world, e5, Bar_h);
    int *world_5 = ecs_get_ptr(world, e5, World_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *foo_5 = 80;
    *bar_5 = 90;
    *world_5 = 100;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 4);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e4);
    test_assertint(ctx.column[0][0], 60);
    test_assertint(ctx.column[1][0], 70);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[2][0], Hello_h);
    test_assertint(ctx.component[3][0], World_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_component_1_or2(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_TYPE(world, Type3, Foo, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bar|Hello);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);
    EcsEntity e5 = ecs_new(world, Type3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *world_5 = ecs_get_ptr(world, e5, World_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *foo_5 = 80;
    *world_5 = 100;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 40);
    test_assertint(ctx.column[1][0], 50);
    test_assertint(ctx.column[0][1], 60);
    test_assertint(ctx.column[1][1], 70);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Hello_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);

    ecs_fini(world);
}


void test_EcsOnFrameSystem_tc_system_1_component_2_or2(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_COMPONENT(world, Bleep);
    ECS_COMPONENT(world, Bloop);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bloop);
    ECS_TYPE(world, Type3, Foo, Bleep, World);
    ECS_TYPE(world, Type4, Foo, Hello, Bloop);
    ECS_TYPE(world, Type5, Bloop, Hello);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bleep|Bloop, Hello|World);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Type1_h);
    EcsEntity e3 = ecs_new(world, Type2_h);
    EcsEntity e4 = ecs_new(world, Type3_h);
    EcsEntity e5 = ecs_new(world, Type4_h);
    EcsEntity e6 = ecs_new(world, Type5_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *foo_2 = ecs_get_ptr(world, e2, Foo_h);
    int *hello_2 = ecs_get_ptr(world, e2, Hello_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bloop_3 = ecs_get_ptr(world, e3, Bloop_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bleep_4 = ecs_get_ptr(world, e4, Bleep_h);
    int *world_4 = ecs_get_ptr(world, e4, World_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *hello_5 = ecs_get_ptr(world, e5, Hello_h);
    int *bloop_5 = ecs_get_ptr(world, e5, Bloop_h);
    int *bloop_6 = ecs_get_ptr(world, e6, Bloop_h);
    int *hello_6 = ecs_get_ptr(world, e6, Hello_h);

    *foo_1 = 10;
    *foo_2 = 20;
    *hello_2 = 30;
    *foo_3 = 40;
    *bloop_3 = 50;
    *foo_4 = 60;
    *bleep_4 = 70;
    *world_4 = 80;
    *foo_5 = 90;
    *hello_5 = 100;
    *bloop_5 = 110;
    *bloop_6 = 120;
    *hello_6 = 130;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 60);
    test_assertint(ctx.column[1][0], 70);
    test_assertint(ctx.column[2][0], 80);
    test_assertint(ctx.column[0][1], 90);
    test_assertint(ctx.column[1][1], 110);
    test_assertint(ctx.column[2][1], 100);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bleep_h);
    test_assertint(ctx.component[2][0], World_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bloop_h);
    test_assertint(ctx.component[2][1], Hello_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_component_2_or3(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_COMPONENT(world, Bleep);
    ECS_COMPONENT(world, Bloop);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_TYPE(world, Type3, Foo, Bar, World);
    ECS_TYPE(world, Type4, Foo, Bar, Bleep);
    ECS_TYPE(world, Type5, Foo, Bar, Hello);
    ECS_TYPE(world, Type6, Foo, Bar, Bloop);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bar, Hello|World|Bleep);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Type1_h);
    EcsEntity e3 = ecs_new(world, Type2_h);
    EcsEntity e4 = ecs_new(world, Type3_h);
    EcsEntity e5 = ecs_new(world, Type4_h);
    EcsEntity e6 = ecs_new(world, Type5_h);
    EcsEntity e7 = ecs_new(world, Type6_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *foo_2 = ecs_get_ptr(world, e2, Foo_h);
    int *hello_2 = ecs_get_ptr(world, e2, Hello_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *world_4 = ecs_get_ptr(world, e4, World_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *bar_5 = ecs_get_ptr(world, e5, Bar_h);
    int *bleep_5 = ecs_get_ptr(world, e5, Bleep_h);
    int *foo_6 = ecs_get_ptr(world, e6, Foo_h);
    int *bar_6 = ecs_get_ptr(world, e6, Bar_h);
    int *hello_6 = ecs_get_ptr(world, e6, Hello_h);
    int *foo_7 = ecs_get_ptr(world, e7, Foo_h);
    int *bar_7 = ecs_get_ptr(world, e7, Bar_h);
    int *bloop_7 = ecs_get_ptr(world, e7, Bloop_h);

    *foo_1 = 10;
    *foo_2 = 20;
    *hello_2 = 30;
    *foo_3 = 40;
    *bar_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *world_4 = 80;
    *foo_5 = 90;
    *bar_5 = 100;
    *bleep_5 = 110;
    *foo_6 = 120;
    *bar_6 = 130;
    *hello_6 = 140;
    *foo_7 = 150;
    *bar_7 = 160;
    *bloop_7 = 170;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assert(ctx.entities[2] == e6);
    test_assertint(ctx.column[0][0], 60);
    test_assertint(ctx.column[1][0], 70);
    test_assertint(ctx.column[2][0], 80);
    test_assertint(ctx.column[0][1], 90);
    test_assertint(ctx.column[1][1], 100);
    test_assertint(ctx.column[2][1], 110);
    test_assertint(ctx.column[0][2], 120);
    test_assertint(ctx.column[1][2], 130);
    test_assertint(ctx.column[2][2], 140);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[2][0], World_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.component[2][1], Bleep_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[1][2], Bar_h);
    test_assertint(ctx.component[2][2], Hello_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_component_or2_both_match(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, Foo, Bar, Hello);
    ECS_TYPE(world, Type2, Foo, Bar);
    ECS_TYPE(world, Type3, Foo, Hello);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, Bar|Hello);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, Type1_h);
    EcsEntity e4 = ecs_new(world, Type2_h);
    EcsEntity e5 = ecs_new(world, Type3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);
    int *hello_3 = ecs_get_ptr(world, e3, Hello_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bar_4 = ecs_get_ptr(world, e4, Bar_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *hello_5 = ecs_get_ptr(world, e5, Hello_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *bar_3 = 50;
    *hello_3 = 60;
    *foo_4 = 70;
    *bar_4 = 80;
    *foo_5 = 90;
    *hello_5 = 100;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assert(ctx.entities[2] == e5);
    test_assertint(ctx.column[0][0], 40);
    test_assertint(ctx.column[1][0], 50);
    test_assertint(ctx.column[0][1], 70);
    test_assertint(ctx.column[1][1], 80);
    test_assertint(ctx.column[0][2], 90);
    test_assertint(ctx.column[1][2], 100);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[1][2], Hello_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_or2(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, Foo);
    ECS_TYPE(world, Type2, Foo, Hello);
    ECS_TYPE(world, Type3, Foo, World);
    ECS_TYPE(world, Type4, Hello);
    ECS_TYPE(world, Type5, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Hello|World);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Type2_h);
    EcsEntity e3 = ecs_new(world, Type3_h);
    EcsEntity e4 = ecs_new(world, Type4_h);
    EcsEntity e5 = ecs_new(world, Type5_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *foo_2 = ecs_get_ptr(world, e2, Foo_h);
    int *hello_2 = ecs_get_ptr(world, e2, Hello_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *world_3 = ecs_get_ptr(world, e3, World_h);
    int *hello_4 = ecs_get_ptr(world, e4, Hello_h);
    int *world_5 = ecs_get_ptr(world, e5, World_h);

    *foo_1 = 10;
    *foo_2 = 20;
    *hello_2 = 30;
    *foo_3 = 40;
    *world_3 = 50;
    *hello_4 = 60;
    *world_5 = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 4);
    test_assert(ctx.entities[0] == e2);
    test_assert(ctx.entities[1] == e3);
    test_assert(ctx.entities[2] == e4);
    test_assert(ctx.entities[3] == e5);
    test_assertint(ctx.column[0][0], 30);
    test_assertint(ctx.column[0][1], 50);
    test_assertint(ctx.column[0][2], 60);
    test_assertint(ctx.column[0][3], 70);
    test_assertint(ctx.component[0][0], Hello_h);
    test_assertint(ctx.component[0][1], World_h);
    test_assertint(ctx.component[0][2], Hello_h);
    test_assertint(ctx.component[0][3], World_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_or2(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_COMPONENT(world, Bleep);
    ECS_COMPONENT(world, Bloop);
    ECS_TYPE(world, Type1, Foo, Hello);
    ECS_TYPE(world, Type2, Foo, Bloop);
    ECS_TYPE(world, Type3, Foo, Bleep, World);
    ECS_TYPE(world, Type4, Foo, Hello, Bloop);
    ECS_TYPE(world, Type5, Bloop, Hello);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Bleep|Bloop, Hello|World);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Type1_h);
    EcsEntity e3 = ecs_new(world, Type2_h);
    EcsEntity e4 = ecs_new(world, Type3_h);
    EcsEntity e5 = ecs_new(world, Type4_h);
    EcsEntity e6 = ecs_new(world, Type5_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *foo_2 = ecs_get_ptr(world, e2, Foo_h);
    int *hello_2 = ecs_get_ptr(world, e2, Hello_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bloop_3 = ecs_get_ptr(world, e3, Bloop_h);
    int *foo_4 = ecs_get_ptr(world, e4, Foo_h);
    int *bleep_4 = ecs_get_ptr(world, e4, Bleep_h);
    int *world_4 = ecs_get_ptr(world, e4, World_h);
    int *foo_5 = ecs_get_ptr(world, e5, Foo_h);
    int *hello_5 = ecs_get_ptr(world, e5, Hello_h);
    int *bloop_5 = ecs_get_ptr(world, e5, Bloop_h);
    int *bloop_6 = ecs_get_ptr(world, e6, Bloop_h);
    int *hello_6 = ecs_get_ptr(world, e6, Hello_h);

    *foo_1 = 10;
    *foo_2 = 20;
    *hello_2 = 30;
    *foo_3 = 40;
    *bloop_3 = 50;
    *foo_4 = 60;
    *bleep_4 = 70;
    *world_4 = 80;
    *foo_5 = 90;
    *hello_5 = 100;
    *bloop_5 = 110;
    *bloop_6 = 120;
    *hello_6 = 130;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assert(ctx.entities[2] == e6);
    test_assertint(ctx.column[0][0], 70);
    test_assertint(ctx.column[1][0], 80);
    test_assertint(ctx.column[0][1], 110);
    test_assertint(ctx.column[1][1], 100);
    test_assertint(ctx.column[0][2], 120);
    test_assertint(ctx.column[1][2], 130);
    test_assertint(ctx.component[0][0], Bleep_h);
    test_assertint(ctx.component[1][0], World_h);
    test_assertint(ctx.component[0][1], Bloop_h);
    test_assertint(ctx.component[1][1], Hello_h);
    test_assertint(ctx.component[0][2], Bloop_h);
    test_assertint(ctx.component[1][2], Hello_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_from_container(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type1, EcsContainer, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, CONTAINER.Bar);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Bar_h);
    EcsEntity e5 = ecs_new(world, Bar_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_stage_add(world, e2, e1);
    ecs_commit(world, e2);
    ecs_stage_add(world, e4, e1);
    ecs_commit(world, e4);

    *(int*)ecs_get_ptr(world, e1, Bar_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Bar_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e2);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 10);
    test_assertint(ctx.component[0][0], Bar_h);
    test_assertint(ctx.component[0][1], Bar_h);
    test_assertint(ctx.entity[0][0], e1);
    test_assertint(ctx.entity[0][1], e1);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_and_1_from_container(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type1, EcsContainer, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, CONTAINER.Bar);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Bar_h);
    EcsEntity e5 = ecs_new(world, Bar_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_stage_add(world, e2, e1);
    ecs_commit(world, e2);
    ecs_stage_add(world, e4, Foo_h);
    ecs_stage_add(world, e4, e1);
    ecs_commit(world, e4);

    *(int*)ecs_get_ptr(world, e1, Bar_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Bar_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e2);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[1][0], 10);
    test_assertint(ctx.column[1][1], 10);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.entity[0][0], e2);
    test_assertint(ctx.entity[1][0], e1);
    test_assertint(ctx.entity[0][1], e4);
    test_assertint(ctx.entity[1][1], e1);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_from_container(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, EcsContainer, Bar);
    ECS_TYPE(world, Type2, EcsContainer, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, CONTAINER.Bar, CONTAINER.World);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Type2_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Bar_h);
    EcsEntity e5 = ecs_new(world, Bar_h);
    EcsEntity e6 = ecs_new(world, World_h);
    EcsEntity e7 = ecs_new(world, World_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);

    ecs_stage_add(world, e3, e1);
    ecs_stage_add(world, e3, e2);
    ecs_commit(world, e3);

    ecs_stage_add(world, e4, e1);
    ecs_stage_add(world, e4, e2);
    ecs_commit(world, e4);

    ecs_stage_add(world, e5, e1);
    ecs_commit(world, e5);

    ecs_stage_add(world, e6, e1);
    ecs_stage_add(world, e6, e2);
    ecs_commit(world, e6);

    ecs_stage_add(world, e7, e2);
    ecs_commit(world, e7);

    *(int*)ecs_get_ptr(world, e1, Bar_h) = 10;
    *(int*)ecs_get_ptr(world, e2, World_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Bar_h) = 50;
    *(int*)ecs_get_ptr(world, e6, World_h) = 60;
    *(int*)ecs_get_ptr(world, e7, World_h) = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assert(ctx.entities[2] == e6);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 20);
    test_assertint(ctx.column[0][1], 10);
    test_assertint(ctx.column[1][1], 20);
    test_assertint(ctx.column[0][2], 10);
    test_assertint(ctx.column[1][2], 20);
    test_assertint(ctx.component[0][0], Bar_h);
    test_assertint(ctx.component[1][0], World_h);
    test_assertint(ctx.component[0][1], Bar_h);
    test_assertint(ctx.component[1][1], World_h);
    test_assertint(ctx.component[0][2], Bar_h);
    test_assertint(ctx.component[1][2], World_h);
    test_assertint(ctx.entity[0][0], e1);
    test_assertint(ctx.entity[1][0], e2);
    test_assertint(ctx.entity[0][1], e1);
    test_assertint(ctx.entity[1][1], e2);
    test_assertint(ctx.entity[0][2], e1);
    test_assertint(ctx.entity[1][2], e2);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_from_container_or2(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, EcsContainer, Bar);
    ECS_TYPE(world, Type2, EcsContainer, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, CONTAINER.Bar|CONTAINER.World);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Type2_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Bar_h);
    EcsEntity e5 = ecs_new(world, Bar_h);
    EcsEntity e6 = ecs_new(world, World_h);
    EcsEntity e7 = ecs_new(world, World_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);

    ecs_stage_add(world, e3, e1);
    ecs_commit(world, e3);

    ecs_stage_add(world, e4, e2);
    ecs_commit(world, e4);

    ecs_commit(world, e5);

    ecs_stage_add(world, e6, e1);
    ecs_stage_add(world, e6, e2);
    ecs_commit(world, e6);

    ecs_commit(world, e7);

    *(int*)ecs_get_ptr(world, e1, Bar_h) = 10;
    *(int*)ecs_get_ptr(world, e2, World_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Bar_h) = 50;
    *(int*)ecs_get_ptr(world, e6, World_h) = 60;
    *(int*)ecs_get_ptr(world, e7, World_h) = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assert(ctx.entities[2] == e6);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[0][2], 10);
    test_assertint(ctx.component[0][0], Bar_h);
    test_assertint(ctx.component[0][1], World_h);
    test_assertint(ctx.component[0][2], Bar_h);
    test_assertint(ctx.entity[0][0], e1);
    test_assertint(ctx.entity[0][1], e2);
    test_assertint(ctx.entity[0][2], e1);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_from_container_or2(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_TYPE(world, Type1, EcsContainer, Foo);
    ECS_TYPE(world, Type2, EcsContainer, Bar);
    ECS_TYPE(world, Type3, EcsContainer, Hello);
    ECS_TYPE(world, Type4, EcsContainer, World);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame,
      CONTAINER.Foo|CONTAINER.Bar,
      CONTAINER.Hello|CONTAINER.World);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Type2_h);
    EcsEntity e3 = ecs_new(world, Type3_h);
    EcsEntity e4 = ecs_new(world, Type4_h);
    EcsEntity e5 = ecs_new(world, Foo_h);
    EcsEntity e6 = ecs_new(world, Foo_h);
    EcsEntity e7 = ecs_new(world, Foo_h);
    EcsEntity e8 = ecs_new(world, Foo_h);
    EcsEntity e9 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);
    test_assert(e8 != 0);
    test_assert(e9 != 0);

    ecs_stage_add(world, e5, e1);
    ecs_stage_add(world, e5, e2);
    ecs_commit(world, e5);

    ecs_stage_add(world, e6, e1);
    ecs_stage_add(world, e6, e3);
    ecs_commit(world, e6);

    ecs_stage_add(world, e7, e3);
    ecs_commit(world, e7);

    ecs_commit(world, e8);

    ecs_stage_add(world, e9, e2);
    ecs_stage_add(world, e9, e4);
    ecs_commit(world, e9);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Hello_h) = 30;
    *(int*)ecs_get_ptr(world, e4, World_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e6, Foo_h) = 60;
    *(int*)ecs_get_ptr(world, e7, Foo_h) = 70;
    *(int*)ecs_get_ptr(world, e8, Foo_h) = 70;
    *(int*)ecs_get_ptr(world, e9, Foo_h) = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e6);
    test_assert(ctx.entities[1] == e9);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 30);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 40);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Hello_h);
    test_assertint(ctx.component[0][1], Bar_h);
    test_assertint(ctx.component[1][1], World_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_from_container_not(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, Type1, EcsContainer, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, !CONTAINER.Foo);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Foo_h);
    EcsEntity e5 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_stage_add(world, e2, e1);
    ecs_commit(world, e2);
    ecs_stage_add(world, e4, e1);
    ecs_commit(world, e4);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Foo_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Foo_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e5);
    test_assert(ctx.entities[2] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 50);
    test_assertint(ctx.column[0][2], 30);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[1][2], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_from_container_not_from_prefab(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_PREFAB(world, Prefab, EcsContainer, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, !CONTAINER.Foo);

    EcsEntity e1 = ecs_new(world, Prefab_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Foo_h);
    EcsEntity e5 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_stage_add(world, e2, e1);
    ecs_commit(world, e2);
    ecs_stage_add(world, e4, e1);
    ecs_commit(world, e4);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Foo_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Foo_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e5);
    test_assert(ctx.entities[2] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 50);
    test_assertint(ctx.column[0][2], 30);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[1][2], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_from_container_not(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type1, EcsContainer, Foo);
    ECS_TYPE(world, Type2, EcsContainer, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, !CONTAINER.Foo, !CONTAINER.Bar);

    EcsEntity e1 = ecs_new(world, Type1_h);
    EcsEntity e2 = ecs_new(world, Type2_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Foo_h);
    EcsEntity e5 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_stage_add(world, e3, e1);
    ecs_commit(world, e3);
    ecs_stage_add(world, e4, e2);
    ecs_commit(world, e4);

    ecs_stage_add(world, e4, e1);
    ecs_stage_add(world, e4, e2);
    ecs_commit(world, e4);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Foo_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Foo_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 50);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Foo_h);
    test_assertint(ctx.component[2][0], Bar_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Foo_h);
    test_assertint(ctx.component[2][1], Bar_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_optional_not_set(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, ?Bar);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 0);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 0);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], 0);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], 0);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_optional_set(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, ?Bar);

    EcsEntity e1 = ecs_new(world, MyType_h);
    EcsEntity e2 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e1, Bar, {15});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e2, Bar, {25});

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 15);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 25);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_1_optional_set_1_not_set(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, MyType, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, ?Bar, ?Hello);

    EcsEntity e1 = ecs_new(world, MyType_h);
    EcsEntity e2 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e1, Bar, {15});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e2, Bar, {25});

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 15);
    test_assertint(ctx.column[2][0], 0);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 25);
    test_assertint(ctx.column[2][1], 0);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[2][0], 0);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.component[2][1], 0);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_optional_not_set(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, ?Bar, ?Hello);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 0);
    test_assertint(ctx.column[2][0], 0);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 0);
    test_assertint(ctx.column[2][1], 0);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], 0);
    test_assertint(ctx.component[2][0], 0);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], 0);
    test_assertint(ctx.component[2][1], 0);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_2_optional_set(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, MyType, Foo, Bar, Hello);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, ?Bar, ?Hello);

    EcsEntity e1 = ecs_new(world, MyType_h);
    EcsEntity e2 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e1, Bar, {15});
    ecs_set(world, e1, Hello, {18});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e2, Bar, {25});
    ecs_set(world, e2, Hello, {28});

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 3);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 15);
    test_assertint(ctx.column[2][0], 18);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 25);
    test_assertint(ctx.column[2][1], 28);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[2][0], Hello_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.component[2][1], Hello_h);
}

void test_EcsOnFrameSystem_tc_system_1_from_system(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, SYSTEM.Bar);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Bar_h);
    EcsEntity e5 = ecs_new(world, Bar_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_set(world, TestSystem_h, Bar, {60});

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Bar_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assert(ctx.entities[2] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 60);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 60);
    test_assertint(ctx.column[0][2], 30);
    test_assertint(ctx.column[1][2], 60);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[1][2], Bar_h);

    ecs_fini(world);
}

void InitBar(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Bar *v = ecs_data(rows, row, 0);
        *v = 60;
    }
}

void test_EcsOnFrameSystem_tc_system_1_from_system_implicit_add(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, InitBar, EcsOnAdd, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, SYSTEM.Bar);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Bar_h);
    EcsEntity e5 = ecs_new(world, Bar_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 20;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e4, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e5, Bar_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assert(ctx.entities[2] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 60);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[1][1], 60);
    test_assertint(ctx.column[0][2], 30);
    test_assertint(ctx.column[1][2], 60);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], Bar_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[1][2], Bar_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_on_demand(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *bar_3 = 50;

    ecs_set_context(world, &ctx);

    ecs_run(world, TestSystem_h, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 40);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_on_demand_w_filter(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyType, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Bar_h);
    EcsEntity e3 = ecs_new(world, MyType_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    int *foo_1 = ecs_get_ptr(world, e1, Foo_h);
    int *bar_2 = ecs_get_ptr(world, e2, Bar_h);
    int *foo_3 = ecs_get_ptr(world, e3, Foo_h);
    int *bar_3 = ecs_get_ptr(world, e3, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *bar_3 = 50;

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 0, 0, 0, Bar_h, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e3);
    test_assertint(ctx.column[0][0], 40);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void NullSystem(EcsRows *rows) {
    bool *called = ecs_get_context(rows->world);
    *called = true;
    test_assert(rows->first == rows->last);
    test_assertint(rows->column_count, 1);
    test_assertint(ecs_component(rows, 0), 0);
}

void test_EcsOnFrameSystem_tc_system_0_component(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_SYSTEM(world, NullSystem, EcsOnFrame, 0);

    bool called = false;
    ecs_set_context(world, &called);

    ecs_progress(world, 1);

    test_assert(called == true);

    ecs_fini(world);
}

typedef struct OneHandle_t {
    EcsEntity handle;
    EcsEntity entity;
    bool called;
} OneHandle_t;

typedef struct TwoHandle_t {
    EcsEntity handle1;
    EcsEntity handle2;
    EcsEntity entity;
    bool called;
} TwoHandle_t;

void TwoHandle(EcsRows *rows) {
    TwoHandle_t *ctx = ecs_get_context(rows->world);
    test_assert(rows->first != rows->last);
    test_assertint(rows->column_count, 3);
    test_assertint(ecs_component(rows, 0), ctx->handle1);
    test_assertint(ecs_component(rows, 1), ctx->handle1);
    test_assertint(ecs_component(rows, 2), ctx->handle2);
    ctx->entity = ecs_entity(rows, rows->first, ECS_ROW_ENTITY);
    ctx->called = true;
}

void test_EcsOnFrameSystem_tc_system_2_handle_component(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);

    ECS_SYSTEM(world, TwoHandle, EcsOnFrame, Foo, ID.Foo, ID.Bar);

    TwoHandle_t ctx = {.handle1 = Foo_h, .handle2 = Bar_h};
    ecs_set_context(world, &ctx);

    EcsEntity entity = ecs_new(world, Foo_h);

    ecs_progress(world, 1);

    test_assert(ctx.called == true);
    test_assert(ctx.entity = entity);

    ecs_fini(world);
}

void OneHandle(EcsRows *rows) {
    OneHandle_t *ctx = ecs_get_context(rows->world);
    test_assert(rows->first != rows->last);
    test_assertint(rows->column_count, 2);
    test_assertint(ecs_component(rows, 0), ctx->handle);
    test_assertint(ecs_component(rows, 1), ctx->handle);
    ctx->entity = ecs_entity(rows, rows->first, ECS_ROW_ENTITY);
    ctx->called = true;
}

void test_EcsOnFrameSystem_tc_system_handle_component(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);

    ECS_SYSTEM(world, OneHandle, EcsOnFrame, Foo, ID.Foo);

    OneHandle_t ctx = {.handle = Foo_h};
    ecs_set_context(world, &ctx);

    EcsEntity entity = ecs_new(world, Foo_h);

    ecs_progress(world, 1);

    test_assert(ctx.called == true);
    test_assert(ctx.entity = entity);

    ecs_fini(world);
}

void TwoHandleOnly(EcsRows *rows) {
    TwoHandle_t *ctx = ecs_get_context(rows->world);
    test_assert(rows->first == rows->last);
    test_assertint(rows->column_count, 2);
    test_assertint(ecs_component(rows, 0), ctx->handle1);
    test_assertint(ecs_component(rows, 1), ctx->handle2);
    ctx->called = true;
}

void test_EcsOnFrameSystem_tc_system_2_handle_only_component(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);

    ECS_SYSTEM(world, TwoHandleOnly, EcsOnFrame, ID.Foo, ID.Bar);

    TwoHandle_t ctx = {.handle1 = Foo_h, .handle2 = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_assert(ctx.called == true);

    ecs_fini(world);
}

void OneHandleOnly(EcsRows *rows) {
    OneHandle_t *ctx = ecs_get_context(rows->world);
    test_assert(rows->first == rows->last);
    test_assertint(rows->column_count, 1);
    test_assertint(ecs_component(rows, 0), ctx->handle);
    ctx->called = true;
}

void test_EcsOnFrameSystem_tc_system_handle_only_component(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);

    ECS_SYSTEM(world, OneHandleOnly, EcsOnFrame, ID.Foo);

    OneHandle_t ctx = {.handle = Foo_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_assert(ctx.called == true);

    ecs_fini(world);
}

void Invoked(EcsRows *rows) {
    bool *called = ecs_get_context(rows->world);
    *called = true;
}

void test_EcsOnFrameSystem_tc_system_disable(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Invoked, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assert(ecs_is_enabled(world, Invoked_h) == true);
    ecs_enable(world, Invoked_h, false);
    test_assert(ecs_is_enabled(world, Invoked_h) == false);

    bool called = false;
    ecs_set_context(world, &called);

    ecs_progress(world, 1);
    test_assert(called == false);

    test_assert(ecs_is_enabled(world, Invoked_h) == false);
    ecs_enable(world, Invoked_h, true);
    test_assert(ecs_is_enabled(world, Invoked_h) == true);

    ecs_progress(world, 1);
    test_assert(called == true);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_systen_2_component_2_or_w_set(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);

    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo | Bar);

    EcsEntity e1 = ecs_new(world, 0);
    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e1, Hello, {20});

    EcsEntity e2 = ecs_new(world, 1);
    ecs_set(world, e2, Bar, {30});

    ecs_set_context(world, &ctx);

    ecs_run(world, TestSystem_h, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 30);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Bar_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_prefab_or_component(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_PREFAB(world, MyPrefab, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Hello, Foo | Bar);

    EcsEntity e1 = ecs_new(world, Hello_h);
    EcsEntity e2 = ecs_new(world, Hello_h);
    EcsEntity e3 = ecs_new(world, MyPrefab_h);
    EcsEntity e4 = ecs_new(world, MyPrefab_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    ecs_add(world, e1, MyPrefab_h);
    ecs_add(world, e3, Hello_h);

    ecs_set(world, e1, Hello, {10});
    ecs_set(world, e2, Hello, {20});
    ecs_set(world, e3, Hello, {30});
    ecs_set(world, MyPrefab_h, Foo, {40});

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[1][0], 40);
    test_assertint(ctx.column[0][1], 30);
    test_assertint(ctx.column[1][1], 40);
    test_assertint(ctx.component[0][0], Hello_h);
    test_assertint(ctx.component[1][0], Foo_h);
    test_assertint(ctx.component[0][1], Hello_h);
    test_assertint(ctx.component[1][1], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_match_prefab(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_PREFAB(world, MyPrefab, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo, MyPrefab);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, MyPrefab_h);
    EcsEntity e3 = ecs_new(world, MyPrefab_h);

    ecs_set(world, MyPrefab_h, Foo, {10});
    ecs_set(world, e1, Foo, {20});
    ecs_set(world, e3, Foo, {30});

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e2);
    test_assert(ctx.entities[1] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 30);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], MyPrefab_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[1][1], MyPrefab_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_limit(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 0, 2, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 1, 0, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e2);
    test_assert(ctx.entities[1] == e3);
    test_assertint(ctx.column[0][0], 20);
    test_assertint(ctx.column[0][1], 30);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset_limit(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 1, 1, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e2);
    test_assertint(ctx.column[0][0], 20);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_limit_2_tables(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type_h);
    EcsEntity e5 = ecs_new(world, Type_h);
    EcsEntity e6 = ecs_new(world, Type_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 0, 4, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 4);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assert(ctx.entities[2] == e3);
    test_assert(ctx.entities[3] == e4);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[0][2], 30);
    test_assertint(ctx.column[0][3], 40);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[0][3], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset_2_tables(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type_h);
    EcsEntity e5 = ecs_new(world, Type_h);
    EcsEntity e6 = ecs_new(world, Type_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 2, 0, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 4);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assert(ctx.entities[2] == e5);
    test_assert(ctx.entities[3] == e6);
    test_assertint(ctx.column[0][0], 30);
    test_assertint(ctx.column[0][1], 40);
    test_assertint(ctx.column[0][2], 50);
    test_assertint(ctx.column[0][3], 60);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[0][3], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset_limit_2_tables(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type_h);
    EcsEntity e5 = ecs_new(world, Type_h);
    EcsEntity e6 = ecs_new(world, Type_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 2, 3, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 3);
    test_assertint(ctx.entities[0], e3);
    test_assertint(ctx.entities[1], e4);
    test_assertint(ctx.entities[2], e5);
    test_assertint(ctx.column[0][0], 30);
    test_assertint(ctx.column[0][1], 40);
    test_assertint(ctx.column[0][2], 50);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_limit_skip_table(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type_h);
    EcsEntity e5 = ecs_new(world, Type_h);
    EcsEntity e6 = ecs_new(world, Type_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 0, 3, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e2);
    test_assert(ctx.entities[2] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 20);
    test_assertint(ctx.column[0][2], 30);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset_limit_skip_table(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type_h);
    EcsEntity e5 = ecs_new(world, Type_h);
    EcsEntity e6 = ecs_new(world, Type_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 3, 2, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 40);
    test_assertint(ctx.column[0][1], 50);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset_skip_table(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, Type, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type_h);
    EcsEntity e5 = ecs_new(world, Type_h);
    EcsEntity e6 = ecs_new(world, Type_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 3, 0, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assert(ctx.entities[2] == e6);
    test_assertint(ctx.column[0][0], 40);
    test_assertint(ctx.column[0][1], 50);
    test_assertint(ctx.column[0][2], 60);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_offset_limit_3_tables(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, Type1, Foo, Bar);
    ECS_TYPE(world, Type2, Foo, Bar, Hello);
    ECS_SYSTEM(world, TestSystem, EcsOnDemand, Foo);

    EcsEntity e1 = ecs_new(world, Foo_h);
    EcsEntity e2 = ecs_new(world, Foo_h);
    EcsEntity e3 = ecs_new(world, Foo_h);
    EcsEntity e4 = ecs_new(world, Type1_h);
    EcsEntity e5 = ecs_new(world, Type1_h);
    EcsEntity e6 = ecs_new(world, Type1_h);
    EcsEntity e7 = ecs_new(world, Type2_h);
    EcsEntity e8 = ecs_new(world, Type2_h);
    EcsEntity e9 = ecs_new(world, Type2_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);
    test_assert(e8 != 0);
    test_assert(e9 != 0);

    ecs_set(world, e1, Foo, {10});
    ecs_set(world, e2, Foo, {20});
    ecs_set(world, e3, Foo, {30});
    ecs_set(world, e4, Foo, {40});
    ecs_set(world, e5, Foo, {50});
    ecs_set(world, e6, Foo, {60});
    ecs_set(world, e7, Foo, {70});
    ecs_set(world, e8, Foo, {80});
    ecs_set(world, e9, Foo, {90});

    ecs_set_context(world, &ctx);

    ecs_run_w_filter(world, TestSystem_h, 1.0, 2, 6, 0, NULL);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 6);
    test_assert(ctx.entities[0] == e3);
    test_assert(ctx.entities[1] == e4);
    test_assert(ctx.entities[2] == e5);
    test_assert(ctx.entities[3] == e6);
    test_assert(ctx.entities[4] == e7);
    test_assert(ctx.entities[5] == e8);
    test_assertint(ctx.column[0][0], 30);
    test_assertint(ctx.column[0][1], 40);
    test_assertint(ctx.column[0][2], 50);
    test_assertint(ctx.column[0][3], 60);
    test_assertint(ctx.column[0][4], 70);
    test_assertint(ctx.column[0][5], 80);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[0][1], Foo_h);
    test_assertint(ctx.component[0][2], Foo_h);
    test_assertint(ctx.component[0][3], Foo_h);
    test_assertint(ctx.component[0][4], Foo_h);
    test_assertint(ctx.component[0][5], Foo_h);

    ecs_fini(world);
}

void test_EcsOnFrameSystem_tc_system_w_same_or_type_as_table(
    test_EcsOnFrameSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);

    ECS_SYSTEM(world, TestSystem, EcsOnFrame, Foo | Bar);

    EcsEntity e1 = ecs_new(world, 0);
    ecs_add(world, e1, Foo_h);
    ecs_add(world, e1, Bar_h);

    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assertint(ctx.count, 1);

    ecs_fini(world);
}

void HasBar(EcsRows *rows) {
    bool *has_bar = ecs_get_context(rows->world);
    *has_bar = ecs_data(rows, rows->first, 1) != NULL;
}

void test_EcsOnFrameSystem_tc_system_optional_from_prefab(
    test_EcsOnFrameSystem this)
{
    EcsWorld *world = ecs_init();
    bool has_bar = false;

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, Prefab, Bar);
    ECS_ENTITY(world, E1, Prefab, Foo);

    ECS_SYSTEM(world, HasBar, EcsOnFrame, Foo, ?Bar);

    ecs_set_context(world, &has_bar);

    ecs_progress(world, 0);

    test_assert(has_bar != false);

    ecs_fini(world);
}
