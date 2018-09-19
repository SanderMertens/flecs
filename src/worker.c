#include "include/private/reflecs.h"

const EcsVectorParams thread_vec_params = {
    .element_size = sizeof(EcsThread),
    .chunk_count = REFLECS_THREAD_CHUNK_COUNT
};

const EcsVectorParams job_vec_params = {
    .element_size = sizeof(EcsJob),
    .chunk_count = REFLECS_THREAD_CHUNK_COUNT
};

static
void* ecs_worker(void *arg) {
    EcsThread *thread = arg;
    EcsWorld *world = thread->world;

    pthread_mutex_lock(&thread->mutex);
    thread->running = true;

    while (!thread->quit) {
        pthread_cond_wait(&thread->cond, &thread->mutex);
        if (thread->quit) {
            break;
        }

        EcsJob *job = thread->job;
        ecs_run_job(world, job);

        pthread_mutex_lock(&world->job_mutex);
        world->jobs_finished ++;
        pthread_cond_signal(&world->job_cond);
        pthread_mutex_unlock(&world->job_mutex);
    }

    pthread_mutex_unlock(&thread->mutex);

    return NULL;
}

static
void ecs_stop_threads(
    EcsWorld *world)
{
    EcsIter it = ecs_vector_iter(world->worker_threads, &thread_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsThread *thread = ecs_iter_next(&it);
        pthread_mutex_lock(&thread->mutex);
        thread->quit = true;
        pthread_cond_signal(&thread->cond);
        pthread_mutex_unlock(&thread->mutex);
        pthread_join(thread->thread, NULL);
        pthread_cond_destroy(&thread->cond);
        pthread_mutex_destroy(&thread->mutex);
    }

    ecs_vector_free(world->worker_threads);
    world->worker_threads = NULL;
    return;
}

static
EcsResult ecs_start_threads(
    EcsWorld *world,
    uint32_t threads)
{
    if (world->worker_threads) {
        return EcsError;
    }

    world->worker_threads = ecs_vector_new(&thread_vec_params);

    int i;
    for (i = 0; i < threads; i ++) {
        EcsThread *thread = ecs_vector_add(world->worker_threads, &thread_vec_params);
        thread->world = world;
        thread->thread = 0;
        thread->quit = false;
        thread->running = false;

        if (pthread_cond_init(&thread->cond, NULL)) {
            goto error;
        }
        if (pthread_mutex_init(&thread->mutex, NULL)) {
            goto error;
        }
        if (pthread_create(&thread->thread, NULL, ecs_worker, thread)) {
            goto error;
        }
    }

    bool running;
    do {
        running = true;
        EcsVectorIter iter_data;
        EcsIter it = _ecs_vector_iter(
            world->worker_threads, &thread_vec_params, &iter_data);
        while (running && ecs_iter_hasnext(&it)) {
            EcsThread *thread = ecs_iter_next(&it);
            pthread_mutex_lock(&thread->mutex);
            if (!thread->running) {
                running = false;
            }
            pthread_mutex_unlock(&thread->mutex);
        }
    } while (!running);

    return EcsOk;
error:
    ecs_stop_threads(world);
    return EcsError;
}

static
EcsResult ecs_schedule_next_job(
    EcsJob *job,
    EcsIter *table_it,
    EcsTable **table_inout,
    uint32_t *table_index_inout,
    uint32_t rows_per_thread)
{
    EcsVectorChunk *chunk = job->chunk;
    uint32_t chunk_index = job->chunk_index;
    uint32_t table_index = *table_index_inout;
    EcsTable *table = *table_inout;

    if (!table) {
        if (!ecs_iter_hasnext(table_it)) {
            goto error;
        }

        EcsSystemTable *systable = ecs_iter_next(table_it);
        table = systable->table;
        chunk = ecs_vector_get_next_chunk(table->rows, NULL);
    } else {
        chunk_index += rows_per_thread;
        uint32_t chunk_count = table->rows_params.chunk_count;
        while (chunk_index > chunk_count) {
            chunk_index -= chunk_count;

            chunk = ecs_vector_get_next_chunk(table->rows, chunk);
            if (!chunk) {
                if (!ecs_iter_hasnext(table_it)) {
                    goto error;
                }

                EcsSystemTable *systable = ecs_iter_next(table_it);
                table = systable->table;
                chunk_count = table->rows_params.chunk_count;
                chunk = ecs_vector_get_next_chunk(table->rows, NULL);
                table_index ++;
            }
        }
    }

    job->chunk = chunk;
    job->chunk_index = chunk_index;
    job->table_index = table_index;
    job->total_rows = rows_per_thread;

    *table_index_inout = table_index;
    *table_inout = table;

    return EcsOk;
error:
    return EcsError;
}

static
void ecs_wait_for_jobs(
    EcsWorld *world)
{
    uint32_t thread_count = ecs_vector_count(world->worker_threads);

    pthread_mutex_lock(&world->job_mutex);

    if (world->jobs_finished != thread_count) {
        do {
            pthread_cond_wait(&world->job_cond, &world->job_mutex);
        } while (world->jobs_finished != thread_count);
    }

    pthread_mutex_unlock(&world->job_mutex);
}

static
void ecs_create_jobs(
    EcsSystem *system_data,
    uint32_t thread_count)
{
    if (system_data->jobs) {
        ecs_vector_free(system_data->jobs);
    }

    system_data->jobs = ecs_vector_new(&job_vec_params);

    int i;
    for (i = 0; i < thread_count; i ++) {
        ecs_vector_add(system_data->jobs, &job_vec_params);
    }
}

void ecs_schedule_jobs(
    EcsWorld *world,
    EcsHandle system)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    uint32_t thread_count = ecs_vector_count(world->worker_threads);
    uint32_t total_rows = 0;
    uint32_t table_index = 0;
    EcsTable *table = NULL;

    if (ecs_vector_count(system_data->jobs) != thread_count) {
        ecs_create_jobs(system_data, thread_count);
    }

    EcsIter table_it = ecs_vector_iter(
        system_data->tables, &system_data->tables_params);

    while (ecs_iter_hasnext(&table_it)) {
        EcsSystemTable *table = ecs_iter_next(&table_it);
        total_rows += ecs_vector_count(table->table->rows);
    }

    int rows_per_thread = total_rows / thread_count;

    EcsIter it = ecs_vector_iter(system_data->jobs, &job_vec_params);
    table_it = ecs_vector_iter(
        system_data->tables, &system_data->tables_params);

    while (ecs_iter_hasnext(&it)) {
        EcsJob *job = ecs_iter_next(&it);
        job->system = system;
        job->system_data = system_data;
        job->chunk = NULL;
        job->chunk_index = 0;
        ecs_schedule_next_job(
            job, &table_it, &table, &table_index, rows_per_thread);
    }

    world->valid_schedule = true;
}

void ecs_run_jobs(
    EcsWorld *world,
    EcsHandle system)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    world->jobs_finished = 0;

    EcsIter job_it = ecs_vector_iter(system_data->jobs, &job_vec_params);
    EcsIter thr_it = ecs_vector_iter(world->worker_threads, &thread_vec_params);

    while (ecs_iter_hasnext(&thr_it)) {
        EcsThread *thread = ecs_iter_next(&thr_it);

        ecs_iter_hasnext(&job_it);
        thread->job = ecs_iter_next(&job_it);

        pthread_mutex_lock(&thread->mutex);
        pthread_cond_signal(&thread->cond);
        pthread_mutex_unlock(&thread->mutex);
    }

    ecs_wait_for_jobs(world);
}

EcsResult ecs_set_threads(
    EcsWorld *world,
    uint32_t threads)
{
    if (ecs_vector_count(world->worker_threads)) {
        ecs_stop_threads(world);
        pthread_cond_destroy(&world->job_cond);
        pthread_mutex_destroy(&world->job_mutex);
    }

    if (threads) {
        pthread_cond_init(&world->job_cond, NULL);
        pthread_mutex_init(&world->job_mutex, NULL);
        if (ecs_start_threads(world, threads) != EcsOk) {
            return EcsError;
        }
    }

    return EcsOk;
}
