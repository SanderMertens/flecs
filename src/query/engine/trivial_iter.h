/**
 * @file query/engine/trivial_iter.h
 * @brief Trivial iterator functions.
 */

#include "../types.h"

/* Iterator for trivial queries. */
bool flecs_query_trivial_search(
    const ecs_query_impl_t *query,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool first,
    ecs_flags64_t field_set);

/* Iterator for trivial queries. */
bool flecs_query_trivial_search_nodata(
    const ecs_query_impl_t *query,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool first,
    ecs_flags64_t field_set);

/* Iterator for trivial queries with wildcard matching. */
bool flecs_query_trivial_search_w_wildcards(
    const ecs_query_impl_t *query,
    const ecs_query_run_ctx_t *ctx,
    ecs_query_trivial_ctx_t *op_ctx,
    bool first,
    ecs_flags64_t field_set);

/* Trivial test for constrained $this. */
bool flecs_query_trivial_test(
    const ecs_query_impl_t *query,
    const ecs_query_run_ctx_t *ctx,
    bool first,
    ecs_flags64_t field_set);

/* Trivial test for constrained $this with wildcard matching. */
bool flecs_query_trivial_test_w_wildcards(
    const ecs_query_impl_t *query,
    const ecs_query_run_ctx_t *ctx,
    bool first,
    ecs_flags64_t field_set);
