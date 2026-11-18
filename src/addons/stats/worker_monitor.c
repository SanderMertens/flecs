/**
 * @file addons/stats/worker_monitor.c
 * @brief Stats addon worker monitor.
 */

#include "flecs.h"

#ifdef FLECS_STATS

#include "stats.h"

ECS_COMPONENT_DECLARE(EcsWorkerStats);

static ECS_CTOR(EcsWorkerStats, ptr, {
    ecs_os_zeromem(ptr);
})

static ECS_COPY(EcsWorkerStats, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "cannot copy worker stats component");
})

static ECS_MOVE(EcsWorkerStats, dst, src, {
    ecs_worker_stats_fini(&dst->stats);
    ecs_os_memcpy_t(dst, src, EcsWorkerStats);
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsWorkerStats, ptr, {
    ecs_worker_stats_fini(&ptr->stats);
})

static void flecs_worker_stats_get(
    ecs_world_t *world, 
    ecs_entity_t res, 
    void *stats)
{
    (void)res;
    ecs_worker_stats_get(world, stats);
}

static void flecs_worker_stats_set_t(
    void *stats, int32_t t)
{
    ecs_assert(t >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t < ECS_STAT_WINDOW, ECS_INTERNAL_ERROR, NULL);
    ((ecs_worker_stats_t*)stats)->t = t;
}

static void flecs_worker_stats_copy_last(
    void *stats, 
    void *src) 
{
    ecs_worker_stats_copy_last(stats, src);
}

static void flecs_worker_stats_reduce(
    void *stats, 
    void *src) 
{
    ecs_worker_stats_reduce(stats, src);
}

static void flecs_worker_stats_reduce_last(
    void *stats, 
    void *last, 
    int32_t reduce_count) 
{
    ecs_worker_stats_reduce_last(stats, last, reduce_count);
}

static void flecs_worker_stats_repeat_last(
    void* stats) 
{
    ecs_worker_stats_repeat_last(stats);
}

static void flecs_worker_stats_fini(
    void *stats) 
{
    ecs_worker_stats_fini(stats);
}

void FlecsWorkerMonitorImport(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsWorkerStats);

    ecs_set_hooks(world, EcsWorkerStats, {
        .ctor = ecs_ctor(EcsWorkerStats),
        .copy = ecs_copy(EcsWorkerStats),
        .move = ecs_move(EcsWorkerStats),
        .dtor = ecs_dtor(EcsWorkerStats)
    });

    ecs_stats_api_t api = {
        .copy_last = flecs_worker_stats_copy_last,
        .get = flecs_worker_stats_get,
        .reduce = flecs_worker_stats_reduce,
        .reduce_last = flecs_worker_stats_reduce_last,
        .repeat_last = flecs_worker_stats_repeat_last,
        .set_t = flecs_worker_stats_set_t,
        .fini = flecs_worker_stats_fini,
        .stats_size = ECS_SIZEOF(ecs_worker_stats_t),
        .monitor_component_id = ecs_id(EcsWorkerStats)
    };

    flecs_stats_api_import(world, &api);
}

#endif
