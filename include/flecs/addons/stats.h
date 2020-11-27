#ifdef FLECS_STATS

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_STAT_WINDOW (60)

typedef struct ecs_stat_int32_t {
    int32_t avg[ECS_STAT_WINDOW];
    int32_t min[ECS_STAT_WINDOW];
    int32_t max[ECS_STAT_WINDOW];
} ecs_stat_int32_t;

typedef struct ecs_stat_float_t {
    float avg[ECS_STAT_WINDOW];
    float min[ECS_STAT_WINDOW];
    float max[ECS_STAT_WINDOW];
} ecs_stat_float_t;

typedef struct ecs_world_stats_t {
    ecs_stat_int32_t entity_count;                   /**< Number of entities */
    ecs_stat_int32_t component_count;                /**< Number of components */
    ecs_stat_int32_t query_count;                    /**< Number of queries */
    ecs_stat_int32_t system_count;                   /**< Number of systems */
    ecs_stat_int32_t table_count;                    /**< Number of tables */
    ecs_stat_int32_t empty_table_count;              /**< Number of empty tables */
    ecs_stat_int32_t singleton_table_count;          /**< Number of singleton tables. Singleton tables are tables with just a single entity that contains itself */
    ecs_stat_int32_t max_entities_per_table;         /**< Max number of entities per table */
    ecs_stat_int32_t max_components_per_table;       /**< Max number of components per table. This includes zero-sized components (tags) */
    ecs_stat_int32_t max_columns_per_table;          /**< Max number of columns per table. A column is a component with a non-zero size */
    ecs_stat_int32_t max_matched_queries_per_table;  /**< Max number of queries matched with a single table */
    ecs_stat_int32_t matched_entity_count;           /**< Number of entities matched by queries */
    ecs_stat_int32_t matched_table_count;            /**< Number of tables matched by queries */

    /* Total number of deferred operations */
    ecs_stat_int32_t new_count;
    ecs_stat_int32_t bulk_new_count;
    ecs_stat_int32_t delete_count;
    ecs_stat_int32_t clear_count;
    ecs_stat_int32_t add_count;
    ecs_stat_int32_t remove_count;
    ecs_stat_int32_t set_count;
    ecs_stat_int32_t discard_count;

    /* Previous values of deferred operations */
    int32_t prev_new_count;
    int32_t prev_bulk_new_count;
    int32_t prev_delete_count;
    int32_t prev_clear_count;
    int32_t prev_add_count;
    int32_t prev_remove_count;
    int32_t prev_set_count;
    int32_t prev_discard_count;

    /* Values from world_info_t */
    ecs_stat_float_t world_time_total_raw;
    ecs_stat_float_t world_time_total;
    ecs_stat_float_t frame_time_total;
    ecs_stat_float_t system_time_total;
    ecs_stat_float_t merge_time_total;
    ecs_stat_float_t fps;

    /* Previous values of world stats time values */
    float prev_world_time_total_raw;
    float prev_world_time_total;
    float prev_frame_time_total;
    float prev_system_time_total;
    float prev_merge_time_total;
    
    ecs_stat_int32_t frame_count_total;
    ecs_stat_int32_t merge_count_total;
    ecs_stat_int32_t pipeline_build_count_total;
    ecs_stat_int32_t systems_ran_frame;

    int32_t prev_frame_count_total;
    int32_t prev_merge_count_total;
    int32_t prev_pipeline_build_count_total;
    int32_t prev_systems_ran_frame;

    /* Current position in ringbuffer */
    int32_t t;
} ecs_world_stats_t;

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

#ifdef __cplusplus
}
#endif

#endif

#endif
