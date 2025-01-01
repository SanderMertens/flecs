/**
 * @file query/engine/trivial_iter.h
 * @brief Trivial iterator functions.
 */

#include "../types.h"

/* Iterator for queries with trivial terms. */
bool flecs_query_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo,
    ecs_flags64_t field_set);

/* Iterator for queries with only trivial terms. */
bool flecs_query_is_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo);

/* Trivial test for constrained $this. */
bool flecs_query_trivial_test(
    const ecs_query_run_ctx_t *ctx,
    bool first,
    ecs_flags64_t field_set);
