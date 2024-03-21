/**
 * @file addons/monitor.h
 * @brief Doc module.
 *
 * The monitor module automatically tracks statistics from the stats addon and
 * stores them in components.
 */

#ifdef FLECS_MONITOR

/**
 * @defgroup c_addons_monitor Monitor
 * @ingroup c_addons
 * The monitor addon periodically tracks statistics for the world and systems.
 *
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
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldSummary);
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

typedef struct {
    /* Target FPS */
    double target_fps;          /**< Target FPS */

    /* Total time */
    double frame_time_total;    /**< Total time spent processing a frame */
    double system_time_total;   /**< Total time spent in systems */
    double merge_time_total;    /**< Total time spent in merges */

    /* Last frame time */
    double frame_time_last;     /**< Time spent processing a frame */
    double system_time_last;    /**< Time spent in systems */
    double merge_time_last;     /**< Time spent in merges */

    int64_t frame_count;        /**< Number of frames processed */
    int64_t command_count;      /**< Number of commands processed */

    /* Build info */
    ecs_build_info_t build_info; /**< Build info */
} EcsWorldSummary;

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
