/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

typedef struct Bar {
    int y;
} Bar;

typedef struct Hello {
    int a;
} Hello;

void test_EcsRemove_tc_remove_2_component(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    EcsEntity e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_stage_remove(world, e, Foo_h);
    ecs_stage_remove(world, e, Bar_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    ecs_fini(world);
}

void test_EcsRemove_tc_remove_component(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_TYPE(world, MyFamily, Foo);
    EcsEntity e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    ecs_stage_remove(world, e, Foo_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    ecs_fini(world);
}

void test_EcsRemove_tc_remove_2_component_from_3(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_TYPE(world, MyFamily, Foo, Bar, Hello);
    EcsEntity e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));
    ecs_stage_remove(world, e, Foo_h);
    ecs_stage_remove(world, e, Bar_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));
    ecs_fini(world);
}

void test_EcsRemove_tc_remove_component_from_2(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    EcsEntity e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_stage_remove(world, e, Foo_h);
    ecs_commit(world, e);
    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    ecs_fini(world);
}

typedef struct Context {
    EcsEntity entity;
    EcsEntity component;
    EcsEntity component2;
    uint32_t count;
} Context;

static
void RemoveNext(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_data(rows, row, 0)) ++;
        (*(int*)ecs_data(rows, row, 1)) += 2;
        if (ecs_entity(rows, row, ECS_ROW_ENTITY) - 1 == ctx->entity) {
            ecs_stage_remove(rows->world, ctx->entity, ctx->component);
            ecs_commit(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

static
void RemovePrev(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_data(rows, row, 0)) ++;
        (*(int*)ecs_data(rows, row, 1)) += 2;
        if (ecs_entity(rows, row, ECS_ROW_ENTITY) + 1 == ctx->entity) {
            ecs_stage_remove(rows->world, ctx->entity, ctx->component);
            ecs_commit(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

static
void RemoveCurrent(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        (*(int*)ecs_data(rows, row, 0)) ++;
        (*(int*)ecs_data(rows, row, 1)) += 2;
        if (ecs_entity(rows, row, ECS_ROW_ENTITY) == ctx->entity) {
            ecs_stage_remove(rows->world, ctx->entity, ctx->component);
            ecs_commit(rows->world, ctx->entity);
        }

        ctx->count ++;
    }
}

static
void RemoveAll(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        ecs_stage_remove(rows->world, ecs_entity(rows, row, ECS_ROW_ENTITY), ctx->component);
        ecs_commit(rows->world, ecs_entity(rows, row, ECS_ROW_ENTITY));
        (*(int*)ecs_data(rows, row, 0)) ++;
        (*(int*)ecs_data(rows, row, 1)) += 2;
        ctx->count ++;
    }
}

void test_EcsRemove_tc_remove_prev_in_progress(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, RemovePrev, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyFamily_h);
    EcsEntity e2 = ecs_new(world, MyFamily_h);
    EcsEntity e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e1, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e3, Bar_h) = 60;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(!ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 51);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 62);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 5);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 24);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 52);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 64);

    ecs_fini(world);
}

void test_EcsRemove_tc_remove_cur_in_progress(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, RemoveCurrent, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyFamily_h);
    EcsEntity e2 = ecs_new(world, MyFamily_h);
    EcsEntity e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e1, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e3, Bar_h) = 60;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(!ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 51);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 62);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 5);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 24);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 52);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 64);

    ecs_fini(world);
}

void test_EcsRemove_tc_remove_next_in_progress(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, RemoveNext, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyFamily_h);
    EcsEntity e2 = ecs_new(world, MyFamily_h);
    EcsEntity e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e1, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e3, Bar_h) = 60;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(!ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 22);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 51);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 62);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 5);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 24);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 52);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 64);

    ecs_fini(world);
}

void test_EcsRemove_tc_remove_all_in_progress(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, RemoveAll, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyFamily_h);
    EcsEntity e2 = ecs_new(world, MyFamily_h);
    EcsEntity e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e1, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e3, Bar_h) = 60;

    Context ctx = {.entity = e2, .component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(!ecs_has(world, e1, Bar_h));
    test_assert(!ecs_has(world, e2, Bar_h));
    test_assert(!ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 51);

    test_assertint(ctx.count, 3);

    ecs_progress(world, 0);
    test_assertint(ctx.count, 3);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 51);

    ecs_fini(world);
}

static
void RemoveAdd(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsEntity Bar_h = ctx->component;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        (*(int*)ecs_data(rows, row, 0)) ++;
        (*(int*)ecs_data(rows, row, 1)) += 2;
        ecs_stage_add(rows->world, entity, ctx->component);
        ecs_stage_remove(rows->world, entity, ctx->component);
        ecs_commit(rows->world, entity);
        ecs_set(rows->world, entity, Bar, {1});
        ctx->count ++;
    }
}

void test_EcsRemove_tc_remove_add_in_progress(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, RemoveAdd, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyFamily_h);
    EcsEntity e2 = ecs_new(world, MyFamily_h);
    EcsEntity e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e1, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e3, Bar_h) = 60;

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e1, Foo_h));
    test_assert(ecs_has(world, e2, Foo_h));
    test_assert(ecs_has(world, e3, Foo_h));
    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 11);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 31);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 51);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 1);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e1, Foo_h));
    test_assert(ecs_has(world, e2, Foo_h));
    test_assert(ecs_has(world, e3, Foo_h));
    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Foo_h), 12);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Foo_h), 32);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Foo_h), 52);

    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 1);
}

static
void Remove2Add(EcsRows *rows) {
    Context *ctx = ecs_get_context(rows->world);
    EcsEntity Bar_h = ctx->component;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        (*(int*)ecs_data(rows, row, 0)) ++;
        (*(int*)ecs_data(rows, row, 1)) += 2;
        ecs_stage_add(rows->world, entity, ctx->component);
        ecs_stage_remove(rows->world, entity, ctx->component);
        ecs_stage_remove(rows->world, entity, ctx->component2);
        ecs_commit(rows->world, entity);
        ecs_set(rows->world, entity, Bar, {1});
        ctx->count ++;
    }
}

void test_EcsRemove_tc_remove_2_add_in_progress(
    test_EcsRemove this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_TYPE(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, Remove2Add, EcsOnFrame, Foo, Bar);

    EcsEntity e1 = ecs_new(world, MyFamily_h);
    EcsEntity e2 = ecs_new(world, MyFamily_h);
    EcsEntity e3 = ecs_new(world, MyFamily_h);

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);

    *(int*)ecs_get_ptr(world, e1, Foo_h) = 10;
    *(int*)ecs_get_ptr(world, e1, Bar_h) = 20;
    *(int*)ecs_get_ptr(world, e2, Foo_h) = 30;
    *(int*)ecs_get_ptr(world, e2, Bar_h) = 40;
    *(int*)ecs_get_ptr(world, e3, Foo_h) = 50;
    *(int*)ecs_get_ptr(world, e3, Bar_h) = 60;

    test_assert(ecs_has(world, e1, Foo_h));
    test_assert(ecs_has(world, e2, Foo_h));
    test_assert(ecs_has(world, e3, Foo_h));
    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    Context ctx = {.component = Bar_h, .component2 = Foo_h};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_assert(!ecs_has(world, e1, Foo_h));
    test_assert(!ecs_has(world, e2, Foo_h));
    test_assert(!ecs_has(world, e3, Foo_h));
    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 1);

    ecs_progress(world, 0);

    test_assert(!ecs_has(world, e1, Foo_h));
    test_assert(!ecs_has(world, e2, Foo_h));
    test_assert(!ecs_has(world, e3, Foo_h));
    test_assert(ecs_has(world, e1, Bar_h));
    test_assert(ecs_has(world, e2, Bar_h));
    test_assert(ecs_has(world, e3, Bar_h));

    test_assertint(*(int*)ecs_get_ptr(world, e1, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e2, Bar_h), 1);
    test_assertint(*(int*)ecs_get_ptr(world, e3, Bar_h), 1);
}
