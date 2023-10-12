/**
 * @file addons/system/system.c
 * @brief System addon.
 */

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

/* -- Public API -- */

ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_system_t *system_data,
    int32_t stage_index,
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
        const EcsTickSource *tick = ecs_get(world, tick_source, EcsTickSource);

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

    if (ecs_should_log_3()) {
        char *path = ecs_get_fullpath(world, system);
        ecs_dbg_3("worker %d: %s", stage_index, path);
        ecs_os_free(path);
    }

    ecs_time_t time_start;
    bool measure_time = ECS_BIT_IS_SET(world->flags, EcsWorldMeasureSystemTime);
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    ecs_world_t *thread_ctx = world;
    if (stage) {
        thread_ctx = stage->thread_ctx;
    } else {
        stage = &world->stages[0];
    }

    /* Prepare the query iterator */
    ecs_iter_t pit, wit, qit = ecs_query_iter(thread_ctx, system_data->query);
    ecs_iter_t *it = &qit;

    qit.system = system;
    qit.delta_time = delta_time;
    qit.delta_system_time = time_elapsed;
    qit.frame_offset = offset;
    qit.param = param;
    qit.ctx = system_data->ctx;
    qit.binding_ctx = system_data->binding_ctx;

    flecs_defer_begin(world, stage);

    if (offset || limit) {
        pit = ecs_page_iter(it, offset, limit);
        it = &pit;
    }

    if (stage_count > 1 && system_data->multi_threaded) {
        wit = ecs_worker_iter(it, stage_index, stage_count);
        it = &wit;
    }

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
        ecs_iter_fini(&qit);
    }

    if (measure_time) {
        system_data->time_spent += (ecs_ftime_t)ecs_time_measure(&time_start);
    }

    system_data->invoke_count ++;

    flecs_defer_end(world, stage);

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
    int32_t stage_index,
    int32_t stage_count,
    ecs_ftime_t delta_time,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_system_t *system_data = ecs_poly_get(world, system, ecs_system_t);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    return ecs_run_intern(
        world, stage, system, system_data, stage_index, stage_count, 
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

void* ecs_system_get_ctx(
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

void* ecs_system_get_binding_ctx(
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
    if (sys->ctx_free) {
        sys->ctx_free(sys->ctx);
    }

    if (sys->binding_ctx_free) {
        sys->binding_ctx_free(sys->binding_ctx);
    }

    ecs_poly_free(sys, ecs_system_t);
}

static
void flecs_system_init_timer(
    ecs_world_t *world,
    ecs_entity_t entity,
    const ecs_system_desc_t *desc)
{
    if (ECS_NEQZERO(desc->interval) || ECS_NEQZERO(desc->rate) || 
        ECS_NEQZERO(desc->tick_source)) 
    {
#ifdef FLECS_TIMER
        if (ECS_NEQZERO(desc->interval)) {
            ecs_set_interval(world, entity, desc->interval);
        }

        if (desc->rate) {
            ecs_entity_t tick_source = desc->tick_source;
            if (!tick_source) {
                tick_source = entity;
            }
            ecs_set_rate(world, entity, desc->rate, tick_source);
        } else if (desc->tick_source) {
            ecs_set_tick_source(world, entity, desc->tick_source);
        }
#else
        (void)world;
        (void)entity;
        ecs_abort(ECS_UNSUPPORTED, "timer module not available");
#endif
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

    ecs_entity_t entity = desc->entity;
    if (!entity) {
        entity = ecs_new(world, 0);
    }
    EcsPoly *poly = ecs_poly_bind(world, entity, ecs_system_t);
    if (!poly->poly) {
        ecs_system_t *system = ecs_poly_new(ecs_system_t);
        ecs_assert(system != NULL, ECS_INTERNAL_ERROR, NULL);
        
        poly->poly = system;
        system->world = world;
        system->dtor = (ecs_poly_dtor_t)flecs_system_fini;
        system->entity = entity;

        ecs_query_desc_t query_desc = desc->query;
        query_desc.filter.entity = entity;

        ecs_query_t *query = ecs_query_init(world, &query_desc);
        if (!query) {
            ecs_delete(world, entity);
            return 0;
        }

        /* Prevent the system from moving while we're initializing */
        flecs_defer_begin(world, &world->stages[0]);

        system->query = query;
        system->query_entity = query->filter.entity;

        system->run = desc->run;
        system->action = desc->callback;

        system->ctx = desc->ctx;
        system->binding_ctx = desc->binding_ctx;

        system->ctx_free = desc->ctx_free;
        system->binding_ctx_free = desc->binding_ctx_free;

        system->tick_source = desc->tick_source;

        system->multi_threaded = desc->multi_threaded;
        system->no_readonly = desc->no_readonly;

        flecs_system_init_timer(world, entity, desc);

        if (ecs_get_name(world, entity)) {
            ecs_trace("#[green]system#[reset] %s created", 
                ecs_get_name(world, entity));
        }

        ecs_defer_end(world);            
    } else {
        ecs_poly_assert(poly->poly, ecs_system_t);
        ecs_system_t *system = (ecs_system_t*)poly->poly;

        if (desc->run) {
            system->run = desc->run;
        }
        if (desc->callback) {
            system->action = desc->callback;
        }

        if (system->ctx_free) {
            if (system->ctx && system->ctx != desc->ctx) {
                system->ctx_free(system->ctx);
            }
        }
        if (system->binding_ctx_free) {
            if (system->binding_ctx && system->binding_ctx != desc->binding_ctx) {
                system->binding_ctx_free(system->binding_ctx);
            }
        }

        if (desc->ctx) {
            system->ctx = desc->ctx;
        }
        if (desc->binding_ctx) {
            system->binding_ctx = desc->binding_ctx;
        }
        if (desc->ctx_free) {
            system->ctx_free = desc->ctx_free;
        }
        if (desc->binding_ctx_free) {
            system->binding_ctx_free = desc->binding_ctx_free;
        }
        if (desc->query.filter.instanced) {
            ECS_BIT_SET(system->query->filter.flags, EcsFilterIsInstanced);
        }
        if (desc->multi_threaded) {
            system->multi_threaded = desc->multi_threaded;
        }
        if (desc->no_readonly) {
            system->no_readonly = desc->no_readonly;
        }

        flecs_system_init_timer(world, entity, desc);
    }

    ecs_poly_modified(world, entity, ecs_system_t);

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

    /* Make sure to never inherit system component. This makes sure that any
     * term created for the System component will default to 'self' traversal,
     * which improves efficiency of the query. */
    ecs_add_id(world, EcsSystem, EcsDontInherit);
}

#endif
