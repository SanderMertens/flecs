#include <simple_threads.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    // Flecs does not have an operating system abstraction for threading. To use
    // threading, an application must first provide threading functions by
    // setting the appropriate functions in the OS API. See the examples in the
    // os_api folder for how this can be achieved. 
    //
    // To run the example, add the posix OS API example as a dependency, and
    // uncomment this line. Without doing this, the example will not run. */

    // posix_set_os_api();

    flecs::world ecs(argc, argv);

    ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
            p.x += v.x;
            p.y += v.y;

            // Print the id of the current stage. This allows us to see which
            // entities are processed by which thread.
            std::cout << "Stage " << e.world().get_stage_id() << ": " 
                << e.id() << std::endl;
        });

    // Create a bunch of entities
    for (int i = 0; i < 10; i ++) {
        ecs.entity()
            .set<Position>({0, 0})
            .set<Velocity>({1, 1});
    }

    // Set target FPS for main loop to 1 frame per second
    ecs.set_target_fps(1);

    // Set number of threads to 2. This will run all systems on all threads, and
    // divide entities equally between the systems.
    ecs.set_threads(2);

    // Run systems.
    while (ecs.progress()) { }

    return 0;
}
