#include <custom_event.h>
#include <iostream>

// Observers can be used to match custom events. Custom events can be emitted
// using the ecs_emit function. This function is also used by builtin events,
// so builtin and custom events use the same rules for matching with observers.
//
// An event consists out of three pieces of data used to match with observers:
//  - An single event kind (EcsOnAdd, EcsOnRemove, ...)
//  - One or more event ids (Position, Velocity, ...)
//  - A source (either an entity or a table)

struct Position {
    double x, y;
};

// Create tag type to use as event (could also use entity)
struct MyEvent { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create observer for custom event
    ecs.observer<Position>()
        .event<MyEvent>()
        .each([](flecs::iter& it, size_t i, Position&) {
            std::cout << " - " << it.event().name() << ": " 
                << it.event_id().str() << ": "
                << it.entity(i).name() << "\n";
        });

    // The observer filter can be matched against the entity, so make sure it
    // has the Position component before emitting the event. This does not 
    // trigger the observer yet.
    flecs::entity e = ecs.entity("e")
        .set<Position>({10, 20});

    // Emit the custom event
    ecs.event<MyEvent>()
        .id<Position>()
        .entity(e)
        .emit();

    // Output
    //   - MyEvent: Position: e
}
