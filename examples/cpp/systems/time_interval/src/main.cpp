#include <time_interval.h>
#include <iostream>

// This example shows how to run a system at a specified time interval.

void Tick(flecs::iter& it) {
    std::cout << it.system().name() << "\n";
}

int main(int, char *[]) {
    flecs::world ecs;

    ecs.system("Tick")
        .interval(1.0) // time in seconds
        .iter(Tick);

    ecs.system("FastTick")
        .interval(0.5)
        .iter(Tick);

    // Run the main loop at 60 FPS
    ecs.set_target_fps(60);

    while (ecs.progress()) { }

    // Output
    //  FastTick
    //  Tick
    //  FastTick
    //  FastTick
    //  Tick
    //  FastTick
    //  FastTick
    //  Tick
    //  FastTick
    //  FastTick
    //  ...
}
