#include <custom_runner.h>
#include <iostream>

// Systems can be created with a custom run function that takes control over the
// entire iteration. By default, a system is invoked once per matched table, 
// which means the function can be called multiple times per frame. In some
// cases that's inconvenient, like when a system has things it needs to do only
// once per frame. For these use cases, the run callback can be used which is
// called once per frame per system.

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto s = ecs.system<Position, const Velocity>()
        // The run function has a signature that accepts a C iterator. By 
        // forwarding the iterator to it->callback, the each function of the
        // system is invoked.
        .run([](flecs::iter_t *it) {
            std::cout << "Move begin\n";

            // Walk over the iterator, forward to the system callback
            while (ecs_iter_next(it)) {
                it->callback(it);
            }

            std::cout << "Move end\n";
        })
        .each([](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
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
    //  Move begin
    //  e1: {11, 22}
    //  e2: {13, 24}
    //  Move end
}
