 /**
 * @file query/engine/cache.h
 * @brief Query cache functions.
 */

#include "../types.h"

/* Create query cache */
ecs_query_cache_t* flecs_query_cache_init(
    ecs_query_impl_t *impl,
    const ecs_query_desc_t *desc);

/* Destroy query cache */
void flecs_query_cache_fini(
    ecs_query_impl_t *impl);

/* Get cache entry for table */
ecs_query_cache_table_t* flecs_query_cache_get_table(
    const ecs_query_cache_t *query,
    const ecs_table_t *table);

/* Sort tables (order_by implementation) */
void flecs_query_cache_sort_tables(
    ecs_world_t *world,
    ecs_query_impl_t *impl);

void flecs_query_cache_build_sorted_tables(
    ecs_query_cache_t *cache);

/* Return number of tables in cache */
int32_t flecs_query_cache_table_count(
    ecs_query_cache_t *cache);

/* Return number of entities in cache (requires iterating tables) */
int32_t flecs_query_cache_entity_count(
    const ecs_query_cache_t *cache);

bool flecs_query_cache_is_trivial(
    ecs_query_cache_t *cache);

/* Return whether query has trivial cache */
bool flecs_query_has_trivial_cache(
    const ecs_query_t *query);
