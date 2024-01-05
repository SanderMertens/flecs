#include <two_components.h>
#include <iostream>

// An observer can match multiple components/tags. Only entities that match the
// entire observer filter will be forwarded to the callback. For example, an
// observer for Position,Velocity won't match an entity that only has Position.

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create observer for custom event
    ecs.observer<Position, Velocity>()
        .event(flecs::OnSet)
        .each([](flecs::iter& it, size_t i, Position& p, Velocity& v) {
            std::cout << " - " << it.event().name() << ": " 
                << it.event_id().str() << ": "
                << it.entity(i).name() << ": "
                << "p: {" << p.x << ", " << p.y << "} "
                << "v: {" << v.x << ", " << v.y << "}\n";
        });

    // Create entity, set Position (emits EcsOnSet, does not yet match observer)
    flecs::entity e = ecs.entity("e")
        .set<Position>({10, 20});

    // Set Velocity (emits EcsOnSet, matches observer)
    e.set<Velocity>({1, 2});

    // Output
    //   - OnSet: Velocity: e: p: {10, 20} v: {1, 2}
}
