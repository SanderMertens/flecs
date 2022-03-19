#include "private_api.h"
#include <stdio.h>

/* Marker object used to differentiate a component vs. a tag edge */
static ecs_table_diff_t ecs_table_edge_is_component;

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

void flecs_table_hashmap_init(ecs_hashmap_t *hm) {
    flecs_hashmap_init(hm, ecs_ids_t, ecs_table_t*, ids_hash, ids_compare);
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
void table_diff_free(
    ecs_table_diff_t *diff) 
{
    ecs_os_free(diff->added.array);
    ecs_os_free(diff->removed.array);
    ecs_os_free(diff->on_set.array);
    ecs_os_free(diff->un_set.array);
    ecs_os_free(diff);
}

static
ecs_graph_edge_t* graph_edge_new(
    ecs_world_t *world)
{
    ecs_graph_edge_t *result = (ecs_graph_edge_t*)world->store.first_free;
    if (result) {
        world->store.first_free = result->hdr.next;
        ecs_os_zeromem(result);
    } else {
        result = ecs_os_calloc_t(ecs_graph_edge_t);
    }
    return result;
}

static
void graph_edge_free(
    ecs_world_t *world,
    ecs_graph_edge_t *edge)
{
    if (world->is_fini) {
        ecs_os_free(edge);
    } else {
        edge->hdr.next = world->store.first_free;
        world->store.first_free = &edge->hdr;
    }
}

static
ecs_graph_edge_t* ensure_hi_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    if (!ecs_map_is_initialized(&edges->hi)) {
        ecs_map_init(&edges->hi, ecs_graph_edge_t*, 1);
    }

    ecs_graph_edge_t **ep = ecs_map_ensure(&edges->hi, ecs_graph_edge_t*, id);
    ecs_graph_edge_t *edge = ep[0];
    if (edge) {
        return edge;
    }

    if (id < ECS_HI_COMPONENT_ID) {
        edge = &edges->lo[id];
    } else {
        edge = graph_edge_new(world);
    }

    ep[0] = edge;
    return edge;
}

static
ecs_graph_edge_t* ensure_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id)
{
    ecs_graph_edge_t *edge;
    
    if (id < ECS_HI_COMPONENT_ID) {
        if (!edges->lo) {
            edges->lo = ecs_os_calloc_n(ecs_graph_edge_t, ECS_HI_COMPONENT_ID);
        }
        edge = &edges->lo[id];
    } else {
        if (!ecs_map_is_initialized(&edges->hi)) {
            ecs_map_init(&edges->hi, ecs_graph_edge_t*, 1);
        }
        edge = ensure_hi_edge(world, edges, id);
    }

    return edge;
}

static
void disconnect_edge(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->id == id, ECS_INTERNAL_ERROR, NULL);
    (void)id;

    /* Remove backref from destination table */
    ecs_graph_edge_hdr_t *next = edge->hdr.next;
    ecs_graph_edge_hdr_t *prev = edge->hdr.prev;

    if (next) {
        next->prev = prev;
    }
    if (prev) {
        prev->next = next;
    }

    /* Remove data associated with edge */
    ecs_table_diff_t *diff = edge->diff;
    if (diff && diff != &ecs_table_edge_is_component) {
        table_diff_free(diff);
    }

    /* If edge id is low, clear it from fast lookup array */
    if (id < ECS_HI_COMPONENT_ID) {
        edge->from = NULL;
    } else {
        graph_edge_free(world, edge);
    }
}

static
void remove_edge(
    ecs_world_t *world,
    ecs_graph_edges_t *edges,
    ecs_id_t id,
    ecs_graph_edge_t *edge)
{
    ecs_assert(edges != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_map_is_initialized(&edges->hi), ECS_INTERNAL_ERROR, NULL);
    disconnect_edge(world, id, edge);
    ecs_map_remove(&edges->hi, id);
}

static
void init_edges(
    ecs_graph_edges_t *edges)
{
    edges->lo = NULL;
    ecs_os_zeromem(&edges->hi);
}

static
void init_node(
    ecs_graph_node_t *node)
{
    init_edges(&node->add);
    init_edges(&node->remove);
}

typedef struct {
    int32_t first;
    int32_t count;
} id_first_count_t;

static
void set_trigger_flags_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    /* Set flags if triggers are registered for table */
    if (flecs_check_triggers_for_event(world, id, EcsOnAdd)) {
        table->flags |= EcsTableHasOnAdd;
    }
    if (flecs_check_triggers_for_event(world, id, EcsOnRemove)) {
        table->flags |= EcsTableHasOnRemove;
    }
    if (flecs_check_triggers_for_event(world, id, EcsOnSet)) {
        table->flags |= EcsTableHasOnSet;
    }
    if (flecs_check_triggers_for_event(world, id, EcsUnSet)) {
        table->flags |= EcsTableHasUnSet;
    }
}

static
void register_table_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column,
    int32_t count,
    ecs_table_record_t *tr)
{
    flecs_register_for_id_record(world, id, table, tr);
    tr->column = column;
    tr->count = count;
    tr->id = id;
    set_trigger_flags_for_id(world, table, id);
    ecs_assert(tr->hdr.table == table, ECS_INTERNAL_ERROR, NULL);
}

static
void flecs_table_records_register(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    int32_t count = ecs_vector_count(table->type);

    if (!count) {
        return;
    }

    /* Count number of unique ids, pairs, relations and objects so we can figure
     * out how many table records are needed for this table. */
    int32_t id_count = 0, pair_count = 0, type_flag_count = 0;
    int32_t first_id = -1, first_pair = -1;
    ecs_map_t relations = ECS_MAP_INIT(0), objects = ECS_MAP_INIT(0);
    bool has_childof = false;

    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        ecs_entity_t rel = 0, obj = 0;

        if (ECS_HAS_ROLE(id, PAIR)) {
            id_first_count_t *r;

            rel = ECS_PAIR_FIRST(id);
            obj = ECS_PAIR_SECOND(id);

            if (0 == pair_count ++) {
                first_pair = i;
            }

            if (rel == EcsChildOf) {
                has_childof = true;
            }

            if (!ecs_map_is_initialized(&relations)) {
                ecs_map_init(&relations, id_first_count_t, count);
                ecs_map_init(&objects, id_first_count_t, count);
            }

            r = ecs_map_ensure(&relations, id_first_count_t, rel);
            if ((++r->count) == 1) {
                r->first = i;
            }

            r = ecs_map_ensure(&objects, id_first_count_t, obj);
            if ((++r->count) == 1) {
                r->first = i;
            }
        } else {
            rel = id & ECS_COMPONENT_MASK;
            if (rel != id) {
                type_flag_count ++;
            }

            if (0 == id_count ++) {
                first_id = i;
            }
        }
    }

    int32_t record_count = count + type_flag_count + (id_count != 0) + 
        (pair_count != 0) + ecs_map_count(&relations) + ecs_map_count(&objects) 
            + 1 /* for any */;

    if (!has_childof) {
        record_count ++;
    }

    table->records = ecs_os_calloc_n(ecs_table_record_t, record_count);
    table->record_count = record_count;

    /* First initialize records for regular (non-wildcard) ids. Make sure that
     * these table records line up with ids in table type. */
    int32_t first_role_id = -1;
    for (i = 0; i < count; i ++) {
        register_table_for_id(world, table, ids[i], i, 1, &table->records[i]);
        if (first_role_id == -1) {
            ecs_entity_t role = ids[i] & ECS_ROLE_MASK;
            if (role && role != ECS_PAIR) {
                first_role_id = i;
            }
        }
    }

    /* Initialize records for ids with roles */
    int32_t r = i;
    if (first_role_id != -1) {
        for (i = first_role_id; i < count; i ++) {
            ecs_id_t id = ids[i];
            ecs_entity_t role = id & ECS_ROLE_MASK;
            if (role && role != ECS_PAIR) {
                id &= ECS_COMPONENT_MASK;
                id = ecs_pair(id, EcsWildcard);
                register_table_for_id(world, table, id, i, 1, &table->records[r]);
                r ++;

                /* Keep track of how many switch/bitset columns there are */
                if (role == ECS_SWITCH) {
                    if (!table->sw_column_count) {
                        table->sw_column_offset = flecs_ito(int16_t, i);
                    }
                    table->sw_column_count ++;
                } else if (role == ECS_DISABLED) {
                    if (!table->bs_column_count) {
                        table->bs_column_offset = flecs_ito(int16_t, i);
                    }
                    table->bs_column_count ++;
                }
            }
        }
    }

    /* Initialize records for relation wildcards */
    ecs_map_iter_t mit = ecs_map_iter(&relations);
    id_first_count_t *elem; 
    uint64_t key;
    while ((elem = ecs_map_next(&mit, id_first_count_t, &key))) {
        ecs_id_t id = ecs_pair(key, EcsWildcard);
        register_table_for_id(world, table, id, elem->first, elem->count,
            &table->records[r]);
        r ++;
    }

    /* Initialize records for object wildcards */
    mit = ecs_map_iter(&objects);
    while ((elem = ecs_map_next(&mit, id_first_count_t, &key))) {
        ecs_id_t id = ecs_pair(EcsWildcard, key);
        register_table_for_id(world, table, id, elem->first, elem->count,
            &table->records[r]);
        r ++;
    }

    /* Initialize records for all wildcards ids */
    if (id_count) {
        register_table_for_id(world, table, EcsWildcard, 
            first_id, id_count, &table->records[r]);
        r ++;
    }
    if (pair_count) {
        register_table_for_id(world, table, ecs_pair(EcsWildcard, EcsWildcard), 
            first_pair, pair_count, &table->records[r]);
        r ++;
    }
    if (count) {
        register_table_for_id(world, table, EcsAny, 0, 1, &table->records[r]);
        r ++;
    }

    /* Insert into (ChildOf, 0) (root) if table doesn't have childof */
    if (!has_childof && count) {
        register_table_for_id(world, table, ecs_pair(EcsChildOf, 0), 
            0, 1, &table->records[r]);
    }

    ecs_map_fini(&relations);
    ecs_map_fini(&objects);
}

void flecs_table_records_unregister(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t i, count = table->record_count;
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &table->records[i];
        ecs_table_cache_t *cache = tr->hdr.cache;
        ecs_id_t id = tr->id;

        ecs_assert(tr->hdr.cache == cache, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(tr->hdr.table == table, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(flecs_get_id_record(world, id) == (ecs_id_record_t*)cache,
            ECS_INTERNAL_ERROR, NULL);

        ecs_table_cache_remove(cache, table, &tr->hdr);

        if (ecs_table_cache_is_empty(cache)) {
            ecs_id_record_t *idr = (ecs_id_record_t*)cache;
            flecs_remove_id_record(world, id, idr);
        }
    }
    
    ecs_os_free(table->records);
}

bool flecs_table_records_update_empty(
    ecs_table_t *table)
{
    bool result = false;
    bool is_empty = ecs_table_count(table) == 0;

    int32_t i, count = table->record_count;
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &table->records[i];
        ecs_table_cache_t *cache = tr->hdr.cache;
        result |= ecs_table_cache_set_empty(cache, table, is_empty);
    }

    return result;
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
    table->type_info = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->alloc_count = 0;
    table->lock = 0;
    table->refcount = 1;

    /* Ensure the component ids for the table exist */
    ensure_columns(world, table);

    init_node(&table->node);
    init_flags(world, table);
    flecs_table_records_register(world, table);
    flecs_table_init_data(world, table); 
}

static
ecs_table_t *create_table(
    ecs_world_t *world,
    ecs_vector_t *type,
    flecs_hashmap_result_t table_elem)
{
    ecs_table_t *result = flecs_sparse_add(&world->store.tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_reclaim(&type, ecs_id_t);

    result->id = flecs_sparse_last_id(&world->store.tables);
    result->type = type;

    init_table(world, result);

    if (ecs_should_log_2()) {
        char *expr = ecs_type_str(world, result->type);
        ecs_dbg_2(
            "#[green]table#[normal] [%s] #[green]created#[normal] with id %d", 
            expr, result->id);
        ecs_os_free(expr);
    }

    ecs_log_push_2();

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

    ecs_log_pop_2();

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
        &world->store.table_map, ids, ecs_table_t*);
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
    ecs_graph_edge_t *edge,
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
         * Store whether the id is a tag or not, so that we can still tell 
         * whether an UnSet handler should be called or not. */
        if (node->storage_table != next->storage_table) {
            edge->diff = &ecs_table_edge_is_component;
        }
        return;
    }

    ecs_table_diff_t *diff = ecs_os_calloc_t(ecs_table_diff_t);
    edge->diff = diff;
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
void init_edge(
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->id == 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->hdr.next == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(edge->hdr.prev == NULL, ECS_INTERNAL_ERROR, NULL);
    
    edge->from = table;
    edge->to = to;
    edge->id = id;
}

static
void init_add_edge(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    init_edge(table, edge, id, to);

    ensure_hi_edge(world, &table->node.add, id);

    if (table != to) {
        /* Add edges are appended to refs.next */
        ecs_graph_edge_hdr_t *to_refs = &to->node.refs;
        ecs_graph_edge_hdr_t *next = to_refs->next;
        
        to_refs->next = &edge->hdr;
        edge->hdr.prev = to_refs;

        edge->hdr.next = next;
        if (next) {
            next->prev = &edge->hdr;
        }

        compute_table_diff(world, table, to, edge, id);
    }
}

static
void init_remove_edge(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_graph_edge_t *edge,
    ecs_id_t id,
    ecs_table_t *to)
{
    init_edge(table, edge, id, to);

    ensure_hi_edge(world, &table->node.remove, id);

    if (table != to) {
        /* Remove edges are appended to refs.prev */
        ecs_graph_edge_hdr_t *to_refs = &to->node.refs;
        ecs_graph_edge_hdr_t *prev = to_refs->prev;

        to_refs->prev = &edge->hdr;
        edge->hdr.next = to_refs;

        edge->hdr.prev = prev;
        if (prev) {
            prev->next = &edge->hdr;
        }

        compute_table_diff(world, table, to, edge, id);
    }
}

static
ecs_table_t* find_or_create_table_without(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_graph_edge_t *edge,
    ecs_id_t id)
{
    ecs_table_t *to = find_or_create_table_without_id(world, node, id);
    
    init_remove_edge(world, node, edge, id, to);

    return to;   
}

static
ecs_table_t* find_or_create_table_with(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_graph_edge_t *edge,
    ecs_id_t id)
{
    ecs_table_t *to = find_or_create_table_with_id(world, node, id);

    if (ECS_HAS_ROLE(id, PAIR) && ECS_PAIR_FIRST(id) == EcsIsA) {
        ecs_entity_t base = ecs_pair_second(world, id);
        to = find_or_create_table_with_isa(world, to, base);
    }

    init_add_edge(world, node, edge, id, to);

    return to;
}

static
void populate_diff( 
    ecs_graph_edge_t *edge,
    ecs_id_t *add_ptr,
    ecs_id_t *remove_ptr,
    ecs_table_diff_t *out)
{
    if (out) {
        ecs_table_diff_t *diff = edge->diff;

        if (diff && diff != &ecs_table_edge_is_component) {
            ecs_assert(!add_ptr || !ECS_HAS_ROLE(add_ptr[0], CASE), 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(!remove_ptr || !ECS_HAS_ROLE(remove_ptr[0], CASE), 
                ECS_INTERNAL_ERROR, NULL);
            *out = *diff;
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
                if (diff == &ecs_table_edge_is_component) {
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
    ecs_graph_edge_t *edge = ensure_edge(world, &node->node.remove, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = find_or_create_table_without(world, node, edge, id);
        ecs_assert(to != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(edge, NULL, id_ptr, diff);

    return to;
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
    ecs_graph_edge_t *edge = ensure_edge(world, &node->node.add, id);
    ecs_table_t *to = edge->to;

    if (!to) {
        to = find_or_create_table_with(world, node, edge, id);
        ecs_assert(to != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(edge->to != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    populate_diff(edge, id_ptr, NULL, diff);

    return to;
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
    uint64_t new_id = flecs_sparse_new_id(&world->store.tables);
    ecs_assert(new_id == 0, ECS_INTERNAL_ERROR, NULL);
    (void)new_id;
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_poly_assert(world, ecs_world_t);

    ecs_log_push_1();

    ecs_map_iter_t it;
    ecs_graph_node_t *table_node = &table->node;
    ecs_graph_edges_t *node_add = &table_node->add;
    ecs_graph_edges_t *node_remove = &table_node->remove;
    ecs_map_t *add_hi = &node_add->hi;
    ecs_map_t *remove_hi = &node_remove->hi;
    ecs_graph_edge_hdr_t *node_refs = &table_node->refs;
    ecs_graph_edge_t *edge;
    uint64_t key;

    /* Cleanup outgoing edges */
    it = ecs_map_iter(add_hi);
    while ((edge = ecs_map_next_ptr(&it, ecs_graph_edge_t*, &key))) {
        disconnect_edge(world, key, edge);
    }

    it = ecs_map_iter(remove_hi);
    while ((edge = ecs_map_next_ptr(&it, ecs_graph_edge_t*, &key))) {
        disconnect_edge(world, key, edge);
    }

    /* Cleanup incoming add edges */
    ecs_graph_edge_hdr_t *next, *cur = node_refs->next;
    if (cur) {
        do {
            edge = (ecs_graph_edge_t*)cur;
            ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(edge->from != NULL, ECS_INTERNAL_ERROR, NULL);
            next = cur->next;
            remove_edge(world, &edge->from->node.add, edge->id, edge);
        } while ((cur = next));
    }

    /* Cleanup incoming remove edges */
    cur = node_refs->prev;
    if (cur) {
        do {
            edge = (ecs_graph_edge_t*)cur;
            ecs_assert(edge->to == table, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(edge->from != NULL, ECS_INTERNAL_ERROR, NULL);
            next = cur->prev;
            remove_edge(world, &edge->from->node.remove, edge->id, edge);
        } while ((cur = next));
    }

    ecs_os_free(node_add->lo);
    ecs_os_free(node_remove->lo);
    ecs_map_fini(add_hi);
    ecs_map_fini(remove_hi);
    table_node->add.lo = NULL;
    table_node->remove.lo = NULL;

    ecs_log_pop_1();
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
