/**
 * @file addons/pipeline/worker.c
 * @brief Functions for running pipelines on one or more threads.
 */

#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

typedef struct ecs_worker_state_t {
    ecs_stage_t *stage;
    ecs_pipeline_state_t *pq;
} ecs_worker_state_t;

/* Worker thread */
static
void* flecs_worker(void *arg) {
    ecs_worker_state_t *state = arg;
    ecs_stage_t *stage = state->stage;
    ecs_pipeline_state_t *pq = state->pq;
    ecs_world_t *world = stage->world;

    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_assert(stage, ecs_stage_t);

    ecs_dbg_2("worker %d: start", stage->id);

    /* Start worker, increase counter so main thread knows how many
     * workers are ready */
    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running ++;

    if (!(world->flags & EcsWorldQuitWorkers)) {
        ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    }

    ecs_os_mutex_unlock(world->sync_mutex);

    while (!(world->flags & EcsWorldQuitWorkers)) {
        ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)stage, 0);

        ecs_dbg_3("worker %d: run", stage->id);
 
        flecs_run_pipeline((ecs_world_t*)stage, pq, world->info.delta_time);

        ecs_set_scope((ecs_world_t*)stage, old_scope);
    }

    ecs_dbg_2("worker %d: finalizing", stage->id);

    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running --;
    ecs_os_mutex_unlock(world->sync_mutex);

    ecs_dbg_2("worker %d: stop", stage->id);

    ecs_os_free(state);

    return NULL;
}

static
bool flecs_is_multithreaded(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    return ecs_get_stage_count(world) > 1;
}

static
bool flecs_is_main_thread(
    ecs_stage_t *stage)
{
    return !stage->id;
}

/* Start threads */
static
void flecs_start_workers(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_set_stage_count(world, threads);

    ecs_assert(ecs_get_stage_count(world) == threads, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < threads - 1; i ++) {
        ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i + 1);
        ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_poly_assert(stage, ecs_stage_t);

        ecs_entity_t pipeline = world->pipeline;
        ecs_assert(pipeline != 0, ECS_INVALID_OPERATION, NULL);
        const EcsPipeline *pqc = ecs_get(world, pipeline, EcsPipeline);
        ecs_assert(pqc != NULL, ECS_INVALID_OPERATION, NULL);
        ecs_pipeline_state_t *pq = pqc->state;
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_worker_state_t *state = ecs_os_calloc_t(ecs_worker_state_t);
        state->stage = stage;
        state->pq = pq;
        stage->thread = ecs_os_thread_new(flecs_worker, state);
        ecs_assert(stage->thread != 0, ECS_OPERATION_FAILED, NULL);
    }
}

/* Wait until all workers are running */
static
void flecs_wait_for_workers(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);

    int32_t stage_count = ecs_get_stage_count(world);
    if (stage_count <= 1) {
        return;
    }

    bool wait = true;
    do {
        ecs_os_mutex_lock(world->sync_mutex);
        if (world->workers_running == (stage_count - 1)) {
            wait = false;
        }
        ecs_os_mutex_unlock(world->sync_mutex);
    } while (wait);
}

/* Wait until all threads are waiting on sync point */
static
void flecs_wait_for_sync(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);
    if (stage_count <= 1) {
        return;
    }

    ecs_dbg_3("#[bold]pipeline: waiting for worker sync");

    ecs_os_mutex_lock(world->sync_mutex);
    if (world->workers_waiting != (stage_count - 1)) {
        ecs_os_cond_wait(world->sync_cond, world->sync_mutex);
    }

    /* We shouldn't have been signalled unless all workers are waiting on sync */
    ecs_assert(world->workers_waiting == (stage_count - 1), 
        ECS_INTERNAL_ERROR, NULL);

    world->workers_waiting = 0;
    ecs_os_mutex_unlock(world->sync_mutex);

    ecs_dbg_3("#[bold]pipeline: workers synced");
}

/* Synchronize workers */
static
void flecs_sync_worker(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);
    if (stage_count <= 1) {
        return;
    }

    /* Signal that thread is waiting */
    ecs_os_mutex_lock(world->sync_mutex);
    if (++ world->workers_waiting == (stage_count - 1)) {
        /* Only signal main thread when all threads are waiting */
        ecs_os_cond_signal(world->sync_cond);
    }

    /* Wait until main thread signals that thread can continue */
    ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Signal workers that they can start/resume work */
static
void flecs_signal_workers(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);
    if (stage_count <= 1) {
        return;
    }

    ecs_dbg_3("#[bold]pipeline: signal workers");
    ecs_os_mutex_lock(world->sync_mutex);
    ecs_os_cond_broadcast(world->worker_cond);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/** Stop workers */
static
bool ecs_stop_threads(
    ecs_world_t *world)
{
    bool threads_active = false;

    /* Test if threads are created. Cannot use workers_running, since this is
     * a potential race if threads haven't spun up yet. */
    ecs_stage_t *stages = world->stages;
    int i, count = world->stage_count;
    for (i = 1; i < count; i ++) {
        ecs_stage_t *stage = &stages[i];
        if (stage->thread) {
            threads_active = true;
            break;
        }
        stage->thread = 0;
    };

    /* If no threads are active, just return */
    if (!threads_active) {
        return false;
    }

    /* Make sure all threads are running, to ensure they catch the signal */
    flecs_wait_for_workers(world);

    /* Signal threads should quit */
    world->flags |= EcsWorldQuitWorkers;
    flecs_signal_workers(world);

    /* Join all threads with main */
    for (i = 1; i < count; i ++) {
        ecs_os_thread_join(stages[i].thread);
        stages[i].thread = 0;
    }

    world->flags &= ~EcsWorldQuitWorkers;
    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);

    /* Deinitialize stages */
    ecs_set_stage_count(world, 1);

    return true;
}

/* -- Private functions -- */
bool flecs_worker_begin(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_pipeline_state_t *pq,
    bool start_of_frame)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_assert(stage, ecs_stage_t);
    bool main_thread = flecs_is_main_thread(stage);
    bool multi_threaded = flecs_is_multithreaded(world);

    if (main_thread) {
        if (ecs_stage_is_readonly(world)) {
            ecs_assert(!pq->no_readonly, ECS_INTERNAL_ERROR, NULL);
            ecs_readonly_end(world);
            pq->no_readonly = false;
        }

        flecs_pipeline_update(world, pq, start_of_frame);
    }

    ecs_pipeline_op_t *cur_op = pq->cur_op;
    if (main_thread && (cur_op != NULL)) {
        pq->no_readonly = cur_op->no_readonly;
        if (!cur_op->no_readonly) {
            ecs_readonly_begin(world);
        }

        ECS_BIT_COND(world->flags, EcsWorldMultiThreaded, 
            cur_op->multi_threaded);
        ecs_assert(world->workers_waiting == 0, 
            ECS_INTERNAL_ERROR, NULL);
    }

    if (main_thread && multi_threaded) {
        flecs_signal_workers(world);
    }

    return pq->cur_op != NULL;
}

void flecs_worker_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_assert(stage, ecs_stage_t);

    if (flecs_is_multithreaded(world)) {
        if (flecs_is_main_thread(stage)) {
            flecs_wait_for_sync(world);
        } else {
            flecs_sync_worker(world);
        }
    }

    if (flecs_is_main_thread(stage)) {
        if (ecs_stage_is_readonly(world)) {
            ecs_readonly_end(world);
        }
    }
}

bool flecs_worker_sync(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_pipeline_state_t *pq,
    ecs_pipeline_op_t **cur_op,
    int32_t *cur_i)
{
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->cur_op != NULL, ECS_INTERNAL_ERROR, NULL);
    bool main_thread = flecs_is_main_thread(stage);

    /* Synchronize workers */
    flecs_worker_end(world, stage);

    /* Store the current state of the schedule after we synchronized the
     * threads, to avoid race conditions. */
    if (main_thread) {
        pq->cur_op = *cur_op;
        pq->cur_i = *cur_i;
    }

    /* Prepare state for running the next part of the schedule */
    bool result = flecs_worker_begin(world, stage, pq, false);
    *cur_op = pq->cur_op;
    *cur_i = pq->cur_i;
    return result;
}

void flecs_workers_progress(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(!ecs_is_deferred(world), ECS_INVALID_OPERATION, NULL);

    /* Make sure workers are running and ready */
    flecs_wait_for_workers(world);

    /* Run pipeline on main thread */
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)stage, 0);
    flecs_run_pipeline(stage, pq, delta_time);
    ecs_set_scope((ecs_world_t*)stage, old_scope);
}

/* -- Public functions -- */

void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_assert(threads <= 1 || ecs_os_has_threading(), ECS_MISSING_OS_API, NULL);

    int32_t stage_count = ecs_get_stage_count(world);

    if (stage_count != threads) {
        /* Stop existing threads */
        if (stage_count > 1) {
            if (ecs_stop_threads(world)) {
                ecs_os_cond_free(world->worker_cond);
                ecs_os_cond_free(world->sync_cond);
                ecs_os_mutex_free(world->sync_mutex);
            }
        }

        /* Start threads if number of threads > 1 */
        if (threads > 1) {
            world->worker_cond = ecs_os_cond_new();
            world->sync_cond = ecs_os_cond_new();
            world->sync_mutex = ecs_os_mutex_new();
            flecs_start_workers(world, threads);
        }
    }
}

#endif
