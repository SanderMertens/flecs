/* This is a managed file. Do not delete this comment. */

#include <include/test.h>

typedef struct Foo {
    int x;
} Foo;

void Progress(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Foo *foo = ecs_data(rows, row, 0);
        foo->x ++;
    }
}

void test_EcsJobs_tc_2_thread_10_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 10, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_2_thread_1_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 1, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_2_thread_2_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 2, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_2_thread_5_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 5, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_3_thread_10_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 10, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_3_thread_1_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 1, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_3_thread_2_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 2, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_3_thread_5_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 5, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_4_thread_10_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 10, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_4_thread_1_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 1, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_4_thread_2_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 2, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_4_thread_5_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 5, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_5_thread_10_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 10, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_5_thread_1_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 1, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_5_thread_2_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 2, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_5_thread_5_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 5, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_6_thread_10_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 10, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_6_thread_1_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 1, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_6_thread_2_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 2, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}


void test_EcsJobs_tc_6_thread_5_entity(
    test_EcsJobs this)
{
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Foo);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Foo);

    int i, ENTITIES = 5, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Foo_h);
        ecs_set(world, handles[i], Foo, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_assertint(ecs_get(world, handles[i], Foo).x, 2);
    }

    ecs_fini(world);
}
