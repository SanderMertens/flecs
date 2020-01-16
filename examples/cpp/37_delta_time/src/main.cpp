#include <delta_time.h>
#include <iostream>

/* Component types */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    flecs::system<Position, Velocity>(world)
        .each([](flecs::entity e, Position& p, Velocity& v) {
            // Use delta_time to update the entity proportionally to the amount 
            // of time that has passed inbetween frames. The delta_time value is
            // the same for each system when computing a frame.
            //
            // The delta_time value obtained through the entity comes from the
            // world and is the value passed into world.progress or, if no value
            // is provided, the value that flecs measured automatically.
            p.x += v.x * e.delta_time();
            p.y += v.y * e.delta_time();

            std::cout << "Moved " << e.name() << " to {" <<
                p.x << ", " << p.y << "}" << std::endl;
        });

    flecs::entity(world, "MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    // Set target FPS for main loop to 60 frames per second. The specified FPS
    // is not a guarantee, which is why applications should use delta_time to
    // progress a simulation, which contains the actual time passed.
    world.set_target_fps(60);

    std::cout << "Application delta_time is running, press CTRL-C to exit..." 
              << std::endl;

    // Run systems. When an application does not provide an explicit value for
    // delta_time flecs will measure the delta_time automatically inbetween 
    // frames.
    //
    // For deterministic simulations, applications will want to pass in a fixed
    // value that does not rely on the system clock.
    while (world.progress()) { }
}
