#include <delta_time.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world ecs;

    // Create system that prints delta_time. This system doesn't query for any
    // components which means it won't match any entities, but will still be ran
    // once for each call to ecs_progress.
    ecs.system<>()
        .iter([](flecs::iter& it) {
            std::cout << "delta_time: " << it.delta_time() << std::endl;
        });

    // Call progress with 0.0f for the delta_time parameter. This will cause
    // ecs_progress to measure the time passed since the last frame. The
    // delta_time of the first frame is a best guess (16ms).
    ecs.progress();

    // The following calls should print a delta_time of approximately 100ms

    ecs_os_sleep(0, 100 * 1000 * 1000);
    ecs.progress();

    ecs_os_sleep(0, 100 * 1000 * 1000);
    ecs.progress();
}
