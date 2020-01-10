#include <periodic_system.h>
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
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    flecs::system<Position, Velocity>(world)
        .action([](const flecs::rows& rows, 
            flecs::column<Position> p, 
            flecs::column<Velocity> v) 
        {    
            for (auto row : rows) {
                p[row].x += v[row].x;
                p[row].y += v[row].y;
            }
        });

    /* Create system that is invoked once per second */
    flecs::system<Position>(world)
        .period(1.0)
        .action([](const flecs::rows& rows, 
            flecs::column<Position> p) 
        {    
            for (auto row : rows) {
                std::cout << "Position of " << rows.entity(row).name() << " is {" <<
                    p[row].x << ", " << p[row].y << "}" << std::endl;
            }
        });        

    flecs::entity(world, "MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    /* Run all normal systems at 60FPS */
    world.set_target_fps(60);

    std::cout << "Application move_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
