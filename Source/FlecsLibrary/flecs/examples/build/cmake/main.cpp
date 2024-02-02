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
    flecs::world ecs(argc, argv);

    ecs.system<Position, const Velocity>()
        .iter([](const flecs::iter& it, 
            Position *p, 
            const Velocity *v) 
        {    
            for (auto row : it) {
                p[row].x += v[row].x;
                p[row].y += v[row].y;

                std::cout << "Moved " << it.entity(row).name() << " to {" <<
                    p[row].x << ", " << p[row].y << "}" << std::endl;
            }
        });

    ecs.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    ecs.set_target_fps(1);

    std::cout << "Application move_system is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
