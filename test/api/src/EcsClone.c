/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef int Foo;
typedef int Bar;

void test_EcsClone_tc_clone_1_component(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    EcsHandle clone = ecs_clone(world, e, false);
    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));

    ecs_fini(world);
}

void CloneOneInProgress(EcsRows *rows) {
    void *row;
    EcsHandle *ctx = ecs_get_context(rows->world);
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        EcsHandle clone = ecs_clone(rows->world, entity, false);
        *ctx = clone;
    }
}

void test_EcsClone_tc_clone_1_component_in_progress(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, CloneOneInProgress, EcsOnFrame, Foo);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    EcsHandle clone = 0;
    ecs_set_context(world, &clone);

    ecs_progress(world, 0);

    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));

    ecs_fini(world);
}


void test_EcsClone_tc_clone_1_component_value(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});
    test_assertint(ecs_get(world, e, Foo), 10);

    EcsHandle clone = ecs_clone(world, e, true);
    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assertint(ecs_get(world, clone, Foo), 10);

    ecs_fini(world);
}

void CloneOneValueInProgress(EcsRows *rows) {
    void *row;
    EcsHandle *ctx = ecs_get_context(rows->world);
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        EcsHandle clone = ecs_clone(rows->world, entity, true);
        *ctx = clone;
    }
}

void test_EcsClone_tc_clone_1_component_value_in_progress(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, CloneOneValueInProgress, EcsOnFrame, Foo);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});
    test_assertint(ecs_get(world, e, Foo), 10);

    EcsHandle clone = 0;
    ecs_set_context(world, &clone);

    ecs_progress(world, 0);

    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assertint(ecs_get(world, clone, Foo), 10);

    ecs_fini(world);
}

void CloneOneValueOverrideInProgress(EcsRows *rows) {
    EcsHandle *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        EcsHandle Foo_h = rows->components[0];
        Foo *foo = ecs_column(rows, row, 0);

        EcsHandle clone = ecs_clone(rows->world, entity, true);
        ecs_set(rows->world, clone, Foo, {*foo * 2});
        *ctx = clone;
    }
}

void test_EcsClone_tc_clone_1_component_value_in_progress_override_w_set(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, CloneOneValueOverrideInProgress, EcsOnFrame, Foo);

    EcsHandle e = ecs_new(world, Foo_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});
    test_assertint(ecs_get(world, e, Foo), 10);

    EcsHandle clone = 0;
    ecs_set_context(world, &clone);

    ecs_progress(world, 0);

    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assertint(ecs_get(world, clone, Foo), 20);

    ecs_fini(world);
}

void test_EcsClone_tc_clone_2_component(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    EcsHandle clone = ecs_clone(world, e, false);
    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assert(ecs_has(world, clone, Bar_h));

    ecs_fini(world);
}


void CloneTwoInProgress(EcsRows *rows) {
    void *row;
    EcsHandle *ctx = ecs_get_context(rows->world);
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        EcsHandle clone = ecs_clone(rows->world, entity, false);
        *ctx = clone;
    }
}

void test_EcsClone_tc_clone_2_component_in_progress(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, CloneTwoInProgress, EcsOnFrame, Foo);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    EcsHandle clone = 0;
    ecs_set_context(world, &clone);

    ecs_progress(world, 0);

    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assert(ecs_has(world, clone, Bar_h));

    ecs_fini(world);
}


void test_EcsClone_tc_clone_2_component_value(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});
    test_assertint(ecs_get(world, e, Foo), 10);
    ecs_set(world, e, Bar, {20});
    test_assertint(ecs_get(world, e, Bar), 20);


    EcsHandle clone = ecs_clone(world, e, true);
    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assert(ecs_has(world, clone, Bar_h));
    test_assertint(ecs_get(world, clone, Foo), 10);
    test_assertint(ecs_get(world, clone, Bar), 20);

    ecs_fini(world);
}

void CloneTwoValueInProgress(EcsRows *rows) {
    void *row;
    EcsHandle *ctx = ecs_get_context(rows->world);
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        EcsHandle clone = ecs_clone(rows->world, entity, true);
        *ctx = clone;
    }
}

void test_EcsClone_tc_clone_2_component_value_in_progress(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, CloneOneValueInProgress, EcsOnFrame, Foo);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});
    test_assertint(ecs_get(world, e, Foo), 10);
    ecs_set(world, e, Bar, {20});
    test_assertint(ecs_get(world, e, Bar), 20);

    EcsHandle clone = 0;
    ecs_set_context(world, &clone);

    ecs_progress(world, 0);

    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assertint(ecs_get(world, clone, Foo), 10);
    test_assert(ecs_has(world, clone, Bar_h));
    test_assertint(ecs_get(world, clone, Bar), 20);

    ecs_fini(world);
}

void CloneTwoValueOverrideInProgress(EcsRows *rows) {
    EcsHandle *ctx = ecs_get_context(rows->world);
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsHandle entity = ecs_entity(row);
        EcsHandle Foo_h = rows->components[0];
        EcsHandle Bar_h = rows->components[1];
        Foo *foo = ecs_column(rows, row, 0);
        Bar *bar = ecs_column(rows, row, 1);

        EcsHandle clone = ecs_clone(rows->world, entity, true);
        ecs_set(rows->world, clone, Foo, {*foo * 2});
        ecs_set(rows->world, clone, Bar, {*bar * 2});
        *ctx = clone;
    }
}

void test_EcsClone_tc_clone_2_component_value_in_progress_override_w_set(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    ECS_COMPONENT(world, Foo);
    ECS_COMPONENT(world, Bar);
    ECS_FAMILY(world, MyFamily, Foo, Bar);
    ECS_SYSTEM(world, CloneTwoValueOverrideInProgress, EcsOnFrame, Foo, Bar);

    EcsHandle e = ecs_new(world, MyFamily_h);
    test_assert(e != 0);

    ecs_set(world, e, Foo, {10});
    test_assertint(ecs_get(world, e, Foo), 10);
    ecs_set(world, e, Bar, {20});
    test_assertint(ecs_get(world, e, Bar), 20);

    EcsHandle clone = 0;
    ecs_set_context(world, &clone);

    ecs_progress(world, 0);

    test_assert(clone != 0);
    test_assert(ecs_has(world, clone, Foo_h));
    test_assertint(ecs_get(world, clone, Foo), 20);
    test_assert(ecs_has(world, clone, Bar_h));
    test_assertint(ecs_get(world, clone, Bar), 40);

    ecs_fini(world);
}


void test_EcsClone_tc_clone_empty(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    EcsHandle clone = ecs_clone(world, e, false);
    test_assert(clone != 0);

    ecs_fini(world);
}


void test_EcsClone_tc_clone_empty_value(
    test_EcsClone this)
{
    EcsWorld *world = ecs_init();
    test_assert(world != NULL);

    EcsHandle e = ecs_new(world, 0);
    test_assert(e != 0);

    EcsHandle clone = ecs_clone(world, e, true);
    test_assert(clone != 0);

    ecs_fini(world);
}
