#include "flecs_private.h"

const ecs_vector_params_t matched_table_params = {
    .element_size = sizeof(ecs_matched_table_t)
};

const ecs_vector_params_t system_column_params = {
    .element_size = sizeof(ecs_system_column_t)
};

const ecs_vector_params_t reference_params = {
    .element_size = sizeof(ecs_reference_t)
};

const ecs_vector_params_t matched_column_params = {
    .element_size = sizeof(uint32_t)
};

static
ecs_entity_t components_contains(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_type_t type,
    ecs_entity_t *entity_out,
    bool match_all)
{
    uint32_t i, count = ecs_vector_count(table_type);
    ecs_entity_t *array = ecs_vector_first(table_type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t entity = array[i];

        if (entity & ECS_CHILDOF) {
            entity &= ECS_ENTITY_MASK;
            ecs_row_t *row = ecs_map_get_ptr(world->main_stage.entity_index, entity);
            ecs_assert(row != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_entity_t component = ecs_type_contains(
                world, row->type, type, match_all, true);

            if (component != 0) {
                if (entity_out) *entity_out = entity;
                return component;
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
        ecs_row_t *row = ecs_map_get_ptr(world->main_stage.entity_index, entity);
        ecs_assert(row != NULL, ECS_INTERNAL_ERROR, NULL);
        type = row->type;
    }

    ecs_entity_t *array = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

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
    ecs_entity_t system,
    EcsColSystem *system_data,
    ecs_table_t *table)
{
    ecs_matched_table_t *table_data;
    ecs_type_t table_type = table ? table->type : NULL;
    uint32_t column_count = ecs_vector_count(system_data->base.columns);

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */
    if (table) {
        table_data = ecs_vector_add(
            &system_data->inactive_tables, &matched_table_params);
    } else {
        /* If no table is provided to function, this is a system that contains
         * no columns that require table matching. In this case, the system will
         * only have one "dummy" table that caches data from the system columns.
         * Always add this dummy table to the list of active tables, since it
         * would never get activated otherwise. */
        table_data = ecs_vector_add(
            &system_data->tables, &matched_table_params);
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
    ecs_system_column_t *columns = ecs_vector_first(system_data->base.columns);
    uint32_t c;

    for (c = 0; c < column_count; c ++) {
        ecs_system_column_t *column = &columns[c];
        ecs_entity_t entity = 0, component = 0;
        ecs_system_expr_elem_kind_t kind = column->kind;
        ecs_system_expr_oper_kind_t oper_kind = column->oper_kind;

        /* Column that retrieves data from self or a fixed entity */
        if (kind == EcsFromSelf || kind == EcsFromEntity || 
            kind == EcsFromOwned || kind == EcsFromShared) 
        {
            if (oper_kind == EcsOperAnd || oper_kind == EcsOperNot) {
                component = column->is.component;
                table_data->columns[c] = 0;
            } else if (oper_kind == EcsOperOptional) {
                component = column->is.component;
            } else if (oper_kind == EcsOperOr) {
                component = ecs_type_contains(
                    world, table_type, column->is.type, 
                    false, true);
            }

            if (kind == EcsFromEntity) {
                entity = column->source;
            }

        /* Column that just passes a handle to the system (no data) */
        } else if (kind == EcsFromEmpty) {
            component = column->is.component;
            table_data->columns[c] = 0;

        /* Column that retrieves data from a dynamic entity */
        } else if (kind == EcsFromContainer || kind == EcsCascade) {
            if (oper_kind == EcsOperAnd ||
                oper_kind == EcsOperOptional)
            {
                component = column->is.component;

                ecs_components_contains_component(
                    world, table_type, component, ECS_CHILDOF, &entity);

            } else if (oper_kind == EcsOperOr) {
                component = components_contains(
                    world,
                    table_type,
                    column->is.type,
                    &entity,
                    false);
            }

        /* Column that retrieves data from a system */
        } else if (kind == EcsFromSystem) {
            if (oper_kind == EcsOperAnd) {
                component = column->is.component;
            }

            entity = system;
        }

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromContainer) and is not just a handle */
        if (!entity && kind != EcsFromEmpty) {
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

        if (oper_kind == EcsOperOptional) {
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
         * If the column kind is FromSingleton, the entity will be 0, but still
         * a reference needs to be added to the singleton component.
         * 
         * If the column kind is Cascade, there may not be an entity in case the
         * current table contains root entities. In that case, still add a
         * reference field. The application can, after the table has matched,
         * change the set of components, so that this column will turn into a
         * reference. Having the reference already linked to the system table
         * makes changing this administation easier when the change happens.
         * */
        if (entity || table_data->columns[c] == -1 || kind == EcsCascade) {
            if (ecs_has(world, component, EcsComponent)) {
                EcsComponent *component_data = ecs_get_ptr(
                        world, component, EcsComponent);
                
                if (component_data->size) {
                    ecs_entity_t e;
                    ecs_reference_t *ref = ecs_vector_add(
                            &table_data->references, &reference_params);

                    /* Find the entity for the component */
                    if (kind == EcsFromEntity) {
                        e = entity;
                    } else if (kind == EcsCascade) {
                        e = entity;
                    } else if (kind == EcsFromSystem) {
                        e = entity;
                    } else {
                        e = ecs_get_entity_for_component(
                            world, entity, table_type, component);
                    }

                    if (kind != EcsCascade) {
                        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
                    }
                    
                    ref->entity = e;
                    ref->component = component;
                    
                    if (e != ECS_INVALID_ENTITY) {
                        ecs_entity_info_t info = {.entity = e};

                        ref->cached_ptr = ecs_get_ptr_intern(
                            world, 
                            &world->main_stage,
                            &info,
                            component,
                            false,
                            true);

                        ecs_set_watch(world, &world->main_stage, e);                     
                    } else {
                        ref->cached_ptr = NULL;
                    }

                    /* Negative number indicates ref instead of offset to ecs_data */
                    table_data->columns[c] = -ecs_vector_count(table_data->references);
                    system_data->base.has_refs = true;
                }
            }
        }

        /* component_data index is not offset by anything */
        table_data->components[c] = component;
    }

    if (table) {
        ecs_table_register_system(world, table, system);
    }
}

/* Remove table */
static
void remove_table(
    EcsColSystem *system_data,
    ecs_vector_t *tables,
    int32_t index)
{
    (void)system_data;
    ecs_vector_remove_index(tables, &matched_table_params, index);
}

/* Match table with system */
static
bool match_table(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t system,
    EcsColSystem *system_data,
    ecs_dbg_match_failure_t *failure_info)
{
    (void)system; /* useful for debugging */

    /* Prevent having to add if not null checks everywhere */
    ecs_dbg_match_failure_t tmp_failure_info;
    if (!failure_info) {
        failure_info = &tmp_failure_info;
    }

    failure_info->reason = EcsMatchOk;
    failure_info->column = 0;

    if (!system_data->base.needs_tables) {
        failure_info->reason = EcsMatchSystemIsATask;
        return false;
    }

    ecs_type_t type, table_type = table->type;

    if (!system_data->base.match_disabled && ecs_type_has_entity_intern(
        world, table_type, EEcsDisabled, false))
    {
        /* Don't match disabled entities */
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    if (!system_data->base.match_prefab && ecs_type_has_entity_intern(
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
        type = system_data->base.and_from_self;
        if (type && !ecs_type_contains(
            world, table_type, type, true, true))
        {
            return false;
        }

        /* Test if table has OWNED columns in owned components */
        type = system_data->base.and_from_owned;
        if (type && !ecs_type_contains(
            world, table_type, type, true, false))
        {
            return false;
        }  

        /* Test if table has SHARED columns in shared components */
        type = system_data->base.and_from_shared;
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

    uint32_t i, column_count = ecs_vector_count(system_data->base.columns);
    ecs_system_column_t *buffer = ecs_vector_first(system_data->base.columns);

    for (i = 0; i < column_count; i ++) {
        ecs_system_column_t *elem = &buffer[i];
        ecs_system_expr_elem_kind_t elem_kind = elem->kind;
        ecs_system_expr_oper_kind_t oper_kind = elem->oper_kind;

        if (oper_kind == EcsOperAnd) {
            if (elem_kind == EcsFromSelf) {
                if (!ecs_type_has_entity_intern(
                        world, table_type, elem->is.component, true))
                {
                    failure_info->reason = EcsMatchFromSelf;
                    failure_info->column = i + 1;
                    return false;
                }
            } else if (elem_kind == EcsFromOwned) {
                if (!ecs_type_has_entity_intern(
                        world, table_type, elem->is.component, false))
                {
                    failure_info->reason = EcsMatchFromOwned;
                    failure_info->column = i + 1;
                    return false;
                }                
            } else if (elem_kind == EcsFromShared) {
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
            } else if (elem_kind == EcsFromContainer) {
                if (!ecs_components_contains_component(
                    world, table_type, elem->is.component, ECS_CHILDOF, NULL))
                {
                    failure_info->reason = EcsMatchFromContainer;
                    failure_info->column = i + 1;
                    return false;
                }
            } else if (elem_kind == EcsFromEntity) {
                ecs_type_t type = ecs_get_type(world, elem->source);
                if (!ecs_type_has_entity(world, type, elem->is.component)) {
                    failure_info->reason = EcsMatchFromEntity;
                    failure_info->column = i + 1;
                    return false;
                }
            }
        } else if (oper_kind == EcsOperOr) {
            type = elem->is.type;
            if (elem_kind == EcsFromSelf) {
                if (!ecs_type_contains(
                    world, table_type, type, false, true))
                {
                    failure_info->reason = EcsMatchOrFromSelf;
                    failure_info->column = i + 1;
                    return false;
                }
            } else if (elem_kind == EcsFromContainer) {
                if (!components_contains(
                    world, table_type, type, NULL, false))
                {
                    failure_info->reason = EcsMatchOrFromContainer;
                    failure_info->column = i + 1;
                    return false;
                }
            }
        } else if (oper_kind == EcsOperNot) {
            if (elem_kind == EcsFromEntity) {
                /* Not a table column constraint, is verified by other logic */
            }
        }
    }

    type = system_data->base.not_from_self;
    if (type && ecs_type_contains(world, table_type, type, false, true))
    {
        failure_info->reason = EcsMatchNotFromSelf;
        return false;
    }

    type = system_data->base.not_from_owned;
    if (type && ecs_type_contains(world, table_type, type, false, false))
    {
        failure_info->reason = EcsMatchNotFromOwned;
        return false;
    }

    type = system_data->base.not_from_shared;
    if (type && !ecs_type_contains(world, table_type, type, false, false))
    {
        if (ecs_type_contains(world, table_type, type, false, true)) {
            failure_info->reason = EcsMatchNotFromShared;
            return false;
        }
    }        

    type = system_data->base.not_from_component;
    if (type && components_contains(
        world, table_type, type, NULL, false))
    {
        failure_info->reason = EcsMatchNotFromContainer;
        return false;
    }

    return true;
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
ecs_entity_t get_cascade_component(
    EcsColSystem *system_data)
{
    ecs_system_column_t *column = ecs_vector_first(system_data->base.columns);
    return column[system_data->base.cascade_by - 1].is.component;
}

static
void order_cascade_tables(
    ecs_world_t *world,
    EcsColSystem *system_data)
{
    uint32_t i, count = ecs_vector_count(system_data->tables);
    ecs_entity_t cascade_component = get_cascade_component(system_data);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = ecs_vector_get(
            system_data->tables, &matched_table_params, i);

        ecs_type_t type = table_data->table->type;
        table_data->depth = ecs_type_container_depth(
            world, type, cascade_component);
    }

    ecs_vector_sort(system_data->tables, &matched_table_params, table_compare);
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data)
{
    uint32_t i, count = ecs_chunked_count(world->main_stage.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_chunked_get(
            world->main_stage.tables, ecs_table_t, i);

        if (match_table(world, table, system, system_data, NULL)) {
            add_table(world, system, system_data, table);
        }
    }

    if (system_data->base.cascade_by) {
        order_cascade_tables(world, system_data);
    }
}

/** Check if a table was matched with the system */
static
int32_t table_matched(
    EcsColSystem *system_data,
    ecs_vector_t *tables,
    ecs_table_t *table)
{
    uint32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables);
    (void)system_data;

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
    EcsColSystem *system_data,
    int32_t table_data_index,
    ecs_type_t table_type)
{
    ecs_matched_table_t *table_data = ecs_vector_get(
        system_data->tables, &matched_table_params, table_data_index);
    
    ecs_assert(table_data->references != 0, ECS_INTERNAL_ERROR, NULL);

    /* Obtain reference index */
    int32_t *column_indices = table_data->columns;
    int32_t column = system_data->base.cascade_by - 1;
    int32_t ref_index = -column_indices[column] - 1;

    /* Obtain pointer to the reference data */
    ecs_reference_t *references = ecs_vector_first(table_data->references);
    ecs_reference_t *ref = &references[ref_index];
    ecs_assert(ref->component == get_cascade_component(system_data), 
        ECS_INTERNAL_ERROR, NULL);

    /* Resolve container entity */
    ecs_entity_t container = 0;
    ecs_components_contains_component(
        world, table_type, ref->component, ECS_CHILDOF, &container);        

    /* If container was found, update the reference */
    if (container) {
        ecs_entity_info_t info = {.entity = container};
        references[ref_index].entity = container;
        references[ref_index].cached_ptr = ecs_get_ptr_intern(
            world, &world->main_stage, &info, ref->component, false, true);
    } else {
        references[ref_index].entity = ECS_INVALID_ENTITY;
        references[ref_index].cached_ptr = NULL;
    }
}

/* -- Private API -- */

/* Rematch system with tables after a change happened to a container or prefab */
void ecs_rematch_system(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, 0);

    ecs_chunked_t *tables = world->main_stage.tables;
    uint32_t i, count = ecs_chunked_count(tables);

    for (i = 0; i < count; i ++) {
        /* Is the system currently matched with the table? */
        ecs_table_t *table = ecs_chunked_get(tables, ecs_table_t, i);
        int32_t match = table_matched(system_data, system_data->tables, table);

        if (match_table(world, table, system, system_data, NULL)) {
            /* If the table matches, and it is not currently matched, add */
            if (match == -1) {
                if (table_matched(system_data, system_data->inactive_tables, table) == -1) {
                    add_table(world, system, system_data, table);
                }

            /* If table still matches and has cascade column, reevaluate the
                * sources of references. This may have changed in case 
                * components were added/removed to container entities */ 
            } else if (system_data->base.cascade_by) {
                resolve_cascade_container(
                    world, system_data, match, table->type);
            }
        } else {
            /* If table no longer matches, remove it */
            if (match != -1) {
                remove_table(system_data, system_data->tables, match);
            } else {
                /* Make sure the table is removed if it was inactive */
                match = table_matched(
                    system_data, system_data->inactive_tables, table);
                if (match != -1) {
                    remove_table(
                        system_data, system_data->inactive_tables, match);
                }
            }
        }
    }

    /* If the system has a CASCADE column and modifications were made, 
        * reorder the system tables so that the depth order is preserved */
    if (system_data->base.cascade_by) {
        order_cascade_tables(world, system_data);
    }

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    ecs_enable(world, system, 
        ecs_check_column_constraints(world, (EcsSystem*)system_data));
}

/** Revalidate references after a realloc occurred in a table */
void ecs_revalidate_system_refs(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, 0);

    if (!system_data->base.has_refs) {
        return;
    }

    uint32_t i, count = ecs_vector_count(system_data->tables);
    ecs_matched_table_t *table_data = ecs_vector_first(system_data->tables);

    for (i = 0; i < count; i ++) {
        if (!table_data[i].references) {
            continue;
        }

        uint32_t r, ref_count = ecs_vector_count(table_data[i].references);
        ecs_reference_t *refs = ecs_vector_first(table_data[i].references);

        for (r = 0; r < ref_count; r ++) {
            ecs_reference_t ref = refs[r];
            ecs_entity_info_t info = {.entity = ref.entity};
            refs[r].cached_ptr = ecs_get_ptr_intern(
                world, &world->main_stage, &info, ref.component, false, true);
        }            
    }
}

/** Match new table against system (table is created after system) */
void ecs_col_system_notify_of_table(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_table_t *table)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    assert(system_data != NULL);

    if (match_table(world, table, system, system_data, NULL)) {
        add_table(world, system, system_data, table);
    }
}

/** Get index of table in system's matched tables */
static
int32_t get_table_param_index(
    ecs_world_t *world,
    EcsColSystem *system_data,
    ecs_table_t *table,
    ecs_vector_t *tables)
{
    uint32_t i, count = ecs_vector_count(tables);
    ecs_matched_table_t *table_data = ecs_vector_first(tables);
    (void)world;
    (void)system_data;

    for (i = 0; i < count; i ++) {
        if (table_data[i].table == table) {
            break;
        }
    }

    ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

    return i;
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
void ecs_system_activate_table(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_table_t *table,
    bool active)
{
    ecs_vector_t *src_array, *dst_array;

    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);
    
    EcsSystemKind kind = system_data->base.kind;

    if (active) {
        src_array = system_data->inactive_tables;
        dst_array = system_data->tables;
    } else {
        src_array = system_data->tables;
        dst_array = system_data->inactive_tables;
    }

    int32_t i = get_table_param_index(world, system_data, table, src_array);

    uint32_t src_count = ecs_vector_move_index(
        &dst_array, src_array, &matched_table_params, i);

    if (active) {
        uint32_t dst_count = ecs_vector_count(dst_array);
        if (dst_count == 1 && system_data->base.enabled) {
            ecs_world_activate_system(
                world, system, kind, true);
        }
        system_data->tables = dst_array;
    } else {
        if (src_count == 0) {
            ecs_world_activate_system(
                world, system, kind, false);
        }
        system_data->inactive_tables = dst_array;
    }
}

ecs_entity_t ecs_new_col_system(
    ecs_world_t *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    ecs_system_action_t action)
{
    uint32_t count = ecs_columns_count(sig);

    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = _ecs_new(world, world->t_col_system);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId);
    *id_data = id;

    EcsColSystem *system_data = ecs_get_ptr(world, result, EcsColSystem);
    memset(system_data, 0, sizeof(EcsColSystem));
    system_data->base.action = action;
    system_data->base.enabled = true;
    system_data->base.signature = ecs_os_strdup(sig);
    system_data->base.time_spent = 0;
    system_data->base.columns = ecs_vector_new(&system_column_params, count);
    system_data->base.invoke_count = 0;
    system_data->base.kind = kind;
    system_data->base.cascade_by = 0;
    system_data->base.has_refs = false;
    system_data->base.needs_tables = ecs_needs_tables(world, sig, id);

    system_data->column_params.element_size = sizeof(int32_t) * (count);
    system_data->ref_params.element_size = sizeof(ecs_reference_t) * count;
    system_data->component_params.element_size = sizeof(ecs_entity_t) * count;
    system_data->period = 0;
    system_data->entity = result;

    system_data->tables = ecs_vector_new(
        &matched_table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->inactive_tables = ecs_vector_new(
        &matched_table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);

    ecs_parse_component_expr(
        world, sig, ecs_parse_signature_action, id, system_data);

    ecs_system_compute_and_families(world, &system_data->base);

    ecs_system_init_base(world, &system_data->base);

    if (system_data->base.needs_tables) {
        match_tables(world, result, system_data);
    } else {
        /* If this system does not match with tables, for example, because it
         * does not have any SELF columns, add a single "matched" table that
         * caches the data for the columns, but does not have a reference to an
         * actual table. */
        add_table(world, result, system_data, NULL /* table is NULL */);
    }

    ecs_entity_t *elem = NULL;

    if (!ecs_vector_count(system_data->tables)) {
        elem = ecs_vector_add(&world->inactive_systems, &handle_arr_params);
    } else {
        if (kind == EcsOnUpdate) {
            elem = ecs_vector_add(&world->on_update_systems, &handle_arr_params);
        } else if (kind == EcsOnValidate) {
            elem = ecs_vector_add(&world->on_validate_systems, &handle_arr_params);            
        } else if (kind == EcsPreUpdate) {
            elem = ecs_vector_add(&world->pre_update_systems, &handle_arr_params);
        } else if (kind == EcsPostUpdate) {
            elem = ecs_vector_add(&world->post_update_systems, &handle_arr_params);
        } else if (kind == EcsOnLoad) {
            elem = ecs_vector_add(&world->on_load_systems, &handle_arr_params);
        } else if (kind == EcsPostLoad) {
            elem = ecs_vector_add(&world->post_load_systems, &handle_arr_params);            
        } else if (kind == EcsPreStore) {
            elem = ecs_vector_add(&world->pre_store_systems, &handle_arr_params);
        } else if (kind == EcsOnStore) {
            elem = ecs_vector_add(&world->on_store_systems, &handle_arr_params);
        } else if (kind == EcsManual) {
            elem = ecs_vector_add(&world->manual_systems, &handle_arr_params);
        }

        /* Parameter checking happened before this, kind must have been one of
         * the checked values. */
        ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    *elem = result;

    return result;
}

/* -- Public API -- */

static
bool should_run(
    EcsColSystem *system_data,
    float period,
    float delta_time)
{
    float time_passed = system_data->time_passed + delta_time;

    if (time_passed >= period) {
        time_passed -= period;
        if (time_passed > period) {
            time_passed = 0;
        }

        system_data->time_passed = time_passed;
    } else {
        system_data->time_passed = time_passed;
        return false;
    }

    return true;
}

ecs_entity_t _ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    uint32_t offset,
    uint32_t limit,
    ecs_type_t filter,
    void *param)
{
    ecs_world_t *real_world = world;
    if (world->magic == ECS_THREAD_MAGIC) {
        real_world = ((ecs_thread_t*)world)->world; /* dispel the magic */
    }

    bool in_progress = real_world->in_progress;  

    ecs_entity_info_t sys_info = {.entity = system};
    EcsColSystem *system_data = ecs_get_ptr_intern(real_world, &real_world->main_stage, 
        &sys_info, EEcsColSystem, false, false);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return 0;
    }

    if (!param) {
        param = system_data->base.ctx;
    }

    float system_delta_time = delta_time + system_data->time_passed;
    float period = system_data->period;
    bool measure_time = real_world->measure_system_time;

    ecs_matched_table_t *tables = ecs_vector_first(system_data->tables);
    uint32_t i, table_count = ecs_vector_count(system_data->tables);

    if (!table_count) {
        return 0;
    }

    if (period) {
        if (!should_run(system_data, period, delta_time)) {
            return 0;
        }
    }

    ecs_stage_t *stage = NULL;
    if (!in_progress) {
        real_world->in_progress = true;
        stage = ecs_get_stage(&real_world);
    }    

    ecs_time_t time_start;
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    uint32_t column_count = ecs_vector_count(system_data->base.columns);
    ecs_entity_t interrupted_by = 0;
    ecs_system_action_t action = system_data->base.action;
    bool offset_limit = (offset | limit) != 0;
    bool limit_set = limit != 0;

    ecs_rows_t info = {
        .world = world,
        .system = system,
        .param = param,
        .column_count = column_count,
        .delta_time = system_delta_time,
        .world_time = world->world_time_total,
        .frame_offset = offset,
        .table_offset = 0,
        .system_data = &system_data->base
    };

    for (i = 0; i < table_count; i ++) {
        ecs_matched_table_t *table = &tables[i];
        ecs_table_t *world_table = table->table;
        ecs_table_column_t *table_data = NULL;
        uint32_t first = 0, count = 0;

        if (world_table) {
            table_data = world_table->columns;
            count = ecs_table_count(world_table);

            if (filter) {
                if (!ecs_type_contains(
                    real_world, world_table->type, filter, true, true))
                {
                    continue;
                }
            }

            if (offset_limit) {
                if (offset) {
                    if (offset > count) {
                        offset -= count;
                        continue;
                    } else {
                        first += offset;
                        count -= offset;
                        offset = 0;
                    }
                }

                if (limit) {
                    if (limit > count) {
                        limit -= count;
                    } else {
                        count = limit;
                        limit = 0;
                    }
                } else if (limit_set) {
                    break;
                }
            }

            if (!count) {
                continue;
            }

            ecs_entity_t *entity_buffer = 
                    ecs_vector_first(table_data[0].data);
            info.entities = &entity_buffer[first];            
        }

        if (table->references) {
            info.references = ecs_vector_first(table->references);
        } else {
            info.references = NULL;
        }

        info.columns = table->columns;
        info.table = world_table;
        info.table_columns = table_data;
        info.components = table->components;
        info.offset = first;
        info.count = count;
        
        action(&info);

        info.frame_offset += count;
        info.table_offset ++;

        if (info.interrupted_by) {
            interrupted_by = info.interrupted_by;
            break;
        }
    }

    /* If world wasn't in progress when we entered this function, we need to
     * merge and reset the value */
    if (!in_progress) {
        real_world->in_progress = false;
        ecs_stage_merge(real_world, stage);
    }

    if (measure_time) {
        system_data->base.time_spent += ecs_time_measure(&time_start);
    }
    
    system_data->base.invoke_count ++;

    return interrupted_by;
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, 0, param);
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
        world, dbg.table, system, system_data, failure_info_out);
}
