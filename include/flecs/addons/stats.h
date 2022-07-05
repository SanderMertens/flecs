/**
 * @file stats.h
 * @brief Statistics addon.
 *
 * The statistics addon enables an application to obtain detailed metrics about
 * the storage, systems and operations of a world.
 */

#ifdef FLECS_STATS

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
    ecs_gauge_t rate;                          /* Keep track of deltas too */
    float value[ECS_STAT_WINDOW];
} ecs_counter_t;

/* Make all metrics the same size, so we can iterate over fields */
typedef union ecs_metric_t {
    ecs_gauge_t gauge;
    ecs_counter_t counter;
} ecs_metric_t;

typedef struct ecs_world_stats_t {
    int32_t first_;

    ecs_metric_t entity_count;               /* Number of entities */
    ecs_metric_t entity_not_alive_count;     /* Number of not alive (recyclable) entity ids */

    /* Components and ids */
    ecs_metric_t id_count;                   /* Number of ids (excluding wildcards) */
    ecs_metric_t tag_id_count;               /* Number of tag ids (ids without data) */
    ecs_metric_t component_id_count;         /* Number of components ids (ids with data) */
    ecs_metric_t pair_id_count;              /* Number of pair ids */
    ecs_metric_t wildcard_id_count;          /* Number of wildcard ids */
    ecs_metric_t component_count;            /* Number of components  (non-zero sized types) */
    ecs_metric_t id_create_count;            /* Number of times id has been created */
    ecs_metric_t id_delete_count;            /* Number of times id has been deleted */

    /* Tables */
    ecs_metric_t table_count;                /* Number of tables */
    ecs_metric_t empty_table_count;          /* Number of empty tables */
    ecs_metric_t tag_table_count;            /* Number of tables with only tags */
    ecs_metric_t trivial_table_count;        /* Number of tables with only trivial components */
    ecs_metric_t table_record_count;         /* Number of table cache records */
    ecs_metric_t table_storage_count;        /* Number of table storages */
    ecs_metric_t table_create_count;         /* Number of times table has been created */
    ecs_metric_t table_delete_count;         /* Number of times table has been deleted */

    /* Queries & events */
    ecs_metric_t query_count;                /* Number of queries */
    ecs_metric_t observer_count;             /* Number of observers */
    ecs_metric_t system_count;               /* Number of systems */

    /* Deferred operations */
    ecs_metric_t new_count;
    ecs_metric_t bulk_new_count;
    ecs_metric_t delete_count;
    ecs_metric_t clear_count;
    ecs_metric_t add_count;
    ecs_metric_t remove_count;
    ecs_metric_t set_count;
    ecs_metric_t discard_count;

    /* Timing */
    ecs_metric_t world_time_total_raw;       /* Actual time passed since simulation start (first time progress() is called) */
    ecs_metric_t world_time_total;           /* Simulation time passed since simulation start. Takes into account time scaling */
    ecs_metric_t frame_time_total;           /* Time spent processing a frame. Smaller than world_time_total when load is not 100% */
    ecs_metric_t system_time_total;          /* Time spent on processing systems. */
    ecs_metric_t merge_time_total;           /* Time spent on merging deferred actions. */
    ecs_metric_t fps;                        /* Frames per second. */
    ecs_metric_t delta_time;                 /* Delta_time. */
    
    /* Frame data */
    ecs_metric_t frame_count_total;          /* Number of frames processed. */
    ecs_metric_t merge_count_total;          /* Number of merges executed. */
    ecs_metric_t pipeline_build_count_total; /* Number of system pipeline rebuilds (occurs when an inactive system becomes active). */
    ecs_metric_t systems_ran_frame;          /* Number of systems ran in the last frame. */

    /* OS API data */
    ecs_metric_t alloc_count;                /* Allocs per frame */
    ecs_metric_t realloc_count;              /* Reallocs per frame */
    ecs_metric_t free_count;                 /* Frees per frame */
    ecs_metric_t outstanding_alloc_count;    /* Difference between allocs & frees */

    int32_t last_;

    /** Current position in ringbuffer */
    int32_t t;
} ecs_world_stats_t;

/* Statistics for a single query (use ecs_query_stats_get) */
typedef struct ecs_query_stats_t {
    int32_t first_;
    ecs_metric_t matched_table_count;       /* Matched non-empty tables */    
    ecs_metric_t matched_empty_table_count; /* Matched empty tables */
    ecs_metric_t matched_entity_count;      /* Number of matched entities */
    int32_t last_;

    /** Current position in ringbuffer */
    int32_t t; 
} ecs_query_stats_t;

/** Statistics for a single system (use ecs_system_stats_get) */
typedef struct ecs_system_stats_t {
    int32_t first_;
    ecs_metric_t time_spent;       /* Time spent processing a system */
    ecs_metric_t invoke_count;     /* Number of times system is invoked */
    ecs_metric_t active;           /* Whether system is active (is matched with >0 entities) */
    ecs_metric_t enabled;          /* Whether system is enabled */
    int32_t last_;

    bool task;                     /* Is system a task */

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

    int32_t system_count; /* Number of systems in pipeline */
    int32_t active_system_count; /* Number of active systems in pipeline */
    int32_t rebuild_count; /* Number of times pipeline has rebuilt */
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

#endif
