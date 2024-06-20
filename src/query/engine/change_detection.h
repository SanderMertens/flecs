/**
 * @file query/engine/change_detection.h
 * @brief Query change detection functions.
 */

#include "../types.h"

/* Synchronize cache monitor with table dirty state */
void flecs_query_sync_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_match_t *match);

/* Mark iterated out fields dirty */
void flecs_query_mark_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it);

/* Compare cache monitor with table dirty state to detect changes */
bool flecs_query_check_table_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_table_t *table,
    int32_t term);

/* Mark out fields with fixed source dirty */
void flecs_query_mark_fixed_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it);

/* Synchronize fixed source monitor */
bool flecs_query_update_fixed_monitor(
    ecs_query_impl_t *impl);

/* Compare fixed source monitor */
bool flecs_query_check_fixed_monitor(
    ecs_query_impl_t *impl);
