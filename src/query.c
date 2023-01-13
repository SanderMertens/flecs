/**
 * @file query.c
 * @brief Cached query implementation.
 * 
 * Cached queries store a list of matched tables. The inputs for a cached query
 * are a filter and an observer. The filter is used to initially populate the
 * cache, and an observer is used to keep the cacne up to date.
 * 
 * Cached queries additionally support features like sorting and grouping. 
 * With sorting, an application can iterate over entities that can be sorted by
 * a component. Grouping allows an application to group matched tables, which is
 * used internally to implement the cascade feature, and can additionally be 
 * used to implement things like world cells.
 */

#include "private_api.h"

static
uint64_t flecs_query_get_group_id(
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (query->group_by) {
        return query->group_by(query->filter.world, table, 
            query->group_by_id, query->group_by_ctx);
    } else {
        return 0;
    }
}

static
void flecs_query_compute_group_id(
    ecs_query_t *query,
    ecs_query_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (query->group_by) {
        ecs_table_t *table = match->node.table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        match->node.group_id = flecs_query_get_group_id(query, table);
    } else {
        match->node.group_id = 0;
    }
}

static
ecs_query_table_list_t* flecs_query_get_group(
    ecs_query_t *query,
    uint64_t group_id)
{
    return ecs_map_get_deref(&query->groups, ecs_query_table_list_t, group_id);
}

static
ecs_query_table_list_t* flecs_query_ensure_group(
    ecs_query_t *query,
    uint64_t id)
{
    ecs_query_table_list_t *group = ecs_map_get_deref(&query->groups, 
        ecs_query_table_list_t, id);

    if (!group) {
        group = ecs_map_insert_alloc_t(&query->groups, 
            ecs_query_table_list_t, id);
        ecs_os_zeromem(group);
        if (query->on_group_create) {
            group->info.ctx = query->on_group_create(
                query->filter.world, id, query->group_by_ctx);
        }
    }

    return group;
}

static
void flecs_query_remove_group(
    ecs_query_t *query,
    uint64_t id)
{
    if (query->on_group_delete) {
        ecs_query_table_list_t *group = ecs_map_get_deref(&query->groups, 
            ecs_query_table_list_t, id);
        if (group) {
            query->on_group_delete(query->filter.world, id, 
                group->info.ctx, query->group_by_ctx);
        }
    }

    ecs_map_remove_free(&query->groups, id);
}

static
uint64_t flecs_query_default_group_by(
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
ecs_query_table_node_t* flecs_query_find_group_insertion_node(
    ecs_query_t *query,
    uint64_t group_id)
{
    /* Grouping must be enabled */
    ecs_assert(query->group_by != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_iter_t it = ecs_map_iter(&query->groups);
    ecs_query_table_list_t *list, *closest_list = NULL;
    uint64_t id, closest_id = 0;

    /* Find closest smaller group id */
    while (ecs_map_next(&it)) {
        id = ecs_map_key(&it);
        if (id >= group_id) {
            continue;
        }

        list = ecs_map_ptr(&it);
        if (!list->last) {
            ecs_assert(list->first == NULL, ECS_INTERNAL_ERROR, NULL);
            continue;
        }

        if (!closest_list || ((group_id - id) < (group_id - closest_id))) {
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
void flecs_query_create_group(
    ecs_query_t *query,
    ecs_query_table_node_t *node)
{
    ecs_query_table_match_t *match = node->match;
    uint64_t group_id = match->node.group_id;

    /* If query has grouping enabled & this is a new/empty group, find
     * the insertion point for the group */
    ecs_query_table_node_t *insert_after = flecs_query_find_group_insertion_node(
        query, group_id);

    if (!insert_after) {
        /* This group should appear first in the query list */
        ecs_query_table_node_t *query_first = query->list.first;
        if (query_first) {
            /* If this is not the first match for the query, insert before it */
            node->next = query_first;
            query_first->prev = node;
            query->list.first = node;
        } else {
            /* If this is the first match of the query, initialize its list */
            ecs_assert(query->list.last == NULL, ECS_INTERNAL_ERROR, NULL);
            query->list.first = node;
            query->list.last = node;
        }
    } else {
        ecs_assert(query->list.first != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(query->list.last != NULL, ECS_INTERNAL_ERROR, NULL);

        /* This group should appear after another group */
        ecs_query_table_node_t *insert_before = insert_after->next;
        node->prev = insert_after;
        insert_after->next = node;
        node->next = insert_before;
        if (insert_before) {
            insert_before->prev = node;
        } else {
            ecs_assert(query->list.last == insert_after, 
                ECS_INTERNAL_ERROR, NULL);
                
            /* This group should appear last in the query list */
            query->list.last = node;
        }
    }
}

/* Find the list the node should be part of */
static
ecs_query_table_list_t* flecs_query_get_node_list(
    ecs_query_t *query,
    ecs_query_table_node_t *node)
{
    ecs_query_table_match_t *match = node->match;
    if (query->group_by) {
        return flecs_query_get_group(query, match->node.group_id);
    } else {
        return &query->list;
    }
}

/* Find or create the list the node should be part of */
static
ecs_query_table_list_t* flecs_query_ensure_node_list(
    ecs_query_t *query,
    ecs_query_table_node_t *node)
{
    ecs_query_table_match_t *match = node->match;
    if (query->group_by) {
        return flecs_query_ensure_group(query, match->node.group_id);
    } else {
        return &query->list;
    }
}

/* Remove node from list */
static
void flecs_query_remove_table_node(
    ecs_query_t *query,
    ecs_query_table_node_t *node)
{
    ecs_query_table_node_t *prev = node->prev;
    ecs_query_table_node_t *next = node->next;

    ecs_assert(prev != node, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(next != node, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!prev || prev != next, ECS_INTERNAL_ERROR, NULL);

    ecs_query_table_list_t *list = flecs_query_get_node_list(query, node);

    if (!list || !list->first) {
        /* If list contains no nodes, the node must be empty */
        ecs_assert(!list || list->last == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(prev == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(next == NULL, ECS_INTERNAL_ERROR, NULL);
        return;
    }

    ecs_assert(prev != NULL || query->list.first == node, 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(next != NULL || query->list.last == node, 
        ECS_INTERNAL_ERROR, NULL);

    if (prev) {
        prev->next = next;
    }
    if (next) {
        next->prev = prev;
    }

    ecs_assert(list->info.table_count > 0, ECS_INTERNAL_ERROR, NULL);
    list->info.table_count --;

    if (query->group_by) {
        ecs_query_table_match_t *match = node->match;
        uint64_t group_id = match->node.group_id;

        /* Make sure query.list is updated if this is the first or last group */
        if (query->list.first == node) {
            ecs_assert(prev == NULL, ECS_INTERNAL_ERROR, NULL);
            query->list.first = next;
            prev = next;
        }
        if (query->list.last == node) {
            ecs_assert(next == NULL, ECS_INTERNAL_ERROR, NULL);
            query->list.last = prev;
            next = prev;
        }

        ecs_assert(query->list.info.table_count > 0, ECS_INTERNAL_ERROR, NULL);
        query->list.info.table_count --;
        list->info.match_count ++;

        /* Make sure group list only contains nodes that belong to the group */
        if (prev && prev->match->node.group_id != group_id) {
            /* The previous node belonged to another group */
            prev = next;
        }
        if (next && next->match->node.group_id != group_id) {
            /* The next node belonged to another group */
            next = prev;
        }

        /* Do check again, in case both prev & next belonged to another group */
        if ((!prev && !next) || (prev && prev->match->node.group_id != group_id)) {
            /* There are no more matches left in this group */
            flecs_query_remove_group(query, group_id);
            list = NULL;
        }
    }

    if (list) {
        if (list->first == node) {
            list->first = next;
        }
        if (list->last == node) {
            list->last = prev;
        }
    }

    node->prev = NULL;
    node->next = NULL;

    query->match_count ++;
}

/* Add node to list */
static
void flecs_query_insert_table_node(
    ecs_query_t *query,
    ecs_query_table_node_t *node)
{
    /* Node should not be part of an existing list */
    ecs_assert(node->prev == NULL && node->next == NULL, 
        ECS_INTERNAL_ERROR, NULL);

    /* If this is the first match, activate system */
    if (!query->list.first && query->filter.entity) {
        ecs_remove_id(query->filter.world, query->filter.entity, EcsEmpty);
    }

    flecs_query_compute_group_id(query, node->match);

    ecs_query_table_list_t *list = flecs_query_ensure_node_list(query, node);
    if (list->last) {
        ecs_assert(query->list.first != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(query->list.last != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(list->first != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_query_table_node_t *last = list->last;
        ecs_query_table_node_t *last_next = last->next;

        node->prev = last;
        node->next = last_next;
        last->next = node;

        if (last_next) {
            last_next->prev = node;
        }

        list->last = node;

        if (query->group_by) {
            /* Make sure to update query list if this is the last group */
            if (query->list.last == last) {
                query->list.last = node;
            }
        }
    } else {
        ecs_assert(list->first == NULL, ECS_INTERNAL_ERROR, NULL);

        list->first = node;
        list->last = node;

        if (query->group_by) {
            /* Initialize group with its first node */
            flecs_query_create_group(query, node);
        }
    }

    if (query->group_by) {
        list->info.table_count ++;
        list->info.match_count ++;
    }

    query->list.info.table_count ++;
    query->match_count ++;

    ecs_assert(node->prev != node, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(node->next != node, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(list->first != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(list->last != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(list->last == node, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query->list.first != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query->list.last != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query->list.first->prev == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query->list.last->next == NULL, ECS_INTERNAL_ERROR, NULL);
}

static
ecs_query_table_match_t* flecs_query_cache_add(
    ecs_world_t *world,
    ecs_query_table_t *elem)
{
    ecs_query_table_match_t *result = 
        flecs_bcalloc(&world->allocators.query_table_match);
    ecs_query_table_node_t *node = &result->node;

    node->match = result;
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

typedef struct {
    ecs_table_t *table;
    int32_t *dirty_state;
    int32_t column;
} table_dirty_state_t;

static
void flecs_query_get_dirty_state(
    ecs_query_t *query,
    ecs_query_table_match_t *match,
    int32_t term,
    table_dirty_state_t *out)
{
    ecs_world_t *world = query->filter.world;
    ecs_entity_t subject = match->sources[term];
    ecs_table_t *table;
    int32_t column = -1;

    if (!subject) {
        table = match->node.table;
        column = match->storage_columns[term];
    } else {
        table = ecs_get_table(world, subject);
        int32_t ref_index = -match->columns[term] - 1;
        ecs_ref_t *ref = ecs_vec_get_t(&match->refs, ecs_ref_t, ref_index);
        if (ref->id != 0) {
            ecs_ref_update(world, ref);
            column = ref->tr->column;
            column = ecs_table_type_to_storage_index(table, column);
        }
    }

    out->table = table;
    out->column = column;
    out->dirty_state = flecs_table_get_dirty_state(world, table);
}

/* Get match monitor. Monitors are used to keep track of whether components 
 * matched by the query in a table have changed. */
static
bool flecs_query_get_match_monitor(
    ecs_query_t *query,
    ecs_query_table_match_t *match)
{
    if (match->monitor) {
        return false;
    }

    int32_t *monitor = flecs_balloc(&query->allocators.monitors);
    monitor[0] = 0;

    /* Mark terms that don't need to be monitored. This saves time when reading
     * and/or updating the monitor. */
    const ecs_filter_t *f = &query->filter;
    int32_t i, t = -1, term_count = f->term_count;
    table_dirty_state_t cur_dirty_state;

    for (i = 0; i < term_count; i ++) {
        if (t == f->terms[i].field_index) {
            if (monitor[t + 1] != -1) {
                continue;
            }
        }

        t = f->terms[i].field_index;
        monitor[t + 1] = -1;

        if (f->terms[i].inout != EcsIn && 
            f->terms[i].inout != EcsInOut &&
            f->terms[i].inout != EcsInOutDefault) {
            continue; /* If term isn't read, don't monitor */
        }

        /* If term is not matched on this, don't track */
        if (!ecs_term_match_this(&f->terms[i])) {
            continue;
        }

        int32_t column = match->columns[t];
        if (column == 0) {
            continue; /* Don't track terms that aren't matched */
        }

        flecs_query_get_dirty_state(query, match, t, &cur_dirty_state);
        if (cur_dirty_state.column == -1) {
            continue; /* Don't track terms that aren't stored */
        }

        monitor[t + 1] = 0;
    }

    match->monitor = monitor;

    query->flags |= EcsQueryHasMonitor;

    return true;
}

/* Synchronize match monitor with table dirty state */
static
void flecs_query_sync_match_monitor(
    ecs_query_t *query,
    ecs_query_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!match->monitor) {
        if (query->flags & EcsQueryHasMonitor) {
            flecs_query_get_match_monitor(query, match);
        } else {
            return;
        }
    }

    int32_t *monitor = match->monitor;
    ecs_table_t *table = match->node.table;
    int32_t *dirty_state = flecs_table_get_dirty_state(query->filter.world, table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    table_dirty_state_t cur;

    monitor[0] = dirty_state[0]; /* Did table gain/lose entities */

    int32_t i, term_count = query->filter.term_count;
    for (i = 0; i < term_count; i ++) {
        int32_t t = query->filter.terms[i].field_index;
        if (monitor[t + 1] == -1) {
            continue;
        }
                
        flecs_query_get_dirty_state(query, match, t, &cur);
        ecs_assert(cur.column != -1, ECS_INTERNAL_ERROR, NULL);
        monitor[t + 1] = cur.dirty_state[cur.column + 1];
    }
}

/* Check if single match term has changed */
static
bool flecs_query_check_match_monitor_term(
    ecs_query_t *query,
    ecs_query_table_match_t *match,
    int32_t term)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_query_get_match_monitor(query, match)) {
        return true;
    }
    
    int32_t *monitor = match->monitor;
    ecs_table_t *table = match->node.table;
    int32_t *dirty_state = flecs_table_get_dirty_state(query->filter.world, table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    table_dirty_state_t cur;

    int32_t state = monitor[term];
    if (state == -1) {
        return false;
    }

    if (!term) {
        return monitor[0] != dirty_state[0];
    }

    flecs_query_get_dirty_state(query, match, term - 1, &cur);
    ecs_assert(cur.column != -1, ECS_INTERNAL_ERROR, NULL);

    return monitor[term] != cur.dirty_state[cur.column + 1];
}

/* Check if any term for match has changed */
static
bool flecs_query_check_match_monitor(
    ecs_query_t *query,
    ecs_query_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (flecs_query_get_match_monitor(query, match)) {
        return true;
    }

    int32_t *monitor = match->monitor;
    ecs_table_t *table = match->node.table;
    int32_t *dirty_state = flecs_table_get_dirty_state(query->filter.world, table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    if (monitor[0] != dirty_state[0]) {
        return true;
    }

    ecs_world_t *world = query->filter.world;
    int32_t i, field_count = query->filter.field_count;
    int32_t *storage_columns = match->storage_columns;
    int32_t *columns = match->columns;
    ecs_vec_t *refs = &match->refs;
    for (i = 0; i < field_count; i ++) {
        int32_t mon = monitor[i + 1];
        if (mon == -1) {
            continue;
        }

        int32_t column = storage_columns[i];
        if (column >= 0) {
            /* owned component */
            if (mon != dirty_state[column + 1]) {
                return true;
            }
            continue;
        } else if (column == -1) {
            continue; /* owned but not a component */
        }

        column = columns[i];
        if (!column) {
            /* Not matched */
            continue;
        }

        ecs_assert(column < 0, ECS_INTERNAL_ERROR, NULL);

        int32_t ref_index = -column - 1;
        ecs_ref_t *ref = ecs_vec_get_t(refs, ecs_ref_t, ref_index);
        if (ref->id != 0) {
            ecs_ref_update(world, ref);
            ecs_table_record_t *tr = ref->tr;
            ecs_table_t *src_table = tr->hdr.table;
            column = tr->column;
            column = ecs_table_type_to_storage_index(src_table, column);
            int32_t *src_dirty_state = flecs_table_get_dirty_state(
                world, src_table);
            if (mon != src_dirty_state[column + 1]) {
                return true;
            }
        }
    }

    return false;
}

/* Check if any term for matched table has changed */
static
bool flecs_query_check_table_monitor(
    ecs_query_t *query,
    ecs_query_table_t *table,
    int32_t term)
{
    ecs_query_table_node_t *cur, *end = table->last->node.next;

    for (cur = &table->first->node; cur != end; cur = cur->next) {
        ecs_query_table_match_t *match = (ecs_query_table_match_t*)cur;
        if (term == -1) {
            if (flecs_query_check_match_monitor(query, match)) {
                return true;
            }
        } else {
            if (flecs_query_check_match_monitor_term(query, match, term)) {
                return true;
            } 
        }
    }

    return false;
}

static
bool flecs_query_check_query_monitor(
    ecs_query_t *query)
{
    ecs_table_cache_iter_t it;
    if (flecs_table_cache_iter(&query->cache, &it)) {
        ecs_query_table_t *qt;
        while ((qt = flecs_table_cache_next(&it, ecs_query_table_t))) {
            if (flecs_query_check_table_monitor(query, qt, -1)) {
                return true;
            }
        }
    }

    return false;
}

static
void flecs_query_init_query_monitors(
    ecs_query_t *query)
{
    ecs_query_table_node_t *cur = query->list.first;

    /* Ensure each match has a monitor */
    for (; cur != NULL; cur = cur->next) {
        ecs_query_table_match_t *match = (ecs_query_table_match_t*)cur;
        flecs_query_get_match_monitor(query, match);
    }
}

/* The group by function for cascade computes the tree depth for the table type.
 * This causes tables in the query cache to be ordered by depth, which ensures
 * breadth-first iteration order. */
static
uint64_t flecs_query_group_by_cascade(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    void *ctx)
{
    (void)id;
    ecs_term_t *term = ctx;
    ecs_entity_t rel = term->src.trav;
    int32_t depth = flecs_relation_depth(world, rel, table);
    return flecs_ito(uint64_t, depth);
}

static
void flecs_query_add_ref(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_table_match_t *qm,
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

    query->flags |= EcsQueryHasRefs;
}

static
ecs_query_table_match_t* flecs_query_add_table_match(
    ecs_query_t *query,
    ecs_query_table_t *qt,
    ecs_table_t *table)
{
    /* Add match for table. One table can have more than one match, if
     * the query contains wildcards. */
    ecs_query_table_match_t *qm = flecs_query_cache_add(query->filter.world, qt);
    qm->node.table = table;

    qm->columns = flecs_balloc(&query->allocators.columns);
    qm->storage_columns = flecs_balloc(&query->allocators.columns);
    qm->ids = flecs_balloc(&query->allocators.ids);
    qm->sources = flecs_balloc(&query->allocators.sources);
    qm->sizes = flecs_balloc(&query->allocators.sizes);

    /* Insert match to iteration list if table is not empty */
    if (!table || ecs_table_count(table) != 0) {
        flecs_query_insert_table_node(query, &qm->node);
    }

    return qm;
}

static
void flecs_query_set_table_match(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_table_match_t *qm,
    ecs_table_t *table,
    ecs_iter_t *it)
{
    ecs_allocator_t *a = &world->allocator;
    ecs_filter_t *filter = &query->filter;
    int32_t i, term_count = filter->term_count;
    int32_t field_count = filter->field_count;
    ecs_term_t *terms = filter->terms;

    /* Reset resources in case this is an existing record */
    ecs_vec_reset_t(a, &qm->refs, ecs_ref_t);
    ecs_os_memcpy_n(qm->columns, it->columns, int32_t, field_count);
    ecs_os_memcpy_n(qm->ids, it->ids, ecs_id_t, field_count);
    ecs_os_memcpy_n(qm->sources, it->sources, ecs_entity_t, field_count);
    ecs_os_memcpy_n(qm->sizes, it->sizes, ecs_size_t, field_count);

    if (table) {
        /* Initialize storage columns for faster access to component storage */
        for (i = 0; i < field_count; i ++) {
            int32_t column = qm->columns[i];
            if (column > 0) {
                qm->storage_columns[i] = ecs_table_type_to_storage_index(table,
                    qm->columns[i] - 1);
            } else {
                /* Shared field (not from table) */
                qm->storage_columns[i] = -2;
            }
        }

        flecs_entity_filter_init(world, &qm->entity_filter, filter, 
            table, qm->ids, qm->columns);
    }

    /* Add references for substituted terms */
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        if (!ecs_term_match_this(term)) {
            /* non-This terms are set during iteration */
            continue;
        }

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
                flecs_query_add_ref(world, query, qm, id, src, size);

                /* Use column index to bind term and ref */
                if (qm->columns[field] != 0) {
                    qm->columns[field] = -ecs_vec_count(&qm->refs);
                }
            }
        }
    }
}

/** Populate query cache with tables */
static
void flecs_query_match_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_table_t *table = NULL;
    ecs_query_table_t *qt = NULL;

    ecs_iter_t it = ecs_filter_iter(world, &query->filter);
    ECS_BIT_SET(it.flags, EcsIterIsInstanced);
    ECS_BIT_SET(it.flags, EcsIterIsFilter);
    ECS_BIT_SET(it.flags, EcsIterEntityOptional);

    while (ecs_filter_next(&it)) {
        if ((table != it.table) || (!it.table && !qt)) {
            /* New table matched, add record to cache */
            qt = flecs_bcalloc(&world->allocators.query_table);
            ecs_table_cache_insert(&query->cache, it.table, &qt->hdr);
            table = it.table;
        }

        ecs_query_table_match_t *qm = flecs_query_add_table_match(query, qt, table);
        flecs_query_set_table_match(world, query, qm, table, &it);
    }
}

static
bool flecs_query_match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (!ecs_map_is_init(&query->cache.index)) {
        return false;
    }

    ecs_query_table_t *qt = NULL;
    ecs_filter_t *filter = &query->filter;
    int var_id = ecs_filter_find_this_var(filter);
    if (var_id == -1) {
        /* If query doesn't match with This term, it can't match with tables */
        return false;
    }

    ecs_iter_t it = flecs_filter_iter_w_flags(world, filter, EcsIterMatchVar|
        EcsIterIsInstanced|EcsIterIsFilter|EcsIterEntityOptional);
    ecs_iter_set_var_as_table(&it, var_id, table);

    while (ecs_filter_next(&it)) {
        ecs_assert(it.table == table, ECS_INTERNAL_ERROR, NULL);
        if (qt == NULL) {
            qt = flecs_bcalloc(&world->allocators.query_table);
            ecs_table_cache_insert(&query->cache, it.table, &qt->hdr);
            table = it.table;
        }

        ecs_query_table_match_t *qm = flecs_query_add_table_match(query, qt, table);
        flecs_query_set_table_match(world, query, qm, table, &it);
    }

    return qt != NULL;
}

ECS_SORT_TABLE_WITH_COMPARE(_, flecs_query_sort_table_generic, order_by, static)

static
void flecs_query_sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_index,
    ecs_order_by_action_t compare,
    ecs_sort_table_action_t sort)
{
    ecs_data_t *data = &table->data;
    if (!ecs_vec_count(&data->entities)) {
        /* Nothing to sort */
        return;
    }

    int32_t count = flecs_table_data_count(data);
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = ecs_vec_first(&data->entities);

    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_type_info_t *ti = table->type_info[column_index];
        ecs_vec_t *column = &data->columns[column_index];
        size = ti->size;
        ptr = ecs_vec_first(column);
    }

    if (sort) {
        sort(world, table, entities, ptr, size, 0, count - 1, compare);
    } else {
        flecs_query_sort_table_generic(world, table, entities, ptr, size, 0, count - 1, compare);
    }
}

/* Helper struct for building sorted table ranges */
typedef struct sort_helper_t {
    ecs_query_table_match_t *match;
    ecs_entity_t *entities;
    const void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
    bool shared;
} sort_helper_t;

static
const void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->elem_size >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->row >= 0, ECS_INTERNAL_ERROR, NULL);
    if (helper->shared) {
        return helper->ptr;
    } else {
        return ECS_ELEM(helper->ptr, helper->elem_size, helper->row);
    }
}

static
ecs_entity_t e_from_helper(
    sort_helper_t *helper)
{
    if (helper->row < helper->count) {
        return helper->entities[helper->row];
    } else {
        return 0;
    }
}

static
void flecs_query_build_sorted_table_range(
    ecs_query_t *query,
    ecs_query_table_list_t *list)
{
    ecs_world_t *world = query->filter.world;
    ecs_entity_t id = query->order_by_component;
    ecs_order_by_action_t compare = query->order_by;
    int32_t table_count = list->info.table_count;
    if (!table_count) {
        return;
    }

    int to_sort = 0;

    sort_helper_t *helper = ecs_os_malloc_n(sort_helper_t, table_count);
    ecs_query_table_node_t *cur, *end = list->last->next;
    for (cur = list->first; cur != end; cur = cur->next) {
        ecs_query_table_match_t *match = cur->match;
        ecs_table_t *table = match->node.table;
        ecs_data_t *data = &table->data;

        ecs_assert(ecs_table_count(table) != 0, ECS_INTERNAL_ERROR, NULL);

        int32_t index = -1;
        if (id) {
            index = ecs_search(world, table->storage_table, id, 0);
        }

        if (index != -1) {
            ecs_type_info_t *ti = table->type_info[index];
            ecs_vec_t *column = &data->columns[index];
            int32_t size = ti->size;
            helper[to_sort].ptr = ecs_vec_first(column);
            helper[to_sort].elem_size = size;
            helper[to_sort].shared = false;
        } else if (id) {
            /* Find component in prefab */
            ecs_entity_t base = 0;
            ecs_search_relation(world, table, 0, id, 
                EcsIsA, EcsUp, &base, 0, 0);

            /* If a base was not found, the query should not have allowed using
             * the component for sorting */
            ecs_assert(base != 0, ECS_INTERNAL_ERROR, NULL);

            const EcsComponent *cptr = ecs_get(world, id, EcsComponent);
            ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);

            helper[to_sort].ptr = ecs_get_id(world, base, id);
            helper[to_sort].elem_size = cptr->size;
            helper[to_sort].shared = true;
        } else {
            helper[to_sort].ptr = NULL;
            helper[to_sort].elem_size = 0;
            helper[to_sort].shared = false;
        }

        helper[to_sort].match = match;
        helper[to_sort].entities = ecs_vec_first(&data->entities);
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table);
        to_sort ++;      
    }

    ecs_assert(to_sort != 0, ECS_INTERNAL_ERROR, NULL);

    bool proceed;
    do {
        int32_t j, min = 0;
        proceed = true;

        ecs_entity_t e1;
        while (!(e1 = e_from_helper(&helper[min]))) {
            min ++;
            if (min == to_sort) {
                proceed = false;
                break;
            }
        }

        if (!proceed) {
            break;
        }

        for (j = min + 1; j < to_sort; j++) {
            ecs_entity_t e2 = e_from_helper(&helper[j]);
            if (!e2) {
                continue;
            }

            const void *ptr1 = ptr_from_helper(&helper[min]);
            const void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
                e1 = e_from_helper(&helper[min]);
            }
        }

        sort_helper_t *cur_helper = &helper[min];
        if (!cur || cur->match != cur_helper->match) {
            cur = ecs_vector_add(&query->table_slices, ecs_query_table_node_t);
            ecs_assert(cur != NULL, ECS_INTERNAL_ERROR, NULL);
            cur->match = cur_helper->match;
            cur->offset = cur_helper->row;
            cur->count = 1;
        } else {
            cur->count ++;
        }

        cur_helper->row ++;
    } while (proceed);

    /* Iterate through the vector of slices to set the prev/next ptrs. This
     * can't be done while building the vector, as reallocs may occur */
    int32_t i, count = ecs_vector_count(query->table_slices);    
    ecs_query_table_node_t *nodes = ecs_vector_first(
        query->table_slices, ecs_query_table_node_t);
    for (i = 0; i < count; i ++) {
        nodes[i].prev = &nodes[i - 1];
        nodes[i].next = &nodes[i + 1];
    }

    nodes[0].prev = NULL;
    nodes[i - 1].next = NULL;

    ecs_os_free(helper);
}

static
void flecs_query_build_sorted_tables(
    ecs_query_t *query)
{
    ecs_vector_clear(query->table_slices);

    if (query->group_by) {
        /* Populate sorted node list in grouping order */
        ecs_query_table_node_t *cur = query->list.first;
        if (cur) {
            do {
                /* Find list for current group */
                ecs_query_table_match_t *match = cur->match;
                ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);
                uint64_t group_id = match->node.group_id;
                ecs_query_table_list_t *list = ecs_map_get_deref(
                    &query->groups, ecs_query_table_list_t, group_id);
                ecs_assert(list != NULL, ECS_INTERNAL_ERROR, NULL);

                /* Sort tables in current group */
                flecs_query_build_sorted_table_range(query, list);
                
                /* Find next group to sort */
                cur = list->last->next;
            } while (cur);
        }
    } else {
        flecs_query_build_sorted_table_range(query, &query->list);
    }
}

static
void flecs_query_sort_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_order_by_action_t compare = query->order_by;
    if (!compare) {
        return;
    }

    ecs_sort_table_action_t sort = query->sort_table;
    
    ecs_entity_t order_by_component = query->order_by_component;
    int32_t i, order_by_term = -1;

    /* Find term that iterates over component (must be at least one) */
    if (order_by_component) {
        const ecs_filter_t *f = &query->filter;
        int32_t term_count = f->term_count;
        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &f->terms[i];
            if (!ecs_term_match_this(term)) {
                continue;
            }

            if (term->id == order_by_component) {
                order_by_term = i;
                break;
            }
        }

        ecs_assert(order_by_term != -1, ECS_INTERNAL_ERROR, NULL);
    }

    /* Iterate over non-empty tables. Don't bother with empty tables as they
     * have nothing to sort */

    bool tables_sorted = false;

    ecs_table_cache_iter_t it;
    ecs_query_table_t *qt;
    flecs_table_cache_iter(&query->cache, &it);

    while ((qt = flecs_table_cache_next(&it, ecs_query_table_t))) {
        ecs_table_t *table = qt->hdr.table;
        bool dirty = false;

        if (flecs_query_check_table_monitor(query, qt, 0)) {
            dirty = true;
        }

        int32_t column = -1;
        if (order_by_component) {
            if (flecs_query_check_table_monitor(query, qt, order_by_term + 1)) {
                dirty = true;
            }

            if (dirty) {
                column = -1;

                ecs_table_t *storage_table = table->storage_table;
                if (storage_table) {
                    column = ecs_search(world, storage_table, 
                        order_by_component, 0);
                }

                if (column == -1) {
                    /* Component is shared, no sorting is needed */
                    dirty = false;
                }
            }
        }

        if (!dirty) {
            continue;
        }

        /* Something has changed, sort the table. Prefers using flecs_query_sort_table when available */
        flecs_query_sort_table(world, table, column, compare, sort);
        tables_sorted = true;
    }

    if (tables_sorted || query->match_count != query->prev_match_count) {
        flecs_query_build_sorted_tables(query);
        query->match_count ++; /* Increase version if tables changed */
    }
}

static
bool flecs_query_has_refs(
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;
    for (i = 0; i < count; i ++) {
        if (terms[i].src.flags & (EcsUp | EcsIsEntity)) {
            return true;
        }
    }

    return false;
}

static
void flecs_query_for_each_component_monitor(
    ecs_world_t *world,
    ecs_query_t *query,
    void(*callback)(
        ecs_world_t* world,
        ecs_id_t id,
        ecs_query_t *query))
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *src = &term->src;

        if (src->flags & EcsUp) {
            callback(world, ecs_pair(src->trav, EcsWildcard), query);
            if (src->trav != EcsIsA) {
                callback(world, ecs_pair(EcsIsA, EcsWildcard), query);
            }
            callback(world, term->id, query);

        } else if (src->flags & EcsSelf && !ecs_term_match_this(term)) {
            callback(world, term->id, query);
        }
    }
}

static
bool flecs_query_is_term_id_supported(
    ecs_term_id_t *term_id)
{
    if (!(term_id->flags & EcsIsVariable)) {
        return true;
    }
    if (ecs_id_is_wildcard(term_id->id)) {
        return true;
    }
    return false;
}

static
int flecs_query_process_signature(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *first = &term->first;
        ecs_term_id_t *src = &term->src;
        ecs_term_id_t *second = &term->second;
        ecs_inout_kind_t inout = term->inout;

        bool is_src_ok = flecs_query_is_term_id_supported(src);
        bool is_first_ok = flecs_query_is_term_id_supported(first);
        bool is_second_ok = flecs_query_is_term_id_supported(second);

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
        ecs_check(!(src->flags & EcsFilter), ECS_INVALID_PARAMETER,
            "invalid usage of Filter for query");

        if (inout != EcsIn && inout != EcsInOutNone) {
            query->flags |= EcsQueryHasOutColumns;
        }

        if (src->flags & EcsCascade) {
            /* Query can only have one cascade column */
            ecs_assert(query->cascade_by == 0, ECS_INVALID_PARAMETER, NULL);
            query->cascade_by = i + 1;
        }

        if ((src->flags & EcsTraverseFlags) == EcsSelf) {
            if (src->flags & EcsIsEntity) {
                flecs_add_flag(world, term->src.id, EcsEntityObserved);
            }
        }
    }

    query->flags |= (ecs_flags32_t)(flecs_query_has_refs(query) * EcsQueryHasRefs);

    if (!(query->flags & EcsQueryIsSubquery)) {
        flecs_query_for_each_component_monitor(world, query, flecs_monitor_register);
    }

    return 0;
error:
    return -1;
}

/** When a table becomes empty remove it from the query list, or vice versa. */
static
void flecs_query_update_table(
    ecs_query_t *query,
    ecs_table_t *table,
    bool empty)
{
    int32_t prev_count = ecs_query_table_count(query);
    ecs_table_cache_set_empty(&query->cache, table, empty);
    int32_t cur_count = ecs_query_table_count(query);

    if (prev_count != cur_count) {
        ecs_query_table_t *qt = ecs_table_cache_get(&query->cache, table);
        ecs_assert(qt != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_query_table_match_t *cur, *next;

        for (cur = qt->first; cur != NULL; cur = next) {
            next = cur->next_match;

            if (empty) {
                ecs_assert(ecs_table_count(table) == 0, 
                    ECS_INTERNAL_ERROR, NULL);

                flecs_query_remove_table_node(query, &cur->node);
            } else {
                ecs_assert(ecs_table_count(table) != 0, 
                    ECS_INTERNAL_ERROR, NULL);
                flecs_query_insert_table_node(query, &cur->node);
            }
        }
    }

    ecs_assert(cur_count || query->list.first == NULL, 
        ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_query_add_subquery(
    ecs_world_t *world, 
    ecs_query_t *parent, 
    ecs_query_t *subquery) 
{
    ecs_query_t **elem = ecs_vector_add(&parent->subqueries, ecs_query_t*);
    *elem = subquery;

    ecs_table_cache_t *cache = &parent->cache;
    ecs_table_cache_iter_t it;
    ecs_query_table_t *qt;
    flecs_table_cache_all_iter(cache, &it);
    while ((qt = flecs_table_cache_next(&it, ecs_query_table_t))) {
        flecs_query_match_table(world, subquery, qt->hdr.table);
    }
}

static
void flecs_query_notify_subqueries(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event)
{
    if (query->subqueries) {
        ecs_query_t **queries = ecs_vector_first(query->subqueries, ecs_query_t*);
        int32_t i, count = ecs_vector_count(query->subqueries);

        ecs_query_event_t sub_event = *event;
        sub_event.parent_query = query;

        for (i = 0; i < count; i ++) {
            ecs_query_t *sub = queries[i];
            flecs_query_notify(world, sub, &sub_event);
        }
    }
}

/* Remove table */
static
void flecs_query_table_match_free(
    ecs_query_t *query,
    ecs_query_table_t *elem,
    ecs_query_table_match_t *first)
{
    ecs_query_table_match_t *cur, *next;
    ecs_world_t *world = query->filter.world;

    for (cur = first; cur != NULL; cur = next) {
        flecs_bfree(&query->allocators.columns, cur->columns);
        flecs_bfree(&query->allocators.columns, cur->storage_columns);
        flecs_bfree(&query->allocators.ids, cur->ids);
        flecs_bfree(&query->allocators.sources, cur->sources);
        flecs_bfree(&query->allocators.sizes, cur->sizes);

        if (cur->monitor) {
            flecs_bfree(&query->allocators.monitors, cur->monitor);
        }
        if (!elem->hdr.empty) {
            flecs_query_remove_table_node(query, &cur->node);
        }
        
        ecs_vec_fini_t(&world->allocator, &cur->refs, ecs_ref_t);
        flecs_entity_filter_fini(world, &cur->entity_filter);

        next = cur->next_match;

        flecs_bfree(&world->allocators.query_table_match, cur);
    }
}

static
void flecs_query_table_free(
    ecs_query_t *query,
    ecs_query_table_t *elem)
{
    flecs_query_table_match_free(query, elem, elem->first);
    flecs_bfree(&query->filter.world->allocators.query_table, elem);
}

static
void flecs_query_unmatch_table(
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_query_table_t *qt = ecs_table_cache_remove(
        &query->cache, table, NULL);
    if (qt) {
        flecs_query_table_free(query, qt);
    }
}

/* Rematch system with tables after a change happened to a watched entity */
static
void flecs_query_rematch_tables(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_t *parent_query)
{
    ecs_iter_t it, parent_it;
    ecs_table_t *table = NULL;
    ecs_query_table_t *qt = NULL;
    ecs_query_table_match_t *qm = NULL;

    if (query->monitor_generation == world->monitor_generation) {
        return;
    }

    query->monitor_generation = world->monitor_generation;

    if (parent_query) {
        parent_it = ecs_query_iter(world, parent_query);
        it = ecs_filter_chain_iter(&parent_it, &query->filter);
    } else {
        it = ecs_filter_iter(world, &query->filter);
    }

    ECS_BIT_SET(it.flags, EcsIterIsInstanced);
    ECS_BIT_SET(it.flags, EcsIterIsFilter);
    ECS_BIT_SET(it.flags, EcsIterEntityOptional);

    world->info.rematch_count_total ++;
    int32_t rematch_count = ++ query->rematch_count;

    ecs_time_t t = {0};
    if (world->flags & EcsWorldMeasureFrameTime) {
        ecs_time_measure(&t);
    }

    while (ecs_filter_next(&it)) {
        if ((table != it.table) || (!it.table && !qt)) {
            if (qm && qm->next_match) {
                flecs_query_table_match_free(query, qt, qm->next_match);
                qm->next_match = NULL;
            }

            table = it.table;

            qt = ecs_table_cache_get(&query->cache, table);
            if (!qt) {
                qt = flecs_bcalloc(&world->allocators.query_table);
                ecs_table_cache_insert(&query->cache, table, &qt->hdr);
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
            qm = flecs_query_add_table_match(query, qt, table);
        }

        flecs_query_set_table_match(world, query, qm, table, &it);

        if (table && ecs_table_count(table) && query->group_by) {
            if (flecs_query_get_group_id(query, table) != qm->node.group_id) {
                /* Update table group */
                flecs_query_remove_table_node(query, &qm->node);
                flecs_query_insert_table_node(query, &qm->node);
            }
        }
    }

    if (qm && qm->next_match) {
        flecs_query_table_match_free(query, qt, qm->next_match);
        qm->next_match = NULL;
    }

    /* Iterate all tables in cache, remove ones that weren't just matched */
    ecs_table_cache_iter_t cache_it;
    if (flecs_table_cache_all_iter(&query->cache, &cache_it)) {
        while ((qt = flecs_table_cache_next(&cache_it, ecs_query_table_t))) {
            if (qt->rematch_count != rematch_count) {
                flecs_query_unmatch_table(query, qt->hdr.table);
            }
        }
    }

    if (world->flags & EcsWorldMeasureFrameTime) {
        world->info.rematch_time_total += (ecs_ftime_t)ecs_time_measure(&t);
    }
}

static
void flecs_query_remove_subquery(
    ecs_query_t *parent, 
    ecs_query_t *sub)
{
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(sub != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->subqueries != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(parent->subqueries);
    ecs_query_t **sq = ecs_vector_first(parent->subqueries, ecs_query_t*);

    for (i = 0; i < count; i ++) {
        if (sq[i] == sub) {
            break;
        }
    }

    ecs_vector_remove(parent->subqueries, ecs_query_t*, i);
}

/* -- Private API -- */

void flecs_query_notify(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event)
{
    bool notify = true;

    switch(event->kind) {
    case EcsQueryTableMatch:
        /* Creation of new table */
        if (flecs_query_match_table(world, query, event->table)) {
            if (query->subqueries) {
                flecs_query_notify_subqueries(world, query, event);
            }
        }
        notify = false;
        break;
    case EcsQueryTableUnmatch:
        /* Deletion of table */
        flecs_query_unmatch_table(query, event->table);
        break;
    case EcsQueryTableRematch:
        /* Rematch tables of query */
        flecs_query_rematch_tables(world, query, event->parent_query);
        break;        
    case EcsQueryOrphan:
        ecs_assert(query->flags & EcsQueryIsSubquery, ECS_INTERNAL_ERROR, NULL);
        query->flags |= EcsQueryIsOrphaned;
        query->parent = NULL;
        break;
    }

    if (notify) {
        flecs_query_notify_subqueries(world, query, event);
    }
}

static
void flecs_query_order_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t order_by_component,
    ecs_order_by_action_t order_by,
    ecs_sort_table_action_t action)
{
    ecs_check(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);    

    query->order_by_component = order_by_component;
    query->order_by = order_by;
    query->sort_table = action;

    ecs_vector_free(query->table_slices);
    query->table_slices = NULL;

    flecs_query_sort_tables(world, query);  

    if (!query->table_slices) {
        flecs_query_build_sorted_tables(query);
    }
error:
    return;
}

static
void flecs_query_group_by(
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_group_by_action_t group_by)
{   
    /* Cannot change grouping once a query has been created */
    ecs_check(query->group_by_id == 0, ECS_INVALID_OPERATION, NULL);
    ecs_check(query->group_by == 0, ECS_INVALID_OPERATION, NULL);

    if (!group_by) {
        /* Builtin function that groups by relationship */
        group_by = flecs_query_default_group_by;   
    }

    query->group_by_id = sort_component;
    query->group_by = group_by;

    ecs_map_init_w_params(&query->groups, 
        &query->filter.world->allocators.query_table_list);
error:
    return;
}

/* Implementation for iterable mixin */
static
void flecs_query_iter_init(
    const ecs_world_t *world,
    const ecs_poly_t *poly,
    ecs_iter_t *iter,
    ecs_term_t *filter)
{
    ecs_poly_assert(poly, ecs_query_t);

    if (filter) {
        iter[1] = ecs_query_iter(world, (ecs_query_t*)poly);
        iter[0] = ecs_term_chain_iter(&iter[1], filter);
    } else {
        iter[0] = ecs_query_iter(world, (ecs_query_t*)poly);
    }
}

static
void flecs_query_on_event(
    ecs_iter_t *it)
{
    /* Because this is the observer::run callback, checking if this is event is
     * already handled is not done for us. */
    ecs_world_t *world = it->world;
    ecs_observer_t *o = it->ctx;
    if (o->last_event_id) {
        if (o->last_event_id[0] == world->event_id) {
            return;
        }
        o->last_event_id[0] = world->event_id;
    }

    ecs_query_t *query = o->ctx;
    ecs_table_t *table = it->table;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* The observer isn't doing the matching because the query can do it more
     * efficiently by checking the table with the query cache. */
    if (ecs_table_cache_get(&query->cache, table) == NULL) {
        return;
    }

    ecs_entity_t event = it->event;
    if (event == EcsOnTableEmpty) {
        flecs_query_update_table(query, table, true);
    } else
    if (event == EcsOnTableFill) {
        flecs_query_update_table(query, table, false);
    }
}

static
void flecs_query_table_cache_free(
    ecs_query_t *query)
{
    ecs_table_cache_iter_t it;
    ecs_query_table_t *qt;

    if (flecs_table_cache_all_iter(&query->cache, &it)) {
        while ((qt = flecs_table_cache_next(&it, ecs_query_table_t))) {
            flecs_query_table_free(query, qt);
        }
    }

    ecs_table_cache_fini(&query->cache);
}

static
void flecs_query_allocators_init(
    ecs_query_t *query)
{
    int32_t field_count = query->filter.field_count;
    if (field_count) {
        flecs_ballocator_init(&query->allocators.columns, 
            field_count * ECS_SIZEOF(int32_t));
        flecs_ballocator_init(&query->allocators.ids, 
            field_count * ECS_SIZEOF(ecs_id_t));
        flecs_ballocator_init(&query->allocators.sources,
            field_count * ECS_SIZEOF(ecs_entity_t));
        flecs_ballocator_init(&query->allocators.sizes,
            field_count * ECS_SIZEOF(ecs_size_t));
        flecs_ballocator_init(&query->allocators.monitors,
            (1 + field_count) * ECS_SIZEOF(int32_t));
    }
}

static
void flecs_query_allocators_fini(
    ecs_query_t *query)
{
    int32_t field_count = query->filter.field_count;
    if (field_count) {
        flecs_ballocator_fini(&query->allocators.columns);
        flecs_ballocator_fini(&query->allocators.ids);
        flecs_ballocator_fini(&query->allocators.sources);
        flecs_ballocator_fini(&query->allocators.sizes);
        flecs_ballocator_fini(&query->allocators.monitors);
    }
}

static
void flecs_query_fini(
    ecs_query_t *query)
{
    ecs_world_t *world = query->filter.world;

    ecs_group_delete_action_t on_delete = query->on_group_delete;
    if (on_delete) {
        ecs_map_iter_t it = ecs_map_iter(&query->groups);
        while (ecs_map_next(&it)) {
            ecs_query_table_list_t *group = ecs_map_ptr(&it);
            uint64_t group_id = ecs_map_key(&it);
            on_delete(world, group_id, group->info.ctx, query->group_by_ctx);
        }
        query->on_group_delete = NULL;
    }

    if (query->group_by_ctx_free) {
        if (query->group_by_ctx) {
            query->group_by_ctx_free(query->group_by_ctx);
        }
    }

    if ((query->flags & EcsQueryIsSubquery) &&
        !(query->flags & EcsQueryIsOrphaned))
    {
        flecs_query_remove_subquery(query->parent, query);
    }

    flecs_query_notify_subqueries(world, query, &(ecs_query_event_t){
        .kind = EcsQueryOrphan
    });

    flecs_query_for_each_component_monitor(world, query, 
        flecs_monitor_unregister);
    flecs_query_table_cache_free(query);

    ecs_map_fini(&query->groups);

    ecs_vector_free(query->subqueries);
    ecs_vector_free(query->table_slices);
    ecs_filter_fini(&query->filter);

    flecs_query_allocators_fini(query);

    ecs_poly_free(query, ecs_query_t);
}

/* -- Public API -- */

ecs_query_t* ecs_query_init(
    ecs_world_t *world,
    const ecs_query_desc_t *desc)
{
    ecs_check(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(world->flags & EcsWorldFini), ECS_INVALID_OPERATION, NULL);

    ecs_query_t *result = ecs_poly_new(ecs_query_t);
    ecs_observer_desc_t observer_desc = { .filter = desc->filter };
    ecs_entity_t entity = desc->filter.entity;

    observer_desc.filter.flags = EcsFilterMatchEmptyTables;
    observer_desc.filter.storage = &result->filter;
    result->filter = ECS_FILTER_INIT;

    if (ecs_filter_init(world, &observer_desc.filter) == NULL) {
        goto error;
    }

    flecs_query_allocators_init(result);

    if (result->filter.term_count) {
        observer_desc.entity = entity;
        observer_desc.run = flecs_query_on_event;
        observer_desc.ctx = result;
        observer_desc.events[0] = EcsOnTableEmpty;
        observer_desc.events[1] = EcsOnTableFill;
        observer_desc.filter.flags |= EcsFilterNoData;
        observer_desc.filter.instanced = true;

        /* ecs_filter_init could have moved away resources from the terms array
         * in the descriptor, so use the terms array from the filter. */
        observer_desc.filter.terms_buffer = result->filter.terms;
        observer_desc.filter.terms_buffer_count = result->filter.term_count;
        observer_desc.filter.expr = NULL; /* Already parsed */

        entity = ecs_observer_init(world, &observer_desc);
        if (!entity) {
            goto error;
        }
    }

    result->iterable.init = flecs_query_iter_init;
    result->dtor = (ecs_poly_dtor_t)flecs_query_fini;
    result->prev_match_count = -1;

    if (ecs_should_log_1()) {
        char *filter_expr = ecs_filter_str(world, &result->filter);
        ecs_dbg_1("#[green]query#[normal] [%s] created", 
            filter_expr ? filter_expr : "");
        ecs_os_free(filter_expr);
    }

    ecs_log_push_1();

    if (flecs_query_process_signature(world, result)) {
        goto error;
    }

    /* Group before matching so we won't have to move tables around later */
    int32_t cascade_by = result->cascade_by;
    if (cascade_by) {
        flecs_query_group_by(result, result->filter.terms[cascade_by - 1].id,
            flecs_query_group_by_cascade);
        result->group_by_ctx = &result->filter.terms[cascade_by - 1];
    }

    if (desc->group_by || desc->group_by_id) {
        /* Can't have a cascade term and group by at the same time, as cascade
         * uses the group_by mechanism */
        ecs_check(!result->cascade_by, ECS_INVALID_PARAMETER, NULL);
        flecs_query_group_by(result, desc->group_by_id, desc->group_by);
        result->group_by_ctx = desc->group_by_ctx;
        result->on_group_create = desc->on_group_create;
        result->on_group_delete = desc->on_group_delete;
        result->group_by_ctx_free = desc->group_by_ctx_free;
    }

    if (desc->parent != NULL) {
        result->flags |= EcsQueryIsSubquery;
    }

    /* If the query refers to itself, add the components that were queried for
     * to the query itself. */
    if (entity)  {
        int32_t t, term_count = result->filter.term_count;
        ecs_term_t *terms = result->filter.terms;

        for (t = 0; t < term_count; t ++) {
            ecs_term_t *term = &terms[t];
            if (term->src.id == entity) {
                ecs_add_id(world, entity, term->id);
            }
        }        
    }

    if (!entity) {
        entity = ecs_new_id(world);
    }

    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_query_t);
    poly->poly = result;
    result->filter.entity = entity;

    /* Ensure that while initially populating the query with tables, they are
     * in the right empty/non-empty list. This ensures the query won't miss
     * empty/non-empty events for tables that are currently out of sync, but
     * change back to being in sync before processing pending events. */
    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    ecs_table_cache_init(world, &result->cache);

    if (!desc->parent) {
        flecs_query_match_tables(world, result);
    } else {
        flecs_query_add_subquery(world, desc->parent, result);
        result->parent = desc->parent;
    }

    if (desc->order_by) {
        flecs_query_order_by(
            world, result, desc->order_by_component, desc->order_by,
            desc->sort_table);
    }

    if (!ecs_query_table_count(result) && result->filter.term_count) {
        ecs_add_id(world, entity, EcsEmpty);
    }

    ecs_poly_modified(world, entity, ecs_query_t);

    ecs_log_pop_1();

    return result;
error:
    if (result) {
        ecs_filter_fini(&result->filter);
        ecs_os_free(result);
    }
    return NULL;
}

void ecs_query_fini(
    ecs_query_t *query)
{
    ecs_poly_assert(query, ecs_query_t);
    ecs_delete(query->filter.world, query->filter.entity);
}

const ecs_filter_t* ecs_query_get_filter(
    ecs_query_t *query)
{
    ecs_poly_assert(query, ecs_query_t);
    return &query->filter;
}

ecs_iter_t ecs_query_iter(
    const ecs_world_t *stage,
    ecs_query_t *query)
{
    ecs_poly_assert(query, ecs_query_t);
    ecs_check(!(query->flags & EcsQueryIsOrphaned),
        ECS_INVALID_PARAMETER, NULL);

    ecs_world_t *world = query->filter.world;
    ecs_poly_assert(world, ecs_world_t);

    /* Process table events to ensure that the list of iterated tables doesn't
     * contain empty tables. */
    flecs_process_pending_tables(world);

    /* If query has order_by, apply sort */
    flecs_query_sort_tables(world, query);

    /* If monitors changed, do query rematching */
    if (!(world->flags & EcsWorldReadonly) && query->flags & EcsQueryHasRefs) {
        flecs_eval_component_monitors(world);
    }

    query->prev_match_count = query->match_count;

    /* Prepare iterator */

    int32_t table_count;
    if (query->table_slices) {
        table_count = ecs_vector_count(query->table_slices);
    } else {
        table_count = ecs_query_table_count(query);
    }

    ecs_query_iter_t it = {
        .query = query,
        .node = query->list.first,
        .last = NULL
    };

    if (query->order_by && query->list.info.table_count) {
        it.node = ecs_vector_first(query->table_slices, ecs_query_table_node_t);
    }

    ecs_flags32_t flags = 0;
    ECS_BIT_COND(flags, EcsIterIsFilter, ECS_BIT_IS_SET(query->filter.flags, 
        EcsFilterNoData));
    ECS_BIT_COND(flags, EcsIterIsInstanced, ECS_BIT_IS_SET(query->filter.flags, 
        EcsFilterIsInstanced));

    ecs_iter_t result = {
        .real_world = world,
        .world = (ecs_world_t*)stage,
        .terms = query->filter.terms,
        .field_count = query->filter.field_count,
        .table_count = table_count,
        .flags = flags,
        .priv.iter.query = it,
        .next = ecs_query_next,
    };

    ecs_filter_t *filter = &query->filter;
    if (filter->flags & EcsFilterMatchOnlyThis) {
        /* When the query only matches This terms, we can reuse the storage from
        * the cache to populate the iterator */
        flecs_iter_init(stage, &result, flecs_iter_cache_ptrs);
    } else {
        /* Check if non-This terms (like singleton terms) still match */
        ecs_iter_t fit = flecs_filter_iter_w_flags(
            (ecs_world_t*)stage, &query->filter, EcsIterIgnoreThis);
        if (!ecs_filter_next(&fit)) {
            /* No match, so return nothing */
            ecs_iter_fini(&fit);
            goto noresults;
        }

        /* Initialize iterator with private storage for ids, ptrs, sizes and 
        * columns so we have a place to store the non-This data */
        flecs_iter_init(stage, &result, flecs_iter_cache_ptrs | flecs_iter_cache_ids |
            flecs_iter_cache_columns | flecs_iter_cache_sizes);

        /* Copy the data */
        int32_t term_count = filter->field_count;
        if (term_count) {
            if (result.ptrs) {
                ecs_os_memcpy_n(result.ptrs, fit.ptrs, void*, term_count);
            }
            ecs_os_memcpy_n(result.ids, fit.ids, ecs_id_t, term_count);
            ecs_os_memcpy_n(result.sizes, fit.sizes, ecs_size_t, term_count);
            ecs_os_memcpy_n(result.columns, fit.columns, int32_t, term_count);
        }

        ecs_iter_fini(&fit);
    }

    return result;
error:
noresults:
    result.priv.iter.query.node = NULL;
    return result;
}

void ecs_query_set_group(
    ecs_iter_t *it,
    uint64_t group_id)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);
    ecs_check(!(it->flags & EcsIterIsValid), ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *qit = &it->priv.iter.query;
    ecs_query_t *q = qit->query;
    ecs_check(q != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_query_table_list_t *node = flecs_query_get_group(q, group_id);
    if (!node) {
        qit->node = NULL;
        return;
    }

    ecs_query_table_node_t *first = node->first;
    if (first) {
        qit->node = node->first;
        qit->last = node->last->next;
    } else {
        qit->node = NULL;
        qit->last = NULL;
    }
    
error:
    return;
}

const ecs_query_group_info_t* ecs_query_get_group_info(
    ecs_query_t *query,
    uint64_t group_id)
{
    ecs_query_table_list_t *node = flecs_query_get_group(query, group_id);
    if (!node) {
        return NULL;
    }
    
    return &node->info;
}

void* ecs_query_get_group_ctx(
    ecs_query_t *query,
    uint64_t group_id)
{
    const ecs_query_group_info_t *info = 
        ecs_query_get_group_info(query, group_id);
    if (!info) {
        return NULL;
    } else {
        return info->ctx;
    }
}

static
void flecs_query_mark_columns_dirty(
    ecs_query_t *query,
    ecs_query_table_match_t *qm)
{
    ecs_table_t *table = qm->node.table;
    if (!table) {
        return;
    }

    int32_t *dirty_state = table->dirty_state;
    if (dirty_state) {
        int32_t *storage_columns = qm->storage_columns;
        ecs_filter_t *filter = &query->filter;
        ecs_term_t *terms = filter->terms;
        int32_t i, count = filter->term_count;

        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            if (term->inout == EcsIn || term->inout == EcsInOutNone) {
                /* Don't mark readonly terms dirty */
                continue;
            }

            int32_t field = term->field_index;
            int32_t column = storage_columns[field];
            if (column < 0) {
                continue;
            }

            dirty_state[column + 1] ++;
        }
    }
}

bool ecs_query_next_table(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    flecs_iter_validate(it);

    ecs_query_iter_t *iter = &it->priv.iter.query;
    ecs_query_table_node_t *node = iter->node; 
    ecs_query_t *query = iter->query;
    
    if ((query->flags & EcsQueryHasOutColumns)) {
        ecs_query_table_node_t *prev = iter->prev;
        if (prev && it->count) {
            flecs_query_mark_columns_dirty(query, prev->match);
        }
    }

    if (node != iter->last) {
        it->table = node->table;
        it->group_id = node->group_id;
        it->count = 0;
        iter->node = node->next;
        iter->prev = node;
        return true;
    }

error:
    ecs_iter_fini(it);
    return false;
}

void ecs_query_populate(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *iter = &it->priv.iter.query;
    ecs_query_table_node_t *node = iter->prev;
    ecs_assert(node != NULL, ECS_INVALID_OPERATION, NULL);

    ecs_table_t *table = node->table;
    ecs_query_table_match_t *match = node->match;
    ecs_query_t *query = iter->query;
    ecs_world_t *world = query->filter.world;
    const ecs_filter_t *filter = &query->filter;
    bool only_this = filter->flags & EcsFilterMatchOnlyThis;

    /* Match has been iterated, update monitor for change tracking */
    if (query->flags & EcsQueryHasMonitor) {
        flecs_query_sync_match_monitor(query, match);
    }

    if (only_this) {
        /* If query has only This terms, reuse cache storage */
        it->ids = match->ids;
        it->columns = match->columns;
        it->sizes = match->sizes;
    } else {
        /* If query has non-This terms make sure not to overwrite them */
        int32_t t, term_count = filter->term_count;
        for (t = 0; t < term_count; t ++) {
            ecs_term_t *term = &filter->terms[t];
            if (!ecs_term_match_this(term)) {
                continue;
            }

            int32_t field = term->field_index;
            it->ids[field] = match->ids[field];
            it->columns[field] = match->columns[field];
            it->sizes[field] = match->sizes[field];
        }
    }

    it->sources = match->sources;
    it->references = ecs_vec_first(&match->refs);
    it->instance_count = 0;

    flecs_iter_populate_data(world, it, table, 0, ecs_table_count(table),
        it->ptrs, NULL);
error:
    return;
}

bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    if (flecs_iter_next_row(it)) {
        return true;
    }

    return flecs_iter_next_instanced(it, ecs_query_next_instanced(it));
error:
    return false;
}

bool ecs_query_next_instanced(
    ecs_iter_t *it)
{
    ecs_check(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);

    ecs_query_iter_t *iter = &it->priv.iter.query;
    ecs_query_t *query = iter->query;
    ecs_world_t *world = query->filter.world;
    ecs_flags32_t flags = query->flags;
    const ecs_filter_t *filter = &query->filter;
    bool only_this = filter->flags & EcsFilterMatchOnlyThis;
    ecs_poly_assert(world, ecs_world_t);

    ecs_entity_filter_iter_t *ent_it = it->priv.entity_iter;
    ecs_query_table_node_t *node, *next, *prev, *last;
    if ((prev = iter->prev)) {
        /* Match has been iterated, update monitor for change tracking */
        if (flags & EcsQueryHasMonitor) {
            flecs_query_sync_match_monitor(query, prev->match);
        }
        if (flags & EcsQueryHasOutColumns) {
            flecs_query_mark_columns_dirty(query, prev->match);
        }
    }

    flecs_iter_validate(it);
    iter->skip_count = 0;
    last = iter->last;

    for (node = iter->node; node != last; node = next) {     
        ecs_assert(ent_it != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_table_range_t *range = &ent_it->range;
        ecs_query_table_match_t *match = node->match;
        ecs_table_t *table = match->node.table;

        next = node->next;

        if (table) {
            range->offset = node->offset;
            range->count = node->count;
            if (!range->count) {
                range->count = ecs_table_count(table);
                ecs_assert(range->count != 0, ECS_INTERNAL_ERROR, NULL);
            }

            if (match->entity_filter.has_filter) {
                ent_it->entity_filter = &match->entity_filter;
                ent_it->columns = match->columns;
                ent_it->range.table = table;
                switch(flecs_entity_filter_next(ent_it)) {
                case 1: continue;
                case -1: next = node;
                default: break;
                }
            }

            it->group_id = match->node.group_id;
        } else {
            range->offset = 0;
            range->count = 0;
        }

        if (only_this) {
            /* If query has only This terms, reuse cache storage */
            it->ids = match->ids;
            it->columns = match->columns;
            it->sizes = match->sizes;
        } else {
            /* If query has non-This terms make sure not to overwrite them */
            int32_t t, term_count = filter->term_count;
            for (t = 0; t < term_count; t ++) {
                ecs_term_t *term = &filter->terms[t];
                if (!ecs_term_match_this(term)) {
                    continue;
                }

                int32_t field = term->field_index;
                it->ids[field] = match->ids[field];
                it->columns[field] = match->columns[field];
                it->sizes[field] = match->sizes[field];
            }
        }

        it->sources = match->sources;
        it->references = ecs_vec_first(&match->refs);
        it->instance_count = 0;

        flecs_iter_populate_data(world, it, table, range->offset, range->count,
            it->ptrs, NULL);

        iter->node = next;
        iter->prev = node;
        goto yield;
    }

error:
    ecs_iter_fini(it);
    return false;

yield:
    return true;  
}

bool ecs_query_changed(
    ecs_query_t *query,
    const ecs_iter_t *it)
{
    if (it) {
        ecs_check(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);
        ecs_check(ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
            ECS_INVALID_PARAMETER, NULL);

        ecs_query_table_match_t *qm = 
            (ecs_query_table_match_t*)it->priv.iter.query.prev;
        ecs_assert(qm != NULL, ECS_INVALID_PARAMETER, NULL);

        if (!query) {
            query = it->priv.iter.query.query;
        } else {
            ecs_check(query == it->priv.iter.query.query, 
                ECS_INVALID_PARAMETER, NULL);
        }

        ecs_check(query != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_poly_assert(query, ecs_query_t);

        return flecs_query_check_match_monitor(query, qm);
    }

    ecs_poly_assert(query, ecs_query_t);
    ecs_check(!(query->flags & EcsQueryIsOrphaned), 
        ECS_INVALID_PARAMETER, NULL);

    flecs_process_pending_tables(query->filter.world);

    if (!(query->flags & EcsQueryHasMonitor)) {
        query->flags |= EcsQueryHasMonitor;
        flecs_query_init_query_monitors(query);
        return true; /* Monitors didn't exist yet */
    }

    if (query->match_count != query->prev_match_count) {
        return true;
    }

    return flecs_query_check_query_monitor(query);
error:
    return false;
}

void ecs_query_skip(
    ecs_iter_t *it)
{
    ecs_assert(it->next == ecs_query_next, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ECS_BIT_IS_SET(it->flags, EcsIterIsValid), 
        ECS_INVALID_PARAMETER, NULL);

    if (it->instance_count > it->count) {
        it->priv.iter.query.skip_count ++;
        if (it->priv.iter.query.skip_count == it->instance_count) {
            /* For non-instanced queries, make sure all entities are skipped */
            it->priv.iter.query.prev = NULL;
        }
    } else {
        it->priv.iter.query.prev = NULL;
    }
}

bool ecs_query_orphaned(
    ecs_query_t *query)
{
    ecs_poly_assert(query, ecs_query_t);
    return query->flags & EcsQueryIsOrphaned;
}

char* ecs_query_str(
    const ecs_query_t *query)
{
    return ecs_filter_str(query->filter.world, &query->filter);
}

int32_t ecs_query_table_count(
    const ecs_query_t *query)
{
    ecs_run_aperiodic(query->filter.world, EcsAperiodicEmptyTables);
    return query->cache.tables.count;
}

int32_t ecs_query_empty_table_count(
    const ecs_query_t *query)
{
    ecs_run_aperiodic(query->filter.world, EcsAperiodicEmptyTables);
    return query->cache.empty_tables.count;
}

int32_t ecs_query_entity_count(
    const ecs_query_t *query)
{
    ecs_run_aperiodic(query->filter.world, EcsAperiodicEmptyTables);
    
    int32_t result = 0;
    ecs_table_cache_hdr_t *cur, *last = query->cache.tables.last;
    if (!last) {
        return 0;
    }

    for (cur = query->cache.tables.first; cur != NULL; cur = cur->next) {
        result += ecs_table_count(cur->table);
    }

    return result;
}
