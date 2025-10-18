/**
 * @file addons/stats.h
 * @brief Statistics addon.
 *
 * The stats addon tracks high resolution statistics for the world, systems and
 * pipelines. The addon can be used as an API where an application calls
 * functions to obtain statistics directly and as a module where statistics are
 * automatically tracked. The latter is required for statistics tracking in the
 * explorer.
 * 
 * When the addon is imported as module, statistics are tracked for each frame,
 * second, minute, hour, day and week with 60 datapoints per tier.
 */

#ifdef FLECS_STATS

/**
 * @defgroup c_addons_stats Stats
 * @ingroup c_addons
 * Collection of statistics for world, queries, systems and pipelines.
 *
 * @{
 */

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_PIPELINE
#define FLECS_PIPELINE
#endif

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

    /* Component ids */
    struct {
        ecs_metric_t tag_count;           /**< Number of tag ids (ids without data) */
        ecs_metric_t component_count;     /**< Number of components ids (ids with data) */
        ecs_metric_t pair_count;          /**< Number of pair ids */
        ecs_metric_t type_count;          /**< Number of registered types */
        ecs_metric_t create_count;        /**< Number of times id has been created */
        ecs_metric_t delete_count;        /**< Number of times id has been deleted */
    } components;

    /* Tables */
    struct {
        ecs_metric_t count;                /**< Number of tables */
        ecs_metric_t empty_count;          /**< Number of empty tables */
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
        ecs_metric_t ensure_count;
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

    /** Current position in ring buffer */
    int32_t t;
} ecs_world_stats_t;

/** Statistics for a single query (use ecs_query_cache_stats_get) */
typedef struct ecs_query_stats_t {
    int64_t first_;
    ecs_metric_t result_count;              /**< Number of query results */
    ecs_metric_t matched_table_count;       /**< Number of matched tables */
    ecs_metric_t matched_entity_count;      /**< Number of matched entities */
    int64_t last_;

    /** Current position in ringbuffer */
    int32_t t; 
} ecs_query_stats_t;

/** Statistics for a single system (use ecs_system_stats_get()) */
typedef struct ecs_system_stats_t {
    int64_t first_;
    ecs_metric_t time_spent;       /**< Time spent processing a system */
    int64_t last_;

    bool task;                     /**< Is system a task */

    ecs_query_stats_t query;
} ecs_system_stats_t;

/** Statistics for sync point */
typedef struct ecs_sync_stats_t {
    int64_t first_;
    ecs_metric_t time_spent;
    ecs_metric_t commands_enqueued;
    int64_t last_;

    int32_t system_count;
    bool multi_threaded;
    bool immediate;
} ecs_sync_stats_t;

/** Statistics for all systems in a pipeline. */
typedef struct ecs_pipeline_stats_t {
    /* Allow for initializing struct with {0} */
    int8_t canary_;

    /** Vector with system ids of all systems in the pipeline. The systems are
     * stored in the order they are executed. Merges are represented by a 0. */
    ecs_vec_t systems;

    /** Vector with sync point stats */
    ecs_vec_t sync_points;

    /** Current position in ring buffer */
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
void ecs_query_cache_stats_reduce(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src);

/** Reduce last measurement into previous measurement, restore old value. */
FLECS_API
void ecs_query_cache_stats_reduce_last(
    ecs_query_stats_t *stats,
    const ecs_query_stats_t *old,
    int32_t count);

/** Repeat last measurement. */
FLECS_API
void ecs_query_cache_stats_repeat_last(
    ecs_query_stats_t *stats);

/** Copy last measurement from source to destination. */
FLECS_API
void ecs_query_cache_stats_copy_last(
    ecs_query_stats_t *dst,
    const ecs_query_stats_t *src);

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

FLECS_API extern ECS_COMPONENT_DECLARE(FlecsStats);        /**< Flecs stats module. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldStats);     /**< Component id for EcsWorldStats. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldSummary);   /**< Component id for EcsWorldSummary. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsSystemStats);    /**< Component id for EcsSystemStats. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsPipelineStats);  /**< Component id for EcsPipelineStats. */

/* Memory statistics components */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_entities_memory_t);    /**< Component id for ecs_entities_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_component_index_memory_t); /**< Component id for ecs_component_index_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_query_memory_t);           /**< Component id for ecs_query_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_component_memory_t);       /**< Component id for ecs_component_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_table_memory_t);           /**< Component id for ecs_table_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_misc_memory_t);        /**< Component id for ecs_misc_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_table_histogram_t);        /**< Component id for ecs_table_histogram_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(ecs_allocator_memory_t); /**< Component id for ecs_allocator_memory_t. */
FLECS_API extern ECS_COMPONENT_DECLARE(EcsWorldMemory);               /**< Component id for EcsWorldMemory. */

FLECS_API extern ecs_entity_t EcsPeriod1s;                 /**< Tag used for metrics collected in last second. */
FLECS_API extern ecs_entity_t EcsPeriod1m;                 /**< Tag used for metrics collected in last minute. */
FLECS_API extern ecs_entity_t EcsPeriod1h;                 /**< Tag used for metrics collected in last hour. */
FLECS_API extern ecs_entity_t EcsPeriod1d;                 /**< Tag used for metrics collected in last day. */
FLECS_API extern ecs_entity_t EcsPeriod1w;                 /**< Tag used for metrics collected in last week. */

/** Common data for statistics. */
typedef struct {
    ecs_ftime_t elapsed;
    int32_t reduce_count;
} EcsStatsHeader;

/** Component that stores world statistics. */
typedef struct {
    EcsStatsHeader hdr;
    ecs_world_stats_t *stats;
} EcsWorldStats;

/** Component that stores system statistics. */
typedef struct {
    EcsStatsHeader hdr;
    ecs_map_t stats;
} EcsSystemStats;

/** Component that stores pipeline statistics. */
typedef struct {
    EcsStatsHeader hdr;
    ecs_map_t stats;
} EcsPipelineStats;

/** Component that stores a summary of world statistics. */
typedef struct {
    /* Time */
    double target_fps;          /**< Target FPS */
    double time_scale;          /**< Simulation time scale */
    double fps;                 /**< FPS */

    /* Totals */
    double frame_time_total;    /**< Total time spent processing a frame */
    double system_time_total;   /**< Total time spent in systems */
    double merge_time_total;    /**< Total time spent in merges */

    int64_t entity_count;
    int64_t table_count;
    int64_t frame_count;        /**< Number of frames processed */
    int64_t command_count;      /**< Number of commands processed */
    int64_t merge_count;        /**< Number of merges executed */

    int64_t systems_ran_total;
    int64_t observers_ran_total;
    int64_t queries_ran_total;

    int32_t tag_count;           /**< Number of tag (no data) ids in the world */
    int32_t component_count;     /**< Number of component (data) ids in the world */
    int32_t pair_count;          /**< Number of pair ids in the world */

    /* Per frame */
    double frame_time_frame;    /**< Time spent processing a frame */
    double system_time_frame;   /**< Time spent in systems */
    double merge_time_frame;    /**< Time spent in merges */

    int64_t merge_count_frame;
    int64_t systems_ran_frame;
    int64_t observers_ran_frame;
    int64_t queries_ran_frame;
    int64_t command_count_frame; /**< Number of commands processed in last frame */

    double simulation_time;     /**< Time spent in simulation */
    uint32_t uptime;            /**< Time since world was created */

    /* Build info */
    ecs_build_info_t build_info; /**< Build info */
} EcsWorldSummary;

/** Entity memory. */
typedef struct {
    int32_t alive_count;                 /** Number of alive entities. */
    int32_t not_alive_count;             /** Number of not alive entities. */
    ecs_size_t bytes_entity_index;       /** Bytes used by entity index. */
    ecs_size_t bytes_names;              /** Bytes used by names, symbols, aliases. */
    ecs_size_t bytes_doc_strings;        /** Bytes used by doc strings. */
} ecs_entities_memory_t;

/* Component memory. */
typedef struct {
    int32_t instances;                  /** Total number of component instances. */
    ecs_size_t bytes_table_components;  /** Bytes used by table columns. */
    ecs_size_t bytes_table_components_unused; /** Unused bytes in table columns. */
    ecs_size_t bytes_toggle_bitsets;    /** Bytes used in bitsets (toggled components). */
    ecs_size_t bytes_sparse_components; /** Bytes used in component sparse sets. */
} ecs_component_memory_t;

/** Component index memory. */
typedef struct {
    int32_t count;                      /** Number of component records. */
    ecs_size_t bytes_component_record;  /** Bytes used by ecs_component_record_t struct. */
    ecs_size_t bytes_table_cache;       /** Bytes used by table cache. */
    ecs_size_t bytes_name_index;        /** Bytes used by name index. */
    ecs_size_t bytes_ordered_children;  /** Bytes used by ordered children vector. */
    ecs_size_t bytes_reachable_cache;   /** Bytes used by reachable cache. */
} ecs_component_index_memory_t;

/** Query memory. */
typedef struct {
    int32_t count;                      /** Number of queries. */
    int32_t cached_count;               /** Number of queries with caches. */
    ecs_size_t bytes_query;             /** Bytes used by ecs_query_impl_t struct. */
    ecs_size_t bytes_cache;             /** Bytes used by query cache. */
    ecs_size_t bytes_group_by;          /** Bytes used by query cache groups (excludes cache elements). */
    ecs_size_t bytes_order_by;          /** Bytes used by table_slices. */
    ecs_size_t bytes_plan;              /** Bytes used by query plan. */
    ecs_size_t bytes_terms;             /** Bytes used by terms array. */
    ecs_size_t bytes_misc;              /** Bytes used by remaining misc arrays. */
} ecs_query_memory_t;

/** Table memory histogram constants */
#define ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT 14
#define ECS_TABLE_MEMORY_HISTOGRAM_MAX_COUNT (1 << ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT)

/** Table memory */
typedef struct {
    int32_t count;                      /** Total number of tables. */
    int32_t empty_count;                /** Number of empty tables. */
    int32_t column_count;               /** Number of table columns. */
    ecs_size_t bytes_table;             /** Bytes used by ecs_table_t struct. */
    ecs_size_t bytes_type;              /** Bytes used by type vector. */
    ecs_size_t bytes_entities;          /** Bytes used by entity vectors. */
    ecs_size_t bytes_overrides;         /** Bytes used by table overrides. */
    ecs_size_t bytes_columns;           /** Bytes used by table columns (excluding component data). */
    ecs_size_t bytes_table_records;     /** Bytes used by table records. */
    ecs_size_t bytes_column_map;        /** Bytes used by column map. */
    ecs_size_t bytes_component_map;     /** Bytes used by component map. */
    ecs_size_t bytes_dirty_state;       /** Bytes used by dirty state. */
    ecs_size_t bytes_edges;             /** Bytes used by table graph edges. */
} ecs_table_memory_t;

/** Table size histogram */
typedef struct {
    int32_t entity_counts[ECS_TABLE_MEMORY_HISTOGRAM_BUCKET_COUNT];
} ecs_table_histogram_t;

/** Misc memory */
typedef struct {
    ecs_size_t bytes_world;             /** Memory used by world and stages */
    ecs_size_t bytes_observers;         /** Memory used by observers. */
    ecs_size_t bytes_systems;           /** Memory used by systems (excluding system queries). */
    ecs_size_t bytes_pipelines;         /** Memory used by pipelines (excluding pipeline queries). */
    ecs_size_t bytes_table_lookup;      /** Bytes used for table lookup data structures. */
    ecs_size_t bytes_component_record_lookup; /** Bytes used for component record lookup data structures. */
    ecs_size_t bytes_type_info;         /** Bytes used for storing type information. */
    ecs_size_t bytes_commands;          /** Command queue */
    ecs_size_t bytes_rematch_monitor;   /** Memory used by monitor used to track rematches */
    ecs_size_t bytes_component_ids;     /** Memory used for mapping global to world-local component ids. */
    ecs_size_t bytes_reflection;        /** Memory used for component reflection not tracked elsewhere. */
    ecs_size_t bytes_stats;             /** Memory used for statistics tracking not tracked elsewhere. */
    ecs_size_t bytes_rest;              /** Memory used by REST HTTP server */
} ecs_misc_memory_t;

/** Allocator memory.
 * Returns memory that's allocated by allocators but not in use. */
typedef struct {
    ecs_size_t bytes_graph_edge;        /** Graph edge allocator. */
    ecs_size_t bytes_component_record;  /** Component record allocator. */
    ecs_size_t bytes_pair_record;       /** Pair record allocator. */
    ecs_size_t bytes_table_diff;        /** Table diff allocator. */
    ecs_size_t bytes_sparse_chunk;      /** Sparse chunk allocator. */
    ecs_size_t bytes_allocator;         /** Generic allocator. */
    ecs_size_t bytes_stack_allocator;   /** Stack allocator. */
    ecs_size_t bytes_cmd_entry_chunk;   /** Command batching entry chunk allocator. */
    ecs_size_t bytes_query_impl;        /** Query struct allocator. */
    ecs_size_t bytes_query_cache;       /** Query cache struct allocator. */
    ecs_size_t bytes_misc;              /** Miscalleneous allocators */
} ecs_allocator_memory_t;

/** Component with memory statistics. */
typedef struct {
    ecs_entities_memory_t entities;
    ecs_component_memory_t components;
    ecs_component_index_memory_t component_index;
    ecs_query_memory_t queries;
    ecs_table_memory_t tables;
    ecs_table_histogram_t table_histogram;
    ecs_misc_memory_t misc;
    ecs_allocator_memory_t allocators;
    double collection_time; /** Time spent collecting statistics. */
} EcsWorldMemory;

/** Memory statistics getters. */
/** Get memory usage statistics for the entity index.
 * 
 * @param world The world.
 * @return Memory statistics for the entity index.
 */
FLECS_API
ecs_entities_memory_t ecs_entity_memory_get(
    const ecs_world_t *world);

/** Get memory usage statistics for single component record.
 * 
 * @param cr The component record.
 * @param result Memory statistics for component record (out).
 */
void ecs_component_record_memory_get(
    const ecs_component_record_t *cr,
    ecs_component_index_memory_t *result);

/** Get memory usage statistics for the component index.
 * 
 * @param world The world.
 * @return Memory statistics for the component index.
 */
FLECS_API
ecs_component_index_memory_t ecs_component_index_memory_get(
    const ecs_world_t *world);

/** Get memory usage statistics for single query.
 * 
 * @param query The query.
 * @param result Memory statistics for query (out).
 */
FLECS_API
void ecs_query_memory_get(
    const ecs_query_t *query,
    ecs_query_memory_t *result);

/** Get memory usage statistics for queries.
 * 
 * @param world The world.
 * @return Memory statistics for queries.
 */
FLECS_API
ecs_query_memory_t ecs_queries_memory_get(
    const ecs_world_t *world);

/** Get component memory for table.
 *
 * @param table The table.
 * @param result The memory used by components stored in this table (out).
 */
void ecs_table_component_memory_get(
    const ecs_table_t *table,
    ecs_component_memory_t *result);

/** Get memory usage statistics for components.
 * 
 * @param world The world.
 * @return Memory statistics for components.
 */
FLECS_API
ecs_component_memory_t ecs_component_memory_get(
    const ecs_world_t *world);

/** Get memory usage statistics for single table.
 *
 * @param table The table.
 * @param result Memory statistics for table (out).
 */
void ecs_table_memory_get(
    const ecs_table_t *table,
    ecs_table_memory_t *result);

/** Get memory usage statistics for tables.
 * 
 * @param world The world.
 * @return Memory statistics for tables.
 */
FLECS_API
ecs_table_memory_t ecs_tables_memory_get(
    const ecs_world_t *world);

/** Get number of tables by number of entities in the table.
 * 
 * @param world The world.
 * @return Number of tables by number of entities in the table.
 */
FLECS_API
ecs_table_histogram_t ecs_table_histogram_get(
    const ecs_world_t *world);

/** Get memory usage statistics for commands.
 * 
 * @param world The world.
 * @return Memory statistics for commands.
 */
FLECS_API
ecs_misc_memory_t ecs_misc_memory_get(
    const ecs_world_t *world);

/** Get memory usage statistics for allocators.
 * 
 * @param world The world.
 * @return Memory statistics for allocators.
 */
FLECS_API
ecs_allocator_memory_t ecs_allocator_memory_get(
    const ecs_world_t *world);

/** Get total memory used by world.
 * 
 * @param world The world.
 */
FLECS_API
ecs_size_t ecs_memory_get(
    const ecs_world_t *world);


/** Stats module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsStats)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsStatsImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
