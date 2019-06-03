#include "flecs_private.h"

const ecs_vector_params_t column_arr_params = {
    .element_size = sizeof(ecs_system_column_t)
};

static
ecs_entity_t components_contains(
    ecs_world_t *world,
    ecs_type_t table_type,
    ecs_type_t type,
    ecs_entity_t *entity_out,
    bool match_all)
{
    ecs_vector_t *components = ecs_type_get(world, &world->main_stage, table_type);
    assert(components != NULL);

    uint32_t i, count = ecs_vector_count(components);
    ecs_entity_t *buffer = ecs_vector_first(components);
    for (i = 0; i < count; i ++) {
        ecs_entity_t h = buffer[i];

        uint64_t row_64 = ecs_map_get64(world->main_stage.entity_index, h);
        assert(row_64 != 0);

        ecs_row_t row = ecs_to_row(row_64);
        ecs_entity_t component = ecs_type_contains(
            world, &world->main_stage, row.type_id, type, match_all, true);
        if (component != 0) {
            if (entity_out) *entity_out = h;
            return component;
        }
    }

    return 0;
}

/* Special indexes in table_data array */
#define TABLE_INDEX (0)
#define REFS_INDEX (1)
#define REFS_COUNT (2)
#define COMPONENTS_INDEX (3)
#define DEPTH_INDEX (4)
#define COLUMNS_INDEX (5)

/* Get ref array for system table */
static
ecs_reference_t* get_ref_data(
    EcsColSystem *system_data,
    int32_t *table_data)
{
    if (!system_data->refs) {
        system_data->refs = ecs_vector_new(&system_data->ref_params, 1);
    }

    ecs_reference_t *ref_data = ecs_vector_add(
        &system_data->refs, &system_data->ref_params);
        
    table_data[REFS_INDEX] = ecs_vector_count(system_data->refs);

    return ref_data;
}

/* Get actual entity on which specified component is stored */
ecs_entity_t ecs_get_entity_for_component(
    ecs_world_t *world,
    bool new_table,
    ecs_entity_t entity,
    ecs_type_t type_id,
    ecs_entity_t component)
{
    if (entity) {
        ecs_row_t row = ecs_to_row(ecs_map_get64(world->main_stage.entity_index, entity));
        type_id = row.type_id;
    }

    ecs_vector_t *type = ecs_type_get(world, NULL, type_id);
    ecs_entity_t *buffer = ecs_vector_first(type);
    uint32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        if (buffer[i] == component) {
            break;
        }
    }

    if (i == count) {
        ecs_stage_t *stage;
        if (world->in_progress) {
            stage = &world->temp_stage;
        } else {
            stage = &world->main_stage;
        }

        ecs_entity_t prefab = ecs_get_prefab_from_type(
            world, stage, new_table, entity, type_id);
        
        if (prefab) {
            return ecs_get_entity_for_component(world, new_table, prefab, 0, component);
        }
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
    int32_t *table_data;
    ecs_reference_t *ref_data = NULL;
    ecs_type_t table_type = table->type_id;
    uint32_t i = COLUMNS_INDEX;
    uint32_t ref = 0;
    uint32_t column_count = ecs_vector_count(system_data->base.columns);

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */
    table_data = ecs_vector_add(
        &system_data->inactive_tables, &system_data->table_params);

    /* Add element to array that contains components for this table. Tables
     * typically share the same component list, unless the system contains OR
     * expressions in the signature. In that case, the system can match against
     * tables that have different components for a column. */
    ecs_entity_t *component_data = ecs_vector_add(
        &system_data->components, &system_data->component_params);

    /* Table index is at element 0 */
    table_data[TABLE_INDEX] = ecs_vector_get_index(
        world->main_stage.tables, &table_arr_params, table);

    /* Index in ref array is at element 1 (0 means no refs) */
    table_data[REFS_INDEX] = 0;
    table_data[REFS_COUNT] = 0;

    /* Index in components array is at element 2 */
    table_data[COMPONENTS_INDEX] = ecs_vector_count(system_data->components) - 1;

    /* Walk columns parsed from the system signature */
    ecs_system_column_t *columns = ecs_vector_first(system_data->base.columns);
    uint32_t c, count = ecs_vector_count(system_data->base.columns);

    for (c = 0; c < count; c ++) {
        ecs_system_column_t *column = &columns[c];
        ecs_entity_t entity = 0, component = 0;
        ecs_system_expr_elem_kind_t kind = column->kind;
        ecs_system_expr_oper_kind_t oper_kind = column->oper_kind;

        /* Column that retrieves data from self or a fixed entity */
        if (kind == EcsFromSelf || kind == EcsFromEntity) {
            if (oper_kind == EcsOperAnd) {
                component = column->is.component;
            } else if (oper_kind == EcsOperOptional) {
                component = column->is.component;
            } else if (oper_kind == EcsOperOr) {
                component = ecs_type_contains(
                    world, &world->main_stage, table_type, column->is.type, 
                    false, true);
            }

            if (kind == EcsFromEntity) {
                entity = column->source;
            }

        /* Column that just passes a handle to the system (no data) */
        } else if (kind == EcsFromId) {
            component = column->is.component;
            table_data[i] = 0;

        /* Column that retrieves data from a dynamic entity */
        } else if (kind == EcsFromContainer || kind == EcsCascade) {
            if (oper_kind == EcsOperAnd ||
                oper_kind == EcsOperOptional)
            {
                component = column->is.component;
                ecs_components_contains_component(
                    world, table_type, component, &entity);

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

        /* Column that retrieves singleton components */
        } else if (kind == EcsFromSingleton) {
            component = column->is.component;
            entity = 0;
        }

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromContainer) and is not just a handle */
        if (!entity && kind != EcsFromId) {
            if (component) {
                /* Retrieve offset for component */
                table_data[i] = ecs_type_index_of(table->type, component);

                /* If column is found, add one to the index, as column zero in
                 * a table is reserved for entity id's */
                if (table_data[i] != -1) {
                    table_data[i] ++;

                    /* Check if component is a tag. If it is, set table_data to
                     * zero, so that a system won't try to access the data */
                    EcsComponent *data = ecs_get_ptr(
                        world, component, EcsComponent);

                    if (!data || !data->size) {
                        table_data[i] = 0;
                    }
                }
                
                /* ecs_table_column_offset may return -1 if the component comes
                 * from a prefab. If so, the component will be resolved as a
                 * reference (see below) */
            } else {
                /* Columns with a NOT expression have no data */
                table_data[i] = 0;
            }
        }

        if (oper_kind == EcsOperOptional) {
            /* If table doesn't have the field, mark it as no data */
            if (!ecs_type_contains_component(
                world, &world->main_stage, table_type, component, true))
            {
                table_data[i] = 0;
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
        if (entity || table_data[i] == -1 || kind == EcsFromSingleton || kind == EcsCascade) {
            if (ecs_has(world, component, EcsComponent)) {
                EcsComponent *component_data = ecs_get_ptr(
                        world, component, EcsComponent);
                
                if (component_data->size) {
                    ecs_entity_t e;

                    if (!ref_data) {
                        ref_data = get_ref_data(system_data, table_data);
                        table_data[REFS_COUNT] = 0;
                    }

                    /* Find the entity for the component */
                    if (kind == EcsFromSingleton) {
                        e = 0;
                    } else if (kind == EcsFromEntity) {
                        e = entity;
                    } else {
                        e = ecs_get_entity_for_component(
                            world, false, entity, table_type, component);

                        if (kind != EcsCascade) {
                            ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
                        } else if (!e) {
                            e = ECS_INVALID_ENTITY;
                        }
                    }

                    ref_data[ref].entity = e;
                    ref_data[ref].component = component;
                    
                    if (e != ECS_INVALID_ENTITY) {
                        ecs_entity_info_t info = {.entity = e};
                        ref_data[ref].cached_ptr = ecs_get_ptr_intern(
                            world, 
                            &world->main_stage,
                            &info,
                            component,
                            false,
                            false);

                        ecs_set_watching(world, e, true);                     
                    } else {
                        ref_data[ref].cached_ptr = NULL;
                    }

                    ref ++;

                    /* Negative number indicates ref instead of offset to ecs_data */
                    table_data[i] = -ref;
                    table_data[REFS_COUNT] ++;

                    system_data->base.has_refs = true;
                }
            }
        }

        /* component_data index is not offset by anything */
        component_data[i - COLUMNS_INDEX] = component;

        i ++;
    }

    if (ref_data && ref < column_count) {
        ref_data[ref].entity = 0;
    }

    ecs_table_register_system(world, table, system);
}

/* Utility function to update an index to a new value after a table has been
 * removed from the system */
static
bool update_table_index(
    EcsColSystem *system_data,
    ecs_vector_t *tables,
    uint32_t data_index,
    uint32_t old_index,
    uint32_t new_index)
{
    int32_t *table = ecs_vector_first(tables);
    uint32_t i, count = ecs_vector_count(tables);

    for (i = 0; i < count; i ++) {
        if (table[data_index] == (int32_t)old_index) {
            table[data_index] = (int32_t)new_index;
            return true;
        }

        table = ECS_OFFSET(
            table, system_data->table_params.element_size);
    }

    return false;
}

static
void update_table_data(
    EcsColSystem *system_data,
    int32_t *table_data,
    uint32_t data_index,
    ecs_vector_t *data_array,
    ecs_vector_params_t *data_params)
{
    uint32_t index = table_data[data_index];
    if (index) {
        uint32_t count = ecs_vector_count(data_array);

        if (index == count) {
            ecs_vector_remove_last(data_array);
        } else {
            /* Removing the refs for this table will cause the ref index for
             * another table to become invalid. Find the table for the last
             * reference array, and change it to ref_index */
            if (!update_table_index(
                system_data, 
                system_data->tables, 
                data_index, 
                count, 
                index)) 
            {
                update_table_index(
                    system_data, 
                    system_data->inactive_tables, 
                    data_index, 
                    count, 
                    index);
            }

            ecs_vector_remove_index(data_array, data_params, index);
        }
    }
}

/* Remove table */
static
void remove_table(
    EcsColSystem *system_data,
    ecs_vector_t *tables,
    int32_t index)
{
    int32_t *table_data = ecs_vector_get(
        tables, &system_data->table_params, index);

    update_table_data(
        system_data, table_data, REFS_INDEX, system_data->refs, 
        &system_data->ref_params);

    update_table_data(
        system_data, table_data, COMPONENTS_INDEX, system_data->components, 
        &system_data->component_params);        

    ecs_vector_remove(tables, &system_data->table_params, table_data);
}

/* Match table with system */
static
bool match_table(
    ecs_world_t *world,
    ecs_table_t *table,
    EcsColSystem *system_data)
{
    ecs_type_t type, table_type;
    table_type = table->type_id;

    if (ecs_type_contains_component(
        world, &world->main_stage, table_type, EEcsPrefab, false))
    {
        /* Never match prefabs */
        return false;
    }

    type = system_data->base.and_from_entity;

    if (type && !ecs_type_contains(
        world, &world->main_stage, table_type, type, true, true))
    {
        return false;
    }

    uint32_t i, column_count = ecs_vector_count(system_data->base.columns);
    ecs_system_column_t *buffer = ecs_vector_first(system_data->base.columns);

    for (i = 0; i < column_count; i ++) {
        ecs_system_column_t *elem = &buffer[i];
        ecs_system_expr_elem_kind_t elem_kind = elem->kind;
        ecs_system_expr_oper_kind_t oper_kind = elem->oper_kind;

        if (oper_kind == EcsOperAnd) {
            if (elem_kind == EcsFromSelf) {
                /* Already validated */
            } else if (elem_kind == EcsFromContainer) {
                if (!ecs_components_contains_component(
                    world, table_type, elem->is.component, NULL))
                {
                    return false;
                }
            }
        } else if (oper_kind == EcsOperOr) {
            type = elem->is.type;
            if (elem_kind == EcsFromSelf) {
                if (!ecs_type_contains(
                    world, &world->main_stage, table_type, type, false, true))
                {
                    return false;
                }
            } else if (elem_kind == EcsFromContainer) {
                if (!components_contains(
                    world, table_type, type, NULL, false))
                {
                    return false;
                }
            }
        }
    }

    type = system_data->base.not_from_entity;
    if (type && ecs_type_contains(
        world, &world->main_stage, table_type, type, false, true))
    {
        return false;
    }

    type = system_data->base.not_from_component;
    if (type && components_contains(
        world, table_type, type, NULL, false))
    {
        return false;
    }

    return true;
}

static
int table_compare(
    const void *table_1,
    const void *table_2)
{
    const int32_t *table_1_data = table_1;
    const int32_t *table_2_data = table_2;

    return table_1_data[DEPTH_INDEX] - table_2_data[DEPTH_INDEX];
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
        int32_t *table_data = ecs_vector_get(
            system_data->tables, &system_data->table_params, i);
        ecs_table_t *tables = ecs_vector_first(world->main_stage.tables);
        ecs_type_t type = tables[table_data[TABLE_INDEX]].type_id;
        table_data[DEPTH_INDEX] = ecs_type_container_depth(
            world, type, cascade_component);
    }

    ecs_vector_sort(system_data->tables, &system_data->table_params, table_compare);
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data)
{
    ecs_table_t *buffer = ecs_vector_first(world->main_stage.tables);
    uint32_t i, count = ecs_vector_count(world->main_stage.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = &buffer[i];
        if (match_table(world, table, system_data)) {
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
    uint32_t table_index)
{
    uint32_t i, count = ecs_vector_count(tables);
    int32_t *table_data = ecs_vector_first(tables);
    uint32_t elem_size = system_data->table_params.element_size;

    for (i = 0; i < count; i ++) {
        if (*table_data == (int32_t)table_index) {
            return i;
        }

        table_data = ECS_OFFSET(table_data, elem_size);
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
    int32_t *table_data = ecs_vector_get(
        system_data->tables, &system_data->table_params, table_data_index);
    
    ecs_assert(table_data[REFS_INDEX] != 0, ECS_INTERNAL_ERROR, NULL);

    /* Obtain reference index */
    int32_t *column_indices = &table_data[COLUMNS_INDEX];
    int32_t column = system_data->base.cascade_by - 1;
    int32_t ref_index = -column_indices[column] - 1;

    /* Obtain pointer to the reference data */
    ecs_reference_t *references = ecs_vector_get(
        system_data->refs, &system_data->ref_params, table_data[REFS_INDEX]-1);
    ecs_reference_t *ref = &references[ref_index];
    ecs_assert(ref->component == get_cascade_component(system_data), 
        ECS_INTERNAL_ERROR, NULL);

    /* Resolve container entity */
    ecs_entity_t container = 0;
    ecs_components_contains_component(
        world, table_type, ref->component, &container);        

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

    /* Only rematch systems that have references */
    if (!system_data->base.has_refs) {
        return;
    }

    ecs_vector_t *tables = world->main_stage.tables;
    uint32_t i, count = ecs_vector_count(tables);
    ecs_table_t *buffer = ecs_vector_first(tables);
    bool changed = false;

    for (i = 0; i < count; i ++) {
        /* Is the system currently matched with the table? */
        int32_t match = table_matched(system_data, system_data->tables, i);
        ecs_table_t *table = &buffer[i];

        if (match_table(world, table, system_data)) {
            /* If the table matches, and it is not currently matched, add */
            if (match == -1) {
                add_table(world, system, system_data, table);
                changed = true;

            /* If table still matches and has cascade column, reevaluate the
                * sources of references. This may have changed in case 
                * components were added/removed to container entities */ 
            } else if (system_data->base.cascade_by) {
                resolve_cascade_container(
                    world, system_data, match, table->type_id);
            }
        } else {
            /* If table no longer matches, remove it */
            if (match != -1) {
                remove_table(system_data, system_data->tables, match);
                changed = true;
            } else {
                /* Make sure the table is removed if it was inactive */
                match = table_matched(
                    system_data, system_data->inactive_tables, i);
                if (match != -1) {
                    remove_table(
                        system_data, system_data->inactive_tables, match);
                    changed = true;
                }
            }
        }
    }

    /* If the system has a CASCADE column and modifications were made, 
        * reorder the system tables so that the depth order is preserved */
    if (changed && system_data->base.cascade_by) {
        order_cascade_tables(world, system_data);
    }
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

    for (i = 0; i < count; i ++) {
        if (!system_data->refs) {
            continue;
        }


        uint32_t *table_data = ecs_vector_get(system_data->tables, &system_data->table_params, i);
        uint32_t r, ref_count = table_data[REFS_COUNT];

        ecs_reference_t *refs = NULL;
        
        if (ref_count) {
            refs = ecs_vector_get(system_data->refs, &system_data->ref_params, table_data[REFS_INDEX] - 1);
            for (r = 0; r < ref_count; r ++) {
                ecs_reference_t ref = refs[r];
                ecs_entity_info_t info = {.entity = ref.entity};
                refs[r].cached_ptr = ecs_get_ptr_intern(
                    world, &world->main_stage, &info, ref.component, false, true);
            }            
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

    if (match_table(world, table, system_data)) {
        add_table(world, system, system_data, table);
    }
}

static
int32_t get_table_param_index(
    ecs_world_t *world,
    EcsColSystem *system_data,
    ecs_table_t *table,
    ecs_vector_t *tables)
{
    uint32_t table_index = ecs_vector_get_index(
        world->main_stage.tables, &table_arr_params, table);

    uint32_t i, count = ecs_vector_count(tables);

    for (i = 0; i < count; i ++) {
        uint32_t *index = ecs_vector_get(
            tables, &system_data->table_params, i);
        if (*index == table_index) {
            break;
        }
    }

    if (i == count) {
        return -1;
    } else {
        return i;
    }
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
    EcsSystemKind kind = system_data->base.kind;

    if (active) {
        src_array = system_data->inactive_tables;
        dst_array = system_data->tables;
    } else {
        src_array = system_data->tables;
        dst_array = system_data->inactive_tables;
    }

    int32_t i = get_table_param_index(world, system_data, table, src_array);

    ecs_assert(i != -1, ECS_INTERNAL_ERROR, "cannot find table to (de)activate");

    uint32_t src_count = ecs_vector_move_index(
        &dst_array, src_array, &system_data->table_params, i);

    if (active) {
        uint32_t dst_count = ecs_vector_count(dst_array);
        if (kind != EcsManual) {
            if (dst_count == 1 && system_data->base.enabled) {
                ecs_world_activate_system(
                    world, system, kind, true);
            }
        }
        system_data->tables = dst_array;
    } else {
        if (kind != EcsManual) {
            if (src_count == 0) {
                ecs_world_activate_system(
                    world, system, kind, false);
            }
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
    if (!count) {
        assert(0);
    }

    ecs_entity_t result = _ecs_new(
        world, world->t_col_system);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId);
    *id_data = id;

    EcsColSystem *system_data = ecs_get_ptr(world, result, EcsColSystem);
    memset(system_data, 0, sizeof(EcsColSystem));
    system_data->base.action = action;
    system_data->base.enabled = true;
    system_data->base.signature = sig;
    system_data->base.time_spent = 0;
    system_data->base.columns = ecs_vector_new(&column_arr_params, count);
    system_data->base.kind = kind;
    system_data->base.cascade_by = 0;
    system_data->base.has_refs = false;

    system_data->table_params.element_size = sizeof(int32_t) * (count + COLUMNS_INDEX);
    system_data->ref_params.element_size = sizeof(ecs_reference_t) * count;
    system_data->component_params.element_size = sizeof(ecs_entity_t) * count;
    system_data->period = 0;
    system_data->entity = result;

    system_data->components = ecs_vector_new(
        &system_data->component_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->tables = ecs_vector_new(
        &system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->inactive_tables = ecs_vector_new(
        &system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);

    if (ecs_parse_component_expr(
        world, sig, ecs_parse_component_action, system_data) != 0)
    {
        assert(0);
    }

    ecs_system_compute_and_families(world, &system_data->base);

    match_tables(world, result, system_data);

    ecs_entity_t *elem = NULL;

    if (kind == EcsManual) {
        elem = ecs_vector_add(&world->on_demand_systems, &handle_arr_params);
    } else if (!ecs_vector_count(system_data->tables)) {
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
        } else {
            ecs_abort(ECS_INVALID_PARAMETERS, NULL);
        }
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

    ecs_entity_info_t sys_info = {.entity = system};
    EcsColSystem *system_data = ecs_get_ptr_intern(real_world, &real_world->main_stage, 
        &sys_info, EEcsColSystem, false, false);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return 0;
    }

    ecs_stage_t *stage = ecs_get_stage(&real_world);

    float system_delta_time = delta_time + system_data->time_passed;
    float period = system_data->period;
    bool measure_time = real_world->measure_system_time;

    ecs_vector_t *tables = system_data->tables;
    uint32_t tables_size = system_data->table_params.element_size;
    int32_t *table_first = ecs_vector_first(tables);
    int32_t *table_last = ECS_OFFSET(table_first, tables_size * ecs_vector_count(tables));

    if (table_first == table_last) {
        return 0;
    }

    if (period) {
        if (!should_run(system_data, period, delta_time)) {
            return 0;
        }
    }

    ecs_time_t time_start;
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    uint32_t column_count = ecs_vector_count(system_data->base.columns);
    uint32_t components_size = system_data->component_params.element_size;
    char *components = ecs_vector_first(system_data->components);
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
        .frame_offset = offset
    };

    int32_t *table = table_first;
    for (; table < table_last; table = ECS_OFFSET(table, tables_size)) {
        int32_t table_index = table[TABLE_INDEX];

        /* A system may introduce a new table if in the main thread. Make sure
         * world_tables points to the valid memory */
        ecs_table_t *world_tables = ecs_vector_first(real_world->main_stage.tables);
        ecs_table_t *w_table = &world_tables[table_index];
        ecs_table_column_t *table_columns = w_table->columns;
        uint32_t first = 0, count = ecs_table_count(w_table);

        if (filter) {
            if (!ecs_type_contains(
                real_world, stage, w_table->type_id, filter, true, true))
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

        uint32_t ref_index = table[REFS_INDEX];

        if (ref_index) {
            info.references = ecs_vector_get(
                system_data->refs, &system_data->ref_params, ref_index - 1);
        } else {
            info.references = NULL;
        }

        info.columns =  &table[COLUMNS_INDEX];
        info.table = w_table;
        info.table_columns = table_columns;
        info.components = ECS_OFFSET(components,
            components_size * table[COMPONENTS_INDEX]);
        info.offset = first;
        info.count = count;

        ecs_entity_t *entity_buffer = 
                ecs_vector_first(((ecs_table_column_t*)info.table_columns)[0].data);
        info.entities = &entity_buffer[first];
        
        action(&info);

        info.frame_offset += count;

        if (info.interrupted_by) {
            interrupted_by = info.interrupted_by;
            break;
        }
    }

    if (measure_time) {
        system_data->base.time_spent += ecs_time_measure(&time_start);
    }

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
