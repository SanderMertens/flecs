/**
 * @file addons/ipeline/pipeline.c
 * @brief Functions for building and running pipelines.
 */

#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

static void flecs_pipeline_free(
    ecs_pipeline_state_t *p) 
{
    if (p) {
        ecs_world_t *world = p->query->filter.world;
        ecs_allocator_t *a = &world->allocator;
        ecs_vec_fini_t(a, &p->ops, ecs_pipeline_op_t);
        ecs_vec_fini_t(a, &p->systems, ecs_entity_t);
        ecs_os_free(p->iters);
        ecs_query_fini(p->query);
        ecs_os_free(p);
    }
}

static ECS_MOVE(EcsPipeline, dst, src, {
    flecs_pipeline_free(dst->state);
    dst->state = src->state;
    src->state = NULL;
})

static ECS_DTOR(EcsPipeline, ptr, {
    flecs_pipeline_free(ptr->state);
})

typedef enum ecs_write_kind_t {
    WriteStateNone = 0,
    WriteStateToStage,
} ecs_write_kind_t;

typedef struct ecs_write_state_t {
    bool write_barrier;
    ecs_map_t ids;
    ecs_map_t wildcard_ids;
} ecs_write_state_t;

static
ecs_write_kind_t flecs_pipeline_get_write_state(
    ecs_write_state_t *write_state,
    ecs_id_t id)
{
    ecs_write_kind_t result = WriteStateNone;

    if (write_state->write_barrier) {
        /* Any component could have been written */
        return WriteStateToStage;
    }

    if (id == EcsWildcard) {
        /* Using a wildcard for id indicates read barrier. Return true if any
         * components could have been staged */
        if (ecs_map_count(&write_state->ids) || 
            ecs_map_count(&write_state->wildcard_ids)) 
        {
            return WriteStateToStage;
        }
    }

    if (!ecs_id_is_wildcard(id)) {
        if (ecs_map_get(&write_state->ids, id)) {
            result = WriteStateToStage;
        }
    } else {
        ecs_map_iter_t it = ecs_map_iter(&write_state->ids);
        while (ecs_map_next(&it)) {
            if (ecs_id_match(ecs_map_key(&it), id)) {
                return WriteStateToStage;
            }
        }
    }

    if (ecs_map_count(&write_state->wildcard_ids)) {
        ecs_map_iter_t it = ecs_map_iter(&write_state->wildcard_ids);
        while (ecs_map_next(&it)) {
            if (ecs_id_match(id, ecs_map_key(&it))) {
                return WriteStateToStage;
            }
        }
    }

    return result;
}

static
void flecs_pipeline_set_write_state(
    ecs_write_state_t *write_state,
    ecs_id_t id)
{
    if (id == EcsWildcard) {
        /* If writing to wildcard, flag all components as written */
        write_state->write_barrier = true;
        return;
    }

    ecs_map_t *ids;
    if (ecs_id_is_wildcard(id)) {
        ids = &write_state->wildcard_ids;
    } else {
        ids = &write_state->ids;
    }

    ecs_map_ensure(ids, id)[0] = true;
}

static
void flecs_pipeline_reset_write_state(
    ecs_write_state_t *write_state)
{
    ecs_map_clear(&write_state->ids);
    ecs_map_clear(&write_state->wildcard_ids);
    write_state->write_barrier = false;
}

static
bool flecs_pipeline_check_term(
    ecs_world_t *world,
    ecs_term_t *term,
    bool is_active,
    ecs_write_state_t *write_state)    
{
    (void)world;

    ecs_term_id_t *src = &term->src;
    if (src->flags & EcsInOutNone) {
        return false;
    }

    ecs_id_t id = term->id;
    ecs_oper_kind_t oper = term->oper;
    ecs_inout_kind_t inout = term->inout;
    bool from_any = ecs_term_match_0(term);
    bool from_this = ecs_term_match_this(term);
    bool is_shared = !from_any && (!from_this || !(src->flags & EcsSelf));

    ecs_write_kind_t ws = flecs_pipeline_get_write_state(write_state, id);

    if (from_this && ws >= WriteStateToStage) {
        /* A staged write could have happened for an id that's matched on the
         * main storage. Even if the id isn't read, still insert a merge so that
         * a write to the main storage after the staged write doesn't get 
         * overwritten. */
        return true;
    }

    if (inout == EcsInOutDefault) {
        if (from_any) {
            /* If no inout kind is specified for terms without a source, this is
             * not interpreted as a read/write annotation but just a (component)
             * id that's passed to a system. */
            return false;
        } else if (is_shared) {
            inout = EcsIn;
        } else {
            /* Default for owned terms is InOut */
            inout = EcsInOut;
        }
    }

    if (oper == EcsNot && inout == EcsOut) {
        /* If a Not term is combined with Out, it signals that the system 
         * intends to add a component that the entity doesn't yet have */
        from_any = true;
    }

    if (from_any) {
        switch(inout) {
        case EcsOut:
        case EcsInOut:
            if (is_active) {
                /* Only flag component as written if system is active */
                flecs_pipeline_set_write_state(write_state, id);
            }
            break;
        default:
            break;
        }

        switch(inout) {
        case EcsIn:
        case EcsInOut:
            if (ws == WriteStateToStage) {
                /* If a system does a get/get_mut, the component is fetched from
                 * the main store so it must be merged first */
                return true;
            }
        default:
            break;
        }
    }

    return false;
}

static
bool flecs_pipeline_check_terms(
    ecs_world_t *world,
    ecs_filter_t *filter,
    bool is_active,
    ecs_write_state_t *ws)
{
    bool needs_merge = false;
    ecs_term_t *terms = filter->terms;
    int32_t t, term_count = filter->term_count;

    /* Check This terms first. This way if a term indicating writing to a stage
     * was added before the term, it won't cause merging. */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (ecs_term_match_this(term)) {
            needs_merge |= flecs_pipeline_check_term(world, term, is_active, ws);
        }
    }

    /* Now check staged terms */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (!ecs_term_match_this(term)) {
            needs_merge |= flecs_pipeline_check_term(world, term, is_active, ws);
        }
    }

    return needs_merge;
}

static
EcsPoly* flecs_pipeline_term_system(
    ecs_iter_t *it)
{
    int32_t index = ecs_search(it->real_world, it->table, 
        ecs_poly_id(EcsSystem), 0);
    ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
    EcsPoly *poly = ecs_table_get_column(it->table, index, it->offset);
    ecs_assert(poly != NULL, ECS_INTERNAL_ERROR, NULL);
    return poly;
}

static
bool flecs_pipeline_build(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq)
{
    ecs_iter_t it = ecs_query_iter(world, pq->query);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        ecs_iter_fini(&it);
        return false;
    }

    world->info.pipeline_build_count_total ++;
    pq->rebuild_count ++;

    ecs_allocator_t *a = &world->allocator;
    ecs_pipeline_op_t *op = NULL;
    ecs_write_state_t ws = {0};
    ecs_map_init(&ws.ids, a);
    ecs_map_init(&ws.wildcard_ids, a);

    ecs_vec_reset_t(a, &pq->ops, ecs_pipeline_op_t);
    ecs_vec_reset_t(a, &pq->systems, ecs_entity_t);

    bool multi_threaded = false;
    bool no_readonly = false;
    bool first = true;

    /* Iterate systems in pipeline, add ops for running / merging */
    while (ecs_query_next(&it)) {
        EcsPoly *poly = flecs_pipeline_term_system(&it);
        bool is_active = ecs_table_get_index(world, it.table, EcsEmpty) == -1;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_system_t *sys = ecs_poly(poly[i].poly, ecs_system_t);
            ecs_query_t *q = sys->query;

            bool needs_merge = false;
            needs_merge = flecs_pipeline_check_terms(
                world, &q->filter, is_active, &ws);

            if (is_active) {
                if (first) {
                    multi_threaded = sys->multi_threaded;
                    no_readonly = sys->no_readonly;
                    first = false;
                }

                if (sys->multi_threaded != multi_threaded) {
                    needs_merge = true;
                    multi_threaded = sys->multi_threaded;
                }
                if (sys->no_readonly != no_readonly) {
                    needs_merge = true;
                    no_readonly = sys->no_readonly;
                }
            }

            if (no_readonly) {
                needs_merge = true;
            }

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                flecs_pipeline_reset_write_state(&ws);

                /* An inactive system can insert a merge if one of its 
                 * components got written, which could make the system 
                 * active. If this is the only system in the pipeline operation,
                 * it results in an empty operation when we get here. If that's
                 * the case, reuse the empty operation for the next op. */
                if (op && op->count) {
                    op = NULL;
                }

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    needs_merge = flecs_pipeline_check_terms(
                        world, &q->filter, true, &ws);
                }

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vec_append_t(a, &pq->ops, ecs_pipeline_op_t);
                op->offset = ecs_vec_count(&pq->systems);
                op->count = 0;
                op->multi_threaded = false;
                op->no_readonly = false;
            }

            /* Don't increase count for inactive systems, as they are ignored by
             * the query used to run the pipeline. */
            if (is_active) {
                ecs_vec_append_t(a, &pq->systems, ecs_entity_t)[0] = 
                    it.entities[i];
                if (!op->count) {
                    op->multi_threaded = multi_threaded;
                    op->no_readonly = no_readonly;
                }
                op->count ++;
            }
        }
    }

    if (op && !op->count && ecs_vec_count(&pq->ops) > 1) {
        ecs_vec_remove_last(&pq->ops);
    }

    ecs_map_fini(&ws.ids);
    ecs_map_fini(&ws.wildcard_ids);

    op = ecs_vec_first_t(&pq->ops, ecs_pipeline_op_t);

    if (!op) {
        ecs_dbg("#[green]pipeline#[reset] is empty");
        return true;
    } else {
        /* Add schedule to debug tracing */
        ecs_dbg("#[bold]pipeline rebuild");
        ecs_log_push_1();

        ecs_dbg("#[green]schedule#[reset]: threading: %d, staging: %d:", 
            op->multi_threaded, !op->no_readonly);
        ecs_log_push_1();

        int32_t i, count = ecs_vec_count(&pq->systems);
        int32_t op_index = 0, ran_since_merge = 0;
        ecs_entity_t *systems = ecs_vec_first_t(&pq->systems, ecs_entity_t);
        for (i = 0; i < count; i ++) {
            ecs_entity_t system = systems[i];
            const EcsPoly *poly = ecs_get_pair(world, system, EcsPoly, EcsSystem);
            ecs_assert(poly != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_system_t *sys = ecs_poly(poly->poly, ecs_system_t);

#ifdef FLECS_LOG_1
            char *path = ecs_get_fullpath(world, system);
            const char *doc_name = NULL;
#ifdef FLECS_DOC
            const EcsDocDescription *doc_name_id = ecs_get_pair(world, system, 
                EcsDocDescription, EcsName);
            if (doc_name_id) {
                doc_name = doc_name_id->value;
            }
#endif
            if (doc_name) {
                ecs_dbg("#[green]system#[reset] %s (%s)", path, doc_name);
            } else {
                ecs_dbg("#[green]system#[reset] %s", path);
            }
            ecs_os_free(path);
#endif

            ecs_assert(op[op_index].offset + ran_since_merge == i,
                ECS_INTERNAL_ERROR, NULL);

            ran_since_merge ++;
            if (ran_since_merge == op[op_index].count) {
                ecs_dbg("#[magenta]merge#[reset]");
                ecs_log_pop_1();
                ran_since_merge = 0;
                op_index ++;
                if (op_index < ecs_vec_count(&pq->ops)) {
                    ecs_dbg(
                        "#[green]schedule#[reset]: "
                        "threading: %d, staging: %d:",
                        op[op_index].multi_threaded, 
                        !op[op_index].no_readonly);
                }
                ecs_log_push_1();
            }

            if (sys->last_frame == (world->info.frame_count_total + 1)) {
                if (op_index < ecs_vec_count(&pq->ops)) {
                    pq->cur_op = &op[op_index];
                    pq->cur_i = i;
                } else {
                    pq->cur_op = NULL;
                    pq->cur_i = 0;
                }
            }
        }

        ecs_log_pop_1();
        ecs_log_pop_1();
    }

    pq->match_count = pq->query->match_count;

    ecs_assert(pq->cur_op <= ecs_vec_last_t(&pq->ops, ecs_pipeline_op_t),
        ECS_INTERNAL_ERROR, NULL);

    return true;
}

static
void flecs_pipeline_next_system(
    ecs_pipeline_state_t *pq)
{
    if (!pq->cur_op) {
        return;
    }

    pq->cur_i ++;
    if (pq->cur_i >= (pq->cur_op->offset + pq->cur_op->count)) {
        pq->cur_op ++;
        if (pq->cur_op > ecs_vec_last_t(&pq->ops, ecs_pipeline_op_t)) {
            pq->cur_op = NULL;
        }
    }    
}

bool flecs_pipeline_update(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    bool start_of_frame)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, NULL);

    /* If any entity mutations happened that could have affected query matching
     * notify appropriate queries so caches are up to date. This includes the
     * pipeline query. */
    if (start_of_frame) {
        ecs_run_aperiodic(world, 0);
    }

    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    bool rebuilt = flecs_pipeline_build(world, pq);
    if (start_of_frame) {
        /* Initialize iterators */
        int32_t i, count = pq->iter_count;
        for (i = 0; i < count; i ++) {
            ecs_world_t *stage = ecs_get_stage(world, i);
            pq->iters[i] = ecs_query_iter(stage, pq->query);
        }
        pq->cur_op = ecs_vec_first_t(&pq->ops, ecs_pipeline_op_t);
        pq->cur_i = 0;
    } else {
        flecs_pipeline_next_system(pq);
    }

    return rebuilt;
}

void ecs_run_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_ftime_t delta_time)
{
    if (!pipeline) {
        pipeline = world->pipeline;
    }

    EcsPipeline *pq = (EcsPipeline*)ecs_get(world, pipeline, EcsPipeline);
    flecs_pipeline_update(world, pq->state, true);
    flecs_run_pipeline((ecs_world_t*)flecs_stage_from_world(&world), 
        pq->state, delta_time);
}

void flecs_run_pipeline(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_OPERATION, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_poly_assert(world, ecs_stage_t);

    ecs_stage_t *stage = flecs_stage_from_world(&world);  
    int32_t stage_index = ecs_get_stage_id(stage->thread_ctx);
    int32_t stage_count = ecs_get_stage_count(world);

    if (!flecs_worker_begin(world, stage, pq, true)) {
        return;
    }

    ecs_time_t st = {0};
    bool main_thread = !stage_index;
    bool measure_time = main_thread && (world->flags & EcsWorldMeasureSystemTime);
    ecs_pipeline_op_t *op = ecs_vec_first_t(&pq->ops, ecs_pipeline_op_t);
    int32_t i = 0;

    do {
        int32_t count = ecs_vec_count(&pq->systems);
        ecs_entity_t *systems = ecs_vec_first_t(&pq->systems, ecs_entity_t);
        int32_t ran_since_merge = i - op->offset;

        if (i == count) {
            break;
        }

        if (measure_time) {
            ecs_time_measure(&st);
        }

        for (; i < count; i ++) {
            /* Run system if:
             * - this is the main thread, or if
             * - the system is multithreaded 
             */
            if (main_thread || op->multi_threaded) {
                ecs_entity_t system = systems[i];
                const EcsPoly *poly = ecs_get_pair(world, system, EcsPoly, EcsSystem);
                ecs_assert(poly != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_system_t *sys = ecs_poly(poly->poly, ecs_system_t);

                /* Keep track of the last frame for which the system has ran, so we
                * know from where to resume the schedule in case the schedule 
                * changes during a merge. */
                sys->last_frame = world->info.frame_count_total + 1;

                ecs_stage_t *s = NULL;
                if (!op->no_readonly) {
                    /* If system is no_readonly it operates on the actual world, not
                     * the stage. Only pass stage to system if it's readonly. */
                    s = stage;
                }

                ecs_run_intern(world, s, system, sys, stage_index, 
                    stage_count, delta_time, 0, 0, NULL);
            }

            world->info.systems_ran_frame ++;
            ran_since_merge ++;

            if (ran_since_merge == op->count) {
                /* Merge */
                break;
            }
        }

        if (measure_time) {
            /* Don't include merge time in system time */
            world->info.system_time_total += 
                (ecs_ftime_t)ecs_time_measure(&st);
        }

        /* Synchronize workers, rebuild pipeline if necessary. Pass current op
         * and system index to function, so we know where to resume from. */
    } while (flecs_worker_sync(world, stage, pq, &op, &i));

    if (measure_time) {
        world->info.system_time_total += (ecs_ftime_t)ecs_time_measure(&st);
    }

    flecs_worker_end(world, stage);

    return;
}

static
void flecs_run_startup_systems(
    ecs_world_t *world)
{
    ecs_id_record_t *idr = flecs_id_record_get(world, 
        ecs_dependson(EcsOnStart));
    if (!idr || !flecs_table_cache_count(&idr->cache)) {
        /* Don't bother creating startup pipeline if no systems exist */
        return;
    }

    ecs_dbg_2("#[bold]startup#[reset]");
    ecs_log_push_2();
    int32_t stage_count = world->stage_count;
    world->stage_count = 1; /* Prevents running startup systems on workers */

    /* Creating a pipeline is relatively expensive, but this only happens 
     * for the first frame. The startup pipeline is deleted afterwards, which
     * eliminates the overhead of keeping its query cache in sync. */
    ecs_dbg_2("#[bold]create startup pipeline#[reset]");
    ecs_log_push_2();
    ecs_entity_t start_pip = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
        .query = {
            .filter.terms = {
                { .id = EcsSystem },
                { .id = EcsPhase, .src.flags = EcsCascade, .src.trav = EcsDependsOn },
                { .id = ecs_dependson(EcsOnStart), .src.trav = EcsDependsOn },
                { .id = EcsDisabled, .src.flags = EcsUp, .src.trav = EcsDependsOn, .oper = EcsNot },
                { .id = EcsDisabled, .src.flags = EcsUp, .src.trav = EcsChildOf, .oper = EcsNot }
            },
            .order_by = flecs_entity_compare
        }
    });
    ecs_log_pop_2();

    /* Run & delete pipeline */
    ecs_dbg_2("#[bold]run startup systems#[reset]");
    ecs_log_push_2();
    ecs_assert(start_pip != 0, ECS_INTERNAL_ERROR, NULL);
    const EcsPipeline *p = ecs_get(world, start_pip, EcsPipeline);
    ecs_check(p != NULL, ECS_INVALID_OPERATION, NULL);
    flecs_workers_progress(world, p->state, 0);
    ecs_log_pop_2();

    ecs_dbg_2("#[bold]delete startup pipeline#[reset]");
    ecs_log_push_2();
    ecs_delete(world, start_pip);
    ecs_log_pop_2();

    world->stage_count = stage_count;
    ecs_log_pop_2();

error:
    return;
}

bool ecs_progress(
    ecs_world_t *world,
    ecs_ftime_t user_delta_time)
{
    ecs_ftime_t delta_time = ecs_frame_begin(world, user_delta_time);
    
    /* If this is the first frame, run startup systems */
    if (world->info.frame_count_total == 0) {
        flecs_run_startup_systems(world);
    }

    ecs_dbg_3("#[bold]progress#[reset](dt = %.2f)", (double)delta_time);
    ecs_log_push_3();
    const EcsPipeline *p = ecs_get(world, world->pipeline, EcsPipeline);
    ecs_check(p != NULL, ECS_INVALID_OPERATION, NULL);
    flecs_workers_progress(world, p->state, delta_time);
    ecs_log_pop_3();

    ecs_frame_end(world);

    return !ECS_BIT_IS_SET(world->flags, EcsWorldQuit);
error:
    return false;
}

void ecs_set_time_scale(
    ecs_world_t *world,
    ecs_ftime_t scale)
{
    world->info.time_scale = scale;
}

void ecs_reset_clock(
    ecs_world_t *world)
{
    world->info.world_time_total = 0;
    world->info.world_time_total_raw = 0;
}

void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check( ecs_get(world, pipeline, EcsPipeline) != NULL, 
        ECS_INVALID_PARAMETER, "not a pipeline");

    int32_t thread_count = ecs_get_stage_count(world);
    if (thread_count > 1) {
        ecs_set_threads(world, 1);
    }
    world->pipeline = pipeline;
    if (thread_count > 1) {
        ecs_set_threads(world, thread_count);
    }
error:
    return;
}

ecs_entity_t ecs_get_pipeline(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->pipeline;
error:
    return 0;
}

ecs_entity_t ecs_pipeline_init(
    ecs_world_t *world,
    const ecs_pipeline_desc_t *desc)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = desc->entity;
    if (!result) {
        result = ecs_new(world, 0);
    }

    ecs_query_desc_t qd = desc->query;
    if (!qd.order_by) {
        qd.order_by = flecs_entity_compare;
    }
    qd.filter.entity = result;

    ecs_query_t *query = ecs_query_init(world, &qd);
    if (!query) {
        ecs_delete(world, result);
        return 0;
    }

    ecs_assert(query->filter.terms[0].id == EcsSystem,
        ECS_INVALID_PARAMETER, NULL);

    ecs_pipeline_state_t *pq = ecs_os_calloc_t(ecs_pipeline_state_t);
    pq->query = query;
    pq->match_count = -1;
    pq->idr_inactive = flecs_id_record_ensure(world, EcsEmpty);
    ecs_set(world, result, EcsPipeline, { pq });

    return result;
}

/* -- Module implementation -- */

static
void FlecsPipelineFini(
    ecs_world_t *world,
    void *ctx)
{
    (void)ctx;
    if (ecs_get_stage_count(world)) {
        ecs_set_threads(world, 0);
    }

    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);
}

#define flecs_bootstrap_phase(world, phase, depends_on)\
    flecs_bootstrap_tag(world, phase);\
    _flecs_bootstrap_phase(world, phase, depends_on)
static
void _flecs_bootstrap_phase(
    ecs_world_t *world,
    ecs_entity_t phase,
    ecs_entity_t depends_on)
{
    ecs_add_id(world, phase, EcsPhase);
    if (depends_on) {
        ecs_add_pair(world, phase, EcsDependsOn, depends_on);
    }
}

void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);
    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsPipeline);
    flecs_bootstrap_tag(world, EcsPhase);

    /* Create anonymous phases to which the builtin phases will have DependsOn 
     * relationships. This ensures that, for example, EcsOnUpdate doesn't have a
     * direct DependsOn relationship on EcsPreUpdate, which ensures that when
     * the EcsPreUpdate phase is disabled, EcsOnUpdate still runs. */
    ecs_entity_t phase_0 = ecs_new(world, 0);
    ecs_entity_t phase_1 = ecs_new_w_pair(world, EcsDependsOn, phase_0);
    ecs_entity_t phase_2 = ecs_new_w_pair(world, EcsDependsOn, phase_1);
    ecs_entity_t phase_3 = ecs_new_w_pair(world, EcsDependsOn, phase_2);
    ecs_entity_t phase_4 = ecs_new_w_pair(world, EcsDependsOn, phase_3);
    ecs_entity_t phase_5 = ecs_new_w_pair(world, EcsDependsOn, phase_4);
    ecs_entity_t phase_6 = ecs_new_w_pair(world, EcsDependsOn, phase_5);
    ecs_entity_t phase_7 = ecs_new_w_pair(world, EcsDependsOn, phase_6);
    ecs_entity_t phase_8 = ecs_new_w_pair(world, EcsDependsOn, phase_7);

    flecs_bootstrap_phase(world, EcsOnStart,   0);
    flecs_bootstrap_phase(world, EcsPreFrame,   0);
    flecs_bootstrap_phase(world, EcsOnLoad,     phase_0);
    flecs_bootstrap_phase(world, EcsPostLoad,   phase_1);
    flecs_bootstrap_phase(world, EcsPreUpdate,  phase_2);
    flecs_bootstrap_phase(world, EcsOnUpdate,   phase_3);
    flecs_bootstrap_phase(world, EcsOnValidate, phase_4);
    flecs_bootstrap_phase(world, EcsPostUpdate, phase_5);
    flecs_bootstrap_phase(world, EcsPreStore,   phase_6);
    flecs_bootstrap_phase(world, EcsOnStore,    phase_7);
    flecs_bootstrap_phase(world, EcsPostFrame,  phase_8);

    ecs_set_hooks(world, EcsPipeline, {
        .ctor = ecs_default_ctor,
        .dtor = ecs_dtor(EcsPipeline),
        .move = ecs_move(EcsPipeline)
    });

    world->pipeline = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
        .entity = ecs_entity(world, { .name = "BuiltinPipeline" }),
        .query = {
            .filter.terms = {
                { .id = EcsSystem },
                { .id = EcsPhase, .src.flags = EcsCascade, .src.trav = EcsDependsOn },
                { .id = ecs_dependson(EcsOnStart), .src.trav = EcsDependsOn, .oper = EcsNot },
                { .id = EcsDisabled, .src.flags = EcsUp, .src.trav = EcsDependsOn, .oper = EcsNot },
                { .id = EcsDisabled, .src.flags = EcsUp, .src.trav = EcsChildOf, .oper = EcsNot }
            },
            .order_by = flecs_entity_compare
        }
    });

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif
