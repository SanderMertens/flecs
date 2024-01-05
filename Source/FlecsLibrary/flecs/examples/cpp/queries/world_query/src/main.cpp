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

    // Ad hoc queries are bit slower to iterate than flecs::query, but are 
    // faster to create, and in most cases require no allocations. Under the
    // hood this API uses flecs::filter, which can be used directly for more
    // complex queries.
    ecs.each([](flecs::entity e, Position& p, Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });
}
