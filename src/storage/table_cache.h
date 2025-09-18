/**
 * @file storage/table_cache.h
 * @brief Data structure for fast table iteration/lookups.
 */

#ifndef FLECS_TABLE_CACHE_H_
#define FLECS_TABLE_CACHE_H_

/** Linked list of tables in table cache */
typedef struct ecs_table_cache_list_t {
    ecs_table_cache_hdr_t *first;
    ecs_table_cache_hdr_t *last;
    int32_t count;
} ecs_table_cache_list_t;

/** Table cache */
typedef struct ecs_table_cache_t {
    ecs_map_t index; /* <table_id, T*> */
    ecs_table_cache_list_t tables;
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

#define flecs_table_cache_count(cache) (cache)->tables.count

bool flecs_table_cache_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out);

bool flecs_table_cache_empty_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out);

bool flecs_table_cache_all_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out);

const ecs_table_cache_hdr_t* flecs_table_cache_next_(
    ecs_table_cache_iter_t *it);

#define flecs_table_cache_next(it, T)\
    (ECS_CONST_CAST(T*, flecs_table_cache_next_(it)))

#endif
