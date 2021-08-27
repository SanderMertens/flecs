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
    ecs_world_t * world,
    ecs_table_t * table)
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
    ecs_world_t * world,
    ecs_table_t * table)
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
ecs_type_t entities_to_type(
    ecs_ids_t *entities)
{
    if (entities->count) {
        ecs_vector_t *result = NULL;
        ecs_vector_set_count(&result, ecs_entity_t, entities->count);
        ecs_entity_t *array = ecs_vector_first(result, ecs_entity_t);
        ecs_os_memcpy(array, entities->array, ECS_SIZEOF(ecs_entity_t) * entities->count);
        return result;
    } else {
        return NULL;
    }
}

static
ecs_edge_t* get_edge(
    ecs_table_t *node,
    ecs_entity_t e)
{
    if (e < ECS_HI_COMPONENT_ID) {
        if (!node->lo_edges) {
            node->lo_edges = ecs_os_calloc_n(ecs_edge_t, ECS_HI_COMPONENT_ID);
        }
        return &node->lo_edges[e];
    } else {
        if (!node->hi_edges) {
            node->hi_edges = ecs_map_new(ecs_edge_t, 1);
        }
        return ecs_map_ensure(node->hi_edges, ecs_edge_t, e);
    }
}

static
void init_edges(
    ecs_table_t * table)
{
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    int32_t count = ecs_vector_count(table->type);

    table->lo_edges = NULL;
    table->hi_edges = NULL;
    
    /* Iterate components for table, initialize edges that point to self */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        ecs_edge_t *edge = get_edge(table, id);
        ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
        edge->add = table;    
    }
}

static
void init_flags(
    ecs_world_t * world,
    ecs_table_t * table)
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
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_ids_t * entities)
{
    table->type = entities_to_type(entities);
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

    init_edges(table);
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
    ecs_world_t * world,
    ecs_ids_t * entities,
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
void add_entity_to_type(
    ecs_type_t type,
    ecs_entity_t add,
    ecs_entity_t replace,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);    
    bool added = false;

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e == replace) {
            continue;
        }

        if (e > add && !added) {
            out->array[el ++] = add;
            added = true;
        }
        
        out->array[el ++] = e;

        ecs_assert(el <= out->count, ECS_INTERNAL_ERROR, NULL);
    }

    if (!added) {
        out->array[el ++] = add;
    }

    out->count = el;

    ecs_assert(out->count != 0, ECS_INTERNAL_ERROR, NULL);
}

static
void remove_entity_from_type(
    ecs_type_t type,
    ecs_entity_t remove,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e != remove) {
            out->array[el ++] = e;
            ecs_assert(el <= count, ECS_INTERNAL_ERROR, NULL);
        }
    }

    out->count = el;
}

static
void create_backlink_after_add(
    ecs_table_t * next,
    ecs_table_t * prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->remove) {
        edge->remove = prev;
    }
}

static
void create_backlink_after_remove(
    ecs_table_t * next,
    ecs_table_t * prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->add) {
        edge->add = prev;
    }
}

static
ecs_entity_t find_xor_replace(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_type_t type,
    ecs_entity_t add)
{
    if (table->flags & EcsTableHasXor) {
        ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
        int32_t i, type_count = ecs_vector_count(type);
        ecs_type_t xor_type = NULL;

        for (i = type_count - 1; i >= 0; i --) {
            ecs_entity_t e = array[i];
            if (ECS_HAS_ROLE(e, XOR)) {
                ecs_entity_t e_type = e & ECS_COMPONENT_MASK;
                const EcsType *type_ptr = ecs_get(world, e_type, EcsType);
                ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                if (ecs_type_has_id(
                    world, type_ptr->normalized, add, true)) 
                {
                    xor_type = type_ptr->normalized;
                }
            } else if (xor_type) {
                if (ecs_type_has_id(world, xor_type, e, true)) {
                    return e;
                }
            }
        }
    }

    return 0;
}

int32_t flecs_table_switch_from_case(
    const ecs_world_t * world,
    const ecs_table_t * table,
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
ecs_table_t *find_or_create_table_include(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_entity_t add)
{
    /* If table has one or more switches and this is a case, return self */
    if (ECS_HAS_ROLE(add, CASE)) {
        ecs_assert((node->flags & EcsTableHasSwitch) != 0, 
            ECS_TYPE_INVALID_CASE, NULL);
        return node;
    } else {
        ecs_type_t type = node->type;
        int32_t count = ecs_vector_count(type);

        ecs_ids_t entities = {
            .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * (count + 1)),
            .count = count + 1
        };

        /* If table has a XOR column, check if the entity that is being added to
         * the table is part of the XOR type, and if it is, find the current 
         * entity in the table type matching the XOR type. This entity must be 
         * replaced in the new table, to ensure the XOR constraint isn't 
         * violated. */
        ecs_entity_t replace = find_xor_replace(world, node, type, add);

        add_entity_to_type(type, add, replace, &entities);

        ecs_table_t *result = flecs_table_find_or_create(world, &entities);
        
        if (result != node) {
            create_backlink_after_add(result, node, add);
        }

        return result;
    }
}

static
ecs_table_t *find_or_create_table_exclude(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_entity_t remove)
{
    ecs_type_t type = node->type;
    int32_t count = ecs_vector_count(type);

    ecs_ids_t entities = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * count),
        .count = count
    };

    remove_entity_from_type(type, remove, &entities);

    ecs_table_t *result = flecs_table_find_or_create(world, &entities);
    if (!result) {
        return NULL;
    }

    if (result != node) {
        create_backlink_after_remove(result, node, remove);
    }

    return result;    
}

ecs_table_t* flecs_table_traverse_remove(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_id_t id,
    ecs_ids_t * removed)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    node = node ? node : &world->store.root;

    /* Removing 0 from an entity is not valid */
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_edge_t *edge = get_edge(node, id);
    ecs_table_t *next = edge->remove;

    if (!next) {
        if (edge->add == node) {
            /* Find table with all components of node except 'e' */
            next = find_or_create_table_exclude(world, node, id);
            if (!next) {
                return NULL;
            }

            edge->remove = next;
        } else {
            return node;
        }
    }

    bool has_case = ECS_HAS_ROLE(id, CASE);
    if (removed && (node != next || has_case)) {
        removed->array[removed->count ++] = id; 
    }

    return next;
}

static
ecs_table_t* find_owned_components(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t base,
    ecs_ids_t *added)
{
    /* If we're adding an IsA relationship, check if the base
     * has OVERRIDE components that need to be added to the instance */
    ecs_type_t t = ecs_get_type(world, base);

    int i, count = ecs_vector_count(t);
    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (ECS_HAS_RELATION(e, EcsIsA)) {
            node = find_owned_components(
                world, node, ECS_PAIR_OBJECT(e), added);
        } else
        if (ECS_HAS_ROLE(e, OVERRIDE)) {
            e = e & ECS_COMPONENT_MASK;
            
            /* If entity is a type, add each component in the type */
            const EcsType *t_ptr = ecs_get(world, e, EcsType);
            if (t_ptr) {
                ecs_type_t n = t_ptr->normalized;
                int32_t j, n_count = ecs_vector_count(n);
                ecs_entity_t *n_ids = ecs_vector_first(n, ecs_entity_t);
                for (j = 0; j < n_count; j ++) {
                    node = flecs_table_traverse_add(
                        world, node, n_ids[j], added);
                }
            } else {
                node = flecs_table_traverse_add(
                    world, node, ECS_PAIR_OBJECT(e), added);
            }
        }
    }

    return node;
}

ecs_table_t* flecs_table_traverse_add(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_id_t id,
    ecs_ids_t * added)    
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    node = node ? node : &world->store.root;

    /* Adding 0 to an entity is not valid */
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_edge_t *edge = get_edge(node, id);
    ecs_table_t *next = edge->add;

    if (!next) {
        next = find_or_create_table_include(world, node, id);
        ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
        edge->add = next;
    }

    bool has_case = ECS_HAS_ROLE(id, CASE);
    if (added && (node != next || has_case)) {
        added->array[added->count ++] = id; 
    }

    if ((node != next) && ECS_HAS_RELATION(id, EcsIsA)) {
        next = find_owned_components(
            world, next, ecs_pair_object(world, id), added);
    }

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
    ecs_world_t * world,
    const ecs_ids_t * components)
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

    uint32_t i;

    if (table->lo_edges) {
        for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
            ecs_edge_t *e = &table->lo_edges[i];
            ecs_table_t *add = e->add, *remove = e->remove;

            if (add) {
                add->lo_edges[i].remove = NULL;
            }
            if (remove) {
                remove->lo_edges[i].add = NULL;
            }
        }
    }

    ecs_map_iter_t it = ecs_map_iter(table->hi_edges);
    ecs_edge_t *edge;
    ecs_map_key_t component;
    while ((edge = ecs_map_next(&it, ecs_edge_t, &component))) {
        ecs_table_t *add = edge->add, *remove = edge->remove;
        if (add) {
            ecs_edge_t *e = get_edge(add, component);
            e->remove = NULL;
            if (!e->add) {
                ecs_map_remove(add->hi_edges, component);
            }
        }
        if (remove) {
            ecs_edge_t *e = get_edge(remove, component);
            e->add = NULL;
            if (!e->remove) {
                ecs_map_remove(remove->hi_edges, component);
            }
        }
    }
}

/* Public convenience functions for traversing table graph */
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    return flecs_table_traverse_add(world, table, id, NULL);
}

ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    return flecs_table_traverse_remove(world, table, id, NULL);
}
