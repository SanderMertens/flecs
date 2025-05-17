/**
 * @file query/cache/match.h
 * @brief Match table one or more times with query.
 */

void flecs_query_cache_match_fini(
    ecs_query_cache_t *cache,
    ecs_query_cache_match_t *qm);

bool flecs_query_cache_match_next(
    ecs_query_cache_t *cache,
    ecs_iter_t *it);
