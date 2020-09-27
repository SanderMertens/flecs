#include "private_api.h"

ecs_entity_t ecs_component_id_from_id(
    ecs_world_t *world,
    ecs_entity_t e)
{
    if (ECS_HAS_ROLE(e, TRAIT)) {
        ecs_entity_t trait = ecs_entity_t_hi(e & ECS_COMPONENT_MASK);
        if (ecs_has(world, trait, EcsComponent)) {
            /* This is not a trait tag, trait is the value */
            return trait;
        } else {
            /* This is a trait tag, component is the value */
            return ecs_entity_t_lo(e);
        }
    } else if (e & ECS_ROLE_MASK) {
        return 0;
    }

    return e;
}

const EcsComponent* ecs_component_from_id(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_entity_t trait = 0;

    /* If this is a trait, get the trait component from the identifier */
    if (ECS_HAS_ROLE(e, TRAIT)) {
        trait = e;
        e = e & ECS_COMPONENT_MASK;
        e = ecs_entity_t_hi(e);
    }

    const EcsComponent *component = ecs_get(world, e, EcsComponent);
    if (!component && trait) {
        /* If this is a trait column and the trait is not a component, use
            * the component type of the component the trait is applied to. */
        e = ecs_entity_t_lo(trait);
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
        if ((component == ecs_entity(EcsComponent)) || 
            (component == ecs_entity(EcsName)) || 
            ecs_component_from_id(world, component) != NULL) 
        {
            count = c_ptr_i + 1;
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

static
ecs_type_t entities_to_type(
    ecs_entities_t *entities)
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
void register_child_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t parent)
{
    /* Register child table with parent */
    ecs_vector_t *child_tables = ecs_map_get_ptr(
            world->child_tables, ecs_vector_t*, parent);
    if (!child_tables) {
        child_tables = ecs_vector_new(ecs_table_t*, 1);
    }
    
    ecs_table_t **el = ecs_vector_add(&child_tables, ecs_table_t*);
    *el = table;

    if (!world->child_tables) {
        world->child_tables = ecs_map_new(ecs_vector_t*, 1);
    }

    ecs_map_set(world->child_tables, parent, &child_tables);
}

static
void init_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_entity_t *entities = ecs_vector_first(table->type, ecs_entity_t);
    int32_t count = ecs_vector_count(table->type);

    table->lo_edges = ecs_os_calloc(sizeof(ecs_edge_t) * ECS_HI_COMPONENT_ID);
    table->hi_edges = ecs_map_new(ecs_edge_t, 0);

    table->lo_edges[0].add = table;
    
    /* Make add edges to own components point to self */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        if (e >= ECS_HI_COMPONENT_ID) {
            ecs_edge_t edge = { .add = table };

            if (count == 1) {
                edge.remove = &world->store.root;
            }

            ecs_map_set(table->hi_edges, e, &edge);
        } else {
            table->lo_edges[e].add = table;

            if (count == 1) {
                table->lo_edges[e].remove = &world->store.root;
            } else {
                table->lo_edges[e].remove = NULL;
            }
        }

        /* As we're iterating over the table components, also set the table
         * flags. These allow us to quickly determine if the table contains
         * data that needs to be handled in a special way, like prefabs or 
         * containers */
        if (e <= EcsLastInternalComponentId) {
            table->flags |= EcsTableHasBuiltins;
        }

        if (e == EcsPrefab) {
            table->flags |= EcsTableIsPrefab;
            table->flags |= EcsTableIsDisabled;
        }

        if (e == EcsDisabled) {
            table->flags |= EcsTableIsDisabled;
        }

        if (e == ecs_entity(EcsComponent)) {
            table->flags |= EcsTableHasComponentData;
        }

        if (ECS_HAS_ROLE(e, XOR)) {
            table->flags |= EcsTableHasXor;
        }

        if (ECS_HAS_ROLE(e, INSTANCEOF)) {
            table->flags |= EcsTableHasBase;
        }

        if (ECS_HAS_ROLE(e, SWITCH)) {
            table->flags |= EcsTableHasSwitch;
        }

        if (ECS_HAS_ROLE(e, CHILDOF)) {
            table->flags |= EcsTableHasParent;

            ecs_entity_t parent = e & ECS_COMPONENT_MASK;
            register_child_table(world, table, parent);
        }

        if (ECS_HAS_ROLE(e, CHILDOF) || ECS_HAS_ROLE(e, INSTANCEOF)) {
            ecs_set_watch(world, e & ECS_COMPONENT_MASK);
        }
    }

    /* Register component info flags for all columns */
    ecs_table_notify(world, table, &(ecs_table_event_t){
        .kind = EcsTableComponentInfo
    });
    
    /* Register as root table */
    if (!(table->flags & EcsTableHasParent)) {
        register_child_table(world, table, 0);
    }
}

static
void init_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entities_t *entities)
{
    table->type = entities_to_type(entities);
    table->c_info = NULL;
    table->data = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->monitors = NULL;
    table->on_set = NULL;
    table->on_set_all = NULL;
    table->on_set_override = NULL;
    table->un_set_all = NULL;
    table->alloc_count = 0;

    table->queries = NULL;
    table->column_count = data_column_count(world, table);
    table->sw_column_count = switch_column_count(table);

    init_edges(world, table);
}

static
ecs_table_t *create_table(
    ecs_world_t *world,
    ecs_entities_t *entities)
{
    ecs_table_t *result = ecs_sparse_add(world->store.tables, ecs_table_t);
    result->id = ecs_to_u32(ecs_sparse_last_id(world->store.tables));

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
    init_table(world, result, entities);

#ifndef NDEBUG
    char *expr = ecs_type_str(world, result->type);
    ecs_trace_2("table #[green][%s]#[normal] created", expr);
    ecs_os_free(expr);
#endif
    ecs_log_push();

    ecs_notify_queries(world, &(ecs_query_event_t) {
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
    ecs_entities_t *out)
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
    ecs_entities_t *out)
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
ecs_edge_t* get_edge(
    ecs_table_t *node,
    ecs_entity_t e)
{
    ecs_edge_t *edge;

    if (e < ECS_HI_COMPONENT_ID) {
        edge = &node->lo_edges[e];
    } else {
        edge = ecs_map_get(node->hi_edges, ecs_edge_t, e);        
        if (!edge) {
            ecs_edge_t new_edge = {0};
            ecs_map_set(node->hi_edges, e, &new_edge);
            edge = ecs_map_get(node->hi_edges, ecs_edge_t, e);    
            ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
        }
    }

    return edge;
}

static
void create_backlink_after_add(
    ecs_table_t *next,
    ecs_table_t *prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->remove) {
        edge->remove = prev;
    }
}

static
void create_backlink_after_remove(
    ecs_table_t *next,
    ecs_table_t *prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->add) {
        edge->add = prev;
    }
}

static
ecs_entity_t find_xor_replace(
    ecs_world_t *world,
    ecs_table_t *table,
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

                if (ecs_type_owns_entity(
                    world, type_ptr->normalized, add, true)) 
                {
                    xor_type = type_ptr->normalized;
                }
            } else if (xor_type) {
                if (ecs_type_owns_entity(world, xor_type, e, true)) {
                    return e;
                }
            }
        }
    }

    return 0;
}

int32_t ecs_table_switch_from_case(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t add)
{
    ecs_type_t type = table->type;
    ecs_data_t *data = ecs_table_get_data(table);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    int32_t i, count = table->sw_column_count;
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    add = add & ECS_COMPONENT_MASK;

    ecs_sw_column_t *sw_columns = NULL;

    if (data && (sw_columns = data->sw_columns)) {
        /* Fast path, we can get the switch type from the column data */
        for (i = 0; i < count; i ++) {
            ecs_type_t sw_type = sw_columns[i].type;
            if (ecs_type_owns_entity(world, sw_type, add, true)) {
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

            if (ecs_type_owns_entity(
                world, type_ptr->normalized, add, true)) 
            {
                return i;
            }
        }
    }

    /* If a table was not found, this is an invalid switch case */
    ecs_abort(ECS_INVALID_CASE, NULL);

    return -1;
}

static
ecs_table_t *find_or_create_table_include(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t add)
{
    /* If table has one or more switches and this is a case, return self */
    if (ECS_HAS_ROLE(add, CASE)) {
        ecs_assert((node->flags & EcsTableHasSwitch) != 0, 
            ECS_INVALID_CASE, NULL);
        return node;
    } else {
        ecs_type_t type = node->type;
        int32_t count = ecs_vector_count(type);

        ecs_entities_t entities = {
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

        ecs_table_t *result = ecs_table_find_or_create(world, &entities);
        
        if (result != node) {
            create_backlink_after_add(result, node, add);
        }

        return result;
    }
}

static
ecs_table_t *find_or_create_table_exclude(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t remove)
{
    ecs_type_t type = node->type;
    int32_t count = ecs_vector_count(type);

    ecs_entities_t entities = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * count),
        .count = count
    };

    remove_entity_from_type(type, remove, &entities);

    ecs_table_t *result = ecs_table_find_or_create(world, &entities);
    if (!result) {
        return NULL;
    }

    if (result != node) {
        create_backlink_after_remove(result, node, remove);
    }

    return result;    
}

static
ecs_table_t* traverse_remove_hi_edges(
    ecs_world_t *world,
    ecs_table_t *node,
    int32_t i,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed)
{
    int32_t count = to_remove->count;
    ecs_entity_t *entities = to_remove->array;

    for (; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_entity_t next_e = e;
        ecs_table_t *next;
        ecs_edge_t *edge;

        edge = get_edge(node, e);

        next = edge->remove;

        if (!next) {
            next = find_or_create_table_exclude(world, node, next_e);
            if (!next) {
                return NULL;
            }

            edge->remove = next;
        }

        if (removed && node != next) removed->array[removed->count ++] = e;

        node = next;        
    }

    return node;
}

ecs_table_t* ecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed)
{
    int32_t i, count = to_remove->count;
    ecs_entity_t *entities = to_remove->array;
    node = node ? node : &world->store.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        /* If the array is not a simple component array, use a function that
         * handles all cases, but is slower */
        if (e >= ECS_HI_COMPONENT_ID) {
            return traverse_remove_hi_edges(world, node, i, to_remove, 
                removed);
        }

        ecs_edge_t *edge = &node->lo_edges[e];
        ecs_table_t *next = edge->remove;

        if (!next) {
            if (edge->add == node) {
                /* Find table with all components of node except 'e' */
                next = find_or_create_table_exclude(world, node, e);
                if (!next) {
                    return NULL;
                }

                edge->remove = next;
            } else {
                /* If the add edge does not point to self, the table
                    * does not have the entity in to_remove. */
                continue;
            }
        }

        if (removed) removed->array[removed->count ++] = e;

        node = next;
    }    

    return node;
}

static
void find_owned_components(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entity_t base,
    ecs_entities_t *owned)
{
    /* If we're adding an INSTANCEOF relationship, check if the base
     * has OWNED components that need to be added to the instance */
    ecs_type_t t = ecs_get_type(world, base);

    int i, count = ecs_vector_count(t);
    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (ECS_HAS_ROLE(e, INSTANCEOF)) {
            find_owned_components(world, node, e & ECS_COMPONENT_MASK, owned);
        } else
        if (ECS_HAS_ROLE(e, OWNED)) {
            e = e & ECS_COMPONENT_MASK;
            
            /* If entity is a type, add each component in the type */
            const EcsType *t_ptr = ecs_get(world, e, EcsType);
            if (t_ptr) {
                ecs_type_t n = t_ptr->normalized;
                int32_t j, n_count = ecs_vector_count(n);
                ecs_entity_t *n_entities = ecs_vector_first(n, ecs_entity_t);
                for (j = 0; j < n_count; j ++) {
                    owned->array[owned->count ++] = n_entities[j];
                }
            } else {
                owned->array[owned->count ++] = e & ECS_COMPONENT_MASK;
            }
        }
    }
}

static
ecs_table_t* traverse_add_hi_edges(
    ecs_world_t *world,
    ecs_table_t *node,
    int32_t i,
    ecs_entities_t *to_add,
    ecs_entities_t *added)
{
    int32_t count = to_add->count;
    ecs_entity_t *entities = to_add->array;

    ecs_entity_t owned_array[ECS_MAX_ADD_REMOVE];
    ecs_entities_t owned = {
        .array = owned_array,
        .count = 0
    };

    for (; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_entity_t next_e = e;
        ecs_table_t *next;
        ecs_edge_t *edge;

        edge = get_edge(node, e);

        next = edge->add;

        if (!next) {
            next = find_or_create_table_include(world, node, next_e);
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
            edge->add = next;
        }

        bool has_case = ECS_HAS_ROLE(e, CASE);
        if (added && (node != next || has_case)) {
            /* If this is a case, find switch and encode it in added id */
            if (has_case) {
                int32_t s_case = ecs_table_switch_from_case(world, node, e);
                ecs_assert(s_case != -1, ECS_INTERNAL_ERROR, NULL);
                e = ECS_CASE | ecs_entity_t_comb(e, s_case);
            }

            added->array[added->count ++] = e; 
        }

        if ((node != next) && ECS_HAS_ROLE(e, INSTANCEOF)) {
            find_owned_components(world, next, ECS_COMPONENT_MASK & e, &owned);
        } 

        node = next;
    }

    /* In case OWNED components were found, add them as well */
    if (owned.count) {
        node = ecs_table_traverse_add(world, node, &owned, added);
    }

    return node;
}

ecs_table_t* ecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *node,
    ecs_entities_t *to_add,
    ecs_entities_t *added)    
{
    int32_t i, count = to_add->count;
    ecs_entity_t *entities = to_add->array;
    node = node ? node : &world->store.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_table_t *next;

        /* If the array is not a simple component array, use a function that
         * handles all cases, but is slower */
        if (e >= ECS_HI_COMPONENT_ID) {
            return traverse_add_hi_edges(world, node, i, to_add, added);
        }

        /* There should always be an edge for adding */
        ecs_edge_t *edge = &node->lo_edges[e];
        next = edge->add;

        if (!next) {
            next = find_or_create_table_include(world, node, e);
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
            edge->add = next;
        }

        if (added && node != next) {
            added->array[added->count ++] = e;
        }

        node = next;
    }

    return node;
}

static
int ecs_entity_compare(
    const void *e1,
    const void *e2)
{
    ecs_entity_t v1 = *(ecs_entity_t*)e1;
    ecs_entity_t v2 = *(ecs_entity_t*)e2;
    if (v1 < v2) {
        return -1;
    } else if (v1 > v2) {
        return 1;
    } else {
        return 0;
    }
}

static
bool ecs_entity_array_is_ordered(
    ecs_entities_t *entities)
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

#ifndef NDEBUG

static
int32_t count_occurrences(
    ecs_world_t *world,
    ecs_entities_t *entities,
    ecs_entity_t entity,
    int32_t constraint_index)    
{
    const EcsType *type_ptr = ecs_get(world, entity, EcsType);
    ecs_assert(type_ptr != NULL, 
        ECS_INVALID_PARAMETER, "flag must be applied to type");

    ecs_type_t type = type_ptr->normalized;
    int32_t count = 0;
    
    int i;
    for (i = 0; i < constraint_index; i ++) {
        ecs_entity_t e = entities->array[i];
        if (e & ECS_ROLE_MASK) {
            break;
        }

        if (ecs_type_has_entity(world, type, e)) {
            count ++;
        }
    }

    return count;
}

static
void verify_constraints(
    ecs_world_t *world,
    ecs_entities_t *entities)
{
    int i, count = entities->count;
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = entities->array[i];
        ecs_entity_t mask = e & ECS_ROLE_MASK;
        if (!mask || 
            ((mask != ECS_OR) &&
             (mask != ECS_XOR) &&
             (mask != ECS_NOT)))
        {
            break;
        }

        ecs_entity_t entity = e & ECS_COMPONENT_MASK;
        int32_t matches = count_occurrences(world, entities, entity, i);
        switch(mask) {
        case ECS_OR:
            ecs_assert(matches >= 1, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);
            break;
        case ECS_XOR:
            ecs_assert(matches == 1, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);
            break;
        case ECS_NOT:
            ecs_assert(matches == 0, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);    
            break;
        }
    }
}

#endif

static
ecs_table_t *find_or_create(
    ecs_world_t *world,
    ecs_entities_t *entities)
{    
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Make sure array is ordered and does not contain duplicates */
    int32_t type_count = entities->count;
    ecs_entity_t *ordered = NULL;

    if (!type_count) {
        return &world->store.root;
    }

    if (!ecs_entity_array_is_ordered(entities)) {
        ecs_size_t size = ECS_SIZEOF(ecs_entity_t) * type_count;
        ordered = ecs_os_alloca(size);
        ecs_os_memcpy(ordered, entities->array, size);
        qsort(ordered, (size_t)type_count, sizeof(ecs_entity_t), ecs_entity_compare);
        type_count = ecs_entity_array_dedup(ordered, type_count);
    } else {
        ordered = entities->array;
    }    

    /* Iterate tables, look if a table matches the type */
    ecs_sparse_t *tables = world->store.tables;
    int32_t i, count = ecs_sparse_count(tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_type_t type = table->type;
        int32_t table_type_count = ecs_vector_count(type);

        /* If types do not contain same number of entities, table won't match */
        if (table_type_count != type_count) {
            continue;
        }

        /* Memcmp the types, as they must match exactly */
        ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
        if (!ecs_os_memcmp(ordered, type_array, type_count * ECS_SIZEOF(ecs_entity_t))) {
            /* Table found */
            return table;
        }
    }  

    ecs_entities_t ordered_entities = {
        .array = ordered,
        .count = type_count
    };

#ifndef NDEBUG
    /* Check for constraint violations */
    verify_constraints(world, &ordered_entities);
#endif

    /* If we get here, the table has not been found. It has to be created. */
    
    ecs_table_t *result = create_table(world, &ordered_entities);

    ecs_assert(ordered_entities.count == ecs_vector_count(result->type), 
        ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_table_t* ecs_table_find_or_create(
    ecs_world_t *world,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    return find_or_create(world, components);
}

ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_entities_t components = ecs_type_to_entities(type);
    return ecs_table_find_or_create(
        world, &components);
}

void ecs_init_root_table(
    ecs_world_t *world)
{
    ecs_entities_t entities = {
        .array = NULL,
        .count = 0
    };

    init_table(world, &world->store.root, &entities);
}
