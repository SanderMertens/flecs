#include "private_api.h"

/* When a new table is created, match it with the queries registered with the
 * world. If a query matches, it will call the esc_table_register_query 
 * function which will add the query to the table administration. */
void ecs_notify_queries_of_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t i, count = ecs_sparse_count(world->queries);

    for (i = 0; i < count; i ++) {
        ecs_query_t *query = ecs_sparse_get(world->queries, ecs_query_t, i);
        ecs_query_match_table(world, query, table);
    }
}

const EcsComponent* ecs_component_from_id(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_entity_t trait = 0;

    /* If this is a trait, get the trait component from the identifier */
    if (e & ECS_TRAIT) {
        trait = e;
        e = e & ECS_ENTITY_MASK;
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
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entity_t parent)
{
    if (stage == &world->stage) {
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
}

static
void init_edges(
    ecs_world_t *world,
    ecs_stage_t *stage,
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
                edge.remove = &stage->root;
            }

            ecs_map_set(table->hi_edges, e, &edge);
        } else {
            table->lo_edges[e].add = table;

            if (count == 1) {
                table->lo_edges[e].remove = &stage->root;
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

        if (e & ECS_XOR) {
            table->flags |= EcsTableHasXor;
        }

        if (e & ECS_INSTANCEOF) {
            table->flags |= EcsTableHasBase;
        }

        if (e & ECS_CHILDOF) {
            table->flags |= EcsTableHasParent;

            ecs_entity_t parent = e & ECS_ENTITY_MASK;
            register_child_table(world, stage, table, parent);
        }

        if (e & (ECS_CHILDOF | ECS_INSTANCEOF)) {
            if (stage == &world->stage) {
                ecs_set_watch(world, stage, e & ECS_ENTITY_MASK);
            }
        }

        /* Set flags based on component actions */
        ecs_c_info_t *ci = ecs_get_c_info(world, e & ECS_ENTITY_MASK);
        if (ci) {
            table->flags |= ecs_get_component_action_flags(ci);
        }
    }
    
    /* Register as root table */
    if (!(table->flags & EcsTableHasParent)) {
        register_child_table(world, stage, table, 0);
    }
}

static
void init_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_entities_t *entities)
{
    table->type = entities_to_type(entities);
    table->data = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->monitors = NULL;
    table->on_set = NULL;
    table->on_set_all = NULL;
    table->on_set_override = NULL;
    table->un_set_all = NULL;
    table->alloc_count = 0;
    
    init_edges(world, stage, table);

    table->queries = NULL;
    table->column_count = data_column_count(world, table);
}

static
ecs_table_t *create_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *entities)
{
    ecs_table_t *result = ecs_sparse_add(stage->tables, ecs_table_t);
    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    init_table(world, stage, result, entities);

#ifndef NDEBUG
    if (stage == &world->stage) {
        char *expr = ecs_type_str(world, result->type);
        ecs_trace_2("table #[green][%s]#[normal] created", expr);
        ecs_os_free(expr);
    }
#endif
    ecs_log_push();

    /* Don't notify queries if table is created in stage */
    if (stage == &world->stage) {
        ecs_notify_queries_of_table(world, result);
    }

    ecs_log_pop();

    return result;
}

void ecs_init_root_table(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_entities_t entities = {
        .array = NULL,
        .count = 0
    };

    init_table(world, stage, &stage->root, &entities);
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
ecs_table_t *find_or_create_table_include(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entity_t add)
{
    ecs_type_t type = node->type;
    int32_t count = ecs_vector_count(type);

    ecs_entities_t entities = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * (count + 1)),
        .count = count + 1
    };

    /* If table has a XOR column, check if the entity that is being added to the
     * table is part of the XOR type, and if it is, find the current entity in
     * the table type matching the XOR type. This entity must be replaced in
     * the new table, to ensure the XOR constraint isn't violated. */
    ecs_entity_t replace = 0;
    if (node->flags & EcsTableHasXor) {
        ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
        int32_t i, type_count = ecs_vector_count(type);
        ecs_type_t xor_type = NULL;

        for (i = type_count - 1; i >= 0; i --) {
            ecs_entity_t e = array[i];
            if (e & ECS_XOR) {
                ecs_entity_t e_type = e & ECS_ENTITY_MASK;
                const EcsType *type_ptr = ecs_get(world, e_type, EcsType);
                ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                if (ecs_type_owns_entity(
                    world, type_ptr->normalized, add, true)) 
                {
                    xor_type = type_ptr->normalized;
                }
            } else if (xor_type) {
                if (ecs_type_owns_entity(world, xor_type, e, true)) {
                    replace = e;
                    break;
                }
            }
        }

        ecs_assert(!xor_type || replace != 0, ECS_INTERNAL_ERROR, NULL);
    }

    add_entity_to_type(type, add, replace, &entities);

    ecs_table_t *result = ecs_table_find_or_create(world, stage, &entities);
    
    if (result != node && stage == &world->stage) {
        create_backlink_after_add(result, node, add);
    }

    return result;
}

static
ecs_table_t *find_or_create_table_exclude(
    ecs_world_t *world,
    ecs_stage_t *stage,
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

    ecs_table_t *result = ecs_table_find_or_create(world, stage, &entities);
    if (!result) {
        return NULL;
    }

    if (result != node && stage == &world->stage) {
        create_backlink_after_remove(result, node, remove);
    }

    return result;    
}

static
ecs_table_t* traverse_remove_hi_edges(
    ecs_world_t *world,
    ecs_stage_t *stage,
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
            next = find_or_create_table_exclude(
                world, stage, node, next_e);
            if (!next) {
                return NULL;
            }

            /* Only make links when not staged, to prevent tables from the main
            * stage pointing to staged tables. */
            if (&world->stage == stage) {
                edge->remove = next;
            }
        }

        if (removed && node != next) removed->array[removed->count ++] = e;

        node = next;        
    }

    return node;
}

ecs_table_t* ecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entities_t *to_remove,
    ecs_entities_t *removed)
{
    int32_t i, count = to_remove->count;
    ecs_entity_t *entities = to_remove->array;
    node = node ? node : &world->stage.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        /* If the array is not a simple component array, use a function that
         * handles all cases, but is slower */
        if (e >= ECS_HI_COMPONENT_ID) {
            return traverse_remove_hi_edges(world, stage, node, i, to_remove, 
                removed);
        }

        ecs_edge_t *edge = &node->lo_edges[e];
        ecs_table_t *next = edge->remove;

        if (!next) {
            if (edge->add == node) {
                /* Find table with all components of node except 'e' */
                next = find_or_create_table_exclude(world, stage, node, e);
                if (!next) {
                    return NULL;
                }

                /* Only make links when not staged, to prevent tables from the main
                * stage pointing to staged tables. */
                if (&world->stage == stage) {
                    edge->remove = next;
                }
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
ecs_table_t* traverse_add_hi_edges(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    int32_t i,
    ecs_entities_t *to_add,
    ecs_entities_t *added)
{
    int32_t count = to_add->count;
    ecs_entity_t *entities = to_add->array;

    for (; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_entity_t next_e = e;
        ecs_table_t *next;
        ecs_edge_t *edge;

        edge = get_edge(node, e);

        next = edge->add;

        if (!next) {
            next = find_or_create_table_include(
                world, stage, node, next_e);
            
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);                    

            /* Only make links when not staged, to prevent tables from the main
             * stage pointing to staged tables. */
            if (&world->stage == stage) {
                edge->add = next;
            }
        }

        if (added && node != next) added->array[added->count ++] = e;

        node = next;        
    }

    return node;
}

ecs_table_t* ecs_table_traverse_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *node,
    ecs_entities_t *to_add,
    ecs_entities_t *added)    
{
    int32_t i, count = to_add->count;
    ecs_entity_t *entities = to_add->array;
    node = node ? node : &world->stage.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_table_t *next;

        /* If the array is not a simple component array, use a function that
         * handles all cases, but is slower */
        if (e >= ECS_HI_COMPONENT_ID) {
            return traverse_add_hi_edges(world, stage, node, i, to_add, added);
        }

        /* There should always be an edge for adding */
        ecs_edge_t *edge = &node->lo_edges[e];
        next = edge->add;

        if (!next) {
            next = find_or_create_table_include(world, stage, node, e);
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);

            /* Only make links when not staged, to prevent tables from the main
             * stage pointing to staged tables. */
            if (&world->stage == stage) {
                edge->add = next;
            }
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
        if (e & ECS_TYPE_ROLE_MASK) {
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
        ecs_entity_t mask = e & ECS_TYPE_ROLE_MASK;
        if (!mask || !(mask & (ECS_OR | ECS_XOR | ECS_NOT))) {
            break;
        }

        ecs_entity_t entity = e & ECS_ENTITY_MASK;
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
    ecs_stage_t *stage,
    ecs_stage_t *search_stage,
    ecs_entities_t *entities)
{    
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);    

    /* Make sure array is ordered and does not contain duplicates */
    int32_t type_count = entities->count;
    ecs_entity_t *ordered = NULL;

    if (!type_count) {
        return &search_stage->root;
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
    ecs_sparse_t *tables = search_stage->tables;
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

    /* Table has not been found. If we were searching in the main stage, but
     * we are staged, try to find the table in the stage */
    if (stage != &world->stage) {
        if (search_stage != stage) {
            /* The table does not yet exist in the main stage, so create it in
             * the stage. Table will be merged later. This is an expensive
             * operation that should rarely happen */
            return find_or_create(world, stage, stage, &ordered_entities);
        }
    }

    /* If we get here, the table has not been found in any stages. It has to be
     * created. */
    
    ecs_table_t *result = create_table(world, stage, &ordered_entities);

    ecs_assert(ordered_entities.count == ecs_vector_count(result->type), 
        ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_table_t* ecs_table_find_or_create(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entities_t *components)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    return find_or_create(
        world, stage, &world->stage, components);
}

ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_type_t type)
{
    ecs_entities_t components = ecs_type_to_entities(type);
    return ecs_table_find_or_create(
        world, stage, &components);
}
