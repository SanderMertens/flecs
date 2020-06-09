#include <empty_system_signature.h>

void MyTask(ecs_iter_t *it) {
    printf("MyTask invoked!\n");
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Define a system with an empty signature. Systems that do not match with
     * any entites are invoked once per frame */
    ECS_SYSTEM(world, MyTask, EcsOnUpdate, 0);

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application empty_system_signature is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
