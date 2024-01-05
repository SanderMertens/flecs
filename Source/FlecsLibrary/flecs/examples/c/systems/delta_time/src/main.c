#include <delta_time.h>
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

    // Call progress with 0.0f for the delta_time parameter. This will cause
    // ecs_progress to measure the time passed since the last frame. The
    // delta_time of the first frame is a best guess (16ms).
    ecs_progress(ecs, 0.0f);

    // The following calls should print a delta_time of approximately 100ms

    ecs_os_sleep(0, 100 * 1000 * 1000);
    ecs_progress(ecs, 0.0f);

    ecs_os_sleep(0, 100 * 1000 * 1000);
    ecs_progress(ecs, 0.0f);

    return ecs_fini(ecs);
}
