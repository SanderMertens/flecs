#include "private_api.h"

static
void ensure_index(
    ecs_table_cache_t *cache)
{
    if (!cache->index) {
        cache->index = ecs_map_new(ecs_table_cache_hdr_t*, 0);
    }
}

static
void table_cache_list_remove(
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
void table_cache_list_insert(
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

    elem->next = NULL;
    elem->prev = last;

    if (last) {
        last->next = elem;
    }
}

void _ecs_table_cache_init(
    ecs_table_cache_t *cache,
    ecs_size_t size,
    ecs_poly_t *parent,
    void(*free_payload)(ecs_poly_t*, void*))
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size >= ECS_SIZEOF(ecs_table_cache_hdr_t), 
        ECS_INTERNAL_ERROR, NULL);

    ecs_os_zeromem(cache);

    cache->size = size;
    cache->parent = parent;
    cache->free_payload = free_payload;
}

static
void free_payload(
    ecs_table_cache_t *cache,
    ecs_table_cache_hdr_t *first)
{
    void(*free_payload_func)(ecs_poly_t*, void*) = cache->free_payload;
    if (free_payload_func) {
        ecs_poly_t *parent = cache->parent;
        ecs_table_cache_hdr_t *cur, *next = first;

        while ((cur = next)) {
            next = cur->next;
            free_payload_func(parent, cur);
            ecs_os_free(cur);
        }
    }
}

void ecs_table_cache_fini(
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_map_free(cache->index);
    free_payload(cache, cache->tables.first);
    free_payload(cache, cache->empty_tables.first);
}

bool ecs_table_cache_is_initialized(
    ecs_table_cache_t *cache)
{
    return cache->size != 0;
}

void* _ecs_table_cache_insert(
    ecs_table_cache_t *cache,
    ecs_size_t size,
    const ecs_table_t *table)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size == cache->size, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table || (_ecs_table_cache_get(cache, size, table) == NULL), 
        ECS_INTERNAL_ERROR, NULL);

    bool empty;
    if (!table) {
        empty = false;
    } else {
        empty = ecs_table_count(table) == 0;
    }

    ecs_table_cache_hdr_t *result = ecs_os_calloc(size);
    result->table = (ecs_table_t*)table;
    result->empty = empty;

    table_cache_list_insert(cache, result);

    if (table) {
        ensure_index(cache);
        ecs_map_set_ptr(cache->index, table->id, result);
    }

    ecs_assert(empty || cache->tables.first != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!empty || cache->empty_tables.first != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_table_cache_hdr_t* _ecs_table_cache_get(
    const ecs_table_cache_t *cache,
    ecs_size_t size,
    const ecs_table_t *table)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size == cache->size, ECS_INTERNAL_ERROR, NULL);
    (void)size;
    return ecs_map_get_ptr(cache->index, ecs_table_cache_hdr_t*, table->id);
}

bool _ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    ecs_size_t size,
    const ecs_table_t *table)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size == cache->size, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    ecs_table_cache_hdr_t *elem = ecs_map_get_ptr(
        cache->index, ecs_table_cache_hdr_t*, table->id);
    if (!elem) {
        return false;
    }

    if (cache->free_payload) {
        cache->free_payload(cache->parent, elem);
    }

    table_cache_list_remove(cache, elem);

    ecs_os_free(elem);

    if (ecs_map_remove(cache->index, table->id) == 0) {
        ecs_map_free(cache->index);
        cache->index = NULL;
        return true;
    } else {
        return false;
    }
}

bool ecs_table_cache_set_empty(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    bool empty)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_cache_hdr_t *elem = ecs_map_get_ptr(
        cache->index, ecs_table_cache_hdr_t*, table->id);
    if (!elem) {
        return false;
    }

    if (elem->empty == empty) {
        return false;
    }

    table_cache_list_remove(cache, elem);
    elem->empty = empty;
    table_cache_list_insert(cache, elem);

    return true;
}

void _ecs_table_cache_fini_delete_all(
    ecs_world_t *world,
    ecs_table_cache_t *cache,
    ecs_size_t size)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!cache->index) {
        return;
    }

    ecs_assert(cache->size == size, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    /* Temporarily set index to NULL, so that when the table tries to remove
     * itself from the cache it won't be able to. This keeps the arrays we're
     * iterating over consistent */
    ecs_map_t *index = cache->index;
    cache->index = NULL;

    ecs_table_cache_hdr_t *cur, *next = cache->tables.first;
    while ((cur = next)) {
        flecs_delete_table(world, cur->table);
        next = cur->next;
        ecs_os_free(cur);
    }

    next = cache->empty_tables.first;
    while ((cur = next)) {
        flecs_delete_table(world, cur->table);
        next = cur->next;
        ecs_os_free(cur);
    }

    cache->index = index;

    ecs_table_cache_fini(cache);
}

bool flecs_table_cache_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->next = cache->tables.first;
    out->cur = NULL;
    return out->next != NULL;
}

bool flecs_table_cache_empty_iter(
    ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->next = cache->empty_tables.first;
    out->cur = NULL;
    return out->next != NULL;
}

ecs_table_cache_hdr_t* _flecs_table_cache_next(
    ecs_table_cache_iter_t *it)
{
    ecs_table_cache_hdr_t *next = it->next;
    if (!next) {
        return false;
    }

    it->cur = next;
    it->next = next->next;
    return next;
}
