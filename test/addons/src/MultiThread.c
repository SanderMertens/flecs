#include <addons.h>

static ECS_COMPONENT_DECLARE(Position);

void MultiThread_setup() {
    ecs_log_set_level(-3);
}

void Progress(ecs_iter_t *it) {
    Position *pos = ecs_term(it, Position, 1);
    int row;
    for (row = 0; row < it->count; row ++) {
        Position *p = &pos[row];
        p->x ++;
    }
}

static
ecs_world_t* init_world(void) {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position);

    ECS_SYSTEM(world, Progress, EcsOnUpdate, Position);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = Progress,
        .multi_threaded = true
    });

    return world;
}

void MultiThread_2_thread_10_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 10, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_2_thread_1_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 1, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

void MultiThread_2_thread_2_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 2, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

void MultiThread_2_thread_5_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 5, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_10_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 10, THREADS = 3;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_1_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 1, THREADS = 3;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_2_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 2, THREADS = 3;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_3_thread_5_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 5, THREADS = 3;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_10_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 10, THREADS = 4;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_1_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 1, THREADS = 4;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_2_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 2, THREADS = 4;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_4_thread_5_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 5, THREADS = 4;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_10_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 10, THREADS = 5;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_1_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 1, THREADS = 5;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_2_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 2, THREADS = 5;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_5_thread_5_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 5, THREADS = 5;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_10_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 10, THREADS = 6;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_1_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 1, THREADS = 6;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}


void MultiThread_6_thread_2_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 2, THREADS = 6;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

void MultiThread_6_thread_5_entity() {
    ecs_world_t *world = init_world();

    int i, ENTITIES = 5, THREADS = 6;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

void MultiThread_2_thread_1_entity_instanced() {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.add = {ecs_dependson(EcsOnUpdate)},
        .callback = Progress,
        .query.filter = {
            .expr = "Position",
            .instanced = true
        },
        .multi_threaded = true
    });
    test_assert(s != 0);

    int i, ENTITIES = 1, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

void MultiThread_2_thread_5_entity_instanced() {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.add = {ecs_dependson(EcsOnUpdate)},
        .callback = Progress,
        .query.filter = {
            .expr = "Position",
            .instanced = true
        },
        .multi_threaded = true
    });
    test_assert(s != 0);

    int i, ENTITIES = 5, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

void MultiThread_2_thread_10_entity_instanced() {
    ecs_world_t *world = ecs_init();
    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity.add = {ecs_dependson(EcsOnUpdate)},
        .callback = Progress,
        .query.filter = {
            .expr = "Position",
            .instanced = true
        },
        .multi_threaded = true
    });
    test_assert(s != 0);

    int i, ENTITIES = 10, THREADS = 2;
    ecs_entity_t *handles = ecs_os_alloca(sizeof(ecs_entity_t) * ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        handles[i] = ecs_new(world, Position);
        ecs_set(world, handles[i], Position, {0});
    }

    ecs_set_threads(world, THREADS);
    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 1);
    }

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        test_int(ecs_get(world, handles[i], Position)->x, 2);
    }

    ecs_fini(world);
}

typedef struct Param {
    ecs_entity_t entity;
    int count;
} Param;

static
void TestSubset(ecs_iter_t *it) {
    Param *param = it->param;

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(param->entity != it->entities[i]);
        param->count ++;
    }    
}

static
void TestAll(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    ecs_entity_t TestSubset = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        Param param = {.entity = it->entities[i], 0};
        ecs_run_w_filter(it->world, TestSubset, 1, it->frame_offset + i + 1, 0, &param);
        p[i].x += param.count;
    }
}

static
void test_combs_100_entity(int THREADS) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ECS_SYSTEM(world, TestSubset, 0, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, TestSubset());

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = TestAll,
        .multi_threaded = true
    });

    int i, ENTITIES = 100;

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, ids[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES; i ++) {
        const Position *p = ecs_get(world, ids[i], Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);    
}

void MultiThread_2_thread_test_combs_100_entity_w_next_worker() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ECS_SYSTEM(world, TestSubset, 0, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, TestSubset()");

    int i, ENTITIES = 100;

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, ids[i], Position, {1, 2});
    }

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t wit = ecs_worker_iter(&it, 0, 2);
    while (ecs_worker_next(&wit)) {
        TestAll(&wit);
    }

    it = ecs_query_iter(world, q);
    wit = ecs_worker_iter(&it, 1, 2);
    while (ecs_worker_next(&wit)) {
        TestAll(&wit);
    }

    for (i = 0; i < ENTITIES; i ++) {
        const Position *p = ecs_get(world, ids[i], Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world); 
}

void MultiThread_2_thread_test_combs_100_entity() {
    test_combs_100_entity(2);
}

void MultiThread_3_thread_test_combs_100_entity() {
    test_combs_100_entity(3);
}

void MultiThread_4_thread_test_combs_100_entity() {
    test_combs_100_entity(4);
}

void MultiThread_5_thread_test_combs_100_entity() {
    test_combs_100_entity(5);
}

void MultiThread_6_thread_test_combs_100_entity() {
    test_combs_100_entity(6);
}

static
void test_combs_100_entity_2_types(int THREADS) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, 0, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, TestSubset());

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = TestAll,
        .multi_threaded = true
    });

    int i, ENTITIES = 100;

    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, ENTITIES / 2);
    ecs_entity_t ids_1[50];
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * ENTITIES / 2);
    const ecs_entity_t *ids_2 = bulk_new_w_type(world, Type, ENTITIES / 2);

    for (i = 0; i < ENTITIES / 2; i ++) {
        ecs_set(world, ids_1[i], Position, {1, 2});
        ecs_set(world, ids_2[i], Position, {1, 2});
    }

    ecs_set_threads(world, THREADS);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES / 2; i ++) {
        const Position *p = ecs_get(world, ids_1[i], Position);
        test_int(p->x, ENTITIES - i);

        p = ecs_get(world, ids_2[i], Position);
        test_int(p->x, ENTITIES - (i + ENTITIES / 2));
    }

    ecs_fini(world);
}

void MultiThread_2_thread_test_combs_100_entity_2_types() {
    test_combs_100_entity_2_types(2);
}

void MultiThread_3_thread_test_combs_100_entity_2_types() {
    test_combs_100_entity_2_types(3);
}

void MultiThread_4_thread_test_combs_100_entity_2_types() {
    test_combs_100_entity_2_types(4);
}

void MultiThread_5_thread_test_combs_100_entity_2_types() {
    test_combs_100_entity_2_types(5);
}

void MultiThread_6_thread_test_combs_100_entity_2_types() {
    test_combs_100_entity_2_types(6);
}

void MultiThread_change_thread_count() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_PREFAB(world, Type, Position, Velocity);

    ECS_SYSTEM(world, TestSubset, 0, Position);
    ECS_SYSTEM(world, TestAll, EcsOnUpdate, Position, TestSubset());

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = TestAll,
        .multi_threaded = true
    });

    int i, ENTITIES = 100;

    const ecs_entity_t *temp_ids_1 = ecs_bulk_new(world, Position, ENTITIES / 2);
    ecs_entity_t ids_1[50];
    memcpy(ids_1, temp_ids_1, sizeof(ecs_entity_t) * ENTITIES / 2);
    const ecs_entity_t *ids_2 = bulk_new_w_type(world, Type, ENTITIES / 2);

    for (i = 0; i < ENTITIES / 2; i ++) {
        ecs_set(world, ids_1[i], Position, {1, 2});
        ecs_set(world, ids_2[i], Position, {1, 2});
    }

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES / 2; i ++) {
        Position *p = ecs_get_mut(world, ids_1[i], Position);
        test_int(p->x, ENTITIES - i);
        p->x = 1;

        p = ecs_get_mut(world, ids_2[i], Position);
        test_int(p->x, ENTITIES - (i + ENTITIES / 2));
        p->x = 1;
    }

    ecs_set_threads(world, 3);

    ecs_progress(world, 0);

    for (i = 0; i < ENTITIES / 2; i ++) {
        const Position *p = ecs_get(world, ids_1[i], Position);
        test_int(p->x, ENTITIES - i);

        p = ecs_get(world, ids_2[i], Position);
        test_int(p->x, ENTITIES - (i + ENTITIES / 2));
    }    

    ecs_fini(world);
}

static
void QuitSystem(ecs_iter_t *it) {
    ecs_quit(it->world);
}

void MultiThread_multithread_quit() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ECS_SYSTEM(world, QuitSystem, EcsOnUpdate, Position);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = QuitSystem,
        .multi_threaded = true
    });

    ecs_bulk_new(world, Position, 100);

    ecs_set_threads(world, 2);

    test_assert( ecs_progress(world, 0) == 0);

    ecs_fini(world);
}

static bool has_ran = false;

static
void MtTask(ecs_iter_t *it) {
    has_ran = true;
}

void MultiThread_schedule_w_tasks() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, MtTask, EcsOnUpdate, 0);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = MtTask,
        .multi_threaded = true
    });

    ecs_set_threads(world, 2);

    test_assert( ecs_progress(world, 0) != 0);

    test_assert( has_ran == true);

    ecs_fini(world);
}

static
void ReactiveDummySystem(ecs_iter_t * it) {
    has_ran = true;
}

static
void PeriodicDummySystem(ecs_iter_t * it) {
    ecs_id_t ecs_id(Position) = ecs_term_id(it, 1);
    
    int i;
    for (i = 0; i < it->count; i++ ) {
        ecs_set(it->world, it->entities[i], Position, {0});
    }
}

void MultiThread_reactive_system() {    
    ecs_world_t * world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);        
    ECS_SYSTEM(world, PeriodicDummySystem, EcsOnUpdate, Position);
    ECS_OBSERVER(world, ReactiveDummySystem, EcsOnSet, Position);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = PeriodicDummySystem,
        .multi_threaded = true
    });

    ecs_bulk_new(world, Position, 2);
    ecs_set_threads(world, 2);

    test_assert(has_ran == false);

    ecs_progress(world, 0);

    test_assert(has_ran == true);

    ecs_fini(world);
}

void MultiThread_fini_after_set_threads() {
    ecs_world_t * world = ecs_init();

    ecs_set_threads(world, 2);

    ecs_fini(world);

    // Make sure code doesn't crash
    test_assert(true);
}

static
void SingleThreadedSystem(ecs_iter_t * it) {
    Position *p = ecs_term(it, Position, 1);
    
    int i;
    for (i = 0; i < it->count; i++ ) {
        p[i].x ++;
        p[i].y ++;
    }
}

void MultiThread_2_threads_single_threaded_system() {
    ecs_world_t * world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);        
    ECS_SYSTEM(world, SingleThreadedSystem, EcsOnUpdate, Position);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity.entity = SingleThreadedSystem,
        .multi_threaded = false
    });

    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 21);
    test_int(p->y, 31);

    ecs_fini(world);
}

static int create_query_invoked = 0;

static
void CreateQuery(ecs_iter_t *it) {
    ecs_query_new(it->world, "0");
    create_query_invoked ++;
}

void MultiThread_no_staging_w_multithread() {
    for (int i = 0; i < 10; i ++) {
        ecs_world_t *world = ecs_init();

        ecs_set_threads(world, 32);

        ecs_system_init(world, &(ecs_system_desc_t){
            .callback = CreateQuery,
            .no_staging = true,
            .entity.add = {ecs_dependson(EcsOnUpdate)}
        });

        create_query_invoked = 0;
        ecs_progress(world, 0);
        test_int(create_query_invoked, 1);

        ecs_fini(world);
    }

    test_assert(true);
}
