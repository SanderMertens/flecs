/**
 * @file query/cache/group.h
 * @brief Adding/removing tables to query groups
 */

ecs_query_cache_group_t* flecs_query_cache_get_group(
    const ecs_query_cache_t *cache,
    uint64_t group_id);

ecs_query_cache_match_t* flecs_query_cache_add_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table);

ecs_query_cache_match_t* flecs_query_cache_ensure_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table);

void flecs_query_cache_remove_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table);

void flecs_query_cache_remove_all_tables(
    ecs_query_cache_t *cache);

ecs_query_cache_table_t* flecs_query_cache_get_table(
    const ecs_query_cache_t *cache,
    ecs_table_t *table);

ecs_query_cache_match_t* flecs_query_cache_match_from_table(
    const ecs_query_cache_t *cache,
    const ecs_query_cache_table_t *qt);
