/**
 * @file query/engine/trivial_iter.h
 * @brief Trivial iterator functions.
 */

#ifndef FLECS_QUERY_TRIVIAL_ITER_H
#define FLECS_QUERY_TRIVIAL_ITER_H

#include "../types.h"

void flecs_query_trivial_set_ids(
    ecs_query_run_ctx_t *ctx);

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

#ifdef FLECS_QUERY_PLANS

void flecs_query_trivial_set_iter_this(
    ecs_iter_t *it,
    const ecs_query_run_ctx_t *ctx);

/* Iterator for query plan operations with trivial terms. */
bool flecs_query_trivial_search(
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool redo,
    ecs_flags64_t field_set);

#endif // FLECS_QUERY_PLANS

#endif
