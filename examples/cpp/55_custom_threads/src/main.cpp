#include <custom_threads.h>
#include <iostream>

// Component types
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

// Type passed as thread arg
struct thread_ctx_t{
    flecs::world& stage;
    flecs::system<Position, const Velocity> system;
    flecs::query<Position, const Velocity> query;
};

// Dummy thread function
void* thread_func(void *arg) {
    thread_ctx_t *ctx = static_cast<thread_ctx_t*>(arg);
    auto& ecs = ctx->stage;
    auto system = ctx->system;
    auto query = ctx->query;

    int32_t stage_id = ecs.get_stage_id();
    int32_t stage_count = ecs.get_stage_count();

    // The worker thread's main loop would start here, but because this is an
    // example without threads and we call the thread explicitly from the main 
    // loop don't actually loop
    // while (!ecs_should_quit(world)) {

        // This is where the thread would wait for the mainthread to signal that
        // it is safe to start processing.
        // wait_for_staging_begin()

        // Run system with the ecs_run_worker function. This will automatically
        // subdivide entities evenly across threads. If the regular ecs_run
        // function is used the system will evaluate all matching entities.
        system.run_worker(stage_id, stage_count).stage(ecs);

        // Evaluate query with the ecs_query_next_worker function. This will,
        // just as with the system, automatically subdivide entities across
        // threads. When the regular ecs_query_next function is used, all
        // entities will be evaluated.
        query.each_worker(stage_id, stage_count, 
            [](flecs::entity, Position& p, const Velocity& v) {
                p.x += v.x;
                p.y += v.y;
            });

        // So far, neither the system nor the query enqueued any operations.
        // Enqueueing commands is done simply by calling the regular API with
        // the provided staging context. The command will be executed when the
        // stage is merged.

        /* The following operations will be enqueued */
        auto e = ecs.entity()
            .set<Position>({10, 20});

        Velocity *v = e.get_mut<Velocity>();
        v->x = 1;
        v->y = 2;
        e.modified<Velocity>();

        // This is be where the thread would signal to the main thread that it
        // is done processing. The main thread will wait for all threads to
        // finish, after which the merge will take place.
        // signal_main()
    // }

    return NULL;
}

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create both a system and a query so both can be demonstrated
    auto move_system = ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
            p.x += v.x;
            p.y += v.y;
            std::cout << "Stage " << e.world().get_stage_id() << ": " 
                << e.id() << std::endl;
        });

    auto q = ecs.query<Position, const Velocity>();

    // Create a bunch of entities
    for (int i = 0; i < 5; i ++) {
        ecs.entity()
            .set<Position>({0, 0})
            .set<Velocity>({1, 1});
    }

    // Create two stages. This lets two threads concurrently mutate the world
    ecs.set_stages(2);

    // Get the stage objects. These are passed to the threads and should be 
    // provided to API calls instead of the world. The stage objects provide a 
    // transparent mechanism for passing a thread context to regular flecs API 
    // functions without requiring a dedicated API for enqueuing commands.
    flecs::world stage_1 = ecs.get_stage(0);
    flecs::world stage_2 = ecs.get_stage(1);

    thread_ctx_t ctx_1 = { stage_1, move_system, q };
    thread_ctx_t ctx_2 = { stage_2, move_system, q };

    // Start threads (replace this with an OS thread create function)
    // thread_new(thread_func, ctx_1);
    // thread_new(thread_func, ctx_2);
    (void)ctx_1; (void)ctx_2;

    // Set target FPS for main loop to 1 frame per second
    ecs.set_target_fps(1);

    // Main loop. Instead of ecs_progress, use these functions to control when a 
    // frame starts/stops, and when merging should occur.

    while ( !ecs.should_quit()) {
        std::cout << std::endl << "Frame begin" << std::endl;

        // Start frame. This does time measurements, and if an application has 
        // set a target FPS, this function will sleep for as long as necessary 
        // to ensure the application does not exceed the set FPS. The number
        // passed to the function is delta_time, which should be set to the
        // same value as what would ordinarly be passed to ecs_progress
        ecs.frame_begin();

        // Start staging. After this call it is safe for threads to start adding 
        // commands to the queue. This also flags the moment at which it is no 
        // longer valid to do mutations on the regular world object. When an 
        // application would attempt to, for example, do 
        //
        // "ecs_add(world, e, Position)" after staging_begin, an assert would be
        // thrown. It is still safe to call functions that do not mutate the 
        // world, such as "ecs_has".
        //
        // If the world were mutated while in staged mode by accident (say,
        // because asserts are not enabled) threads that are asynchronously
        // accessing the world may run into undefined behavior as a result of
        // data races. 
        ecs.staging_begin();

        // This is where the main thread would signal the threads to start
        // processing (and enqueueing commands) */
        // signal_threads();

        // Stub running the threads
        thread_func(&ctx_1);
        thread_func(&ctx_2);

        // This is where the main thread would wait for the threads to finish
        // processing
        // wait_for_threads();

        // End staging. This will automatically merge data from all threads. 
        // Applications will be able to disable automatic merging, and even take
        // control over which stages are merged and which stages aren't. It is 
        // valid to begin/end staging multiple times per frame. This is commonly 
        // required when an application has multiple sync points.
        //
        // After staging_end() writing to a stage is no longer allowed until the
        // next staging_begin(). Doing so may trigger an assert.
        ecs.staging_end();

        // Finish frame. This executes post-frame actions
        ecs.frame_end();
    }
}
