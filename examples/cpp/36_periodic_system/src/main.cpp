#include <periodic_system.h>
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

    ecs.system<Position, Velocity>()
        .each([](Position& p, Velocity& v) {    
            p.x += v.x;
            p.y += v.y;
        });

    /* Create system that is invoked once per second */
    ecs.system<Position>()
        .interval(1.0)
        .each([](flecs::entity e, Position& p) {    
            std::cout << "Position of " << e.name() << " is {" <<
                p.x << ", " << p.y << "}" << std::endl;
        });        

    ecs.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    /* Run all normal systems at 60FPS */
    ecs.set_target_fps(60);

    std::cout << "Application move_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
