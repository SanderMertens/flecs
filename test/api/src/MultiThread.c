#include <include/api.h>

void Progress(EcsRows *rows) {
    int row;
    for (row = 0; row < rows->count; row ++) {
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

typedef struct Param {
    EcsEntity entity;
    int count;
} Param;

static
void TestSubset(EcsRows *rows) {
    Param *param = rows->param;

    int i;
    for (i = 0; i < rows->count; i ++) {
        test_assert(param->entity != rows->entities[i]);
        param->count ++;
    }    
}

static
void TestAll(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    EcsEntity TestSubset = ecs_column_component(rows, 2);

    int i;
    for (i = 0; i < rows->count; i ++) {
        Param param = {.entity = rows->entities[i], 0};
        ecs_run_w_filter(rows->world, TestSubset, 1, rows->frame_offset + i + 1, 0, 0, &param);
        p[i].x += param.count;
    }
}

void MultiThread_2_thread_test_combs_100_entity() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 2;
    EcsEntity handles[ENTITIES];

    ecs_new_w_count(world, Position, ENTITIES, handles);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, handles[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, handles[i], Position);
        test_int(p->x, 100 - i);
    }

    ecs_fini(world);
}

void MultiThread_3_thread_test_combs_100_entity() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 3;
    EcsEntity handles[ENTITIES];

    ecs_new_w_count(world, Position, ENTITIES, handles);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, handles[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, handles[i], Position);
        test_int(p->x, 100 - i);
    }

    ecs_fini(world);
}

void MultiThread_4_thread_test_combs_100_entity() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 4;
    EcsEntity handles[ENTITIES];

    ecs_new_w_count(world, Position, ENTITIES, handles);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, handles[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, handles[i], Position);
        test_int(p->x, 100 - i);
    }

    ecs_fini(world);
}

void MultiThread_5_thread_test_combs_100_entity() {
      EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 5;
    EcsEntity handles[ENTITIES];

    ecs_new_w_count(world, Position, ENTITIES, handles);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, handles[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, handles[i], Position);
        test_int(p->x, 100 - i);
    }

    ecs_fini(world);
}

void MultiThread_6_thread_test_combs_100_entity() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 6;
    EcsEntity handles[ENTITIES];

    ecs_new_w_count(world, Position, ENTITIES, handles);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, handles[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, handles[i], Position);
        test_int(p->x, 100 - i);
    }

    ecs_fini(world);
}

void MultiThread_2_thread_test_combs_100_entity_2_types() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 2;

    EcsEntity start = ecs_new_w_count(world, Position, ENTITIES / 2, NULL);
    ecs_new_w_count(world, Type, ENTITIES / 2, NULL);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void MultiThread_3_thread_test_combs_100_entity_2_types() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 20, THREADS = 3;

    EcsEntity start = ecs_new_w_count(world, Position, ENTITIES / 2, NULL);
    ecs_new_w_count(world, Type, ENTITIES / 2, NULL);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void MultiThread_4_thread_test_combs_100_entity_2_types() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 4;

    EcsEntity start = ecs_new_w_count(world, Position, ENTITIES / 2, NULL);
    ecs_new_w_count(world, Type, ENTITIES / 2, NULL);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void MultiThread_5_thread_test_combs_100_entity_2_types() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 5;

    EcsEntity start = ecs_new_w_count(world, Position, ENTITIES / 2, NULL);
    ecs_new_w_count(world, Type, ENTITIES / 2, NULL);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void MultiThread_6_thread_test_combs_100_entity_2_types() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, EcsManual, Position);
    ECS_SYSTEM(world, TestAll, EcsOnFrame, Position, ID.TestSubset);

    int i, ENTITIES = 100, THREADS = 6;

    EcsEntity start = ecs_new_w_count(world, Position, ENTITIES / 2, NULL);
    ecs_new_w_count(world, Type, ENTITIES / 2, NULL);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, start + i, Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        Position *p = ecs_get_ptr(world, start + i, Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}
