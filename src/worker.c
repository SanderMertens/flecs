#include "include/private/reflecs.h"

const EcsVectorParams thread_vec_params = {
    .element_size = sizeof(EcsThread),
    .chunk_count = REFLECS_THREAD_CHUNK_COUNT
};

static
void* ecs_worker(void *arg) {
    EcsThread *thread = arg;
    EcsJob *job = &thread->job;

    pthread_mutex_lock(&thread->job_mutex);
    thread->running = true;

    while (!thread->quit) {
        pthread_cond_wait(&thread->job_cond, &thread->job_mutex);
        if (thread->quit) {
            break;
        }

        ecs_run_job(job);

        pthread_mutex_lock(&job->world->job_mutex);
        job->world->jobs_finished ++;
        pthread_cond_signal(&job->world->job_cond);
        pthread_mutex_unlock(&job->world->job_mutex);
    }

    pthread_mutex_unlock(&thread->job_mutex);

    return NULL;
}

static
void ecs_stop_threads(
    EcsWorld *world)
{
    EcsIter it = ecs_vector_iter(world->worker_threads, &thread_vec_params);
    while (ecs_iter_hasnext(&it)) {
        EcsThread *thread = ecs_iter_next(&it);
        pthread_mutex_lock(&thread->job_mutex);
        thread->quit = true;
        pthread_cond_signal(&thread->job_cond);
        pthread_mutex_unlock(&thread->job_mutex);
        pthread_join(thread->thread, NULL);
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
        thread->thread = 0;
        thread->quit = false;
        thread->running = false;

        if (pthread_cond_init(&thread->job_cond, NULL)) {
            goto error;
        }
        if (pthread_mutex_init(&thread->job_mutex, NULL)) {
            goto error;
        }
        if (pthread_create(&thread->thread, NULL, ecs_worker, thread)) {
            goto error;
        }
    }

    bool running;
    do {
        running = true;
        EcsIter it = ecs_vector_iter(world->worker_threads, &thread_vec_params);
        while (running && ecs_iter_hasnext(&it)) {
            EcsThread *thread = ecs_iter_next(&it);
            pthread_mutex_lock(&thread->job_mutex);
            if (!thread->running) {
                running = false;
            }
            pthread_mutex_unlock(&thread->job_mutex);
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
    job->total_rows = rows_per_thread;

    *table_index_inout = table_index;
    *table_inout = table;

    return EcsOk;
error:
    return EcsError;
}

void ecs_schedule_system(
    EcsWorld *world,
    EcsHandle system)
{
    EcsSystem *system_data = ecs_get(world, system, world->system);
    uint32_t thread_count = ecs_vector_count(world->worker_threads);
    uint32_t total_rows = 0;
    uint32_t table_index = 0;
    EcsTable *table = NULL;

    world->jobs_finished = 0;

    EcsIter table_it = ecs_vector_iter(
        system_data->tables, &system_data->tables_params);

    while (ecs_iter_hasnext(&table_it)) {
        EcsSystemTable *table = ecs_iter_next(&table_it);
        total_rows += ecs_vector_count(table->table->rows);
    }

    int rows_per_thread = total_rows / thread_count;

    EcsIter it = ecs_vector_iter(world->worker_threads, &thread_vec_params);
    table_it = ecs_vector_iter(
        system_data->tables, &system_data->tables_params);

    while (ecs_iter_hasnext(&it)) {
        EcsThread *thread = ecs_iter_next(&it);

        pthread_mutex_lock(&thread->job_mutex);
        thread->job.world = world;
        thread->job.system = system;
        thread->job.system_data = system_data;
        thread->job.finished = false;
        thread->job.chunk = NULL;
        thread->job.chunk_index = 0;
        ecs_schedule_next_job(
            &thread->job, &table_it, &table, &table_index, rows_per_thread);
        pthread_cond_signal(&thread->job_cond);
        pthread_mutex_unlock(&thread->job_mutex);
    }

    pthread_mutex_lock(&world->job_mutex);

    do {
        pthread_cond_wait(&world->job_cond, &world->job_mutex);
    } while (world->jobs_finished != thread_count);

    pthread_mutex_unlock(&world->job_mutex);
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
