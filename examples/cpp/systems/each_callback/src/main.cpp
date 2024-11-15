#include <each_callback.h>
#include <iostream>

// This example shows how to write a system with the each callback, which
// provides the easiest API for iterating over matched components.

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    flecs::system s = ecs.system<Position, const Velocity>() 
        // Arguments passed to each match components passed to system
        .each([](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
            std::cout << "{" << p.x << ", " << p.y << "}\n";
        });

    // Create a few test entities for a Position, Velocity query
    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity("e2")
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    // This entity will not match as it does not have Position, Velocity
    ecs.entity("e3")
        .set<Position>({10, 20});

    // Run the system
    s.run();

    // Output
    //  {11, 22}
    //  {13, 24}
}
