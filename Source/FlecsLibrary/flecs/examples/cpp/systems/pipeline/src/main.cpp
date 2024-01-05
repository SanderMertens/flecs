#include <pipeline.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create a system for moving an entity
    ecs.system<Position, const Velocity>()
        .kind(flecs::OnUpdate) // A phase orders a system in a pipeline
        .each([](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    // Create a system for printing the entity position
    ecs.system<const Position>()
        .kind(flecs::PostUpdate)
        .each([](flecs::entity e, const Position& p) {
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
        });

    // Create a few test entities for a Position, Velocity query
    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity("e2")
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    // Run the default pipeline. This will run all systems ordered by their
    // phase. Systems within the same phase are ran in declaration order. This
    // function is usually called in a loop.
    ecs.progress();
}
