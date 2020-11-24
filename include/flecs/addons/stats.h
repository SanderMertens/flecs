#ifdef FLECS_STATS

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_world_stats_t {
    int32_t entity_count;                   /**< Number of entities */
    int32_t component_count;                /**< Number of components */
    int32_t query_count;                    /**< Number of queries */
    int32_t system_count;                   /**< Number of systems */
    int32_t table_count;                    /**< Number of tables */
    int32_t empty_table_count;              /**< Number of empty tables */
    int32_t singleton_table_count;          /**< Number of singleton tables. Singleton tables are tables with just a single entity that contains itself */
    int32_t max_entities_per_table;         /**< Max number of entities per table */
    int32_t max_components_per_table;       /**< Max number of components per table. This includes zero-sized components (tags) */
    int32_t max_columns_per_table;          /**< Max number of columns per table. A column is a component with a non-zero size */
    int32_t max_matched_queries_per_table;  /**< Max number of queries matched with a single table */

    /* Total number of deferred operations */
    int32_t new_count;
    int32_t bulk_new_count;
    int32_t delete_count;
    int32_t clear_count;
    int32_t add_count;
    int32_t remove_count;
    int32_t set_count;
    int32_t discard_count;
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
