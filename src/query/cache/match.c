/**
 * @file query/cache/match.c
 * @brief Match table one or more times with query.
 */

#include "../../private_api.h"

/* Free cache entry element. */
static
void flecs_query_cache_match_elem_fini(
    ecs_query_cache_t *cache,
    ecs_query_cache_match_t *qm)
{
    flecs_bfree(&cache->allocators.pointers, 
        ECS_CONST_CAST(void*, qm->base.trs));

    flecs_bfree(&cache->allocators.pointers, 
        ECS_CONST_CAST(void*, qm->base.ptrs));

    if (!flecs_query_cache_is_trivial(cache)) {
        if (qm->_ids != cache->query->ids) {
            flecs_bfree(&cache->allocators.ids, qm->_ids);
        }

        if (qm->_sources != cache->sources) {
            flecs_bfree(&cache->allocators.ids, qm->_sources);
        }

        if (qm->_tables) {
            flecs_bfree(&cache->allocators.pointers, qm->_tables);
        }

        if (qm->_monitor) {
            flecs_bfree(&cache->allocators.monitors, qm->_monitor);
        }
    }
}

/* Free cache entry element and optional wildcard matches. */
void flecs_query_cache_match_fini(
    ecs_query_cache_t *cache,
    ecs_query_cache_match_t *qm)
{
    flecs_query_cache_match_elem_fini(cache, qm);

    if (!flecs_query_cache_is_trivial(cache)) {
        if (qm->wildcard_matches) {
            ecs_query_cache_match_t *elems = ecs_vec_first(qm->wildcard_matches);
            int32_t i, count = ecs_vec_count(qm->wildcard_matches);
            for (i = 0; i < count; i ++) {
                flecs_query_cache_match_elem_fini(cache, &elems[i]);
            }

            ecs_allocator_t *a = &cache->query->real_world->allocator;
            ecs_vec_fini_t(a, qm->wildcard_matches, ecs_query_cache_match_t);
            flecs_free_t(a, ecs_vec_t, qm->wildcard_matches);
        }
    }
}

/* Initialize cache entry element. */
static
void flecs_query_cache_match_set(
    ecs_query_cache_t *cache,
    ecs_query_cache_match_t *qm,
    ecs_iter_t *it)
{
    bool trivial_cache = flecs_query_cache_is_trivial(cache);
    ecs_query_t *query = cache->query;
    int8_t i, field_count = query->field_count;
    ecs_assert(field_count > 0, ECS_INTERNAL_ERROR, NULL);

    qm->base.table = it->table;
    qm->base.set_fields = it->set_fields;

    if (!qm->base.ptrs) {
        qm->base.ptrs = flecs_balloc(&cache->allocators.pointers);
    }

    if (!qm->base.trs) {
        qm->base.trs = flecs_balloc(&cache->allocators.pointers);
    }

    /* Reset resources in case this is an existing record */
    ecs_os_memcpy_n(ECS_CONST_CAST(ecs_table_record_t**, qm->base.trs), 
        it->trs, ecs_table_record_t*, field_count);
    
    /* Initialize pointers*/
    ecs_os_memset_n(qm->base.ptrs, 0, void*, field_count);

    /* Set table version to sentinel that'll force reevaluation */
    qm->base.table_version = UINT32_MAX;

    /* Find out whether to store result-specific ids array or fixed array */
    ecs_id_t *ids = cache->query->ids;
    for (i = 0; i < field_count; i ++) {
        if (it->ids[i] != ids[i]) {
            break;
        }
    }

    if (!trivial_cache) {
        if (i != field_count) {
            if (qm->_ids == ids || !qm->_ids) {
                qm->_ids = flecs_balloc(&cache->allocators.ids);
            }
            ecs_os_memcpy_n(qm->_ids, it->ids, ecs_id_t, field_count);
        } else {
            if (qm->_ids != ids) {
                flecs_bfree(&cache->allocators.ids, qm->_ids);
                qm->_ids = ids;
            }
        }
    }

    /* Find out whether to store result-specific sources array or fixed array */
    for (i = 0; i < field_count; i ++) {
        if (it->sources[i]) {
            break;
        }
    }

    if (!trivial_cache) {
        if (i != field_count) {
            if (qm->_sources == cache->sources || !qm->_sources) {
                qm->_sources = flecs_balloc(&cache->allocators.ids);
            }
            ecs_os_memcpy_n(qm->_sources, it->sources, ecs_entity_t, field_count);
            if (!qm->_tables) {
                qm->_tables = flecs_balloc(&cache->allocators.pointers);
            }
            for (i = 0; i < field_count; i ++) {
                if (it->trs[i]) {
                    qm->_tables[i] = it->trs[i]->hdr.table;
                }
            }
        } else {
            if (qm->_sources != cache->sources) {
                flecs_bfree(&cache->allocators.ids, qm->_sources);
                qm->_sources = cache->sources;
            }
            if (qm->_tables) {
                flecs_bfree(&cache->allocators.pointers, qm->_tables);
                qm->_tables = NULL;
            }
        }

        qm->_up_fields = it->up_fields;
    } else {
        /* If this is a trivial cache, we shouldn't have any fields with 
         * non-$this sources */
        ecs_assert(i == field_count, ECS_INTERNAL_ERROR, NULL);
    }
}

/* Iterate the next match for table. This function accepts an iterator for the 
 * cache query and will keep on iterating until a result for a different table
 * is returned. Typically each table only returns one result, but wildcard 
 * queries can return multiple results for the same table. 
 * 
 * For each iterated result the function will initialize the cache entry for the
 * matched table. */
bool flecs_query_cache_match_next(
    ecs_query_cache_t *cache,
    ecs_iter_t *it)
{
    ecs_table_t *table = it->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_match_t *first = flecs_query_cache_add_table(cache, table);
    ecs_query_cache_match_t *qm = first;

    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    ecs_allocator_t *a = &cache->query->real_world->allocator;

    do {
        flecs_query_cache_match_set(cache, qm, it);

        if (!ecs_query_next(it)) {
            return false;
        }

        if (it->table != table) {
            return true;
        }

        /* Another match for the same table (for wildcard queries) */

        if (!first->wildcard_matches) {
            first->wildcard_matches = flecs_alloc_t(a, ecs_vec_t);
            ecs_vec_init(a, first->wildcard_matches, elem_size, 1);
        }

        qm = ecs_vec_append(a, first->wildcard_matches, elem_size);
        ecs_os_zeromem(qm);
    } while (true);
}

/* Same as flecs_query_cache_match_next, but for rematching. This function will
 * overwrite existing cache entries with new match data. */
static
bool flecs_query_cache_rematch_next(
    ecs_query_cache_t *cache,
    ecs_iter_t *it)
{
    ecs_table_t *table = it->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_match_t *first = 
        flecs_query_cache_ensure_table(cache, table);
    ecs_query_cache_match_t *qm = first;
    
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    ecs_allocator_t *a = &cache->query->real_world->allocator;
    ecs_vec_t *wildcard_matches = first->wildcard_matches;
    int32_t wildcard_elem = 0;

    bool result = true, has_more = true;

    /* So we can tell which tables in the cache got rematched. All tables for 
     * which this counter hasn't changed are no longer matched by the query. */
    first->rematch_count = cache->rematch_count;

    do {
        flecs_query_cache_match_set(cache, qm, it);

        if (!ecs_query_next(it)) {
            has_more = false;
            result = false;
        }

        if (it->table != table) {
            has_more = false;
        }

        /* Are there more results for this table? */
        if (!has_more) {
            /* If existing match had more wildcard matches than new match free
             * the superfluous ones. */

            if (wildcard_matches) {
                int32_t i, count = ecs_vec_count(wildcard_matches);
                for (i = wildcard_elem; i < count; i ++) {
                    ecs_query_cache_match_t *qm_elem = ecs_vec_get(
                        wildcard_matches, elem_size, i);
                    flecs_query_cache_match_fini(cache, qm_elem);
                }

                if (!wildcard_elem) {
                    ecs_vec_fini(a, wildcard_matches, elem_size);
                    flecs_free_t(a, ecs_vec_t, wildcard_matches);
                    qm->wildcard_matches = NULL;
                }
            }

            /* Are there more results for other tables? */
            return result;
        }

        /* Another match for the same table (for wildcard queries) */

        if (!wildcard_matches) {
            first->wildcard_matches = wildcard_matches = 
                flecs_alloc_t(a, ecs_vec_t);
            ecs_vec_init(a, wildcard_matches, elem_size, 1);
        }

        if (ecs_vec_count(wildcard_matches) <= wildcard_elem) {
            qm = ecs_vec_append(a, wildcard_matches, elem_size);
            ecs_os_zeromem(qm);
        } else {
            qm = ecs_vec_get(wildcard_matches, elem_size, wildcard_elem);
        }

        wildcard_elem ++;
    } while (true);
}

/* Rematch query cache. This function is called whenever something happened that
 * could have caused a previously matching table to no longer match with the 
 * query. This function is not called for regular table creation or deletion.
 * 
 * An example of when this function is called is when a query matched a 
 * component on a parent, and that component was removed from the parent. This
 * means that tables with (ChildOf, parent) previously matched the query, but
 * after the component got removed, no longer match.
 * 
 * This operation is expensive, since it needs to:
 * - make sure that optional fields matched on parents are updated
 * - groups are up to date for all the matched tables
 * - tables that no longer match are removed from the cache.
 */
void flecs_query_rematch(
    ecs_world_t *world,
    ecs_query_t *q)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_allocator_t *a = &world->allocator;

    ecs_iter_t it;
    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Queries with trivial caches can't trigger rematching */
    ecs_assert(!flecs_query_cache_is_trivial(cache), ECS_INTERNAL_ERROR, NULL);

    if (cache->monitor_generation == world->monitor_generation) {
        return;
    }

    ecs_os_perf_trace_push("flecs.query.rematch");

    cache->monitor_generation = world->monitor_generation;
    cache->match_count ++;

    world->info.rematch_count_total ++;
    int32_t rematch_count = ++ cache->rematch_count;

    ecs_time_t t = {0};
    if (world->flags & EcsWorldMeasureFrameTime) {
        ecs_time_measure(&t);
    }

    it = ecs_query_iter(world, cache->query);
    ECS_BIT_SET(it.flags, EcsIterNoData);

    if (!ecs_query_next(&it)) {
        flecs_query_cache_remove_all_tables(cache);
        goto done;
    }

    while (flecs_query_cache_rematch_next(cache, &it)) { }

    /* Iterate all tables in cache, remove ones that weren't just matched */
    ecs_vec_t unmatched; ecs_vec_init_t(a, &unmatched, ecs_table_t*, 0);
    ecs_size_t elem_size = flecs_query_cache_elem_size(cache);
    ecs_query_cache_group_t *cur = &cache->default_group;
    do {
        int32_t i, count = ecs_vec_count(&cur->tables);
        for (i = 0; i < count; i ++) {
            ecs_query_cache_match_t *qm = 
                ecs_vec_get(&cur->tables, elem_size, i);
            if (qm->rematch_count != rematch_count) {
                /* Collect tables, don't modify map while updating it */
                ecs_vec_append_t(a, &unmatched, ecs_table_t*)[0] = 
                    qm->base.table;
            }
        }
    } while ((cur = cur->next));

    /* Actually unmatch tables */
    int32_t i, count = ecs_vec_count(&unmatched);
    ecs_table_t **unmatched_tables = ecs_vec_first(&unmatched);
    for (i = 0; i < count; i ++) {
        flecs_query_cache_remove_table(cache, unmatched_tables[i]);
        ecs_assert(flecs_query_cache_get_table(
            cache, unmatched_tables[i]) == NULL, ECS_INTERNAL_ERROR, NULL);
    }
    ecs_vec_fini_t(a, &unmatched, ecs_table_t*);

    if (world->flags & EcsWorldMeasureFrameTime) {
        world->info.rematch_time_total += (ecs_ftime_t)ecs_time_measure(&t);
    }

done:
    ecs_os_perf_trace_pop("flecs.query.rematch");
}
