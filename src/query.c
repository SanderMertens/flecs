#include "private_api.h"

static
void flecs_query_compute_group_id(
    ecs_query_t *query,
    ecs_query_table_match_t *match)
{
    ecs_assert(match != NULL, ECS_INTERNAL_ERROR, NULL);

    if (query->group_by) {
        ecs_table_t *table = match->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        match->group_id = query->group_by(query->world, table, 
            query->group_by_id, query->group_by_ctx);
    } else {
        match->group_id = 0;
    }
}

static
ecs_query_table_list_t* flecs_query_get_group(
    ecs_query_t *query,
    uint64_t group_id)
{
    return ecs_map_get(&query->groups, ecs_query_table_list_t, group_id);
}

static
ecs_query_table_list_t* flecs_query_ensure_group(
    ecs_query_t *query,
    uint64_t group_id)
{
    return ecs_map_ensure(&query->groups, ecs_query_table_list_t, group_id);
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
    while ((list = ecs_map_next(&it, ecs_query_table_list_t, &id))) {
        if (id >= group_id) {
            continue;
        }

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
    uint64_t group_id = match->group_id;

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

static
void flecs_query_remove_group(
    ecs_query_t *query,
    uint64_t group_id)
{
    ecs_map_remove(&query->groups, group_id);
}

/* Find the list the node should be part of */
static
ecs_query_table_list_t* flecs_query_get_node_list(
    ecs_query_t *query,
    ecs_query_table_node_t *node)
{
    ecs_query_table_match_t *match = node->match;
    if (query->group_by) {
        return flecs_query_get_group(query, match->group_id);
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
        return flecs_query_ensure_group(query, match->group_id);
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

    ecs_assert(list->count > 0, ECS_INTERNAL_ERROR, NULL);
    list->count --;

    if (query->group_by) {
        ecs_query_table_match_t *match = node->match;
        uint64_t group_id = match->group_id;

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

        ecs_assert(query->list.count > 0, ECS_INTERNAL_ERROR, NULL);
        query->list.count --;

        /* Make sure group list only contains nodes that belong to the group */
        if (prev && prev->match->group_id != group_id) {
            /* The previous node belonged to another group */
            prev = next;
        }
        if (next && next->match->group_id != group_id) {
            /* The next node belonged to another group */
            next = prev;
        }

        /* Do check again, in case both prev & next belonged to another group */
        if (prev && prev->match->group_id != group_id) {
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
    if (!query->list.first && query->entity) {
        ecs_remove_id(query->world, query->entity, EcsEmpty);
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
        query->list.count ++;
    }

    list->count ++;
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
    ecs_query_table_t *elem)
{
    ecs_query_table_match_t *result = ecs_os_calloc_t(ecs_query_table_match_t);
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
    ecs_world_t *world = query->world;
    ecs_entity_t subject = match->subjects[term];
    int32_t column;

    if (!subject) {
        out->table = match->table;
        column = match->columns[term];
        if (column == -1) {
            column = 0;
        }
    } else {
        out->table = ecs_get_table(world, subject);
        column = -match->columns[term];
    }

    out->dirty_state = flecs_table_get_dirty_state(out->table);

    if (column) {
        out->column = ecs_table_type_to_storage_index(out->table, column - 1);
    } else {
        out->column = -1;
    }
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

    int32_t *monitor = ecs_os_calloc_n(int32_t, query->filter.term_count + 1);

    /* Mark terms that don't need to be monitored. This saves time when reading
     * and/or updating the monitor. */
    const ecs_filter_t *f = &query->filter;
    int32_t i, t = -1, term_count = f->term_count_actual;
    table_dirty_state_t cur_dirty_state;

    for (i = 0; i < term_count; i ++) {
        if (t == f->terms[i].index) {
            if (monitor[t + 1] != -1) {
                continue;
            }
        }

        t = f->terms[i].index;
        monitor[t + 1] = -1;

        if (f->terms[i].inout != EcsIn && 
            f->terms[i].inout != EcsInOut &&
            f->terms[i].inout != EcsInOutDefault) {
            continue; /* If term isn't read, don't monitor */
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
    ecs_table_t *table = match->table;
    int32_t *dirty_state = flecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    table_dirty_state_t cur;

    monitor[0] = dirty_state[0]; /* Did table gain/lose entities */

    int32_t i, term_count = query->filter.term_count_actual;
    for (i = 0; i < term_count; i ++) {
        int32_t t = query->filter.terms[i].index;
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
    ecs_table_t *table = match->table;
    int32_t *dirty_state = flecs_table_get_dirty_state(table);
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
    ecs_table_t *table = match->table;
    int32_t *dirty_state = flecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    table_dirty_state_t cur;

    if (monitor[0] != dirty_state[0]) {
        return true;
    }

    ecs_filter_t *f = &query->filter;
    int32_t i, term_count = f->term_count_actual;
    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &f->terms[i];
        int32_t t = term->index;
        if (monitor[t + 1] == -1) {
            continue;
        }

        flecs_query_get_dirty_state(query, match, t, &cur);
        ecs_assert(cur.column != -1, ECS_INTERNAL_ERROR, NULL);

        if (monitor[t + 1] != cur.dirty_state[cur.column + 1]) {
            return true;
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
    ecs_entity_t rel = term->subj.set.relation;
    int32_t depth = flecs_relation_depth(world, rel, table);
    return flecs_ito(uint64_t, depth);
}

static
ecs_vector_t* flecs_query_add_ref(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_vector_t *references,
    ecs_term_t *term,
    ecs_entity_t component,
    ecs_entity_t entity,
    ecs_size_t size)
{
    ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_ref_t *ref = ecs_vector_add(&references, ecs_ref_t);
    ecs_term_id_t *subj = &term->subj;

    if (!(subj->set.mask & EcsCascade)) {
        ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    }
    
    if (size) {
        *ref = ecs_ref_init_id(world, entity, component);
    } else {
        *ref = (ecs_ref_t){
            .entity = entity,
            .id = 0
        };
    }

    query->flags |= EcsQueryHasRefs;

    return references;
}

static
ecs_query_table_match_t* flecs_query_add_table_match(
    ecs_query_t *query,
    ecs_query_table_t *qt,
    ecs_table_t *table)
{
    ecs_filter_t *filter = &query->filter;
    int32_t term_count = filter->term_count;

    /* Add match for table. One table can have more than one match, if
     * the query contains wildcards. */
    ecs_query_table_match_t *qm = flecs_query_cache_add(qt);
    qm->table = table;
    qm->columns = ecs_os_malloc_n(int32_t, term_count);
    qm->ids = ecs_os_malloc_n(ecs_id_t, term_count);
    qm->subjects = ecs_os_malloc_n(ecs_entity_t, term_count);
    qm->sizes = ecs_os_malloc_n(ecs_size_t, term_count);

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
    ecs_filter_t *filter = &query->filter;
    int32_t i, term_count = filter->term_count;
    int32_t term_count_actual = filter->term_count_actual;
    ecs_term_t *terms = filter->terms;

    /* Reset resources in case this is an existing record */
    if (qm->sparse_columns) {
        ecs_vector_free(qm->sparse_columns);
        qm->sparse_columns = NULL;
    }
    if (qm->bitset_columns) {
        ecs_vector_free(qm->bitset_columns);
        qm->bitset_columns = NULL;
    }
    if (qm->references) {
        ecs_os_free(qm->references);
        qm->references = NULL;
    }

    ecs_os_memcpy_n(qm->columns, it->columns, int32_t, term_count_actual);
    ecs_os_memcpy_n(qm->ids, it->ids, ecs_id_t, term_count_actual);
    ecs_os_memcpy_n(qm->subjects, it->subjects, ecs_entity_t, term_count_actual);
    ecs_os_memcpy_n(qm->sizes, it->sizes, ecs_size_t, term_count_actual);

    /* Look for union & disabled terms */
    if (table) {
        if (table->flags & EcsTableHasUnion) {
            for (i = 0; i < term_count; i ++) {
                if (terms[i].subj.set.mask == EcsNothing) {
                    continue;
                }

                ecs_id_t id = terms[i].id;
                if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_SECOND(id) == EcsWildcard) {
                    continue;
                }
                
                int32_t actual_index = terms[i].index;
                int32_t column = it->columns[actual_index];
                if (column <= 0) {
                    continue;
                }

                ecs_id_t table_id = table->type.array[column - 1];
                if (ECS_PAIR_FIRST(table_id) != EcsUnion) {
                    continue;
                }

                flecs_switch_term_t *sc = ecs_vector_add(
                     &qm->sparse_columns, flecs_switch_term_t);
                sc->signature_column_index = actual_index;
                sc->sw_case = ECS_PAIR_SECOND(id);
                sc->sw_column = NULL;
                qm->ids[actual_index] = id;
            }
        }
        if (table->flags & EcsTableHasDisabled) {
            for (i = 0; i < term_count; i ++) {
                if (terms[i].subj.set.mask == EcsNothing) {
                    continue;
                }

                int32_t actual_index = terms[i].index;
                ecs_id_t id = it->ids[actual_index];
                ecs_id_t bs_id = ECS_DISABLED | (id & ECS_COMPONENT_MASK);
                int32_t bs_index = ecs_search(world, table, bs_id, 0);
                if (bs_index != -1) {
                    flecs_bitset_term_t *bc = ecs_vector_add(
                        &qm->bitset_columns, flecs_bitset_term_t);
                    bc->column_index = bs_index;
                    bc->bs_column = NULL;
                }
            }
        }
    }

    /* Add references for substituted terms */
    if (!ECS_BIT_IS_SET(filter->flags, EcsFilterMatchOnlyThis)) {
        ecs_vector_t *refs = NULL;
        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &terms[i];
            if (term->subj.entity != EcsThis) {
                /* non-This terms are set during iteration */
                continue;
            }

            int32_t actual_index = terms[i].index;
            ecs_entity_t src = it->subjects[actual_index];
            ecs_size_t size = 0;
            if (it->sizes) {
                size = it->sizes[actual_index];
            }
            if (src) {
                ecs_id_t id = it->ids[actual_index];
                ecs_assert(ecs_is_valid(world, src), ECS_INTERNAL_ERROR, NULL);

                if (id) {
                    refs = flecs_query_add_ref(world, query, refs, term, id, src, size);

                    /* Use column index to bind term and ref */
                    if (qm->columns[actual_index] != 0) {
                        qm->columns[actual_index] = -ecs_vector_count(refs);
                    }
                }
            }
        }
        if (refs) {
            int32_t count = ecs_vector_count(refs);
            ecs_ref_t *ptr = ecs_vector_first(refs, ecs_ref_t);
            qm->references = ecs_os_memdup_n(ptr, ecs_ref_t, count);
            ecs_vector_free(refs);
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
            qt = ecs_os_calloc_t(ecs_query_table_t);
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
    if (!ecs_map_is_initialized(&query->cache.index)) {
        return false;
    }

    ecs_query_table_t *qt = NULL;
    int var_id = ecs_filter_find_this_var(&query->filter);
    if (var_id == -1) {
        /* If query doesn't match with This term, it can't match with tables */
        return false;
    }

    ecs_iter_t it = ecs_filter_iter(world, &query->filter);
    ECS_BIT_SET(it.flags, EcsIterIsInstanced);
    ECS_BIT_SET(it.flags, EcsIterIsFilter);
    ECS_BIT_SET(it.flags, EcsIterEntityOptional);
    ecs_iter_set_var_as_table(&it, var_id, table);

    while (ecs_filter_next(&it)) {
        ecs_assert(it.table == table, ECS_INTERNAL_ERROR, NULL);
        if (qt == NULL) {
            qt = ecs_os_calloc_t(ecs_query_table_t);
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
    if (!ecs_storage_count(&data->entities)) {
        /* Nothing to sort */
        return;
    }

    int32_t count = flecs_table_data_count(data);
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = ecs_storage_first(&data->entities);

    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_type_info_t *ti = table->type_info[column_index];
        ecs_column_t *column = &data->columns[column_index];
        size = ti->size;
        ptr = ecs_storage_first(column);
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
    ecs_world_t *world = query->world;
    ecs_entity_t id = query->order_by_component;
    ecs_order_by_action_t compare = query->order_by;
    
    if (!list->count) {
        return;
    }

    int to_sort = 0;

    sort_helper_t *helper = ecs_os_malloc_n(sort_helper_t, list->count);
    ecs_query_table_node_t *cur, *end = list->last->next;
    for (cur = list->first; cur != end; cur = cur->next) {
        ecs_query_table_match_t *match = cur->match;
        ecs_table_t *table = match->table;
        ecs_data_t *data = &table->data;

        ecs_assert(ecs_table_count(table) != 0, ECS_INTERNAL_ERROR, NULL);

        int32_t index = -1;
        if (id) {
            index = ecs_search(world, table->storage_table, id, 0);
        }

        if (index != -1) {
            ecs_type_info_t *ti = table->type_info[index];
            ecs_column_t *column = &data->columns[index];
            int32_t size = ti->size;
            helper[to_sort].ptr = ecs_storage_first(column);
            helper[to_sort].elem_size = size;
            helper[to_sort].shared = false;
        } else if (id) {
            /* Find component in prefab */
            ecs_entity_t base = 0;
            ecs_search_relation(world, table, 0, id, 
                EcsIsA, 1, 0, &base, 0, 0, 0);

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
        helper[to_sort].entities = ecs_storage_first(&data->entities);
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
                uint64_t group_id = match->group_id;
                ecs_query_table_list_t *list = ecs_map_get(&query->groups, 
                    ecs_query_table_list_t, group_id);
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
        int32_t term_count = f->term_count_actual;
        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &f->terms[i];
            if (term->subj.entity != EcsThis) {
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
                        order_by_component, NULL);
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
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->subj;

        if (term->oper == EcsNot && !subj->entity) {
            /* Special case: if oper kind is Not and the query contained a
             * shared expression, the expression is translated to FromEmpty to
             * prevent resolving the ref */
            return true;
        } else if (subj->entity && (subj->entity != EcsThis || subj->set.mask != EcsSelf)) {
            /* If entity is not this, or if it can be substituted by other
             * entities, the query can have references. */
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
        ecs_term_id_t *subj = &term->subj;

        if (subj->set.mask & EcsSuperSet) {
            callback(world, ecs_pair(subj->set.relation, EcsWildcard), query);
            if (subj->set.relation != EcsIsA) {
                callback(world, ecs_pair(EcsIsA, EcsWildcard), query);
            }
            callback(world, term->id, query);

        } else if (subj->set.mask & EcsSelf && subj->entity != EcsThis) {
            callback(world, term->id, query);
        }
    }
}

static
bool flecs_query_is_term_id_supported(
    ecs_term_id_t *term_id)
{
    if (term_id->var != EcsVarIsVariable) {
        return true;
    }
    if ((term_id->entity == EcsWildcard) || (term_id->entity == EcsAny)) {
        return true;
    }
    return false;
}

static
void flecs_query_process_signature(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *pred = &term->pred;
        ecs_term_id_t *subj = &term->subj;
        ecs_term_id_t *obj = &term->obj;
        ecs_inout_kind_t inout = term->inout;

        bool is_pred_supported = flecs_query_is_term_id_supported(pred);
        bool is_subj_supported = flecs_query_is_term_id_supported(subj);
        bool is_obj_supported = flecs_query_is_term_id_supported(obj);

        (void)pred;
        (void)obj;
        (void)is_pred_supported;
        (void)is_subj_supported;
        (void)is_obj_supported;

        /* Queries do not support named variables */
        ecs_check(is_pred_supported, ECS_UNSUPPORTED, NULL);
        ecs_check(is_obj_supported,  ECS_UNSUPPORTED, NULL);
        ecs_check(is_subj_supported || subj->entity == EcsThis, 
            ECS_UNSUPPORTED, NULL);

        if (inout != EcsIn) {
            query->flags |= EcsQueryHasOutColumns;
        }

        if (subj->set.mask & EcsCascade) {
            /* Query can only have one cascade column */
            ecs_assert(query->cascade_by == 0, ECS_INVALID_PARAMETER, NULL);
            query->cascade_by = i + 1;
        }

        if (subj->entity && subj->entity != EcsThis && 
            subj->set.mask == EcsSelf) 
        {
            flecs_add_flag(world, term->subj.entity, EcsEntityObserved);
        }
    }

    query->flags |= (ecs_flags32_t)(flecs_query_has_refs(query) * EcsQueryHasRefs);

    if (!(query->flags & EcsQueryIsSubquery)) {
        flecs_query_for_each_component_monitor(world, query, flecs_monitor_register);
    }
error:
    return;
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

    for (cur = first; cur != NULL; cur = next) {
        ecs_os_free(cur->columns);
        ecs_os_free(cur->ids);
        ecs_os_free(cur->subjects);
        ecs_os_free(cur->sizes);
        ecs_os_free(cur->references);
        ecs_os_free(cur->sparse_columns);
        ecs_os_free(cur->bitset_columns);
        ecs_os_free(cur->monitor);

        if (!elem->hdr.empty) {
            flecs_query_remove_table_node(query, &cur->node);
        }

        next = cur->next_match;

        ecs_os_free(cur);
    }
}

static
void flecs_query_table_free(
    ecs_query_t *query,
    ecs_query_table_t *elem)
{
    flecs_query_table_match_free(query, elem, elem->first);
    ecs_os_free(elem);
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

    if (parent_query) {
        parent_it = ecs_query_iter(world, parent_query);
        it = ecs_filter_chain_iter(&parent_it, &query->filter);
    } else {
        it = ecs_filter_iter(world, &query->filter);
    }

    ECS_BIT_SET(it.flags, EcsIterIsInstanced);
    ECS_BIT_SET(it.flags, EcsIterIsFilter);
    ECS_BIT_SET(it.flags, EcsIterEntityOptional);

    int32_t rematch_count = ++ query->rematch_count;

    while (ecs_iter_next(&it)) {
        if ((table != it.table) || (!it.table && !qt)) {
            if (qm && qm->next_match) {
                flecs_query_table_match_free(query, qt, qm->next_match);
                qm->next_match = NULL;
            }

            table = it.table;

            qt = ecs_table_cache_get(&query->cache, table);
            if (!qt) {
                qt = ecs_os_calloc_t(ecs_query_table_t);
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
            /* If grouping is enabled, make sure match is in the right place */
            flecs_query_remove_table_node(query, &qm->node);
            flecs_query_insert_table_node(query, &qm->node);
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

    query->group_by_id = sort_component;
    query->group_by = group_by;
    ecs_map_init(&query->groups, ecs_query_table_list_t, 16);
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
void flecs_query_fini(
    ecs_query_t *query)
{
    ecs_world_t *world = query->world;

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
    ecs_entity_t entity = desc->entity;

    observer_desc.filter.flags = EcsFilterMatchEmptyTables;
    observer_desc.filter.storage = &result->filter;
    result->filter = ECS_FILTER_INIT;

    if (ecs_filter_init(world, &observer_desc.filter) == NULL) {
        goto error;
    }

    if (result->filter.term_count) {
        observer_desc.entity.entity = entity;
        observer_desc.run = flecs_query_on_event;
        observer_desc.ctx = result;
        observer_desc.events[0] = EcsOnTableEmpty;
        observer_desc.events[1] = EcsOnTableFill;
        observer_desc.filter.flags |= EcsFilterIsFilter;

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

    result->world = world;
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

    flecs_query_process_signature(world, result);

    /* Group before matching so we won't have to move tables around later */
    int32_t cascade_by = result->cascade_by;
    if (cascade_by) {
        flecs_query_group_by(result, result->filter.terms[cascade_by - 1].id,
            flecs_query_group_by_cascade);
        result->group_by_ctx = &result->filter.terms[cascade_by - 1];
    }

    if (desc->group_by) {
        /* Can't have a cascade term and group by at the same time, as cascade
         * uses the group_by mechanism */
        ecs_check(!result->cascade_by, ECS_INVALID_PARAMETER, NULL);
        flecs_query_group_by(result, desc->group_by_id, desc->group_by);
        result->group_by_ctx = desc->group_by_ctx;
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
            if (term->subj.entity == entity) {
                ecs_add_id(world, entity, term->id);
            }
        }        
    }

    if (!entity) {
        entity = ecs_new_id(world);
    }

    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_query_t);
    poly->poly = result;
    result->entity = entity;

    /* Ensure that while initially populating the query with tables, they are
     * in the right empty/non-empty list. This ensures the query won't miss
     * empty/non-empty events for tables that are currently out of sync, but
     * change back to being in sync before processing pending events. */
    ecs_run_aperiodic(world, EcsAperiodicEmptyTables);

    ecs_table_cache_init(&result->cache);

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
    ecs_delete(query->world, query->entity);
}

const ecs_filter_t* ecs_query_get_filter(
    ecs_query_t *query)
{
    ecs_poly_assert(query, ecs_query_t);
    return &query->filter;
}

/* Create query iterator */
ecs_iter_t ecs_query_iter(
    const ecs_world_t *stage,
    ecs_query_t *query)
{
    ecs_poly_assert(query, ecs_query_t);
    ecs_check(!(query->flags & EcsQueryIsOrphaned),
        ECS_INVALID_PARAMETER, NULL);

    ecs_world_t *world = query->world;
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
        .node = query->list.first
    };

    if (query->order_by && query->list.count) {
        it.node = ecs_vector_first(query->table_slices, ecs_query_table_node_t);
    }

    ecs_flags32_t flags = 0;
    ECS_BIT_COND(flags, EcsIterIsFilter, ECS_BIT_IS_SET(query->filter.flags, 
        EcsFilterIsFilter));
    ECS_BIT_COND(flags, EcsIterIsInstanced, ECS_BIT_IS_SET(query->filter.flags, 
        EcsFilterIsInstanced));

    ecs_iter_t result = {
        .real_world = world,
        .world = (ecs_world_t*)stage,
        .terms = query->filter.terms,
        .term_count = query->filter.term_count_actual,
        .table_count = table_count,
        .flags = flags,
        .priv.iter.query = it,
        .next = ecs_query_next,
    };

    ecs_filter_t *filter = &query->filter;
    if (filter->flags & EcsFilterMatchOnlyThis) {
        /* When the query only matches This terms, we can reuse the storage from
         * the cache to populate the iterator */
        flecs_iter_init(&result, flecs_iter_cache_ptrs);
    } else {
        /* Check if non-This terms (like singleton terms) still match */
        ecs_iter_t fit = flecs_filter_iter_w_flags(
            world, &query->filter, EcsIterIgnoreThis);
        if (!ecs_filter_next(&fit)) {
            /* No match, so return nothing */
            ecs_iter_fini(&result);
            goto noresults;
        }

        /* Initialize iterator with private storage for ids, ptrs, sizes and 
         * columns so we have a place to store the non-This data */
        flecs_iter_init(&result, flecs_iter_cache_ptrs | flecs_iter_cache_ids |
            flecs_iter_cache_columns | flecs_iter_cache_sizes);

        /* Copy the data */
        int32_t term_count = filter->term_count_actual;
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
    return (ecs_iter_t) {
        .flags = EcsIterNoResults,
        .next = ecs_query_next
    };
}

static
int find_smallest_column(
    ecs_table_t *table,
    ecs_query_table_match_t *table_data,
    ecs_vector_t *sparse_columns)
{
    flecs_switch_term_t *sparse_column_array = 
        ecs_vector_first(sparse_columns, flecs_switch_term_t);
    int32_t i, count = ecs_vector_count(sparse_columns);
    int32_t min = INT_MAX, index = 0;

    for (i = 0; i < count; i ++) {
        /* The array with sparse queries for the matched table */
        flecs_switch_term_t *sparse_column = &sparse_column_array[i];

        /* Pointer to the switch column struct of the table */
        ecs_switch_t *sw = sparse_column->sw_column;

        /* If the sparse column pointer hadn't been retrieved yet, do it now */
        if (!sw) {
            /* Get the table column index from the signature column index */
            int32_t table_column_index = table_data->columns[
                sparse_column->signature_column_index];

            /* Translate the table column index to switch column index */
            table_column_index -= table->sw_offset;
            ecs_assert(table_column_index >= 1, ECS_INTERNAL_ERROR, NULL);

            /* Get the sparse column */
            ecs_data_t *data = &table->data;
            sw = sparse_column->sw_column = 
                &data->sw_columns[table_column_index - 1];
        }

        /* Find the smallest column */
        int32_t case_count = flecs_switch_case_count(sw, sparse_column->sw_case);
        if (case_count < min) {
            min = case_count;
            index = i + 1;
        }
    }

    return index;
}

typedef struct {
    int32_t first;
    int32_t count;
} query_iter_cursor_t;

static
int sparse_column_next(
    ecs_table_t *table,
    ecs_query_table_match_t *matched_table,
    ecs_vector_t *sparse_columns,
    ecs_query_iter_t *iter,
    query_iter_cursor_t *cur,
    bool filter)
{
    bool first_iteration = false;
    int32_t sparse_smallest;

    if (!(sparse_smallest = iter->sparse_smallest)) {
        sparse_smallest = iter->sparse_smallest = find_smallest_column(
            table, matched_table, sparse_columns);
        first_iteration = true;
    }

    sparse_smallest -= 1;

    flecs_switch_term_t *columns = ecs_vector_first(
        sparse_columns, flecs_switch_term_t);
    flecs_switch_term_t *column = &columns[sparse_smallest];
    ecs_switch_t *sw, *sw_smallest = column->sw_column;
    ecs_entity_t case_smallest = column->sw_case;

    /* Find next entity to iterate in sparse column */
    int32_t first, sparse_first = iter->sparse_first;

    if (!filter) {
        if (first_iteration) {
            first = flecs_switch_first(sw_smallest, case_smallest);
        } else {
            first = flecs_switch_next(sw_smallest, sparse_first);
        }
    } else {
        int32_t cur_first = cur->first, cur_count = cur->count;
        first = cur_first;
        while (flecs_switch_get(sw_smallest, first) != case_smallest) {
            first ++;
            if (first >= (cur_first + cur_count)) {
                first = -1;
                break;
            }
        }
    }

    if (first == -1) {
        goto done;
    }

    /* Check if entity matches with other sparse columns, if any */
    int32_t i, count = ecs_vector_count(sparse_columns);
    do {
        for (i = 0; i < count; i ++) {
            if (i == sparse_smallest) {
                /* Already validated this one */
                continue;
            }

            column = &columns[i];
            sw = column->sw_column;

            if (flecs_switch_get(sw, first) != column->sw_case) {
                first = flecs_switch_next(sw_smallest, first);
                if (first == -1) {
                    goto done;
                }
            }
        }
    } while (i != count);

    cur->first = iter->sparse_first = first;
    cur->count = 1;

    return 0;
done:
    /* Iterated all elements in the sparse list, we should move to the
     * next matched table. */
    iter->sparse_smallest = 0;
    iter->sparse_first = 0;

    return -1;
}

#define BS_MAX ((uint64_t)0xFFFFFFFFFFFFFFFF)

static
int bitset_column_next(
    ecs_table_t *table,
    ecs_vector_t *bitset_columns,
    ecs_query_iter_t *iter,
    query_iter_cursor_t *cur)
{
    /* Precomputed single-bit test */
    static const uint64_t bitmask[64] = {
    (uint64_t)1 << 0, (uint64_t)1 << 1, (uint64_t)1 << 2, (uint64_t)1 << 3,
    (uint64_t)1 << 4, (uint64_t)1 << 5, (uint64_t)1 << 6, (uint64_t)1 << 7,
    (uint64_t)1 << 8, (uint64_t)1 << 9, (uint64_t)1 << 10, (uint64_t)1 << 11,
    (uint64_t)1 << 12, (uint64_t)1 << 13, (uint64_t)1 << 14, (uint64_t)1 << 15,
    (uint64_t)1 << 16, (uint64_t)1 << 17, (uint64_t)1 << 18, (uint64_t)1 << 19,
    (uint64_t)1 << 20, (uint64_t)1 << 21, (uint64_t)1 << 22, (uint64_t)1 << 23,
    (uint64_t)1 << 24, (uint64_t)1 << 25, (uint64_t)1 << 26, (uint64_t)1 << 27,  
    (uint64_t)1 << 28, (uint64_t)1 << 29, (uint64_t)1 << 30, (uint64_t)1 << 31,
    (uint64_t)1 << 32, (uint64_t)1 << 33, (uint64_t)1 << 34, (uint64_t)1 << 35,  
    (uint64_t)1 << 36, (uint64_t)1 << 37, (uint64_t)1 << 38, (uint64_t)1 << 39,
    (uint64_t)1 << 40, (uint64_t)1 << 41, (uint64_t)1 << 42, (uint64_t)1 << 43,
    (uint64_t)1 << 44, (uint64_t)1 << 45, (uint64_t)1 << 46, (uint64_t)1 << 47,  
    (uint64_t)1 << 48, (uint64_t)1 << 49, (uint64_t)1 << 50, (uint64_t)1 << 51,
    (uint64_t)1 << 52, (uint64_t)1 << 53, (uint64_t)1 << 54, (uint64_t)1 << 55,  
    (uint64_t)1 << 56, (uint64_t)1 << 57, (uint64_t)1 << 58, (uint64_t)1 << 59,
    (uint64_t)1 << 60, (uint64_t)1 << 61, (uint64_t)1 << 62, (uint64_t)1 << 63
    };

    /* Precomputed test to verify if remainder of block is set (or not) */
    static const uint64_t bitmask_remain[64] = {
    BS_MAX, BS_MAX - (BS_MAX >> 63), BS_MAX - (BS_MAX >> 62),
    BS_MAX - (BS_MAX >> 61), BS_MAX - (BS_MAX >> 60), BS_MAX - (BS_MAX >> 59),
    BS_MAX - (BS_MAX >> 58), BS_MAX - (BS_MAX >> 57), BS_MAX - (BS_MAX >> 56), 
    BS_MAX - (BS_MAX >> 55), BS_MAX - (BS_MAX >> 54), BS_MAX - (BS_MAX >> 53), 
    BS_MAX - (BS_MAX >> 52), BS_MAX - (BS_MAX >> 51), BS_MAX - (BS_MAX >> 50), 
    BS_MAX - (BS_MAX >> 49), BS_MAX - (BS_MAX >> 48), BS_MAX - (BS_MAX >> 47), 
    BS_MAX - (BS_MAX >> 46), BS_MAX - (BS_MAX >> 45), BS_MAX - (BS_MAX >> 44), 
    BS_MAX - (BS_MAX >> 43), BS_MAX - (BS_MAX >> 42), BS_MAX - (BS_MAX >> 41), 
    BS_MAX - (BS_MAX >> 40), BS_MAX - (BS_MAX >> 39), BS_MAX - (BS_MAX >> 38), 
    BS_MAX - (BS_MAX >> 37), BS_MAX - (BS_MAX >> 36), BS_MAX - (BS_MAX >> 35), 
    BS_MAX - (BS_MAX >> 34), BS_MAX - (BS_MAX >> 33), BS_MAX - (BS_MAX >> 32), 
    BS_MAX - (BS_MAX >> 31), BS_MAX - (BS_MAX >> 30), BS_MAX - (BS_MAX >> 29), 
    BS_MAX - (BS_MAX >> 28), BS_MAX - (BS_MAX >> 27), BS_MAX - (BS_MAX >> 26), 
    BS_MAX - (BS_MAX >> 25), BS_MAX - (BS_MAX >> 24), BS_MAX - (BS_MAX >> 23), 
    BS_MAX - (BS_MAX >> 22), BS_MAX - (BS_MAX >> 21), BS_MAX - (BS_MAX >> 20), 
    BS_MAX - (BS_MAX >> 19), BS_MAX - (BS_MAX >> 18), BS_MAX - (BS_MAX >> 17), 
    BS_MAX - (BS_MAX >> 16), BS_MAX - (BS_MAX >> 15), BS_MAX - (BS_MAX >> 14), 
    BS_MAX - (BS_MAX >> 13), BS_MAX - (BS_MAX >> 12), BS_MAX - (BS_MAX >> 11), 
    BS_MAX - (BS_MAX >> 10), BS_MAX - (BS_MAX >> 9), BS_MAX - (BS_MAX >> 8), 
    BS_MAX - (BS_MAX >> 7), BS_MAX - (BS_MAX >> 6), BS_MAX - (BS_MAX >> 5), 
    BS_MAX - (BS_MAX >> 4), BS_MAX - (BS_MAX >> 3), BS_MAX - (BS_MAX >> 2),
    BS_MAX - (BS_MAX >> 1)
    };

    int32_t i, count = ecs_vector_count(bitset_columns);
    flecs_bitset_term_t *columns = ecs_vector_first(
        bitset_columns, flecs_bitset_term_t);
    int32_t bs_offset = table->bs_offset;

    int32_t first = iter->bitset_first;
    int32_t last = 0;

    for (i = 0; i < count; i ++) {
        flecs_bitset_term_t *column = &columns[i];
        ecs_bitset_t *bs = columns[i].bs_column;

        if (!bs) {
            int32_t index = column->column_index;
            ecs_assert((index - bs_offset >= 0), ECS_INTERNAL_ERROR, NULL);
            bs = &table->data.bs_columns[index - bs_offset];
            columns[i].bs_column = bs;
        }
        
        int32_t bs_elem_count = bs->count;
        int32_t bs_block = first >> 6;
        int32_t bs_block_count = ((bs_elem_count - 1) >> 6) + 1;

        if (bs_block >= bs_block_count) {
            goto done;
        }

        uint64_t *data = bs->data;
        int32_t bs_start = first & 0x3F;

        /* Step 1: find the first non-empty block */
        uint64_t v = data[bs_block];
        uint64_t remain = bitmask_remain[bs_start];
        while (!(v & remain)) {
            /* If no elements are remaining, move to next block */
            if ((++bs_block) >= bs_block_count) {
                /* No non-empty blocks left */
                goto done;
            }

            bs_start = 0;
            remain = BS_MAX; /* Test the full block */
            v = data[bs_block];
        }

        /* Step 2: find the first non-empty element in the block */
        while (!(v & bitmask[bs_start])) {
            bs_start ++;

            /* Block was not empty, so bs_start must be smaller than 64 */
            ecs_assert(bs_start < 64, ECS_INTERNAL_ERROR, NULL);
        }
        
        /* Step 3: Find number of contiguous enabled elements after start */
        int32_t bs_end = bs_start, bs_block_end = bs_block;
        
        remain = bitmask_remain[bs_end];
        while ((v & remain) == remain) {
            bs_end = 0;
            bs_block_end ++;

            if (bs_block_end == bs_block_count) {
                break;
            }

            v = data[bs_block_end];
            remain = BS_MAX; /* Test the full block */
        }

        /* Step 4: find remainder of enabled elements in current block */
        if (bs_block_end != bs_block_count) {
            while ((v & bitmask[bs_end])) {
                bs_end ++;
            }
        }

        /* Block was not 100% occupied, so bs_start must be smaller than 64 */
        ecs_assert(bs_end < 64, ECS_INTERNAL_ERROR, NULL);

        /* Step 5: translate to element start/end and make sure that each column
         * range is a subset of the previous one. */
        first = bs_block * 64 + bs_start;
        int32_t cur_last = bs_block_end * 64 + bs_end;
        
        /* No enabled elements found in table */
        if (first == cur_last) {
            goto done;
        }

        /* If multiple bitsets are evaluated, make sure each subsequent range
         * is equal or a subset of the previous range */
        if (i) {
            /* If the first element of a subsequent bitset is larger than the
             * previous last value, start over. */
            if (first >= last) {
                i = -1;
                continue;
            }

            /* Make sure the last element of the range doesn't exceed the last
             * element of the previous range. */
            if (cur_last > last) {
                cur_last = last;
            }
        }

        last = cur_last;
        int32_t elem_count = last - first;

        /* Make sure last element doesn't exceed total number of elements in 
         * the table */
        if (elem_count > (bs_elem_count - first)) {
            elem_count = (bs_elem_count - first);
            if (!elem_count) {
                iter->bitset_first = 0;
                goto done;
            }
        }
        
        cur->first = first;
        cur->count = elem_count;
        iter->bitset_first = first;
    }
    
    /* Keep track of last processed element for iteration */ 
    iter->bitset_first = last;

    return 0;
done:
    iter->sparse_smallest = 0;
    iter->sparse_first = 0;
    return -1;
}

static
void mark_columns_dirty(
    ecs_query_t *query,
    ecs_query_table_match_t *table_data)
{
    ecs_table_t *table = table_data->table;

    if (table && table->dirty_state) {
        ecs_term_t *terms = query->filter.terms;
        int32_t i, count = query->filter.term_count_actual;
        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            int32_t ti = term->index;

            if (term->inout == EcsIn || term->inout == EcsInOutFilter) {
                /* Don't mark readonly terms dirty */
                continue;
            }

            if (table_data->subjects[ti] != 0) {
                /* Don't mark table dirty if term is not from the table */
                continue;
            }

            int32_t index = table_data->columns[ti];
            if (index <= 0) {
                /* If term is not set, there's nothing to mark dirty */
                continue;
            }

            /* Potential candidate for marking table dirty, if a component */
            int32_t storage_index = ecs_table_type_to_storage_index(
                table, index - 1);
            if (storage_index >= 0) {
                table->dirty_state[storage_index + 1] ++;
            }
        }
    }
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

    if (ECS_BIT_IS_SET(it->flags, EcsIterNoResults)) {
        goto done;
    }

    ECS_BIT_SET(it->flags, EcsIterIsValid);

    ecs_query_iter_t *iter = &it->priv.iter.query;
    ecs_query_t *query = iter->query;
    ecs_world_t *world = query->world;
    ecs_flags32_t flags = query->flags;
    const ecs_filter_t *filter = &query->filter;
    bool only_this = filter->flags & EcsFilterMatchOnlyThis;

    ecs_poly_assert(world, ecs_world_t);
    (void)world;

    query_iter_cursor_t cur;
    ecs_query_table_node_t *node, *next, *prev;
    if ((prev = iter->prev)) {
        /* Match has been iterated, update monitor for change tracking */
        if (flags & EcsQueryHasMonitor) {
            flecs_query_sync_match_monitor(query, prev->match);
        }
        if (flags & EcsQueryHasOutColumns) {
            mark_columns_dirty(query, prev->match);
        }
    }

    iter->skip_count = 0;

    flecs_iter_validate(it);

    for (node = iter->node; node != NULL; node = next) {     
        ecs_query_table_match_t *match = node->match;
        ecs_table_t *table = match->table;

        next = node->next;

        if (table) {
            cur.first = node->offset;
            cur.count = node->count;
            if (!cur.count) {
                cur.count = ecs_table_count(table);

                /* List should never contain empty tables */
                ecs_assert(cur.count != 0, ECS_INTERNAL_ERROR, NULL);
            }

            ecs_vector_t *bitset_columns = match->bitset_columns;
            ecs_vector_t *sparse_columns = match->sparse_columns;

            if (bitset_columns || sparse_columns) {
                bool found = false;

                do {
                    found = false;

                    if (bitset_columns) {
                        if (bitset_column_next(table, bitset_columns, iter, 
                            &cur) == -1) 
                        {
                            /* No more enabled components for table */
                            iter->bitset_first = 0;
                            break;
                        } else {
                            found = true;
                            next = node;
                        }
                    }

                    if (sparse_columns) {
                        if (sparse_column_next(table, match,
                            sparse_columns, iter, &cur, found) == -1)
                        {
                            /* No more elements in sparse column */
                            if (found) {
                                /* Try again */
                                next = node->next;
                                found = false;
                            } else {
                                /* Nothing found */
                                iter->bitset_first = 0;
                                break;
                            }
                        } else {
                            found = true;
                            next = node;
                            iter->bitset_first = cur.first + cur.count;
                        }
                    }
                } while (!found);

                if (!found) {
                    continue;
                }
            }
        } else {
            cur.count = 0;
            cur.first = 0;
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
                if (term->subj.entity != EcsThis) {
                    continue;
                }
                
                int32_t actual_index = term->index;
                it->ids[actual_index] = match->ids[actual_index];
                it->columns[actual_index] = match->columns[actual_index];
                it->sizes[actual_index] = match->sizes[actual_index];
            }
        }

        it->subjects = match->subjects;
        it->references = match->references;
        it->instance_count = 0;

        flecs_iter_populate_data(world, it, match->table, cur.first, cur.count, 
            it->ptrs, NULL);

        iter->node = next;
        iter->prev = node;
        goto yield;
    }

done:
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

        ecs_query_table_match_t *qt = 
            (ecs_query_table_match_t*)it->priv.iter.query.prev;
        ecs_assert(qt != NULL, ECS_INVALID_PARAMETER, NULL);

        if (!query) {
            query = it->priv.iter.query.query;
        } else {
            ecs_check(query == it->priv.iter.query.query, 
                ECS_INVALID_PARAMETER, NULL);
        }

        ecs_check(query != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_poly_assert(query, ecs_query_t);

        flecs_process_pending_tables(it->real_world);

        return flecs_query_check_match_monitor(query, qt);
    }

    ecs_poly_assert(query, ecs_query_t);
    ecs_check(!(query->flags & EcsQueryIsOrphaned), 
        ECS_INVALID_PARAMETER, NULL);

    flecs_process_pending_tables(query->world);

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
    return ecs_filter_str(query->world, &query->filter);
}

int32_t ecs_query_table_count(
    const ecs_query_t *query)
{
    ecs_run_aperiodic(query->world, EcsAperiodicEmptyTables);
    return query->cache.tables.count;
}

int32_t ecs_query_empty_table_count(
    const ecs_query_t *query)
{
    ecs_run_aperiodic(query->world, EcsAperiodicEmptyTables);
    return query->cache.empty_tables.count;
}

int32_t ecs_query_entity_count(
    const ecs_query_t *query)
{
    ecs_run_aperiodic(query->world, EcsAperiodicEmptyTables);
    
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
