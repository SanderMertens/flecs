#include <addons.h>

#define test_delta(prev, cur, field, value)\
    test_int(value, (cur)->field - (prev)->field);\
    (prev)->field = (cur)->field

void Stats_get_world_stats(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    test_int(stats.t, 1);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_before_progress_mini_world(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsPipeline);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), false);

    test_assert(ecs_vec_count(&stats.systems) == 0);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_before_progress(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_assert(ecs_vec_count(&stats.systems) == 0);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_no_systems(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vec_count(&stats.systems), 2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_lookup(world, "flecs.script.ProgressTasks"));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

static void FooSys(ecs_iter_t *it) { }
static void BarSys(ecs_iter_t *it) { }

void Stats_get_pipeline_stats_after_progress_1_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, 0);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vec_count(&stats.systems), 4);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_lookup(world, "flecs.script.ProgressTasks"));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], ecs_id(FooSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], 0); /* merge */

    ecs_progress(world, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_1_inactive_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, FooSys, EcsOnUpdate, Position); // no matching entities

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vec_count(&stats.systems), 2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_lookup(world, "flecs.script.ProgressTasks"));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */

    ecs_progress(world, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_2_systems(void) {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, 0);
    ECS_SYSTEM(world, BarSys, EcsOnUpdate, 0);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vec_count(&stats.systems), 5);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_lookup(world, "flecs.script.ProgressTasks"));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], ecs_id(FooSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], ecs_id(BarSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[4], 0); /* merge */

    ecs_progress(world, 0);

    ecs_run(world, ecs_id(BarSys), 0, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_2_systems_one_merge(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new_w(world, Position); // Make sure systems are active

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, [out] Position());
    ECS_SYSTEM(world, BarSys, EcsOnUpdate, Position);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vec_count(&stats.systems), 6);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_lookup(world, "flecs.script.ProgressTasks"));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], ecs_id(FooSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[4], ecs_id(BarSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[5], 0); /* merge */

    ecs_progress(world, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_w_task_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, 0);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);
    test_int(ecs_vec_count(&stats.systems), 0);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_entity_count(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    float count;
    float prev = count = stats.entities.count.gauge.avg[stats.t];
    test_assert(count != 0);

    ecs_entity_t e = ecs_new(world);

    ecs_world_stats_get(world, &stats);
    count = stats.entities.count.gauge.avg[stats.t];
    test_int(count - prev, 1);

    ecs_delete(world, e);

    prev = count;
    ecs_world_stats_get(world, &stats);
    count = stats.entities.count.gauge.avg[stats.t];
    test_int(count - prev, -1);

    ecs_fini(world);
}

void Stats_get_not_alive_entity_count(void) {
    ecs_world_t *world = ecs_init();

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    float count;
    float prev = count = stats.entities.not_alive_count.gauge.avg[stats.t];
    test_int(count, 0);

    ecs_entity_t e = ecs_new(world);

    prev = count;
    ecs_world_stats_get(world, &stats);
    count = stats.entities.not_alive_count.gauge.avg[stats.t];
    test_int(count - prev, 0);

    ecs_delete(world, e);

    prev = count;
    ecs_world_stats_get(world, &stats);
    count = stats.entities.not_alive_count.gauge.avg[stats.t];
    test_int(count - prev, 1);

    ecs_fini(world);
}

void Stats_progress_stats_systems(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsStats);

    for (int i = 0; i < 60 * 60; i ++) {
        ecs_progress(world, 0.016);
    }

    test_assert(true); // used to catch memory leaks

    ecs_fini(world);
}

void Stats_get_worker_stats(void) {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, 0);

    ecs_progress(world, 0);

    ecs_worker_stats_t stats = {0};
    ecs_worker_stats_get(world, &stats);

    test_int(stats.t, 1);
    test_int(ecs_vec_count(&stats.threads), 1);

    ecs_thread_stats_t *thread = ecs_vec_get_t(
        &stats.threads, ecs_thread_stats_t, 0);
    test_int(thread->systems_ran.counter.value[stats.t], 
        ecs_get_world_info(world)->systems_ran_total);
    test_assert(thread->systems_ran.counter.value[stats.t] > 0);

    ecs_worker_stats_fini(&stats);

    ecs_fini(world);
}

static void MtSys(ecs_iter_t *it) { }

void Stats_get_worker_stats_w_threads(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_system(world, {
        .entity = ecs_entity(world, { .name = "MtSys" }),
        .phase = EcsOnUpdate,
        .query.terms = {{ ecs_id(Position) }},
        .callback = MtSys,
        .multi_threaded = true
    });

    for (int i = 0; i < 100; i ++) {
        ecs_insert(world, ecs_value(Position, {10, 20}));
    }

    ecs_set_threads(world, 4);

    ecs_progress(world, 0);

    ecs_worker_stats_t stats = {0};
    ecs_worker_stats_get(world, &stats);
    test_int(ecs_vec_count(&stats.threads), 4);

    double total = 0;
    for (int i = 0; i < 4; i ++) {
        ecs_thread_stats_t *thread = ecs_vec_get_t(
            &stats.threads, ecs_thread_stats_t, i);
        test_assert(thread->systems_ran.counter.value[stats.t] >= 1);
        total += thread->systems_ran.counter.value[stats.t];
    }

    test_int(total, ecs_get_world_info(world)->systems_ran_total);

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    ecs_worker_stats_get(world, &stats);
    test_int(ecs_vec_count(&stats.threads), 2);

    ecs_worker_stats_fini(&stats);

    ecs_fini(world);
}

static void SleepSys(ecs_iter_t *it) {
    /* Sleep may return early relative to the clock used to measure system
     * time (e.g. on Windows), so wait until at least 10ms has elapsed. */
    ecs_time_t start = {0};
    ecs_time_measure(&start);
    double elapsed = 0;
    do {
        ecs_os_sleep(0, 1000 * 1000);
        ecs_time_t t = start;
        elapsed = ecs_time_measure(&t);
    } while (elapsed < 0.0101);
}

void Stats_system_time_w_threads(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t sys = ecs_system(world, {
        .entity = ecs_entity(world, { .name = "SleepSys" }),
        .phase = EcsOnUpdate,
        .query.terms = {{ ecs_id(Position) }},
        .callback = SleepSys,
        .multi_threaded = true
    });

    for (int i = 0; i < 100; i ++) {
        ecs_insert(world, ecs_value(Position, {10, 20}));
    }

    ecs_set_threads(world, 4);
    ecs_measure_system_time(world, true);

    ecs_world_stats_t world_stats = {0};
    ecs_world_stats_get(world, &world_stats);

    ecs_progress(world, 0);

    const ecs_system_t *sys_data = ecs_system_get(world, sys);
    test_assert(sys_data != NULL);
    test_assert(sys_data->time_spent >= 0.039);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    test_assert(info->system_time_total >= 0.0099);
    test_assert(info->system_time_total < sys_data->time_spent);

    ecs_world_stats_get(world, &world_stats);
    int32_t t = world_stats.t;
    test_assert(world_stats.performance.system_cpu_time.counter.value[t] >= 
        sys_data->time_spent);
    test_assert(world_stats.performance.system_time.counter.value[t] < 
        world_stats.performance.system_cpu_time.counter.value[t]);

    ecs_worker_stats_t stats = {0};
    ecs_worker_stats_get(world, &stats);
    test_int(ecs_vec_count(&stats.threads), 4);
    for (int i = 0; i < 4; i ++) {
        ecs_thread_stats_t *thread = ecs_vec_get_t(
            &stats.threads, ecs_thread_stats_t, i);
        test_assert(thread->system_time.counter.value[stats.t] >= 0.0099);
    }

    ecs_worker_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_progress_stats_w_threads(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsStats);

    ecs_set_threads(world, 2);

    for (int i = 0; i < 60 * 2; i ++) {
        ecs_progress(world, 0.016);
    }

    const EcsWorkerStats *ws = ecs_get_pair(
        world, EcsWorld, EcsWorkerStats, EcsPeriod1s);
    test_assert(ws != NULL);
    test_int(ecs_vec_count(&ws->stats.threads), 2);

    ws = ecs_get_pair(world, EcsWorld, EcsWorkerStats, EcsPeriod1m);
    test_assert(ws != NULL);
    test_int(ecs_vec_count(&ws->stats.threads), 2);

    const EcsWorldSummary *summary = ecs_get(world, EcsWorld, EcsWorldSummary);
    test_assert(summary != NULL);
    test_int(summary->thread_count, 2);
    test_assert(summary->system_cpu_time_total > 0);

    ecs_fini(world);
}
