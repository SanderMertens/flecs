#include <basics.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create a query for Position, Velocity. Queries are the fastest way to
    // iterate entities as they cache results.
    auto q = ecs.query<Position, const Velocity>();

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

    // Iterate entities matching the query
    q.each([](flecs::entity e, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });

    // Ad hoc queries are bit slower to iterate than queries, but are faster to
    // create as they don't require setting up a cache.
    ecs.each([](flecs::entity e, Position& p, Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });
}
