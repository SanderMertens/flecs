/**
 * @file addons/stats/pipeline_monitor.c
 * @brief Stats addon pipeline monitor
 */

#include "flecs.h"
#include "stats.h"
#include "../pipeline/pipeline.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(EcsPipelineStats);

static
void flecs_pipeline_monitor_dtor(EcsPipelineStats *ptr) {
    ecs_map_iter_t it = ecs_map_iter(&ptr->stats);
    while (ecs_map_next(&it)) {
        ecs_pipeline_stats_t *stats = ecs_map_ptr(&it);
        ecs_pipeline_stats_fini(stats);
        ecs_os_free(stats);
    }
    ecs_map_fini(&ptr->stats);
}

static ECS_CTOR(EcsPipelineStats, ptr, {
    ecs_os_zeromem(ptr);
    ecs_map_init(&ptr->stats, NULL);
})

static ECS_COPY(EcsPipelineStats, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "cannot copy pipeline stats component");
})

static ECS_MOVE(EcsPipelineStats, dst, src, {
    flecs_pipeline_monitor_dtor(dst);
    ecs_os_memcpy_t(dst, src, EcsPipelineStats);
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsPipelineStats, ptr, {
    flecs_pipeline_monitor_dtor(ptr);
})

static 
void flecs_pipeline_stats_set_t(
    void *stats, int32_t t)
{
    ecs_assert(t >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t < ECS_STAT_WINDOW, ECS_INTERNAL_ERROR, NULL);
    ((ecs_pipeline_stats_t*)stats)->t = t;
}


static
void flecs_pipeline_stats_copy_last(
    void *stats, 
    void *src) 
{
    ecs_pipeline_stats_copy_last(stats, src);
}

static
void flecs_pipeline_stats_get(
    ecs_world_t *world, 
    ecs_entity_t res, 
    void *stats) 
{
    ecs_pipeline_stats_get(world, res, stats);
}

static
void flecs_pipeline_stats_reduce(
    void *stats, 
    void *src) 
{
    ecs_pipeline_stats_reduce(stats, src);
}

static
void flecs_pipeline_stats_reduce_last(
    void *stats, 
    void *last, 
    int32_t reduce_count) 
{
    ecs_pipeline_stats_reduce_last(stats, last, reduce_count);
}

static
void flecs_pipeline_stats_repeat_last(
    void* stats) 
{
    ecs_pipeline_stats_repeat_last(stats);
}

static
void flecs_pipeline_stats_fini(
    void *stats) 
{
    ecs_pipeline_stats_fini(stats);
}

void FlecsPipelineMonitorImport(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsPipelineStats);

    ecs_set_hooks(world, EcsPipelineStats, {
        .ctor = ecs_ctor(EcsPipelineStats),
        .copy = ecs_copy(EcsPipelineStats),
        .move = ecs_move(EcsPipelineStats),
        .dtor = ecs_dtor(EcsPipelineStats)
    });

    ecs_stats_api_t api = {
        .copy_last = flecs_pipeline_stats_copy_last,
        .get = flecs_pipeline_stats_get,
        .reduce = flecs_pipeline_stats_reduce,
        .reduce_last = flecs_pipeline_stats_reduce_last,
        .repeat_last = flecs_pipeline_stats_repeat_last,
        .set_t = flecs_pipeline_stats_set_t,
        .fini = flecs_pipeline_stats_fini,
        .stats_size = ECS_SIZEOF(ecs_pipeline_stats_t),
        .monitor_component_id = ecs_id(EcsPipelineStats),
        .query_component_id = ecs_id(EcsPipeline)
    };

    flecs_stats_api_import(world, &api);
}

#endif
