/**
 * @file table_cache.h
 * @brief Data structure for fast table iteration/lookups.
 */

#ifndef FLECS_TABLE_CACHE_H_
#define FLECS_TABLE_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

void ecs_table_cache_init(
    ecs_table_cache_t *cache);

void ecs_table_cache_fini(
    ecs_table_cache_t *cache);

void ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *result);

void* ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *elem);

void* ecs_table_cache_get(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table);

bool ecs_table_cache_set_empty(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    bool empty);

bool ecs_table_cache_is_empty(
    const ecs_table_cache_t *cache);

#define ecs_table_cache_count(cache) (cache)->tables.count
#define ecs_table_cache_empty_count(cache) (cache)->empty_tables.count

void ecs_table_cache_fini_delete_all(
    ecs_world_t *world,
    ecs_table_cache_t *cache);

#ifdef __cplusplus
}
#endif

#endif
