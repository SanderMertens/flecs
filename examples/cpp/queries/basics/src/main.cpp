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


    // The next lines show the different ways in which a query can be iterated.
    // Note how the 'const' qualifier matches the query template arguments.

    // The each() function iterates each entity individually and accepts an
    // entity argument plus arguments for each query component:
    q.each([](flecs::entity e, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });

    // You can omit the flecs::entity argument if it's not needed:
    q.each([](Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << "{" << p.x << ", " << p.y << "}\n";
    });

    // Each also accepts flecs::iter + index (for the iterated entity) arguemnts
    // currently being iterated. A flecs::iter has lots of information on what
    // is being iterated, which is demonstrated in the "iter" example.
    q.each([](flecs::iter& it, size_t i, Position& p, const Velocity& v) {
        p.x += v.x;
        p.y += v.y;
        std::cout << it.entity(i).name() << ": {" << p.x << ", " << p.y << "}\n";
    });

    // Iter is a bit more verbose, but allows for more control over how entities
    // are iterated as it provides multiple entities in the same callback.
    q.iter([](flecs::iter& it, Position *p, const Velocity *v) {
        for (auto i : it) {
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            std::cout << it.entity(i).name() << 
                ": {" << p[i].x << ", " << p[i].y << "}\n";
        }
    });
}
