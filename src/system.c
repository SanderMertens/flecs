#include "flecs_private.h"

static
void match_type(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsRowSystem *system_data,
    ecs_type_t type)
{
    /* Test if the components of the system are equal or a subset of the 
     * components of the type */
    ecs_entity_t match = ecs_type_contains(
        world, type, system_data->sig.and_from_self, true, false);

    /* If there is a match, add the system to the type-row_system index */
    if (match) {
        ecs_map_t *index = NULL;
        ecs_system_kind_t kind = system_data->base.kind;

        if (kind == EcsOnAdd) {
            index = world->type_sys_add_index;
        } else if (kind == EcsOnRemove) {
            index = world->type_sys_remove_index;
        } else if (kind == EcsOnSet) {
            index = world->type_sys_set_index;
        }

        ecs_assert(index != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_vector_t *systems = NULL;
        if (!ecs_map_has(index, (uintptr_t)type, &systems)) {
            systems = ecs_vector_new(ecs_entity_t, 1);
        }

        ecs_entity_t *new_elem = ecs_vector_add(&systems, ecs_entity_t);
        *new_elem = system;

        /* Always set the system entry, as array may have been realloc'd */
        ecs_map_set(index, (uintptr_t)type, &systems);
    }
}

/* Match system against existing types to build the type-rowsys index */
static
void match_families(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsRowSystem *system_data)
{
    ecs_type_link_t *link = &world->main_stage.type_root.link;

    do {
        match_type(world, system, system_data, link->type);
    } while ((link = link->next));
}

/** Create a new row system. A row system is a system executed on a single row,
 * typically as a result of a ADD, REMOVE or SET trigger.
 */
static
ecs_entity_t new_row_system(
    ecs_world_t *world,
    const char *id,
    ecs_system_kind_t kind,
    bool needs_tables,
    const char *sig,
    ecs_system_action_t action)
{
    int32_t count = ecs_columns_count(sig);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = _ecs_new(world, world->t_row_system);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId);
    ecs_assert(id_data != NULL, ECS_INTERNAL_ERROR, NULL);

    *id_data = id;

    EcsRowSystem *system_data = ecs_get_ptr(world, result, EcsRowSystem);
    memset(system_data, 0, sizeof(EcsRowSystem));
    system_data->base.action = action;
    system_data->base.enabled = true;
    system_data->base.invoke_count = 0;
    system_data->base.kind = kind;
    system_data->components = ecs_vector_new(ecs_entity_t, count);

    ecs_sig_init(world, sig, &system_data->sig);

    ecs_type_t type_id = 0;
    int32_t i, column_count = ecs_vector_count(system_data->sig.columns);
    ecs_sig_column_t *buffer = ecs_vector_first(system_data->sig.columns);

    for (i = 0; i < column_count; i ++) {
        ecs_entity_t *h = ecs_vector_add(
            &system_data->components, ecs_entity_t);

        ecs_sig_column_t *column = &buffer[i];
        *h = column->is.component;

        if (column->from_kind != EcsFromEmpty) {
            type_id = ecs_type_add_intern(
                world, NULL, type_id, column->is.component);
        }
    }

    ecs_entity_t *elem = NULL;

    if (!needs_tables && kind == EcsOnRemove) {
        elem = ecs_vector_add(&world->fini_tasks, ecs_entity_t);
    } else {
        if (kind == EcsOnAdd) {
            elem = ecs_vector_add(&world->add_systems, ecs_entity_t);
        } else if (kind == EcsOnRemove) {
            elem = ecs_vector_add(&world->remove_systems, ecs_entity_t);
        } else if (kind == EcsOnSet) {
            elem = ecs_vector_add(&world->set_systems, ecs_entity_t);
        }
    }

    if (elem) {
        *elem = result;
    }

    if (needs_tables) {
        match_families(world, result, system_data);
    }

    return result;
}

/* -- Private API -- */

/** Run system on a single row */
ecs_type_t ecs_notify_row_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_type_t type,
    ecs_table_t *table,
    ecs_column_t *table_columns,
    int32_t offset,
    int32_t limit)
{
    ecs_entity_info_t info = {.entity = system};
    ecs_world_t *real_world = world;
    ecs_get_stage(&real_world);

    EcsRowSystem *system_data = ecs_get_ptr_intern(
        real_world, &real_world->main_stage, &info, EEcsRowSystem, false, true);
    
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!system_data->base.enabled) {
        return false;
    }

    if (table && table->flags & EcsTableIsPrefab && 
        !system_data->sig.match_prefab) 
    {
        return 0;
    }

    ecs_system_action_t action = system_data->base.action;
    int32_t i, column_count = ecs_vector_count(system_data->sig.columns);
    ecs_sig_column_t *buffer = ecs_vector_first(system_data->sig.columns);
    int32_t *columns = ecs_os_alloca(int32_t, column_count);
    ecs_reference_t *references = ecs_os_alloca(ecs_reference_t, column_count);

    int32_t ref_id = 0;

    /* Iterate over system columns, resolve data from table or references */

    for (i = 0; i < column_count; i ++) {
        ecs_entity_t entity = 0;

        if (buffer[i].from_kind == EcsFromSelf) {
            /* If a regular column, find corresponding column in table */
            columns[i] = ecs_type_index_of(type, buffer[i].is.component) + 1;

            if (!columns[i] && table) {
                /* If column is not found, it could come from a prefab. Look for
                 * components of components */
                entity = ecs_get_entity_for_component(
                    real_world, 0, table->type, buffer[i].is.component);

                ecs_assert(entity != 0 || 
                           buffer[i].oper_kind == EcsOperOptional, 
                                ECS_INTERNAL_ERROR, 
                                ecs_get_id(real_world, buffer[i].is.component));
            }
        }

        if (entity || buffer[i].from_kind != EcsFromSelf) {
            /* If not a regular column, it is a reference */
            ecs_entity_t component = buffer[i].is.component;

            /* Resolve component from the right source */
            
            if (buffer[i].from_kind == EcsFromSystem) {
                /* The source is the system itself */
                entity = system;
            } else if (buffer[i].from_kind == EcsFromEntity) {
                /* The source is another entity (prefab, container, other) */
                entity = buffer[i].source;
            }

            /* Store the reference data so the system callback can access it */
            info = (ecs_entity_info_t){.entity = entity};
            references[ref_id] = (ecs_reference_t){
                .entity = entity, 
                .component = component,
                .cached_ptr = ecs_get_ptr_intern(real_world, 
                    &real_world->main_stage, &info, component, false, true)
            };

            /* Update the column vector with the entry to the ref vector */
            ref_id ++;
            columns[i] = -ref_id;
        }
    }

    /* Prepare ecs_rows_t for system callback */
    ecs_rows_t rows = {
        .world = world,
        .system = system,
        .columns = columns,
        .column_count = ecs_vector_count(system_data->components),
        .table = table,
        .table_columns = table_columns,
        .components = ecs_vector_first(system_data->components),
        .frame_offset = 0,
        .offset = offset,
        .count = limit,
        .param = system_data->base.ctx,
        .system_data = &system_data->base
    };

    /* Set references metadata if system has references */
    if (ref_id) {
        rows.references = references;
    }

    /* Obtain pointer to vector with entity identifiers */
    if (table_columns) {
        ecs_entity_t *entities = ecs_vector_first(table_columns[0].data);
        rows.entities = &entities[rows.offset];
    }

    /* Run system */
    action(&rows);

    /* Return the components that the system promised to init/read/fini */
    return system_data->sig.and_from_self;
}

/** Run a task. A task is a system that contains no columns that can be matched
 * against a table. Examples of such columns are EcsFromSystem or EcsFromEmpty.
 * Tasks are ran once every frame. */
void ecs_run_task(
    ecs_world_t *world,
    ecs_entity_t system)
{
    ecs_notify_row_system(world, system, NULL, NULL, NULL, 0, 1);
}

/* Notify row system of a new type */
void ecs_row_system_notify_of_type(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_type_t type)
{
    (void)stage;

    EcsRowSystem *system_data = ecs_get_ptr(world, system, EcsRowSystem);
    match_type(world, system, system_data, type);
}

/* -- Public API -- */

ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    const char *id,
    ecs_system_kind_t kind,
    const char *sig,
    ecs_system_action_t action)
{
    ecs_assert(kind == EcsManual ||
               kind == EcsOnLoad ||
               kind == EcsPostLoad ||
               kind == EcsPreUpdate ||
               kind == EcsOnUpdate ||
               kind == EcsOnValidate ||
               kind == EcsPostUpdate ||
               kind == EcsPreStore ||
               kind == EcsOnStore ||
               kind == EcsOnAdd ||
               kind == EcsOnRemove ||
               kind == EcsOnSet,
               ECS_INVALID_PARAMETER, NULL);

    bool needs_tables = ecs_needs_tables(world, sig, id);
    bool is_reactive = false;

    ecs_assert(needs_tables || 
        !((kind == EcsOnAdd) || (kind == EcsOnSet)),
        ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = 0;

    if (id) {
        result = ecs_lookup(world, id);
        if (result) {
            return result;
        }
    }

    if ((kind == EcsOnLoad || kind == EcsPostLoad ||
        kind == EcsPreUpdate || kind == EcsOnUpdate ||
        kind == EcsOnValidate || kind == EcsPostUpdate ||
        kind == EcsPreStore || kind == EcsOnStore ||
        kind == EcsManual))
    {
        result = ecs_new_col_system(world, id, kind, sig, action);
    } else if (!needs_tables ||
        (kind == EcsOnAdd || kind == EcsOnRemove || kind == EcsOnSet))
    {
        result = new_row_system(world, id, kind, needs_tables, sig, action);
        is_reactive = true;
    }

    ecs_assert(result != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_type_t and_from_system;
    EcsSystem *system_data;
    EcsColSystem* col_sys = ecs_get_ptr(world, result, EcsColSystem);

    if (col_sys) {
        system_data = &col_sys->base;
        and_from_system = col_sys->query->sig.and_from_system;
    } else {
        EcsRowSystem *row_sys = ecs_get_ptr(world, result, EcsRowSystem);
        ecs_assert(row_sys != NULL, ECS_INTERNAL_ERROR, NULL);
        system_data = &row_sys->base;
        and_from_system = row_sys->sig.and_from_system;
    }

    /* If system contains FromSystem params, add them tot the system */
    if (and_from_system) {
        ecs_entity_t *array = ecs_vector_first(and_from_system);
        int32_t i, count = ecs_vector_count(and_from_system);
        for (i = 0; i < count; i ++) {
            ecs_type_t type = ecs_type_from_entity(world, array[i]);
            _ecs_add(world, result, type);
        }
    }

    /* If this is an OnDemand system, disable if there is no interest for it */
    if (ecs_has(world, result, EcsOnDemand)) {
        ecs_assert(is_reactive == false, ECS_INVALID_PARAMETER, NULL);
        
        EcsColSystem *col_system_data = (EcsColSystem*)system_data;
        ecs_assert(col_system_data != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(col_system_data->on_demand != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If there are no systems currently interested in any of the [out]
         * columns of the on demand system, disable it */
        if (!col_system_data->on_demand->count) {
            ecs_enable(world, result, false);
        }
    }

    return result;
}

bool ecs_is_enabled(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsSystem *system_data = (EcsSystem*)ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        system_data = (EcsSystem*)ecs_get_ptr(world, system, EcsRowSystem);
    }

    if (system_data) {
        return system_data->enabled;
    } else {
        return true;
    }
}

void ecs_set_period(
    ecs_world_t *world,
    ecs_entity_t system,
    float period)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (system_data) {
        system_data->period = period;
    }
}

static
EcsSystem* get_system_ptr(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsSystem *result = NULL;
    EcsColSystem *cs = ecs_get_ptr(world, system, EcsColSystem);
    if (cs) {
        result = (EcsSystem*)cs;
    } else {
        EcsRowSystem *rs = ecs_get_ptr(world, system, EcsRowSystem);
        if (rs) {
            result = (EcsSystem*)rs;
        }
    }

    return result;
}

void ecs_set_system_context(
    ecs_world_t *world,
    ecs_entity_t system,
    const void *ctx)
{
    EcsSystem *system_data = get_system_ptr(world, system);

    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->ctx = (void*)ctx;
}

void* ecs_get_system_context(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsSystem *system_data = get_system_ptr(world, system);

    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    return system_data->ctx;
}
