#include "flecs.h"

#ifdef FLECS_SYSTEM

#include "../../private_api.h"
#include "system.h"

ecs_mixins_t ecs_system_t_mixins = {
    .type_name = "ecs_system_t",
    .elems = {
        [EcsMixinWorld] = offsetof(ecs_system_t, world),
        [EcsMixinEntity] = offsetof(ecs_system_t, entity),
        [EcsMixinDtor] = offsetof(ecs_system_t, dtor)
    }
};

static
void invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    const ecs_system_t *system_data,
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
    bool activate,
    const ecs_system_t *system_data)
{
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);

    if (activate) {
        /* If activating system, ensure that it doesn't have the Inactive tag.
         * Systems are implicitly activated so they are kept out of the main
         * loop as long as they aren't used. They are not implicitly deactivated
         * to prevent overhead in case of oscillating app behavior. 
         * After activation, systems that aren't matched with anything can be
         * deactivated again by explicitly calling ecs_deactivate_systems.
         */
        ecs_remove_id(world, system, EcsInactive);
    }

    if (!system_data) {
        system_data = ecs_poly_get(world, system, ecs_system_t);
    }
    if (!system_data || !system_data->query) {
        return;
    }

    if (!activate) {
        if (ecs_has_id(world, system, EcsDisabled)) {
            if (!ecs_query_table_count(system_data->query)) {
                /* If deactivating a disabled system that isn't matched with
                 * any active tables, there is nothing to deactivate. */
                return;
            }
        }            
    }

    /* Invoke system status action */
    invoke_status_action(world, system, system_data, 
        activate ? EcsSystemActivated : EcsSystemDeactivated);

    ecs_dbg_2("#[green]system#[reset] %s %s", 
        ecs_get_name(world, system), 
        activate ? "activated" : "deactivated");
}

/* Actually enable or disable system */
static
void ecs_enable_system(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_t *system_data,
    bool enabled)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INTERNAL_ERROR, NULL);

    ecs_query_t *query = system_data->query;
    if (!query) {
        return;
    }

    if (ecs_query_table_count(query)) {
        /* Only (de)activate system if it has non-empty tables. */
        ecs_system_activate(world, system, enabled, system_data);
    }
    
    /* Invoke action for enable/disable status */
    invoke_status_action(
        world, system, system_data,
        enabled ? EcsSystemEnabled : EcsSystemDisabled);
}

/* -- Public API -- */

ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_system_t *system_data,
    int32_t stage_current,
    int32_t stage_count,    
    ecs_ftime_t delta_time,
    int32_t offset,
    int32_t limit,
    void *param) 
{
    ecs_ftime_t time_elapsed = delta_time;
    ecs_entity_t tick_source = system_data->tick_source;

    /* Support legacy behavior */
    if (!param) {
        param = system_data->ctx;
    }

    if (tick_source) {
        const EcsTickSource *tick = ecs_get(
            world, tick_source, EcsTickSource);

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
    bool measure_time = ECS_BIT_IS_SET(world->flags, EcsWorldMeasureSystemTime);
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    ecs_world_t *thread_ctx = world;
    if (stage) {
        thread_ctx = stage->thread_ctx;
    }

    ecs_defer_begin(thread_ctx);

    /* Prepare the query iterator */
    ecs_iter_t pit, wit, qit = ecs_query_iter(thread_ctx, system_data->query);
    ecs_iter_t *it = &qit;

    if (offset || limit) {
        pit = ecs_page_iter(it, offset, limit);
        it = &pit;
    }

    if (stage_count > 1 && system_data->multi_threaded) {
        wit = ecs_worker_iter(it, stage_current, stage_count);
        it = &wit;
    }

    qit.system = system;
    qit.self = system_data->self;
    qit.delta_time = delta_time;
    qit.delta_system_time = time_elapsed;
    qit.frame_offset = offset;
    qit.param = param;
    qit.ctx = system_data->ctx;
    qit.binding_ctx = system_data->binding_ctx;

    ecs_iter_action_t action = system_data->action;
    it->callback = action;
    
    ecs_run_action_t run = system_data->run;
    if (run) {
        run(it);
    } else if (system_data->query->filter.term_count) {
        if (it == &qit) {
            while (ecs_query_next(&qit)) {
                action(&qit);
            }
        } else {
            while (ecs_iter_next(it)) {
                action(it);
            }
        }
    } else {
        action(&qit);
    }

    if (measure_time) {
        system_data->time_spent += (float)ecs_time_measure(&time_start);
    }

    system_data->invoke_count ++;

    ecs_defer_end(thread_ctx);

    return it->interrupted_by;
}

/* -- Public API -- */

ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_ftime_t delta_time,
    int32_t offset,
    int32_t limit,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_system_t *system_data = ecs_poly_get(world, system, ecs_system_t);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_run_intern(world, stage, system, system_data, 0, 0, delta_time, 
        offset, limit, param);
}

ecs_entity_t ecs_run_worker(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t stage_current,
    int32_t stage_count,
    ecs_ftime_t delta_time,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_system_t *system_data = ecs_poly_get(world, system, ecs_system_t);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    return ecs_run_intern(
        world, stage, system, system_data, stage_current, stage_count, 
        delta_time, 0, 0, param);
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_ftime_t delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, param);
}

ecs_query_t* ecs_system_get_query(
    const ecs_world_t *world,
    ecs_entity_t system)
{
    const ecs_system_t *s = ecs_poly_get(world, system, ecs_system_t);
    if (s) {
        return s->query;
    } else {
        return NULL;
    }
}

void* ecs_get_system_ctx(
    const ecs_world_t *world,
    ecs_entity_t system)
{
    const ecs_system_t *s = ecs_poly_get(world, system, ecs_system_t);
    if (s) {
        return s->ctx;
    } else {
        return NULL;
    }   
}

void* ecs_get_system_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t system)
{
    const ecs_system_t *s = ecs_poly_get(world, system, ecs_system_t);
    if (s) {
        return s->binding_ctx;
    } else {
        return NULL;
    }   
}

/* System deinitialization */
static
void flecs_system_fini(ecs_system_t *sys) {
    ecs_world_t *world = sys->world;
    ecs_entity_t entity = sys->entity;

    /* Invoke Disabled action for enabled systems */
    if (!ecs_has_id(world, entity, EcsDisabled)) {
        invoke_status_action(world, entity, sys, EcsSystemDisabled);
    }

    if (sys->ctx_free) {
        sys->ctx_free(sys->ctx);
    }

    if (sys->status_ctx_free) {
        sys->status_ctx_free(sys->status_ctx);
    }

    if (sys->binding_ctx_free) {
        sys->binding_ctx_free(sys->binding_ctx);
    }

    ecs_poly_free(sys, ecs_system_t);
}

static
void EnableMonitor(
    ecs_iter_t *it)
{
    if (ecs_is_fini(it->world)) {
        return;
    }

    EcsPoly *poly = ecs_term(it, EcsPoly, 1);
    int32_t i, count = it->count;
    for (i = 0; i < count; i ++) {
        if (poly[i].poly == NULL) {
            continue; /* This is a new system */
        }

        ecs_system_t *sys = ecs_poly(poly[i].poly, ecs_system_t);
        if (it->event == EcsOnAdd) {
            ecs_enable_system(it->world, it->entities[i], sys, true);
        } else if (it->event == EcsOnRemove) {
            ecs_enable_system(it->world, it->entities[i], sys, false);
        }
    }
}

ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(world->flags & EcsWorldReadonly), 
        ECS_INVALID_WHILE_READONLY, NULL);

    ecs_entity_t entity = ecs_poly_entity_init(world, &desc->entity);
    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_system_t);
    if (!poly->poly) {
        ecs_system_t *system = ecs_poly_new(ecs_system_t);
        ecs_assert(system != NULL, ECS_INTERNAL_ERROR, NULL);
        
        poly->poly = system;
        system->world = world;
        system->dtor = (ecs_poly_dtor_t)flecs_system_fini;
        system->entity = entity;

        ecs_query_desc_t query_desc = desc->query;
        query_desc.filter.name = desc->entity.name;
        query_desc.system = entity;

        ecs_query_t *query = ecs_query_init(world, &query_desc);
        if (!query) {
            ecs_delete(world, entity);
            return 0;
        }

        /* Prevent the system from moving while we're initializing */
        ecs_defer_begin(world);

        system->query = query;
        system->query_entity = query->entity;

        system->run = desc->run;
        system->action = desc->callback;
        system->status_action = desc->status_callback;

        system->self = desc->self;
        system->ctx = desc->ctx;
        system->status_ctx = desc->status_ctx;
        system->binding_ctx = desc->binding_ctx;

        system->ctx_free = desc->ctx_free;
        system->status_ctx_free = desc->status_ctx_free;
        system->binding_ctx_free = desc->binding_ctx_free;

        system->tick_source = desc->tick_source;

        system->multi_threaded = desc->multi_threaded;
        system->no_staging = desc->no_staging;

        /* If tables have been matched with this system it is active, and we
         * should activate the in terms, if any. This will ensure that any
         * OnDemand systems get enabled. */
        if (ecs_query_table_count(query)) {
            ecs_system_activate(world, entity, true, system);
        } else if (query->filter.term_count) {
            /* If system isn't matched with any tables, mark it as inactive. This
             * causes it to be ignored by the main loop. When the system matches
             * with a table it will be activated. */
            ecs_add_id(world, entity, EcsInactive);
        }

        if (!ecs_has_id(world, entity, EcsDisabled)) {
            /* If system is already enabled, generate enable status. The API 
            * should guarantee that it exactly matches enable-disable 
            * notifications and activate-deactivate notifications. */
            invoke_status_action(world, entity, system, EcsSystemEnabled);

            /* If column system has active (non-empty) tables, also generate the
            * activate status. */
            if (ecs_query_table_count(system->query)) {
                invoke_status_action(world, entity, system, EcsSystemActivated);
            }
        }

        if (desc->interval != 0 || desc->rate != 0 || desc->tick_source != 0) {
#ifdef FLECS_TIMER
            if (desc->interval != 0) {
                ecs_set_interval(world, entity, desc->interval);
            }

            if (desc->rate) {
                ecs_set_rate(world, entity, desc->rate, desc->tick_source);
            } else if (desc->tick_source) {
                ecs_set_tick_source(world, entity, desc->tick_source);
            }
#else
            ecs_abort(ECS_UNSUPPORTED, "timer module not available");
#endif
        }

        if (desc->entity.name) {
            ecs_trace("#[green]system#[reset] %s created", 
                ecs_get_name(world, entity));
        }

        ecs_defer_end(world);            
    } else {
        ecs_system_t *system = ecs_poly(poly->poly, ecs_system_t);

        if (desc->run) {
            system->run = desc->run;
        }
        if (desc->callback) {
            system->action = desc->callback;
        }
        if (desc->ctx) {
            system->ctx = desc->ctx;
        }
        if (desc->binding_ctx) {
            system->binding_ctx = desc->binding_ctx;
        }
        if (desc->query.filter.instanced) {
            ECS_BIT_SET(system->query->filter.flags, EcsFilterIsInstanced);
        }
        if (desc->multi_threaded) {
            system->multi_threaded = desc->multi_threaded;
        }
        if (desc->no_staging) {
            system->no_staging = desc->no_staging;
        }
    }

    return entity;
error:
    return 0;
}

void FlecsSystemImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_tag(world, EcsSystem);
    flecs_bootstrap_component(world, EcsTickSource);

    /* Put following tags in flecs.core so they can be looked up
     * without using the flecs.systems prefix. */
    ecs_entity_t old_scope = ecs_set_scope(world, EcsFlecsCore);
    flecs_bootstrap_tag(world, EcsInactive);
    flecs_bootstrap_tag(world, EcsMonitor);
    ecs_set_scope(world, old_scope);

    ecs_observer_init(world, &(ecs_observer_desc_t) {
        .entity.name = "EnableMonitor",
        .filter.terms = {
            { .id = ecs_poly_id(EcsSystem) },
            { .id = EcsDisabled, .oper = EcsNot },
        },
        .events = {EcsMonitor},
        .callback = EnableMonitor
    });
}

#endif
