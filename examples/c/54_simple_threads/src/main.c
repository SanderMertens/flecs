#include <simple_threads.h>

typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;

        /* Print the id of the current stage. This allows us to see which
         * entities are processed by which thread. */
        printf("Stage %d: %u\n", 
            ecs_get_stage_id(it->world), 
            (uint32_t)it->entities[i]);
    }
}

int main(int argc, char *argv[]) {
    /* Flecs does not have an operating system abstraction for threading. To use
     * threading, an application must first provide threading functions by
     * setting the appropriate functions in the OS API. See the examples in the
     * os_api folder for how this can be achieved. 
     *
     * To run the example, add the posix OS API example as a dependency, and
     * uncomment this line. Without doing this, the example will not run. */

    // posix_set_os_api();

    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    /* Create a bunch of entities */
    for (int i = 0; i < 10; i ++) {
        ecs_entity_t e = ecs_set(world, 0, Position, {0, 0});
        ecs_set(world, e, Velocity, {1, 1});
    }

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    /* Set number of threads to 2. This will run all systems on all threads, and
     * divide entities equally between the systems. */
    ecs_set_threads(world, 2);

    /* Run systems. */
    while (ecs_progress(world, 0)) { }

    /* Cleanup */
    return ecs_fini(world);
}
