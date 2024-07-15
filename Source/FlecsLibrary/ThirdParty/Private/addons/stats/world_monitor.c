/**
 * @file addons/stats/world_monitor.c
 * @brief Stats addon world monitor.
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(EcsWorldStats);

static
void flecs_world_stats_get(
    ecs_world_t *world, ecs_entity_t res, void *stats)
{
    (void)res;
    ecs_world_stats_get(world, stats);
}

static 
void flecs_world_stats_set_t(
    void *stats, int32_t t)
{
    ecs_assert(t >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t < ECS_STAT_WINDOW, ECS_INTERNAL_ERROR, NULL);
    ((ecs_world_stats_t*)stats)->t = t;
}

static
void flecs_world_stats_copy_last(
    void *stats, 
    void *src) 
{
    ecs_world_stats_copy_last(stats, src);
}

static
void flecs_world_stats_reduce(
    void *stats, 
    void *src) 
{
    ecs_world_stats_reduce(stats, src);
}

static
void flecs_world_stats_reduce_last(
    void *stats, 
    void *last, 
    int32_t reduce_count) 
{
    ecs_world_stats_reduce_last(stats, last, reduce_count);
}

static
void flecs_world_stats_repeat_last(
    void* stats) 
{
    ecs_world_stats_repeat_last(stats);
}

void FlecsWorldMonitorImport(
    ecs_world_t *world) 
{
    ECS_COMPONENT_DEFINE(world, EcsWorldStats);

    ecs_set_hooks(world, EcsWorldStats, {
        .ctor = flecs_default_ctor
    });

    ecs_stats_api_t api = {
        .copy_last = flecs_world_stats_copy_last,
        .get = flecs_world_stats_get,
        .reduce = flecs_world_stats_reduce,
        .reduce_last = flecs_world_stats_reduce_last,
        .repeat_last = flecs_world_stats_repeat_last,
        .set_t = flecs_world_stats_set_t,
        .fini = NULL,
        .stats_size = ECS_SIZEOF(ecs_world_stats_t),
        .monitor_component_id = ecs_id(EcsWorldStats)
    };

    flecs_stats_api_import(world, &api);
}

#endif
