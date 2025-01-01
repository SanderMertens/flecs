/**
 * @file storage/table_cache.h
 * @brief Data structure for fast table iteration/lookups.
 */

#ifndef FLECS_TABLE_CACHE_H_
#define FLECS_TABLE_CACHE_H_

void ecs_table_cache_init(
    ecs_world_t *world,
    ecs_table_cache_t *cache);

void ecs_table_cache_fini(
    ecs_table_cache_t *cache);

void ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *result);

void ecs_table_cache_insert_w_empty(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *result,
    bool is_empty);

void ecs_table_cache_replace(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *elem);

void* ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    uint64_t table_id,
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

#define flecs_table_cache_count(cache) (cache)->tables.count
#define flecs_table_cache_empty_count(cache) (cache)->empty_tables.count
#define flecs_table_cache_all_count(cache) ((cache)->tables.count + (cache)->empty_tables.count)

bool flecs_table_cache_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out);

bool flecs_table_cache_empty_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out);

bool flecs_table_cache_all_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out);

ecs_table_cache_hdr_t* flecs_table_cache_next_(
    ecs_table_cache_iter_t *it);

#define flecs_table_cache_next(it, T)\
    (ECS_CAST(T*, flecs_table_cache_next_(it)))

#endif
