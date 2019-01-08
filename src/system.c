#include <assert.h>
#include <string.h>
#include "include/private/reflecs.h"
#include "include/util/time.h"

/** Create a new row system. A row system is a system executed on a single row,
 * typically as a result of a ADD, REMOVE or SET trigger.
 */
static
EcsHandle new_row_system(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    bool needs_tables,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = ecs_columns_count(sig);
    if (!count) {
        return 0;
    }

    EcsHandle result = ecs_new_w_family(world, NULL, world->row_system_family);
    EcsId *id_data = ecs_get_ptr(world, result, EcsId_h);
    *id_data = id;

    EcsRowSystem *system_data = ecs_get_ptr(world, result, EcsRowSystem_h);
    memset(system_data, 0, sizeof(EcsRowSystem));
    system_data->base.action = action;
    system_data->base.signature = sig;
    system_data->base.enabled = true;
    system_data->components = ecs_array_new(&handle_arr_params, count);

    if (ecs_parse_component_expr(
        world, sig, ecs_parse_component_action, system_data) != EcsOk)
    {
        printf("Expression error '%s'\n", sig);
        return 0;
    }

    EcsFamily family_id = 0;
    uint32_t i, column_count = ecs_array_count(system_data->base.columns);
    EcsSystemColumn *buffer = ecs_array_buffer(system_data->base.columns);

    for (i = 0; i < column_count; i ++) {
        EcsHandle *h = ecs_array_add(
            &system_data->components, &handle_arr_params);

        EcsSystemColumn *column = &buffer[i];
        *h = column->is.component;

        if (column->kind != EcsFromHandle) {
            family_id = ecs_family_add(
                world, NULL, family_id, column->is.component);
        }
    }

    EcsMap *index = NULL;
    if (kind == EcsOnAdd) {
        index = world->add_systems;
    } else if (kind == EcsOnRemove) {
        if (needs_tables) {
            index = world->remove_systems;
        }
    } else if (kind == EcsOnSet) {
        index = world->set_systems;
    }

    if (index) {
        assert(!ecs_map_has(index, family_id, NULL));
        ecs_map_set64(index, family_id, result);
    } else {
        if (kind == EcsOnRemove) {
            EcsHandle *system = ecs_array_add(
                &world->fini_tasks, &handle_arr_params);
            *system = result;
        } else if (kind == EcsOnFrame) {
            EcsHandle *system = ecs_array_add(
                &world->tasks, &handle_arr_params);
            *system = result;
        } else {
            assert(0);
        }
    }

    return result;
}

/* -- Private API -- */

/** Parse callback that adds component to the components array for a system */
EcsResult ecs_parse_component_action(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    void *data)
{
    EcsSystem *system_data = data;
    EcsSystemColumn *elem;

    /* Lookup component handly by string identifier */
    EcsHandle component = ecs_lookup(world, component_id);
    if (!component) {
        /* "0" is a valid expression used to indicate that a system matches no
         * components */
        if (strcmp(component_id, "0")) {
            ecs_abort(ECS_INVALID_COMPONENT_ID, component_id);
        }
    }

    /* If retrieving a component from a system, only the AND operator is
     * supported. The set of system components is expected to be constant, and
     * thus no conditional operators are needed. */
    if (elem_kind == EcsFromSystem && oper_kind != EcsOperAnd) {
        ecs_abort(ECS_INVALID_COMPONENT_EXPRESSION, 0);
    }

    /* AND (default) and optional columns are stored the same way */
    if (oper_kind == EcsOperAnd || oper_kind == EcsOperOptional) {
        elem = ecs_array_add(&system_data->columns, &column_arr_params);
        elem->kind = elem_kind;
        elem->oper_kind = oper_kind;
        elem->is.component = component;

    /* OR columns store a family id instead of a single component */
    } else if (oper_kind == EcsOperOr) {
        elem = ecs_array_last(system_data->columns, &column_arr_params);
        if (elem->oper_kind == EcsOperAnd) {
            elem->is.family = ecs_family_add(
                world, NULL, 0, elem->is.component);
        } else {
            if (elem->kind != elem_kind) {
                /* Cannot mix FromEntity and FromComponent in OR */
                goto error;
            }
        }

        elem->is.family = ecs_family_add(
            world, NULL, elem->is.family, component);
        elem->kind = elem_kind;
        elem->oper_kind = EcsOperOr;

    /* NOT columns are not added to the columns list. Instead, the system
     * stores two NOT familes; one for entities and one for components. These
     * can be quickly & efficiently used to exclude tables with
     * ecs_family_contains. */
    } else if (oper_kind == EcsOperNot) {
        if (elem_kind == EcsFromEntity) {
            system_data->not_from_entity =
                ecs_family_add(
                    world, NULL, system_data->not_from_entity, component);
        } else {
            system_data->not_from_component =
              ecs_family_add(
                  world, NULL, system_data->not_from_component, component);
        }
    }

    return EcsOk;
error:
    return EcsError;
}

/** Run system on a single row */
void ecs_row_notify(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle system,
    EcsRowSystem *system_data,
    EcsArray *rows,
    EcsArrayParams *row_params,
    uint32_t row_index,
    int32_t *columns)
{
    EcsSystemAction action = system_data->base.action;
    uint32_t column_count = ecs_array_count(system_data->components);
    EcsRows info = {
        .world = world,
        .system = system,
        .column_count = column_count,
        .columns = columns
    };

    info.element_size = row_params->element_size;
    info.first = ecs_array_get(rows, row_params, row_index);
    info.last = ECS_OFFSET(info.first, info.element_size);
    info.components = ecs_array_buffer(system_data->components);

    action(&info);
}

/** Run a task. A task is a system that contains no columns that can be matched
 * against a table. Examples of such columns are EcsFromSystem or EcsFromHandle.
 * Tasks are ran once every frame. */
void ecs_run_task(
    EcsWorld *world,
    EcsHandle system,
    float delta_time)
{
    EcsRowSystem *system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return;
    }

    bool measure_time = world->measure_system_time;
    struct timespec time_start;
    if (measure_time) {
        ut_time_get(&time_start);
    }

    EcsRows info = {
        .world = world,
        .system = system,
        .delta_time = delta_time,
        .components = ecs_array_buffer(system_data->components),
        .column_count = ecs_array_count(system_data->components)
    };

    system_data->base.action(&info);

    if (measure_time) {
        system_data->base.time_spent += ut_time_measure(&time_start);
    }
}

/* -- Public API -- */

EcsHandle ecs_new_system(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action)
{
    EcsHandle result = ecs_lookup(world, id);
    if (result) {
        return result;
    }

    bool needs_tables = ecs_needs_tables(world, sig);

    if (!needs_tables && (kind == EcsOnAdd ||
        kind == EcsOnSet))
    {
        ecs_abort(ECS_INVALID_PARAMETERS, 0);
    }

    if (needs_tables && (kind == EcsOnFrame || kind == EcsOnDemand)) {
        result = ecs_new_table_system(world, id, kind, sig, action);
    } else if (!needs_tables ||
        (kind == EcsOnAdd || kind == EcsOnRemove || kind == EcsOnSet))
    {
        result = new_row_system(world, id, kind, needs_tables, sig, action);
    } else {
        ecs_abort(ECS_INVALID_PARAMETERS, 0);
    }

    return result;
}

void* ecs_set_system_context_ptr(
    EcsWorld *world,
    EcsHandle system,
    EcsHandle component,
    void *value)
{
    EcsSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (!system_data) {
        system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
    }

    system_data->ctx_handle = component;

    ecs_set_ptr(world, system, component, value);
    void *result = ecs_get_ptr(world, system, component);
    assert(result != NULL);

    return result;
}

void* ecs_get_system_context(
    EcsWorld *world,
    EcsHandle system)
{
    EcsSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (!system_data) {
        system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
    }

    void *result = ecs_get_ptr(world, system, system_data->ctx_handle);
    if (!result) {
        ecs_abort(ECS_MISSING_SYSTEM_CONTEXT, 0);
    }

    return result;
}

void ecs_enable(
    EcsWorld *world,
    EcsHandle system,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    bool table_system = false;

    /* Try to get either TableSystem or RowSystem data */
    EcsSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (!system_data) {
        system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
    } else {
        table_system = true;
    }

    /* If entity is neither TableSystem nor RowSystem, it should be a family */
    if (!system_data) {
        EcsFamilyComponent *family_data = ecs_get_ptr(
            world, system, EcsFamilyComponent_h);

        assert(family_data != NULL);

        EcsWorld *world_temp = world;
        EcsStage *stage = ecs_get_stage(&world_temp);
        EcsArray *family = ecs_family_get(world, stage, family_data->family);
        EcsHandle *buffer = ecs_array_buffer(family);
        uint32_t i, count = ecs_array_count(family);
        for (i = 0; i < count; i ++) {
            /* Enable/disable all systems in family */
            ecs_enable(world, buffer[i], enabled);
        }
    } else {
        if (table_system) {
            EcsTableSystem *table_system = (EcsTableSystem*)system_data;
            if (enabled) {
                if (!system_data->enabled) {
                    if (ecs_array_count(table_system->tables)) {
                        ecs_world_activate_system(world, system, true);
                    }
                }
            } else {
                if (system_data->enabled) {
                    if (ecs_array_count(table_system->tables)) {
                        ecs_world_activate_system(world, system, false);
                    }
                }
            }
        }

        system_data->enabled = enabled;
    }
}

bool ecs_is_enabled(
    EcsWorld *world,
    EcsHandle system)
{
    EcsSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (!system_data) {
        system_data = ecs_get_ptr(world, system, EcsRowSystem_h);
    }

    if (system_data) {
        return system_data->enabled;
    } else {
        return true;
    }
}

void ecs_set_period(
    EcsWorld *world,
    EcsHandle system,
    float period)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    EcsTableSystem *system_data = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (system_data) {
        system_data->period = period;
    }
}
