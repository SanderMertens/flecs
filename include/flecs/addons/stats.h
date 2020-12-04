#ifdef FLECS_STATS

#define FLECS_SYSTEM

#include "../modules/system.h"

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STAT_WINDOW (60)

/** Simple value that indicates current state */
typedef struct ecs_gauge_t {
    float avg[ECS_STAT_WINDOW];
    float min[ECS_STAT_WINDOW];
    float max[ECS_STAT_WINDOW];
} ecs_gauge_t;

/* Monotonically increasing counter */
typedef struct ecs_counter_t {
    ecs_gauge_t rate;                          /**< Keep track of deltas too */
    float value[ECS_STAT_WINDOW];
} ecs_counter_t;

typedef struct ecs_world_stats_t {
    /* Allows struct to be initialized with {0} */
    int32_t dummy_;

    ecs_gauge_t entity_count;                 /**< Number of entities */
    ecs_gauge_t component_count;              /**< Number of components */
    ecs_gauge_t query_count;                  /**< Number of queries */
    ecs_gauge_t system_count;                 /**< Number of systems */
    ecs_gauge_t table_count;                  /**< Number of tables */
    ecs_gauge_t empty_table_count;            /**< Number of empty tables */
    ecs_gauge_t singleton_table_count;        /**< Number of singleton tables. Singleton tables are tables with just a single entity that contains itself */
    ecs_gauge_t matched_entity_count;         /**< Number of entities matched by queries */
    ecs_gauge_t matched_table_count;          /**< Number of tables matched by queries */

    /* Deferred operations */
    ecs_counter_t new_count;
    ecs_counter_t bulk_new_count;
    ecs_counter_t delete_count;
    ecs_counter_t clear_count;
    ecs_counter_t add_count;
    ecs_counter_t remove_count;
    ecs_counter_t set_count;
    ecs_counter_t discard_count;

    /* Timing */
    ecs_counter_t world_time_total_raw;       /**< Actual time passed since simulation start (first time progress() is called) */
    ecs_counter_t world_time_total;           /**< Simulation time passed since simulation start. Takes into account time scaling */
    ecs_counter_t frame_time_total;           /**< Time spent processing a frame. Smaller than world_time_total when load is not 100% */
    ecs_counter_t system_time_total;          /**< Time spent on processing systems. */
    ecs_counter_t merge_time_total;           /**< Time spent on merging deferred actions. */
    ecs_gauge_t fps;                          /**< Frames per second. */
    ecs_gauge_t delta_time;                   /**< Delta_time. */
    
    /* Frame data */
    ecs_counter_t frame_count_total;          /**< Number of frames processed. */
    ecs_counter_t merge_count_total;          /**< Number of merges executed. */
    ecs_counter_t pipeline_build_count_total; /**< Number of system pipeline rebuilds (occurs when an inactive system becomes active). */
    ecs_counter_t systems_ran_frame;          /**< Number of systems ran in the last frame. */

    /** Current position in ringbuffer */
    int32_t t;
} ecs_world_stats_t;

/* Statistics for a single query (use ecs_get_query_stats) */
typedef struct ecs_query_stats_t {
    ecs_gauge_t matched_table_count;       /**< Number of matched non-empty tables. This is the number of tables 
                                            * iterated over when evaluating a query. */    

    ecs_gauge_t matched_empty_table_count; /**< Number of matched empty tables. Empty tables are not iterated over when
                                            * evaluating a query. */
    
    ecs_gauge_t matched_entity_count;      /**< Number of matched entities across all tables */

    /** Current position in ringbuffer */
    int32_t t; 
} ecs_query_stats_t;

/** Statistics for a single system (use ecs_get_system_stats) */
typedef struct ecs_system_stats_t {
    ecs_query_stats_t query_stats;
    ecs_counter_t time_spent;       /**< Time spent processing a system */
    ecs_counter_t invoke_count;     /**< Number of times system is invoked */
    ecs_gauge_t active;             /**< Whether system is active (is matched with >0 entities) */
    ecs_gauge_t enabled;            /**< Whether system is enabled */
} ecs_system_stats_t;

/** Statistics for all systems in a pipeline. */
typedef struct ecs_pipeline_stats_t {
    /** Vector with system ids of all systems in the pipeline. The systems are
     * stored in the order they are executed. Merges are represented by a 0. */
    ecs_vector_t *systems;

    /** Map with system statistics. For each system in the systems vector, an
     * entry in the map exists of type ecs_system_stats_t. */
    ecs_map_t *system_stats;
} ecs_pipeline_stats_t;

/** Get world statistics.
 * Obtain statistics for the provided world. This operation loops several times
 * over the tables in the world, and can impact application performance.
 *
 * @param world The world.
 * @param stats Out parameter for statistics.
 */
FLECS_API void ecs_get_world_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats);

/** Print world statistics.
 * Print statistics obtained by ecs_get_world_statistics and in the
 * ecs_world_info_t struct.
 * 
 * @param world The world.
 * @param stats The statistics to print.
 */
FLECS_API void ecs_dump_world_stats(
    ecs_world_t *world,
    const ecs_world_stats_t *stats);

/** Get query statistics.
 * Obtain statistics for the provided query.
 *
 * @param world The world.
 * @param query The query.
 * @param stats Out parameter for statistics.
 */
FLECS_API void ecs_get_query_stats(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_stats_t *s);

/** Get system statistics.
 * Obtain statistics for the provided system.
 *
 * @param world The world.
 * @param system The system.
 * @param stats Out parameter for statistics.
 * @return true if success, false if not a system.
 */
FLECS_API bool ecs_get_system_stats(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *stats);

/** Get pipeline statistics.
 * Obtain statistics for the provided pipeline.
 *
 * @param world The world.
 * @param pipeline The pipeline.
 * @param stats Out parameter for statistics.
 * @return true if success, false if not a pipeline.
 */
FLECS_API bool ecs_get_pipeline_stats(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_pipeline_stats_t *stats);

/** Measure frame time. 
 * Frame time measurements measure the total time passed in a single frame, and 
 * how much of that time was spent on systems and on merging.
 *
 * Frame time measurements add a small constant-time overhead to an application.
 * When an application sets a target FPS, frame time measurements are enabled by
 * default.
 *
 * @param world The world.
 * @param enable Whether to enable or disable frame time measuring.
 */
FLECS_API void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable);

/** Measure system time. 
 * System time measurements measure the time spent in each system.
 *
 * System time measurements add overhead to every system invocation and 
 * therefore have a small but measurable impact on application performance.
 * System time measurements must be enabled before obtaining system statistics.
 *
 * @param world The world.
 * @param enable Whether to enable or disable system time measuring.
 */
FLECS_API void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable);

FLECS_API void ecs_gauge_reduce(
    ecs_gauge_t *dst,
    int32_t t_dst,
    ecs_gauge_t *src,
    int32_t t_src);

#ifdef __cplusplus
}
#endif

#endif

#endif
