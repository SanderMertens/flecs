/**
 * @file query/cache/cache_iter.h
 * @brief Cache iterator functions.
 */

#include "../types.h"

void flecs_query_cache_iter_init(
    ecs_iter_t *it,
    ecs_query_iter_t *qit,
    ecs_query_impl_t *impl);

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

bool flecs_query_is_trivial_cache_search(
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_is_trivial_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo);
