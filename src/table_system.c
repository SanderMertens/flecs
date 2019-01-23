#include <string.h>
#include <assert.h>
#include "include/private/reflecs.h"
#include "include/util/time.h"

const EcsArrayParams column_arr_params = {
    .element_size = sizeof(EcsSystemColumn)
};

static
void compute_and_families(
    EcsWorld *world,
    EcsTableSystem *system_data)
{
    uint32_t i, column_count = ecs_array_count(system_data->base.columns);
    EcsSystemColumn *buffer = ecs_array_buffer(system_data->base.columns);

    for (i = 0; i < column_count; i ++) {
        EcsSystemColumn *elem = &buffer[i];
        EcsSystemExprElemKind elem_kind = elem->kind;
        EcsSystemExprOperKind oper_kind = elem->oper_kind;

        if (elem_kind == EcsFromEntity) {
            if (oper_kind == EcsOperAnd) {
                system_data->and_from_entity = ecs_family_add(
                 world, NULL, system_data->and_from_entity, elem->is.component);
            }
        } else if (elem_kind == EcsFromSystem) {
            if (oper_kind == EcsOperAnd) {
                system_data->and_from_system = ecs_family_add(
                 world, NULL, system_data->and_from_system, elem->is.component);
            }
        }
    }
}

static
EcsEntity components_contains(
    EcsWorld *world,
    EcsStage *stage,
    EcsFamily table_family,
    EcsFamily family,
    EcsEntity *entity_out,
    bool match_all)
{
    EcsArray *components = ecs_family_get(world, stage, table_family);
    assert(components != NULL);

    uint32_t i, count = ecs_array_count(components);
    for (i = 0; i < count; i ++) {
        EcsEntity h = *(EcsEntity*)ecs_array_get(
            components, &handle_arr_params, i);

        uint64_t row_64 = ecs_map_get64(world->entity_index, h);
        assert(row_64 != 0);

        EcsRow row = ecs_to_row(row_64);
        EcsEntity component = ecs_family_contains(
            world, stage, row.family_id, family, match_all, true);
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
    EcsStage *stage,
    EcsFamily table_family,
    EcsEntity component,
    EcsEntity *entity_out)
{
    EcsArray *components = ecs_family_get(world, stage, table_family);
    assert(components != NULL);

    uint32_t i, count = ecs_array_count(components);
    for (i = 0; i < count; i ++) {
        EcsEntity h = *(EcsEntity*)ecs_array_get(
            components, &handle_arr_params, i);

        uint64_t row_64 = ecs_map_get64(world->entity_index, h);
        assert(row_64 != 0);

        EcsRow row = ecs_to_row(row_64);
        bool result = ecs_family_contains_component(
            world, stage, row.family_id, component);
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
#define COMPONENTS_INDEX (2)
#define COLUMNS_INDEX (3)

/* Get ref array for system table */
static
EcsSystemRef* get_ref_data(
    EcsWorld *world,
    EcsTableSystem *system_data,
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
            system_data->refs, &system_data->ref_params, table_data[1] - 1);
    }

    return ref_data;
}

/* Get actual entity on which specified component is stored */
static
EcsEntity get_entity_for_component(
    EcsWorld *world,
    EcsEntity entity,
    EcsFamily family_id,
    EcsEntity component)
{
    if (entity) {
        EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, entity));
        family_id = row.family_id;
    }

    EcsArray *family = ecs_family_get(world, NULL, family_id);
    EcsEntity *buffer = ecs_array_buffer(family);
    uint32_t i, count = ecs_array_count(family);

    for (i = 0; i < count; i ++) {
        if (buffer[i] == component) {
            break;
        }
    }

    if (i == count) {
        EcsEntity prefab = ecs_map_get64(world->prefab_index, family_id);
        if (prefab) {
            return get_entity_for_component(world, prefab, 0, component);
        }
    }

    /* This function must only be called if it has already been validated that
     * a component is available for a given family or entity */
    assert(entity != 0);

    return entity;
}

/** Add table to system, compute offsets for system components in table rows */
static
void add_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity system,
    EcsTableSystem *system_data,
    EcsTable *table)
{
    int32_t *table_data;
    EcsSystemRef *ref_data = NULL;
    EcsFamily table_family = table->family_id;
    uint32_t i = COLUMNS_INDEX;
    uint32_t ref = 0;
    uint32_t column_count = ecs_array_count(system_data->base.columns);

    /* If the table is empty, add it to the inactive array, so it is skipped
     * when the system is evaluated */
    if (ecs_array_count(table->rows)) {
        table_data = ecs_array_add(
            &system_data->tables, &system_data->table_params);
    } else {
        table_data = ecs_array_add(
            &system_data->inactive_tables, &system_data->table_params);
    }

    /* Add element to array that contains components for this table. Tables
     * typically share the same component list, unless the system contains OR
     * expressions in the signature. In that case, the system can match against
     * tables that have different components for a column. */
    EcsEntity *component_data = ecs_array_add(
        &system_data->components, &system_data->component_params);

    /* Table index is at element 0 */
    table_data[TABLE_INDEX] = ecs_array_get_index(
        world->table_db, &table_arr_params, table);

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
        if (column->kind == EcsFromEntity) {
            if (column->oper_kind == EcsOperAnd) {
                component = column->is.component;
            } else if (column->oper_kind == EcsOperOptional) {
                component = column->is.component;
                if (!ecs_family_contains_component(
                    world, stage, table_family, component))
                {
                    component = 0;
                }

            } else if (column->oper_kind == EcsOperOr) {
                component = ecs_family_contains(
                    world, stage, table_family, column->is.family, false, true);
            }

        /* Column that just passes a handle to the system (no data) */
        } else if (column->kind == EcsFromHandle) {
            component = column->is.component;
            table_data[i] = 0;

        /* Column that retrieves data from a component */
        } else if (column->kind == EcsFromComponent) {
            if (column->oper_kind == EcsOperAnd ||
                column->oper_kind == EcsOperOptional)
            {
                component = column->is.component;
                components_contains_component(
                    world, stage, table_family, component, &entity);

            } else if (column->oper_kind == EcsOperOr) {
                component = components_contains(
                    world,
                    stage,
                    table_family,
                    column->is.family,
                    &entity,
                    false);
            }

        /* Column that retrieves data from a system */
        } else if (column->kind == EcsFromSystem) {
            if (column->oper_kind == EcsOperAnd) {
                component = column->is.component;
            }

            entity = system;
        }

        /* This column does not retrieve data from a static entity (either
         * EcsFromSystem or EcsFromComponent) and is not just a handle */
        if (!entity && column->kind != EcsFromHandle) {
            if (component) {
                /* Retrieve offset for component */
                table_data[i] = ecs_table_column_offset(table, component);

                /* ecs_table_column_offset may return -1 if the component comes
                 * from a prefab. If so, the component will be resolved as a
                 * reference (see below) */
            } else {
                /* Columns with a NOT expression have no data */
                table_data[i] = 0;
            }
        }

        /* If entity is set, or component is not found in table, add it as a ref
         * to data of a specific entity. */
        if (entity || table_data[i] == -1) {
            if (!ref_data) {
                ref_data = get_ref_data(world, system_data, table_data);
            }

            /* Find the entity for the component. If the code gets here, this
             * function will return a prefab. */
            ref_data[ref].entity = get_entity_for_component(
                world, entity, table_family, component);
            ref_data[ref].component = component;
            ref ++;

            /* Negative number indicates ref instead of offset to ecs_data */
            table_data[i] = -ref;
        }

        /* component_data index is not offset by anything */
        component_data[i - COLUMNS_INDEX] = component;

        i ++;
    }

    if (ref_data && ref < column_count) {
        ref_data[ref].entity = 0;
    }

    /* Register system with the table */
    EcsEntity *h = ecs_array_add(&table->frame_systems, &handle_arr_params);;
    if (h) *h = system;
}

/* Match table with system */
static
bool match_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsTable *table,
    EcsEntity system,
    EcsTableSystem *system_data)
{
    EcsFamily family, table_family;
    table_family = table->family_id;

    if (ecs_family_contains_component(world, stage, table_family, EcsPrefab_h)){
        /* Never match prefabs */
        return false;
    }

    family = system_data->and_from_entity;

    if (family && !ecs_family_contains(
        world, stage, table_family, family, true, true))
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
            if (elem_kind == EcsFromEntity) {
                /* Already validated */
            } else if (elem_kind == EcsFromComponent) {
                if (!components_contains_component(
                    world, stage, table_family, elem->is.component, NULL))
                {
                    return false;
                }
            }
        } else if (oper_kind == EcsOperOr) {
            family = elem->is.family;
            if (elem_kind == EcsFromEntity) {
                if (!ecs_family_contains(
                    world, stage, table_family, family, false, true))
                {
                    return false;
                }
            } else if (elem_kind == EcsFromComponent) {
                if (!components_contains(
                    world, stage, table_family, family, NULL, false))
                {
                    return false;
                }
            }
        }
    }

    family = system_data->base.not_from_entity;
    if (family && ecs_family_contains(
        world, stage, table_family, family, false, true))
    {
        return false;
    }

    family = system_data->base.not_from_component;
    if (family && components_contains(
        world, stage, table_family, family, NULL, false))
    {
        return false;
    }

    return true;
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity system,
    EcsTableSystem *system_data)
{
    EcsIter it = ecs_array_iter(world->table_db, &table_arr_params);
    while (ecs_iter_hasnext(&it)) {
        EcsTable *table = ecs_iter_next(&it);
        if (match_table(world, stage, table, system, system_data)) {
            add_table(world, stage, system, system_data, table);
        }
    }
}

/** Resolve references */
static
void resolve_refs(
    EcsWorld *world,
    EcsTableSystem *system_data,
    uint32_t refs_index,
    EcsRows *info)
{
    EcsArray *system_refs = system_data->refs;
    EcsSystemRef *refs = ecs_array_get(
        system_refs, &system_data->ref_params, refs_index - 1);
    uint32_t i, count = ecs_array_count(system_data->base.columns);

    for (i = 0; i < count; i ++) {
        EcsSystemRef *ref = &refs[i];
        EcsEntity entity = ref->entity;
        if (!entity) {
            break;
        }

        info->refs_entity[i] = entity;
        info->refs_data[i] = ecs_get_ptr(world, entity, ref->component);
    }
}


/* -- Private functions -- */

/** Match new table against system (table is created after system) */
EcsResult ecs_system_notify_create_table(
    EcsWorld *world,
    EcsStage *stage,
    EcsEntity system,
    EcsTable *table)
{
    EcsTableSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (!system_data) {
        return EcsError;
    }

    if (match_table(world, stage, table, system, system_data)) {
        add_table(world, stage, system, system_data, table);
    }

    return EcsOk;
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
    EcsTableSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    EcsSystemKind kind = system_data->base.kind;

    uint32_t table_index = ecs_array_get_index(
        world->table_db, &table_arr_params, table);

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

    assert(i != count);

    uint32_t src_count = ecs_array_move_index(
        &dst_array, src_array, &system_data->table_params, i);

    if (active) {
        uint32_t dst_count = ecs_array_count(dst_array);
        if (kind != EcsOnDemand) {
            if (dst_count == 1 && system_data->base.enabled) {
                ecs_world_activate_system(
                    world, system, kind, true);
            }
        }
        system_data->tables = dst_array;
    } else {
        if (kind != EcsOnDemand) {
            if (src_count == 0) {
                ecs_world_activate_system(
                    world, system, kind, false);
            }
        }
        system_data->inactive_tables = dst_array;
    }
}

/** Run subset of the matching entities for a system (used in worker threads) */
void ecs_run_job(
    EcsWorld *world,
    EcsThread *thread,
    EcsJob *job)
{
    EcsEntity system = job->system;
    EcsTableSystem *system_data = job->system_data;
    EcsSystemAction action = system_data->base.action;
    uint32_t table_element_size = system_data->table_params.element_size;
    uint32_t component_element_size =
      system_data->component_params.element_size;
    uint32_t table_index = job->table_index;
    uint32_t start_index = job->start_index;
    uint32_t remaining = job->row_count;
    uint32_t column_count = ecs_array_count(system_data->base.columns);
    void *refs_data[column_count];
    EcsEntity refs_entity[column_count];
    int32_t *table_buffer = ecs_array_get(
        system_data->tables, &system_data->table_params, table_index);
    char *component_buffer = ecs_array_buffer(system_data->components);

    void *component_buffer_el = ECS_OFFSET(component_buffer,
        component_element_size * table_buffer[COMPONENTS_INDEX]);;

    EcsRows info = {
        .world = thread ? (EcsWorld*)thread : world,
        .system = system,
        .refs_data = refs_data,
        .refs_entity = refs_entity,
        .column_count = column_count
    };

    do {
        EcsTable *table = ecs_array_get(
            world->table_db, &table_arr_params, table_buffer[TABLE_INDEX]);
        EcsArray *rows = table->rows;
        void *start = ecs_array_get(rows, &table->row_params, start_index);
        uint32_t count = ecs_array_count(rows);
        uint32_t element_size = table->row_params.element_size;
        uint32_t refs_index = table_buffer[REFS_INDEX];

        component_buffer_el = ECS_OFFSET(component_buffer,
            component_element_size * table_buffer[COMPONENTS_INDEX]);

        info.element_size = element_size;
        info.columns = ECS_OFFSET(table_buffer, sizeof(uint32_t) * COLUMNS_INDEX);
        info.components = component_buffer_el;
        info.first = start;

        if (refs_index) {
            resolve_refs(world, system_data, refs_index, &info);
        }

        if (remaining >= count) {
            info.last = ECS_OFFSET(info.first, element_size * count);
            table_buffer = ECS_OFFSET(table_buffer, table_element_size);
            component_buffer_el = ECS_OFFSET(component_buffer,
                table_buffer[COMPONENTS_INDEX] * component_element_size);
            start_index = 0;
            remaining -= count;

        } else {
            info.last = ECS_OFFSET(info.first, element_size * remaining);
            remaining = 0;
        }

        action(&info);
        if (info.interrupted_by) break;
    } while (remaining);
}


/* -- Private API -- */

EcsEntity ecs_new_table_system(
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

    EcsEntity result = ecs_new_w_family(
        world, NULL, world->table_system_family);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId_h);
    *id_data = id;

    EcsTableSystem *system_data = ecs_get_ptr(world, result, EcsTableSystem_h);
    memset(system_data, 0, sizeof(EcsTableSystem));
    system_data->base.action = action;
    system_data->base.enabled = true;
    system_data->base.signature = sig;
    system_data->base.time_spent = 0;
    system_data->base.columns = ecs_array_new(&column_arr_params, count);
    system_data->base.kind = kind;
    system_data->table_params.element_size = sizeof(int32_t) * (count + 3);
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

    compute_and_families(world, system_data);
    match_tables(world, NULL, result, system_data);

    EcsEntity *elem = NULL;

    if (kind == EcsOnDemand) {
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

    if (system_data->and_from_system) {
        EcsArray *f = ecs_family_get(world, NULL, system_data->and_from_system);
        EcsEntity *buffer = ecs_array_buffer(f);
        uint32_t i, count = ecs_array_count(f);
        for (i = 0; i < count; i ++) {
            ecs_stage_add(world, result, buffer[i]);
        }
        ecs_commit(world, result);
    }

    return result;
}

/* -- Public API -- */

static
bool should_run(
    EcsTableSystem *system_data,
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

EcsEntity ecs_run_w_filter(
    EcsWorld *world,
    EcsEntity system,
    float delta_time,
    uint32_t offset,
    uint32_t limit,
    EcsEntity filter,
    void *param)
{
    EcsTableSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
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

    EcsTable *world_tables = ecs_array_buffer(real_world->table_db);
    uint32_t column_count = ecs_array_count(system_data->base.columns);
    uint32_t components_size = system_data->component_params.element_size;
    char *components = ecs_array_buffer(system_data->components);
    EcsFamily filter_id = 0;
    EcsEntity interrupted_by = 0;
    void *refs_data[column_count];
    EcsEntity refs_entity[column_count];
    EcsSystemAction action = system_data->base.action;
    bool offset_limit = offset | limit;
    bool limit_set = limit != 0;

    EcsRows info = {
        .world = world,
        .system = system,
        .param = param,
        .refs_entity = refs_entity,
        .refs_data = refs_data,
        .column_count = column_count,
        .delta_time = system_delta_time
    };

    if (filter) {
        filter_id = ecs_family_from_handle(real_world, stage, filter, NULL);
    }

    int32_t *table = table_first;
    for (; table < table_last; table = ECS_OFFSET(table, tables_size)) {
        int32_t table_index = table[TABLE_INDEX];
        EcsTable *w_table = &world_tables[table_index];

        if (filter_id) {
            if (!ecs_family_contains(
                real_world, stage, w_table->family_id, filter_id, true, true))
            {
                continue;
            }
        }

        EcsArray *rows = w_table->rows;
        uint32_t row_count = ecs_array_count(rows);
        uint32_t row_size = w_table->row_params.element_size;
        void *first = ecs_array_buffer(rows);
        void *last = ECS_OFFSET(first, row_size * row_count);

        if (offset_limit) {
            if (offset) {
                if (offset > row_count) {
                    offset -= row_count;
                    continue;
                } else {
                    first = ECS_OFFSET(first, row_size * offset);
                    row_count -= offset;
                    offset = 0;
                }
            }

            if (limit) {
                if (limit > row_count) {
                    limit -= row_count;
                } else {
                    last = ECS_OFFSET(first, row_size * limit);
                    limit = 0;
                }
            } else if (limit_set) {
                break;
            }
        }

        int32_t refs_index = table[REFS_INDEX];
        if (refs_index) {
            resolve_refs(world, system_data, refs_index, &info);
        }

        info.element_size = row_size;
        info.first = first;
        info.last = last;
        info.columns = ECS_OFFSET(table, sizeof(uint32_t) * COLUMNS_INDEX);
        info.components = ECS_OFFSET(components,
            components_size * table[COMPONENTS_INDEX]);

        action(&info);

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
