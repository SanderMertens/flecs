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
    EcsHandle entities[COUNT];
    int column[COLUMN_COUNT][COUNT];
    EcsHandle component[COLUMN_COUNT][COUNT];
    int column_count;
} Context;

static
void TestSystem(EcsRows *rows) {
    void *row;
    Context *ctx = ecs_get_context(rows->world);
    int column;

    ctx->column_count = rows->column_count;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ctx->entities[ctx->count] = ecs_entity(row);

        for (column = 0; column < rows->column_count; column ++) {
            ctx->column[column][ctx->count] = *(int*)ecs_column(rows, row, column);
            ctx->component[column][ctx->count] = rows->components[column];
        }

        ctx->count ++;
    }
}

void test_EcsPeriodicSystem_tc_system_component(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *bar_3 = ecs_get(world, e3, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *bar_3 = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e3);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 40);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void test_EcsPeriodicSystem_tc_system_2_component(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar, Hello);
    ECS_FAMILY(world, Family3, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Hello);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);
    EcsHandle e5 = ecs_new(world, Family3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *hello_4 = ecs_get(world, e4, Hello_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *bar_5 = ecs_get(world, e5, Bar_h);

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

    ecs_progress(world);

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


void test_EcsPeriodicSystem_tc_system_prefab(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Foo);
    ECS_FAMILY(world, MyFamily, MyPrefab, Bar);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, MyFamily_h);
    EcsHandle e4 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *bar_3 = ecs_get(world, e3, Bar_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_prefab = ecs_get(world, MyPrefab_h, Foo_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *bar_3 = 50;
    *bar_4 = 60;
    *foo_prefab = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

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


void test_EcsPeriodicSystem_tc_system_prefab_component(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_PREFAB(world, MyPrefab, Foo);
    ECS_FAMILY(world, MyFamily, MyPrefab, Bar);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bar);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, MyFamily_h);
    EcsHandle e4 = ecs_new(world, MyFamily_h);
    EcsHandle e5 = ecs_new(world, Hello_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *bar_3 = ecs_get(world, e3, Bar_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_prefab = ecs_get(world, MyPrefab_h, Foo_h);
    int *hello_5 = ecs_get(world, e5, Hello_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *bar_3 = 40;
    *bar_4 = 60;
    *foo_prefab = 70;
    *hello_5 = 80;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

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

void test_EcsPeriodicSystem_tc_system_1_component_1_not(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, !Hello);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 60);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void test_EcsPeriodicSystem_tc_system_1_component_2_not(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_FAMILY(world, Family3, Foo, World);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, !Hello, !Bar);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);
    EcsHandle e5 = ecs_new(world, Family3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *world_5 = ecs_get(world, e5, World_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *foo_5 = 80;
    *world_5 = 90;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e1);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 80);
    test_assertint(ctx.component[0][0], Foo_h);

    ecs_fini(world);
}

void test_EcsPeriodicSystem_tc_system_2_component_1_not(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_FAMILY(world, Family3, Foo, Bar, World);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bar, !Hello);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);
    EcsHandle e5 = ecs_new(world, Family3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *bar_5 = ecs_get(world, e5, Bar_h);
    int *world_5 = ecs_get(world, e5, World_h);

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

    ecs_progress(world);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e4);
    test_assert(ctx.entities[1] == e5);
    test_assertint(ctx.column[0][0], 60);
    test_assertint(ctx.column[1][0], 70);
    test_assertint(ctx.column[0][1], 80);
    test_assertint(ctx.column[1][1], 90);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);

    ecs_fini(world);
}


void test_EcsPeriodicSystem_tc_system_2_component_2_not(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_FAMILY(world, Family3, Foo, Bar, World);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bar, !Hello, !World);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);
    EcsHandle e5 = ecs_new(world, Family3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *bar_5 = ecs_get(world, e5, Bar_h);
    int *world_5 = ecs_get(world, e5, World_h);

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

    ecs_progress(world);

    test_assertint(ctx.column_count, 2);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e4);
    test_assertint(ctx.column[0][0], 60);
    test_assertint(ctx.column[1][0], 70);
    test_assertint(ctx.component[0][0], Foo_h);
    test_assertint(ctx.component[1][0], Bar_h);

    ecs_fini(world);
}

void test_EcsPeriodicSystem_tc_system_1_component_1_or2(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_FAMILY(world, Family3, Foo, World);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bar|Hello);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);
    EcsHandle e5 = ecs_new(world, Family3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *world_5 = ecs_get(world, e5, World_h);

    *foo_1 = 10;
    *bar_2 = 20;
    *foo_3 = 40;
    *hello_3 = 50;
    *foo_4 = 60;
    *bar_4 = 70;
    *foo_5 = 80;
    *world_5 = 100;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

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


void test_EcsPeriodicSystem_tc_system_1_component_2_or2(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_COMPONENT(world, Bleep);
    ECS_COMPONENT(world, Bloop);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bloop);
    ECS_FAMILY(world, Family3, Foo, Bleep, World);
    ECS_FAMILY(world, Family4, Foo, Hello, Bloop);
    ECS_FAMILY(world, Family5, Bloop, Hello);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bleep|Bloop, Hello|World);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Family1_h);
    EcsHandle e3 = ecs_new(world, Family2_h);
    EcsHandle e4 = ecs_new(world, Family3_h);
    EcsHandle e5 = ecs_new(world, Family4_h);
    EcsHandle e6 = ecs_new(world, Family5_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *foo_2 = ecs_get(world, e2, Foo_h);
    int *hello_2 = ecs_get(world, e2, Hello_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *bloop_3 = ecs_get(world, e3, Bloop_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bleep_4 = ecs_get(world, e4, Bleep_h);
    int *world_4 = ecs_get(world, e4, World_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *hello_5 = ecs_get(world, e5, Hello_h);
    int *bloop_5 = ecs_get(world, e5, Bloop_h);
    int *bloop_6 = ecs_get(world, e6, Bloop_h);
    int *hello_6 = ecs_get(world, e6, Hello_h);

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

    ecs_progress(world);

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

void test_EcsPeriodicSystem_tc_system_2_component_2_or3(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_COMPONENT(world, Bleep);
    ECS_COMPONENT(world, Bloop);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_FAMILY(world, Family3, Foo, Bar, World);
    ECS_FAMILY(world, Family4, Foo, Bar, Bleep);
    ECS_FAMILY(world, Family5, Foo, Bar, Hello);
    ECS_FAMILY(world, Family6, Foo, Bar, Bloop);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bar, Hello|World|Bleep);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Family1_h);
    EcsHandle e3 = ecs_new(world, Family2_h);
    EcsHandle e4 = ecs_new(world, Family3_h);
    EcsHandle e5 = ecs_new(world, Family4_h);
    EcsHandle e6 = ecs_new(world, Family5_h);
    EcsHandle e7 = ecs_new(world, Family6_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *foo_2 = ecs_get(world, e2, Foo_h);
    int *hello_2 = ecs_get(world, e2, Hello_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *bar_3 = ecs_get(world, e3, Bar_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *world_4 = ecs_get(world, e4, World_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *bar_5 = ecs_get(world, e5, Bar_h);
    int *bleep_5 = ecs_get(world, e5, Bleep_h);
    int *foo_6 = ecs_get(world, e6, Foo_h);
    int *bar_6 = ecs_get(world, e6, Bar_h);
    int *hello_6 = ecs_get(world, e6, Hello_h);
    int *foo_7 = ecs_get(world, e7, Foo_h);
    int *bar_7 = ecs_get(world, e7, Bar_h);
    int *bloop_7 = ecs_get(world, e7, Bloop_h);

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

    ecs_progress(world);

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

void test_EcsPeriodicSystem_tc_system_1_component_or2_both_match(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, Foo, Bar, Hello);
    ECS_FAMILY(world, Family2, Foo, Bar);
    ECS_FAMILY(world, Family3, Foo, Hello);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Foo, Bar|Hello);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Bar_h);
    EcsHandle e3 = ecs_new(world, Family1_h);
    EcsHandle e4 = ecs_new(world, Family2_h);
    EcsHandle e5 = ecs_new(world, Family3_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *bar_2 = ecs_get(world, e2, Bar_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *bar_3 = ecs_get(world, e3, Bar_h);
    int *hello_3 = ecs_get(world, e3, Hello_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bar_4 = ecs_get(world, e4, Bar_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *hello_5 = ecs_get(world, e5, Hello_h);

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

    ecs_progress(world);

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

void test_EcsPeriodicSystem_tc_system_1_or2(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, Foo);
    ECS_FAMILY(world, Family2, Foo, Hello);
    ECS_FAMILY(world, Family3, Foo, World);
    ECS_FAMILY(world, Family4, Hello);
    ECS_FAMILY(world, Family5, World);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Hello|World);

    EcsHandle e1 = ecs_new(world, Family1_h);
    EcsHandle e2 = ecs_new(world, Family2_h);
    EcsHandle e3 = ecs_new(world, Family3_h);
    EcsHandle e4 = ecs_new(world, Family4_h);
    EcsHandle e5 = ecs_new(world, Family5_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *foo_2 = ecs_get(world, e2, Foo_h);
    int *hello_2 = ecs_get(world, e2, Hello_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *world_3 = ecs_get(world, e3, World_h);
    int *hello_4 = ecs_get(world, e4, Hello_h);
    int *world_5 = ecs_get(world, e5, World_h);

    *foo_1 = 10;
    *foo_2 = 20;
    *hello_2 = 30;
    *foo_3 = 40;
    *world_3 = 50;
    *hello_4 = 60;
    *world_5 = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

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

void test_EcsPeriodicSystem_tc_system_2_or2(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_COMPONENT(world, Bleep);
    ECS_COMPONENT(world, Bloop);
    ECS_FAMILY(world, Family1, Foo, Hello);
    ECS_FAMILY(world, Family2, Foo, Bloop);
    ECS_FAMILY(world, Family3, Foo, Bleep, World);
    ECS_FAMILY(world, Family4, Foo, Hello, Bloop);
    ECS_FAMILY(world, Family5, Bloop, Hello);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, Bleep|Bloop, Hello|World);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Family1_h);
    EcsHandle e3 = ecs_new(world, Family2_h);
    EcsHandle e4 = ecs_new(world, Family3_h);
    EcsHandle e5 = ecs_new(world, Family4_h);
    EcsHandle e6 = ecs_new(world, Family5_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);

    int *foo_1 = ecs_get(world, e1, Foo_h);
    int *foo_2 = ecs_get(world, e2, Foo_h);
    int *hello_2 = ecs_get(world, e2, Hello_h);
    int *foo_3 = ecs_get(world, e3, Foo_h);
    int *bloop_3 = ecs_get(world, e3, Bloop_h);
    int *foo_4 = ecs_get(world, e4, Foo_h);
    int *bleep_4 = ecs_get(world, e4, Bleep_h);
    int *world_4 = ecs_get(world, e4, World_h);
    int *foo_5 = ecs_get(world, e5, Foo_h);
    int *hello_5 = ecs_get(world, e5, Hello_h);
    int *bloop_5 = ecs_get(world, e5, Bloop_h);
    int *bloop_6 = ecs_get(world, e6, Bloop_h);
    int *hello_6 = ecs_get(world, e6, Hello_h);

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

    ecs_progress(world);

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

void test_EcsPeriodicSystem_tc_system_1_from_component(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, Family1, EcsComponent, Bar);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, COMPONENT.Bar);

    EcsHandle e1 = ecs_new(world, Family1_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);
    EcsHandle e4 = ecs_new(world, Bar_h);
    EcsHandle e5 = ecs_new(world, Bar_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);

    ecs_add(world, e2, e1);
    ecs_commit(world, e2);
    ecs_add(world, e4, e1);
    ecs_commit(world, e4);

    *(int*)ecs_get(world, e1, Bar_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;
    *(int*)ecs_get(world, e4, Bar_h) = 40;
    *(int*)ecs_get(world, e5, Bar_h) = 50;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 2);
    test_assert(ctx.entities[0] == e2);
    test_assert(ctx.entities[1] == e4);
    test_assertint(ctx.column[0][0], 10);
    test_assertint(ctx.column[0][1], 10);
    test_assertint(ctx.component[0][0], Bar_h);
    test_assertint(ctx.component[0][1], Bar_h);

    ecs_fini(world);
}

void test_EcsPeriodicSystem_tc_system_2_from_component(
    test_EcsPeriodicSystem this)
{
    Context ctx = {0};
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_COMPONENT(world, World);
    ECS_FAMILY(world, Family1, EcsComponent, Bar);
    ECS_FAMILY(world, Family2, EcsComponent, World);
    ECS_SYSTEM(world, TestSystem, EcsPeriodic, COMPONENT.Bar, COMPONENT.World);

    EcsHandle e1 = ecs_new(world, Family1_h);
    EcsHandle e2 = ecs_new(world, Family2_h);
    EcsHandle e3 = ecs_new(world, Foo_h);
    EcsHandle e4 = ecs_new(world, Bar_h);
    EcsHandle e5 = ecs_new(world, Bar_h);
    EcsHandle e6 = ecs_new(world, World_h);
    EcsHandle e7 = ecs_new(world, World_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);
    test_assert(e5 != 0);
    test_assert(e6 != 0);
    test_assert(e7 != 0);

    ecs_add(world, e3, e1);
    ecs_add(world, e3, e2);
    ecs_commit(world, e3);

    ecs_add(world, e4, e1);
    ecs_add(world, e4, e2);
    ecs_commit(world, e4);

    ecs_add(world, e5, e1);
    ecs_commit(world, e5);

    ecs_add(world, e6, e1);
    ecs_add(world, e6, e2);
    ecs_commit(world, e6);

    ecs_add(world, e7, e2);
    ecs_commit(world, e7);

    *(int*)ecs_get(world, e1, Bar_h) = 10;
    *(int*)ecs_get(world, e2, World_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;
    *(int*)ecs_get(world, e4, Bar_h) = 40;
    *(int*)ecs_get(world, e5, Bar_h) = 50;
    *(int*)ecs_get(world, e6, World_h) = 60;
    *(int*)ecs_get(world, e7, World_h) = 70;

    ecs_set_context(world, &ctx);

    ecs_progress(world);

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

    ecs_fini(world);
}

void test_EcsPeriodicSystem_tc_system_1_from_component_or2(
    test_EcsPeriodicSystem this)
{

}

void test_EcsPeriodicSystem_tc_system_2_from_component_or2(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}
