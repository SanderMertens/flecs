/**
 * @file query/cache/match.h
 * @brief Cache match lifecycle: finalize and advance through iterator results.
 */

void flecs_query_cache_match_fini(
    ecs_query_cache_t *cache,
    ecs_query_cache_match_t *qm);

bool flecs_query_cache_match_next(
    ecs_query_cache_t *cache,
    ecs_iter_t *it);
