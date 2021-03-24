#include <move_system.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
            p.x += v.x;
            p.y += v.y;

            std::cout << "Moved " << e.name() << " to {" <<
                p.x << ", " << p.y << "}" << std::endl;
        });

    ecs.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    ecs.set_target_fps(1);

    std::cout << "Application move_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
