#include "flecs.h"
#include "../system/system.h"

#ifdef FLECS_PIPELINE
#include "pipeline.h"

/* Worker thread */
static
void* worker(void *arg) {
    ecs_stage_t *stage = arg;
    ecs_world_t *world = stage->world;

    /* Start worker thread, increase counter so main thread knows how many
     * workers are ready */
    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running ++;

    if (!world->quit_workers) {
        ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    }

    ecs_os_mutex_unlock(world->sync_mutex);

    while (!world->quit_workers) {
        ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)stage, 0);
 
        ecs_run_pipeline(
            (ecs_world_t*)stage, 
            world->pipeline, 
            world->stats.delta_time);

        ecs_set_scope((ecs_world_t*)stage, old_scope);
    }

    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running --;
    ecs_os_mutex_unlock(world->sync_mutex);

    return NULL;
}

/* Start threads */
static
void start_workers(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_set_stages(world, threads);

    ecs_assert(ecs_get_stage_count(world) == threads, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < threads; i ++) {
        ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
        ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_poly_assert(stage, ecs_stage_t);

        ecs_vector_get(world->worker_stages, ecs_stage_t, i);
        stage->thread = ecs_os_thread_new(worker, stage);
        ecs_assert(stage->thread != 0, ECS_OPERATION_FAILED, NULL);
    }
}

/* Wait until all workers are running */
static
void wait_for_workers(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);
    bool wait = true;

    do {
        ecs_os_mutex_lock(world->sync_mutex);
        if (world->workers_running == stage_count) {
            wait = false;
        }
        ecs_os_mutex_unlock(world->sync_mutex);
    } while (wait);
}

/* Synchronize worker threads */
static
void sync_worker(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);

    /* Signal that thread is waiting */
    ecs_os_mutex_lock(world->sync_mutex);
    if (++ world->workers_waiting == stage_count) {
        /* Only signal main thread when all threads are waiting */
        ecs_os_cond_signal(world->sync_cond);
    }

    /* Wait until main thread signals that thread can continue */
    ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Wait until all threads are waiting on sync point */
static
void wait_for_sync(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_os_mutex_lock(world->sync_mutex);
    if (world->workers_waiting != stage_count) {
        ecs_os_cond_wait(world->sync_cond, world->sync_mutex);
    }
    
    /* We should have been signalled unless all workers are waiting on sync */
    ecs_assert(world->workers_waiting == stage_count, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Signal workers that they can start/resume work */
static
void signal_workers(
    ecs_world_t *world)
{
    ecs_os_mutex_lock(world->sync_mutex);
    ecs_os_cond_broadcast(world->worker_cond);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/** Stop worker threads */
static
bool ecs_stop_threads(
    ecs_world_t *world)
{
    bool threads_active = false;

    /* Test if threads are created. Cannot use workers_running, since this is
     * a potential race if threads haven't spun up yet. */
    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        if (stage->thread) {
            threads_active = true;
            break;
        }
        stage->thread = 0;
    });

    /* If no threads are active, just return */
    if (!threads_active) {
        return false;
    }

    /* Make sure all threads are running, to ensure they catch the signal */
    wait_for_workers(world);

    /* Signal threads should quit */
    world->quit_workers = true;
    signal_workers(world);

    /* Join all threads with main */
    ecs_stage_t *stages = ecs_vector_first(world->worker_stages, ecs_stage_t);
    int32_t i, count = ecs_vector_count(world->worker_stages);
    for (i = 0; i < count; i ++) {
        ecs_os_thread_join(stages[i].thread);
        stages[i].thread = 0;
    }

    world->quit_workers = false;
    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);

    /* Deinitialize stages */
    ecs_set_stages(world, 0);

    return true;
}

/* -- Private functions -- */

void ecs_worker_begin(
    ecs_world_t *world)
{
    flecs_stage_from_world(&world);
    int32_t stage_count = ecs_get_stage_count(world);
    ecs_assert(stage_count != 0, ECS_INTERNAL_ERROR, NULL);
    
    if (stage_count == 1) {
        ecs_entity_t pipeline = world->pipeline;
        const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
        if (!op || !op->no_staging) {
            ecs_staging_begin(world);
        }
    }
}

int32_t ecs_worker_sync(
    ecs_world_t *world,
    const EcsPipelineQuery *pq,
    ecs_iter_t *it,
    int32_t i,
    ecs_pipeline_op_t **op_out,
    ecs_pipeline_op_t **last_op_out)
{
    int32_t stage_count = ecs_get_stage_count(world);
    ecs_assert(stage_count != 0, ECS_INTERNAL_ERROR, NULL);
    int32_t build_count = world->stats.pipeline_build_count_total;

    /* If there are no threads, merge in place */
    if (stage_count == 1) {
        if (!op_out[0]->no_staging) {
            ecs_staging_end(world);
        }

        ecs_pipeline_update(world, world->pipeline, false);

    /* Synchronize all workers. The last worker to reach the sync point will
     * signal the main thread, which will perform the merge. */
    } else {
        sync_worker(world);
    }

    if (build_count != world->stats.pipeline_build_count_total) {
        i = ecs_pipeline_reset_iter(world, pq, it, op_out, last_op_out);
    } else {
        op_out[0] ++;
    }

    if (stage_count == 1) {
        if (!op_out[0]->no_staging) {
            ecs_staging_begin(world);
        }
    }

    return i;
}

void ecs_worker_end(
    ecs_world_t *world)
{
    flecs_stage_from_world(&world);

    int32_t stage_count = ecs_get_stage_count(world);
    ecs_assert(stage_count != 0, ECS_INTERNAL_ERROR, NULL);

    /* If there are no threads, merge in place */
    if (stage_count == 1) {
        if (ecs_stage_is_readonly(world)) {
            ecs_staging_end(world);
        }

    /* Synchronize all workers. The last worker to reach the sync point will
     * signal the main thread, which will perform the merge. */
    } else {
        sync_worker(world);
    }
}

void ecs_workers_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time)
{
    ecs_poly_assert(world, ecs_world_t);
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_time_t start = {0};
    if (world->measure_frame_time) {
        ecs_time_measure(&start);
    }

    if (stage_count == 1) {
        ecs_pipeline_update(world, pipeline, true);
        ecs_entity_t old_scope = ecs_set_scope(world, 0);
        ecs_world_t *stage = ecs_get_stage(world, 0);
        ecs_run_pipeline(stage, pipeline, delta_time);
        ecs_set_scope(world, old_scope);
    } else {
        ecs_pipeline_update(world, pipeline, true);

        const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
        ecs_vector_t *ops = pq->ops;
        ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
        ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);

        /* Make sure workers are running and ready */
        wait_for_workers(world);

        /* Synchronize n times for each op in the pipeline */
        for (; op <= op_last; op ++) {
            if (!op->no_staging) {
                ecs_staging_begin(world);
            }

            /* Signal workers that they should start running systems */
            world->workers_waiting = 0;
            signal_workers(world);

            /* Wait until all workers are waiting on sync point */
            wait_for_sync(world);

            /* Merge */
            if (!op->no_staging) {
                ecs_staging_end(world);
            }

            if (ecs_pipeline_update(world, pipeline, false)) {
                /* Refetch, in case pipeline itself has moved */
                pq = ecs_get(world, pipeline, EcsPipelineQuery);

                /* Pipeline has changed, reset position in pipeline */
                ecs_iter_t it;
                ecs_pipeline_reset_iter(world, pq, &it, &op, &op_last);
                op --;
            }
        }
    }

    if (world->measure_frame_time) {
        world->stats.system_time_total += (float)ecs_time_measure(&start);
    }    
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
            start_workers(world, threads);
        }
    }
}

#endif
