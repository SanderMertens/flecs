/**
 * @file query/cache/cache.c
 * @brief Cached query implementation.
 *
 * Pre-computes matched tables so iteration avoids searching. Uses a derived
 * "cache query" with only cacheable terms. An observer syncs the cache on
 * table create/delete; up-traversal queries rematch when ancestors change.
 *
 * Trivial caches (no wildcards/traversal, And/Not/Optional only) use smaller
 * storage and a faster iterator. group_by partitions tables by user callback
 * (cascade uses hierarchy depth). order_by sorts entities across tables.
 * Wildcard matches are stored in wildcard_matches on the first cache element.
 */

#include "../../private_api.h"

/* Check if cache is trivial. */
bool flecs_query_cache_is_trivial(
    const ecs_query_cache_t *cache)
{
    return (cache->query->flags & EcsQueryTrivialCache) != 0;
}

/* Return cache element size based on whether cache is trivial. */
ecs_size_t flecs_query_cache_elem_size(
    const ecs_query_cache_t *cache) 
{
    return flecs_query_cache_is_trivial(cache) 
        ? ECS_SIZEOF(ecs_query_triv_cache_match_t) 
        : ECS_SIZEOF(ecs_query_cache_match_t)
        ;
}

/* Group tables by relationship target using the default group_by logic. */
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

/* Group tables by hierarchy depth for cascade-based breadth-first iteration. */
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

/* Initialize cache by matching all existing tables. */
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

/* Match a single table against the cache query. */
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

/* Iterate component monitors for fields that use relationship traversal. */
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

/* Process query terms to initialize cascade and ref bookkeeping. */
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

/* Configure order_by sorting for the query cache. */
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

/* Configure group_by grouping for the query cache. */
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

/* Observer callback: syncs cache on table create/delete events. */
static
void flecs_query_cache_on_event(
    ecs_iter_t *it)
{
    /* Deduplicate: run callbacks don't get automatic dedup from the framework */
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

    /* Skip tables not in the cache (observer fires broadly, cache checks cheaply) */
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

/* Initialize query-specific block allocators sized by field count. */
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

/* Free query-specific block allocators. */
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

/* Free query cache and all associated resources. */
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

/* Create and initialize a new query cache. */
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

    /* Check if this is a trivial cache (enables faster iteration path) */
    if (impl->pub.flags & EcsQueryIsCacheable) {
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

            /* Flag component record so modified() calls aren't skipped for
             * components used in change-detection queries. */
            
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

    /* The cache query always matches empty tables. This flag controls whether
     * empty tables are exposed to the user during iteration. */
    ECS_BIT_COND(q->flags, EcsQueryCacheYieldEmptyTables, 
        !!(query_flags & EcsQueryMatchEmptyTables));

    flecs_query_cache_allocators_init(result);

    /* Shared zero-initialized sources array for $this-only matches.
     * Saves memory and improves cache locality during iteration. */
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
