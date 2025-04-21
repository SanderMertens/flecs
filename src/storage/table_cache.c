/**
 * @file storage/table_cache.c
 * @brief Data structure for fast table iteration/lookups.
 * 
 * A table cache is a data structure that provides constant time operations for
 * insertion and removal of tables, and to testing whether a table is registered
 * with the cache. A table cache also provides functions to iterate the tables
 * in a cache.
 * 
 * The world stores a table cache per (component) id inside the component record 
 * administration. Cached queries store a table cache with matched tables.
 * 
 * A table cache has separate lists for non-empty tables and empty tables. This
 * improves performance as applications don't waste time iterating empty tables.
 */

#include "../private_api.h"

static
void flecs_table_cache_list_remove(
    ecs_table_cache_t *cache,
    ecs_table_cache_hdr_t *elem)
{
    ecs_table_cache_hdr_t *next = elem->next;
    ecs_table_cache_hdr_t *prev = elem->prev;

    if (next) {
        next->prev = prev;
    }
    if (prev) {
        prev->next = next;
    }

    cache->tables.count --;

    if (cache->tables.first == elem) {
        cache->tables.first = next;
    }
    if (cache->tables.last == elem) {
        cache->tables.last = prev;
    }

    ecs_assert(cache->tables.first == NULL || cache->tables.count,
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cache->tables.first == NULL || cache->tables.last != NULL,
        ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_table_cache_list_insert(
    ecs_table_cache_t *cache,
    ecs_table_cache_hdr_t *elem)
{
    ecs_table_cache_hdr_t *last = cache->tables.last;
    cache->tables.last = elem;
    if ((++ cache->tables.count) == 1) {
        cache->tables.first = elem;
    }

    elem->next = NULL;
    elem->prev = last;

    if (last) {
        last->next = elem;
    }

    ecs_assert(
        cache->tables.count != 1 || cache->tables.first == cache->tables.last,
        ECS_INTERNAL_ERROR, NULL);
}

void ecs_table_cache_init(
    ecs_world_t *world,
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_init_w_params(&cache->index, &world->allocators.ptr);
}

void ecs_table_cache_fini(
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_fini(&cache->index);
}

void ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *result)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_table_cache_get(cache, table) == NULL,
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    result->cache = cache;
    result->table = ECS_CONST_CAST(ecs_table_t*, table);

    flecs_table_cache_list_insert(cache, result);

    if (table) {
        ecs_map_insert_ptr(&cache->index, table->id, result);
    }

    ecs_assert(cache->tables.first != NULL, ECS_INTERNAL_ERROR, NULL);
}

void ecs_table_cache_replace(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *elem)
{
    ecs_table_cache_hdr_t **r = ecs_map_get_ref(
        &cache->index, ecs_table_cache_hdr_t, table->id);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_cache_hdr_t *old = *r;
    ecs_assert(old != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_cache_hdr_t *prev = old->prev, *next = old->next;
    if (prev) {
        ecs_assert(prev->next == old, ECS_INTERNAL_ERROR, NULL);
        prev->next = elem;
    }
    if (next) {
        ecs_assert(next->prev == old, ECS_INTERNAL_ERROR, NULL);
        next->prev = elem;
    }

    if (cache->tables.first == old) {
        cache->tables.first = elem;
    }
    if (cache->tables.last == old) {
        cache->tables.last = elem;
    }

    *r = elem;
    elem->prev = prev;
    elem->next = next;
}

void* ecs_table_cache_get(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    if (table) {
        if (ecs_map_is_init(&cache->index)) {
            return ecs_map_get_deref(&cache->index, void**, table->id);
        }
        return NULL;
    } else {
        ecs_table_cache_hdr_t *elem = cache->tables.first;
        ecs_assert(!elem || elem->table == NULL, ECS_INTERNAL_ERROR, NULL);
        return elem;
    }
}

void* ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    uint64_t table_id,
    ecs_table_cache_hdr_t *elem)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(elem->cache == cache, ECS_INTERNAL_ERROR, NULL);

    flecs_table_cache_list_remove(cache, elem);
    ecs_map_remove(&cache->index, table_id);

    return elem;
}

bool flecs_table_cache_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->next = cache->tables.first;
    out->cur = NULL;
    out->iter_fill = true;
    out->iter_empty = false;
    return out->next != NULL;
}

bool flecs_table_cache_empty_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->next = cache->tables.first;
    out->cur = NULL;
    out->iter_fill = false;
    out->iter_empty = true;
    return out->next != NULL;
}

bool flecs_table_cache_all_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->next = cache->tables.first;
    out->cur = NULL;
    out->iter_fill = true;
    out->iter_empty = true;
    return out->next != NULL;
}

const ecs_table_cache_hdr_t* flecs_table_cache_next_(
    ecs_table_cache_iter_t *it)
{
    const ecs_table_cache_hdr_t *next;

repeat:
    next = it->next;
    it->cur = next;

    if (next) {
        it->next = next->next;

        if (ecs_table_count(next->table)) {
            if (!it->iter_fill) {
                goto repeat;
            }
        } else {
            if (!it->iter_empty) {
                goto repeat;
            }
        }
    }

    return next;
}
