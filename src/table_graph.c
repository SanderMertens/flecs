#include "private_api.h"

static
uint64_t ids_hash(const void *ptr) {
    const ecs_ids_t *type = ptr;
    ecs_id_t *ids = type->array;
    int32_t count = type->count;
    uint64_t hash = flecs_hash(ids, count * ECS_SIZEOF(ecs_id_t));
    return hash;
}

static
int ids_compare(const void *ptr_1, const void *ptr_2) {
    const ecs_ids_t *type_1 = ptr_1;
    const ecs_ids_t *type_2 = ptr_2;

    int32_t count_1 = type_1->count;
    int32_t count_2 = type_2->count;

    if (count_1 != count_2) {
        return (count_1 > count_2) - (count_1 < count_2);
    }

    const ecs_id_t *ids_1 = type_1->array;
    const ecs_id_t *ids_2 = type_2->array;
    
    int32_t i;
    for (i = 0; i < count_1; i ++) {
        ecs_id_t id_1 = ids_1[i];
        ecs_id_t id_2 = ids_2[i];

        if (id_1 != id_2) {
            return (id_1 > id_2) - (id_1 < id_2);
        }
    }

    return 0;
}

ecs_hashmap_t flecs_table_hashmap_new(void) {
    return flecs_hashmap_new(ecs_ids_t, ecs_table_t*, ids_hash, ids_compare);
}

const EcsComponent* flecs_component_from_id(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_entity_t pair = 0;

    /* If this is a pair, get the pair component from the identifier */
    if (ECS_HAS_ROLE(e, PAIR)) {
        pair = e;
        e = ecs_get_alive(world, ECS_PAIR_FIRST(e));

        if (ecs_has_id(world, e, EcsTag)) {
            return NULL;
        }
    }

    if (e & ECS_ROLE_MASK) {
        return NULL;
    }

    const EcsComponent *component = ecs_get(world, e, EcsComponent);
    if ((!component || !component->size) && pair) {
        /* If this is a pair column and the pair is not a component, use
         * the component type of the component the pair is applied to. */
        e = ECS_PAIR_SECOND(pair);

        /* Because generations are not stored in the pair, get the currently
         * alive id */
        e = ecs_get_alive(world, e);

        /* If a pair is used with a not alive id, the pair is not valid */
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

        component = ecs_get(world, e, EcsComponent);
    }

    return component;
}

/* Ensure the ids used in the columns exist */
static
int32_t ensure_columns(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t i, count = ecs_vector_count(table->type);
    ecs_id_t* ids = ecs_vector_first(table->type, ecs_id_t);

    for (i = 0; i < count; i++) {
        ecs_ensure_id(world, ids[i]);
    }

    return count;
}

static
ecs_vector_t* ids_to_vector(
    const ecs_ids_t *entities)
{
    if (entities->count) {
        ecs_vector_t *result = NULL;
        ecs_vector_set_count(&result, ecs_entity_t, entities->count);
        ecs_entity_t *array = ecs_vector_first(result, ecs_entity_t);
        ecs_os_memcpy_n(array, entities->array, ecs_entity_t, entities->count);
        return result;
    } else {
        return NULL;
    }
}

static
ecs_edge_t* get_edge(
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    if (id < ECS_HI_COMPONENT_ID) {
        if (!edges->lo) {
            return NULL;
        }
        return &edges->lo[id];
    } else {
        if (!edges->hi) {
            return NULL;
        }
        return ecs_map_get(edges->hi, ecs_edge_t, id);
    }
}

static
ecs_edge_t* ensure_edge(
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    if (id < ECS_HI_COMPONENT_ID) {
        if (!edges->lo) {
            edges->lo = ecs_os_calloc_n(ecs_edge_t, ECS_HI_COMPONENT_ID);
        }
        return &edges->lo[id];
    } else {
        if (!edges->hi) {
            edges->hi = ecs_map_new(ecs_edge_t, 1);
        }
        return ecs_map_ensure(edges->hi, ecs_edge_t, id);
    }
}

static
void init_edges(
    ecs_graph_edges_t *edges)
{
    edges->lo = NULL;
    edges->hi = NULL;
}

static
void init_node(
    ecs_graph_node_t *node)
{
    init_edges(&node->add);
    init_edges(&node->remove);
}

static
void init_flags(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    int32_t count = ecs_vector_count(table->type);
    
    /* Iterate components to initialize table flags */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        /* As we're iterating over the table components, also set the table
         * flags. These allow us to quickly determine if the table contains
         * data that needs to be handled in a special way, like prefabs or 
         * containers */
        if (id <= EcsLastInternalComponentId) {
            table->flags |= EcsTableHasBuiltins;
        }

        if (id == EcsModule) {
            table->flags |= EcsTableHasBuiltins;
            table->flags |= EcsTableHasModule;
        }

        if (id == EcsPrefab) {
            table->flags |= EcsTableIsPrefab;
        }

        /* If table contains disabled entities, mark it as disabled */
        if (id == EcsDisabled) {
            table->flags |= EcsTableIsDisabled;
        }

        /* Does table have exclusive or columns */
        if (ECS_HAS_ROLE(id, XOR)) {
            table->flags |= EcsTableHasXor;
        }

        /* Does the table have pairs */
        if (ECS_HAS_ROLE(id, PAIR)) {
            table->flags |= EcsTableHasPairs;
        }

        /* Does table have IsA relations */
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            table->flags |= EcsTableHasIsA;
        }

        /* Does table have ChildOf relations */
        if (ECS_HAS_RELATION(id, EcsChildOf)) {
            table->flags |= EcsTableHasChildOf;
        }

        /* Does table have switch columns */
        if (ECS_HAS_ROLE(id, SWITCH)) {
            table->flags |= EcsTableHasSwitch;
        }

        /* Does table support component disabling */
        if (ECS_HAS_ROLE(id, DISABLED)) {
            table->flags |= EcsTableHasDisabled;
        }

        if (ECS_HAS_RELATION(id, EcsChildOf)) {
            ecs_poly_assert(world, ecs_world_t);
            ecs_entity_t obj = ecs_pair_second(world, id);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            if (obj == EcsFlecs || obj == EcsFlecsCore || 
                ecs_has_id(world, obj, EcsModule)) 
            {
                /* If table contains entities that are inside one of the builtin
                 * modules, it contains builtin entities */
                table->flags |= EcsTableHasBuiltins;
                table->flags |= EcsTableHasModule;
            }
        }      
    }
}

static
void init_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    table->c_info = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->alloc_count = 0;
    table->lock = 0;
    table->refcount = 1;

    /* Ensure the component ids for the table exist */
    ensure_columns(world, table);

    init_node(&table->node);
    init_flags(world, table);

    flecs_register_table(world, table);
    flecs_table_init_data(world, table);

    /* Register component info flags for all columns */
    flecs_table_notify(world, table, &(ecs_table_event_t){
        .kind = EcsTableComponentInfo
    });    
}

static
ecs_table_t *create_table(
    ecs_world_t *world,
    ecs_vector_t *type,
    flecs_hashmap_result_t table_elem)
{
    ecs_table_t *result = flecs_sparse_add(world->store.tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_reclaim(&type, ecs_id_t);

    result->id = flecs_sparse_last_id(world->store.tables);
    result->type = type;

    init_table(world, result);

#ifndef NDEBUG
    char *expr = ecs_type_str(world, result->type);
    ecs_dbg_2("#[green]table#[normal] [%s] created with id %d", expr, result->id);
    ecs_os_free(expr);
#endif
    ecs_log_push();

    /* Store table in table hashmap */
    *(ecs_table_t**)table_elem.value = result;

    /* Set keyvalue to one that has the same lifecycle as the table */
    ecs_ids_t key = {
        .array = ecs_vector_first(result->type, ecs_id_t),
        .count = ecs_vector_count(result->type)
    };
    *(ecs_ids_t*)table_elem.key = key;

    flecs_notify_queries(world, &(ecs_query_event_t) {
        .kind = EcsQueryTableMatch,
        .table = result
    });

    ecs_log_pop();

    return result;
}

static
ecs_table_t* find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *ids,
    ecs_vector_t *type)
{    
    ecs_poly_assert(world, ecs_world_t);   

    /* Make sure array is ordered and does not contain duplicates */
    int32_t id_count = ids->count;

    if (!id_count) {
        return &world->store.root;
    }

    ecs_table_t *table;
    flecs_hashmap_result_t elem = flecs_hashmap_ensure(
        world->store.table_map, ids, ecs_table_t*);
    if ((table = *(ecs_table_t**)elem.value)) {
        if (type) {
            ecs_vector_free(type);
        }
        return table;
    }

    if (!type) {
        type = ids_to_vector(ids);
    }

    /* If we get here, table needs to be created which is only allowed when the
     * application is not currently in progress */
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

    /* If we get here, the table has not been found, so create it. */
    return create_table(world, type, elem);
}

static
void add_id_to_ids(
    ecs_vector_t **idv,
    ecs_entity_t add,
    ecs_entity_t r_exclusive)
{
    int32_t i, count = ecs_vector_count(idv[0]);
    ecs_id_t *array = ecs_vector_first(idv[0], ecs_id_t);

    for (i = 0; i < count; i ++) {
        ecs_id_t e = array[i];

        if (e == add) {
            return;
        }

        if (r_exclusive && ECS_HAS_ROLE(e, PAIR)) {
            if (ECS_PAIR_FIRST(e) == r_exclusive) {
                array[i] = add; /* Replace */
                return;
            }
        }

        if (e >= add) {
            if (e != add) {
                ecs_id_t *ptr = ecs_vector_insert_at(idv, ecs_id_t, i);
                ptr[0] = add;
                return;
            }
        }
    }

    ecs_id_t *ptr = ecs_vector_add(idv, ecs_id_t);
    ptr[0] = add;
}

static
void remove_id_from_ids(
    ecs_type_t type,
    ecs_id_t remove,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_id_t *array = ecs_vector_first(type, ecs_id_t);
    int32_t i, el = 0;

    if (ecs_id_is_wildcard(remove)) {
        for (i = 0; i < count; i ++) {
            ecs_id_t id = array[i];
            if (!ecs_id_match(id, remove)) {
                out->array[el ++] = id;
                ecs_assert(el <= count, ECS_INTERNAL_ERROR, NULL);
            }
        }
    } else {
        for (i = 0; i < count; i ++) {
            ecs_id_t id = array[i];
            if (id != remove) {
                out->array[el ++] = id;
                ecs_assert(el <= count, ECS_INTERNAL_ERROR, NULL);
            }
        }
    }

    out->count = el;
}

int32_t flecs_table_switch_from_case(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_entity_t add)
{
    ecs_type_t type = table->type;
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    int32_t i, count = table->sw_column_count;
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    add = add & ECS_COMPONENT_MASK;

    ecs_sw_column_t *sw_columns = NULL;

    if ((sw_columns = table->storage.sw_columns)) {
        /* Fast path, we can get the switch type from the column data */
        for (i = 0; i < count; i ++) {
            ecs_table_t *sw_type = sw_columns[i].type;
            if (ecs_search(world, sw_type, add, 0) != -1) {
                return i;
            }
        }
    } else {
        /* Slow path, table is empty, so we'll have to get the switch types by
         * actually inspecting the switch type entities. */
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = array[i + table->sw_column_offset];
            ecs_assert(ECS_HAS_ROLE(e, SWITCH), ECS_INTERNAL_ERROR, NULL);
            e = e & ECS_COMPONENT_MASK;

            const EcsType *type_ptr = ecs_get(world, e, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

            if (ecs_search(world, type_ptr->normalized, add, 0) != -1) {
                return i;
            }
        }
    }

    /* If a table was not found, this is an invalid switch case */
    ecs_abort(ECS_TYPE_INVALID_CASE, NULL);

    return -1;
}

static
void ids_append(
    ecs_ids_t *ids,
    ecs_id_t id)
{
    ids->array = ecs_os_realloc_n(ids->array, ecs_id_t, ids->count + 1);
    ids->array[ids->count ++] = id;   
}

static
void diff_insert_isa(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *base_diff,
    ecs_ids_t *append_to,
    ecs_ids_t *append_from,
    ecs_id_t add)
{
    ecs_entity_t base = ecs_pair_second(world, add);
    ecs_table_t *base_table = ecs_get_table(world, base);
    if (!base_table) {
        return;
    }

    ecs_type_t base_type = base_table->type, type = table->type;
    ecs_table_t *table_wo_base = base_table;

    /* If the table does not have a component from the base, it should
     * trigger an OnSet */
    ecs_id_t *ids = ecs_vector_first(base_type, ecs_id_t);
    int32_t j, i, count = ecs_vector_count(base_type);
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            /* The base has an IsA relation. Find table without the base, which
             * gives us the list of ids the current base inherits and doesn't
             * override. This saves us from having to recursively check for each
             * base in the hierarchy whether the component is overridden. */
            table_wo_base = flecs_table_traverse_remove(
                world, table_wo_base, &id, base_diff);

            /* Because we removed, the ids are stored in un_set vs. on_set */
            for (j = 0; j < append_from->count; j ++) {
                ecs_id_t base_id = append_from->array[j];
                /* We still have to make sure the id isn't overridden by the
                 * current table */
                if (!type || ecs_search(world, table, base_id, NULL) == -1) {
                    ids_append(append_to, base_id);
                }
            }

            continue;
        }

        /* Identifiers are not inherited */
        if (ECS_HAS_RELATION(id, ecs_id(EcsIdentifier))) {
            continue;
        }

        if (!ecs_get_typeid(world, id)) {
            continue;
        }

        if (!type || ecs_search(world, table, id, NULL) == -1) {
            ids_append(append_to, id);
        }
    }
}

static
void diff_insert_added_isa(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    ecs_table_diff_t base_diff;
    diff_insert_isa(world, table, &base_diff, &diff->on_set, 
        &base_diff.un_set, id);
}

static
void diff_insert_removed_isa(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    ecs_table_diff_t base_diff;
    diff_insert_isa(world, table, &base_diff, &diff->un_set, 
        &base_diff.un_set, id);
}

static
void diff_insert_added(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    diff->added.array[diff->added.count ++] = id;

    if (ECS_HAS_RELATION(id, EcsIsA)) {
        diff_insert_added_isa(world, table, diff, id);
    }
}

static
void diff_insert_removed(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    diff->removed.array[diff->removed.count ++] = id;

    if (ECS_HAS_RELATION(id, EcsIsA)) {
        /* Removing an IsA relation also "removes" all components from the
         * instance. Any id from base that's not overridden should be UnSet. */
        diff_insert_removed_isa(world, table, diff, id);
        return;
    }

    if (table->flags & EcsTableHasIsA) {
        if (!ecs_get_typeid(world, id)) {
            /* Do nothing if id is not a component */
            return;
        }

        /* If next table has a base and component is removed, check if
         * the removed component was an override. Removed overrides reexpose the
         * base component, thus "changing" the value which requires an OnSet. */
        if (ecs_search_relation(world, table, 0, id, EcsIsA,
            1, -1, NULL, NULL, NULL) != -1)
        {
            ids_append(&diff->on_set, id);
            return;
        }
    }

    if (ecs_get_typeid(world, id) != 0) {
        ids_append(&diff->un_set, id);
    }
}

static
void compute_table_diff(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_table_t *next,
    ecs_edge_t *edge,
    ecs_id_t id)
{
    if (node == next) {
        return;
    }

    ecs_type_t node_type = node->type;
    ecs_type_t next_type = next->type;

    ecs_id_t *ids_node = ecs_vector_first(node_type, ecs_id_t);
    ecs_id_t *ids_next = ecs_vector_first(next_type, ecs_id_t);
    int32_t i_node = 0, node_count = ecs_vector_count(node_type);
    int32_t i_next = 0, next_count = ecs_vector_count(next_type);
    int32_t added_count = 0;
    int32_t removed_count = 0;
    bool trivial_edge = !ECS_HAS_RELATION(id, EcsIsA) && 
        !(node->flags & EcsTableHasIsA) && !(next->flags & EcsTableHasIsA);

    /* First do a scan to see how big the diff is, so we don't have to realloc
     * or alloc more memory than required. */
    for (; i_node < node_count && i_next < next_count; ) {
        ecs_id_t id_node = ids_node[i_node];
        ecs_id_t id_next = ids_next[i_next];

        bool added = id_next < id_node;
        bool removed = id_node < id_next;

        trivial_edge &= !added || id_next == id;
        trivial_edge &= !removed || id_node == id;

        added_count += added;
        removed_count += removed;

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    added_count += next_count - i_next;
    removed_count += node_count - i_node;

    trivial_edge &= (added_count + removed_count) <= 1 && 
        !ecs_id_is_wildcard(id);

    if (trivial_edge) {
        /* If edge is trivial there's no need to create a diff element for it.
         * Encode in the id whether the id is a tag or not, so that wen can
         * still tell whether an UnSet handler should be called or not. */
        edge->diff_index = -1 * (ecs_get_typeid(world, id) == 0);
        return;
    }

    ecs_table_diff_t *diff = ecs_vector_add(&node->node.diffs, ecs_table_diff_t);
    ecs_os_memset_t(diff, 0, ecs_table_diff_t);
    edge->diff_index = ecs_vector_count(node->node.diffs);
    if (added_count) {
        diff->added.array = ecs_os_malloc_n(ecs_id_t, added_count);
        diff->added.count = 0;
        diff->added.size = added_count;
    }
    if (removed_count) {
        diff->removed.array = ecs_os_malloc_n(ecs_id_t, removed_count);
        diff->removed.count = 0;
        diff->removed.size = removed_count;
    }

    for (i_node = 0, i_next = 0; i_node < node_count && i_next < next_count; ) {
        ecs_id_t id_node = ids_node[i_node];
        ecs_id_t id_next = ids_next[i_next];

        if (id_next < id_node) {
            diff_insert_added(world, node, diff, id_next);
        } else if (id_node < id_next) {
            diff_insert_removed(world, next, diff, id_node);
        }

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    for (; i_next < next_count; i_next ++) {
        diff_insert_added(world, node, diff, ids_next[i_next]);
    }
    for (; i_node < node_count; i_node ++) {
        diff_insert_removed(world, next, diff, ids_node[i_node]);
    }

    ecs_assert(diff->added.count == added_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(diff->removed.count == removed_count, ECS_INTERNAL_ERROR, NULL);
}

static
void add_with_ids_to_ids(
    ecs_world_t *world,
    ecs_vector_t **idv,
    ecs_entity_t r,
    ecs_entity_t o)
{
    /* Check if component/relation has With pairs, which contain ids
     * that need to be added to the table. */
    ecs_table_t *id_table = ecs_get_table(world, r);
    if (!id_table) {
        return;
    }
    
    ecs_table_record_t *tr = flecs_get_table_record(world, id_table, 
        ecs_pair(EcsWith, EcsWildcard));
    if (tr) {
        int32_t i, with_count = tr->count;
        int32_t start = tr->column;
        int32_t end = start + with_count;
        ecs_id_t *id_ids = ecs_vector_first(id_table->type, ecs_id_t);

        for (i = start; i < end; i ++) {
            ecs_assert(ECS_PAIR_FIRST(id_ids[i]) == EcsWith, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_id_t id_r = ECS_PAIR_SECOND(id_ids[i]);
            ecs_id_t id = id_r;
            if (o) {
                id = ecs_pair(id_r, o);
            }

            /* Always make sure vector has room for one more */
            add_id_to_ids(idv, id, 0);

            /* Add recursively in case id also has With pairs */
            add_with_ids_to_ids(world, idv, id_r, o);
        }
    }
}

static
ecs_table_t* find_or_create_table_with_id(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t id)
{
    /* If table has one or more switches and this is a case, return self */
    if (ECS_HAS_ROLE(id, CASE)) {
        ecs_assert((node->flags & EcsTableHasSwitch) != 0, 
            ECS_TYPE_INVALID_CASE, NULL);
        return node;
    } else {
        ecs_type_t type = node->type;
        ecs_entity_t r_exclusive = 0;
        ecs_entity_t r = 0, o = 0, re = 0;

        if (ECS_HAS_ROLE(id, PAIR)) {
            r = ECS_PAIR_FIRST(id);
            o = ECS_PAIR_SECOND(id);
            re = ecs_get_alive(world, r);
            if (re && ecs_has_id(world, re, EcsExclusive)) {
                r_exclusive = (uint32_t)re;
            }
        } else {
            r = id & ECS_COMPONENT_MASK;
            re = ecs_get_alive(world, r);
        }

        ecs_vector_t *idv = ecs_vector_copy(type, ecs_id_t);
        add_id_to_ids(&idv, id, r_exclusive);
        if (re) {
            add_with_ids_to_ids(world, &idv, re, o);
        }

        ecs_ids_t ids = {
            .array = ecs_vector_first(idv, ecs_id_t),
            .count = ecs_vector_count(idv)
        };

        return find_or_create(world, &ids, idv);
    }
}

static
ecs_table_t* find_or_create_table_without_id(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t id)
{
    /* If table has one or more switches and this is a case, return self */
    if (ECS_HAS_ROLE(id, CASE)) {
        ecs_assert((node->flags & EcsTableHasSwitch) != 0, 
            ECS_TYPE_INVALID_CASE, NULL);
        return node;
    } else {
        ecs_type_t type = node->type;
        int32_t count = ecs_vector_count(type);

        ecs_ids_t ids = {
            .array = ecs_os_alloca_n(ecs_id_t, count),
            .count = count
        };

        remove_id_from_ids(type, id, &ids);

        return flecs_table_find_or_create(world, &ids);;
    }
}

static
ecs_table_t* find_or_create_table_with_isa(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t base)
{
    ecs_type_t base_type = ecs_get_type(world, base);
    ecs_id_t *ids = ecs_vector_first(base_type, ecs_id_t);
    int32_t i, count = ecs_vector_count(base_type);

    /* Start from back, as roles have high ids */
    for (i = count - 1; i >= 0; i --) {
        ecs_id_t id = ids[i];
        if (!(id & ECS_ROLE_MASK)) { /* early out if we found everything */
            break;
        }

        if (ECS_HAS_RELATION(id, EcsIsA)) {
            ecs_entity_t base_of_base = ecs_pair_second(world, id);
            node = find_or_create_table_with_isa(world, node, base_of_base);
        }

        if (ECS_HAS_ROLE(id, OVERRIDE)) {
            /* Override found, add it to table */
            id &= ECS_COMPONENT_MASK;
            node = flecs_table_traverse_add(world, node, &id, NULL);
        }
    }

    return node;
}

static
ecs_table_t* find_or_create_table_without(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_edge_t *edge,
    ecs_id_t id)
{
    ecs_table_t *next = find_or_create_table_without_id(world, node, id);

    edge->next = next;

    compute_table_diff(world, node, next, edge, id);

    if (node != next) {
        flecs_register_remove_ref(world, node, id);
    }

    return next;   
}

static
ecs_table_t* find_or_create_table_with(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_edge_t *edge,
    ecs_id_t id)
{
    ecs_table_t *next = find_or_create_table_with_id(world, node, id);

    if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_FIRST(id) == EcsIsA) {
        ecs_entity_t base = ecs_pair_second(world, id);
        next = find_or_create_table_with_isa(world, next, base);
    }

    edge->next = next;

    compute_table_diff(world, node, next, edge, id);

    if (node != next) {
        flecs_register_add_ref(world, node, id);
    }

    return next;
}

static
void populate_diff(
    ecs_table_t *table, 
    ecs_edge_t *edge,
    ecs_id_t *add_ptr,
    ecs_id_t *remove_ptr,
    ecs_table_diff_t *out)
{
    if (out) {
        int32_t di = edge->diff_index;
        if (di > 0) {
            ecs_assert(!add_ptr || !ECS_HAS_ROLE(add_ptr[0], CASE), 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(!remove_ptr || !ECS_HAS_ROLE(remove_ptr[0], CASE), 
                ECS_INTERNAL_ERROR, NULL);
            *out = ecs_vector_first(table->node.diffs, ecs_table_diff_t)[di - 1];
        } else {
            out->on_set.count = 0;

            if (add_ptr) {
                out->added.array = add_ptr;
                out->added.count = 1;
            } else {
                out->added.count = 0;
            }

            if (remove_ptr) {
                out->removed.array = remove_ptr;
                out->removed.count = 1;
                if (di == 0) {
                    out->un_set.array = remove_ptr;
                    out->un_set.count = 1;
                } else {
                    out->un_set.count = 0;
                }
            } else {
                out->removed.count = 0;
                out->un_set.count = 0;
            }
        }
    }
}

ecs_table_t* flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    ecs_poly_assert(world, ecs_world_t);

    node = node ? node : &world->store.root;

    /* Removing 0 from an entity is not valid */
    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_edge_t *edge = ensure_edge(&node->node.remove, id);
    ecs_table_t *next = edge->next;

    if (!next) {
        next = find_or_create_table_without(world, node, edge, id);
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->next != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(node, edge, NULL, id_ptr, diff);

    return next;
error:
    return NULL;
}

ecs_table_t* flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    ecs_poly_assert(world, ecs_world_t);

    node = node ? node : &world->store.root;

    /* Adding 0 to an entity is not valid */
    ecs_check(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_edge_t *edge = ensure_edge(&node->node.add, id);
    ecs_table_t *next = edge->next;

    if (!next) {
        next = find_or_create_table_with(world, node, edge, id);
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->next != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(node, edge, id_ptr, NULL, diff);

    return next;
error:
    return NULL;
}

ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *ids)
{
    ecs_poly_assert(world, ecs_world_t);
    return find_or_create(world, ids, NULL);
}

void flecs_init_root_table(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);   

    ecs_ids_t entities = {
        .array = NULL,
        .count = 0
    };

    world->store.root.type = ids_to_vector(&entities);
    init_table(world, &world->store.root);

    /* Ensure table indices start at 1, as 0 is reserved for the root */
    uint64_t new_id = flecs_sparse_new_id(world->store.tables);
    ecs_assert(new_id == 0, ECS_INTERNAL_ERROR, NULL);
    (void)new_id;
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_poly_assert(world, ecs_world_t);

    int32_t i;
    ecs_graph_node_t *node = &table->node;

    ecs_os_free(node->add.lo);
    ecs_os_free(node->remove.lo);
    ecs_map_free(node->add.hi);
    ecs_map_free(node->remove.hi);
    node->add.lo = NULL;
    node->remove.lo = NULL;
    node->add.hi = NULL;
    node->remove.hi = NULL;

    int32_t count = ecs_vector_count(node->diffs);
    ecs_table_diff_t *diffs = ecs_vector_first(node->diffs, ecs_table_diff_t);
    for (i = 0; i < count; i ++) {
        ecs_table_diff_t *diff = &diffs[i];
        ecs_os_free(diff->added.array);
        ecs_os_free(diff->removed.array);
        ecs_os_free(diff->on_set.array);
        ecs_os_free(diff->un_set.array);
    }

    ecs_vector_free(node->diffs);
    node->diffs = NULL;
}

void flecs_table_clear_add_edge(
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_edge_t *edge = get_edge(&table->node.add, id);
    if (edge) {
        edge->next = NULL;
        edge->diff_index = 0;
    }
}

void flecs_table_clear_remove_edge(
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_edge_t *edge = get_edge(&table->node.remove, id);
    if (edge) {
        edge->next = NULL;
        edge->diff_index = 0;
    }
}

/* Public convenience functions for traversing table graph */
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    return flecs_table_traverse_add(world, table, &id, NULL);
}

ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    return flecs_table_traverse_remove(world, table, &id, NULL);
}
