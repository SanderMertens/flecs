/**
 * @file query/engine/trav_cache.h
 * @brief Caches for relationship traversal (up/down) during query evaluation.
 */

#include "../types.h"

void flecs_query_get_trav_down_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t entity);

void flecs_query_get_trav_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_table_t *table);

void flecs_query_trav_cache_fini(
    ecs_allocator_t *a,
    ecs_trav_cache_t *cache);

ecs_trav_down_t* flecs_query_get_down_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_component_record_t *cr_with,
    bool self,
    bool empty);

void flecs_query_down_cache_fini(
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache);

ecs_trav_up_t* flecs_query_get_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t with,
    ecs_entity_t trav,
    ecs_component_record_t *cr_with,
    ecs_component_record_t *cr_trav);

void flecs_query_up_cache_fini(
    ecs_trav_up_cache_t *cache);
