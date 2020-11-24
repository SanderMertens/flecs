#ifdef FLECS_STATS

#ifndef FLECS_STATS_H
#define FLECS_STATS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_world_stats_t {
    int32_t entity_count;
    int32_t component_count;
    int32_t query_count;
    int32_t system_count;
    int32_t table_count;
    int32_t empty_table_count;
    int32_t singleton_table_count;
    int32_t max_entities_per_table;
    int32_t max_components_per_table;
    int32_t max_columns_per_table;
    int32_t max_matched_queries_per_table;

    int32_t new_count;
    int32_t bulk_new_count;
    int32_t delete_count;
    int32_t clear_count;
    int32_t add_count;
    int32_t remove_count;
    int32_t set_count;
    int32_t discard_count;
} ecs_world_stats_t;

FLECS_API void ecs_get_world_stats(
    ecs_world_t *world,
    ecs_world_stats_t *stats);

FLECS_API void ecs_dump_world_stats(
    ecs_world_t *world,
    const ecs_world_stats_t *stats);

#ifdef __cplusplus
}
#endif

#endif

#endif
