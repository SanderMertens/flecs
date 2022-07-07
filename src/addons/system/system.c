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
        wit = ecs_worker_iter(it, stage_index, stage_count);
        it = &wit;
    }

    qit.system = system;
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
    if (sys->ctx_free) {
        sys->ctx_free(sys->ctx);
    }

    if (sys->binding_ctx_free) {
        sys->binding_ctx_free(sys->binding_ctx);
    }

    ecs_poly_free(sys, ecs_system_t);
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

    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);
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
        query_desc.entity = entity;

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

        system->ctx = desc->ctx;
        system->binding_ctx = desc->binding_ctx;

        system->ctx_free = desc->ctx_free;
        system->binding_ctx_free = desc->binding_ctx_free;

        system->tick_source = desc->tick_source;

        system->multi_threaded = desc->multi_threaded;
        system->no_staging = desc->no_staging;

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

    /* Put following tags in flecs.core so they can be looked up
     * without using the flecs.systems prefix. */
    ecs_entity_t old_scope = ecs_set_scope(world, EcsFlecsCore);
    flecs_bootstrap_tag(world, EcsMonitor);
    ecs_set_scope(world, old_scope);
}

#endif
