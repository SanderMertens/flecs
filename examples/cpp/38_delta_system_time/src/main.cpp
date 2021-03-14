#include <delta_system_time.h>
#include <iostream>

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create three systems with different periods, and print for each the
    // delta_system_time.
    //
    // For systems with a period, the regular delta_time is not as useful as it
    // provides the time passed since the last frame. The delta_system_time
    // addresses this by keeping track of the time elapsed since the last time
    // the system was invoked.
    //
    // This value should approximate the specified period, but may differ due to
    // aliassing issues (the main loop still ticks at its own frequency) or due
    // to variability introduced by operating system scheduling / system clock.

    ecs.system<>()
        .interval(0.5)
        .iter([](flecs::iter& it) {
            std::cout << "t = 0.5, time elampsed = " << it.delta_system_time() 
                      << std::endl;
        });

    ecs.system<>()
        .interval(1.0)
        .iter([](flecs::iter& it) {
            std::cout << "t = 1.0, time elampsed = " << it.delta_system_time() 
                      << std::endl;
        });

    ecs.system<>()
        .interval(2.0)
        .iter([](flecs::iter& it) {
            std::cout << "t = 2.0, time elampsed = " << it.delta_system_time() 
                      << std::endl;
        });

    ecs.set_target_fps(60);

    while (ecs.progress()) { }
}
