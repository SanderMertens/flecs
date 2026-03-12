/**
 * @file addons/pipeline/worker.c
 * @brief Functions for running pipelines on one or more threads.
 */

#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

/* Synchronize a worker thread with the main thread. */
static
void flecs_sync_worker(
    ecs_world_t* world)
{
    int32_t stage_count = ecs_get_stage_count(world);
    if (stage_count <= 1) {
        return;
    }

    /* Signal that thread is waiting */
    ecs_os_mutex_lock(world->sync_mutex);
    if (++world->workers_waiting == (stage_count - 1)) {
        /* Only signal main thread when all threads are waiting */
        ecs_os_cond_signal(world->sync_cond);
    }

    /* Wait until main thread signals that thread can continue */
    ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Worker thread entry point that runs pipeline operations. */
static
void* flecs_worker(void *arg) {
    ecs_stage_t *stage = arg;
    ecs_world_t *world = stage->world;

    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);

    ecs_dbg_2("worker %d: start", stage->id);

    /* Increment running counter; main thread spins until all workers report */
    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running ++;

    if (!(world->flags & EcsWorldQuitWorkers)) {
        ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    }

    ecs_os_mutex_unlock(world->sync_mutex);

    while (!(world->flags & EcsWorldQuitWorkers)) {
        ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)stage, 0);

        ecs_dbg_3("worker %d: run", stage->id);
        flecs_run_pipeline_ops(world, stage, stage->id, world->stage_count, 
            world->info.delta_time);

        ecs_set_scope((ecs_world_t*)stage, old_scope);

        flecs_sync_worker(world);
    }

    ecs_dbg_2("worker %d: finalizing", stage->id);

    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running --;
    ecs_os_mutex_unlock(world->sync_mutex);

    ecs_dbg_2("worker %d: stop", stage->id);

    return NULL;
}

/* Create worker threads or tasks for each stage. */
void flecs_create_worker_threads(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    int32_t stages = ecs_get_stage_count(world);

    for (int32_t i = 1; i < stages; i ++) {
        ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
        ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_poly_assert(stage, ecs_stage_t);

        ecs_assert(stage->thread == 0, ECS_INTERNAL_ERROR, NULL);
        if (ecs_using_task_threads(world)) {
            stage->thread = ecs_os_task_new(flecs_worker, stage);
        } else {
            stage->thread = ecs_os_thread_new(flecs_worker, stage);
        }
        ecs_assert(stage->thread != 0, ECS_OPERATION_FAILED,
            "failed to create thread");
    }
}

/* Set stage count and start worker threads. */
static
void flecs_start_workers(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_set_stage_count(world, threads);

    ecs_assert(ecs_get_stage_count(world) == threads, ECS_INTERNAL_ERROR, NULL);

    if (!ecs_using_task_threads(world)) {
        flecs_create_worker_threads(world);
    }
}

/* Wait until all worker threads are running. */
static
void flecs_wait_for_workers(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);

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

/* Wait until all worker threads reach the sync point. */
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

    /* All workers must be waiting when we receive the sync signal */
    ecs_assert(world->workers_waiting == (stage_count - 1), 
        ECS_INTERNAL_ERROR, NULL);

    world->workers_waiting = 0;
    ecs_os_mutex_unlock(world->sync_mutex);

    ecs_dbg_3("#[bold]pipeline: workers synced");
}

/* Signal worker threads to start or resume work. */
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

/* Signal workers to quit and join all worker threads. */
void flecs_join_worker_threads(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    bool threads_active = false;

    /* Check thread handles directly; workers_running counter is unreliable
     * before all workers have completed startup. */
    int i, count = world->stage_count;
    for (i = 1; i < count; i ++) {
        ecs_stage_t *stage = world->stages[i];
        if (stage->thread) {
            threads_active = true;
            break;
        }
    };

    if (!threads_active) {
        return;
    }

    /* Ensure all workers are running before signaling quit */
    flecs_wait_for_workers(world);

    world->flags |= EcsWorldQuitWorkers;
    flecs_signal_workers(world);

    /* Join all worker threads */
    for (i = 1; i < count; i ++) {
        ecs_stage_t *stage = world->stages[i];
        if (ecs_using_task_threads(world)) {
            ecs_os_task_join(stage->thread);
        } else {
            ecs_os_thread_join(stage->thread);
        }
        stage->thread = 0;
    }

    world->flags &= ~EcsWorldQuitWorkers;
    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);
}

/* Entry point for pipeline progress: waits for workers, runs on stage 0. */
void flecs_workers_progress(
    ecs_world_t *world,
    ecs_pipeline_state_t *pq,
    ecs_ftime_t delta_time)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(!ecs_is_deferred(world), ECS_INVALID_OPERATION, 
        "cannot call progress while world is deferred");

    flecs_wait_for_workers(world);

    /* Run pipeline on main thread (stage 0) */
    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)stage, 0);
    flecs_run_pipeline(stage, pq, delta_time);
    ecs_set_scope((ecs_world_t*)stage, old_scope);
}

/* Configure the number of worker threads or tasks. */
static
void flecs_set_threads_internal(
    ecs_world_t *world,
    int32_t threads,
    bool use_task_api)
{
    ecs_assert(threads <= 1 || (use_task_api 
        ? ecs_os_has_task_support() 
        : ecs_os_has_threading()), 
            ECS_MISSING_OS_API, NULL);

    int32_t stage_count = ecs_get_stage_count(world);
    bool worker_method_changed = (use_task_api != world->workers_use_task_api);

    if ((stage_count != threads) || worker_method_changed) {
        if (stage_count > 1) {
            flecs_join_worker_threads(world);
            ecs_set_stage_count(world, 1);

            if (world->worker_cond) {
                ecs_os_cond_free(world->worker_cond);
            }
            if (world->sync_cond) {
                ecs_os_cond_free(world->sync_cond);
            }
            if (world->sync_mutex) {
                ecs_os_mutex_free(world->sync_mutex);
            }
        }

        world->workers_use_task_api = use_task_api;

        if (threads > 1) {
            world->worker_cond = ecs_os_cond_new();
            world->sync_cond = ecs_os_cond_new();
            world->sync_mutex = ecs_os_mutex_new();
            flecs_start_workers(world, threads);
        }
    }
}

void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads)
{
    flecs_set_threads_internal(world, threads, false /* use thread API */);
}

void ecs_set_task_threads(
    ecs_world_t *world,
    int32_t task_threads)
{
    flecs_set_threads_internal(world, task_threads, true /* use task API */);
}

bool ecs_using_task_threads(
    ecs_world_t *world)
{
    return world->workers_use_task_api;
}

#endif
