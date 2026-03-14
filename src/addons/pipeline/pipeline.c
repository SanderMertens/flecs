/**
 * @file addons/pipeline/pipeline.c
 * @brief Functions for building and running pipelines.
 */

#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

/* Free pipeline state and associated resources. */
static void flecs_pipeline_free(
    ecs_pipeline_state_t *p)
{
    if (p) {
        ecs_world_t *world = p->query->world;
        ecs_allocator_t *a = &world->allocator;
        ecs_vec_fini_t(a, &p->ops, ecs_pipeline_op_t);
        ecs_vec_fini_t(a, &p->systems, ecs_system_t*);
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

/* Determine whether a component id has been written to a stage. */
static
ecs_write_kind_t flecs_pipeline_get_write_state(
    ecs_write_state_t *write_state,
    ecs_id_t id)
{
    ecs_write_kind_t result = WriteStateNone;

    if (write_state->write_barrier) {
        return WriteStateToStage; /* Global write barrier active */
    }

    if (id == EcsWildcard) {
        /* Wildcard id acts as a read barrier; return true if any
         * components have been staged */
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

/* Mark a component id as written to a stage. */
static
void flecs_pipeline_set_write_state(
    ecs_write_state_t *write_state,
    ecs_id_t id)
{
    if (id == EcsWildcard) {
        /* Wildcard write sets a global barrier flag */
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

/* Clear all write state flags after a merge point. */
static
void flecs_pipeline_reset_write_state(
    ecs_write_state_t *write_state)
{
    ecs_map_clear(&write_state->ids);
    ecs_map_clear(&write_state->wildcard_ids);
    write_state->write_barrier = false;
}

/* Check whether a query term's read/write pattern conflicts with prior
 * staged writes, requiring a merge point before this system can run. */
static
bool flecs_pipeline_check_term(
    ecs_world_t *world,
    ecs_term_t *term,
    bool is_active,
    ecs_write_state_t *write_state)
{
    (void)world;

    ecs_term_ref_t *src = &term->src;
    if (term->inout == EcsInOutFilter) {
        return false;
    }

    ecs_id_t id = term->id;
    int16_t oper = term->oper;
    int16_t inout = term->inout;
    bool from_any = ecs_term_match_0(term);
    bool from_this = ecs_term_match_this(term);
    bool is_shared = !from_any && (!from_this || !(src->id & EcsSelf));

    ecs_write_kind_t ws = flecs_pipeline_get_write_state(write_state, id);

    if (from_this && ws >= WriteStateToStage) {
        /* A prior staged write to this id requires merging before a
         * This-source read, to prevent reading stale main storage. */
        return true;
    }

    if (inout == EcsInOutDefault) {
        if (from_any) {
            /* Sourceless terms with default inout are just id references,
             * not read/write annotations. */
            return false;
        } else if (is_shared) {
            inout = EcsIn;
        } else {
            /* Owned terms default to InOut */
            inout = EcsInOut;
        }
    }

    if (oper == EcsNot && inout == EcsOut) {
        /* Not + Out means the system intends to add a component the entity
         * doesn't yet have; treat as a sourceless write. */
        from_any = true;
    }

    if (from_any) {
        switch(inout) {
        case EcsOut:
        case EcsInOut:
            if (is_active) {
                /* Only mark as written if system is active */
                flecs_pipeline_set_write_state(write_state, id);
            }
            break;
        case EcsInOutDefault:
        case EcsInOutNone:
        case EcsInOutFilter:
        case EcsIn:
            break;
        }

        switch(inout) {
        case EcsIn:
        case EcsInOut:
            if (ws == WriteStateToStage) {
                /* get/ensure fetches from main store, so merge first */
                return true;
            }
            /* fall through */
        case EcsInOutDefault:
        case EcsInOutNone:
        case EcsInOutFilter:
        case EcsOut:
            break;
        }
    }

    return false;
}

/* Check all terms of a system query for merge requirements. */
static
bool flecs_pipeline_check_terms(
    ecs_world_t *world,
    ecs_query_t *query,
    bool is_active,
    ecs_write_state_t *ws)
{
    bool needs_merge = false;
    ecs_term_t *terms = query->terms;
    int32_t t, term_count = query->term_count;

    /* Check This terms first so that a staged write term preceding a This
     * read term doesn't cause a spurious merge. */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (ecs_term_match_this(term)) {
            needs_merge |= flecs_pipeline_check_term(world, term, is_active, ws);
        }
    }

    /* Then check non-This terms (which write to stages, not main storage) */
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        if (!ecs_term_match_this(term)) {
            needs_merge |= flecs_pipeline_check_term(world, term, is_active, ws);
        }
    }

    return needs_merge;
}

/* Get the system poly component from a pipeline query iterator. */
static
EcsPoly* flecs_pipeline_term_system(
    ecs_iter_t *it)
{
    int32_t index = ecs_table_get_column_index(
        it->real_world, it->table, flecs_poly_id(EcsSystem));
    ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
    EcsPoly *poly = ecs_table_get_column(it->table, index, it->offset);
    ecs_assert(poly != NULL, ECS_INTERNAL_ERROR, NULL);
    return poly;
}

/* Build the pipeline schedule from matched systems. */
static
bool flecs_pipeline_build(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq)
{
    ecs_iter_t it = ecs_query_iter(world, pq->query);

    int32_t new_match_count = ecs_query_match_count(pq->query);
    if (pq->match_count == new_match_count) {
        ecs_iter_fini(&it);
        return false; /* No query changes since last build */
    }

    world->info.pipeline_build_count_total ++;
    pq->rebuild_count ++;

    ecs_allocator_t *a = &world->allocator;
    ecs_pipeline_op_t *op = NULL;
    ecs_write_state_t ws = {0};
    ecs_map_init(&ws.ids, a);
    ecs_map_init(&ws.wildcard_ids, a);

    ecs_vec_reset_t(a, &pq->ops, ecs_pipeline_op_t);
    ecs_vec_reset_t(a, &pq->systems, ecs_system_t*);

    bool multi_threaded = false;
    bool immediate = false;
    bool first = true;

    /* Build ops: each op is a batch of systems between merge points */
    while (ecs_query_next(&it)) {
        EcsPoly *poly = flecs_pipeline_term_system(&it);
        bool is_active = ecs_table_get_type_index(
            world, it.table, EcsEmpty) == -1;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            flecs_poly_assert(poly[i].poly, ecs_system_t);
            ecs_system_t *sys = (ecs_system_t*)poly[i].poly;
            ecs_query_t *q = sys->query;

            bool needs_merge = false;
            needs_merge = flecs_pipeline_check_terms(
                world, q, is_active, &ws);

            if (is_active) {
                if (first) {
                    multi_threaded = sys->multi_threaded;
                    immediate = sys->immediate;
                    first = false;
                }

                if (sys->multi_threaded != multi_threaded) {
                    needs_merge = true;
                    multi_threaded = sys->multi_threaded;
                }
                if (sys->immediate != immediate) {
                    needs_merge = true;
                    immediate = sys->immediate;
                }
            }

            if (immediate) {
                needs_merge = true;
            }

            if (needs_merge) {
                /* Reset write state after merge point */
                flecs_pipeline_reset_write_state(&ws);

                /* Reuse the current op if it has no systems (e.g. because
                 * an inactive system triggered a merge as the only entry). */
                if (op && op->count) {
                    op = NULL;
                }

                /* Re-evaluate terms to set write flags; inactive systems
                 * can't write, so skip them to avoid unnecessary merges. */
                needs_merge = false;
                if (is_active) {
                    needs_merge = flecs_pipeline_check_terms(
                        world, q, true, &ws);
                }

                /* States just reset -- another merge here means a bug */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vec_append_t(a, &pq->ops, ecs_pipeline_op_t);
                op->offset = ecs_vec_count(&pq->systems);
                op->count = 0;
                op->multi_threaded = false;
                op->immediate = false;
                op->time_spent = 0;
                op->commands_enqueued = 0;
            }

            /* Inactive systems participate in merge analysis but are not
             * added to the schedule. */
            if (is_active) {
                ecs_vec_append_t(a, &pq->systems, ecs_system_t*)[0] = sys;
                if (!op->count) {
                    op->multi_threaded = multi_threaded;
                    op->immediate = immediate;
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
        ecs_dbg("#[bold]pipeline rebuild");
        ecs_log_push_1();

        ecs_dbg("#[green]schedule#[reset]: threading: %d, staging: %d:", 
            op->multi_threaded, !op->immediate);
        ecs_log_push_1();

        int32_t i, count = ecs_vec_count(&pq->systems);
        int32_t op_index = 0, ran_since_merge = 0;
        ecs_system_t **systems = ecs_vec_first_t(&pq->systems, ecs_system_t*);
        for (i = 0; i < count; i ++) {
            ecs_system_t *sys = systems[i];
            ecs_entity_t system = sys->query->entity;
            ecs_assert(system != 0, ECS_INTERNAL_ERROR, NULL);
            (void)system;

#ifdef FLECS_LOG_1
            char *path = ecs_get_path(world, system);
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
                        !op[op_index].immediate);
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

    pq->match_count = new_match_count;

    if (ecs_vec_count(&pq->ops)) {
        ecs_assert((pq->cur_op <= ecs_vec_last_t(&pq->ops, ecs_pipeline_op_t)),
            ECS_INTERNAL_ERROR, NULL);
    }

    return true;
}

/* Advance the pipeline cursor to the next system. */
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

/* Update the pipeline schedule, rebuilding if necessary. */
bool flecs_pipeline_update(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    bool start_of_frame)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(!(world->flags & EcsWorldReadonly), ECS_INVALID_OPERATION, 
        "cannot update pipeline while world is in readonly mode");

    /* Flush deferred operations so query caches are up to date before
     * checking whether the pipeline schedule needs rebuilding. */
    if (start_of_frame) {
        ecs_run_aperiodic(world, 0);
    }

    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    bool rebuilt = flecs_pipeline_build(world, pq);
    if (start_of_frame) {
        /* Create per-stage iterators for this frame */
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

    /* Create temporary worker threads if using task API */
    if (ecs_using_task_threads(world)) {
        flecs_create_worker_threads(world);
    }

    EcsPipeline *p = 
        ECS_CONST_CAST(EcsPipeline*, ecs_get(world, pipeline, EcsPipeline));
    flecs_workers_progress(world, p->state, delta_time);

    if (ecs_using_task_threads(world)) {
        /* Task threads are ephemeral; join them now */
        flecs_join_worker_threads(world);
    }
}

/* Run pipeline operations for a single stage until the next merge point. */
int32_t flecs_run_pipeline_ops(
    ecs_world_t* world,
    ecs_stage_t* stage,
    int32_t stage_index,
    int32_t stage_count,
    ecs_ftime_t delta_time)
{
    ecs_pipeline_state_t* pq = world->pq;
    ecs_pipeline_op_t* op = pq->cur_op;
    int32_t i = pq->cur_i;

    ecs_assert(!stage_index || op->multi_threaded, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vec_count(&pq->systems);
    ecs_system_t **systems = ecs_vec_first_t(&pq->systems, ecs_system_t*);
    int32_t ran_since_merge = i - op->offset;

    for (; i < count; i++) {
        ecs_system_t* sys = systems[i];

        /* Track last-run frame so the schedule can resume correctly
         * after a mid-frame pipeline rebuild. */
        if (stage_index == 0) {
            sys->last_frame = world->info.frame_count_total + 1;
        }

        ecs_stage_t* s = NULL;
        if (!op->immediate) {
            /* Non-immediate systems write to a deferred stage */
            s = stage;
        }

        flecs_run_system(world, s, sys->query->entity, sys, stage_index,
            stage_count, delta_time, NULL);

        ecs_os_linc(&world->info.systems_ran_total);
        ran_since_merge++;

        if (ran_since_merge == op->count) {
            break; /* Reached merge point */
        }
    }

    return i;
}

/* Run the full pipeline, coordinating workers and merge points. */
void flecs_run_pipeline(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);
    flecs_poly_assert(world, ecs_stage_t);

    ecs_stage_t *stage = flecs_stage_from_world(&world);  
    int32_t stage_index = ecs_stage_get_id(stage->thread_ctx);
    int32_t stage_count = ecs_get_stage_count(world);
    bool multi_threaded = world->worker_cond != 0;

    ecs_assert(!stage_index, ECS_INVALID_OPERATION, 
        "cannot run pipeline on stage");

    /* Publish pipeline state so worker threads can access it */
    world->pq = pq;

    /* Build/rebuild the schedule before waking workers */
    flecs_pipeline_update(world, pq, true);

    /* Main loop: each iteration runs one op batch then merges. The loop
     * exits when there are no more ops (cur_op == NULL). */
    while (pq->cur_op != NULL) {
        if (pq->cur_i == ecs_vec_count(&pq->systems)) {
            flecs_pipeline_update(world, pq, false);
            continue;
        }

        bool immediate = pq->cur_op->immediate;
        bool op_multi_threaded = multi_threaded && pq->cur_op->multi_threaded;

        pq->immediate = immediate;

        if (!immediate) {
            ecs_readonly_begin(world, multi_threaded);
        } else {
            flecs_defer_begin(world, stage);
        }

        ECS_BIT_COND(world->flags, EcsWorldMultiThreaded, op_multi_threaded);
        ecs_assert(world->workers_waiting == 0, ECS_INTERNAL_ERROR, NULL);

        if (op_multi_threaded) {
            flecs_signal_workers(world);
        }

        ecs_time_t st = { 0 };
        bool measure_time = world->flags & EcsWorldMeasureSystemTime;
        if (measure_time) {
            ecs_time_measure(&st);
        }

        const int32_t i = flecs_run_pipeline_ops(
            world, stage, stage_index, stage_count, delta_time);

        if (measure_time) {
            /* Exclude merge time from system time measurement */
            world->info.system_time_total += (ecs_ftime_t)ecs_time_measure(&st);
        }

        if (op_multi_threaded) {
            flecs_wait_for_sync(world);
        }

        if (!immediate) {
            ecs_time_t mt = { 0 };
            if (measure_time) {
                ecs_time_measure(&mt);
            }

            int32_t si;
            for (si = 0; si < stage_count; si ++) {
                ecs_stage_t *s = world->stages[si];
                pq->cur_op->commands_enqueued += ecs_vec_count(&s->cmd->queue);
            }

            ecs_readonly_end(world);
            if (measure_time) {
                pq->cur_op->time_spent += ecs_time_measure(&mt);
            }
        } else {
            flecs_defer_end(world, stage);
        }

        /* Update schedule cursor after sync to avoid races with workers */
        pq->cur_i = i;

        flecs_pipeline_update(world, pq, false);
    }
}

/* Create and run a temporary pipeline for startup systems. */
static
void flecs_run_startup_systems(
    ecs_world_t *world)
{
    ecs_component_record_t *cr = flecs_components_get(world, 
        ecs_dependson(EcsOnStart));
    if (!cr || !flecs_table_cache_count(&cr->cache)) {
        return; /* No startup systems registered */
    }

    ecs_dbg_2("#[bold]startup#[reset]");
    ecs_log_push_2();
    int32_t stage_count = world->stage_count;
    world->stage_count = 1; /* Run startup systems single-threaded */

    /* Startup pipeline is created, run, then deleted to avoid ongoing
     * query cache maintenance for systems that only run once. */
    ecs_dbg_2("#[bold]create startup pipeline#[reset]");
    ecs_log_push_2();
    ecs_entity_t start_pip = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
        .query = {
            .terms = {
                { .id = EcsSystem },
                { .id = EcsPhase, .src.id = EcsCascade, .trav = EcsDependsOn },
                { .id = ecs_dependson(EcsOnStart), .trav = EcsDependsOn },
                { .id = EcsDisabled, .src.id = EcsUp, .trav = EcsDependsOn, .oper = EcsNot },
                { .id = EcsDisabled, .src.id = EcsUp, .trav = EcsChildOf, .oper = EcsNot }
            },
            .order_by_callback = flecs_entity_compare
        }
    });
    ecs_log_pop_2();

    ecs_dbg_2("#[bold]run startup systems#[reset]");
    ecs_log_push_2();
    ecs_assert(start_pip != 0, ECS_INTERNAL_ERROR, NULL);
    const EcsPipeline *p = ecs_get(world, start_pip, EcsPipeline);
    ecs_check(p != NULL, ECS_INVALID_OPERATION, 
        "pipeline entity is missing flecs.pipeline.Pipeline component");
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
    
    /* Run startup systems on the first frame only */
    if (world->info.frame_count_total == 0) {
        flecs_run_startup_systems(world);
    }

    /* Create temporary worker threads if using task API */
    if (ecs_using_task_threads(world)) {
        flecs_create_worker_threads(world);
    }

    ecs_dbg_3("#[bold]progress#[reset](dt = %.2f)", (double)delta_time);
    ecs_log_push_3();
    const EcsPipeline *p = ecs_get(world, world->pipeline, EcsPipeline);
    ecs_check(p != NULL, ECS_INVALID_OPERATION,
        "pipeline entity is missing flecs.pipeline.Pipeline component");
    flecs_workers_progress(world, p->state, delta_time);
    ecs_log_pop_3();

    ecs_frame_end(world);

    if (ecs_using_task_threads(world)) {
        /* Task threads are ephemeral; join them now */
        flecs_join_worker_threads(world);
    }

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
    flecs_poly_assert(world, ecs_world_t);
    ecs_check( ecs_get(world, pipeline, EcsPipeline) != NULL, 
        ECS_INVALID_PARAMETER, "not a pipeline");

    world->pipeline = pipeline;
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
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(desc != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t result = desc->entity;
    if (!result) {
        result = ecs_new(world);
    }

    ecs_query_desc_t qd = desc->query;
    if (!qd.order_by_callback) {
        qd.order_by_callback = flecs_entity_compare;
    }
    qd.entity = result;

    ecs_query_t *query = ecs_query_init(world, &qd);
    if (!query) {
        ecs_delete(world, result);
        return 0;
    }

    ecs_check(query->terms != NULL, ECS_INVALID_PARAMETER, 
        "pipeline query cannot be empty");
    ecs_check(query->terms[0].id == EcsSystem,
        ECS_INVALID_PARAMETER, "pipeline must start with System term");

    ecs_pipeline_state_t *pq = ecs_os_calloc_t(ecs_pipeline_state_t);
    pq->query = query;
    pq->match_count = -1;
    ecs_set(world, result, EcsPipeline, { pq });

    return result;
error:
    return 0;
}

/* Clean up pipeline threads when the world is destroyed. */
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
    flecs_bootstrap_phase_(world, phase, depends_on)
/* Register a phase entity with an optional dependency. */
static
void flecs_bootstrap_phase_(
    ecs_world_t *world,
    ecs_entity_t phase,
    ecs_entity_t depends_on)
{
    ecs_add_id(world, phase, EcsPhase);
    if (depends_on) {
        ecs_add_pair(world, phase, EcsDependsOn, depends_on);
    }
}

/* Import the pipeline module and register builtin phases. */
void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);
    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsPipeline);
    flecs_bootstrap_tag(world, EcsPhase);

    /* Create anonymous intermediate phases between each builtin phase.
     * Builtin phases depend on these instead of on each other directly,
     * so disabling e.g. EcsPreUpdate does not prevent EcsOnUpdate from
     * running. */
    ecs_entity_t phase_0 = ecs_entity(world, {0});
    ecs_entity_t phase_1 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_0)) });
    ecs_entity_t phase_2 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_1)) });
    ecs_entity_t phase_3 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_2)) });
    ecs_entity_t phase_4 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_3)) });
    ecs_entity_t phase_5 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_4)) });
    ecs_entity_t phase_6 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_5)) });
    ecs_entity_t phase_7 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_6)) });
    ecs_entity_t phase_8 = ecs_entity(world, { .add = ecs_ids(ecs_dependson(phase_7)) });

    flecs_bootstrap_phase(world, EcsOnStart,    0);
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
        .ctor = flecs_default_ctor,
        .dtor = ecs_dtor(EcsPipeline),
        .move = ecs_move(EcsPipeline)
    });

    world->pipeline = ecs_pipeline(world, {
        .entity = ecs_entity(world, { .name = "BuiltinPipeline" }),
        .query = {
            .terms = {
                { .id = EcsSystem },
                { .id = EcsPhase, .src.id = EcsCascade, .trav = EcsDependsOn },
                { .id = ecs_dependson(EcsOnStart), .trav = EcsDependsOn, .oper = EcsNot },
                { .id = EcsDisabled, .src.id = EcsUp, .trav = EcsDependsOn, .oper = EcsNot },
                { .id = EcsDisabled, .src.id = EcsUp, .trav = EcsChildOf, .oper = EcsNot }
            },
            .order_by_callback = flecs_entity_compare
        }
    });

    /* Clean up worker threads on world destruction */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif
