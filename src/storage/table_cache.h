/**
 * @file storage/table_cache.h
 * @brief Data structure for fast table iteration/lookups.
 */

#ifndef FLECS_TABLE_CACHE_H_
#define FLECS_TABLE_CACHE_H_

/** Table cache.
 * Stores table records in a dense array for cache-friendly iteration. The map
 * tracks the index of each table record into the array so that insertion and
 * removal remain O(1) amortized. */
typedef struct ecs_table_cache_t {
    ecs_map_t index;  /* <table_id, int32_t array index> */
    ecs_vec_t records; /* vec<ecs_table_cache_elem_t> */
    int32_t queryable_count; /* Number of queryable tables, stored at front */
} ecs_table_cache_t;

void ecs_table_cache_init(
    ecs_world_t *world,
    ecs_table_cache_t *cache);

void ecs_table_cache_fini(
    ecs_table_cache_t *cache);

void ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *result);

void flecs_table_cache_set_column(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    int16_t column);

void* ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    uint64_t table_id,
    ecs_table_cache_hdr_t *elem);

void* ecs_table_cache_get(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table);

const ecs_table_cache_elem_t* flecs_table_cache_get_elem(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table);

#define flecs_table_cache_count(cache) ecs_vec_count(&(cache)->records)

bool flecs_table_cache_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out,
    ecs_flags32_t empty_flags);

bool flecs_table_cache_queryable_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out,
    ecs_flags32_t empty_flags);

const ecs_table_cache_elem_t* flecs_table_cache_next(
    ecs_table_cache_iter_t *it);

#endif
