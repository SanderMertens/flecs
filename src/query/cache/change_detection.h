/**
 * @file query/cache/change_detection.h
 * @brief Query change detection implementation.
 */

void flecs_query_sync_match_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *match);

void flecs_query_mark_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it);

bool flecs_query_check_table_monitor(
    ecs_query_impl_t *impl,
    ecs_query_cache_match_t *qm,
    int32_t term);

void flecs_query_mark_fixed_fields_dirty(
    ecs_query_impl_t *impl,
    ecs_iter_t *it);

bool flecs_query_update_fixed_monitor(
    ecs_query_impl_t *impl);

bool flecs_query_check_fixed_monitor(
    ecs_query_impl_t *impl);
