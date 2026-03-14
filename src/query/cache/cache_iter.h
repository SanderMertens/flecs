/**
 * @file query/cache/cache_iter.h
 * @brief Cache iteration: search all matches or test a specific table.
 */

#include "../types.h"

void flecs_query_cache_iter_init(
    ecs_iter_t *it,
    ecs_query_iter_t *qit,
    ecs_query_impl_t *impl);

bool flecs_query_cache_search(
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_is_cache_search(
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo);

bool flecs_query_is_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo);

bool flecs_query_is_trivial_cache_search(
    const ecs_query_run_ctx_t *ctx);

bool flecs_query_is_trivial_cache_test(
    const ecs_query_run_ctx_t *ctx,
    bool redo);
