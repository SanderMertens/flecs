/**
 * @file table_cache.h
 * @brief Data structure for fast table iteration/lookups.
 */

#ifndef FLECS_TABLE_CACHE_H_
#define FLECS_TABLE_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

void _ecs_table_cache_init(
    ecs_table_cache_t *cache,
    ecs_size_t size,
    ecs_poly_t *parent,
    void(*free_payload)(ecs_poly_t *parent, void*));

#define ecs_table_cache_init(cache, T, parent, free_payload)\
    _ecs_table_cache_init(cache, ECS_SIZEOF(T), parent, free_payload)

void ecs_table_cache_fini(
    ecs_table_cache_t *cache);

void* _ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    ecs_size_t size,
    const ecs_table_t *table);

#define ecs_table_cache_insert(cache, T, table)\
    ECS_CAST(T*, _ecs_table_cache_insert(cache, ECS_SIZEOF(T), table))

void _ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    const ecs_table_t *table);

#define ecs_table_cache_remove(cache, table)\
    _ecs_table_cache_remove(cache, table)

void* _ecs_table_cache_get(
    const ecs_table_cache_t *cache,
    ecs_size_t size,
    const ecs_table_t *table);

#define ecs_table_cache_get(cache, T, table)\
    ECS_CAST(T*, _ecs_table_cache_get(cache, ECS_SIZEOF(T), table))

void _ecs_table_cache_set_empty(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    bool empty);

#define ecs_table_cache_set_empty(cache, table, empty)\
    _ecs_table_cache_set_empty(cache, table, empty)

#ifdef __cplusplus
}
#endif

#endif
