#include <custom_threads.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

/* Type passed as thread arg */
typedef struct {
    ecs_world_t *stage;
    ecs_entity_t system;
    ecs_query_t *query;
} thread_ctx_t;

/* Forward component declarations */
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

/* Implement a simple move system */
void Move(ecs_iter_t *it) {
    /* Get the two columns from the system signature */
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;

        printf("Stage %d: %u\n", 
            ecs_get_stage_id(it->world), 
            (uint32_t)it->entities[i]);
    }
}

/* Dummy thread function */
void* thread_func(void *arg) {
    thread_ctx_t *ctx = arg;
    ecs_world_t *world = ctx->stage;

    int32_t stage_id = ecs_get_stage_id(world);
    int32_t stage_count = ecs_get_stage_count(world);

    /* The worker thread's main loop would start here, but because this is an
     * example without threads and we call the thread explicitly from the main 
     * loop don't actually loop */

    // while (!ecs_should_quit(world)) {

        /* This is where the thread would wait for the mainthread to signal that
         * it is safe to start processing */
        // wait_for_staging_begin()

        /* Run system with the ecs_run_worker function. This will automatically
         * subdivide entities evenly across threads. If the regular ecs_run
         * function is used the system will evaluate all matching entities. */
        ecs_run_worker(world, ctx->system, stage_id, stage_count, 0, NULL);

        /* Evaluate query with the ecs_query_next_worker function. This will,
         * just as with the system, automatically subdivide entities across
         * threads. When the regular ecs_query_next function is used, all
         * entities will be evaluated. */
        ecs_iter_t it = ecs_query_iter(ctx->query);

        while (ecs_query_next_worker(&it, stage_id, stage_count)) {
            Position *p = ecs_term(&it, Position, 1);
            Velocity *v = ecs_term(&it, Velocity, 2);

            for (int i = 0; i < it.count; i ++) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
            }            
        }

        /* So far, neither the system nor the query enqueued any operations.
         * Enqueueing commands is done simply by calling the regular API with
         * the provided staging context. The command will be executed when the
         * stage is merged. */

        /* The following operations will be enqueued */
        ecs_entity_t e = ecs_new(world, 0);
        ecs_set(world, e, Position, {10, 20});

        Velocity *v = ecs_get_mut(world, e, Velocity, NULL);
        v->x = 1;
        v->y = 2;
        ecs_modified(world, e, Velocity);

        /* This is be where the thread would signal to the main thread that it
         * is done processing. The main thread will wait for all threads to
         * finish, after which the merge will take place. */
        // signal_main()
    // }

    return NULL;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    /* Create both a system and a query so both can be demonstrated */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");

    /* Create a bunch of entities */
    for (int i = 0; i < 5; i ++) {
        ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});
        ecs_set(world, e, Velocity, {1, 1});
    }

    /* Create two stages. This lets two threads concurrently mutate the world */
    ecs_set_stages(world, 2);

    /* Get the stage objects. These are passed to the threads and should be 
     * provided to API calls instead of the world. The stage objects provide a 
     * transparent mechanism for passing a thread context to regular flecs API 
     * functions without requiring a dedicated API for enqueuing commands. */
    ecs_world_t *stage_1 = ecs_get_stage(world, 0);
    ecs_world_t *stage_2 = ecs_get_stage(world, 1);

    thread_ctx_t ctx_1 = {
        .stage = stage_1,
        .system = Move,
        .query = q
    };

    thread_ctx_t ctx_2 = {
        .stage = stage_2,
        .system = Move,
        .query = q
    };

    /* Start threads (replace this with an OS thread create function) */
    // thread_new(thread_func, ctx_1);
    // thread_new(thread_func, ctx_2);
    (void)ctx_1; (void)ctx_2;

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);    

    /* Main loop. Instead of ecs_progress, use these functions to control when a 
     * frame starts/stops, and when merging should occur. */

    while ( !ecs_should_quit(world)) {
        printf("\nFrame begin\n");

        /* Start frame. This does time measurements, and if an application has 
         * set a target FPS, this function will sleep for as long as necessary 
         * to ensure the application does not exceed the set FPS. The number
         * passed to the function is delta_time, which should be set to the
         * same value as what would ordinarly be passed to ecs_progress */
        ecs_frame_begin(world, 0);

        /* Start staging. After this call it is safe for threads to start adding 
         * commands to the queue. This also flags the moment at which it is no 
         * longer valid to do mutations on the regular world object. When an 
         * application would attempt to, for example, do 
         * "ecs_add(world, e, Position)" after staging_begin, an assert would be
         * thrown. It is still safe to call functions that do not mutate the 
         * world, such as "ecs_has". 
         *
         * If the world were mutated while in staged mode by accident (say,
         * because asserts are not enabled) threads that are asynchronously
         * accessing the world may run into undefined behavior as a result of
         * data races.          
         */
        ecs_staging_begin(world);

        /* This is where the main thread would signal the threads to start
         * processing (and enqueueing commands) */
        // signal_threads();

        /* Stub running the threads */
        thread_func(&ctx_1);
        thread_func(&ctx_2);

        /* This is where the main thread would wait for the threads to finish
         * processing */
        // wait_for_threads();

        /* End staging. This will automatically merge data from all threads. 
         * Applications will be able to disable automatic merging, and even take
         * control over which stages are merged and which stages aren't. It is 
         * valid to begin/end staging multiple times per frame. This is commonly 
         * required when an application has multiple sync points. */
        ecs_staging_end(world);

        /* Finish frame. This executes post-frame actions */
        ecs_frame_end(world);
    }

    /* Cleanup */
    return ecs_fini(world);
}
