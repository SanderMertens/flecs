/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

#define COLUMN_COUNT (8)
#define COUNT (8)

typedef int Foo;
typedef int Bar;
typedef int Hello;
typedef int World;

typedef struct Context {
    int count;
    EcsHandle entities[COUNT];
    int column[COLUMN_COUNT][COUNT];
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

    ecs_fini(world);
}


void test_EcsPeriodicSystem_tc_system_prefab(
    test_EcsPeriodicSystem this)
{

}


void test_EcsPeriodicSystem_tc_system_prefab_component(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}

void test_EcsPeriodicSystem_tc_system_1_component_1_not(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}


void test_EcsPeriodicSystem_tc_system_1_component_1_or2(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}


void test_EcsPeriodicSystem_tc_system_1_component_2_not(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}


void test_EcsPeriodicSystem_tc_system_1_component_2_or2(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}


void test_EcsPeriodicSystem_tc_system_2_component_1_not(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}


void test_EcsPeriodicSystem_tc_system_2_component_2_not(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}


void test_EcsPeriodicSystem_tc_system_2_component_2_or3(
    test_EcsPeriodicSystem this)
{
    /* Insert implementation */
}
