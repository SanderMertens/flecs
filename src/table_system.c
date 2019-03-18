#include <string.h>
#include <assert.h>
#include "include/private/flecs.h"
#include "include/util/time.h"

const EcsArrayParams column_arr_params = {
    .element_size = sizeof(EcsSystemColumn)
};

static
EcsEntity components_contains(
    EcsWorld *world,
    EcsType table_type,
    EcsType type,
    EcsEntity *entity_out,
    bool match_all)
{
    EcsArray *components = ecs_type_get(world, &world->main_stage, table_type);
    assert(components != NULL);

    uint32_t i, count = ecs_array_count(components);
    for (i = 0; i < count; i ++) {
        EcsEntity h = *(EcsEntity*)ecs_array_get(
            components, &handle_arr_params, i);

        if (ecs_has(world, h, EcsPrefab)) {
            return 0;
        }

        uint64_t row_64 = ecs_map_get64(world->main_stage.entity_index, h);
        assert(row_64 != 0);

        EcsRow row = ecs_to_row(row_64);
        EcsEntity component = ecs_type_contains(
            world, &world->main_stage, row.type_id, type, match_all, true);
        if (component != 0) {
            if (entity_out) *entity_out = h;
            return component;
        }
    }

    return 0;
}

static
bool components_contains_component(
    EcsWorld *world,
    EcsType table_type,
    EcsEntity component,
    EcsEntity *entity_out)
{
    EcsArray *components = ecs_type_get(world, &world->main_stage, table_type);
    assert(components != NULL);

    uint32_t i, count = ecs_array_count(components);
    for (i = 0; i < count; i ++) {
        EcsEntity h = *(EcsEntity*)ecs_array_get(
            components, &handle_arr_params, i);

        uint64_t row_64 = ecs_map_get64(world->main_stage.entity_index, h);
        assert(row_64 != 0);

        EcsRow row = ecs_to_row(row_64);
        bool result = ecs_type_contains_component(
            world, &world->main_stage, row.type_id, component, true);
        if (result) {
            if (entity_out) *entity_out = h;
            return true;
        }
    }

    return false;
}

/* Special indexes in table_data array */
#define TABLE_INDEX (0)
#define REFS_INDEX (1)
#define REFS_COUNT (2)
#define COMPONENTS_INDEX (3)
#define COLUMNS_INDEX (4)

/* Get ref array for system table */
static
EcsSystemRef* get_ref_data(
    EcsWorld *world,
    EcsColSystem *system_data,
    int32_t *table_data)
{
    EcsSystemRef *ref_data = NULL;

    if (!system_data->refs) {
        system_data->refs = ecs_array_new(&system_data->ref_params, 1);
    }

    if (!table_data[REFS_INDEX]) {
        ref_data = ecs_array_add(
            &system_data->refs, &system_data->ref_params);
        table_data[REFS_INDEX] = ecs_array_count(system_data->refs);
    } else {
        ref_data = ecs_array_get(
            system_data->refs, &system_data->ref_params, table_data[REFS_INDEX] - 1);
    }

    return ref_data;
}

/* Get actual entity on which specified component is stored */
static
EcsEntity get_entity_for_component(
    EcsWorld *world,
    EcsEntity entity,
    EcsType type_id,
    EcsEntity component)
{
    if (entity) {
        EcsRow row = ecs_to_row(ecs_map_get64(world->main_stage.entity_index, entity));
        type_id = row.type_id;
    }

    EcsArray *type = ecs_type_get(world, NULL, type_id);
    EcsEntity *buffer = ecs_array_buffer(type);
    uint32_t i, count = ecs_array_count(type);

    for (i = 0; i < count; i ++) {
        if (buffer[i] == component) {
            break;
        }
    }

    if (i == count) {
        EcsEntity prefab = ecs_map_get64(world->prefab_index, type_id);
        if (prefab) {
            return get_entity_for_component(world, prefab, 0, component);
        }
    }

    /* This function must only be called if it has already been validated that
     * a component is available for a given type or entity */
    assert(entity != 0);

    return entity;
}

/** Add table to system, compute offsets for system components in table rows */
static
void add_table(
    EcsWorld *world,
    EcsEntity system,
    EcsColSystem *system_data,
    EcsTable *table)
{
    int32_t *table_data;
    EcsSystemRef *ref_data = NULL;
    EcsType table_type = table->type_id;
    uint32_t i = COLUMNS_INDEX;
    uint32_t ref = 0;
    uint32_t column_count = ecs_array_count(system_data->base.columns);

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */
    table_data = ecs_array_add(
        &system_data->inactive_tables, &system_data->table_params);

    /* Add element to array that contains components for this table. Tables
     * typically share the same component list, unless the system contains OR
     * expressions in the signature. In that case, the system can match against
     * tables that have different components for a column. */
    EcsEntity *component_data = ecs_array_add(
        &system_data->components, &system_data->component_params);

    /* Table index is at element 0 */
    table_data[TABLE_INDEX] = ecs_array_get_index(
        world->main_stage.tables, &table_arr_params, table);

    /* Index in ref array is at element 1 (0 means no refs) */
    table_data[REFS_INDEX] = 0;

    /* Index in components array is at element 2 */
    table_data[COMPONENTS_INDEX] = ecs_array_count(system_data->components) - 1;

    /* Walk columns parsed from the system signature */
    EcsIter it = ecs_array_iter(system_data->base.columns, &column_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsSystemColumn *column = ecs_iter_next(&it);
        EcsEntity entity = 0, component = 0;

        /* Column that retrieves data from an entity */
        if (column->kind == EcsFromSelf || column->kind == EcsFromEntity) {
            if (column->oper_kind == EcsOperAnd) {
                component = column->is.component;
            } else if (column->oper_kind == EcsOperOptional) {
                component = column->is.component;
            } else if (column->oper_kind == EcsOperOr) {
                component = ecs_type_contains(
                    world, &world->main_stage, table_type, column->is.type, 
                    false, true);
            }

            if (column->kind == EcsFromEntity) {
                entity = column->source;
            }

        /* Column that just passes a handle to the system (no data) */
        } else if (column->kind == EcsFromId) {
            component = column->is.component;
            table_data[i] = 0;

        /* Column that retrieves data from a component */
        } else if (column->kind == EcsFromContainer) {
            if (column->oper_kind == EcsOperAnd ||
                column->oper_kind == EcsOperOptional)
            {
                component = column->is.component;
                components_contains_component(
                    world, table_type, component, &entity);

            } else if (column->oper_kind == EcsOperOr) {
                component = components_contains(
                    world,
                    table_type,
                    column->is.type,
                    &entity,
                    false);
            }

        /* Column that retrieves data from a system */
        } else if (column->kind == EcsFromSystem) {
            if (column->oper_kind == EcsOperAnd) {
                component = column->is.component;
            }

            entity = system;

        /* Column that retrieves singleton components */
        } else if (column->kind == EcsFromSingleton) {
            component = column->is.component;
            entity = 0;
        }

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromContainer) and is not just a handle */
        if (!entity && column->kind != EcsFromId) {
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

                    if (!data->size) {
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

        if (column->oper_kind == EcsOperOptional) {
            /* If table doesn't have the field, mark it as no data */
            if (!ecs_type_contains_component(
                world, &world->main_stage, table_type, component, true))
            {
                table_data[i] = 0;
            }
        }

        /* If entity is set, or component is not found in table, add it as a ref
         * to data of a specific entity. */
        if (entity || table_data[i] == -1 || column->kind == EcsFromSingleton) {
            if (ecs_has(world, component, EcsComponent)) {
                if (!ref_data) {
                    ref_data = get_ref_data(world, system_data, table_data);
                    table_data[REFS_COUNT] = 0;
                }

                /* Find the entity for the component. If the code gets here, this
                * function will return a prefab. */
                if (column->kind == EcsFromSingleton) {
                    ref_data[ref].entity = 0;
                } else if (column->kind == EcsFromEntity) {
                    ref_data[ref].entity = entity;
                } else {
                    ref_data[ref].entity = get_entity_for_component(
                        world, entity, table_type, component);
                }

                ref_data[ref].component = ecs_type_from_entity(world, component);
                ref ++;

                /* Negative number indicates ref instead of offset to ecs_data */
                table_data[i] = -ref;
                table_data[REFS_COUNT] ++;
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

/* Match table with system */
static
bool match_table(
    EcsWorld *world,
    EcsTable *table,
    EcsEntity system,
    EcsColSystem *system_data)
{
    EcsType type, table_type;
    table_type = table->type_id;

    if (ecs_type_contains_component(
        world, &world->main_stage, table_type, TEcsPrefab, false))
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

    uint32_t i, column_count = ecs_array_count(system_data->base.columns);
    EcsSystemColumn *buffer = ecs_array_buffer(system_data->base.columns);

    for (i = 0; i < column_count; i ++) {
        EcsSystemColumn *elem = &buffer[i];
        EcsSystemExprElemKind elem_kind = elem->kind;
        EcsSystemExprOperKind oper_kind = elem->oper_kind;

        if (oper_kind == EcsOperAnd) {
            if (elem_kind == EcsFromSelf) {
                /* Already validated */
            } else if (elem_kind == EcsFromContainer) {
                if (!components_contains_component(
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

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    EcsWorld *world,
    EcsEntity system,
    EcsColSystem *system_data)
{
    EcsTable *buffer = ecs_array_buffer(world->main_stage.tables);
    uint32_t i, count = ecs_array_count(world->main_stage.tables);

    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        if (match_table(world, table, system, system_data)) {
            add_table(world, system, system_data, table);
        }
    }
}

/* -- Private functions -- */

/** Match new table against system (table is created after system) */
void ecs_col_system_notify_of_table(
    EcsWorld *world,
    EcsEntity system,
    EcsTable *table)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    assert(system_data != NULL);

    if (match_table(world, table, system, system_data)) {
        add_table(world, system, system_data, table);
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
void ecs_system_activate_table(
    EcsWorld *world,
    EcsEntity system,
    EcsTable *table,
    bool active)
{
    EcsArray *src_array, *dst_array;
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    EcsSystemKind kind = system_data->base.kind;

    uint32_t table_index = ecs_array_get_index(
        world->main_stage.tables, &table_arr_params, table);

    if (active) {
        src_array = system_data->inactive_tables;
        dst_array = system_data->tables;
    } else {
        src_array = system_data->tables;
        dst_array = system_data->inactive_tables;
    }

    uint32_t count = ecs_array_count(src_array);
    int i;
    for (i = 0; i < count; i ++) {
        uint32_t *index = ecs_array_get(
            src_array, &system_data->table_params, i);
        if (*index == table_index) {
            break;
        }
    }

    ecs_assert(i != count, ECS_INTERNAL_ERROR, "cannot find table to (de)activate");

    uint32_t src_count = ecs_array_move_index(
        &dst_array, src_array, &system_data->table_params, i);

    if (active) {
        uint32_t dst_count = ecs_array_count(dst_array);
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

/* -- Private API -- */

EcsEntity ecs_new_col_system(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = ecs_columns_count(sig);
    if (!count) {
        assert(0);
    }

    EcsEntity result = _ecs_new(
        world, world->t_col_system);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId);
    *id_data = id;

    EcsColSystem *system_data = ecs_get_ptr(world, result, EcsColSystem);
    memset(system_data, 0, sizeof(EcsColSystem));
    system_data->base.action = action;
    system_data->base.enabled = true;
    system_data->base.signature = sig;
    system_data->base.time_spent = 0;
    system_data->base.columns = ecs_array_new(&column_arr_params, count);
    system_data->base.kind = kind;

    system_data->table_params.element_size = sizeof(int32_t) * (count + COLUMNS_INDEX);
    system_data->ref_params.element_size = sizeof(EcsSystemRef) * count;
    system_data->component_params.element_size = sizeof(EcsEntity) * count;
    system_data->period = 0;
    system_data->entity = result;

    system_data->components = ecs_array_new(
        &system_data->component_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->tables = ecs_array_new(
        &system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);
    system_data->inactive_tables = ecs_array_new(
        &system_data->table_params, ECS_SYSTEM_INITIAL_TABLE_COUNT);

    if (ecs_parse_component_expr(
        world, sig, ecs_parse_component_action, system_data) != EcsOk)
    {
        assert(0);
    }

    ecs_system_compute_and_families(world, result, &system_data->base);

    match_tables(world, result, system_data);

    EcsEntity *elem = NULL;

    if (kind == EcsManual) {
        elem = ecs_array_add(&world->on_demand_systems, &handle_arr_params);
    } else if (!ecs_array_count(system_data->tables)) {
        elem = ecs_array_add(&world->inactive_systems, &handle_arr_params);
    } else {
        if (kind == EcsOnFrame) {
            elem = ecs_array_add(&world->on_frame_systems, &handle_arr_params);
        } else if (kind == EcsPreFrame) {
            elem = ecs_array_add(&world->pre_frame_systems, &handle_arr_params);
        } else if (kind == EcsPostFrame) {
            elem = ecs_array_add(&world->post_frame_systems, &handle_arr_params);
        } else if (kind == EcsOnLoad) {
            elem = ecs_array_add(&world->on_load_systems, &handle_arr_params);
        } else if (kind == EcsOnStore) {
            elem = ecs_array_add(&world->on_store_systems, &handle_arr_params);
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

    delta_time = time_passed;

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

EcsEntity _ecs_run_w_filter(
    EcsWorld *world,
    EcsEntity system,
    float delta_time,
    uint32_t offset,
    uint32_t limit,
    EcsType filter,
    void *param)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return 0;
    }

    EcsWorld *real_world = world;
    EcsStage *stage = ecs_get_stage(&real_world);

    float system_delta_time = delta_time + system_data->time_passed;
    float period = system_data->period;
    bool measure_time = real_world->measure_system_time;

    EcsArray *tables = system_data->tables;
    uint32_t tables_size = system_data->table_params.element_size;
    int32_t *table_first = ecs_array_buffer(tables);
    int32_t *table_last = ECS_OFFSET(table_first, tables_size * ecs_array_count(tables));

    if (table_first == table_last) {
        return 0;
    }

    if (period) {
        if (!should_run(system_data, period, delta_time)) {
            return 0;
        }
    }

    struct timespec time_start;
    if (measure_time) {
        ut_time_get(&time_start);
    }

    uint32_t column_count = ecs_array_count(system_data->base.columns);
    uint32_t components_size = system_data->component_params.element_size;
    char *components = ecs_array_buffer(system_data->components);
    EcsEntity interrupted_by = 0;
    EcsSystemAction action = system_data->base.action;
    bool offset_limit = (offset | limit) != 0;
    bool limit_set = limit != 0;
    void *ref_ptrs[column_count]; /* Use worst-case size for references */

    EcsRows info = {
        .world = world,
        .system = system,
        .param = param,
        .column_count = column_count,
        .delta_time = system_delta_time,
        .index_offset = 0,
        .ref_ptrs = ref_ptrs
    };

    int32_t *table = table_first;
    for (; table < table_last; table = ECS_OFFSET(table, tables_size)) {
        int32_t table_index = table[TABLE_INDEX];

        /* A system may introduce a new table if in the main thread. Make sure
         * world_tables points to the valid memory */
        EcsTable *world_tables = ecs_array_buffer(world->main_stage.tables);
        EcsTable *w_table = &world_tables[table_index];
        EcsTableColumn *table_columns = w_table->columns;
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
            info.references = ecs_array_get(
                system_data->refs, &system_data->ref_params, ref_index - 1);

            /* Resolve references */
            int i, count = table[REFS_COUNT];

            for (i = 0; i < count; i ++) {
                info.ref_ptrs[i] = _ecs_get_ptr(world, 
                    info.references[i].entity, info.references[i].component);
            }
        } else {
            info.references = NULL;
        }

        info.columns =  &table[COLUMNS_INDEX];
        info.table_columns = table_columns;
        info.components = ECS_OFFSET(components,
            components_size * table[COMPONENTS_INDEX]);
        info.begin = first;
        info.count = count;
        info.end = first + count;
        info.entities = ecs_array_buffer(((EcsTableColumn*)info.table_columns)[0].data);
        
        action(&info);

        info.index_offset += count;

        if (info.interrupted_by) {
            interrupted_by = info.interrupted_by;
            break;
        }
    }

    if (measure_time) {
        system_data->base.time_spent += ut_time_measure(&time_start);
    }

    return interrupted_by;
}

EcsEntity ecs_run(
    EcsWorld *world,
    EcsEntity system,
    float delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, 0, param);
}
