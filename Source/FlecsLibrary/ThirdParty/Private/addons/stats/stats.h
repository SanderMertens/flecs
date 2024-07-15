/**
 * @file addons/stats/stats.h
 * @brief Internal functions/types for stats addon.
 */

#ifndef FLECS_STATS_PRIVATE_H
#define FLECS_STATS_PRIVATE_H

#include "../../private_api.h"

typedef struct {
    /* Statistics API interface */
    void (*copy_last)(void *stats, void *src);
    void (*get)(ecs_world_t *world, ecs_entity_t res, void *stats);
    void (*reduce)(void *stats, void *src);
    void (*reduce_last)(void *stats, void *last, int32_t reduce_count);
    void (*repeat_last)(void* stats);
    void (*set_t)(void *stats, int32_t t);
    void (*fini)(void *stats);

    /* Size of statistics type */
    ecs_size_t stats_size;

    /* Id of component that contains the statistics */
    ecs_entity_t monitor_component_id;

    /* Id of component used to query for monitored resources (optional) */
    ecs_id_t query_component_id;
} ecs_stats_api_t;

void flecs_stats_api_import(
    ecs_world_t *world,
    ecs_stats_api_t *api);

void FlecsWorldSummaryImport(
    ecs_world_t *world);

void FlecsWorldMonitorImport(
    ecs_world_t *world);

void FlecsSystemMonitorImport(
    ecs_world_t *world);

void FlecsPipelineMonitorImport(
    ecs_world_t *world);

#endif
