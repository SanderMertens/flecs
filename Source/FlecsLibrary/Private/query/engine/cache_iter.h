/**
 * @file query/engine/cache_iter.h
 * @brief Cache iterator functions.
 */

#include "../types.h"

/* Cache search */
bool flecs_query_cache_search(
    const ecs_query_run_ctx_t *ctx);

/* Cache search where entire query is cached */
bool flecs_query_is_cache_search(
    const ecs_query_run_ctx_t *ctx);

/* Cache test */
bool flecs_query_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo);

/* Cache test where entire query is cached */
bool flecs_query_is_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo);
