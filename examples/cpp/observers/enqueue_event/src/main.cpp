#include <enqueue_event.h>
#include <iostream>

// This is the same example as custom_event, but instead of emit() the example
// uses the enqueue() method. Whereas emit() invokes observers synchronously,
// enqueue() adds the event to the command queue, which delays invoking 
// observers until the next time the command queue is flushed.

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

    // The observer query can be matched against the entity, so make sure it
    // has the Position component before emitting the event. This does not 
    // trigger the observer yet.
    flecs::entity e = ecs.entity("e")
        .set<Position>({10, 20});

    // We can only call enqueue events while the world is deferred mode.
    ecs.defer_begin();

    // Emit the custom event
    ecs.event<MyEvent>()
        .id<Position>()
        .entity(e)
        .enqueue();

    std::cout << "Event enqueued!\n";

    // Flushes the queue, and invokes the observer
    ecs.defer_end();

    // Output
    //  Event enqueued!
    //   - MyEvent: Position: e
}
