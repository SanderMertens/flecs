/**
 * @file addons/stats.h
 * @brief Statistics addon.
 *
 * The statistics addon enables an application to obtain detailed metrics about
 * the storage, systems and operations of a world.
 */

#ifdef FLECS_STATS

/**
 * @defgroup c_addons_stats Stats
 * @brief Collection of statistics for world, queries, systems and pipelines.
 * 
 * \ingroup c_addons
 * @{
 */

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STAT_WINDOW (60)

/** Simple value that indicates current state */
typedef struct ecs_gauge_t {
    ecs_float_t avg[ECS_STAT_WINDOW];
    ecs_float_t min[ECS_STAT_WINDOW];
    ecs_float_t max[ECS_STAT_WINDOW];
} ecs_gauge_t;

/** Monotonically increasing counter */
typedef struct ecs_counter_t {
    ecs_gauge_t rate;                     /**< Keep track of deltas too */
    double value[ECS_STAT_WINDOW];
} ecs_counter_t;

/** Make all metrics the same size, so we can iterate over fields */
typedef union ecs_metric_t {
    ecs_gauge_t gauge;
    ecs_counter_t counter;
} ecs_metric_t;

typedef struct ecs_world_stats_t {
    int64_t first_;

    /* Entities */
    struct {
        ecs_metric_t count;               /**< Number of entities */
        ecs_metric_t not_alive_count;     /**< Number of not alive (recyclable) entity ids */
    } entities;

    /* Components and ids */
    struct {
        ecs_metric_t count;               /**< Number of ids (excluding wildcards) */
        ecs_metric_t tag_count;           /**< Number of tag ids (ids without data) */
        ecs_metric_t component_count;     /**< Number of components ids (ids with data) */
        ecs_metric_t pair_count;          /**< Number of pair ids */
        ecs_metric_t wildcard_count;      /**< Number of wildcard ids */
        ecs_metric_t type_count;          /**< Number of registered types */
        ecs_metric_t create_count;        /**< Number of times id has been created */
        ecs_metric_t delete_count;        /**< Number of times id has been deleted */
    } ids;

    /* Tables */
    struct {
        ecs_metric_t count;                /**< Number of tables */
        ecs_metric_t empty_count;          /**< Number of empty tables */
        ecs_metric_t tag_only_count;       /**< Number of tables with only tags */
        ecs_metric_t trivial_only_count;   /**< Number of tables with only trivial components */
        ecs_metric_t record_count;         /**< Number of table cache records */
        ecs_metric_t storage_count;        /**< Number of table storages */
        ecs_metric_t create_count;         /**< Number of times table has been created */
        ecs_metric_t delete_count;         /**< Number of times table has been deleted */
    } tables;

    /* Queries & events */
    struct {
        ecs_metric_t query_count;          /**< Number of queries */
        ecs_metric_t observer_count;       /**< Number of observers */
        ecs_metric_t system_count;         /**< Number of systems */
    } queries;

    /* Commands */
    struct {
        ecs_metric_t add_count;
        ecs_metric_t remove_count;
        ecs_metric_t delete_count;
        ecs_metric_t clear_count;
        ecs_metric_t set_count;
        ecs_metric_t get_mut_count;
        ecs_metric_t modified_count;
        ecs_metric_t other_count;
        ecs_metric_t discard_count;
        ecs_metric_t batched_entity_count;
        ecs_metric_t batched_count;
    } commands;

    /* Frame data */
    struct {
        ecs_metric_t frame_count;          /**< Number of frames processed. */
        ecs_metric_t merge_count;          /**< Number of merges executed. */
        ecs_metric_t rematch_count;        /**< Number of query rematches */
        ecs_metric_t pipeline_build_count; /**< Number of system pipeline rebuilds (occurs when an inactive system becomes active). */
        ecs_metric_t systems_ran;          /**< Number of systems ran. */
        ecs_metric_t observers_ran;        /**< Number of times an observer was invoked. */
        ecs_metric_t event_emit_count;     /**< Number of events emitted */
    } frame;

    /* Timing */
    struct {
        ecs_metric_t world_time_raw;       /**< Actual time passed since simulation start (first time progress() is called) */
        ecs_metric_t world_time;           /**< Simulation time passed since simulation start. Takes into account time scaling */
        ecs_metric_t frame_time;           /**< Time spent processing a frame. Smaller than world_time_total when load is not 100% */
        ecs_metric_t system_time;          /**< Time spent on running systems. */
        ecs_metric_t emit_time;            /**< Time spent on notifying observers. */
        ecs_metric_t merge_time;           /**< Time spent on merging commands. */
        ecs_metric_t rematch_time;         /**< Time spent on rematching. */
        ecs_metric_t fps;                  /**< Frames per second. */
        ecs_metric_t delta_time;           /**< Delta_time. */
    } performance;

    struct {
        /* Memory allocation data */
        ecs_metric_t alloc_count;          /**< Allocs per frame */
        ecs_metric_t realloc_count;        /**< Reallocs per frame */
        ecs_metric_t free_count;           /**< Frees per frame */
        ecs_metric_t outstanding_alloc_count; /**< Difference between allocs & frees */

        /* Memory allocator data */
        ecs_metric_t block_alloc_count;    /**< Block allocations per frame */
        ecs_metric_t block_free_count;     /**< Block frees per frame */
        ecs_metric_t block_outstanding_alloc_count; /**< Difference between allocs & frees */
        ecs_metric_t stack_alloc_count;    /**< Page allocations per frame */
        ecs_metric_t stack_free_count;     /**< Page frees per frame */
        ecs_metric_t stack_outstanding_alloc_count; /**< Difference between allocs & frees */
    } memory;

    /* REST statistics */
    struct {
        ecs_metric_t request_count;
        ecs_metric_t entity_count;
        ecs_metric_t entity_error_count;
        ecs_metric_t query_count;
        ecs_metric_t query_error_count;
        ecs_metric_t query_name_count;
        ecs_metric_t query_name_error_count;
        ecs_metric_t query_name_from_cache_count;
        ecs_metric_t enable_count;
        ecs_metric_t enable_error_count;
        ecs_metric_t world_stats_count;
        ecs_metric_t pipeline_stats_count;
        ecs_metric_t stats_error_count;
    } rest;

    /* HTTP statistics */
    struct {
        ecs_metric_t request_received_count;
        ecs_metric_t request_invalid_count;
        ecs_metric_t request_handled_ok_count;
        ecs_metric_t request_handled_error_count;
        ecs_metric_t request_not_handled_count;
        ecs_metric_t request_preflight_count;
        ecs_metric_t send_ok_count;
        ecs_metric_t send_error_count;
        ecs_metric_t busy_count;
    } http;

    int64_t last_;

    /** Current position in ringbuffer */
    int32_t t;
} ecs_world_stats_t;

/** Statistics for a single query (use ecs_query_stats_get) */
typedef struct ecs_query_stats_t {
    int64_t first_;
    ecs_metric_t matched_table_count;       /**< Matched non-empty tables */    
    ecs_metric_t matched_empty_table_count; /**< Matched empty tables */
    ecs_metric_t matched_entity_count;      /**< Number of matched entities */
    int64_t last_;

    /** Current position in ringbuffer */
    int32_t t; 
} ecs_query_stats_t;

/** Statistics for a single system (use ecs_system_stats_get) */
typedef struct ecs_system_stats_t {
    int64_t first_;
    ecs_metric_t time_spent;       /**< Time spent processing a system */
    ecs_metric_t invoke_count;     /**< Number of times system is invoked */
    ecs_metric_t active;           /**< Whether system is active (is matched with >0 entities) */
    ecs_metric_t enabled;          /**< Whether system is enabled */
    int64_t last_;

    bool task;                     /**< Is system a task */

    ecs_query_stats_t query;
} ecs_system_stats_t;

/** Statistics for all systems in a pipeline. */
typedef struct ecs_pipeline_stats_t {
    /** Vector with system ids of all systems in the pipeline. The systems are
     * stored in the order they are executed. Merges are represented by a 0. */
    ecs_vector_t *systems;

    /** Map with system statistics. For each system in the systems vector, an
     * entry in the map exists of type ecs_system_stats_t. */
    ecs_map_t system_stats;

    /** Current position in ringbuffer */
    int32_t t;

    int32_t system_count;        /**< Number of systems in pipeline */
    int32_t active_system_count; /**< Number of active systems in pipeline */
    int32_t rebuild_count;       /**< Number of times pipeline has rebuilt */
} ecs_pipeline_stats_t;

/** Get world statistics.
 *
 * @param world The world.
 * @param stats Out parameter for statistics.
 */
FLECS_API 
void ecs_world_stats_get(
    const ecs_world_t *world,
    ecs_world_stats_t *stats);

/** Reduce source measurement window into single destination measurement. */
FLECS_API 
void ecs_world_stats_reduce(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_world_stats_reduce_last(
    ecs_world_stats_t *stats,
    const ecs_world_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_world_stats_repeat_last(
    ecs_world_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_world_stats_copy_last(
    ecs_world_stats_t *dst,
    const ecs_world_stats_t *src);

FLECS_API 
void ecs_world_stats_log(
    const ecs_world_t *world,
    const ecs_world_stats_t *stats);

/** Get query statistics.
 * Obtain statistics for the provided query.
 *
 * @param world The world.
 * @param query The query.
 * @param stats Out parameter for statistics.
 */
FLECS_API 
void ecs_query_stats_get(
    const ecs_world_t *world,
    const ecs_query_t *query,
    ecs_query_stats_t *stats);

/** Reduce source measurement window into single destination measurement. */
FLECS_API 
void ecs_query_stats_reduce(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_query_stats_reduce_last(
    ecs_query_stats_t *stats,
    const ecs_query_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_query_stats_repeat_last(
    ecs_query_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_query_stats_copy_last(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src);

#ifdef FLECS_SYSTEM
/** Get system statistics.
 * Obtain statistics for the provided system.
 *
 * @param world The world.
 * @param system The system.
 * @param stats Out parameter for statistics.
 * @return true if success, false if not a system.
 */
FLECS_API 
bool ecs_system_stats_get(
    const ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *stats);

/** Reduce source measurement window into single destination measurement */
FLECS_API 
void ecs_system_stats_reduce(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_system_stats_reduce_last(
    ecs_system_stats_t *stats,
    const ecs_system_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_system_stats_repeat_last(
    ecs_system_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_system_stats_copy_last(
    ecs_system_stats_t *dst,
    const ecs_system_stats_t *src);
#endif

#ifdef FLECS_PIPELINE
/** Get pipeline statistics.
 * Obtain statistics for the provided pipeline.
 *
 * @param world The world.
 * @param pipeline The pipeline.
 * @param stats Out parameter for statistics.
 * @return true if success, false if not a pipeline.
 */
FLECS_API 
bool ecs_pipeline_stats_get(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_pipeline_stats_t *stats);

/** Free pipeline stats.
 * 
 * @param stats The stats to free.
 */
FLECS_API
void ecs_pipeline_stats_fini(
    ecs_pipeline_stats_t *stats);

/** Reduce source measurement window into single destination measurement */
FLECS_API 
void ecs_pipeline_stats_reduce(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_pipeline_stats_reduce_last(
    ecs_pipeline_stats_t *stats,
    const ecs_pipeline_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_pipeline_stats_repeat_last(
    ecs_pipeline_stats_t *stats);

/** Copy last measurement to destination.
 * This operation copies the last measurement into the destination. It does not
 * modify the cursor.
 * 
 * @param dst The metrics.
 * @param src The metrics to copy.
 */
FLECS_API
void ecs_pipeline_stats_copy_last(
    ecs_pipeline_stats_t *dst,
    const ecs_pipeline_stats_t *src);

#endif

/** Reduce all measurements from a window into a single measurement. */
FLECS_API 
void ecs_metric_reduce(
    ecs_metric_t *dst,
    const ecs_metric_t *src,
    int32_t t_dst,
    int32_t t_src);

/** Reduce last measurement into previous measurement */
FLECS_API
void ecs_metric_reduce_last(
    ecs_metric_t *m,
    int32_t t,
    int32_t count);

/** Copy measurement */
FLECS_API
void ecs_metric_copy(
    ecs_metric_t *m,
    int32_t dst,
    int32_t src);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
