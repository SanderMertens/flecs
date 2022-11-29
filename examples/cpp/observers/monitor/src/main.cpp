#include <monitor.h>
#include <iostream>

// A monitor observer triggers when an entity starts/stop matching the observer
// filter. The observer communicates whether an entity is "entering/leaving" the
// monitor by setting ecs_iter_t::event to EcsOnAdd (for entering) or 
// EcsOnRemove (for leaving).
//
// To specify that an observer is a monitor observer, the EcsMonitor tag must be
// provided as event. No additional event kinds should be provided for a monitor
// observer.

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
        .event(flecs::Monitor) // Monitor entities entering/leaving the query
        .each([](flecs::iter& it, size_t i, Position&, Velocity&) {
            if (it.event() == flecs::OnAdd) {
                std::cout << " - Enter: "
                    << it.event_id().str() << ": "
                    << it.entity(i).name() << "\n";
            } else if (it.event() == flecs::OnRemove) {
                std::cout << " - Leave: "
                    << it.event_id().str() << ": "
                    << it.entity(i).name() << "\n";
            }
        });

    // Create entity
    flecs::entity e = ecs.entity("e");

    // This does not yet trigger the monitor, as the entity does not yet match.
    e.set<Position>({10, 20});

    // This triggers the monitor with EcsOnAdd, as the entity now matches.
    e.set<Velocity>({1, 2});

    // This triggers the monitor with EcsOnRemove, as the entity no longer matches.
    e.remove<Position>();

    // Output
    //   - Enter: Velocity: e
    //   - Leave: Position: e
}
