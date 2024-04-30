#include <world_query.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

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

    // world::each is a quick way to run simple component queries.
    ecs.each([](flecs::entity e, Position& p, Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });
}
