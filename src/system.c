#include <assert.h>
#include <string.h>
#include "include/private/reflecs.h"

/** Parse callback that adds component to row system component array */
static
EcsResult add_component(
    EcsWorld *world,
    EcsSystemExprElemKind elem_kind,
    EcsSystemExprOperKind oper_kind,
    const char *component_id,
    void *data)
{
    EcsRowSystem *system_data = data;
    EcsHandle *elem = ecs_array_add(
        &system_data->components, &handle_arr_params);
    EcsHandle component = ecs_lookup(world, component_id);
    if (!component) {
        return EcsError;
    }

    if (oper_kind != EcsOperAnd) {
        return EcsError;
    }

    if (elem_kind != EcsFromEntity) {
        return EcsError;
    }

    *elem = component;

    return EcsOk;
}

static
EcsHandle new_row_system(
    EcsWorld *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    EcsSystemAction action)
{
    uint32_t count = ecs_parse_components_count(sig);
    if (!count) {
        return 0;
    }

    EcsHandle result = ecs_new_w_family(world, NULL, world->row_system_family);
    EcsId *id_data = ecs_get_ptr(world, result, EcsId_h);
    id_data->id = id;

    EcsRowSystem *system_data = ecs_get_ptr(world, result, EcsRowSystem_h);
    memset(system_data, 0, sizeof(EcsRowSystem));
    system_data->action = action;
    system_data->components = ecs_array_new(&handle_arr_params, count);

    if (ecs_parse_component_expr(
        world, sig, add_component, system_data) != EcsOk)
    {
        return 0;
    }

    EcsMap *index;
    if (kind == EcsOnAdd) {
        index = world->add_systems;
    } else if (kind == EcsOnRemove) {
        index = world->remove_systems;
    } else if (kind == EcsOnSet) {
        index = world->set_systems;
    }

    EcsFamily family_id = 0;
    uint32_t i, component_count = ecs_array_count(system_data->components);
    EcsHandle *buffer = ecs_array_buffer(system_data->components);
    for (i = 0; i < component_count; i ++) {
        family_id = ecs_family_add(world, NULL, family_id, buffer[i]);
    }

    assert(!ecs_map_has(index, family_id, NULL));

    ecs_map_set64(index, family_id, result);

    return result;
}

/* -- Private API -- */

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
    EcsSystemAction action = system_data->action;
    uint32_t column_count = ecs_array_count(system_data->components);
    EcsRows info = {
        .world = world,
        .system = system,
        .param = NULL,
        .refs = NULL,
        .column_count = column_count,
        .columns = columns
    };

    info.element_size = row_params->element_size;
    info.first = ecs_array_get(rows, row_params, row_index);
    info.last = ECS_OFFSET(info.first, info.element_size);
    info.components = ecs_array_buffer(system_data->components);

    action(&info);
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

    if (kind == EcsOnFrame || kind == EcsPeriodic || kind == EcsOnDemand) {
        result = ecs_new_table_system(world, id, kind, sig, action);
    } else if (kind == EcsOnAdd || kind == EcsOnRemove || kind == EcsOnSet) {
        result = new_row_system(world, id, kind, sig, action);
    } else {
        abort();
    }

    return result;
}

void* ecs_set_system_context_ptr(
    EcsWorld *world,
    EcsHandle system,
    EcsHandle component,
    void *value)
{
    EcsTableSystem *table_system = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (table_system) {
        table_system->ctx_handle = component;
    } else {
        EcsRowSystem *row_system = ecs_get_ptr(world, system, EcsRowSystem_h);
        if (row_system) {
            row_system->ctx_handle = component;
        } else {
            abort();
        }
    }

    ecs_set_ptr(world, system, component, value);
    void *result = ecs_get_ptr(world, system, component);
    assert(result != NULL);

    return result;
}

void* ecs_get_system_context(
    EcsWorld *world,
    EcsHandle system)
{
    EcsHandle component = 0;

    EcsTableSystem *table_system = ecs_get_ptr(world, system, EcsTableSystem_h);
    if (table_system) {
        component = table_system->ctx_handle;
    } else {
        EcsRowSystem *row_system = ecs_get_ptr(world, system, EcsRowSystem_h);
        if (row_system) {
            component = row_system->ctx_handle;
        } else {
            abort();
        }
    }

    void *result = ecs_get_ptr(world, system, component);
    assert(result != NULL);

    return result;
}
