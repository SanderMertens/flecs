/**
 * @file table_cache.c
 * @brief Data structure for fast table iteration/lookups.
 * 
 * A table cache is a data structure that provides constant time operations for
 * insertion and removal of tables, and to testing whether a table is registered
 * with the cache. A table cache also provides functions to iterate the tables
 * in a cache.
 * 
 * The world stores a table cache per (component) id inside the id record 
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

    cache->empty_tables.count -= !!elem->empty;
    cache->tables.count -= !elem->empty;

    if (cache->empty_tables.first == elem) {
        cache->empty_tables.first = next;
    } else if (cache->tables.first == elem) {
        cache->tables.first = next;
    }
    if (cache->empty_tables.last == elem) {
        cache->empty_tables.last = prev;
    }
    if (cache->tables.last == elem) {
        cache->tables.last = prev;
    }
}

static
void flecs_table_cache_list_insert(
    ecs_table_cache_t *cache,
    ecs_table_cache_hdr_t *elem)
{
    ecs_table_cache_hdr_t *last;
    if (elem->empty) {
        last = cache->empty_tables.last;
        cache->empty_tables.last = elem;
        if ((++ cache->empty_tables.count) == 1) {
            cache->empty_tables.first = elem;
        }
    } else {
        last = cache->tables.last;
        cache->tables.last = elem;
        if ((++ cache->tables.count) == 1) {
            cache->tables.first = elem;
        }
    }

    elem->next = nullptr;
    elem->prev = last;

    if (last) {
        last->next = elem;
    }
}

void ecs_table_cache_init(
    ecs_world_t *world,
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_map_init_w_params(&cache->index, &world->allocators.ptr);
}

void ecs_table_cache_fini(
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_map_fini(&cache->index);
}

bool ecs_table_cache_is_empty(
    const ecs_table_cache_t *cache)
{
    return ecs_map_count(&cache->index) == 0;
}

void ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *result)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(ecs_table_cache_get(cache, table) == nullptr,
        ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(result != nullptr, ECS_INTERNAL_ERROR, nullptr);

    bool empty;
    if (!table) {
        empty = false;
    } else {
        empty = ecs_table_count(table) == 0;
    }

    result->cache = cache;
    result->table = ECS_CONST_CAST(ecs_table_t*, table);
    result->empty = empty;

    flecs_table_cache_list_insert(cache, result);

    if (table) {
        ecs_map_insert_ptr(&cache->index, table->id, result);
    }

    ecs_assert(empty || cache->tables.first != nullptr, 
        ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(!empty || cache->empty_tables.first != nullptr, 
        ECS_INTERNAL_ERROR, nullptr);
}

void ecs_table_cache_replace(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    ecs_table_cache_hdr_t *elem)
{
    ecs_table_cache_hdr_t **r = ecs_map_get_ref(
        &cache->index, ecs_table_cache_hdr_t, table->id);
    ecs_assert(r != nullptr, ECS_INTERNAL_ERROR, nullptr);

    ecs_table_cache_hdr_t *old = *r;
    ecs_assert(old != nullptr, ECS_INTERNAL_ERROR, nullptr);

    ecs_table_cache_hdr_t *prev = old->prev, *next = old->next;
    if (prev) {
        ecs_assert(prev->next == old, ECS_INTERNAL_ERROR, nullptr);
        prev->next = elem;
    }
    if (next) {
        ecs_assert(next->prev == old, ECS_INTERNAL_ERROR, nullptr);
        next->prev = elem;
    }

    if (cache->empty_tables.first == old) {
        cache->empty_tables.first = elem;
    }
    if (cache->empty_tables.last == old) {
        cache->empty_tables.last = elem;
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
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    if (table) {
        if (ecs_map_is_init(&cache->index)) {
            return ecs_map_get_deref(&cache->index, void**, table->id);
        }
        return nullptr;
    } else {
        ecs_table_cache_hdr_t *elem = cache->tables.first;
        ecs_assert(!elem || elem->table == nullptr, ECS_INTERNAL_ERROR, nullptr);
        return elem;
    }
}

void* ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    uint64_t table_id,
    ecs_table_cache_hdr_t *elem)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(table_id != 0, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(elem != nullptr, ECS_INTERNAL_ERROR, nullptr);

    ecs_assert(elem->cache == cache, ECS_INTERNAL_ERROR, nullptr);

    flecs_table_cache_list_remove(cache, elem);
    ecs_map_remove(&cache->index, table_id);

    return elem;
}

bool ecs_table_cache_set_empty(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    bool empty)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(table != nullptr, ECS_INTERNAL_ERROR, nullptr);

    ecs_table_cache_hdr_t *elem = ecs_map_get_deref(&cache->index, 
        ecs_table_cache_hdr_t, table->id);
    if (!elem) {
        return false;
    }

    if (elem->empty == empty) {
        return false;
    }

    flecs_table_cache_list_remove(cache, elem);
    elem->empty = empty;
    flecs_table_cache_list_insert(cache, elem);

    return true;
}

bool flecs_table_cache_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(out != nullptr, ECS_INTERNAL_ERROR, nullptr);
    out->next = cache->tables.first;
    out->next_list = nullptr;
    out->cur = nullptr;
    return out->next != nullptr;
}

bool flecs_table_cache_empty_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(out != nullptr, ECS_INTERNAL_ERROR, nullptr);
    out->next = cache->empty_tables.first;
    out->next_list = nullptr;
    out->cur = nullptr;
    return out->next != nullptr;
}

bool flecs_table_cache_all_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != nullptr, ECS_INTERNAL_ERROR, nullptr);
    ecs_assert(out != nullptr, ECS_INTERNAL_ERROR, nullptr);
    out->next = cache->empty_tables.first;
    out->next_list = cache->tables.first;
    out->cur = nullptr;
    return out->next != nullptr || out->next_list != nullptr;
}

ecs_table_cache_hdr_t* flecs_table_cache_next_(
    ecs_table_cache_iter_t *it)
{
    ecs_table_cache_hdr_t *next = it->next;
    if (!next) {
        next = it->next_list;
        it->next_list = nullptr;
        if (!next) {
            return nullptr;
        }
    }

    it->cur = next;
    it->next = next->next;
    return next;
}
