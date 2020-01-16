#include "flecs_private.h"

static
ecs_on_demand_in_t* get_in_component(
    ecs_map_t *component_map,
    ecs_entity_t component)
{
    ecs_on_demand_in_t *in = ecs_map_get(
        component_map, ecs_on_demand_in_t, component);
    if (!in) {
        ecs_on_demand_in_t in_value = {0};
        ecs_map_set(component_map, component, &in_value);
        in = ecs_map_get(component_map, ecs_on_demand_in_t, component);
        ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return in;
}

static
void activate_in_columns(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_map_t *component_map,
    bool activate)
{
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns);
    int32_t i, count = ecs_vector_count(query->sig.columns);

    for (i = 0; i < count; i ++) {
        if (columns[i].inout_kind == EcsIn) {
            ecs_on_demand_in_t *in = get_in_component(
                component_map, columns[i].is.component);

            ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

            in->count += activate ? 1 : -1;

            ecs_assert(in->count >= 0, ECS_INTERNAL_ERROR, NULL);

            /* If this is the first system that registers the in component, walk
             * over all already registered systems to enable them */
            if (in->systems && 
               ((activate && in->count == 1) || 
                (!activate && !in->count))) 
            {
                ecs_on_demand_out_t **out = ecs_vector_first(in->systems);
                int32_t s, count = ecs_vector_count(in->systems);

                for (s = 0; s < count; s ++) {
                    /* Increase the count of the system with the out params */
                    out[s]->count += activate ? 1 : -1;
                    
                    /* If this is the first out column that is requested from
                     * the OnDemand system, enable it */
                    if (activate && out[s]->count == 1) {
                        ecs_enable(world, out[s]->system, true);
                    } else if (!activate && !out[s]->count) {                
                        ecs_enable(world, out[s]->system, false);
                    }
                }
            }
        }
    }    
}

static
void register_out_column(
    ecs_map_t *component_map,
    ecs_entity_t component,
    ecs_on_demand_out_t *on_demand_out)
{
    ecs_on_demand_in_t *in = get_in_component(component_map, component);
    ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

    on_demand_out->count += in->count;
    ecs_on_demand_out_t **elem = ecs_vector_add(&in->systems, ecs_on_demand_out_t*);
    *elem = on_demand_out;
}

static
void register_out_columns(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data)
{
    ecs_query_t *query = system_data->query;
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns);
    int32_t i, out_count = 0, count = ecs_vector_count(query->sig.columns);

    for (i = 0; i < count; i ++) {
        if (columns[i].inout_kind == EcsOut) {
            if (!system_data->on_demand) {
                system_data->on_demand = ecs_os_malloc(sizeof(ecs_on_demand_out_t));
                ecs_assert(system_data->on_demand != NULL, ECS_OUT_OF_MEMORY, NULL);

                system_data->on_demand->system = system;
                system_data->on_demand->count = 0;
            }

            /* If column operator is NOT and the inout kind is [out], the system
             * explicitly states that it will create the component (it is not
             * there, yet it is an out column). In this case it doesn't make
             * sense to wait until [in] columns get activated (matched with
             * entities) since the component is not there yet. Therefore add it
             * to the on_enable_components list, so this system will be enabled
             * when a [in] column is enabled, rather than activated */
            ecs_map_t *component_map;
            if (columns[i].oper_kind == EcsOperNot) {
                component_map = world->on_enable_components;
            } else {
                component_map = world->on_activate_components;
            }

            register_out_column(
                component_map, columns[i].is.component, 
                system_data->on_demand);

            out_count ++;
        }
    }

    /* If there are no out columns in the on-demand system, the system will
     * never be enabled */
    ecs_assert(out_count != 0, ECS_NO_OUT_COLUMNS, NULL);
}

/* -- Private API -- */

void ecs_invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data,
    ecs_system_status_t status)
{
    ecs_system_status_action_t action = system_data->status_action;
    if (action) {
        action(world, system, status, system_data->status_ctx);
    }
}

/* Invoked when system becomes active or inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* If system contains in columns, signal that they are now in use */
    activate_in_columns(world, system_data->query, world->on_activate_components, activate);

    /* Invoke system status action */
    ecs_invoke_status_action(world, system, system_data, 
        activate ? EcsSystemActivated : EcsSystemDeactivated);
}

void ecs_col_system_free(
    EcsColSystem *system_data)
{
    ecs_query_free(system_data->query);
    ecs_vector_free(system_data->jobs);
}

ecs_entity_t ecs_new_col_system(
    ecs_world_t *world,
    const char *id,
    ecs_system_kind_t kind,
    ecs_sig_t *sig,
    ecs_system_action_t action)
{
    ecs_entity_t result = _ecs_new(
        world, world->t_col_system);

    EcsId *id_data = ecs_get_ptr(world, result, EcsId);
    *id_data = id;

    EcsColSystem *system_data = ecs_get_ptr(world, result, EcsColSystem);
    memset(system_data, 0, sizeof(EcsColSystem));
    system_data->base.action = action;
    system_data->base.enabled = true;
    system_data->base.time_spent = 0;
    system_data->base.kind = kind;
    system_data->query = ecs_query_new_w_sig(world, result, sig);
    ecs_assert(system_data->query != NULL, ECS_INTERNAL_ERROR, NULL);

    uint32_t column_count = ecs_vector_count(system_data->query->sig.columns);

    system_data->column_size = sizeof(int32_t) * (column_count);
    system_data->ref_size = sizeof(ecs_reference_t) * column_count;
    system_data->component_size = sizeof(ecs_entity_t) * column_count;
    system_data->period = 0;
    system_data->entity = result;

    ecs_entity_t *elem = NULL;

    if (!ecs_vector_count(system_data->query->tables)) {
        elem = ecs_vector_add(&world->inactive_systems, ecs_entity_t);
    } else {
        if (kind == EcsManual) {
            elem = ecs_vector_add(&world->manual_systems, ecs_entity_t);
        } else if (kind == EcsOnUpdate) {
            elem = ecs_vector_add(&world->on_update_systems, ecs_entity_t);
        } else if (kind == EcsOnValidate) {
            elem = ecs_vector_add(&world->on_validate_systems, ecs_entity_t);            
        } else if (kind == EcsPreUpdate) {
            elem = ecs_vector_add(&world->pre_update_systems, ecs_entity_t);
        } else if (kind == EcsPostUpdate) {
            elem = ecs_vector_add(&world->post_update_systems, ecs_entity_t);
        } else if (kind == EcsOnLoad) {
            elem = ecs_vector_add(&world->on_load_systems, ecs_entity_t);
        } else if (kind == EcsPostLoad) {
            elem = ecs_vector_add(&world->post_load_systems, ecs_entity_t);            
        } else if (kind == EcsPreStore) {
            elem = ecs_vector_add(&world->pre_store_systems, ecs_entity_t);
        } else if (kind == EcsOnStore) {
            elem = ecs_vector_add(&world->on_store_systems, ecs_entity_t);
        }
    }

    *elem = result;

    /* If tables have been matched with this system it is active, and we
     * should activate the in-columns, if any. This will ensure that any
     * OnDemand systems get enabled. */
    if (ecs_vector_count(system_data->query->tables)) {
        ecs_system_activate(world, result, true);
    }

    /* If system is enabled, trigger enable components */
    if (system_data->base.enabled) {
        activate_in_columns(
            world, system_data->query, 
            world->on_enable_components, 
            true);   
    }

    /* Check if all non-table column constraints are met. If not, disable
    * system (system will be enabled once constraints are met) */
    if (!ecs_sig_check_constraints(world, &system_data->query->sig)) {
        ecs_enable(world, result, false);
    }

    /* If the query has a OnDemand system tag, register its [out] columns */
    if (ecs_type_contains(
        world, system_data->query->sig.and_from_system, 
        TEcsOnDemand, true, false)) 
    {
        register_out_columns(world, result, system_data);
    }

    return result;
}


/* -- Public API -- */

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t system,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);
    bool col_system = false;

    /* Try to get either ColSystem or RowSystem data */
    EcsSystem *system_data = (EcsSystem*)ecs_get_ptr(world, system, EcsColSystem);
    if (!system_data) {
        system_data = (EcsSystem*)ecs_get_ptr(world, system, EcsRowSystem);
    } else {
        col_system = true;
    }

    if (system_data) {
        if (col_system) {
            EcsColSystem *col_system_data = (EcsColSystem*)system_data;

            if (system_data->enabled != enabled) {
                system_data->enabled = enabled;

                if (ecs_vector_count(col_system_data->query->tables)) {
                    /* Only (de)activate system if it has non-empty tables. */
                    ecs_world_activate_system(
                        world, system, col_system_data->base.kind, enabled);
                }

                /* Enable/disable systems that trigger on [in] enablement */
                activate_in_columns(
                    world, 
                    col_system_data->query, 
                    world->on_enable_components, 
                    enabled);
                
                /* Invoke action for enable/disable status */
                ecs_invoke_status_action(
                    world, system, col_system_data, 
                    enabled ? EcsSystemEnabled : EcsSystemDisabled);
            }
        } else {
            system_data->enabled = enabled;
        }
    } else {
        /* If entity is neither ColSystem nor RowSystem, it should be a type */
        EcsTypeComponent *type_data = ecs_get_ptr(
            world, system, EcsTypeComponent);

        assert(type_data != NULL);

        ecs_type_t type = type_data->type;
        ecs_entity_t *array = ecs_vector_first(type);
        int32_t i, count = ecs_vector_count(type);
        for (i = 0; i < count; i ++) {
            /* Enable/disable all systems in type */
            ecs_enable(world, array[i], enabled);
        }        
    }
}

void ecs_set_system_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_action_t action,
    const void *ctx)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->status_action = action;
    system_data->status_ctx = (void*)ctx;

    if (system_data->base.enabled) {
        /* If system is already enabled, generate enable status. The API 
         * should guarantee that it exactly matches enable-disable 
         * notifications and activate-deactivate notifications. */
        ecs_invoke_status_action(world, system, system_data, EcsSystemEnabled);

        /* If column system has active (non-empty) tables, also generate the
         * activate status. */
        if (ecs_vector_count(system_data->query->tables)) {
            ecs_invoke_status_action(
                world, system, system_data, EcsSystemActivated);
        }
    }
}

ecs_entity_t _ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    int32_t offset,
    int32_t limit,
    ecs_type_t filter,
    void *param)
{
    ecs_world_t *real_world = world;

    if (world->magic == ECS_THREAD_MAGIC) {
        real_world = ((ecs_thread_t*)world)->world; /* dispel the magic */
    }

    EcsColSystem *system_data = ecs_get_ptr( real_world, system, EcsColSystem);
    assert(system_data != NULL);

    if (!system_data->base.enabled) {
        return 0;
    }

    ecs_get_stage(&real_world);

    /* If system should run at a fixed time interval, test if system should run
     * this iteration */
    float period = system_data->period;
    float time_passed = system_data->time_passed + delta_time;

    if (time_passed >= period) {
        float t = time_passed - period;
        if (t > period) {
            t = 0;
        }

        system_data->time_passed = t;
    } else {
        system_data->time_passed = time_passed;
        return 0;
    }

    /* If system profiling is enabled, record the time spent in the system */
    ecs_time_t time_start;
    bool measure_time = real_world->measure_system_time;
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    /* Prepare the query iterator */
    ecs_query_iter_t qiter = ecs_query_iter(system_data->query, offset, limit);
    qiter.rows.world = world;
    qiter.rows.system = system;
    qiter.rows.delta_time = delta_time;
    qiter.rows.delta_system_time = time_passed;
    qiter.rows.world_time = world->world_time_total;
    qiter.rows.frame_offset = offset;
    
    /* Set param if provided, otherwise use system context */
    if (param) {
        qiter.rows.param = param;
    } else {
        qiter.rows.param = system_data->base.ctx;
    }

    ecs_system_action_t action = system_data->base.action;

    /* If no filter is provided, just iterate tables & invoke action */
    if (!filter) {
        while (ecs_query_next(&qiter)) {
            action(&qiter.rows);
        }

    /* If filter is provided, match each table with the provided filter */
    } else {
        while (ecs_query_next(&qiter)) {
            ecs_table_t *table = qiter.rows.table;
            if (!ecs_type_contains(real_world, table->type, filter, true, true))
            {
                continue;
            }

            action(&qiter.rows);
        }        
    }

    if (measure_time) {
        system_data->base.time_spent += ecs_time_measure(&time_start);
    }    

    return qiter.rows.interrupted_by;
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, 0, param);
}
