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
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
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
                ecs_on_demand_out_t **out = ecs_vector_first(in->systems, ecs_on_demand_out_t*);
                int32_t s, count = ecs_vector_count(in->systems);

                for (s = 0; s < count; s ++) {
                    /* Increase the count of the system with the out params */
                    out[s]->count += activate ? 1 : -1;
                    
                    /* If this is the first out column that is requested from
                     * the OnDemand system, enable it */
                    if (activate && out[s]->count == 1) {
                        ecs_remove_entity(world, out[s]->system, EcsDisabledIntern);
                    } else if (!activate && !out[s]->count) {
                        ecs_add_entity(world, out[s]->system, EcsDisabledIntern);             
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
    EcsSystem *system_data)
{
    ecs_query_t *query = system_data->query;
    ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns, ecs_sig_column_t);
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
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    if (activate) {
        ecs_remove_entity(world, system, EcsInactive);
    }

    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    if (!system_data || !system_data->query) {
        return;
    }

    /* If system contains in columns, signal that they are now in use */
    activate_in_columns(
        world, system_data->query, world->on_activate_components, activate);

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
    EcsSystem *system_data,
    bool enabled)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    ecs_query_t *query = system_data->query;
    if (!query) {
        return;
    }

    if (ecs_vector_count(query->tables)) {
        /* Only (de)activate system if it has non-empty tables. */
        ecs_system_activate(world, system, enabled);
        system_data = ecs_get_mut(world, system, EcsSystem, NULL);
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

void ecs_init_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_view_action_t action,
    ecs_query_t *query,
    void *ctx)
{
    ecs_assert(!world->in_progress, ECS_INTERNAL_ERROR, NULL);

    ecs_trace_1("system #[green]%s#[reset] (%d) created with #[red]%s#[normal]", 
        ecs_get_name(world, system), system, query->sig.expr);

    ecs_trace_push();

    /* Add & initialize the EcsSystem component */
    bool is_added = false;
    EcsSystem *sptr = ecs_get_mut(world, system, EcsSystem, &is_added);
    ecs_assert(sptr != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!is_added) {
        ecs_assert(sptr->query == query, ECS_INVALID_PARAMETER, NULL);
    } else {
        memset(sptr, 0, sizeof(EcsSystem));
        sptr->query = query;
        sptr->entity = system;
        sptr->tick_source = 0;
        sptr->time_spent = 0;
    }

    /* Sanity check to make sure creating the query didn't add any additional
     * tags or components to the system */
    sptr->action = action;
    sptr->ctx = ctx;

    /* Only run this code when the system is created for the first time */
    if (is_added) {
        /* If tables have been matched with this system it is active, and we
        * should activate the in-columns, if any. This will ensure that any
        * OnDemand systems get enabled. */
        if (ecs_vector_count(query->tables)) {
            ecs_system_activate(world, system, true);
        } else {
            /* If system isn't matched with any tables, mark it as inactive. This
            * causes it to be ignored by the main loop. When the system matches
            * with a table it will be activated. */
            ecs_add_entity(world, system, EcsInactive);
        }

        /* If system is enabled, trigger enable components */
        activate_in_columns(world, query, world->on_enable_components, true);

        /* Check if all non-table column constraints are met. If not, disable
        * system (system will be enabled once constraints are met) */
        if (!ecs_sig_check_constraints(world, &query->sig)) {
            ecs_add_entity(world, system, EcsDisabledIntern);
        }

        /* If the query has a OnDemand system tag, register its [out] columns */
        if (ecs_has_entity(world, system, EcsOnDemand)) {
            sptr = ecs_get_mut(world, system, EcsSystem, NULL);

            register_out_columns(world, system, sptr);
            ecs_assert(sptr->on_demand != NULL, ECS_INTERNAL_ERROR, NULL);

            /* If there are no systems currently interested in any of the [out]
            * columns of the on demand system, disable it */
            if (!sptr->on_demand->count) {
                ecs_add_entity(world, system, EcsDisabledIntern);
            }        
        }
    }

    ecs_trace_pop();
}

/* -- Private API -- */

void ecs_invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    const EcsSystem *system_data,
    ecs_system_status_t status)
{
    ecs_system_status_action_t action = system_data->status_action;
    if (action) {
        action(world, system, status, system_data->status_ctx);
    }
}

void ecs_col_system_free(
    EcsSystem *system_data)
{
    ecs_query_free(system_data->query);
    ecs_vector_free(system_data->jobs);
}

/* -- Public API -- */

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled)
{
    assert(world->magic == ECS_WORLD_MAGIC);

    const EcsType *type_ptr = ecs_get( world, entity, EcsType);
    if (type_ptr) {
        /* If entity is a type, disable all entities in the type */
        ecs_vector_each(type_ptr->normalized, ecs_entity_t, e, {
            ecs_enable(world, *e, enabled);
        });
    } else {
        if (enabled) {
            ecs_remove_entity(world, entity, EcsDisabled);
        } else {
            ecs_add_entity(world, entity, EcsDisabled);
        }
    }
}

void ecs_set_system_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_status_action_t action,
    const void *ctx)
{
    EcsSystem *system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->status_action = action;
    system_data->status_ctx = (void*)ctx;

    if (!ecs_has_entity(world, system, EcsDisabled)) {
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
    EcsSystem *system_data,
    float delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param,
    bool ran_by_app) 
{
    if (!param) {
        param = system_data->ctx;
    }

    float time_elapsed = delta_time;
    ecs_entity_t tick_source = system_data->tick_source;

    if (tick_source) {
        const EcsTickSource *tick = ecs_get(
            real_world, tick_source, EcsTickSource);

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
    ecs_iter_t it = ecs_query_iter_page(system_data->query, offset, limit);
    it.world = world;
    it.system = system;
    it.delta_time = delta_time;
    it.delta_system_time = time_elapsed;
    it.world_time = real_world->stats.world_time_total;
    it.frame_offset = offset;
    
    /* Set param if provided, otherwise use system context */
    if (param) {
        it.param = param;
    } else {
        it.param = system_data->ctx;
    }

    ecs_view_action_t action = system_data->action;

    /* If no filter is provided, just iterate tables & invoke action */
    if (!filter) {
        if (ran_by_app || world == real_world) {
            while (ecs_query_next(&it)) {
                action(&it);
            }
        } else {
            ecs_thread_t *thread = (ecs_thread_t*)world;
            int32_t total = ecs_vector_count(real_world->workers);
            int32_t current = thread->index;

            while (ecs_query_next_worker(&it, current, total)) {
                action(&it);
            }
        }

    /* If filter is provided, match each table with the provided filter */
    } else {
        while (ecs_query_next(&it)) {
            ecs_table_t *table = it.table;
            if (!ecs_table_match_filter(real_world, table, filter))
            {
                continue;
            }

            action(&it);
        }        
    }

    if (measure_time) {
        system_data->time_spent += ecs_time_measure(&time_start);
    }
    
    system_data->invoke_count ++;

    return it.interrupted_by;
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

    EcsSystem *system_data = (EcsSystem*)ecs_get(
        real_world, system, EcsSystem);
    assert(system_data != NULL);

    ecs_entity_t interrupted_by = ecs_run_intern(
        world, real_world, system, system_data, delta_time, offset, limit, 
        filter, param, true);

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

void ecs_run_monitor(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_matched_query_t *monitor,
    ecs_entities_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities)
{
    ecs_query_t *query = monitor->query;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t system = query->system;
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!system_data->action) {
        return;
    }

    ecs_iter_t it = {0};
    ecs_query_set_view( world, stage, query, &it, 
        monitor->matched_table_index, row, count);

    it.triggered_by = components;
    it.param = system_data->ctx;

    if (entities) {
        it.entities = entities;
    }

    it.system = system;
    system_data->action(&it);
}
