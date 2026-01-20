/**
 * @file query/cache/cache.c
 * @brief Cached query implementation.
 * 
 * Implements a cache that stores a list of tables that matches the query. 
 * Cached queries outperform uncached queries in many scenarios since they don't
 * have to search for tables that match a query, but just iterate a list.
 * 
 * A cache has a "cache query" which is derived from the actual query the 
 * application provides. This cache query differs in two significant ways from 
 * the application-provided query:
 * - It does not contain terms that aren't cacheable
 * - It always matches empty tables
 * 
 * If the number of terms in the actual query differs from the cache query, the
 * query will create a field_map array that maps from cached field indices to
 * actual query indices.
 * 
 * Cached queries use an observer to get notified of new/deleted tables. In most
 * cases this is sufficient to keep the cache up to date, but for queries that
 * match components outside of a table (for example, from a parent entity) the
 * cache will have to be revalidated after a parent changes a matched component.
 * This is called rematching, the implementation can be found in match.c.
 * 
 * A cache can be trivial or non-trivial. A trivial cache is a cache for a query
 * that meets the following criteria:
 * - The query doesn't have any wildcards
 * - The query doesn't use relationship traversal
 * - The query doesn't use operators other than And, Not or Optional
 * - The cached query and actual query have the same terms
 * 
 * A trivial cache has to store much less data for each cached element, and uses
 * an iterator implementation that is much faster. Because of this difference 
 * the code often needs to lookup the size of cache elements before doing work.
 * 
 * The following types are important for the cache:
 * - ecs_query_cache_match_t: Stores a single match for a table.
 * - ecs_query_cache_table_t: Element in the cache->tables map, which allows for
 *                            looking up a match by table id.
 * - ecs_query_cache_group_t: Stores an array of matched tables. A query has a
 *                            single group by default, but can have more (see 
 *                            group_by).
 * 
 * There are three cache features that significantly alter the way how elements
 * are stored in the cache, which are group_by, order_by and wildcards.
 * 
 * Group_by
 * ========
 * Group_by assigns a group id (unsigned 64bit integer) to each table. This 
 * number is computed by a group_by function that can be provided by the 
 * application. A group can only be computed from which components are stored in
 * a table (the table type).
 * 
 * By default a query only has a single group which contains all matched tables
 * stored in an array. When a query uses group_by, the matched tables are split
 * up across multiple groups depending on the computed group id of the table.
 * When a group becomes empty (there are no more tables with the group id) it is
 * deleted from the cache.
 * 
 * The query cache contains a "groups" map and a "tables" map. The groups map
 * allows for group id -> group lookups. The tables map allows for table id ->
 * table element lookups. Because tables are stored in arrays that can be 
 * reallocated, rather than a pointer to the table element, the tables map 
 * stores a pointer to the group the table is part of, and an index into the
 * group's table array.
 * 
 * Queries can use group_by for two main reasons:
 * - It allows for quickly iterating subsets of the matched query results
 * - Groups are iterated in order based on group id
 * 
 * An application can iterate a single group by calling ecs_iter_set_group on an
 * iterator created from a query that uses group_by. This will cause the 
 * iterator to only iterate the table array for that specific group. To find the
 * group's table array, the ecs_iter_set_group function uses the groups map.
 * 
 * Groups are stored in a linked list that's ordered by the group id. This can
 * be in ascending or descending order, depending on the query. Because of this
 * ordering, group insertion and group removal is an O(N) operation where N is
 * the number of groups in the query. The head of the list is stored in the
 * first_group member of the query.
 * 
 * The cascade query feature is built on top of group_by. It provides a callback
 * that computes a hierarchy depth for the table with a specified relationship.
 * Because groups are stored in ascending or descending order, this effectively
 * means that tables will be iterated in a breadth-first order, which can be
 * useful for hierarchy traversal code or transform systems.
 * 
 * Order_by
 * ========
 * Order_by will cause the query to return results in an order that is defined
 * by either a component or entity id. To accomplish this, the query has to find
 * the order across different tables. The code will first sort the elements in
 * each matched table, and then build a list of (offset, count) slices across 
 * the matched tables that represents the correct iteration order. The algorithm
 * used for sorting is qsort.
 * 
 * Resorting is a very expensive operation. Queries use change detection, which
 * at a table level can detect if any changes occurred to the entities or ordered
 * by component. Only if a change has been detected will resorting occur. Even
 * then, this remains an expensive feature and should only be used for data that
 * doesn't change often. Flecs uses the query sorting feature to ensure that
 * pipeline queries return systems in a well defined order.
 * 
 * The sorted list of slices is stored in the table_slices member of the cache,
 * and is only populated for sorted queries.
 * 
 * When group_by and order_by are combined in a single query, the group order
 * takes precedence over order_by.
 * 
 * It is currently not possible to iterate a single group when order_by is used
 * together with group_by. This is a TODO that has to be addressed by adding a
 * table_slices array to each group instead of as a member of the cache object.
 * 
 * Wildcards
 * =========
 * Wildcards are a query feature that can cause a single table to be matched
 * multiple times. A typical example would be a (Likes, *) query. If an entity
 * has both (Likes, Bob) and (Likes, Alice), it (and all entities in the same 
 * table) will get matched twice by the query.
 * 
 * When a table matches multiple times (through one or more wildcard terms), any
 * match after the first is stored in a separate wildcard_matches member on the
 * cache element. This separate array ensures that tables can be removed from
 * groups without leaving arbitrarily sized holes in the group->tables array.
 * 
 * When iterating a wildcard query, the iterator will alternate between 
 * iterating the group->tables array and the wildcard_matches array on each
 * matched table, in a way that all matches for the same table are iterated
 * together.
 */

#include "../../private_api.h"

/* Is cache trivial? */
bool flecs_query_cache_is_trivial(
    const ecs_query_cache_t *cache)
{
    return (cache->query->flags & EcsQueryTrivialCache) != 0;
}

/* Trivial caches have a significantly smaller cache element size */
ecs_size_t flecs_query_cache_elem_size(
    const ecs_query_cache_t *cache) 
{
    return flecs_query_cache_is_trivial(cache) 
        ? ECS_SIZEOF(ecs_query_triv_cache_match_t) 
        : ECS_SIZEOF(ecs_query_cache_match_t)
        ;
}

/* The default group_by function. When an application specifies a relationship
 * for the ecs_query_desc_t::group_by field but does not provide a 
 * group_by_callback, this function will be automatically used. It will cause
 * the query cache to be grouped by relationship target. */
static
uint64_t flecs_query_cache_default_group_by(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_id_t id, 
    void *ctx) 
{
    (void)ctx;

    ecs_id_t match;
    if (ecs_search(world, table, ecs_pair(id, EcsWildcard), &match) != -1) {
        if (ECS_IS_VALUE_PAIR(match)) {
            return ECS_PAIR_SECOND(match);
        } else {
            return ecs_pair_second(world, match);
        }
    }
    return 0;
}

/* The group_by function that's used for the cascade feature. Groups tables by
 * hierarchy depth, resulting in breadth-first iteration. */
static
uint64_t flecs_query_cache_group_by_cascade(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    void *ctx)
{
    (void)id;
    ecs_term_t *term = ctx;
    ecs_entity_t rel = term->trav;
    int32_t depth = flecs_relation_depth(world, rel, table);
    return flecs_ito(uint64_t, depth);
}

/* Initialize cache with matching tables. */
static
void flecs_query_cache_match_tables(
    ecs_world_t *world,
    ecs_query_cache_t *cache)
{
    ecs_iter_t it = ecs_query_iter(world, cache->query);
    ECS_BIT_SET(it.flags, EcsIterNoData);
    ECS_BIT_SET(it.flags, EcsIterTableOnly);

    if (!ecs_query_next(&it)) {
        return;
    }

    while (flecs_query_cache_match_next(cache, &it)) { }
}

/* Match new table with cache. */
static
bool flecs_query_cache_match_table(
    ecs_world_t *world,
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    if (!ecs_map_is_init(&cache->tables)) {
        return false;
    }

    ecs_query_t *q = cache->query;

#ifndef FLECS_SANITIZE
    if (!flecs_table_bloom_filter_test(table, q->bloom_filter)) {
        return false;
    }
#endif

    if (table->flags & EcsTableNotQueryable) {
        return false;
    }

    /* Iterate uncached query for table to check if it matches. If this is a
     * wildcard query, a table can match multiple times. */
    ecs_iter_t it = flecs_query_iter(world, q);
    it.flags |= EcsIterNoData;
    ecs_iter_set_var_as_table(&it, 0, table);

    if (!ecs_query_next(&it)) {
        /* Table doesn't match */
        return false;
    }

    if (flecs_query_cache_match_next(cache, &it)) {
        /* Should not return for more than one table. */
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

#ifdef FLECS_SANITIZE
    /* Sanity check to make sure bloom filter is correct */
    ecs_assert(flecs_table_bloom_filter_test(table, q->bloom_filter),
        ECS_INTERNAL_ERROR, NULL);
#endif

    return true;
}

/* Iterate component monitors for cache. Each field that could potentially cause
 * up traversal will create a monitor. Component monitors are registered with 
 * the world and are used to determine whether a rematch is necessary. */
static
void flecs_query_cache_for_each_component_monitor(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_t *cache,
    void(*callback)(
        ecs_world_t* world,
        ecs_id_t id,
        ecs_query_t *q))
{
    ecs_query_t *q = &impl->pub;
    ecs_term_t *terms = cache->query->terms;
    int32_t i, count = cache->query->term_count;

    for (i = 0; i < count; i++) {
        ecs_term_t *term = &terms[i];
        ecs_term_ref_t *src = &term->src;

        if (src->id & EcsUp) {
            callback(world, ecs_pair(term->trav, EcsWildcard), q);
            if (term->trav != EcsIsA) {
                callback(world, ecs_pair(EcsIsA, EcsWildcard), q);
            }
            callback(world, term->id, q);

        } else if (src->id & EcsSelf && !ecs_term_match_this(term)) {
            ecs_assert(!(src->id & EcsSelf) || ecs_term_match_this(term),   
                ECS_INTERNAL_ERROR, NULL);
        }
    }
}

/* Iterate over terms in query to initialize necessary bookkeeping. */
static
int flecs_query_cache_process_query(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_term_t *terms = cache->query->terms;
    int32_t i, count = cache->query->term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_ref_t *src = &term->src;

        /* Find cascade terms if query has any */
        if (src->id & EcsCascade) {
            ecs_assert(cache->cascade_by == 0, ECS_INVALID_PARAMETER,
                "query can only have one cascade term");
            cache->cascade_by = i + 1;
        }

        /* Set the EcsQueryHasRefs flag. Ref fields are fields that can be 
         * matched on another entity, and can require rematching. */
        if (src->id & EcsUp) {
            impl->pub.flags |= EcsQueryHasRefs;
        }
    }

    /* Register component monitor for each ref field. */
    flecs_query_cache_for_each_component_monitor(
        world, impl, cache, flecs_monitor_register);

    return 0;
}

/* -- Private API -- */

/* Do bookkeeping after enabling order_by */
static
int flecs_query_cache_order_by(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_entity_t order_by,
    ecs_order_by_action_t order_by_callback,
    ecs_sort_table_action_t action)
{
    ecs_check(impl != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_query_cache_t *cache = impl->cache;
    ecs_check(cache != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!ecs_id_is_wildcard(order_by), 
        ECS_INVALID_PARAMETER, "cannot order by wildcard component");

    /* Find order_by term & make sure it is queried for */
    const ecs_query_t *query = cache->query;
    int32_t i, count = query->term_count;
    int32_t order_by_term = -1;

    if (order_by) {
        for (i = 0; i < count; i ++) {
            const ecs_term_t *term = &query->terms[i];
            
            /* Only And terms are supported */
            if (term->id == order_by && term->oper == EcsAnd) {
                order_by_term = i;
                break;
            }
        }

        if (order_by_term == -1) {
            char *id_str = ecs_id_str(world, order_by);
            ecs_err("order_by component '%s' is not queried for", id_str);
            ecs_os_free(id_str);
            goto error;
        }
    }

    cache->order_by = order_by;
    cache->order_by_callback = order_by_callback;
    cache->order_by_term = order_by_term;
    cache->order_by_table_callback = action;

    ecs_vec_fini_t(NULL, &cache->table_slices, ecs_query_cache_match_t);
    flecs_query_cache_sort_tables(world, impl);

    if (!cache->table_slices.array) {
        flecs_query_cache_build_sorted_tables(cache);
    }

    return 0;
error:
    return -1;
}

/* Do bookkeeping after enabling group_by */
static
void flecs_query_cache_group_by(
    ecs_query_cache_t *cache,
    ecs_entity_t sort_component,
    ecs_group_by_action_t group_by)
{   
    ecs_check(cache->group_by == 0, ECS_INVALID_OPERATION,
        "query is already grouped");
    ecs_check(cache->group_by_callback == 0, ECS_INVALID_OPERATION,
        "query is already grouped");

    if (!group_by) {
        /* Builtin function that groups by relationship */
        group_by = flecs_query_cache_default_group_by;   
    }

    cache->group_by = sort_component;
    cache->group_by_callback = group_by;

    ecs_map_init(&cache->groups, &cache->query->world->allocator);
error:
    return;
}

/* Callback for the observer that is subscribed for table events. This function
 * is the entry point for matching/unmatching new tables with the query. */
static
void flecs_query_cache_on_event(
    ecs_iter_t *it)
{
    /* Because this is the observer::run callback, checking if this is event is
     * already handled is not done for us. */
    ecs_world_t *world = it->world;
    ecs_observer_t *o = it->ctx;
    ecs_observer_impl_t *o_impl = flecs_observer_impl(o);
    if (o_impl->last_event_id) {
        if (o_impl->last_event_id[0] == world->event_id) {
            return;
        }
        o_impl->last_event_id[0] = world->event_id;
    }

    ecs_query_impl_t *impl = o->ctx;
    flecs_poly_assert(impl, ecs_query_t);
    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_table_t *table = it->table;
    ecs_entity_t event = it->event;

    if (event == EcsOnTableCreate) {
        /* Creation of new table */
        if (flecs_query_cache_match_table(world, cache, table)) {
            if (ecs_should_log_3()) {
                char *table_str = ecs_table_str(world, table);
                ecs_dbg_3("query cache event: %s for [%s]", 
                    ecs_get_name(world, event),
                    table_str);
                ecs_os_free(table_str);
            }
        }
        return;
    }

    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);

    /* The observer isn't doing the matching because the query can do it more
     * efficiently by checking the table with the query cache. */
    if (ecs_map_get(&cache->tables, table->id) == NULL) {
        return;
    }

    if (ecs_should_log_3()) {
        char *table_str = ecs_table_str(world, table);
        ecs_dbg_3("query cache event: %s for [%s]", 
            ecs_get_name(world, event),
            table_str);
        ecs_os_free(table_str);
    }

    if (event == EcsOnTableDelete) {
        /* Deletion of table */
        flecs_query_cache_remove_table(cache, table);
        return;
    }
}

/* Create query-specific allocators. The reason these allocators are 
 * specific to the query is because they depend on the number of terms the
 * query has. */
static
void flecs_query_cache_allocators_init(
    ecs_query_cache_t *cache)
{
    int32_t field_count = cache->query->field_count;
    if (field_count) {
        flecs_ballocator_init(&cache->allocators.pointers, 
            field_count * ECS_SIZEOF(ecs_table_record_t*));
        flecs_ballocator_init(&cache->allocators.ids, 
            field_count * ECS_SIZEOF(ecs_id_t));
        flecs_ballocator_init(&cache->allocators.monitors,
            (1 + field_count) * ECS_SIZEOF(int32_t));
    }
}

/* Free query-specific allocators. */
static
void flecs_query_cache_allocators_fini(
    ecs_query_cache_t *cache)
{
    int32_t field_count = cache->query->field_count;
    if (field_count) {
        flecs_ballocator_fini(&cache->allocators.pointers);
        flecs_ballocator_fini(&cache->allocators.ids);
        flecs_ballocator_fini(&cache->allocators.monitors);
    }
}

/* Free cache. */
void flecs_query_cache_fini(
    ecs_query_impl_t *impl)
{
    ecs_world_t *world = impl->pub.world;
    ecs_stage_t *stage = impl->stage;
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_t *cache = impl->cache;
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);

    if (cache->observer) {
        flecs_observer_fini(cache->observer);
    }

    ecs_group_delete_action_t on_delete = cache->on_group_delete;
    if (on_delete) {
        ecs_map_iter_t it = ecs_map_iter(&cache->groups);
        while (ecs_map_next(&it)) {
            ecs_query_cache_group_t *group = ecs_map_ptr(&it);
            uint64_t group_id = ecs_map_key(&it);
            on_delete(world, group_id, group->info.ctx, cache->group_by_ctx);
        }
        cache->on_group_delete = NULL;
    }

    if (cache->group_by_ctx_free) {
        if (cache->group_by_ctx) {
            cache->group_by_ctx_free(cache->group_by_ctx);
        }
    }

    flecs_query_cache_for_each_component_monitor(world, impl, cache,
        flecs_monitor_unregister);

    flecs_query_cache_remove_all_tables(cache);

    ecs_assert(ecs_map_count(&cache->tables) == 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_map_count(&cache->groups) == 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_map_fini(&cache->tables);
    ecs_map_fini(&cache->groups);
    ecs_vec_fini_t(NULL, &cache->table_slices, ecs_query_cache_match_t);
    
    if (cache->query->term_count) {
        flecs_bfree(&cache->allocators.ids, cache->sources);
    }

    flecs_query_cache_allocators_fini(cache);
    ecs_query_fini(cache->query);

    flecs_bfree(&stage->allocators.query_cache, cache);
}

/* Create new cache. */
ecs_query_cache_t* flecs_query_cache_init(
    ecs_query_impl_t *impl,
    const ecs_query_desc_t *const_desc)
{
    ecs_world_t *world = impl->pub.real_world;
    flecs_poly_assert(world, ecs_world_t);

    ecs_stage_t *stage = impl->stage;
    flecs_poly_assert(stage, ecs_stage_t);

    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(const_desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(const_desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_query_desc_t was not initialized to zero");
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, 
        "cannot create query during world fini");

    /* Create private version of desc to create the uncached query that will
     * populate the query cache. */
    ecs_query_desc_t desc = *const_desc;
    ecs_entity_t entity = desc.entity;
    desc.cache_kind = EcsQueryCacheNone; /* Don't create caches recursively */
    desc.group_by_callback = NULL;
    desc.group_by = 0;
    desc.order_by_callback = NULL;
    desc.order_by = 0;
    desc.entity = 0;

    /* Don't pass ctx/binding_ctx to uncached query */
    desc.ctx = NULL;
    desc.binding_ctx = NULL;
    desc.ctx_free = NULL;
    desc.binding_ctx_free = NULL;

    ecs_query_cache_t *result = flecs_bcalloc(&stage->allocators.query_cache);
    result->entity = entity;
    impl->cache = result;

    ecs_observer_desc_t observer_desc = { .query = desc };
    observer_desc.query.flags |= EcsQueryNested;

    ecs_flags32_t query_flags = const_desc->flags | world->default_query_flags;
    desc.flags |= EcsQueryMatchEmptyTables | EcsQueryTableOnly | EcsQueryNested;

    /* order_by is not compatible with matching empty tables, as it causes
     * a query to return table slices, not entire tables. */
    if (const_desc->order_by_callback) {
        query_flags &= ~EcsQueryMatchEmptyTables;
    }

    ecs_query_t *q = result->query = ecs_query_init(world, &desc);
    if (!q) {
        goto error;
    }

    /* Set flag for trivial caches which allows for faster iteration */
    if (impl->pub.flags & EcsQueryIsCacheable) {
        /* Trivial caches may only contain And/Not operators. */
        int32_t t, count = q->term_count;
        for (t = 0; t < count; t ++) {
            if (q->terms[t].oper != EcsAnd && q->terms[t].oper != EcsNot && q->terms[t].oper != EcsOptional) {
                break;
            }
        }

        if ((t == count) && (q->flags & EcsQueryMatchOnlySelf) &&
           !(q->flags & EcsQueryMatchWildcards) &&
           !(q->flags & EcsQueryCacheWithFilter))
        {
            if (!const_desc->order_by && !const_desc->group_by && 
                !const_desc->order_by_callback && 
                !const_desc->group_by_callback &&
                !(const_desc->flags & EcsQueryDetectChanges))
            {
                
                q->flags |= EcsQueryTrivialCache;
            }
        }
    }

    if (const_desc->flags & EcsQueryDetectChanges) {
        for (int i = 0; i < q->term_count; i ++) {
            ecs_term_t *term = &q->terms[i];

            /* If query has change detection, flag this on the component record. 
             * This allows code to skip calling modified() if there are no OnSet
             * hooks/observers, and the component isn't used in any queries that use
             * change detection. */
            
            ecs_component_record_t *cr = 
                flecs_components_ensure(world, term->id);
            cr->flags |= EcsIdHasOnSet;

            if (term->id < FLECS_HI_COMPONENT_ID) {
                world->non_trivial_set[term->id] = true;
            }
        }
    }

    if (const_desc->order_by) {
        ecs_component_record_t *cr = 
            flecs_components_ensure(world, const_desc->order_by);
        if (cr) {
            cr->flags |= EcsIdHasOnSet;

            if (const_desc->order_by < FLECS_HI_COMPONENT_ID) {
                world->non_trivial_set[const_desc->order_by] = true;
            }
        }
    }

    ecs_size_t elem_size = flecs_query_cache_elem_size(result);
    ecs_vec_init(&world->allocator, &result->default_group.tables, 
        elem_size, 0);
    result->first_group = &result->default_group;

    /* The uncached query used to populate the cache always matches empty 
     * tables. This flag determines whether the empty tables are stored 
     * separately in the cache or are treated as regular tables. This is only
     * enabled if the user requested that the query matches empty tables. */
    ECS_BIT_COND(q->flags, EcsQueryCacheYieldEmptyTables, 
        !!(query_flags & EcsQueryMatchEmptyTables));

    flecs_query_cache_allocators_init(result);

    /* Zero'd out sources array that's used for results that only match $this. 
     * This reduces the amount of memory used by the cache, and improves CPU
     * cache locality during iteration when doing source checks. */
    if (result->query->term_count) {
        result->sources = flecs_bcalloc(&result->allocators.ids);
    }

    if (q->term_count) {
        observer_desc.run = flecs_query_cache_on_event;
        observer_desc.ctx = impl;

        int32_t event_index = 0;
        observer_desc.events[event_index ++] = EcsOnTableCreate;
        observer_desc.events[event_index ++] = EcsOnTableDelete;
        observer_desc.flags_ = EcsObserverBypassQuery;

        /* ecs_query_init could have moved away resources from the terms array
         * in the descriptor, so use the terms array from the query. */
        ecs_os_memcpy_n(observer_desc.query.terms, q->terms, 
            ecs_term_t, q->term_count);
        observer_desc.query.expr = NULL; /* Already parsed */
        observer_desc.query.flags |= EcsQueryTableOnly;

        result->observer = flecs_observer_init(world, entity, &observer_desc);
        if (!result->observer) {
            goto error;
        }
    }

    result->prev_match_count = -1;

    if (ecs_should_log_1()) {
        char *query_expr = ecs_query_str(result->query);
        ecs_dbg_1("#[green]query#[normal] [%s] created", 
            query_expr ? query_expr : "");
        ecs_os_free(query_expr);
    }

    ecs_log_push_1();

    if (flecs_query_cache_process_query(world, impl, result)) {
        goto error;
    }

    /* Group before matching so we won't have to move tables around later */
    int32_t cascade_by = result->cascade_by;
    if (cascade_by) {
        flecs_query_cache_group_by(result, result->query->terms[cascade_by - 1].id,
            flecs_query_cache_group_by_cascade);
        result->group_by_ctx = &result->query->terms[cascade_by - 1];
    }

    if (const_desc->group_by_callback || const_desc->group_by) {
        ecs_check(!result->cascade_by, ECS_INVALID_PARAMETER,
            "cannot mix cascade and group_by");
        flecs_query_cache_group_by(result, 
            const_desc->group_by, const_desc->group_by_callback);
        result->group_by_ctx = const_desc->group_by_ctx;
        result->on_group_create = const_desc->on_group_create;
        result->on_group_delete = const_desc->on_group_delete;
        result->group_by_ctx_free = const_desc->group_by_ctx_free;
    }

    ecs_map_init(&result->tables, &world->allocator);
    flecs_query_cache_match_tables(world, result);

    if (const_desc->order_by_callback) {
        if (flecs_query_cache_order_by(world, impl, 
            const_desc->order_by, const_desc->order_by_callback,
            const_desc->order_by_table_callback))
        {
            goto error;
        }
    }

    if (entity) {
        if (!ecs_map_count(&result->tables) && result->query->term_count){
            ecs_add_id(world, entity, EcsEmpty);
        }
    }

    ecs_log_pop_1();

    return result;
error:
    return NULL;
}
