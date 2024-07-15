/**
 * @file query/engine/cache_iter.h
 * @brief Cache iterator functions.
 */

#include "../types.h"

/* Cache search without data */
bool flecs_query_cache_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx);

/* Cache search with data */
bool flecs_query_cache_data_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx);

/* Cache search without data where entire query is cached */
bool flecs_query_is_cache_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx);

/* Cache search with data where entire query is cached */
bool flecs_query_is_cache_data_search(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx);

/* Cache test without data */
bool flecs_query_cache_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first);

/* Cache test without data where entire query is cached */
bool flecs_query_is_cache_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first);

/* Cache test with data */
bool flecs_query_cache_data_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first);

/* Cache test with data where entire query is cached */
bool flecs_query_is_cache_data_test(
    const ecs_query_impl_t *impl,
    const ecs_query_run_ctx_t *ctx,
    bool first);
