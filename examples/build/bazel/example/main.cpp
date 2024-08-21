#include "flecs.h"
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    world.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
            p.x += v.x;
            p.y += v.y;

            std::cout << "Moved " << e.name() << " to {" <<
                p.x << ", " << p.y << "}" << std::endl;
        });

    world.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    world.set_target_fps(1);

    std::cout << "Application move_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
