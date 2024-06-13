/**
 * @file addons/stats/world_monitor.c
 * @brief Stats addon world monitor.
 */

#include "flecs.h"
#include "stats.h"

#ifdef FLECS_STATS

ECS_COMPONENT_DECLARE(EcsWorldStats);

static
void flecs_world_stats_get_wrapper(
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

void FlecsWorldMonitorImport(
    ecs_world_t *world) 
{
    ECS_COMPONENT_DEFINE(world, EcsWorldStats);

    ecs_set_hooks(world, EcsWorldStats, {
        .ctor = flecs_default_ctor
    });

    ecs_stats_api_t api = {
        .copy_last = (void(*)(void*,void*))ecs_world_stats_copy_last,
        .get = (void(*)(ecs_world_t*,ecs_entity_t,void*))flecs_world_stats_get_wrapper,
        .reduce = (void(*)(void*,void*))ecs_world_stats_reduce,
        .reduce_last = (void(*)(void*,void*,int32_t))ecs_world_stats_reduce_last,
        .repeat_last = (void(*)(void*))ecs_world_stats_repeat_last,
        .set_t = flecs_world_stats_set_t,
        .fini = NULL,
        .stats_size = ECS_SIZEOF(ecs_world_stats_t),
        .monitor_component_id = ecs_id(EcsWorldStats)
    };

    flecs_stats_api_import(world, &api);
}

#endif
