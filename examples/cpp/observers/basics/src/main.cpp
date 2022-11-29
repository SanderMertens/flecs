#include <basics.h>
#include <iostream>

// Observers provide a mechanism for responding to builtin and user defined
// events. They are similar to systems, in that they have the same callback
// signature and use the same query interface to match with entities, but 
// instead of a phase have an event kind.
//
// The most commonly used builtin events are:
//  - flecs::OnAdd: a component was added
//  - flecs::OnRemove: a component was removed
//  - flecs::OnSet: a component's value was changed
//
// The OnAdd and OnRemove events are only thrown when a component is 
// actually added or removed. If an application invokes add and the entity
// already has the component, no event is emitted. Similarly, if an application
// invokes remove for a component the entity doesn't have, no event is 
// emitted. That is in contrast to OnSet, which is invoked each time set 
// or modified is invoked.
//
// Observers are different from component hooks in a number of ways:
//  - A component can only have one hook, whereas it can match many observers
//  - A hook matches one component, whereas observers can match complex queries
//  - Hooks are for add/set/remove events, observers can match custom events.

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create an observer for three events
    ecs.observer<Position>()
        .event(flecs::OnAdd)
        .event(flecs::OnRemove)
        .event(flecs::OnSet)
        .each([](flecs::iter& it, size_t i, Position& p) {
            if (it.event() == flecs::OnAdd) {
                // No assumptions about the component value should be made here. If 
                // a ctor for the component was registered it will be called before
                // the EcsOnAdd event, but a value assigned by set won't be visible.
                std::cout << " - OnAdd: " << it.event_id().str() << ": "
                    << it.entity(i).name() << "\n";
            } else {
                // EcsOnSet or EcsOnRemove event
                std::cout << " - " << it.event().name() << ": " 
                    << it.event_id().str() << ": "
                    << it.entity(i).name() 
                    << ": {" << p.x << ", " << p.y << "}\n";
            }
        });

    // Create entity, set Position (emits EcsOnAdd and EcsOnSet)
    flecs::entity e = ecs.entity("e")
        .set<Position>({10, 20});

    // Remove component (emits EcsOnRemove)
    e.remove<Position>();

    // Remove component again (no event is emitted)
    e.remove<Position>();

    // Output
    //   - OnAdd: Position: e
    //   - OnSet: Position: e: {10, 20}
    //   - OnRemove: Position: e: {10, 20}
}
