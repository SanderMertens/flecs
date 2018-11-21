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
    ECS_SYSTEM(world, TestInit, EcsOnAdd, Bar);

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
    ECS_SYSTEM(world, TestInit, EcsOnAdd, Bar);

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
    ECS_SYSTEM(world, TestInit, EcsOnAdd, Foo);

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
    ECS_SYSTEM(world, TestInit, EcsOnAdd, Foo);

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
    ECS_SYSTEM(world, TestInit, EcsOnRemove, Foo);

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
    ECS_SYSTEM(world, TestInit, EcsOnRemove, Foo);

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
    ECS_SYSTEM(world, TestInit, EcsOnRemove, Foo);

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
    ECS_SYSTEM(world, AddInInit, EcsOnAdd, Foo);

    HandleCtx ctx = {.h = Bar_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Bar), 10);

    ecs_fini(world);
}

static
void AddInInitPrefab(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    EcsHandle int_h = ctx->h;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        ecs_add(world, entity, int_h);
        ecs_commit(world, entity);
        int prefab_value = ecs_get(world, entity, int);
        ecs_set(world, entity, int, prefab_value + 10);
    }
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
    ECS_SYSTEM(world, AddInInitPrefab, EcsOnAdd, Foo);

    ecs_set(world, MyPrefab_h, Bar, 20);

    HandleCtx ctx = {.h = Bar_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, MyPrefab_h, Bar), 20);
    test_assertint(ecs_get(world, e, Bar), 30);

    ecs_fini(world);
}

typedef struct Node {
    int value;
    EcsHandle entity;
} Node;

static
void NewNode(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    EcsHandle int_h = ctx->h;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Node *node = ecs_column(rows, row, 0);
        node->value = 10;
        node->entity = ecs_new(world, int_h);
        ecs_set(world, node->entity, int, 20);
    }
}

void test_EcsInitSystem_tc_new_in_init(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Node);
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, NewNode, EcsOnAdd, Node);

    HandleCtx ctx = {.h = Foo_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Node_h);
    test_assert(e != 0);

    EcsHandle new_entity = ecs_get(world, e, Node).entity;

    test_assert(new_entity != 0);
    test_assert(ecs_has(world, new_entity, Foo_h));
    test_assertint(ecs_get(world, new_entity, Foo), 20);

    ecs_fini(world);
}

static
void RemoveInInit(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    EcsHandle int_h = ctx->h;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        int *v1 = ecs_column(rows, row, 0);
        int *v2 = ecs_column(rows, row, 1);
        ecs_remove(world, entity, int_h);
        ecs_commit(world, entity);
        (*v1) = 10;
        (*v2) = 20;
    }
}

void test_EcsInitSystem_tc_remove_in_init(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, RemoveInInit, EcsOnAdd, Foo, Bar);

    HandleCtx ctx = {.h = Bar_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo), 10);

    ecs_fini(world);
}

static
void InitTest(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        int *v = ecs_column(rows, row, 0);
        *v = 100;
    }
}

static
void NewInProgress(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    EcsHandle int_h = ctx->h;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Node *node = ecs_column(rows, row, 0);
        node->value = 10;
        node->entity = ecs_new(world, int_h);
    }
}

void test_EcsInitSystem_tc_init_after_new_in_progress(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Node);
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, NewInProgress, EcsOnFrame, Node);
    ECS_SYSTEM(world, InitTest, EcsOnAdd, Foo);

    HandleCtx ctx = {.h = Foo_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Node_h);
    test_assert(e != 0);

    ecs_set(world, e, Node, {0, 0});

    ecs_progress(world, 0);

    EcsHandle new_e = ecs_get(world, e, Node).entity;
    test_assertint(ecs_get(world, e, Node).value, 10);
    test_assert(new_e != 0);
    test_assert(ecs_has(world, new_e, Foo_h));
    test_assertint(ecs_get(world, new_e, Foo), 100);

    ecs_fini(world);
}

static
void AddInProgress(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    EcsHandle int_h = ctx->h;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        int *v = ecs_column(rows, row, 0);
        ecs_add(world, entity, int_h);
        ecs_commit(world, entity);
        (*v) ++;
    }
}

void test_EcsInitSystem_tc_init_after_add_in_progress(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, AddInProgress, EcsOnFrame, Foo);
    ECS_SYSTEM(world, InitTest, EcsOnAdd, Bar);

    HandleCtx ctx = {.h = Bar_h};
    ecs_set_context(world, &ctx);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);
    ecs_set(world, e, Foo, 10);
    test_assertint(ecs_get(world, e, Foo), 10);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo), 11);
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Bar), 100);

    ecs_fini(world);
}

typedef int *IntPtr;

static
void DeinitTest(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        int **v = ecs_column(rows, row, 0);
        (**v) --;
    }
}

static
void DeleteInProgress(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Node *node = ecs_column(rows, row, 0);
        node->value = 10;
        ecs_delete(world, node->entity);
    }
}

void test_EcsInitSystem_tc_deinit_after_delete_in_progress(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Node);
    ECS_COMPONENT(world, IntPtr);
    ECS_SYSTEM(world, DeleteInProgress, EcsOnFrame, Node);
    ECS_SYSTEM(world, DeinitTest, EcsOnRemove, IntPtr);

    EcsHandle e = ecs_new(world, Node_h);
    test_assert(e != 0);

    EcsHandle e2 = ecs_new(world, IntPtr_h);
    test_assert(e2 != 0);

    int counter = 1;
    ecs_set(world, e2, IntPtr, &counter);
    ecs_set(world, e, Node, {.value = 0, .entity = e2});

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Node).value, 10);
    test_assertint(counter, 0);

    ecs_fini(world);
}

static
void RemoveInProgress(EcsRows *rows) {
    void *row;
    EcsWorld *world = rows->world;
    HandleCtx *ctx = ecs_get_context(world);
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        int *v = ecs_column(rows, row, 0);
        ecs_remove(world, entity, ctx->h);
        ecs_commit(world, entity);
        (*v) ++;
    }
}

void test_EcsInitSystem_tc_deinit_after_remove_in_progress(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, IntPtr);
    ECS_FAMILY(world, MyFamily, Foo, IntPtr);
    ECS_SYSTEM(world, RemoveInProgress, EcsOnFrame, Foo, IntPtr);
    ECS_SYSTEM(world, DeinitTest, EcsOnRemove, IntPtr);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    int counter = 1;
    ecs_set(world, e, IntPtr, &counter);
    ecs_set(world, e, Foo, 10);

    HandleCtx ctx = {.h = IntPtr_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, IntPtr_h));
    test_assertint(ecs_get(world, e, Foo), 11);
    test_assertint(counter, 0);

    ecs_fini(world);
}

static
void InitTwo(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Bar *b = ecs_column(rows, row, 0);
        Foo *f = ecs_column(rows, row, 1);
        *b = 20;
        *f = 10;
    }
}

void test_EcsInitSystem_tc_init_2_components(
    test_EcsInitSystem this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, InitTwo, EcsOnAdd, Bar, Foo);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo), 10);
    test_assertint(ecs_get(world, e, Bar), 20);

    ecs_fini(world);
}
