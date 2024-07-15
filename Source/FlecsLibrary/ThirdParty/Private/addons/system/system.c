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

ecs_entity_t flecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    ecs_system_t *system_data,
    int32_t stage_index,
    int32_t stage_count,    
    ecs_ftime_t delta_time,
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
        char *path = ecs_get_path(world, system);
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
        stage = world->stages[0];
    }

    /* Prepare the query iterator */
    ecs_iter_t wit, qit = ecs_query_iter(thread_ctx, system_data->query);
    ecs_iter_t *it = &qit;

    qit.system = system;
    qit.delta_time = delta_time;
    qit.delta_system_time = time_elapsed;
    qit.param = param;
    qit.ctx = system_data->ctx;
    qit.callback_ctx = system_data->callback_ctx;
    qit.run_ctx = system_data->run_ctx;

    flecs_defer_begin(world, stage);

    if (stage_count > 1 && system_data->multi_threaded) {
        wit = ecs_worker_iter(it, stage_index, stage_count);
        it = &wit;
    }

    ecs_entity_t old_system = flecs_stage_set_system(stage, system);
    ecs_iter_action_t action = system_data->action;
    it->callback = action;

    ecs_run_action_t run = system_data->run;
    if (run) {
        if (!system_data->query->term_count) {
            it->next = flecs_default_next_callback; /* Return once */
            run(it);
            ecs_iter_fini(&qit);
        } else {
            run(it);
        }
    } else {
        if (system_data->query->term_count) {
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
    }

    flecs_stage_set_system(stage, old_system);

    if (measure_time) {
        system_data->time_spent += (ecs_ftime_t)ecs_time_measure(&time_start);
    }

    flecs_defer_end(world, stage);

    return it->interrupted_by;
}

/* -- Public API -- */

ecs_entity_t ecs_run_worker(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t stage_index,
    int32_t stage_count,
    ecs_ftime_t delta_time,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_system_t *system_data = flecs_poly_get(world, system, ecs_system_t);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    return flecs_run_intern(
        world, stage, system, system_data, stage_index, stage_count, 
        delta_time, param);
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_ftime_t delta_time,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_system_t *system_data = flecs_poly_get(world, system, ecs_system_t);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);
    return flecs_run_intern(
        world, stage, system, system_data, 0, 0, delta_time, param);
}

/* System deinitialization */
static
void flecs_system_fini(ecs_system_t *sys) {
    if (sys->ctx_free) {
        sys->ctx_free(sys->ctx);
    }

    if (sys->callback_ctx_free) {
        sys->callback_ctx_free(sys->callback_ctx);
    }

    if (sys->run_ctx_free) {
        sys->run_ctx_free(sys->run_ctx);
    }

    flecs_poly_free(sys, ecs_system_t);
}

/* ecs_poly_dtor_t-compatible wrapper */
static
void flecs_system_poly_fini(void *sys)
{
    flecs_system_fini(sys);
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
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(desc->_canary == 0, ECS_INVALID_PARAMETER,
        "ecs_system_desc_t was not initialized to zero");
    ecs_assert(!(world->flags & EcsWorldReadonly), 
        ECS_INVALID_WHILE_READONLY, NULL);

    ecs_entity_t entity = desc->entity;
    if (!entity) {
        entity = ecs_entity(world, {0});
    }

    EcsPoly *poly = flecs_poly_bind(world, entity, ecs_system_t);
    if (!poly->poly) {
        ecs_system_t *system = flecs_poly_new(ecs_system_t);
        ecs_assert(system != NULL, ECS_INTERNAL_ERROR, NULL);
        
        poly->poly = system;
        system->world = world;
        system->dtor = flecs_system_poly_fini;
        system->entity = entity;

        ecs_query_desc_t query_desc = desc->query;
        query_desc.entity = entity;

        ecs_query_t *query = ecs_query_init(world, &query_desc);
        if (!query) {
            ecs_delete(world, entity);
            return 0;
        }

        /* Prevent the system from moving while we're initializing */
        flecs_defer_begin(world, world->stages[0]);

        system->query = query;
        system->query_entity = query->entity;

        system->run = desc->run;
        system->action = desc->callback;

        system->ctx = desc->ctx;
        system->callback_ctx = desc->callback_ctx;
        system->run_ctx = desc->run_ctx;

        system->ctx_free = desc->ctx_free;
        system->callback_ctx_free = desc->callback_ctx_free;
        system->run_ctx_free = desc->run_ctx_free;

        system->tick_source = desc->tick_source;

        system->multi_threaded = desc->multi_threaded;
        system->immediate = desc->immediate;

        flecs_system_init_timer(world, entity, desc);

        if (ecs_get_name(world, entity)) {
            ecs_trace("#[green]system#[reset] %s created", 
                ecs_get_name(world, entity));
        }

        ecs_defer_end(world);            
    } else {
        flecs_poly_assert(poly->poly, ecs_system_t);
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

        if (system->callback_ctx_free) {
            if (system->callback_ctx && system->callback_ctx != desc->callback_ctx) {
                system->callback_ctx_free(system->callback_ctx);
            }
        }

        if (system->run_ctx_free) {
            if (system->run_ctx && system->run_ctx != desc->run_ctx) {
                system->run_ctx_free(system->run_ctx);
            }
        }

        if (desc->ctx) {
            system->ctx = desc->ctx;
        }

        if (desc->callback_ctx) {
            system->callback_ctx = desc->callback_ctx;
        }

        if (desc->run_ctx) {
            system->run_ctx = desc->run_ctx;
        }

        if (desc->ctx_free) {
            system->ctx_free = desc->ctx_free;
        }

        if (desc->callback_ctx_free) {
            system->callback_ctx_free = desc->callback_ctx_free;
        }

        if (desc->run_ctx_free) {
            system->run_ctx_free = desc->run_ctx_free;
        }

        if (desc->query.flags & EcsQueryIsInstanced) {
            ECS_BIT_SET(system->query->flags, EcsQueryIsInstanced);
        }

        if (desc->multi_threaded) {
            system->multi_threaded = desc->multi_threaded;
        }

        if (desc->immediate) {
            system->immediate = desc->immediate;
        }

        flecs_system_init_timer(world, entity, desc);
    }

    flecs_poly_modified(world, entity, ecs_system_t);

    return entity;
error:
    return 0;
}

const ecs_system_t* ecs_system_get(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    return flecs_poly_get(world, entity, ecs_system_t);
}

void FlecsSystemImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystem);
#ifdef FLECS_DOC
    ECS_IMPORT(world, FlecsDoc);
    ecs_doc_set_brief(world, ecs_id(FlecsSystem), 
        "Module that implements Flecs systems");
#endif

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_tag(world, EcsSystem);
    flecs_bootstrap_component(world, EcsTickSource);

    /* Make sure to never inherit system component. This makes sure that any
     * term created for the System component will default to 'self' traversal,
     * which improves efficiency of the query. */
    ecs_add_pair(world, EcsSystem, EcsOnInstantiate, EcsDontInherit);
}

#endif
