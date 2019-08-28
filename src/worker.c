#include "flecs_private.h"

const ecs_vector_params_t thread_arr_params = {
    .element_size = sizeof(ecs_thread_t)
};

const ecs_vector_params_t job_arr_params = {
    .element_size = sizeof(ecs_job_t)
};

/** Worker thread code. Processes a job for one system */
static
void* ecs_worker(void *arg) {
    ecs_thread_t *thread = arg;
    ecs_world_t *world = thread->world;
    uint32_t i;

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
    uint32_t thread_count = ecs_vector_count(world->worker_threads) - 1;
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
    uint32_t thread_count = ecs_vector_count(world->worker_threads) - 1;

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

    ecs_thread_t *buffer = ecs_vector_first(world->worker_threads);
    uint32_t i, count = ecs_vector_count(world->worker_threads);
    for (i = 1; i < count; i ++) {
        ecs_os_thread_join(buffer[i].thread);
        ecs_stage_deinit(world, buffer[i].stage);
    }

    ecs_vector_free(world->worker_threads);
    ecs_vector_free(world->worker_stages);
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

    world->worker_threads = ecs_vector_new(&thread_arr_params, threads);
    world->worker_stages = ecs_vector_new(&stage_arr_params, threads);

    uint32_t i;
    for (i = 0; i < threads; i ++) {
        ecs_thread_t *thread =
            ecs_vector_add(&world->worker_threads, &thread_arr_params);

        thread->magic = ECS_THREAD_MAGIC;
        thread->world = world;
        thread->thread = 0;
        thread->job_count = 0;
        thread->index = i;

        thread->stage = ecs_vector_add(&world->worker_stages, &stage_arr_params);
        ecs_stage_init(world, thread->stage);

        if (i != 0) {
            thread->thread = ecs_os_thread_new(ecs_worker, thread);
            ecs_assert(thread->thread != 0, ECS_THREAD_ERROR, NULL);
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
        ecs_vector_free(system_data->jobs);
    }

    system_data->jobs = ecs_vector_new(&job_arr_params, thread_count);

    uint32_t i;
    for (i = 0; i < thread_count; i ++) {
        ecs_vector_add(&system_data->jobs, &job_arr_params);
    }
}


/* -- Private functions -- */

/** Create a job per available thread for system */
void ecs_schedule_jobs(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    uint32_t thread_count = ecs_vector_count(world->worker_threads);
    uint32_t total_rows = 0;
    bool is_task = false;

    ecs_matched_table_t *tables = ecs_vector_first(system_data->tables);
    uint32_t i, count = ecs_vector_count(system_data->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = tables[i].table;
        if (table) {
            total_rows += ecs_vector_count(table->columns[0].data);
        } else {
            is_task = true;
        }

        /* Task systems should only have one matched table which is empty */
        ecs_assert(!is_task || !i, ECS_INTERNAL_ERROR, NULL);
    }

    if (is_task) {
        thread_count = 1; /* Tasks are always scheduled to the main thread */
    } else if (total_rows < thread_count) {
        thread_count = total_rows;
    }

    if (ecs_vector_count(system_data->jobs) != thread_count) {
        create_jobs(system_data, thread_count);
    }

    float rows_per_thread = (float)total_rows / (float)thread_count;
    float residual = 0;
    int32_t rows_per_thread_i = rows_per_thread;

    uint32_t start_index = 0;

    ecs_job_t *job = NULL;

    for (i = 0; i < thread_count; i ++) {
        job = ecs_vector_get(system_data->jobs, &job_arr_params, i);
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

    if (i && residual >= 0.9) {
        job->limit ++;
    }
}

/** Assign jobs to worker threads, signal workers */
void ecs_prepare_jobs(
    ecs_world_t *world,
    ecs_entity_t system)
{
    EcsColSystem *system_data = ecs_get_ptr(world, system, EcsColSystem);
    ecs_vector_t *threads = world->worker_threads;
    ecs_vector_t *jobs = system_data->jobs;
    uint32_t i;

    uint32_t thread_count = ecs_vector_count(jobs);

    for (i = 0; i < thread_count; i++) {
        ecs_thread_t *thr = ecs_vector_get(threads, &thread_arr_params, i);
        uint32_t job_count = thr->job_count;
        thr->jobs[job_count] = ecs_vector_get(jobs, &job_arr_params, i);
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
    ecs_thread_t *thread = ecs_vector_first(world->worker_threads);
    ecs_job_t **jobs = thread->jobs;
    uint32_t i, job_count = thread->job_count;

    for (i = 0; i < job_count; i ++) {
        ecs_run_w_filter(
            (ecs_world_t*)thread, jobs[i]->system, world->delta_time, jobs[i]->offset, jobs[i]->limit, 0, NULL);
    }
    thread->job_count = 0;

    if (world->jobs_finished != ecs_vector_count(world->worker_threads) - 1) {
        wait_for_jobs(world);
    }
}


/* -- Public functions -- */

void ecs_set_threads(
    ecs_world_t *world,
    uint32_t threads)
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

    if (!world->arg_threads) {
        if (ecs_vector_count(world->worker_threads)) {
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
