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

            if (record->type) {
                ecs_entity_t component = ecs_type_contains(
                    world, record->type, type, match_all, true);

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
ecs_entity_t ecs_get_entity_for_component(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    ecs_entity_t component)
{
    if (entity) {
        ecs_record_t *record = ecs_eis_get(&world->stage, entity);
        ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        type = record->type;
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
    } else {
        /* If no table is provided to function, this is a system that contains
         * no columns that require table matching. In this case, the system will
         * only have one "dummy" table that caches data from the system columns.
         * Always add this dummy table to the list of active tables, since it
         * would never get activated otherwise. */
        table_data = ecs_vector_add(
            &query->tables, ecs_matched_table_t);
    }

    table_data->table = table;
    table_data->references = NULL;
    table_data->columns = NULL;
    table_data->components = NULL;

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
                    EcsComponent *data = ecs_get_ptr(
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
            if (!ecs_type_has_entity_intern(
                world, table_type, component, true))
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
                EcsComponent *component_data = ecs_get_ptr(
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
                    e = ecs_get_entity_for_component(
                        world, entity, table_type, component);
                }

                if (from != EcsCascade) {
                    ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
                }
                
                ref->entity = e;
                ref->component = component;

                if (component_data->size) {
                    if (e != ECS_INVALID_ENTITY) {
                        ref->cached_ptr = _ecs_get_ptr(world, e, component);
                        ecs_set_watch(world, &world->stage, e);                     
                    } else {
                        ref->cached_ptr = NULL;
                    }

                    /* Negative number indicates ref instead of offset to ecs_data */
                    query->sig.has_refs = true;
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

    if (!query->sig.needs_tables) {
        failure_info->reason = EcsMatchSystemIsATask;
        return false;
    }

    ecs_type_t type, table_type = table->type;

    if (!query->sig.match_disabled && ecs_type_has_entity_intern(
        world, table_type, EEcsDisabled, false))
    {
        /* Don't match disabled entities */
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    if (!query->sig.match_prefab && ecs_type_has_entity_intern(
        world, table_type, EEcsPrefab, false))
    {
        /* Don't match prefab entities */
        failure_info->reason = EcsMatchEntityIsPrefab;
        return false;
    }

    /* Only do quick checks if failure info is not requested. These checks do
     * not provide information about which column caused the match failure. */
    if (failure_info == &tmp_failure_info) {
        /* Test if table has SELF columns in either owned or inherited components */
        type = query->sig.and_from_self;
        if (type && !ecs_type_contains(
            world, table_type, type, true, true))
        {
            return false;
        }

        /* Test if table has OWNED columns in owned components */
        type = query->sig.and_from_owned;
        if (type && !ecs_type_contains(
            world, table_type, type, true, false))
        {
            return false;
        }  

        /* Test if table has SHARED columns in shared components */
        type = query->sig.and_from_shared;
        if (type && ecs_type_contains(
            world, table_type, type, true, false))
        {
            /* If table has owned components that override the SHARED component, the
            * table won't match. */
            return false;
        } else if (type && !ecs_type_contains(
            world, table_type, type, true, true))
        {
            /* If the table does not have owned components, ensure that a SHARED
            * component can be found in prefabs. If not, the table doesn't match. */
            return false;
        }
    }

    int32_t i, column_count = ecs_vector_count(query->sig.columns);
    ecs_sig_column_t *buffer = ecs_vector_first(query->sig.columns);

    for (i = 0; i < column_count; i ++) {
        ecs_sig_column_t *elem = &buffer[i];
        ecs_sig_from_kind_t from_kind = elem->from_kind;
        ecs_sig_oper_kind_t oper_kind = elem->oper_kind;

        if (oper_kind == EcsOperAnd) {
            if (from_kind == EcsFromSelf) {
                if (!ecs_type_has_entity_intern(
                        world, table_type, elem->is.component, true))
                {
                    failure_info->reason = EcsMatchFromSelf;
                    failure_info->column = i + 1;
                    return false;
                }
            } else if (from_kind == EcsFromOwned) {
                if (!ecs_type_has_entity_intern(
                        world, table_type, elem->is.component, false))
                {
                    failure_info->reason = EcsMatchFromOwned;
                    failure_info->column = i + 1;
                    return false;
                }                
            } else if (from_kind == EcsFromShared) {
                if (ecs_type_has_entity_intern(
                        world, table_type, elem->is.component, false))
                {
                    failure_info->reason = EcsMatchFromSelf;
                    failure_info->column = i + 1;
                    return false;
                } else
                if (!ecs_type_has_entity_intern(
                    world, table_type, elem->is.component, true))
                {
                    failure_info->reason = EcsMatchFromSelf;
                    failure_info->column = i + 1;
                    return false;                    
                }
            } else if (from_kind == EcsFromContainer) {
                if (!ecs_components_contains_component(
                    world, table_type, elem->is.component, ECS_CHILDOF, NULL))
                {
                    failure_info->reason = EcsMatchFromContainer;
                    failure_info->column = i + 1;
                    return false;
                }
            } else if (from_kind == EcsFromEntity) {
                ecs_type_t type = ecs_get_type(world, elem->source);
                if (!ecs_type_has_entity(world, type, elem->is.component)) {
                    failure_info->reason = EcsMatchFromEntity;
                    failure_info->column = i + 1;
                    return false;
                }
            }
        } else if (oper_kind == EcsOperOr) {
            type = elem->is.type;
            if (from_kind == EcsFromSelf) {
                if (!ecs_type_contains(
                    world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromSelf;
                    failure_info->column = i + 1;
                    return false;
                }
            } else if (from_kind == EcsFromContainer) {
                if (!components_contains(
                    world, table_type, type, NULL, false))
                {
                    failure_info->reason = EcsMatchOrFromContainer;
                    failure_info->column = i + 1;
                    return false;
                }
            }
        } else if (oper_kind == EcsOperNot) {
            if (from_kind == EcsFromEntity) {
                /* Not a table column constraint, is verified by other logic */
            }
        }
    }

    type = query->sig.not_from_self;
    if (type && ecs_type_contains(world, table_type, type, false, true))
    {
        failure_info->reason = EcsMatchNotFromSelf;
        return false;
    }

    type = query->sig.not_from_owned;
    if (type && ecs_type_contains(world, table_type, type, false, false))
    {
        failure_info->reason = EcsMatchNotFromOwned;
        return false;
    }

    type = query->sig.not_from_shared;
    if (type && !ecs_type_contains(world, table_type, type, false, false))
    {
        if (ecs_type_contains(world, table_type, type, false, true)) {
            failure_info->reason = EcsMatchNotFromShared;
            return false;
        }
    }        

    type = query->sig.not_from_container;
    if (type && components_contains(
        world, table_type, type, NULL, false))
    {
        failure_info->reason = EcsMatchNotFromContainer;
        return false;
    }

    return true;
}

static
ecs_entity_t get_cascade_component(
    ecs_query_t *query)
{
    ecs_sig_column_t *column = ecs_vector_first(query->sig.columns);
    return column[query->sig.cascade_by - 1].is.component;
}

static
int table_compare(
    const void *t1,
    const void *t2)
{
    const ecs_matched_table_t *table_1 = t1;
    const ecs_matched_table_t *table_2 = t2;

    return table_1->depth - table_2->depth;
}

static
void order_cascade_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_entity_t cascade_component = get_cascade_component(query);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = ecs_vector_get(
            query->tables, ecs_matched_table_t, i);

        ecs_type_t type = table_data->table->type;
        table_data->depth = ecs_type_container_depth(
            world, type, cascade_component);
    }

    ecs_vector_sort(query->tables, ecs_matched_table_t, table_compare);
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

    if (query->sig.cascade_by) {
        order_cascade_tables(world, query);
    }
}

/** Match new table against system (table is created after system) */
void ecs_query_notify_of_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (match_table(world, table, query, NULL)) {
        add_table(world, query, table);
    }
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
    int32_t column = query->sig.cascade_by - 1;
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
        references[ref_index].cached_ptr = ecs_get_ptr_intern(
            world, &world->stage, container, ref->component, false, true);
    } else {
        references[ref_index].entity = ECS_INVALID_ENTITY;
        references[ref_index].cached_ptr = NULL;
    }
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
        EcsColSystem *system_data = ecs_get_ptr(
            world, query->system, EcsColSystem);
        ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);   

        ecs_system_kind_t kind = system_data->base.kind;

        if (active) {
            int32_t dst_count = ecs_vector_count(dst_array);
            if (dst_count == 1 && system_data->base.enabled) {
                ecs_world_activate_system(
                    world, query->system, kind, true);
            }
        } else {
            if (src_count == 0) {
                ecs_world_activate_system(
                    world, query->system, kind, false);
            }
        }
    }
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
            } else if (query->sig.cascade_by) {
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

    /* If the system has a CASCADE column and modifications were made, 
        * reorder the system tables so that the depth order is preserved */
    if (query->sig.cascade_by) {
        order_cascade_tables(world, query);
    }

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    if (query->system) {
        EcsColSystem* system_data = ecs_get_ptr(world, query->system, EcsColSystem);
        bool enable = ecs_sig_check_constraints(world, &query->sig);
        ecs_enable_intern(world, query->system, (EcsSystem*)system_data, enable, false);
    }
}

/** Revalidate references after a realloc occurred in a table */
void ecs_revalidate_query_refs(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (!query->sig.has_refs) {
        return;
    }

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *table_data = ecs_vector_first(query->tables);

    for (i = 0; i < count; i ++) {
        if (!table_data[i].references) {
            continue;
        }

        int32_t r, ref_count = ecs_vector_count(table_data[i].references);
        ecs_reference_t *refs = ecs_vector_first(table_data[i].references);

        for (r = 0; r < ref_count; r ++) {
            ecs_reference_t ref = refs[r];
            refs[r].cached_ptr = ecs_get_ptr_intern(
                world, &world->stage, ref.entity, ref.component, false, true);
        }            
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

    if (!result->sig.needs_tables) {
        add_table(world, result, NULL);
    } else {
        match_tables(world, result);
    }

    return result;
}

ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *expr)
{
    ecs_sig_t sig = {0};
    ecs_sig_init(world, NULL, expr, &sig);
    return ecs_query_new_w_sig(world, 0, &sig);
}

void ecs_query_free(
    ecs_query_t *query)
{
    int32_t t;
    ecs_matched_table_t *tables = ecs_vector_first(query->inactive_tables);
    for (t = 0; t < ecs_vector_count(query->inactive_tables); t ++) {
        ecs_os_free(tables[t].components);
    }

    tables = ecs_vector_first(query->tables);
    for (t = 0; t < ecs_vector_count(query->tables); t ++) {
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
            .table_count = ecs_vector_count(query->tables),
            .inactive_table_count = ecs_vector_count(query->inactive_tables)
        }
    };
}

/* Return next table */
bool ecs_query_next(
    ecs_query_iter_t *iter)
{
    ecs_assert(iter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_query_t *query = iter->query;
    ecs_rows_t *rows = &iter->rows;
    ecs_world_t *world = rows->world;
    int32_t table_count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables);

    ecs_world_t *real_world = world;
    ecs_get_stage(&real_world);

    int32_t offset = iter->offset;
    int32_t limit = iter->limit;
    int32_t remaining = iter->remaining;
    int32_t prev_count = rows->count;
    bool offset_limit = (offset | limit) != 0;

    int i;
    for (i = iter->index; i < table_count; i ++) {
        ecs_matched_table_t *table = &tables[i];
        ecs_table_t *world_table = table->table;
        ecs_data_t *table_data = NULL;

        if (world_table) {
            table_data = ecs_table_get_data(real_world, world_table);
            ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);
            rows->table_columns = table_data->columns;
        }

        if (table_data) {
            int32_t first = 0, count = ecs_table_count(world_table);

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

/* -- Debug functionality -- */

bool ecs_dbg_match_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t system,
    ecs_dbg_match_failure_t *failure_info_out)
{
    ecs_dbg_entity_t dbg;
    ecs_dbg_entity(world, entity, &dbg);

    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        failure_info_out->reason = EcsMatchNotASystem;
        failure_info_out->column = -1;
        return false;
    }

    return match_table(
        world, dbg.table, system_data->query, failure_info_out);
}
