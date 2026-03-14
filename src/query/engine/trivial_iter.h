/**
 * @file query/engine/trivial_iter.h
 * @brief Fast-path iterators for simple queries (no wildcards, no traversal).
 */

#include "../types.h"

bool flecs_query_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo,
    ecs_flags64_t field_set);

bool flecs_query_is_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo);

bool flecs_query_trivial_test(
    const ecs_query_run_ctx_t *ctx,
    bool first,
    ecs_flags64_t field_set);
