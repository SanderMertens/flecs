#include <addons.h>

#define test_delta(prev, cur, field, value)\
    test_int(value, (cur)->field - (prev)->field);\
    (prev)->field = (cur)->field

void Stats_get_world_stats() {
    ecs_world_t *world = ecs_init();

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    test_int(stats.t, 1);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_before_progress_mini_world() {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsPipeline);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), false);

    test_assert(stats.systems == NULL);
    test_assert(ecs_map_count(&stats.system_stats) == 0);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_before_progress() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_assert(stats.systems == NULL);
    test_assert(ecs_map_count(&stats.system_stats) != 0); /* Inactive systems */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_no_systems() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vector_count(stats.systems), 1);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], 0); /* merge */
    
    test_assert(ecs_map_count(&stats.system_stats) != 0); /* Inactive systems */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

static void FooSys(ecs_iter_t *it) { }
static void BarSys(ecs_iter_t *it) { }

void Stats_get_pipeline_stats_after_progress_1_system() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, 0);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vector_count(stats.systems), 2);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], ecs_id(FooSys));
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    
    test_assert(ecs_map_count(&stats.system_stats) != 0);
    ecs_system_stats_t *sys_stats = ecs_map_get(
        &stats.system_stats, ecs_system_stats_t, ecs_id(FooSys));
    test_assert(sys_stats != NULL);
    test_int(sys_stats->query.t, 1);
    test_int(sys_stats->invoke_count.counter.value[1], 1);

    ecs_progress(world, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);
    test_int(sys_stats->query.t, 2);
    test_int(sys_stats->invoke_count.counter.value[2], 2);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_1_inactive_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, FooSys, EcsOnUpdate, Position); // no matching entities

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vector_count(stats.systems), 1);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], 0); /* merge */
    
    test_assert(ecs_map_count(&stats.system_stats) != 0);
    ecs_system_stats_t *sys_stats = ecs_map_get(
        &stats.system_stats, ecs_system_stats_t, ecs_id(FooSys));
    test_assert(sys_stats != NULL);
    test_int(sys_stats->query.t, 1);
    test_int(sys_stats->invoke_count.counter.value[1], 0);

    ecs_progress(world, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);
    test_int(sys_stats->query.t, 2);
    test_int(sys_stats->invoke_count.counter.value[2], 0);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_2_systems() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, 0);
    ECS_SYSTEM(world, BarSys, EcsOnUpdate, 0);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vector_count(stats.systems), 3);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], ecs_id(FooSys));
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], ecs_id(BarSys));
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[2], 0); /* merge */
    
    test_assert(ecs_map_count(&stats.system_stats) != 0);
    ecs_system_stats_t *sys_foo_stats = ecs_map_get(
        &stats.system_stats, ecs_system_stats_t, ecs_id(FooSys));
    test_assert(sys_foo_stats != NULL);
    test_int(sys_foo_stats->query.t, 1);
    test_int(sys_foo_stats->invoke_count.counter.value[1], 1);

    ecs_system_stats_t *sys_bar_stats = ecs_map_get(
        &stats.system_stats, ecs_system_stats_t, ecs_id(BarSys));
    test_assert(sys_bar_stats != NULL);
    test_int(sys_bar_stats->query.t, 1);
    test_int(sys_bar_stats->invoke_count.counter.value[1], 1);

    ecs_progress(world, 0);

    ecs_run(world, ecs_id(BarSys), 0, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);
    test_int(sys_foo_stats->query.t, 2);
    test_int(sys_foo_stats->invoke_count.counter.value[2], 2);

    test_int(sys_bar_stats->query.t, 2);
    test_int(sys_bar_stats->invoke_count.counter.value[2], 3);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_pipeline_stats_after_progress_2_systems_one_merge() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new(world, Position); // Make sure systems are active

    ECS_SYSTEM(world, FooSys, EcsOnUpdate, Position());
    ECS_SYSTEM(world, BarSys, EcsOnUpdate, Position);

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    ecs_progress(world, 0);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);

    test_int(ecs_vector_count(stats.systems), 4);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], ecs_id(FooSys));
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[2], ecs_id(BarSys));
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[3], 0); /* merge */
    
    test_assert(ecs_map_count(&stats.system_stats) != 0);
    ecs_system_stats_t *sys_foo_stats = ecs_map_get(
        &stats.system_stats, ecs_system_stats_t, ecs_id(FooSys));
    test_assert(sys_foo_stats != NULL);
    test_int(sys_foo_stats->query.t, 1);
    test_int(sys_foo_stats->invoke_count.counter.value[1], 1);

    ecs_system_stats_t *sys_bar_stats = ecs_map_get(
        &stats.system_stats, ecs_system_stats_t, ecs_id(BarSys));
    test_assert(sys_bar_stats != NULL);
    test_int(sys_bar_stats->query.t, 1);
    test_int(sys_bar_stats->invoke_count.counter.value[1], 1);

    ecs_progress(world, 0);

    test_bool(ecs_pipeline_stats_get(world, pipeline, &stats), true);
    test_int(sys_foo_stats->query.t, 2);
    test_int(sys_foo_stats->invoke_count.counter.value[2], 2);

    test_int(sys_bar_stats->query.t, 2);
    test_int(sys_bar_stats->invoke_count.counter.value[2], 2);

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Stats_get_entity_count() {
    ecs_world_t *world = ecs_init();

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    float count;
    float prev = count = stats.entity_count.gauge.avg[stats.t];
    test_assert(count != 0);

    ecs_entity_t e = ecs_new_id(world);

    ecs_world_stats_get(world, &stats);
    count = stats.entity_count.gauge.avg[stats.t];
    test_int(count - prev, 1);

    ecs_delete(world, e);

    prev = count;
    ecs_world_stats_get(world, &stats);
    count = stats.entity_count.gauge.avg[stats.t];
    test_int(count - prev, -1);

    ecs_fini(world);
}

void Stats_get_not_alive_entity_count() {
    ecs_world_t *world = ecs_init();

    ecs_world_stats_t stats = {0};
    ecs_world_stats_get(world, &stats);

    float count;
    float prev = count = stats.entity_not_alive_count.gauge.avg[stats.t];
    test_assert(count == 0);

    ecs_entity_t e = ecs_new_id(world);

    prev = count;
    ecs_world_stats_get(world, &stats);
    count = stats.entity_not_alive_count.gauge.avg[stats.t];
    test_int(count - prev, 0);

    ecs_delete(world, e);

    prev = count;
    ecs_world_stats_get(world, &stats);
    count = stats.entity_not_alive_count.gauge.avg[stats.t];
    test_int(count - prev, 1);

    ecs_fini(world);
}
