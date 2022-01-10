#include <target_fps.h>
#include <stdio.h>

void PrintDeltaTime(ecs_iter_t *it) {
    // Print delta_time. The same value is passed to all systems.
    printf("delta_time: %f\n", (double)it->delta_time);
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create system that prints delta_time. This system doesn't query for any
    // components which means it won't match any entities, but will still be ran
    // once for each call to ecs_progress.
    ECS_SYSTEM(ecs, PrintDeltaTime, EcsOnUpdate, 0);

    // Set target FPS to 1 frame per second
    ecs_set_target_fps(ecs, 1);

    // Run 5 frames
    for (int i = 0; i < 5; i ++) {
        // To make sure the frame doesn't run faster than the specified target
        // FPS ecs_progress will insert a sleep if the measured delta_time is
        // smaller than 1 / target_fps.
        //
        // By default ecs_progress uses the sleep function provided by the OS
        // which is not always very accurate. If more accuracy is required the
        // sleep function of the OS API can be overridden with a custom one.
        //
        // If a value other than 0 is provided to the delta_time argument of
        // ecs_progress, this value will be used to calculate the length of
        // the sleep to insert.
        ecs_progress(ecs, 0);
    }

    return ecs_fini(ecs);
}
