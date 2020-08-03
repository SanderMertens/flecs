#include "flecs.h"

#ifdef FLECS_PIPELINE

#include "pipeline.h"

/* Worker thread */
static
void* worker(void *arg) {
    ecs_thread_t *thread = arg;
    ecs_world_t *world = thread->world;

    /* Start worker thread, increase counter so main thread knows how many
     * workers are ready */
    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running ++;

    if (!world->quit_workers) {
        ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    }

    ecs_os_mutex_unlock(world->sync_mutex);

    while (!world->quit_workers) {
        ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)thread, 0);
        ecs_pipeline_progress(
            (ecs_world_t*)thread, 
            world->pipeline, 
            world->stats.delta_time);
        ecs_set_scope((ecs_world_t*)thread, old_scope);
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
    ecs_assert(world->workers == NULL, ECS_INTERNAL_ERROR, NULL);

    world->workers = ecs_vector_new(ecs_thread_t, threads);
    world->worker_stages = ecs_vector_new(ecs_stage_t, threads);

    int32_t i;
    for (i = 0; i < threads; i ++) {
        ecs_thread_t *thread =
            ecs_vector_add(&world->workers, ecs_thread_t);

        thread->magic = ECS_THREAD_MAGIC;
        thread->world = world;
        thread->thread = 0;
        thread->index = i;

        thread->stage = ecs_vector_add(&world->worker_stages, ecs_stage_t);
        ecs_stage_init(world, thread->stage);
        thread->stage->id = 2 + i; /* 0 and 1 are reserved for main and temp */
        thread->stage->world = (ecs_world_t*)thread;

        thread->thread = ecs_os_thread_new(worker, thread);
        ecs_assert(thread->thread != 0, ECS_THREAD_ERROR, NULL);
    }
}

/* Wait until all workers are running */
static
void wait_for_workers(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);
    bool wait = true;

    do {
        ecs_os_mutex_lock(world->sync_mutex);
        if (world->workers_running == thread_count) {
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
    int32_t thread_count = ecs_vector_count(world->workers);

    /* Signal that thread is waiting */
    ecs_os_mutex_lock(world->sync_mutex);
    if (++ world->workers_waiting == thread_count) {
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
    int32_t thread_count = ecs_vector_count(world->workers);

    ecs_os_mutex_lock(world->sync_mutex);
    if (world->workers_waiting != thread_count) {
        ecs_os_cond_wait(world->sync_cond, world->sync_mutex);
    }
    
    /* We should have been signalled unless all workers are waiting on sync */
    ecs_assert(world->workers_waiting == thread_count, 
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
void ecs_stop_threads(
    ecs_world_t *world)
{
    world->quit_workers = true;
    signal_workers(world);

    ecs_vector_each(world->workers, ecs_thread_t, thr, {
        ecs_os_thread_join(thr->thread);
        ecs_stage_deinit(world, thr->stage);
    });

    ecs_vector_free(world->workers);
    ecs_vector_free(world->worker_stages);
    world->worker_stages = NULL;
    world->workers = NULL;
    world->quit_workers = false;
    
    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);
}

/* -- Private functions -- */

void ecs_worker_begin(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);
    if (!thread_count) {
        ecs_staging_begin(world);
    }
}

bool ecs_worker_sync(
    ecs_world_t *world)
{
    int32_t build_count = world->stats.pipeline_build_count_total;

    int32_t thread_count = ecs_vector_count(world->workers);
    if (!thread_count) {
        ecs_staging_end(world, false);

        ecs_pipeline_update(world, world->pipeline);

        ecs_staging_begin(world);
    } else {
        sync_worker(world);
    }

    return world->stats.pipeline_build_count_total != build_count;
}

void ecs_worker_end(
    ecs_world_t *world)
{
    int32_t thread_count = ecs_vector_count(world->workers);
    if (!thread_count) {
        ecs_staging_end(world, false);
    } else {
        sync_worker(world);
    }
}

void ecs_workers_progress(
    ecs_world_t *world)
{
    ecs_entity_t pipeline = world->pipeline;
    int32_t thread_count = ecs_vector_count(world->workers);

    ecs_time_t start = {0};
    if (world->measure_frame_time) {
        ecs_time_measure(&start);
    }

    world->stats.systems_ran_frame = 0;

    if (thread_count <= 1) {
        ecs_pipeline_begin(world, pipeline);
        ecs_entity_t old_scope = ecs_set_scope(world, 0);
        ecs_pipeline_progress(world, pipeline, world->stats.delta_time);
        ecs_set_scope(world, old_scope);
        ecs_pipeline_end(world);
    } else {
        int32_t i, sync_count = ecs_pipeline_begin(world, pipeline);

        /* Make sure workers are running and ready */
        wait_for_workers(world);

        /* Synchronize n times for each op in the pipeline */
        for (i = 0; i < sync_count; i ++) {
            ecs_staging_begin(world);

            /* Signal workers that they should start running systems */
            world->workers_waiting = 0;
            signal_workers(world);

            /* Wait until all workers are waiting on sync point */
            wait_for_sync(world);

            /* Merge */
            ecs_staging_end(world, false);

            int32_t update_count;
            if ((update_count = ecs_pipeline_update(world, pipeline))) {
                /* The number of operations in the pipeline could have changed
                 * as result of the merge */
                sync_count = update_count;
            }
        }

        ecs_pipeline_end(world);
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
    ecs_assert(!threads || ecs_os_api.thread_new, ECS_MISSING_OS_API, "thread_new");
    ecs_assert(!threads || ecs_os_api.thread_join, ECS_MISSING_OS_API, "thread_join");
    ecs_assert(!threads || ecs_os_api.mutex_new, ECS_MISSING_OS_API, "mutex_new");
    ecs_assert(!threads || ecs_os_api.mutex_free, ECS_MISSING_OS_API, "mutex_free");
    ecs_assert(!threads || ecs_os_api.mutex_lock, ECS_MISSING_OS_API, "mutex_lock");
    ecs_assert(!threads || ecs_os_api.mutex_unlock, ECS_MISSING_OS_API, "mutex_unlock");
    ecs_assert(!threads || ecs_os_api.cond_new, ECS_MISSING_OS_API, "cond_new");
    ecs_assert(!threads || ecs_os_api.cond_free, ECS_MISSING_OS_API, "cond_free");
    ecs_assert(!threads || ecs_os_api.cond_wait, ECS_MISSING_OS_API, "cond_wait");
    ecs_assert(!threads || ecs_os_api.cond_signal, ECS_MISSING_OS_API, "cond_signal");
    ecs_assert(!threads || ecs_os_api.cond_broadcast, ECS_MISSING_OS_API, "cond_broadcast");

    int32_t thread_count = ecs_vector_count(world->workers);

    if (!world->arg_threads && thread_count != threads) {
        /* Stop existing threads */
        if (ecs_vector_count(world->workers)) {
            ecs_stop_threads(world);
            ecs_os_cond_free(world->worker_cond);
            ecs_os_cond_free(world->sync_cond);
            ecs_os_mutex_free(world->sync_mutex);
        }

        /* Start threads if number of threads > 1 */
        if (threads > 1) {
            world->worker_cond = ecs_os_cond_new();
            world->sync_cond = ecs_os_cond_new();
            world->sync_mutex = ecs_os_mutex_new();
            world->stage_count = 2 + threads;
            start_workers(world, threads);
        }

        /* Iterate tables, make sure the ecs_data_t arrays are large enough */
        ecs_sparse_each(world->stage.tables, ecs_table_t, table, {
            ecs_table_get_data(world, table);
        });
    }
}

#endif
