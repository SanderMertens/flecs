/**
 * @file query/engine/cache.c
 * @brief Cached query implementation.
 */

#include "../../private_api.h"

int32_t flecs_query_cache_table_count(
    ecs_query_cache_t *cache)
{
    ecs_run_aperiodic(cache->query->world, EcsAperiodicEmptyTables);
    return cache->cache.tables.count;
}

int32_t flecs_query_cache_empty_table_count(
    ecs_query_cache_t *cache)
{
    ecs_run_aperiodic(cache->query->world, EcsAperiodicEmptyTables);
    return cache->cache.empty_tables.count;
}

int32_t flecs_query_cache_entity_count(
    const ecs_query_cache_t *cache)
{
    ecs_run_aperiodic(cache->query->world, EcsAperiodicEmptyTables);
    
    int32_t result = 0;
    ecs_table_cache_hdr_t *cur, *last = cache->cache.tables.last;
    if (!last) {
        return 0;
    }

    for (cur = cache->cache.tables.first; cur != NULL; cur = cur->next) {
        result += ecs_table_count(cur->table);
    }

    return result;
}

static
uint64_t flecs_query_cache_get_group_id(
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    if (cache->group_by_callback) {
        return cache->group_by_callback(cache->query->world, table, 
            cache->group_by, cache->group_by_ctx);
    } else {
        return 0;
    }
}

static
void flecs_query_cache_compute_group_id(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (cache->group_by_callback) {
        ecs_table_t *table = match->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        match->group_id = flecs_query_cache_get_group_id(cache, table);
    } else {
        match->group_id = 0;
    }
}

static
ecs_query_cache_table_list_t* flecs_query_cache_get_group(
    const ecs_query_cache_t *cache,
    uint64_t group_id)
{
    return ecs_map_get_deref(
        &cache->groups, ecs_query_cache_table_list_t, group_id);
}

static
ecs_query_cache_table_list_t* flecs_query_cache_ensure_group(
    ecs_query_cache_t *cache,
    uint64_t id)
{
    ecs_query_cache_table_list_t *group = ecs_map_get_deref(&cache->groups, 
        ecs_query_cache_table_list_t, id);

    if (!group) {
        group = ecs_map_insert_alloc_t(&cache->groups, 
            ecs_query_cache_table_list_t, id);
        ecs_os_zeromem(group);
        if (cache->on_group_create) {
            group->info.ctx = cache->on_group_create(
                cache->query->world, id, cache->group_by_ctx);
        }
    }

    return group;
}

static
void flecs_query_cache_remove_group(
    ecs_query_cache_t *cache,
    uint64_t id)
{
    if (cache->on_group_delete) {
        ecs_query_cache_table_list_t *group = ecs_map_get_deref(&cache->groups, 
            ecs_query_cache_table_list_t, id);
        if (group) {
            cache->on_group_delete(cache->query->world, id, 
                group->info.ctx, cache->group_by_ctx);
        }
    }

    ecs_map_remove_free(&cache->groups, id);
}

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
        return ecs_pair_second(world, match);
    }
    return 0;
}

/* Find the last node of the group after which this group should be inserted */
static
ecs_query_cache_table_match_t* flecs_query_cache_find_group_insertion_node(
    ecs_query_cache_t *cache,
    uint64_t group_id)
{
    /* Grouping must be enabled */
    ecs_assert(cache->group_by_callback != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_iter_t it = ecs_map_iter(&cache->groups);
    ecs_query_cache_table_list_t *list, *closest_list = NULL;
    uint64_t id, closest_id = 0;
    
    bool desc = false;

    if (cache->cascade_by) {
        desc = (cache->query->terms[
            cache->cascade_by - 1].src.id & EcsDesc) != 0;
    }

    /* Find closest smaller group id */
    while (ecs_map_next(&it)) {
        id = ecs_map_key(&it);

        if (!desc) {
            if (id >= group_id) {
                continue;
            }
        } else {
            if (id <= group_id) {
                continue;
            }
        }

        list = ecs_map_ptr(&it);
        if (!list->last) {
            ecs_assert(list->first == NULL, ECS_INTERNAL_ERROR, NULL);
            continue;
        }

        bool comp;
        if (!desc) {
            comp = ((group_id - id) < (group_id - closest_id));
        } else {
            comp = ((group_id - id) > (group_id - closest_id));
        }

        if (!closest_list || comp) {
            closest_id = id;
            closest_list = list;
        }
    }

    if (closest_list) {
        return closest_list->last;
    } else {
        return NULL; /* Group should be first in query */
    }
}

/* Initialize group with first node */
static
void flecs_query_cache_create_group(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *match)
{
    uint64_t group_id = match->group_id;

    /* If query has grouping enabled & this is a new/empty group, find
     * the insertion point for the group */
    ecs_query_cache_table_match_t *insert_after = 
        flecs_query_cache_find_group_insertion_node(cache, group_id);

    if (!insert_after) {
        /* This group should appear first in the query list */
        ecs_query_cache_table_match_t *query_first = cache->list.first;
        if (query_first) {
            /* If this is not the first match for the query, insert before it */
            match->next = query_first;
            query_first->prev = match;
            cache->list.first = match;
        } else {
            /* If this is the first match of the query, initialize its list */
            ecs_assert(cache->list.last == NULL, ECS_INTERNAL_ERROR, NULL);
            cache->list.first = match;
            cache->list.last = match;
        }
    } else {
        ecs_assert(cache->list.first != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cache->list.last != NULL, ECS_INTERNAL_ERROR, NULL);

        /* This group should appear after another group */
        ecs_query_cache_table_match_t *insert_before = insert_after->next;
        match->prev = insert_after;
        insert_after->next = match;
        match->next = insert_before;
        if (insert_before) {
            insert_before->prev = match;
        } else {
            ecs_assert(cache->list.last == insert_after, 
                ECS_INTERNAL_ERROR, NULL);
                
            /* This group should appear last in the query list */
            cache->list.last = match;
        }
    }
}

/* Find the list the node should be part of */
static
ecs_query_cache_table_list_t* flecs_query_cache_get_node_list(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *match)
{
    if (cache->group_by_callback) {
        return flecs_query_cache_get_group(cache, match->group_id);
    } else {
        return &cache->list;
    }
}

/* Find or create the list the node should be part of */
static
ecs_query_cache_table_list_t* flecs_query_cache_ensure_node_list(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *match)
{
    if (cache->group_by_callback) {
        return flecs_query_cache_ensure_group(cache, match->group_id);
    } else {
        return &cache->list;
    }
}

/* Remove node from list */
static
void flecs_query_cache_remove_table_node(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *match)
{
    ecs_query_cache_table_match_t *prev = match->prev;
    ecs_query_cache_table_match_t *next = match->next;

    ecs_assert(prev != match, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(next != match, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!prev || prev != next, ECS_INTERNAL_ERROR, NULL);

    ecs_query_cache_table_list_t *list = 
        flecs_query_cache_get_node_list(cache, match);

    if (!list || !list->first) {
        /* If list contains no matches, the match must be empty */
        ecs_assert(!list || list->last == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(prev == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(next == NULL, ECS_INTERNAL_ERROR, NULL);
        return;
    }

    ecs_assert(prev != NULL || cache->list.first == match, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(next != NULL || cache->list.last == match, 
        ECS_INTERNAL_ERROR, NULL);

    if (prev) {
        prev->next = next;
    }
    if (next) {
        next->prev = prev;
    }

    ecs_assert(list->info.table_count > 0, ECS_INTERNAL_ERROR, NULL);
    list->info.table_count --;

    if (cache->group_by_callback) {
        uint64_t group_id = match->group_id;

        /* Make sure query.list is updated if this is the first or last group */
        if (cache->list.first == match) {
            ecs_assert(prev == NULL, ECS_INTERNAL_ERROR, NULL);
            cache->list.first = next;
            prev = next;
        }
        if (cache->list.last == match) {
            ecs_assert(next == NULL, ECS_INTERNAL_ERROR, NULL);
            cache->list.last = prev;
            next = prev;
        }

        ecs_assert(cache->list.info.table_count > 0, ECS_INTERNAL_ERROR, NULL);
        cache->list.info.table_count --;
        list->info.match_count ++;

        /* Make sure group list only contains nodes that belong to the group */
        if (prev && prev->group_id != group_id) {
            /* The previous node belonged to another group */
            prev = next;
        }
        if (next && next->group_id != group_id) {
            /* The next node belonged to another group */
            next = prev;
        }

        /* Do check again, in case both prev & next belonged to another group */
        if ((!prev && !next) || (prev && prev->group_id != group_id)) {
            /* There are no more matches left in this group */
            flecs_query_cache_remove_group(cache, group_id);
            list = NULL;
        }
    }

    if (list) {
        if (list->first == match) {
            list->first = next;
        }
        if (list->last == match) {
            list->last = prev;
        }
    }

    match->prev = NULL;
    match->next = NULL;

    cache->match_count ++;
}

/* Add node to list */
static
void flecs_query_cache_insert_table_node(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *match)
{
    /* Node should not be part of an existing list */
    ecs_assert(match->prev == NULL && match->next == NULL, 
        ECS_INTERNAL_ERROR, NULL);

    /* If this is the first match, activate system */
    if (!cache->list.first && cache->entity) {
        ecs_remove_id(cache->query->world, cache->entity, EcsEmpty);
    }

    flecs_query_cache_compute_group_id(cache, match);

    ecs_query_cache_table_list_t *list = 
        flecs_query_cache_ensure_node_list(cache, match);
    if (list->last) {
        ecs_assert(cache->list.first != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(cache->list.last != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(list->first != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_query_cache_table_match_t *last = list->last;
        ecs_query_cache_table_match_t *last_next = last->next;

        match->prev = last;
        match->next = last_next;
        last->next = match;

        if (last_next) {
            last_next->prev = match;
        }

        list->last = match;

        if (cache->group_by_callback) {
            /* Make sure to update query list if this is the last group */
            if (cache->list.last == last) {
                cache->list.last = match;
            }
        }
    } else {
        ecs_assert(list->first == NULL, ECS_INTERNAL_ERROR, NULL);

        list->first = match;
        list->last = match;

        if (cache->group_by_callback) {
            /* Initialize group with its first node */
            flecs_query_cache_create_group(cache, match);
        }
    }

    if (cache->group_by_callback) {
        list->info.table_count ++;
        list->info.match_count ++;
    }

    cache->list.info.table_count ++;
    cache->match_count ++;

    ecs_assert(match->prev != match, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(match->next != match, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(list->first != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(list->last != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(list->last == match, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cache->list.first != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cache->list.last != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cache->list.first->prev == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(cache->list.last->next == NULL, ECS_INTERNAL_ERROR, NULL);
}

static
ecs_query_cache_table_match_t* flecs_query_cache_cache_add(
    ecs_world_t *world,
    ecs_query_cache_table_t *elem)
{
    ecs_query_cache_table_match_t *result = 
        flecs_bcalloc(&world->allocators.query_table_match);

    if (!elem->first) {
        elem->first = result;
        elem->last = result;
    } else {
        ecs_assert(elem->last != NULL, ECS_INTERNAL_ERROR, NULL);
        elem->last->next_match = result;
        elem->last = result;
    }

    return result;
}

/* The group by function for cascade computes the tree depth for the table type.
 * This causes tables in the query cache to be ordered by depth, which ensures
 * breadth-first iteration order. */
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

static
void flecs_query_cache_add_ref(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_table_match_t *qm,
    ecs_entity_t component,
    ecs_entity_t entity,
    ecs_size_t size)
{
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_ref_t *ref = ecs_vec_append_t(&world->allocator, &qm->refs, ecs_ref_t);
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    
    if (size) {
        *ref = ecs_ref_init_id(world, entity, component);
    } else {
        *ref = (ecs_ref_t){
            .entity = entity,
            .id = 0
        };
    }

    impl->pub.flags |= EcsQueryHasRefs;
}

static
ecs_query_cache_table_match_t* flecs_query_cache_add_table_match(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_t *qt,
    ecs_table_t *table)
{
    /* Add match for table. One table can have more than one match, if
     * the query contains wildcards. */
    ecs_query_cache_table_match_t *qm = flecs_query_cache_cache_add(
        cache->query->world, qt);
    qm->table = table;

    qm->columns = flecs_balloc(&cache->allocators.columns);
    qm->storage_columns = flecs_balloc(&cache->allocators.columns);
    qm->ids = flecs_balloc(&cache->allocators.ids);
    qm->sources = flecs_balloc(&cache->allocators.sources);

    /* Insert match to iteration list if table is not empty */
    if (!table || ecs_table_count(table) != 0 || 
        (cache->query->flags & EcsQueryCacheYieldEmptyTables))
    {
        flecs_query_cache_insert_table_node(cache, qm);
    }

    return qm;
}

static
void flecs_query_cache_set_table_match(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_t *cache,
    ecs_query_cache_table_match_t *qm,
    ecs_table_t *table,
    ecs_iter_t *it)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_query_t *query = cache->query;
    int32_t i, term_count = query->term_count;
    int32_t field_count = query->field_count;
    ecs_term_t *terms = query->terms;

    /* Reset resources in case this is an existing record */
    ecs_vec_reset_t(a, &qm->refs, ecs_ref_t);
    ecs_os_memcpy_n(qm->columns, it->columns, int32_t, field_count);
    ecs_os_memcpy_n(qm->ids, it->ids, ecs_id_t, field_count);
    ecs_os_memcpy_n(qm->sources, it->sources, ecs_entity_t, field_count);
    qm->set_fields = it->set_fields;
    qm->up_fields = it->up_fields;

    if (table) {
        /* Initialize storage columns for faster access to component storage */
        for (i = 0; i < field_count; i ++) {
            int32_t column = it->columns[i];
            if (!ecs_field_is_set(it, i) || terms[i].inout == EcsInOutNone) {
                qm->storage_columns[i] = -1;
                continue;
            }

            ecs_entity_t src = it->sources[i];
            if (!src) {
                qm->storage_columns[i] = ecs_table_type_to_column_index(
                    table, column);
            } else {
                /* Shared field (not from table) */
                qm->storage_columns[i] = -2;
            }
        }
    }

    /* Add references for substituted terms */
    for (i = 0; i < term_count; i ++) {
        int32_t field = terms[i].field_index;
        ecs_entity_t src = it->sources[field];
        ecs_size_t size = 0;
        if (it->sizes) {
            size = it->sizes[field];
        }
        if (src) {
            ecs_id_t id = it->ids[field];
            ecs_assert(ecs_is_valid(world, src), ECS_INTERNAL_ERROR, NULL);

            if (id) {
                flecs_query_cache_add_ref(
                    world, impl, qm, id, src, size);

                /* Use column index to bind term and ref */
                if (qm->set_fields & (1llu << field)) {
                    qm->columns[field] = ecs_vec_count(&qm->refs) - 1;
                }
            }
        }
    }
}

static
ecs_query_cache_table_t* flecs_query_cache_table_insert(
    ecs_world_t *world,
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    ecs_query_cache_table_t *qt = flecs_bcalloc(&world->allocators.query_table);
    if (table) {
        qt->table_id = table->id;
    } else {
        qt->table_id = 0;
    }

    if (cache->query->flags & EcsQueryCacheYieldEmptyTables) {
        ecs_table_cache_insert_w_empty(&cache->cache, table, &qt->hdr, false);
    } else {
        ecs_table_cache_insert(&cache->cache, table, &qt->hdr);
    }

    return qt;
}

/** Populate query cache with tables */
static
void flecs_query_cache_match_tables(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_t *cache)
{
    ecs_table_t *table = NULL;
    ecs_query_cache_table_t *qt = NULL;

    ecs_iter_t it = ecs_query_iter(world, cache->query);
    ECS_BIT_SET(it.flags, EcsIterIsInstanced);
    ECS_BIT_SET(it.flags, EcsIterNoData);
    ECS_BIT_SET(it.flags, EcsIterTableOnly);

    while (ecs_query_next(&it)) {
        if ((table != it.table) || (!it.table && !qt)) {
            /* New table matched, add record to cache */
            table = it.table;
            qt = flecs_query_cache_table_insert(world, cache, table);
        }

        ecs_query_cache_table_match_t *qm = 
            flecs_query_cache_add_table_match(cache, qt, table);
        flecs_query_cache_set_table_match(world, impl, cache, qm, table, &it);
    }
}

static
bool flecs_query_cache_match_table(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    if (!ecs_map_is_init(&cache->cache.index)) {
        return false;
    }

    ecs_query_cache_table_t *qt = NULL;
    ecs_query_t *q = cache->query;

    /* Iterate uncached query for table to check if it matches. If this is a
     * wildcard query, a table can match multiple times. */
    ecs_iter_t it = flecs_query_iter(world, q);
    it.flags |= EcsIterIsInstanced;
    it.flags |= EcsIterNoData;
    ecs_iter_set_var_as_table(&it, 0, table);

    while (ecs_query_next(&it)) {
        ecs_assert(it.table == table, ECS_INTERNAL_ERROR, NULL);
        if (qt == NULL) {
            table = it.table;
            qt = flecs_query_cache_table_insert(world, cache, table);
        }

        ecs_query_cache_table_match_t *qm = flecs_query_cache_add_table_match(
            cache, qt, table);
        flecs_query_cache_set_table_match(world, impl, cache, qm, table, &it);
    }

    return qt != NULL;
}

static
bool flecs_query_cache_has_refs(
    ecs_query_cache_t *cache)
{
    ecs_term_t *terms = cache->query->terms;
    int32_t i, count = cache->query->term_count;
    for (i = 0; i < count; i ++) {
        if (terms[i].src.id & (EcsUp | EcsIsEntity)) {
            return true;
        }
    }

    return false;
}

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
            callback(world, term->id, q);
        }
    }
}

static
bool flecs_query_cache_is_term_ref_supported(
    ecs_term_ref_t *ref)
{
    if (!(ref->id & EcsIsVariable)) {
        return true;
    }
    if (ecs_id_is_wildcard(ref->id)) {
        return true;
    }
    return false;
}

static
int flecs_query_cache_process_signature(
    ecs_world_t *world,
    ecs_query_impl_t *impl,
    ecs_query_cache_t *cache)
{
    ecs_assert(cache != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_term_t *terms = cache->query->terms;
    int32_t i, count = cache->query->term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_ref_t *first = &term->first;
        ecs_term_ref_t *src = &term->src;
        ecs_term_ref_t *second = &term->second;

        bool is_src_ok = flecs_query_cache_is_term_ref_supported(src);
        bool is_first_ok = flecs_query_cache_is_term_ref_supported(first);
        bool is_second_ok = flecs_query_cache_is_term_ref_supported(second);

        (void)first;
        (void)second;
        (void)is_src_ok;
        (void)is_first_ok;
        (void)is_second_ok;

        /* Queries do not support named variables */
        ecs_check(is_src_ok || ecs_term_match_this(term),
            ECS_UNSUPPORTED, NULL);
        ecs_check(is_first_ok, ECS_UNSUPPORTED, NULL);
        ecs_check(is_second_ok,  ECS_UNSUPPORTED, NULL);
        ecs_check(term->inout != EcsInOutFilter, ECS_INVALID_PARAMETER,
            "invalid usage of InOutFilter for query");

        if (src->id & EcsCascade) {
            ecs_assert(cache->cascade_by == 0, ECS_INVALID_PARAMETER,
                "query can only have one cascade term");
            cache->cascade_by = i + 1;
        }
    }

    impl->pub.flags |= 
        (ecs_flags32_t)(flecs_query_cache_has_refs(cache) * EcsQueryHasRefs);

    flecs_query_cache_for_each_component_monitor(
        world, impl, cache, flecs_monitor_register);

    return 0;
error:
    return -1;
}

/** When a table becomes empty remove it from the query list, or vice versa. */
static
void flecs_query_cache_update_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table,
    bool empty)
{
    int32_t prev_count = flecs_query_cache_table_count(cache);
    ecs_table_cache_set_empty(&cache->cache, table, empty);
    int32_t cur_count = flecs_query_cache_table_count(cache);

    if (prev_count != cur_count) {
        ecs_query_cache_table_t *qt = ecs_table_cache_get(&cache->cache, table);
        ecs_assert(qt != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_query_cache_table_match_t *cur, *next;

        for (cur = qt->first; cur != NULL; cur = next) {
            next = cur->next_match;

            if (empty) {
                ecs_assert(ecs_table_count(table) == 0, 
                    ECS_INTERNAL_ERROR, NULL);

                flecs_query_cache_remove_table_node(cache, cur);
            } else {
                ecs_assert(ecs_table_count(table) != 0, 
                    ECS_INTERNAL_ERROR, NULL);

                flecs_query_cache_insert_table_node(cache, cur);
            }
        }
    }

    ecs_assert(cur_count || cache->list.first == NULL, 
        ECS_INTERNAL_ERROR, NULL);
}

/* Remove table */
static
void flecs_query_cache_table_match_free(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_t *elem,
    ecs_query_cache_table_match_t *first)
{
    ecs_query_cache_table_match_t *cur, *next;
    ecs_world_t *world = cache->query->world;

    for (cur = first; cur != NULL; cur = next) {
        flecs_bfree(&cache->allocators.columns, cur->columns);
        flecs_bfree(&cache->allocators.columns, cur->storage_columns);
        flecs_bfree(&cache->allocators.ids, cur->ids);
        flecs_bfree(&cache->allocators.sources, cur->sources);

        if (cur->monitor) {
            flecs_bfree(&cache->allocators.monitors, cur->monitor);
        }
        if (!elem->hdr.empty) {
            flecs_query_cache_remove_table_node(cache, cur);
        }
        
        ecs_vec_fini_t(&world->allocator, &cur->refs, ecs_ref_t);

        next = cur->next_match;

        flecs_bfree(&world->allocators.query_table_match, cur);
    }
}

static
void flecs_query_cache_table_free(
    ecs_query_cache_t *cache,
    ecs_query_cache_table_t *elem)
{
    flecs_query_cache_table_match_free(cache, elem, elem->first);
    flecs_bfree(&cache->query->world->allocators.query_table, elem);
}

static
void flecs_query_cache_unmatch_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table,
    ecs_query_cache_table_t *elem)
{
    if (!elem) {
        elem = ecs_table_cache_get(&cache->cache, table);
    }
    if (elem) {
        ecs_table_cache_remove(&cache->cache, elem->table_id, &elem->hdr);
        flecs_query_cache_table_free(cache, elem);
    }
}

/* Rematch system with tables after a change happened to a watched entity */
static
void flecs_query_cache_rematch_tables(
    ecs_world_t *world,
    ecs_query_impl_t *impl)
{
    ecs_iter_t it;
    ecs_table_t *table = NULL;
    ecs_query_cache_table_t *qt = NULL;
    ecs_query_cache_table_match_t *qm = NULL;
    ecs_query_cache_t *cache = impl->cache;

    if (cache->monitor_generation == world->monitor_generation) {
        return;
    }

    cache->monitor_generation = world->monitor_generation;

    it = ecs_query_iter(world, cache->query);
    ECS_BIT_SET(it.flags, EcsIterIsInstanced);
    ECS_BIT_SET(it.flags, EcsIterNoData);

    world->info.rematch_count_total ++;
    int32_t rematch_count = ++ cache->rematch_count;

    ecs_time_t t = {0};
    if (world->flags & EcsWorldMeasureFrameTime) {
        ecs_time_measure(&t);
    }

    while (ecs_query_next(&it)) {
        if ((table != it.table) || (!it.table && !qt)) {
            if (qm && qm->next_match) {
                flecs_query_cache_table_match_free(cache, qt, qm->next_match);
                qm->next_match = NULL;
            }

            table = it.table;

            qt = ecs_table_cache_get(&cache->cache, table);
            if (!qt) {
                qt = flecs_query_cache_table_insert(world, cache, table);
            }

            ecs_assert(qt->hdr.table == table, ECS_INTERNAL_ERROR, NULL);
            qt->rematch_count = rematch_count;
            qm = NULL;
        }
        if (!qm) {
            qm = qt->first;
        } else {
            qm = qm->next_match;
        }
        if (!qm) {
            qm = flecs_query_cache_add_table_match(cache, qt, table);
        }

        flecs_query_cache_set_table_match(world, impl, cache, qm, table, &it);

        if (table && ecs_table_count(table) && cache->group_by_callback) {
            if (flecs_query_cache_get_group_id(cache, table) != qm->group_id) {
                /* Update table group */
                flecs_query_cache_remove_table_node(cache, qm);
                flecs_query_cache_insert_table_node(cache, qm);
            }
        }
    }

    if (qm && qm->next_match) {
        flecs_query_cache_table_match_free(cache, qt, qm->next_match);
        qm->next_match = NULL;
    }

    /* Iterate all tables in cache, remove ones that weren't just matched */
    ecs_table_cache_iter_t cache_it;
    if (flecs_table_cache_all_iter(&cache->cache, &cache_it)) {
        while ((qt = flecs_table_cache_next(&cache_it, ecs_query_cache_table_t))) {
            if (qt->rematch_count != rematch_count) {
                flecs_query_cache_unmatch_table(cache, qt->hdr.table, qt);
            }
        }
    }

    if (world->flags & EcsWorldMeasureFrameTime) {
        world->info.rematch_time_total += (ecs_ftime_t)ecs_time_measure(&t);
    }
}

/* -- Private API -- */

void flecs_query_cache_notify(
    ecs_world_t *world,
    ecs_query_t *q,
    ecs_query_cache_event_t *event)
{
    flecs_poly_assert(q, ecs_query_t);
    ecs_query_impl_t *impl = flecs_query_impl(q);
    ecs_query_cache_t *cache = impl->cache;

    switch(event->kind) {
    case EcsQueryTableMatch:
        /* Creation of new table */
        flecs_query_cache_match_table(world, impl, cache, event->table);
        break;
    case EcsQueryTableUnmatch:
        /* Deletion of table */
        flecs_query_cache_unmatch_table(cache, event->table, NULL);
        break;
    case EcsQueryTableRematch:
        /* Rematch tables of query */
        flecs_query_cache_rematch_tables(world, impl);
        break;
    }
}

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
        ECS_INVALID_PARAMETER, NULL);

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

    ecs_vec_fini_t(NULL, &cache->table_slices, ecs_query_cache_table_match_t);
    flecs_query_cache_sort_tables(world, impl);

    if (!cache->table_slices.array) {
        flecs_query_cache_build_sorted_tables(cache);
    }

    return 0;
error:
    return -1;
}

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

    ecs_map_init_w_params(&cache->groups, 
        &cache->query->world->allocators.query_table_list);
error:
    return;
}

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
        if (flecs_query_cache_match_table(world, impl, cache, table)) {
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
    if (ecs_table_cache_get(&cache->cache, table) == NULL) {
        return;
    }

    if (ecs_should_log_3()) {
        char *table_str = ecs_table_str(world, table);
        ecs_dbg_3("query cache event: %s for [%s]", 
            ecs_get_name(world, event),
            table_str);
        ecs_os_free(table_str);
    }

    if (event == EcsOnTableEmpty) {
        flecs_query_cache_update_table(cache, table, true);
    } else
    if (event == EcsOnTableFill) {
        flecs_query_cache_update_table(cache, table, false);
    } else if (event == EcsOnTableDelete) {
        /* Deletion of table */
        flecs_query_cache_unmatch_table(cache, table, NULL);
        return;
    }
}

static
void flecs_query_cache_table_cache_free(
    ecs_query_cache_t *cache)
{
    ecs_table_cache_iter_t it;
    ecs_query_cache_table_t *qt;

    if (flecs_table_cache_all_iter(&cache->cache, &it)) {
        while ((qt = flecs_table_cache_next(&it, ecs_query_cache_table_t))) {
            flecs_query_cache_table_free(cache, qt);
        }
    }

    ecs_table_cache_fini(&cache->cache);
}

static
void flecs_query_cache_allocators_init(
    ecs_query_cache_t *cache)
{
    int32_t field_count = cache->query->field_count;
    if (field_count) {
        flecs_ballocator_init(&cache->allocators.columns, 
            field_count * ECS_SIZEOF(int32_t));
        flecs_ballocator_init(&cache->allocators.ids, 
            field_count * ECS_SIZEOF(ecs_id_t));
        flecs_ballocator_init(&cache->allocators.sources,
            field_count * ECS_SIZEOF(ecs_entity_t));
        flecs_ballocator_init(&cache->allocators.monitors,
            (1 + field_count) * ECS_SIZEOF(int32_t));
    }
}

static
void flecs_query_cache_allocators_fini(
    ecs_query_cache_t *cache)
{
    int32_t field_count = cache->query->field_count;
    if (field_count) {
        flecs_ballocator_fini(&cache->allocators.columns);
        flecs_ballocator_fini(&cache->allocators.ids);
        flecs_ballocator_fini(&cache->allocators.sources);
        flecs_ballocator_fini(&cache->allocators.monitors);
    }
}

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
            ecs_query_cache_table_list_t *group = ecs_map_ptr(&it);
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
    flecs_query_cache_table_cache_free(cache);

    ecs_map_fini(&cache->groups);

    ecs_vec_fini_t(NULL, &cache->table_slices, ecs_query_cache_table_match_t);
    flecs_query_cache_allocators_fini(cache);
    ecs_query_fini(cache->query);

    flecs_bfree(&stage->allocators.query_cache, cache);
}

/* -- Public API -- */

ecs_query_cache_t* flecs_query_cache_init(
    ecs_query_impl_t *impl,
    const ecs_query_desc_t *const_desc)
{
    ecs_world_t *world = impl->pub.real_world;
    ecs_stage_t *stage = impl->stage;
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

    ecs_flags32_t query_flags = const_desc->flags | world->default_query_flags;
    desc.flags |= EcsQueryMatchEmptyTables | EcsQueryTableOnly;
    ecs_query_t *q = result->query = ecs_query_init(world, &desc);
    if (!q) {
        goto error;
    }

    /* The uncached query used to populate the cache always matches empty 
     * tables. This flag determines whether the empty tables are stored 
     * separately in the cache or are treated as regular tables. This is only
     * enabled if the user requested that the query matches empty tables. */
    ECS_BIT_COND(q->flags, EcsQueryCacheYieldEmptyTables, 
        !!(query_flags & EcsQueryMatchEmptyTables));

    flecs_query_cache_allocators_init(result);

    if (q->term_count) {
        observer_desc.run = flecs_query_cache_on_event;
        observer_desc.ctx = impl;

        int32_t event_index = 0;
        if (!(q->flags & EcsQueryCacheYieldEmptyTables)) {
            observer_desc.events[event_index ++] = EcsOnTableEmpty;
            observer_desc.events[event_index ++] = EcsOnTableFill;
        }

        observer_desc.events[event_index ++] = EcsOnTableCreate;
        observer_desc.events[event_index ++] = EcsOnTableDelete;
        observer_desc.query.flags |= EcsQueryNoData|EcsQueryIsInstanced;
        observer_desc.flags_ = EcsObserverBypassQuery;

        /* ecs_query_init could have moved away resources from the terms array
         * in the descriptor, so use the terms array from the query. */
        ecs_os_memcpy_n(observer_desc.query.terms, q->terms, 
            ecs_term_t, FLECS_TERM_COUNT_MAX);
        observer_desc.query.expr = NULL; /* Already parsed */

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

    if (flecs_query_cache_process_signature(world, impl, result)) {
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

    /* Ensure that while initially populating the query with tables, they are
     * in the right empty/non-empty list. This ensures the query won't miss
     * empty/non-empty events for tables that are currently out of sync, but
     * change back to being in sync before processing pending events. */
    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);
    ecs_table_cache_init(world, &result->cache);
    flecs_query_cache_match_tables(world, impl, result);

    if (const_desc->order_by_callback) {
        if (flecs_query_cache_order_by(world, impl, 
            const_desc->order_by, const_desc->order_by_callback,
            const_desc->order_by_table_callback))
        {
            goto error;
        }
    }

    if (entity) {
        if (!flecs_query_cache_table_count(result) && result->query->term_count){
            ecs_add_id(world, entity, EcsEmpty);
        }
    }

    ecs_log_pop_1();

    return result;
error:
    return NULL;
}

ecs_query_cache_table_t* flecs_query_cache_get_table(
    ecs_query_cache_t *cache,
    ecs_table_t *table)
{
    return ecs_table_cache_get(&cache->cache, table);
}

void ecs_iter_set_group(
    ecs_iter_t *it,
    uint64_t group_id)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv_.iter.query;
    ecs_query_impl_t *q = flecs_query_impl(qit->query);
    ecs_check(q != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_poly_assert(q, ecs_query_t);
    ecs_query_cache_t *cache = q->cache;
    ecs_check(cache != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_query_cache_table_list_t *node = flecs_query_cache_get_group(
        cache, group_id);
    if (!node) {
        qit->node = NULL;
        qit->last = NULL;
        return;
    }

    ecs_query_cache_table_match_t *first = node->first;
    if (first) {
        qit->node = node->first;
        qit->last = node->last;
    } else {
        qit->node = NULL;
        qit->last = NULL;
    }
    
error:
    return;
}

const ecs_query_group_info_t* ecs_query_get_group_info(
    const ecs_query_t *query,
    uint64_t group_id)
{
    flecs_poly_assert(query, ecs_query_t);
    ecs_query_cache_table_list_t *node = flecs_query_cache_get_group(
        flecs_query_impl(query)->cache, group_id);
    if (!node) {
        return NULL;
    }
    
    return &node->info;
}

void* ecs_query_get_group_ctx(
    const ecs_query_t *query,
    uint64_t group_id)
{
    flecs_poly_assert(query, ecs_query_t);
    const ecs_query_group_info_t *info = ecs_query_get_group_info(
        query, group_id);
    if (!info) {
        return NULL;
    } else {
        return info->ctx;
    }
}
