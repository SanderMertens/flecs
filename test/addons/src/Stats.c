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

    test_int(ecs_vec_count(&stats.systems), 1);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], 0); /* merge */

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

    test_int(ecs_vec_count(&stats.systems), 2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_id(FooSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */

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

    test_int(ecs_vec_count(&stats.systems), 1);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], 0); /* merge */

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

    test_int(ecs_vec_count(&stats.systems), 3);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_id(FooSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], ecs_id(BarSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], 0); /* merge */

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

    test_int(ecs_vec_count(&stats.systems), 4);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], ecs_id(FooSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], ecs_id(BarSys));
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], 0); /* merge */

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

/* Clock that never advances, like a host-stepped clock that is only updated in
 * between frames. */
static void stats_stalled_get_time(ecs_time_t *time_out) {
    time_out->sec = 1000;
    time_out->nanosec = 0;
}

void Stats_world_summary_fps_w_stalled_clock(void) {
    ecs_os_set_api_defaults();

    ecs_os_api_t prev_os_api = ecs_os_api;
    test_assert(prev_os_api.get_time_ != stats_stalled_get_time);

    ecs_os_api_t os_api = prev_os_api;
    os_api.get_time_ = stats_stalled_get_time;

    /* Marks the OS API as initialized, so that a later ecs_init doesn't
     * reinstall the defaults. Is a no-op if it already was initialized, hence
     * the assignment below. */
    ecs_os_set_api(&os_api);
    ecs_os_api.get_time_ = stats_stalled_get_time;
    test_assert(ecs_os_api.get_time_ == stats_stalled_get_time);

    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsStats);

    const EcsWorldSummary *summary = ecs_get(world, EcsWorld, EcsWorldSummary);
    test_assert(summary != NULL);
    test_int(summary->fps, 0);

    /* The first frame reports a best-guess delta rather than a measured one,
     * so start the gauge window after it. */
    ecs_progress(world, 0);

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    int32_t i;
    for (i = 0; i < 4; i ++) {
        ecs_progress(world, 0);

        summary = ecs_get(world, EcsWorld, EcsWorldSummary);
        test_assert(summary != NULL);

        /* The clock does not advance, so frames report the minimum delta
         * time. That is not a frame rate of a billion. */
        test_assert(summary->fps >= 0);
        test_assert(summary->fps < 1000000.0);

        if (i) {
            test_int(summary->fps, 0);
        }
    }

    /* The windowed fps gauge over the same frames has the same contract. */
    ecs_world_stats_get(world, &stats);
    test_assert(stats.performance.fps.gauge.avg[stats.t] >= 0);
    test_assert(stats.performance.fps.gauge.avg[stats.t] < 1000000.0);
    test_int(stats.performance.fps.gauge.avg[stats.t], 0);

    ecs_fini(world);

    ecs_os_api = prev_os_api;
}
