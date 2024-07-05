/**
 * @file addons/stats/system_monitor.c
 * @brief Stats addon system monitor
 */

#include "flecs.h"
#include "stats.h"
#include "../pipeline/pipeline.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(EcsSystemStats);

static
void flecs_system_monitor_dtor(EcsSystemStats *ptr) {
    ecs_map_iter_t it = ecs_map_iter(&ptr->stats);
    while (ecs_map_next(&it)) {
        ecs_system_stats_t *stats = ecs_map_ptr(&it);
        ecs_os_free(stats);
    }
    ecs_map_fini(&ptr->stats);
}

static ECS_CTOR(EcsSystemStats, ptr, {
    ecs_os_zeromem(ptr);
    ecs_map_init(&ptr->stats, NULL);
})

static ECS_COPY(EcsSystemStats, dst, src, {
    (void)dst;
    (void)src;
    ecs_abort(ECS_INVALID_OPERATION, "cannot copy system stats component");
})

static ECS_MOVE(EcsSystemStats, dst, src, {
    flecs_system_monitor_dtor(dst);
    ecs_os_memcpy_t(dst, src, EcsSystemStats);
    ecs_os_zeromem(src);
})

static ECS_DTOR(EcsSystemStats, ptr, {
    flecs_system_monitor_dtor(ptr);
})

static 
void flecs_system_stats_set_t(
    void *stats, int32_t t)
{
    ecs_assert(t >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t < ECS_STAT_WINDOW, ECS_INTERNAL_ERROR, NULL);
    ((ecs_system_stats_t*)stats)->query.t = t;
}

static
void flecs_system_stats_copy_last(
    void *stats, 
    void *src) 
{
    ecs_system_stats_copy_last(stats, src);
}

static
void flecs_system_stats_get(
    ecs_world_t *world, 
    ecs_entity_t res, 
    void *stats) 
{
    ecs_system_stats_get(world, res, stats);
}

static
void flecs_system_stats_reduce(
    void *stats, 
    void *src) 
{
    ecs_system_stats_reduce(stats, src);
}

static
void flecs_system_stats_reduce_last(
    void *stats, 
    void *last, 
    int32_t reduce_count) 
{
    ecs_system_stats_reduce_last(stats, last, reduce_count);
}

static
void flecs_system_stats_repeat_last(
    void* stats) 
{
    ecs_system_stats_repeat_last(stats);
}

void FlecsSystemMonitorImport(
    ecs_world_t *world)
{
    ECS_COMPONENT_DEFINE(world, EcsSystemStats);

    ecs_set_hooks(world, EcsSystemStats, {
        .ctor = ecs_ctor(EcsSystemStats),
        .copy = ecs_copy(EcsSystemStats),
        .move = ecs_move(EcsSystemStats),
        .dtor = ecs_dtor(EcsSystemStats)
    });

    ecs_stats_api_t api = {
        .copy_last = flecs_system_stats_copy_last,
        .get = flecs_system_stats_get,
        .reduce = flecs_system_stats_reduce,
        .reduce_last = flecs_system_stats_reduce_last,
        .repeat_last = flecs_system_stats_repeat_last,
        .set_t = flecs_system_stats_set_t,
        .stats_size = ECS_SIZEOF(ecs_system_stats_t),
        .monitor_component_id = ecs_id(EcsSystemStats),
        .query_component_id = EcsSystem
    };

    flecs_stats_api_import(world, &api);
}

#endif
