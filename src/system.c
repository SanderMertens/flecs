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
    uint32_t i, count = ecs_vector_count(query->sig.columns);

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
                uint32_t s, count = ecs_vector_count(in->systems);

                for (s = 0; s < count; s ++) {
                    /* Increase the count of the system with the out params */
                    out[s]->count += activate ? 1 : -1;
                    
                    /* If this is the first out column that is requested from
                     * the OnDemand system, enable it */
                    if (activate && out[s]->count == 1) {
                        EcsColSystem *ptr = ecs_get_ptr(world, out[s]->system, EcsColSystem);
                        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
                        ecs_enable_intern(world, out[s]->system, ptr, true, false);
                    } else if (!activate && !out[s]->count) {
                        EcsColSystem *ptr = ecs_get_ptr(world, out[s]->system, EcsColSystem); 
                        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);               
                        ecs_enable_intern(world, out[s]->system, ptr, false, false);
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
    ecs_assert(out_count != 0, ECS_NO_OUT_COLUMNS, ecs_get_name(world, system));
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

    ecs_trace_1("system #[green]%s#[reset] %s", 
        ecs_get_name(world, system), 
        activate ? "activated" : "deactivated");
}

/* Actually enable or disable system */
void ecs_enable_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data,
    bool enabled)
{
    ecs_query_t *query = system_data->query;

    if (system_data->enabled != enabled) {
        system_data->enabled = enabled;

        if (ecs_vector_count(query->tables)) {
            /* Only (de)activate system if it has non-empty tables. */
            ecs_world_activate_system(
                world, system, system_data->kind, enabled);
        }

        /* Enable/disable systems that trigger on [in] enablement */
        activate_in_columns(
            world, 
            query, 
            world->on_enable_components, 
            enabled);
        
        /* Invoke action for enable/disable status */
        ecs_invoke_status_action(
            world, system, system_data,
            enabled ? EcsSystemEnabled : EcsSystemDisabled);
    }
}

/* Evaluate user & demand state, and determine whether the system actually needs
 * to be enabled or disabled */
void ecs_enable_intern(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsColSystem *system_data,
    bool enabled,
    bool by_user)
{
    if (by_user) {
        /* If the user state did not change, nothing needs to be done */
        if (system_data->enabled_by_user != enabled) {
            if (enabled) {
                ecs_assert(
                    system_data->enabled == false, 
                    ECS_INTERNAL_ERROR, 
                    NULL);

                if (system_data->enabled_by_demand) {
                    /* System can be turned on as there is demand for it */                    
                    ecs_enable_system(world, system, system_data, true);

                    ecs_trace_1("system #[green]%s#[reset] %s by user", 
                        ecs_get_name(world, system), 
                        enabled ? "enabled" : "disabled");                    
                } else {
                    /* User enabled system but there is no demand for it, so
                        * don't turn it on. System will turn on when demand is
                        * created. */                        
                }
            } else {
                if (!system_data->enabled_by_demand) {
                    ecs_assert(
                        system_data->enabled == false, 
                        ECS_INTERNAL_ERROR, 
                        NULL);

                    /* If the user disabled the system but it was already
                        * disabled because there was no demand for it, nothing
                        * needs to be done right now. */                       
                } else {
                    ecs_assert(
                        system_data->enabled == true, 
                        ECS_INTERNAL_ERROR, 
                        NULL);

                    /* If the user disabled the system and it was enabled
                        * because there was demand for it, it needs to be
                        * disabled. */
                    ecs_enable_system(world, system, system_data, false);

                    ecs_trace_1("system #[green]%s#[reset] %s by user", 
                        ecs_get_name(world, system), 
                        enabled ? "enabled" : "disabled");                    
                }
            }

            system_data->enabled_by_user = enabled;
        }
    } else {
        /* If the demand state did not change, nothing needs to be done */
        if (system_data->enabled_by_demand != enabled) {
            if (enabled) {
                ecs_assert(
                    system_data->enabled == false, 
                    ECS_INTERNAL_ERROR, 
                    NULL);

                if (system_data->enabled_by_user) {
                    /* System can be turned on since the user has enabled it
                        * and there is now demand. */
                    ecs_enable_system(world, system, system_data, true);

                    ecs_trace_1("system #[green]%s#[reset] %s by framework", 
                        ecs_get_name(world, system), 
                        enabled ? "enabled" : "disabled");                    
                } else {
                    /* Demand was created, but the user explicitly disabled
                        * the system. Don't do anything. */
                }
            } else {
                if (!system_data->enabled_by_user) {
                    ecs_assert(
                        system_data->enabled == false, 
                        ECS_INTERNAL_ERROR, 
                        NULL); 

                    /* If the system is disabled because the user disabled
                        * it and now there is also no more demand for it,
                        * nothing needs to be done. */
                } else {
                    ecs_assert(
                        system_data->enabled == true, 
                        ECS_INTERNAL_ERROR, 
                        NULL);                        

                    /* If the system was enabled by the user and now there
                        * is no more demand for it, disable the system */
                    ecs_enable_system(world, system, system_data, false);

                    ecs_trace_1("system #[green]%s#[reset] %s by framework", 
                        ecs_get_name(world, system), 
                        enabled ? "enabled" : "disabled");                    
                }
            }

            system_data->enabled_by_demand = enabled;
        }
    }
}

static 
void set_system(
    ecs_world_t *world,
    ecs_entity_t system,
    const char *name,
    ecs_system_kind_t kind,
    ecs_iter_action_t action,
    char *signature)
{
    /* Parse signature */
    ecs_sig_t sig = {0};
    ecs_sig_init(world, name, signature, &sig);

    ecs_trace_1("system #[green]%s#[reset] (%d) created with #[red]%s#[normal]", 
        name, system, signature);
    ecs_trace_push();

    bool is_added = false;
    EcsColSystem *system_data = ecs_get_mutable(
        world, system, EcsColSystem, &is_added);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(is_added == true, ECS_UNSUPPORTED, NULL);

    memset(system_data, 0, sizeof(EcsColSystem));
    system_data->action = action;
    system_data->enabled = true;
    system_data->enabled_by_user = true;
    system_data->enabled_by_demand = true;
    system_data->time_spent = 0;
    system_data->kind = kind;
    system_data->query = ecs_query_new_w_sig(world, system, &sig);
    ecs_assert(system_data->query != NULL, ECS_INTERNAL_ERROR, NULL);

    system_data->tick_source = 0;
    system_data->entity = system;

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

        /* If tables have been matched with this system it is active, and we
        * should activate the in-columns, if any. This will ensure that any
        * OnDemand systems get enabled. */
        ecs_system_activate(world, system, true);
    }

    *elem = system;

    /* If system is enabled, trigger enable components */
    if (system_data->enabled) {
        activate_in_columns(
            world, system_data->query, 
            world->on_enable_components, 
            true);   
    }

    /* Check if all non-table column constraints are met. If not, disable
     * system (system will be enabled once constraints are met) */
    if (!ecs_sig_check_constraints(world, &system_data->query->sig)) {
        ecs_enable_intern(world, system, system_data, false, false);
    }

    /* If the query has a OnDemand system tag, register its [out] columns */
    if (ecs_type_contains(
        world, system_data->query->sig.and_from_system, 
        TEcsOnDemand, true, false)) 
    {
        register_out_columns(world, system, system_data);
    }

    /* If system contains FromSystem params, add them tot the system */
    ecs_type_t and_from_system = system_data->query->sig.and_from_system;
    if (and_from_system) {
        ecs_entity_t *array = ecs_vector_first(and_from_system);
        int32_t i, count = ecs_vector_count(and_from_system);
        for (i = 0; i < count; i ++) {
            ecs_add_entity(world, system, array[i]);
        }

        /* Re-obtain system_data, as it might have changed */
        system_data = ecs_get_ptr(world, system, EcsColSystem);
        ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    /* If this is an OnDemand system, register its [out] columns */
    if (ecs_has(world, system, EcsOnDemand)) {
        ecs_assert(system_data->on_demand != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If there are no systems currently interested in any of the [out]
         * columns of the on demand system, disable it */
        if (!system_data->on_demand->count) {
            ecs_enable_intern(world, system, system_data, false, false);
        }
    } 

    ecs_trace_pop();
}

static 
void EcsOnSetSystem(
    ecs_rows_t *rows)
{
    ECS_COLUMN(rows, EcsSystem, sys, 1);

    ecs_world_t *world = rows->world;
    ecs_entity_t *entities = rows->entities;

    int32_t i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = entities[i];
        set_system(world, entities[i], ecs_get_name(world, e), sys[i].kind, 
            sys[i].action, sys[i].signature);
    }
}

/* -- Private API -- */

void ecs_init_system_builtins(
    ecs_world_t *world)
{
    ECS_TRIGGER(world, EcsOnSetSystem, EcsOnSet, EcsSystem, 0);
}

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

void ecs_col_system_free(
    EcsColSystem *system_data)
{
    ecs_query_free(system_data->query);
    ecs_vector_free(system_data->jobs);
}

/* -- Public API -- */

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t system,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (system_data) {
        ecs_enable_intern(world, system, system_data, enabled, true);
    } else {
        /* If entity is neither ColSystem nor RowSystem, it should be a type */
        EcsType *type_data = ecs_get_ptr(
            world, system, EcsType);

        assert(type_data != NULL);

        ecs_type_t type = type_data->type;
        ecs_entity_t *array = ecs_vector_first(type);
        uint32_t i, count = ecs_vector_count(type);
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

    if (system_data->enabled) {
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

ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_world_t *real_world,
    ecs_entity_t system,
    float delta_time,
    uint32_t offset,
    uint32_t limit,
    const ecs_filter_t *filter,
    void *param) 
{
    EcsColSystem *system_data = ecs_get_ptr(real_world, system, EcsColSystem);
    assert(system_data != NULL);

    if (!system_data->enabled) {
        return 0;
    }

    if (!param) {
        param = system_data->ctx;
    }

    float time_elapsed = delta_time;
    ecs_entity_t tick_source = system_data->tick_source;

    if (tick_source) {
        EcsTickSource *tick = ecs_get_ptr(real_world, tick_source, EcsTickSource);

        if (tick) {
            time_elapsed = tick->time_elapsed;

            /* If timer hasn't fired we shouldn't run the system */
            if (!tick->tick) {
                return 0;
            }
        } else {
            /* If a timer has been set but the timer entity does not have the
             * EcsTimer component, don't run the system. This can be the result
             * of a single-shot timer that has fired already. Not resetting the
             * timer field of the system will ensure that the system won't be
             * ran after the timer has fired. */
            return 0;
        }
    }

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
    qiter.rows.delta_system_time = time_elapsed;
    qiter.rows.world_time = real_world->world_time_total;
    qiter.rows.frame_offset = offset;
    
    /* Set param if provided, otherwise use system context */
    if (param) {
        qiter.rows.param = param;
    } else {
        qiter.rows.param = system_data->ctx;
    }

    ecs_iter_action_t action = system_data->action;

    /* If no filter is provided, just iterate tables & invoke action */
    if (!filter) {
        while (ecs_query_next(&qiter)) {
            action(&qiter.rows);
        }

    /* If filter is provided, match each table with the provided filter */
    } else {
        while (ecs_query_next(&qiter)) {
            ecs_table_t *table = qiter.rows.table;
            if (!ecs_table_match_filter(real_world, table, filter))
            {
                continue;
            }

            action(&qiter.rows);
        }        
    }

    if (measure_time) {
        system_data->time_spent += ecs_time_measure(&time_start);
    }
    
    system_data->invoke_count ++;

    return qiter.rows.interrupted_by;
}

/* -- Public API -- */

ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param)
{
    ecs_world_t *real_world = world;
    if (world->magic == ECS_THREAD_MAGIC) {
        real_world = ((ecs_thread_t*)world)->world; /* dispel the magic */
    }

    ecs_get_stage(&real_world);
    bool in_progress = ecs_staging_begin(real_world);

    ecs_entity_t interrupted_by = ecs_run_intern(
        world, real_world, system, delta_time, offset, limit, filter, param);

    /* If world wasn't in progress when we entered this function, we need to
     * merge and reset the in_progress value */
    ecs_staging_end(real_world, in_progress);

    return interrupted_by;
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    float delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, NULL, param);
}

bool ecs_is_enabled(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    if (system_data) {
        return system_data->enabled;
    } else {
        return true;
    }
}

void ecs_set_system_context(
    ecs_world_t *world,
    ecs_entity_t system,
    const void *ctx)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);
    system_data->ctx = (void*)ctx;
}

void* ecs_get_system_context(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);
    return system_data->ctx;
}
