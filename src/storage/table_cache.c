/**
 * @file storage/table_cache.c
 * @brief Data structure for fast table iteration/lookups.
 *
 * A table cache is a data structure that provides constant time operations for
 * insertion and removal of tables, and for testing whether a table is registered
 * with the cache. A table cache also provides functions to iterate the tables
 * in a cache.
 *
 * The world stores a table cache per (component) id inside the component record
 * administration. Cached queries store a table cache with matched tables.
 *
 * Records are stored in a dense array of pointers for fast, cache-friendly
 * iteration. A map tracks the index of each record in the array so that
 * insertion and removal remain O(1) amortized.
 */

#include "../private_api.h"

#define FLECS_TABLE_CACHE_INDEX_THRESHOLD (4)

static void flecs_table_cache_move(
    ecs_table_cache_t *cache,
    ecs_table_cache_elem_t *records,
    int32_t dst,
    int32_t src)
{
    if (dst == src) {
        return;
    }

    records[dst] = records[src];

    if (ecs_map_is_init(&cache->index)) {
        ecs_map_val_t *idx = ecs_map_get(&cache->index, records[dst].table->id);
        ecs_assert(idx != NULL, ECS_INTERNAL_ERROR, NULL);
        *idx = flecs_ito(uint64_t, dst);
    }
}

static void flecs_table_cache_swap(
    ecs_table_cache_t *cache,
    ecs_table_cache_elem_t *records,
    int32_t a,
    int32_t b)
{
    if (a == b) {
        return;
    }

    ecs_table_cache_elem_t tmp = records[a];
    records[a] = records[b];
    records[b] = tmp;

    if (ecs_map_is_init(&cache->index)) {
        ecs_map_val_t *a_idx = ecs_map_get(&cache->index, records[a].table->id);
        ecs_map_val_t *b_idx = ecs_map_get(&cache->index, records[b].table->id);
        ecs_assert(a_idx != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(b_idx != NULL, ECS_INTERNAL_ERROR, NULL);
        *a_idx = flecs_ito(uint64_t, a);
        *b_idx = flecs_ito(uint64_t, b);
    }
}

static int32_t flecs_table_cache_find(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table)
{
    if (ecs_map_is_init(&cache->index)) {
        ecs_map_val_t *r = ecs_map_get(&cache->index, table->id);
        if (!r) {
            return -1;
        }
        return flecs_uto(int32_t, *r);
    }

    int32_t i, count = ecs_vec_count(&cache->records);
    const ecs_table_cache_elem_t *records = ecs_vec_first_t(
        ECS_CONST_CAST(ecs_vec_t*, &cache->records), ecs_table_cache_elem_t);
    for (i = 0; i < count; i ++) {
        if (records[i].table == table) {
            return i;
        }
    }

    return -1;
}

void ecs_table_cache_init(
    ecs_world_t *world,
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_os_zeromem(&cache->index);
    cache->index.allocator = &world->allocator;
    ecs_vec_init_t(&world->allocator, &cache->records, ecs_table_cache_elem_t, 0);
    cache->queryable_count = 0;
}

void ecs_table_cache_fini(
    ecs_table_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_vec_fini_t(cache->index.allocator, &cache->records, ecs_table_cache_elem_t);
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
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    result->cr = (ecs_component_record_t*)cache;
    result->table = ECS_CONST_CAST(ecs_table_t*, table);

    int32_t index = ecs_vec_count(&cache->records);
    ecs_table_cache_elem_t *slot = ecs_vec_append_t(
        cache->index.allocator, &cache->records, ecs_table_cache_elem_t);
    slot->table = ECS_CONST_CAST(ecs_table_t*, table);
    slot->tr = (ecs_table_record_t*)result;
    slot->column = -1;
    slot->index = ((ecs_table_record_t*)result)->index;

    if (ecs_map_is_init(&cache->index)) {
        ecs_map_insert(&cache->index, table->id, flecs_ito(uint64_t, index));
    } else if ((index + 1) > FLECS_TABLE_CACHE_INDEX_THRESHOLD) {
        ecs_map_init(&cache->index, cache->index.allocator);
        ecs_table_cache_elem_t *records = ecs_vec_first_t(
            &cache->records, ecs_table_cache_elem_t);
        int32_t i;
        for (i = 0; i <= index; i ++) {
            ecs_map_insert(&cache->index, records[i].table->id,
                flecs_ito(uint64_t, i));
        }
    }

    if (!(table->flags & (EcsTableNotQueryable|EcsTableIsPrefab|EcsTableIsDisabled))) {
        ecs_table_cache_elem_t *records = ecs_vec_first_t(
            &cache->records, ecs_table_cache_elem_t);
        flecs_table_cache_swap(cache, records, cache->queryable_count, index);
        cache->queryable_count ++;
    }
}

void flecs_table_cache_set_column(
    ecs_table_cache_t *cache,
    const ecs_table_t *table,
    int16_t column)
{
    int32_t index = flecs_table_cache_find(cache, table);
    ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);

    ecs_table_cache_elem_t *slot = ecs_vec_get_t(
        &cache->records, ecs_table_cache_elem_t, index);
    ecs_assert(slot != NULL, ECS_INTERNAL_ERROR, NULL);
    slot->column = column;
}

void* ecs_table_cache_get(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t index = flecs_table_cache_find(cache, table);
    if (index == -1) {
        return NULL;
    }

    ecs_table_cache_elem_t *slot = ecs_vec_get_t(
        ECS_CONST_CAST(ecs_vec_t*, &cache->records),
        ecs_table_cache_elem_t, index);
    return slot ? slot->tr : NULL;
}

const ecs_table_cache_elem_t* flecs_table_cache_get_elem(
    const ecs_table_cache_t *cache,
    const ecs_table_t *table)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t index = flecs_table_cache_find(cache, table);
    if (index == -1) {
        return NULL;
    }

    return ecs_vec_get_t(
        ECS_CONST_CAST(ecs_vec_t*, &cache->records),
        ecs_table_cache_elem_t, index);
}

void* ecs_table_cache_remove(
    ecs_table_cache_t *cache,
    uint64_t table_id,
    ecs_table_cache_hdr_t *elem)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(elem->cr == (ecs_component_record_t*)cache,
        ECS_INTERNAL_ERROR, NULL);

    int32_t last = ecs_vec_count(&cache->records) - 1;
    ecs_table_cache_elem_t *records = ecs_vec_first_t(
        &cache->records, ecs_table_cache_elem_t);

    int32_t index;
    if (ecs_map_is_init(&cache->index)) {
        ecs_map_val_t v = ecs_map_remove(&cache->index, table_id);
        index = flecs_uto(int32_t, v);
    } else {
        for (index = 0; index <= last; index ++) {
            if (records[index].tr == (ecs_table_record_t*)elem) {
                break;
            }
        }
        ecs_assert(index <= last, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_assert(records[index].tr == (ecs_table_record_t*)elem,
        ECS_INTERNAL_ERROR, NULL);

    if (index < cache->queryable_count) {
        int32_t last_queryable = cache->queryable_count - 1;
        flecs_table_cache_move(cache, records, index, last_queryable);
        flecs_table_cache_move(cache, records, last_queryable, last);
        cache->queryable_count --;
    } else {
        flecs_table_cache_move(cache, records, index, last);
    }

    ecs_vec_remove_last(&cache->records);

    return elem;
}

bool flecs_table_cache_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out,
    ecs_flags32_t empty_flags)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->elems = ecs_vec_first_t(
        ECS_CONST_CAST(ecs_vec_t*, &cache->records),
        ecs_table_cache_elem_t);
    out->remaining = ecs_vec_count(&cache->records);
    out->cur = NULL;
    out->flags = empty_flags;
    return out->remaining != 0;
}

bool flecs_table_cache_queryable_iter(
    const ecs_table_cache_t *cache,
    ecs_table_cache_iter_t *out,
    ecs_flags32_t empty_flags)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(out != NULL, ECS_INTERNAL_ERROR, NULL);
    out->elems = ecs_vec_first_t(
        ECS_CONST_CAST(ecs_vec_t*, &cache->records),
        ecs_table_cache_elem_t);
    out->remaining = cache->queryable_count;
    out->cur = NULL;
    out->flags = empty_flags;
    return out->remaining != 0;
}

const ecs_table_cache_elem_t* flecs_table_cache_next(
    ecs_table_cache_iter_t *it)
{
    ecs_flags32_t flags = it->flags;

    while (it->remaining > 0) {
        const ecs_table_cache_elem_t *next = it->elems;
        it->elems ++;
        it->remaining --;

        if (next->table->flags & flags) {
            it->cur = next;
            return next;
        }
    }

    it->cur = NULL;
    return NULL;
}
