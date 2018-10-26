/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

void test_EcsDelete_tc_delete_1st_of_3(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    ecs_delete(world, e1);

    test_assert(ecs_valid(world, e1) == false);
    test_assert(ecs_valid(world, e2) == true);
    test_assert(ecs_valid(world, e3) == true);

    test_assert(ecs_get(world, e1, Foo_h) == NULL);
    test_assert(ecs_get(world, e2, Foo_h) != NULL);
    test_assert(ecs_get(world, e3, Foo_h) != NULL);

    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 20);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 30);

    ecs_fini(world);
}

void test_EcsDelete_tc_delete_2nd_of_3(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    ecs_delete(world, e2);

    test_assert(ecs_valid(world, e1) == true);
    test_assert(ecs_valid(world, e2) == false);
    test_assert(ecs_valid(world, e3) == true);

    test_assert(ecs_get(world, e1, Foo_h) != NULL);
    test_assert(ecs_get(world, e2, Foo_h) == NULL);
    test_assert(ecs_get(world, e3, Foo_h) != NULL);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 10);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 30);

    ecs_fini(world);
}

void test_EcsDelete_tc_delete_3rd_of_3(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    ecs_delete(world, e3);

    test_assert(ecs_valid(world, e1) == true);
    test_assert(ecs_valid(world, e2) == true);
    test_assert(ecs_valid(world, e3) == false);

    test_assert(ecs_get(world, e1, Foo_h) != NULL);
    test_assert(ecs_get(world, e2, Foo_h) != NULL);
    test_assert(ecs_get(world, e3, Foo_h) == NULL);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 10);
    test_assertint(*(int*)ecs_get(world, e2, Foo_h), 20);

    ecs_fini(world);
}

void test_EcsDelete_tc_delete_2_of_3(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    ecs_delete(world, e1);
    ecs_delete(world, e2);

    test_assert(ecs_valid(world, e1) == false);
    test_assert(ecs_valid(world, e2) == false);
    test_assert(ecs_valid(world, e3) == true);

    test_assert(ecs_get(world, e1, Foo_h) == NULL);
    test_assert(ecs_get(world, e2, Foo_h) == NULL);
    test_assert(ecs_get(world, e3, Foo_h) != NULL);

    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 30);

    ecs_fini(world);
}

void test_EcsDelete_tc_delete_3_of_3(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    test_assert(ecs_valid(world, e1) == false);
    test_assert(ecs_valid(world, e2) == false);
    test_assert(ecs_valid(world, e3) == false);

    test_assert(ecs_get(world, e1, Foo_h) == NULL);
    test_assert(ecs_get(world, e2, Foo_h) == NULL);
    test_assert(ecs_get(world, e3, Foo_h) == NULL);

    ecs_fini(world);
}

typedef struct Context {
    EcsHandle entity;
    uint32_t count;
} Context;

void DeleteNext(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_column(rows, row, 0)) ++;
        if (ecs_entity(row) - 1 == ctx->entity) {
            ecs_delete(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

void DeletePrev(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_column(rows, row, 0)) ++;
        if (ecs_entity(row) + 1 == ctx->entity) {
            ecs_delete(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

void DeleteCurrent(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_column(rows, row, 0)) ++;
        if (ecs_entity(row) == ctx->entity) {
            ecs_delete(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

void test_EcsDelete_tc_delete_cur_in_progress(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, DeleteCurrent, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.count, 3);

    test_assert(ecs_valid(world, e1) == true);
    test_assert(ecs_valid(world, e2) == false);
    test_assert(ecs_valid(world, e3) == true);

    test_assert(ecs_get(world, e1, Foo_h) != NULL);
    test_assert(ecs_get(world, e2, Foo_h) == NULL);
    test_assert(ecs_get(world, e3, Foo_h) != NULL);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    ecs_fini(world);
}

void test_EcsDelete_tc_delete_next_in_progress(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, DeleteNext, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.count, 3);

    test_assert(ecs_valid(world, e1) == true);
    test_assert(ecs_valid(world, e2) == false);
    test_assert(ecs_valid(world, e3) == true);

    test_assert(ecs_get(world, e1, Foo_h) != NULL);
    test_assert(ecs_get(world, e2, Foo_h) == NULL);
    test_assert(ecs_get(world, e3, Foo_h) != NULL);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    ecs_fini(world);
}

void test_EcsDelete_tc_delete_prev_in_progress(
    test_EcsDelete this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, DeletePrev, EcsPeriodic, Foo);

    EcsHandle e1 = ecs_new(world, Foo_h);
    EcsHandle e2 = ecs_new(world, Foo_h);
    EcsHandle e3 = ecs_new(world, Foo_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get(world, e1, Foo_h) = 10;
    *(int*)ecs_get(world, e2, Foo_h) = 20;
    *(int*)ecs_get(world, e3, Foo_h) = 30;

    Context ctx = {.entity = e2};
    ecs_set_context(world, &ctx);

    ecs_progress(world);

    test_assertint(ctx.count, 3);

    test_assert(ecs_valid(world, e1) == true);
    test_assert(ecs_valid(world, e2) == false);
    test_assert(ecs_valid(world, e3) == true);

    test_assert(ecs_get(world, e1, Foo_h) != NULL);
    test_assert(ecs_get(world, e2, Foo_h) == NULL);
    test_assert(ecs_get(world, e3, Foo_h) != NULL);

    test_assertint(*(int*)ecs_get(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get(world, e3, Foo_h), 31);

    ecs_fini(world);
}
