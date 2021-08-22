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
        e = ecs_get_alive(world, ECS_PAIR_RELATION(e));

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
        e = ECS_PAIR_OBJECT(pair);

        /* Because generations are not stored in the pair, get the currently
         * alive id */
        e = ecs_get_alive(world, e);

        /* If a pair is used with a not alive id, the pair is not valid */
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

        component = ecs_get(world, e, EcsComponent);
    }

    return component;
}

/* Count number of columns with data (excluding tags) */
static
int32_t data_column_count(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        /* Typically all components will be clustered together at the start of
         * the type as components are created from a separate id pool, and type
         * vectors are sorted. 
         * Explicitly check for EcsComponent and EcsName since the ecs_has check
         * doesn't work during bootstrap. */
        if ((component == ecs_id(EcsComponent)) || 
            (component == ecs_pair(ecs_id(EcsIdentifier), EcsName)) || 
            (component == ecs_pair(ecs_id(EcsIdentifier), EcsSymbol)) || 
            flecs_component_from_id(world, component) != NULL) 
        {
            count = c_ptr_i + 1;
        }
    });

    return count;
}

/* Ensure the ids used in the columns exist */
static
int32_t ensure_columns(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, PAIR)) {
            ecs_entity_t rel = ECS_PAIR_RELATION(component);
            ecs_entity_t obj = ECS_PAIR_OBJECT(component);
            ecs_ensure(world, rel);
            ecs_ensure(world, obj);
        } else if (component & ECS_ROLE_MASK) {
            ecs_entity_t e = ECS_PAIR_OBJECT(component);
            ecs_ensure(world, e);
        } else {
            ecs_ensure(world, component);
        }
    });

    return count;
}

/* Count number of switch columns */
static
int32_t switch_column_count(
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, SWITCH)) {
            if (!count) {
                table->sw_column_offset = c_ptr_i;
            }
            count ++;
        }
    });

    return count;
}

/* Count number of bitset columns */
static
int32_t bitset_column_count(
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, DISABLED)) {
            if (!count) {
                table->bs_column_offset = c_ptr_i;
            }
            count ++;
        }
    });

    return count;
}

static
ecs_type_t ids_to_type(
    ecs_ids_t *entities)
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
            table->flags |= EcsTableIsDisabled;
        }

        /* If table contains disabled entities, mark it as disabled */
        if (id == EcsDisabled) {
            table->flags |= EcsTableIsDisabled;
        }

        /* Does table have exclusive or columns */
        if (ECS_HAS_ROLE(id, XOR)) {
            table->flags |= EcsTableHasXor;
        }

        /* Does table have IsA relations */
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            table->flags |= EcsTableHasIsA;
        }

        /* Does table have switch columns */
        if (ECS_HAS_ROLE(id, SWITCH)) {
            table->flags |= EcsTableHasSwitch;
        }

        /* Does table support component disabling */
        if (ECS_HAS_ROLE(id, DISABLED)) {
            table->flags |= EcsTableHasDisabled;
        }   

        /* Does table have ChildOf relations */
        if (ECS_HAS_RELATION(id, EcsChildOf)) {
            ecs_entity_t obj = ecs_pair_object(world, id);
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
    ecs_table_t *table,
    ecs_ids_t *entities)
{
    table->type = ids_to_type(entities);
    table->c_info = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->monitors = NULL;
    table->on_set = NULL;
    table->on_set_all = NULL;
    table->on_set_override = NULL;
    table->un_set_all = NULL;
    table->alloc_count = 0;
    table->lock = 0;

    /* Ensure the component ids for the table exist */
    ensure_columns(world, table);

    table->queries = NULL;
    table->column_count = data_column_count(world, table);
    table->sw_column_count = switch_column_count(table);
    table->bs_column_count = bitset_column_count(table);

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
    ecs_ids_t *entities,
    flecs_hashmap_result_t table_elem)
{
    ecs_table_t *result = flecs_sparse_add(world->store.tables, ecs_table_t);
    result->id = flecs_sparse_last_id(world->store.tables);

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    init_table(world, result, entities);

#ifndef NDEBUG
    char *expr = ecs_type_str(world, result->type);
    ecs_trace_2("table #[green][%s]#[normal] created", expr);
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
void add_id_to_ids(
    ecs_type_t type,
    ecs_entity_t add,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_id_t *array = ecs_vector_first(type, ecs_id_t);    
    bool added = false;

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_id_t e = array[i];

        if (e >= add && !added) {
            if (e != add) {
                out->array[el ++] = add;
            }
            added = true;
        }
        
        out->array[el ++] = e;
        ecs_assert(el <= out->count, ECS_INTERNAL_ERROR, NULL);
    }

    if (!added) {
        out->array[el ++] = add;
    }

    out->count = el;
}

static
void remove_id_from_ids(
    ecs_type_t type,
    ecs_entity_t remove,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_id_t *array = ecs_vector_first(type, ecs_id_t);

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_id_t e = array[i];
        if (e != remove) {
            out->array[el ++] = e;
            ecs_assert(el <= count, ECS_INTERNAL_ERROR, NULL);
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
            ecs_type_t sw_type = sw_columns[i].type;
            if (ecs_type_has_id(world, sw_type, add, true)) {
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

            if (ecs_type_has_id(
                world, type_ptr->normalized, add, true)) 
            {
                return i;
            }
        }
    }

    /* If a table was not found, this is an invalid switch case */
    ecs_abort(ECS_TYPE_INVALID_CASE, NULL);

    return -1;
}

static
void diff_insert_removed(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_diff_t *diff,
    ecs_id_t id)
{
    diff->removed.array[diff->removed.count ++] = id;
    bool un_set = true;

    if (table->flags & EcsTableHasIsA) {
        /* If next table has a base and component is removed, check if
         * the removed component was an override. */
        if (ecs_type_match(world, table, table->type, 0, id, EcsIsA,
            1, 0, NULL) != -1)
        {
            /* If the component was an override, add it to on_set as
             * reexposing a base component is a change in value */
            if (!diff->on_set.array) {
                diff->on_set.array = ecs_os_malloc_n(
                    ecs_id_t, diff->removed.size);
                diff->on_set.size = diff->removed.size;
            }

            diff->on_set.array[diff->on_set.count ++] = id;

            /* Prevent component from being marked as unset */
            un_set = false;
        }
    }

    if (!un_set) {
        if (!diff->un_set.array) {
            diff->un_set.array = ecs_os_malloc_n(
                ecs_id_t, diff->removed.size);
            diff->un_set.size = diff->removed.size;
        }

        diff->un_set.array[diff->un_set.count ++] = id;
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
    ecs_type_t node_type = node->type;
    ecs_type_t next_type = next->type;

    ecs_id_t *ids_node = ecs_vector_first(node_type, ecs_id_t);
    ecs_id_t *ids_next = ecs_vector_first(next_type, ecs_id_t);
    int32_t i_node = 0, node_count = ecs_vector_count(node_type);
    int32_t i_next = 0, next_count = ecs_vector_count(next_type);
    int32_t added_count = 0;
    int32_t removed_count = 0;
    bool trivial_edge = true;

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

    trivial_edge &= (added_count + removed_count) <= 1;

    if (trivial_edge) {
        /* If edge is trivial there's no need to create a diff element for it */
        edge->diff_index = 0;
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
            diff->added.array[diff->added.count ++] = id_next;
        } else if (id_node < id_next) {
            diff_insert_removed(world, next, diff, id_node);
        }

        i_node += id_node <= id_next;
        i_next += id_next <= id_node;
    }

    for (; i_next < next_count; i_next ++) {
        ecs_id_t id_next = ids_next[i_next];
        diff->added.array[diff->added.count ++] = id_next;
    }
    for (; i_node < node_count; i_node ++) {
        ecs_id_t id_node = ids_node[i_next];
        diff_insert_removed(world, next, diff, id_node);
    }

    ecs_assert(diff->added.count == added_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(diff->removed.count == removed_count, ECS_INTERNAL_ERROR, NULL);
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
        int32_t count = ecs_vector_count(type);

        ecs_ids_t entities = {
            .array = ecs_os_alloca_n(ecs_id_t, count + 1),
            .count = count + 1
        };

        add_id_to_ids(type, id, &entities);

        return flecs_table_find_or_create(world, &entities);;
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
            .array = ecs_os_alloca_n(ecs_entity_t, count),
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
            ecs_entity_t base_of_base = ecs_pair_object(world, id);
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

    if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_RELATION(id) == EcsIsA) {
        ecs_entity_t base = ecs_pair_object(world, id);
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
        if (di) {
            *out = ecs_vector_first(table->node.diffs, ecs_table_diff_t)[di - 1];
        } else {
            if (add_ptr) {
                out->added.array = add_ptr;
                out->added.count = 1;
            } else {
                out->added.count = 0;
            }
            if (remove_ptr) {
                out->removed.array = remove_ptr;
                out->removed.count = 1;
                out->un_set.array = remove_ptr;
                out->un_set.count = 1;
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
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    node = node ? node : &world->store.root;

    /* Removing 0 from an entity is not valid */
    ecs_assert(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_id_t id = id_ptr[0];
    ecs_edge_t *edge = ensure_edge(&node->node.remove, id);
    ecs_table_t *next = edge->next;

    if (!next) {
        next = find_or_create_table_without(world, node, edge, id);
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->next != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(next, edge, NULL, id_ptr, diff);

    return next;
}

ecs_table_t* flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_id_t *id_ptr,
    ecs_table_diff_t *diff)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    node = node ? node : &world->store.root;

    /* Adding 0 to an entity is not valid */
    ecs_assert(id_ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(id_ptr[0] != 0, ECS_INVALID_PARAMETER, NULL);

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
}

static
bool ecs_entity_array_is_ordered(
    const ecs_ids_t *entities)
{
    ecs_entity_t prev = 0;
    ecs_entity_t *array = entities->array;
    int32_t i, count = entities->count;

    for (i = 0; i < count; i ++) {
        if (!array[i] && !prev) {
            continue;
        }
        if (array[i] <= prev) {
            return false;
        }
        prev = array[i];
    }    

    return true;
}

static
int32_t ecs_entity_array_dedup(
    ecs_entity_t *array,
    int32_t count)
{
    int32_t j, k;
    ecs_entity_t prev = array[0];

    for (k = j = 1; k < count; j ++, k++) {
        ecs_entity_t e = array[k];
        if (e == prev) {
            k ++;
        }

        array[j] = e;
        prev = e;
    }

    return count - (k - j);
}

static
ecs_table_t* find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *ids)
{    
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   

    /* Make sure array is ordered and does not contain duplicates */
    int32_t type_count = ids->count;
    ecs_id_t *ordered = NULL;

    if (!type_count) {
        return &world->store.root;
    }

    if (!ecs_entity_array_is_ordered(ids)) {
        ecs_size_t size = ECS_SIZEOF(ecs_entity_t) * type_count;
        ordered = ecs_os_alloca(size);
        ecs_os_memcpy(ordered, ids->array, size);
        qsort(ordered, (size_t)type_count, sizeof(ecs_entity_t), 
            flecs_entity_compare_qsort);
        type_count = ecs_entity_array_dedup(ordered, type_count);        
    } else {
        ordered = ids->array;
    }

    ecs_ids_t ordered_ids = {
        .array = ordered,
        .count = type_count
    };

    ecs_table_t *table;
    flecs_hashmap_result_t elem = flecs_hashmap_ensure(
        world->store.table_map, &ordered_ids, ecs_table_t*);
    if ((table = *(ecs_table_t**)elem.value)) {
        return table;
    }

    /* If we get here, table needs to be created which is only allowed when the
     * application is not currently in progress */
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

    /* If we get here, the table has not been found, so create it. */
    ecs_table_t *result = create_table(world, &ordered_ids, elem);
    
    ecs_assert(ordered_ids.count == ecs_vector_count(result->type), 
        ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   
    return find_or_create(world, components);
}

void flecs_init_root_table(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   

    ecs_ids_t entities = {
        .array = NULL,
        .count = 0
    };

    init_table(world, &world->store.root, &entities);
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

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
