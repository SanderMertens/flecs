/**
 * @file addons/doc.h
 * @brief Doc module.
 *
 * The monitor module automatically tracks statistics from the stats addon and
 * stores them in components.
 */

#ifdef FLECS_MONITOR

/**
 * @defgroup c_addons_monitor Monitor
 * @brief The monitor addon periodically tracks statistics for the world and systems.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_MONITOR_H
#define FLECS_MONITOR_H

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_STATS
#define FLECS_STATS
#endif

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API extern ECS_COMPONENT_DECLARE(FlecsMonitor);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldStats);
FLECS_API extern ECS_COMPONENT_DECLARE(EcsPipelineStats);

FLECS_API extern ecs_entity_t EcsPeriod1s;
FLECS_API extern ecs_entity_t EcsPeriod1m;
FLECS_API extern ecs_entity_t EcsPeriod1h;
FLECS_API extern ecs_entity_t EcsPeriod1d;
FLECS_API extern ecs_entity_t EcsPeriod1w;

typedef struct {
    ecs_ftime_t elapsed;
    int32_t reduce_count;
} EcsStatsHeader;

typedef struct {
    EcsStatsHeader hdr; 
    ecs_world_stats_t stats;
} EcsWorldStats;

typedef struct {
    EcsStatsHeader hdr;
    ecs_pipeline_stats_t stats;
} EcsPipelineStats;

/* Module import */
FLECS_API
void FlecsMonitorImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
