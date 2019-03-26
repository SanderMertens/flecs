#include "include/private/flecs.h"
#include <assert.h>
#include <math.h>

const ecs_array_params_t thread_arr_params = {
    .element_size = sizeof(ecs_thread_t)
};

const ecs_array_params_t job_arr_params = {
    .element_size = sizeof(ecs_job_t)
};

/** Worker thread code. Processes a job for one system */
static
void* ecs_worker(void *arg) {
    ecs_thread_t *thread = arg;
    ecs_world_t *world = thread->world;
    int i;

    ecs_os_mutex_lock(world->thread_mutex);
    world->threads_running ++;

    while (!world->quit_workers) {
        ecs_os_cond_wait(world->thread_cond, world->thread_mutex);
        if (world->quit_workers) {
            break;
        }

        ecs_job_t **jobs = thread->jobs;
        uint32_t job_count = thread->job_count;
        ecs_os_mutex_unlock(world->thread_mutex);

        for (i = 0; i < job_count; i ++) {
            ecs_run_w_filter(
                (ecs_world_t*)thread, /* magic */
                jobs[i]->system, 
                world->delta_time, 
                jobs[i]->offset, 
                jobs[i]->limit, 
                0, 
                NULL);
        }

        ecs_os_mutex_lock(world->thread_mutex);
        thread->job_count = 0;

        ecs_os_mutex_lock(world->job_mutex);
        world->jobs_finished ++;
        if (world->jobs_finished == world->threads_running) {
            ecs_os_cond_signal(world->job_cond);
        }
        ecs_os_mutex_unlock(world->job_mutex);
    }

    ecs_os_mutex_unlock(world->thread_mutex);

    return NULL;
}

/** Wait until threads have started (busy loop) */
static
void wait_for_threads(
    ecs_world_t *world)
{
    uint32_t thread_count = ecs_array_count(world->worker_threads) - 1;
    bool wait = true;

    do {
        ecs_os_mutex_lock(world->thread_mutex);
        if (world->threads_running == thread_count) {
            wait = false;
        }
        ecs_os_mutex_unlock(world->thread_mutex);
    } while (wait);
}

/** Wait until threads have finished processing their jobs */
static
void wait_for_jobs(
    ecs_world_t *world)
{
    uint32_t thread_count = ecs_array_count(world->worker_threads) - 1;

    ecs_os_mutex_lock(world->job_mutex);
    if (world->jobs_finished != thread_count) {
        do {
            ecs_os_cond_wait(world->job_cond, world->job_mutex);
        } while (world->jobs_finished != thread_count);
    }
    ecs_os_mutex_unlock(world->job_mutex);
}

/** Stop worker threads */
static
void ecs_stop_threads(
    ecs_world_t *world)
{
    ecs_os_mutex_lock(world->thread_mutex);
    world->quit_workers = true;
    ecs_os_cond_broadcast(world->thread_cond);
    ecs_os_mutex_unlock(world->thread_mutex);

    ecs_thread_t *buffer = ecs_array_buffer(world->worker_threads);
    uint32_t i, count = ecs_array_count(world->worker_threads);
    for (i = 1; i < count; i ++) {
        ecs_os_thread_join(buffer[i].thread);
        ecs_stage_deinit(world, buffer[i].stage);
    }

    ecs_array_free(world->worker_threads);
    ecs_array_free(world->worker_stages);
    world->worker_stages = NULL;
    world->worker_threads = NULL;
    world->quit_workers = false;
    world->threads_running = 0;
}

/** Start worker threads, wait until they are running */
void start_threads(
    ecs_world_t *world,
    uint32_t threads)
{
    ecs_assert(world->worker_threads == NULL, ECS_INTERNAL_ERROR, NULL);

    world->worker_threads = ecs_array_new(&thread_arr_params, threads);
    world->worker_stages = ecs_array_new(&stage_arr_params, threads - 1);

    int i;
    for (i = 0; i < threads; i ++) {
        ecs_thread_t *thread =
            ecs_array_add(&world->worker_threads, &thread_arr_params);

        thread->magic = ECS_THREAD_MAGIC;
        thread->world = world;
        thread->thread = 0;
        thread->job_count = 0;

        if (i != 0) {
            thread->stage = ecs_array_add(&world->worker_stages, &stage_arr_params);
            ecs_stage_init(world, thread->stage);
            thread->thread = ecs_os_thread_new(ecs_worker, thread);
            if (!thread->thread) {
                ecs_abort(ECS_THREAD_ERROR, NULL);
            }
        } else {
            thread->stage = NULL;
        }
    }
}

/** Create jobs for system */
static
void create_jobs(
    EcsColSystem *system_data,
    uint32_t thread_count)
{
    if (system_data->jobs) {
        ecs_array_free(system_data->jobs);
    }

    system_data->jobs = ecs_array_new(&job_arr_params, thread_count);

    int i;
    for (i = 0; i < thread_count; i ++) {
        ecs_array_add(&system_data->jobs, &job_arr_params);
    }
}


/* -- Private functions -- */

/** Create a job per available thread for system */
void ecs_schedule_jobs(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    uint32_t thread_count = ecs_array_count(world->worker_threads);
    uint32_t total_rows = 0;

    EcsIter table_it = ecs_array_iter(
        system_data->tables, &system_data->table_params);

    while (ecs_iter_hasnext(&table_it)) {
        uint32_t table_index = *(uint32_t*)ecs_iter_next(&table_it);
        ecs_table_t *table = ecs_array_get(
            world->main_stage.tables, &table_arr_params, table_index);
        total_rows += ecs_array_count(table->columns[0].data);
    }

    if (total_rows < thread_count) {
        thread_count = total_rows;
    }

    if (ecs_array_count(system_data->jobs) != thread_count) {
        create_jobs(system_data, thread_count);
    }

    float rows_per_thread = (float)total_rows / (float)thread_count;
    float residual = 0;
    int32_t rows_per_thread_i = rows_per_thread;

    table_it = ecs_array_iter(system_data->tables, &system_data->table_params);
    uint32_t start_index = 0;

    ecs_job_t *job = NULL;
    uint32_t i;
    for (i = 0; i < thread_count; i ++) {
        job = ecs_array_get(system_data->jobs, &job_arr_params, i);
        int32_t rows_per_job = rows_per_thread_i;
        residual += rows_per_thread - rows_per_job;
        if (residual > 1) {
            rows_per_job ++;
            residual --;
        }

        job->system = system;
        job->system_data = system_data;
        job->offset = start_index;
        job->limit = rows_per_job;

        start_index += rows_per_job;
    }

    if (residual >= 0.9) {
        job->limit ++;
    }
}

/** Assign jobs to worker threads, signal workers */
void ecs_prepare_jobs(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_array_t *threads = world->worker_threads;
    ecs_array_t *jobs = system_data->jobs;
    int i;

    uint32_t thread_count = ecs_array_count(jobs);

    for (i = 0; i < thread_count; i++) {
        ecs_thread_t *thr = ecs_array_get(threads, &thread_arr_params, i);
        uint32_t job_count = thr->job_count;
        thr->jobs[job_count] = ecs_array_get(jobs, &job_arr_params, i);
        thr->job_count = job_count + 1;
    }
}

void ecs_run_jobs(
    ecs_world_t *world)
{
    /* Make sure threads are ready to accept jobs */
    wait_for_threads(world);

    ecs_os_mutex_lock(world->thread_mutex);
    world->jobs_finished = 0;
    ecs_os_cond_broadcast(world->thread_cond);
    ecs_os_mutex_unlock(world->thread_mutex);

    /* Run job for thread 0 in main thread */
    ecs_thread_t *thread = ecs_array_buffer(world->worker_threads);
    ecs_job_t **jobs = thread->jobs;
    uint32_t i, job_count = thread->job_count;

    for (i = 0; i < job_count; i ++) {
        ecs_run_w_filter(
            world, jobs[i]->system, world->delta_time, jobs[i]->offset, jobs[i]->limit, 0, NULL);
    }
    thread->job_count = 0;

    if (world->jobs_finished != ecs_array_count(world->worker_threads) - 1) {
        wait_for_jobs(world);
    }
}


/* -- Public functions -- */

void ecs_set_threads(
    ecs_world_t *world,
    uint32_t threads)
{
    if (!world->arg_threads) {
        if (ecs_array_count(world->worker_threads)) {
            ecs_stop_threads(world);
            ecs_os_cond_free(world->thread_cond);
            ecs_os_mutex_free(world->thread_mutex);
            ecs_os_cond_free(world->job_cond);
            ecs_os_mutex_free(world->job_mutex);
        }

        if (threads > 1) {
            world->thread_cond = ecs_os_cond_new();
            world->thread_mutex = ecs_os_mutex_new();
            world->job_cond = ecs_os_cond_new();
            world->job_mutex = ecs_os_mutex_new();
            start_threads(world, threads);
        }

        world->valid_schedule = false;
    }
}
