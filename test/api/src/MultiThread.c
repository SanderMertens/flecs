#include <include/api.h>

void Progress(EcsRows *rows) {
    int row;
    for (row = rows->begin; row < rows->end; row ++) {
        Position *foo = ecs_field(rows, Position, row, 1);
        foo->x ++;
    }
}

void MultiThread_2_thread_10_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 10, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_2_thread_1_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 1, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_2_thread_2_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 2, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_2_thread_5_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 5, THREADS = 2;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_10_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 10, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_1_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 1, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_2_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 2, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_5_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 5, THREADS = 3;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_10_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 10, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_1_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 1, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_2_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 2, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_5_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 5, THREADS = 4;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_10_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 10, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_1_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 1, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_2_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 2, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_5_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 5, THREADS = 5;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_10_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 10, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_1_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 1, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_2_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 2, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_5_entity() {
    EcsWorld *world = ecs_init();
    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, Progress, EcsOnFrame, Position);

    int i, ENTITIES = 5, THREADS = 6;
    EcsEntity handles[ENTITIES];

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position).x, 2);
    }

    ecs_fini(world);
}
