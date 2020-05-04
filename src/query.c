#include "flecs_private.h"

static
ecs_entity_t components_contains(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_type_t type,
    ecs_entity_t *entity_out,
    bool match_all)
{
    int32_t i, count = ecs_vector_count(table_type);
    ecs_entity_t *array = ecs_vector_first(table_type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t entity = array[i];

        if (entity & ECS_CHILDOF) {
            entity &= ECS_ENTITY_MASK;

            ecs_record_t *record = ecs_eis_get(&world->stage, entity);
            ecs_assert(record != 0, ECS_INTERNAL_ERROR, NULL);

            if (record->table) {
                ecs_entity_t component = ecs_type_contains(
                    world, record->table->type, type, match_all, true);

                if (component) {
                    if (entity_out) *entity_out = entity;
                    return component;
                }
            }
        }
    }

    return 0;
}

/* Get actual entity on which specified component is stored */
static
ecs_entity_t get_entity_for_component(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component)
{
    if (entity) {
        ecs_record_t *record = ecs_eis_get(&world->stage, entity);
        ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        if (record->table) {
            type = record->table->type;
        } else {
            type = NULL;
        }
    }

    ecs_entity_t *array = ecs_vector_first(type);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        if (array[i] == component) {
            break;
        }
    }

    if (i == count) {
        entity = ecs_find_entity_in_prefabs(world, entity, type, component, 0);
    }

    return entity;
}

static
ecs_entity_t get_cascade_component(
    ecs_query_t *query)
{
    ecs_sig_column_t *column = ecs_vector_first(query->sig.columns);
    return column[query->cascade_by - 1].is.component;
}

static
int32_t rank_by_depth(
    ecs_world_t *world,
    ecs_entity_t rank_by_component,
    ecs_type_t type)
{
    int32_t result = 0;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type);

    for (i = count - 1; i >= 0; i --) {
        if (array[i] & ECS_CHILDOF) {
            ecs_type_t c_type = ecs_get_type(world, array[i] & ECS_ENTITY_MASK);
            int32_t j, c_count = ecs_vector_count(c_type);
            ecs_entity_t *c_array = ecs_vector_first(c_type);

            for (j = 0; j < c_count; j ++) {
                if (c_array[j] == rank_by_component) {
                    result ++;
                    result += rank_by_depth(world, rank_by_component, c_type);
                    break;
                }
            }

            if (j != c_count) {
                break;
            }
        } else if (!(array[i] & ECS_TYPE_FLAG_MASK)) {
            /* No more parents after this */
            break;
        }
    }

    return result;
}

static
int table_compare(
    const void *t1,
    const void *t2)
{
    const ecs_matched_table_t *table_1 = t1;
    const ecs_matched_table_t *table_2 = t2;

    return table_1->rank - table_2->rank;
}

static
void order_ranked_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->rank_table) {
        ecs_vector_sort(query->tables, ecs_matched_table_t, table_compare);
    }

    if (query->is_monitor) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            ecs_table_register_monitor(
                world, table->table, query->system, table_i);
        })
    }
}

static
void rank_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_matched_table_t *table)
{
    if (query->rank_table) {
        table->rank = query->rank_table(
            world, query->rank_on_component, table->table->type);
    } else {
        table->rank = 0;
    }
}

/* Rank all tables of query. Only necessary if a new ranking function was
 * provided or if a monitored entity set the component used for ranking. */
static
void rank_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->rank_table) {
        int i, count = ecs_vector_count(query->tables);
        ecs_matched_table_t *tables = ecs_vector_first(query->tables);
        for (i = 0; i < count; i ++) {
            ecs_matched_table_t *table = &tables[i];
            rank_table(world, query, table);
        }

        count = ecs_vector_count(query->inactive_tables);
        tables = ecs_vector_first(query->inactive_tables);
        for (i = 0; i < count; i ++) {
            ecs_matched_table_t *table = &tables[i];
            rank_table(world, query, table);
        }        
    }
}

/** Add table to system, compute offsets for system components in table rows */
static
void add_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_matched_table_t *table_data;
    ecs_type_t table_type = NULL;
    int32_t c, column_count = ecs_vector_count(query->sig.columns);

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */
    if (table) {
        table_type = table->type;
        table_data = ecs_vector_add(
            &query->inactive_tables, ecs_matched_table_t);

        table_data->table = table;

        /* Assign a rank to the table if table sorting is enabled for query */
        rank_table(world, query, table_data);

#ifndef NDEBUG
        char *type_expr = ecs_type_str(world, table->type);
        if (query->system) {
            ecs_trace_1("query #[green]%s#[reset] matched with table #[green][%s]",
                ecs_get_name(world, query->system), type_expr);
        } else {
            ecs_trace_1("query %p matched with table #[green][%s]",
                query, type_expr);            
        }
        ecs_os_free(type_expr);
#endif
    } else {
        /* If no table is provided to function, this is a system that contains
         * no columns that require table matching. In this case, the system will
         * only have one "dummy" table that caches data from the system columns.
         * Always add this dummy table to the list of active tables, since it
         * would never get activated otherwise. */
        table_data = ecs_vector_add(
            &query->tables, ecs_matched_table_t);

        table_data->table = NULL;
    }

    table_data->references = NULL;
    table_data->columns = NULL;
    table_data->components = NULL;
    table_data->monitor = NULL;
    rank_table(world, query, table_data);

    if (column_count) {
        /* Array that contains the system column to table column mapping */
        table_data->columns = ecs_os_malloc(sizeof(uint32_t) * column_count);
        ecs_assert(table_data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Store the components of the matched table. In the case of OR expressions,
        * components may differ per matched table. */
        table_data->components = ecs_os_malloc(sizeof(ecs_entity_t) * column_count);
        ecs_assert(table_data->components != NULL, ECS_OUT_OF_MEMORY, NULL);
    }

    /* Walk columns parsed from the system signature */
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns);

    for (c = 0; c < column_count; c ++) {
        ecs_sig_column_t *column = &columns[c];
        ecs_entity_t entity = 0, component = 0;
        ecs_sig_oper_kind_t op = column->oper_kind;
        ecs_sig_from_kind_t from = column->from_kind;

        if (op == EcsOperNot) {
            from = EcsFromEmpty;
        }        

        /* Column that retrieves data from self or a fixed entity */
        if (from == EcsFromSelf || from == EcsFromEntity || 
            from == EcsFromOwned || from == EcsFromShared) 
        {
            if (op == EcsOperAnd || op == EcsOperNot) {
                component = column->is.component;
            } else if (op == EcsOperOptional) {
                component = column->is.component;
            } else if (op == EcsOperOr) {
                component = ecs_type_contains(
                    world, table_type, column->is.type, 
                    false, true);
            }

            if (from == EcsFromEntity) {
                entity = column->source;
            }

        /* Column that just passes a handle to the system (no data) */
        } else if (from == EcsFromEmpty) {
            component = column->is.component;
            table_data->columns[c] = 0;

        /* Column that retrieves data from a dynamic entity */
        } else if (from == EcsFromContainer || from == EcsCascade) {
            if (op == EcsOperAnd ||
                op == EcsOperOptional)
            {
                component = column->is.component;

                ecs_components_contains_component(
                    world, table_type, component, ECS_CHILDOF, &entity);

            } else if (op == EcsOperOr) {
                component = components_contains(
                    world,
                    table_type,
                    column->is.type,
                    &entity,
                    false);
            }

        /* Column that retrieves data from a system */
        } else if (from == EcsFromSystem) {
            if (op == EcsOperAnd) {
                component = column->is.component;
            }

            entity = query->system;
        }

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromContainer) and is not just a handle */
        if (!entity && from != EcsFromEmpty) {
            if (component) {
                /* Retrieve offset for component */
                table_data->columns[c] = ecs_type_index_of(table_type, component);

                /* If column is found, add one to the index, as column zero in
                 * a table is reserved for entity id's */
                if (table_data->columns[c] != -1) {
                    table_data->columns[c] ++;

                    /* Check if component is a tag. If it is, set table_data to
                     * zero, so that a system won't try to access the data */
                    const EcsComponent *data = ecs_get_ptr(
                        world, component, EcsComponent);

                    if (!data || !data->size) {
                        table_data->columns[c] = 0;
                    }
                }
                
                /* ecs_table_column_offset may return -1 if the component comes
                 * from a prefab. If so, the component will be resolved as a
                 * reference (see below) */
            }
        }

        if (op == EcsOperOptional) {
            /* If table doesn't have the field, mark it as no data */
            if (!ecs_type_has_entity(
                world, table_type, component))
            {
                table_data->columns[c] = 0;
            }
        }

        /* Check if a the component is a reference. If 'entity' is set, the
         * component must be resolved from another entity, which is the case
         * for FromEntity and FromContainer. 
         * 
         * If no entity is set but the component is not found in the table, it
         * must come from a prefab. This is guaranteed, as at this point it is
         * already validated that the table matches with the system.
         * 
         * If the column from is FromSingleton, the entity will be 0, but still
         * a reference needs to be added to the singleton component.
         * 
         * If the column from is Cascade, there may not be an entity in case the
         * current table contains root entities. In that case, still add a
         * reference field. The application can, after the table has matched,
         * change the set of components, so that this column will turn into a
         * reference. Having the reference already linked to the system table
         * makes changing this administation easier when the change happens.
         * */
        if (entity || table_data->columns[c] == -1 || from == EcsCascade) {
            if (ecs_has(world, component, EcsComponent)) {
                const EcsComponent *component_data = ecs_get_ptr(
                        world, component, EcsComponent);
                
                ecs_entity_t e;
                ecs_reference_t *ref = ecs_vector_add(
                        &table_data->references, ecs_reference_t);
                
                table_data->columns[c] = -ecs_vector_count(table_data->references);

                /* Find the entity for the component */
                if (from == EcsFromEntity) {
                    e = entity;
                } else if (from == EcsCascade) {
                    e = entity;
                } else if (from == EcsFromSystem) {
                    e = entity;
                } else {
                    e = get_entity_for_component(
                        world, entity, table_type, component);
                }

                if (from != EcsCascade) {
                    ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
                }
                
                ref->entity = e;
                ref->component = component;

                if (component_data->size) {
                    if (e) {
                        ref->cached_ptr = (ecs_cached_ptr_t){0};

                        ecs_get_cached_ptr_w_entity(
                            world, &ref->cached_ptr, e, component);
                        ecs_set_watch(world, &world->stage, e);                     
                    }

                    /* Negative number indicates ref instead of offset to ecs_data */
                    query->has_refs = true;
                }
            }
        }

        /* component_data index is not offset by anything */
        table_data->components[c] = component;
    }

    if (table) {
        ecs_table_register_query(world, table, query);
    }
}

static
bool match_column(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_entity_t component,
    ecs_entity_t source,
    ecs_dbg_match_failure_t *failure_info)
{
    if (from_kind == EcsFromSelf) {
        failure_info->reason = EcsMatchFromSelf;
        return ecs_type_has_entity(world, type, component);
        
    } else if (from_kind == EcsFromOwned) {
        failure_info->reason = EcsMatchFromOwned;
        return ecs_type_has_owned_entity(world, type, component, true);

    } else if (from_kind == EcsFromShared) {
        failure_info->reason = EcsMatchFromSelf;
        return !ecs_type_has_owned_entity(world, type, component, true) &&
            ecs_type_has_owned_entity(world, type, component, false);

    } else if (from_kind == EcsFromContainer) {
        failure_info->reason = EcsMatchFromContainer;
        return ecs_components_contains_component(
            world, type, component, ECS_CHILDOF, NULL);

    } else if (from_kind == EcsFromEntity) {
        failure_info->reason = EcsMatchFromEntity;
        ecs_type_t type = ecs_get_type(world, source);
        return ecs_type_has_entity(world, type, component);
    } else {
        return true;
    }
}

/* Match table with system */
static
bool match_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    ecs_dbg_match_failure_t *failure_info)
{
    /* Prevent having to add if not null checks everywhere */
    ecs_dbg_match_failure_t tmp_failure_info;
    if (!failure_info) {
        failure_info = &tmp_failure_info;
    }

    failure_info->reason = EcsMatchOk;
    failure_info->column = 0;

    if (!query->needs_matching) {
        failure_info->reason = EcsMatchSystemIsATask;
        return false;
    }

    ecs_type_t type, table_type = table->type;

    /* Don't match disabled entities */
    if (!query->match_disabled && ecs_type_has_owned_entity(
        world, table_type, EEcsDisabled, true))
    {
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    /* Don't match prefab entities */
    if (!query->match_prefab && ecs_type_has_owned_entity(
        world, table_type, EEcsPrefab, true))
    {
        failure_info->reason = EcsMatchEntityIsPrefab;
        return false;
    }

    int32_t i, column_count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns);

    for (i = 0; i < column_count; i ++) {
        ecs_sig_column_t *elem = &columns[i];
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        failure_info->column = i + 1;     

        if (oper_kind == EcsOperAnd) {
            if (!match_column(
                world, table_type, from_kind, oper_kind, elem->is.component, 
                elem->source, failure_info)) 
            {
                return false;
            }

        } else if (oper_kind == EcsOperNot) {
            if (match_column(
                world, table_type, from_kind, oper_kind, elem->is.component, 
                elem->source, failure_info)) 
            {
                return false;
            }

        } else if (oper_kind == EcsOperOr) {
            type = elem->is.type;

            if (from_kind == EcsFromSelf) {
                if (!ecs_type_contains(
                    world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromSelf;
                    return false;
                }
            } else if (from_kind == EcsFromContainer) {
                if (!components_contains(
                    world, table_type, type, NULL, false))
                {
                    failure_info->reason = EcsMatchOrFromContainer;
                    return false;
                }
            }
        }
    }

    return true;
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int32_t i, count = ecs_sparse_count(world->stage.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(
            world->stage.tables, ecs_table_t, i);

        if (match_table(world, table, query, NULL)) {
            add_table(world, query, table);
        }
    }

    order_ranked_tables(world, query);
}

/** Get index of table in system's matched tables */
static
int32_t get_table_param_index(
    ecs_table_t *table,
    ecs_vector_t *tables)
{
    int32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables);

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            break;
        }
    }

    ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

    return i;
}

/** Check if a table was matched with the system */
static
int32_t table_matched(
    ecs_vector_t *tables,
    ecs_table_t *table)
{
    int32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables);

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            return i;
        }
    }

    return -1;
}

static
void resolve_cascade_container(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t table_data_index,
    ecs_type_t table_type)
{
    ecs_matched_table_t *table_data = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_data_index);
    
    ecs_assert(table_data->references != 0, ECS_INTERNAL_ERROR, NULL);

    /* Obtain reference index */
    int32_t *column_indices = table_data->columns;
    int32_t column = query->cascade_by - 1;
    int32_t ref_index = -column_indices[column] - 1;

    /* Obtain pointer to the reference data */
    ecs_reference_t *references = ecs_vector_first(table_data->references);
    ecs_reference_t *ref = &references[ref_index];
    ecs_assert(ref->component == get_cascade_component(query), 
        ECS_INTERNAL_ERROR, NULL);

    /* Resolve container entity */
    ecs_entity_t container = 0;
    ecs_components_contains_component(
        world, table_type, ref->component, ECS_CHILDOF, &container);    

    /* If container was found, update the reference */
    if (container) {
        references[ref_index].entity = container;
        ecs_get_cached_ptr_w_entity(
            world, &references[ref_index].cached_ptr, container, 
            ref->component);
    } else {
        references[ref_index].entity = 0;
    }
}

#define ELEM(ptr, size, index) ECS_OFFSET(ptr, size * index)

static
int32_t qsort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,    
    int32_t elem_size,
    int32_t lo,
    int32_t hi,
    ecs_compare_action_t compare)
{
repeat:
    {
        int32_t p = (hi + lo) / 2;
        void *pivot = ELEM(ptr, elem_size, p);
        int32_t i = lo - 1, j = hi;
        void *el;

        do {
            i ++;
            el = ELEM(ptr, elem_size, i);
        } while ( compare(entities[i], el, entities[p], pivot) < 0);

        do {
            j --;
            el = ELEM(ptr, elem_size, j);
        } while ( compare(entities[j], el, entities[p], pivot) > 0);

        if (i >= j) {
            return j;
        }

        ecs_table_swap(world, &world->stage, table, data, i, j);
        goto repeat;
    }
}

static
void sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t sort_on_component,
    int32_t column_index,
    ecs_compare_action_t compare)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data || !data->columns) {
        /* Nothing to sort */
        return;
    }

    int32_t count = ecs_table_data_count(data);
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities);

    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_column_t *column = &data->columns[column_index];
        ptr = ecs_vector_first(column->data);
        size = column->size;
    }

    int32_t p = qsort_table(
        world, table, data, entities, ptr, size, 0, count, compare);

    qsort_table(
        world, table, data, entities, ptr, size, 0, p, compare);        

    qsort_table(
        world, table, data, entities, ptr, size, p + 1, count, compare);
}

/* Helper struct for building sorted table ranges */
typedef struct sort_helper_t {
    ecs_matched_table_t *table;
    ecs_entity_t *entities;
    void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
} sort_helper_t;

static
void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    return ELEM(helper->ptr, helper->elem_size, helper->row);
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
void build_sorted_table_range(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t start,
    int32_t end)
{
    ecs_entity_t component = query->sort_on_component;
    ecs_compare_action_t compare = query->compare;

    /* Fetch data from all matched tables */
    ecs_matched_table_t *tables = ecs_vector_first(query->tables);
    sort_helper_t *helper = ecs_os_malloc((end - start) * sizeof(sort_helper_t));

    int i, to_sort = 0;
    for (i = start; i < end; i ++) {
        ecs_matched_table_t *table = &tables[i];
        ecs_data_t *data = ecs_table_get_data(world, table->table);
        if (!data || !data->columns || !ecs_table_count(table->table)) {
            continue;
        }

        int32_t index = ecs_type_index_of(table->table->type, component);
        if (index != -1) {
            ecs_column_t *column = &data->columns[index];
            helper[to_sort].ptr = ecs_vector_first(column->data);
            helper[to_sort].elem_size = column->size;
        }

        helper[to_sort].table = table;
        helper[to_sort].entities = ecs_vector_first(data->entities);
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table->table);
        to_sort ++;
    }

    ecs_table_range_t *cur = NULL;

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

            void *ptr1 = ptr_from_helper(&helper[min]);
            void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
            }
        }

        sort_helper_t *cur_helper = &helper[min];

        if (!cur || cur->table != cur_helper->table) {
            cur = ecs_vector_add(
                &query->table_ranges, ecs_table_range_t);
            ecs_assert(cur != NULL, ECS_INTERNAL_ERROR, NULL);
            cur->table = cur_helper->table;
            cur->start_row = cur_helper->row;
            cur->count = 1;
        } else {
            cur->count ++;
        }

        cur_helper->row ++;
    } while (proceed);

    ecs_os_free(helper);
}

static
void build_sorted_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    /* Clean previous sorted tables */
    ecs_vector_free(query->table_ranges);
    query->table_ranges = NULL;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables);
    ecs_matched_table_t *table = NULL;

    int32_t start = 0, rank = 0;
    for (i = 0; i < count; i ++) {
        table = &tables[i];
        if (rank != table->rank && start != i) {
            build_sorted_table_range(world, query, start, i);
            start = i;
            rank = table->rank;
        }
    }

    if (start != i) {
        build_sorted_table_range(world, query, start, i);
    }    
}

static
void sort_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_entity_t sort_on_component = query->sort_on_component;
    ecs_compare_action_t compare = query->compare;
    if (!compare) {
        return;
    }

    /* Iterate over active tables. Don't bother with inactive tables, since
     * they're empty */
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables);
    bool tables_sorted = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *m_table = &tables[i];
        ecs_table_t *table = m_table->table;

        /* If no monitor had been created for the table yet, create it now */
        bool is_dirty = false;
        if (!m_table->monitor) {
            m_table->monitor = ecs_table_get_monitor(table);

            /* A new table is always dirty */
            is_dirty = true;
        }

        int32_t *dirty_state = ecs_table_get_dirty_state(table);

        is_dirty |= dirty_state[0] != m_table->monitor[0];

        int32_t index = -1;
        if (sort_on_component) {
            /* Get index of sorted component. We only care if the component we're
            * sorting on has changed or if entities have been added / re(moved) */
            int32_t column_count = ecs_vector_count(table->type);
            index = ecs_type_index_of(table->type, sort_on_component);
            ecs_assert(index != -1, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(index < column_count, ECS_INTERNAL_ERROR, NULL);

            is_dirty |= dirty_state[index + 1] != m_table->monitor[index + 1];
        }      
        
        /* Check both if entities have moved (element 0) or if the component
         * we're sorting on has changed (index + 1) */
        if (is_dirty) {
            /* Sort the tables */
            sort_table(world, table, sort_on_component, index, compare);

            /* Sorting the table will make it dirty again, so update our monitor
             * after the sort */
            m_table->monitor[0] = dirty_state[0];
            m_table->monitor[index + 1] = dirty_state[index + 1];
            tables_sorted = true;
        }
    }

    if (tables_sorted) {
        build_sorted_tables(world, query);
    }
}

static
bool has_refs(
    ecs_sig_t *sig)
{
    int32_t i, count = ecs_vector_count(sig->columns);
    ecs_sig_column_t *columns = ecs_vector_first(sig->columns);

    for (i = 0; i < count; i ++) {
        ecs_sig_from_kind_t from_kind = columns[i].from_kind;

        if (columns[i].oper_kind == EcsOperNot && from_kind == EcsFromEmpty) {
            /* Special case: if oper kind is Not and the query contained a
             * shared expression, the expression is translated to FromId to
             * prevent resolving the ref */
            return true;
        } else if (from_kind != EcsFromSelf && from_kind != EcsFromEmpty) {
            /* If the component is not from the entity being iterated over, and
             * the column is not just passing an id, it must be a reference to
             * another entity. */
            return true;
        }
    }

    return false;
}

static
void process_signature(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int i, count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns);

    for (i = 0; i < count; i ++) {
        ecs_sig_column_t *column = &columns[i];
        ecs_sig_oper_kind_t op = column->oper_kind; 
        ecs_sig_from_kind_t from = column->from_kind; 

        if (!query->match_disabled) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsDisabled,
                * signal that disabled entities should be matched. By default,
                * disabled entities are not matched. */
                if (ecs_type_has_owned_entity(
                    world, column->is.type, ecs_entity(EcsDisabled), true))
                {
                    query->match_disabled = true;
                }         
            } else if (op == EcsOperAnd || op == EcsOperOptional) {
                if (column->is.component == ecs_entity(EcsDisabled)) {
                    query->match_disabled = true;
                }
            }
        }

        if (!query->match_prefab) {
            if (op == EcsOperOr) {
                /* If the signature explicitly indicates interest in EcsPrefab,
                * signal that disabled entities should be matched. By default,
                * prefab entities are not matched. */
                if (ecs_type_has_owned_entity(
                    world, column->is.type, ecs_entity(EcsPrefab), true))
                {
                    query->match_prefab = true;
                }            
            } else if (op == EcsOperAnd || op == EcsOperOptional) {
                if (column->is.component == ecs_entity(EcsPrefab)) {
                    query->match_prefab = true;
                }
            }
        }

        if (from == EcsFromSelf || 
            from == EcsFromOwned ||
            from == EcsFromShared ||
            from == EcsFromContainer) 
        {
            query->needs_matching = true;
        }

        if (from == EcsCascade) {
            query->cascade_by = i;
        }

        if (from == EcsFromEntity) {
            ecs_assert(column->source != 0, ECS_INTERNAL_ERROR, NULL);
            ecs_set_watch(world, &world->stage, column->source);
        }
    }

    query->has_refs = has_refs(&query->sig);
}


/* -- Private API -- */

void ecs_query_match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (match_table(world, table, query, NULL)) {
        add_table(world, query, table);
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
void ecs_query_activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active)
{
    ecs_vector_t *src_array, *dst_array;

    if (active) {
        src_array = query->inactive_tables;
        dst_array = query->tables;
    } else {
        src_array = query->tables;
        dst_array = query->inactive_tables;
    }

    int32_t i = get_table_param_index(table, src_array);
    int32_t src_count = ecs_vector_move_index(
        &dst_array, src_array, ecs_matched_table_t, i);

    if (active) {
        query->tables = dst_array;
    } else {
        query->inactive_tables = dst_array;
    }

    if (query->system) {
        int32_t dst_count = ecs_vector_count(dst_array);
        if (active) {
            if (dst_count == 1) {
                ecs_system_activate(world, query->system, true);
            }
        } else if (src_count == 0) {
            ecs_system_activate(world, query->system, false);
        }
    }

    order_ranked_tables(world, query);
}

/* Remove table */
static
void remove_table(
    ecs_vector_t *tables,
    int32_t index)
{
    ecs_vector_remove_index(tables, ecs_matched_table_t, index);
}

/* Rematch system with tables after a change happened to a container or prefab */
void ecs_rematch_query(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_sparse_t *tables = world->stage.tables;
    int32_t i, count = ecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        /* Is the system currently matched with the table? */
        ecs_table_t *table = ecs_sparse_get(tables, ecs_table_t, i);
        int32_t match = table_matched(query->tables, table);

        if (match_table(world, table, query, NULL)) {
            /* If the table matches, and it is not currently matched, add */
            if (match == -1) {
                if (table_matched(query->inactive_tables, table) == -1) {
                    add_table(world, query, table);
                }

            /* If table still matches and has cascade column, reevaluate the
                * sources of references. This may have changed in case 
                * components were added/removed to container entities */ 
            } else if (query->cascade_by) {
                resolve_cascade_container(
                    world, query, match, table->type);
            }
        } else {
            /* If table no longer matches, remove it */
            if (match != -1) {
                remove_table(query->tables, match);
            } else {
                /* Make sure the table is removed if it was inactive */
                match = table_matched(
                    query->inactive_tables, table);
                if (match != -1) {
                    remove_table(query->inactive_tables, match);
                }
            }
        }
    }

    order_ranked_tables(world, query);

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    if (query->system) {
        if (ecs_sig_check_constraints(world, &query->sig)) {
            ecs_remove(world, query->system, EcsDisabledIntern);
        } else {
            ecs_add(world, query->system, EcsDisabledIntern);
        }
    }
}

void ecs_query_set_monitor(
    ecs_world_t *world,
    ecs_query_t *query,
    bool is_monitor)
{
    ecs_assert(!is_monitor || query->system != 0, ECS_INTERNAL_ERROR, NULL);

    query->is_monitor = is_monitor;

    if (is_monitor) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            ecs_table_register_monitor(
                world, table->table, query->system, table_i);
        });
    }
}

/* -- Public API -- */

ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_sig_t *sig)
{
    ecs_query_t *result = ecs_sparse_add(world->queries, ecs_query_t);
    memset(result, 0, sizeof(ecs_query_t));
    result->world = world;
    result->sig = *sig;
    result->tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->inactive_tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->system = system;

    process_signature(world, result);

    if (system) {
        ecs_trace_1("query #[green]%s#[reset] created with expression #[red]%s", 
            ecs_get_name(world, system), result->sig.expr);
    } else {
        ecs_trace_1("query %p created with expression #[red]%s", 
            result, result->sig.expr);
    }

    ecs_trace_push();

    if (!result->needs_matching) {
        add_table(world, result, NULL);
    } else {
        match_tables(world, result);
    }

    if (result->cascade_by) {
        result->rank_on_component = result->cascade_by;
        result->rank_table = rank_by_depth;
    }

    ecs_trace_pop();

    /* Make sure application can't try to free sig resources */
    *sig = (ecs_sig_t){ 0 };

    return result;
}

ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *expr)
{
    ecs_sig_t sig = { 0 };
    ecs_sig_init(world, NULL, expr, &sig);
    return ecs_query_new_w_sig(world, 0, &sig);
}

void ecs_query_free(
    ecs_query_t *query)
{
    int32_t t;
    ecs_matched_table_t *tables = ecs_vector_first(query->inactive_tables);
    for (t = 0; t < ecs_vector_count(query->inactive_tables); t ++) {
        ecs_os_free(tables[t].columns);
        ecs_os_free(tables[t].components);
    }

    tables = ecs_vector_first(query->tables);
    for (t = 0; t < ecs_vector_count(query->tables); t ++) {
        ecs_os_free(tables[t].columns);
        ecs_os_free(tables[t].components);
    }

    ecs_vector_free(query->tables);
    ecs_vector_free(query->inactive_tables);
    ecs_sig_deinit(&query->sig);
}

/* Create query iterator */
ecs_query_iter_t ecs_query_iter(
    ecs_query_t *query,
    int32_t offset,
    int32_t limit)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);

    sort_tables(query->world, query);

    int32_t table_count;
    if (query->table_ranges) {
        table_count = ecs_vector_count(query->table_ranges);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    return (ecs_query_iter_t){
        .query = query,
        .offset = offset,
        .limit = limit,
        .remaining = limit,
        .index = 0,
        .rows = {
            .world = query->world,
            .query = query,
            .column_count = ecs_vector_count(query->sig.columns),
            .table_count = table_count,
            .inactive_table_count = ecs_vector_count(query->inactive_tables)
        }
    };
}

void ecs_query_set_rows(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_query_t *query,
    ecs_rows_t *rows,
    int32_t table_index,
    int32_t row,
    int32_t count)
{
    ecs_matched_table_t *table = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_index);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *world_table = table->table;
    ecs_data_t *table_data = ecs_table_get_staged_data(world, stage, world_table);
    ecs_entity_t *entity_buffer = ecs_vector_first(table_data->entities);            
    rows->entities = &entity_buffer[row];

    rows->world = world;
    rows->query = query;
    rows->column_count = ecs_vector_count(query->sig.columns);
    rows->table_count = 1;
    rows->inactive_table_count = 0;
    rows->table = world_table;
    rows->table_columns = table_data->columns;
    rows->columns = table->columns;
    rows->components = table->components;
    rows->references = ecs_vector_first(table->references);
    rows->offset = row;
    rows->count = count;
}


/* Return next table */
bool ecs_query_next(
    ecs_query_iter_t *iter)
{
    ecs_assert(iter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_query_t *query = iter->query;
    ecs_rows_t *rows = &iter->rows;
    ecs_world_t *world = rows->world;

    ecs_matched_table_t *tables = ecs_vector_first(query->tables);
    ecs_table_range_t *ranges = ecs_vector_first(query->table_ranges);

    ecs_assert(!ranges || query->compare, ECS_INTERNAL_ERROR, NULL);
    
    int32_t table_count = iter->rows.table_count;
    if (ranges) {
        table_count = ecs_vector_count(query->table_ranges);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_world_t *real_world = world;
    ecs_get_stage(&real_world);

    int32_t offset = iter->offset;
    int32_t limit = iter->limit;
    int32_t remaining = iter->remaining;
    int32_t prev_count = rows->count;
    bool offset_limit = (offset | limit) != 0;

    int i;
    for (i = iter->index; i < table_count; i ++) {
        ecs_matched_table_t *table;

        if (ranges) {
            table = ranges[i].table;
        } else {
            table = &tables[i];
        }

        ecs_table_t *world_table = table->table;
        ecs_data_t *table_data = NULL;
        int32_t first = 0, count = 0;

        if (world_table) {
            table_data = ecs_table_get_data(real_world, world_table);
            ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);
            rows->table_columns = table_data->columns;
            
            if (ranges) {
                first = ranges[i].start_row;
                count = ranges[i].count;
            } else {
                count = ecs_table_count(world_table);
            }
        }

        if (table_data) {
            if (offset_limit) {
                if (offset) {
                    if (offset > count) {
                        /* No entities to iterate in current table */
                        offset = iter->offset -= count;
                        continue;
                    } else {
                        first += offset;
                        count -= offset;
                        offset = iter->offset = 0;
                    }
                }

                if (remaining) {
                    if (remaining > count) {
                        remaining = iter->remaining -= count;
                    } else {
                        count = remaining;
                        remaining = iter->remaining = 0;
                    }
                } else if (limit) {
                    /* Limit hit: no more entities left to iterate */
                    return false;
                }
            }

            if (!count) {
                /* No entities to iterate in current table */
                continue;
            }

            ecs_entity_t *entity_buffer = ecs_vector_first(table_data->entities);            
            rows->entities = &entity_buffer[first];
            rows->offset = first;
            rows->count = count;
        }

        rows->table = world_table;
        rows->columns = table->columns;
        rows->components = table->components;
        rows->references = ecs_vector_first(table->references);
        rows->frame_offset += prev_count;

        /* Table is ready to be iterated, return rows struct */
        iter->index = ++ i;

        return true;
    }

    return false;
}

void ecs_query_sort(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_compare_action_t compare)
{
    ecs_assert(query->needs_matching, ECS_INVALID_PARAMETER, NULL);

    query->sort_on_component = sort_component;
    query->compare = compare;

    ecs_vector_free(query->table_ranges);
    query->table_ranges = NULL;

    sort_tables(world, query);    

    if (!query->table_ranges) {
        build_sorted_tables(world, query);
    }
}

void ecs_query_sort_types(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_rank_type_action_t rank_table)
{
    ecs_assert(query->needs_matching, ECS_INVALID_PARAMETER, NULL);

    query->rank_on_component = sort_component;
    query->rank_table = rank_table;

    rank_tables(world, query);

    order_ranked_tables(world, query);

    build_sorted_tables(world, query);
}

/* -- Debug functionality -- */

bool ecs_dbg_match_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t system,
    ecs_dbg_match_failure_t *failure_info_out)
{
    ecs_dbg_entity_t dbg;
    ecs_dbg_entity(world, entity, &dbg);

    const EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        failure_info_out->reason = EcsMatchNotASystem;
        failure_info_out->column = -1;
        return false;
    }

    return match_table(
        world, dbg.table, system_data->query, failure_info_out);
}
