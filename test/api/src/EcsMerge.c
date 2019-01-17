/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

typedef struct Bar {
    int x;
} Bar;

typedef struct Hello {
    int x;
} Hello;

typedef struct Context {
    EcsEntity component;
    EcsEntity component2;
} Context;

void MergeAdd(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    EcsEntity Bar_h = ctx->component;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);
        ecs_stage_add(world, entity, ctx->component);
        ecs_commit(world, entity);
        ecs_set(world, entity, Bar, {foo->x * 2});
        foo->x += 2;
    }
}

void test_EcsMerge_tc_merge_add(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, MergeAdd, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {10});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 24);

    ecs_fini(world);
}

void MergeAdd2(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    EcsEntity Bar_h = ctx->component;
    EcsEntity Hello_h = ctx->component2;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);
        ecs_stage_add(world, entity, ctx->component);
        ecs_stage_add(world, entity, ctx->component2);
        ecs_commit(world, entity);
        ecs_set(world, entity, Bar, {foo->x * 2});
        ecs_set(world, entity, Hello, {foo->x * 3});
        foo->x += 2;
    }
}

void test_EcsMerge_tc_merge_add_2(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_SYSTEM(world, MergeAdd2, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {10});

    Context ctx = {.component = Bar_h, .component2 = Hello_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assert(!ecs_has(world, e, Hello_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 20);
    test_assertint(ecs_get(world, e, Hello).x, 30);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 20);
    test_assertint(ecs_get(world, e, Hello).x, 30);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 24);
    test_assertint(ecs_get(world, e, Hello).x, 36);

    ecs_fini(world);
}

void test_EcsMerge_tc_merge_add_existing(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, FooBar, Foo, Bar);
    ECS_SYSTEM(world, MergeAdd, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, FooBar_h);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_set(world, e, Foo, {10});
    ecs_set(world, e, Bar, {5});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 5);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 24);

    ecs_fini(world);
}

void MergeAddRemove(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);
        ecs_stage_add(world, entity, ctx->component);
        ecs_stage_remove(world, entity, ctx->component);
        ecs_commit(world, entity);
        foo->x += 2;
    }
}

void test_EcsMerge_tc_merge_add_remove(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, MergeAddRemove, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {10});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);

    ecs_fini(world);
}

void MergeRemove(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        ecs_stage_remove(world, entity, ctx->component);
        ecs_commit(world, entity);
    }
}

void test_EcsMerge_tc_merge_remove(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, FooBar, Foo, Bar);
    ECS_SYSTEM(world, MergeRemove, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, FooBar_h);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_set(world, e, Foo, {10});
    ecs_set(world, e, Bar, {20});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 10);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 10);

    ecs_fini(world);
}

void MergeRemove2(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        ecs_stage_remove(world, entity, ctx->component);
        ecs_stage_remove(world, entity, ctx->component2);
        ecs_commit(world, entity);
    }
}

void test_EcsMerge_tc_merge_remove_2(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, FooBar, Foo, Bar);
    ECS_SYSTEM(world, MergeRemove2, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, FooBar_h);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_set(world, e, Foo, {10});
    ecs_set(world, e, Bar, {20});

    Context ctx = {.component = Foo_h, .component2 = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 10);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_merge(world);

    test_assert(!ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_fini(world);
}

void MergeAddN(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    EcsEntity Bar_h = ctx->component;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);

        if (!ecs_has(world, entity, Bar_h)) {
            ecs_stage_add(world, entity, Bar_h);
            ecs_commit(world, entity);
            ecs_set(world, entity, Bar, {0});
        }

        ecs_set(world, entity, Bar, {ecs_get(world, entity, Bar).x + 1});
        foo->x += 1;
    }
}

void test_EcsMerge_tc_merge_n_add(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, MergeAddN, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {1});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo).x, 5);
    test_assertint(ecs_get(world, e, Bar).x, 4);

    ecs_fini(world);
}

void MergeAddRemoveN(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    EcsEntity Bar_h = ctx->component;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);

        if (!ecs_has(world, entity, Bar_h)) {
            ecs_stage_add(world, entity, Bar_h);
            foo->x += 1;
        } else {
            ecs_stage_remove(world, entity, Bar_h);
        }

        ecs_commit(world, entity);
    }
}

void test_EcsMerge_tc_merge_n_add_remove(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, MergeAddRemoveN, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {1});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo).x, 3);

    ecs_progress(world, 0);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo).x, 4);

    ecs_merge(world);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assertint(ecs_get(world, e, Foo).x, 4);

    ecs_fini(world);
}

void MergeSet(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    EcsEntity Bar_h = ctx->component;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);
        ecs_set(world, entity, Bar, {foo->x * 2});
        foo->x += 2;
    }
}

void test_EcsMerge_tc_merge_set(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_SYSTEM(world, MergeSet, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {10});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 24);

    ecs_fini(world);
}

void MergeSet2(EcsRows *rows) {
    EcsWorld *world = rows->world;
    Context *ctx = ecs_get_context(world);
    EcsEntity Bar_h = ctx->component;
    EcsEntity Hello_h = ctx->component2;
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Foo *foo = ecs_data(rows, row, 0);
        ecs_set(world, entity, Bar, {foo->x * 2});
        ecs_set(world, entity, Hello, {foo->x * 3});
        foo->x += 2;
    }
}

void test_EcsMerge_tc_merge_set_2(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_COMPONENT(world, Hello);
    ECS_SYSTEM(world, MergeAdd2, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, Foo_h);
    test_assert(ecs_has(world, e, Foo_h));

    ecs_set(world, e, Foo, {10});

    Context ctx = {.component = Bar_h, .component2 = Hello_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(!ecs_has(world, e, Bar_h));
    test_assert(!ecs_has(world, e, Hello_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 20);
    test_assertint(ecs_get(world, e, Hello).x, 30);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 20);
    test_assertint(ecs_get(world, e, Hello).x, 30);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));
    test_assert(ecs_has(world, e, Hello_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 24);
    test_assertint(ecs_get(world, e, Hello).x, 36);

    ecs_fini(world);
}

void test_EcsMerge_tc_merge_set_existing(
    test_EcsMerge this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, FooBar, Foo, Bar);
    ECS_SYSTEM(world, MergeSet, EcsOnFrame, Foo);

    EcsEntity e = ecs_new(world, FooBar_h);
    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    ecs_set(world, e, Foo, {10});
    ecs_set(world, e, Bar, {5});

    Context ctx = {.component = Bar_h};
    ecs_set_context(world, &ctx);

    ecs_set_automerge(world, false);
    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 5);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 12);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_progress(world, 0);

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 20);

    ecs_merge(world);

    test_assert(ecs_has(world, e, Foo_h));
    test_assert(ecs_has(world, e, Bar_h));

    test_assertint(ecs_get(world, e, Foo).x, 14);
    test_assertint(ecs_get(world, e, Bar).x, 24);

    ecs_fini(world);
}
