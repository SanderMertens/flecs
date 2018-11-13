/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

#define COLUMN_COUNT (8)
#define COUNT (8)

typedef int Foo;

typedef int Bar;

typedef struct Context {
    int count;
    EcsHandle entities[COUNT];
    int column[COLUMN_COUNT][COUNT];
    EcsHandle component[COLUMN_COUNT][COUNT];
    int column_count;
} Context;

static
void TestInit(EcsRows *rows) {
    void *row;
    Context *ctx = ecs_get_context(rows->world);
    int column;

    ctx->column_count = rows->column_count;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ctx->entities[ctx->count] = ecs_entity(row);

        for (column = 0; column < rows->column_count; column ++) {
            int* ptr = ecs_column(rows, row, column);

            if (ptr) {
                ctx->column[column][ctx->count] = *ptr;
                ctx->component[column][ctx->count] = rows->components[column];
            } else {
                ctx->column[column][ctx->count] = 0;
                ctx->component[column][ctx->count] = 0;
            }

            *ptr = 100;
        }

        ctx->count ++;
    }
}

void test_EcsInitSystem_tc_init_after_add(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnInit, Bar);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, 10);

    ecs_add(world, e, Bar_h);
    ecs_commit(world, e);

    test_assertint(ecs_get(world, e, Foo), 10);
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Bar), 100);
    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e);
    test_assert(ctx.component[0][0] == Bar_h);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_init_after_add_to_empty(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnInit, Bar);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Bar_h);
    ecs_commit(world, e);

    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Bar), 100);
    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e);
    test_assert(ctx.component[0][0] == Bar_h);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_init_after_new(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnInit, Foo);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assertint(ecs_get(world, e, Foo), 100);
    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 1);
    test_assert(ctx.entities[0] == e);
    test_assert(ctx.component[0][0] == Foo_h);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_init_after_new_w_count(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnInit, Foo);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle handles[4] = {0};
    ecs_new_w_count(world, Foo_h, 3, handles);
    test_assert(handles[0] != 0);
    test_assert(handles[1] != 0);
    test_assert(handles[2] != 0);
    test_assertint(handles[3], 0);

    test_assert(ecs_has(world, handles[0], Foo_h));
    test_assert(ecs_has(world, handles[1], Foo_h));
    test_assert(ecs_has(world, handles[2], Foo_h));
    test_assertint(ecs_get(world, handles[0], Foo), 100);
    test_assertint(ecs_get(world, handles[1], Foo), 100);
    test_assertint(ecs_get(world, handles[2], Foo), 100);

    test_assertint(ctx.column_count, 1);
    test_assertint(ctx.count, 3);
    test_assert(ctx.entities[0] == handles[0]);
    test_assert(ctx.entities[1] == handles[1]);
    test_assert(ctx.entities[2] == handles[2]);
    test_assert(ctx.component[0][0] == Foo_h);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_deinit_after_delete(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnDeinit, Foo);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assertint(ctx.count, 0);
    ecs_delete(world, e);

    test_assertint(ctx.count, 1);
    test_assertint(ctx.column_count, 1);
    test_assert(ctx.entities[0] == e);
    test_assert(ctx.component[0][0] == Foo_h);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_deinit_after_fini(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnDeinit, Foo);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assertint(ctx.count, 0);
    ecs_fini(world);

    test_assertint(ctx.count, 1);
    test_assertint(ctx.column_count, 1);
    test_assert(ctx.entities[0] == e);
    test_assert(ctx.component[0][0] == Foo_h);
}

void test_EcsInitSystem_tc_deinit_after_remove(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, TestInit, EcsOnDeinit, Foo);

    Context ctx = {0};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assertint(ctx.count, 0);
    ecs_remove(world, e, Foo_h);
    ecs_commit(world, e);

    test_assertint(ctx.count, 1);
    test_assertint(ctx.column_count, 1);
    test_assert(ctx.entities[0] == e);
    test_assert(ctx.component[0][0] == Foo_h);

    ecs_fini(world);
}

typedef struct HandleCtx {
    EcsHandle h;
} HandleCtx;

static
void AddInInit(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    EcsHandle int_h = ctx->h;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        ecs_add(world, entity, int_h);
        ecs_commit(world, entity);
        ecs_set(world, entity, int, 10);
    }
}

void test_EcsInitSystem_tc_add_in_init(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddInInit, EcsOnInit, Foo);

    HandleCtx ctx = {.h = Bar_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Bar), 10);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_add_in_init_from_prefab(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_PREFAB(world, MyPrefab, Bar);
    ECS_FAMILY(world, MyFamily, Foo, MyPrefab);
    ECS_SYSTEM(world, AddInInit, EcsOnInit, Foo);

    ecs_set(world, MyPrefab_h, Bar, 20);

    HandleCtx ctx = {.h = Bar_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, MyPrefab_h, Bar), 20);
    test_assertint(ecs_get(world, e, Bar), 20);

    ecs_fini(world);
}

void test_EcsInitSystem_tc_new_in_init(
    test_EcsInitSystem this)
{
    /* Insert implementation */
}

void test_EcsInitSystem_tc_remove_in_init(
    test_EcsInitSystem this)
{
    /* Insert implementation */
}

void test_EcsInitSystem_tc_init_after_new_in_progress(
    test_EcsInitSystem this)
{
    /* Insert implementation */
}

void test_EcsInitSystem_tc_init_after_add_in_progress(
    test_EcsInitSystem this)
{
    /* Insert implementation */
}

void test_EcsInitSystem_tc_deinit_after_delete_in_progress(
    test_EcsInitSystem this)
{
    /* Insert implementation */
}

void test_EcsInitSystem_tc_deinit_after_remove_in_progress(
    test_EcsInitSystem this)
{
    /* Insert implementation */
}
