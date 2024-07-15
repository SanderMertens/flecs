/**
 * @file query/engine/trav_cache.h
 * @brief Traversal cache functions
 */

#include "../types.h"

/* Traversal cache for transitive queries. Finds all reachable entities by
 * following a relationship */

/* Find all entities when traversing downwards */
void flecs_query_get_trav_down_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t entity);

/* Find all entities when traversing upwards */
void flecs_query_get_trav_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_cache_t *cache,
    ecs_entity_t trav,
    ecs_table_t *table);

/* Free traversal cache */
void flecs_query_trav_cache_fini(
    ecs_allocator_t *a,
    ecs_trav_cache_t *cache);

/* Traversal caches for up traversal. Enables searching upwards until an entity
 * with the queried for id has been found. */

/* Traverse downwards from starting entity to find all tables for which the 
 * specified entity is the source of the queried for id ('with'). */
ecs_trav_down_t* flecs_query_get_down_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_entity_t trav,
    ecs_entity_t entity,
    ecs_id_record_t *idr_with,
    bool self,
    bool empty);

/* Free down traversal cache */
void flecs_query_down_cache_fini(
    ecs_allocator_t *a,
    ecs_trav_up_cache_t *cache);

ecs_trav_up_t* flecs_query_get_up_cache(
    const ecs_query_run_ctx_t *ctx,
    ecs_trav_up_cache_t *cache,
    ecs_table_t *table,
    ecs_id_t with,
    ecs_entity_t trav,
    ecs_id_record_t *idr_with,
    ecs_id_record_t *idr_trav);

/* Free up traversal cache */
void flecs_query_up_cache_fini(
    ecs_trav_up_cache_t *cache);
