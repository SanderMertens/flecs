#include <${id underscore}.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Set target FPS for main loop */
    ecs_set_target_fps(world, 60);

    printf("Application ${id} is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    return ecs_fini(world);
}
