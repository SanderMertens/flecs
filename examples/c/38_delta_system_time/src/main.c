#include <delta_system_time.h>

void Period05(ecs_iter_t *it) {
    printf("t = 0.5, elapsed time = %f\n", (double)it->delta_system_time);
}

void Period10(ecs_iter_t *it) {
    printf("t = 1.0, elapsed time = %f\n", (double)it->delta_system_time);
}

void Period20(ecs_iter_t *it) {
    printf("t = 2.0, elapsed time = %f\n", (double)it->delta_system_time);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Create three systems with different periods, and print for each the
     * delta_system_time.
     *
     * For systems with a period, the regular delta_time is not as useful as it
     * provides the time passed since the last frame. The delta_system_time
     * addresses this by keeping track of the time elapsed since the last time
     * the system was invoked.
     *
     * This value should approximate the specified period, but may differ due to
     * aliassing issues (the main loop still ticks at its own frequency) or due
     * to variability introduced by operating system scheduling / system clock.
     */

    ECS_SYSTEM(world, Period05, EcsOnUpdate, 0);
    ECS_SYSTEM(world, Period10, EcsOnUpdate, 0);
    ECS_SYSTEM(world, Period20, EcsOnUpdate, 0);

    ecs_set_interval(world, Period05, 0.5);
    ecs_set_interval(world, Period10, 1.0);
    ecs_set_interval(world, Period20, 2.0);

    ecs_set_target_fps(world, 60);

    printf("Application delta_system_time is running, press CTRL-C to exit...\n");

    while ( ecs_progress(world, 0));

    return ecs_fini(world);
}
