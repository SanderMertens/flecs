#include <yield_existing.h>
#include <iostream>

// Observers can enable a "yield_existing" feature that upon creation of the
// observer produces events for all entities that match the observer query. The
// feature is only implemented for the builtin EcsOnAdd and EcsOnSet events.
//
// Custom events can also implement behavior for yield_existing by adding the
// Iterable component to the event (see EcsIterable for more details).

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create existing entities with Position component
    ecs.entity("e1").set<Position>({10, 20});
    ecs.entity("e2").set<Position>({20, 30});

    // Create an observer for three events
    ecs.observer<Position>()
        .event(flecs::OnSet)
        .yield_existing() // Trigger for existing matching entities
        .each([](flecs::iter& it, size_t i, Position& p) {
            std::cout << " - " << it.event().name() << ": " 
                << it.event_id().str() << ": "
                << it.entity(i).name() 
                << ": {" << p.x << ", " << p.y << "}\n";
        });

    // Output
    //   - OnSet: Position: e1: {10, 20}
    //   - OnSet: Position: e2: {20, 30}
}
