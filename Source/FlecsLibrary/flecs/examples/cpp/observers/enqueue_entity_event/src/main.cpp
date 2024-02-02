#include <enqueue_entity_event.h>
#include <iostream>

// This is the same example as entity_event, but instead of emit() the example
// uses the enqueue() method. Whereas emit() invokes observers synchronously,
// enqueue() adds the event to the command queue, which delays invoking 
// observers until the next time the command queue is flushed.

// An event without payload
struct Click { };

// An event with payload
struct Resize {
    double width, height;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create a widget entity
    flecs::entity widget = ecs.entity("MyWidget");

    // Observe the Click event on the widget entity.
    widget.observe<Click>([]() {
        std::cout << "clicked!\n";
    });

    // Observers can have an entity argument that holds the event source.
    // This allows the same function to be reused for different entities.
    widget.observe<Click>([](flecs::entity src) {
        std::cout << "clicked on " << src.path() << "!\n";
    });

    // Observe the Resize event. Events with payload are passed as arguments
    // to the observer callback.
    widget.observe([](Resize& p) {
        std::cout << "resized to {" 
                  << p.width << ", " << p.height << "}!\n"; 
    });

    // We can only call enqueue events while the world is deferred mode.
    ecs.defer_begin();

    // Emit the Click event
    widget.enqueue<Click>();

    // Emit the Resize event
    widget.enqueue<Resize>({100, 200});

    std::cout << "Events enqueued!\n";

    // Flushes the queue, and invokes the observer
    ecs.defer_end();

    // Output
    //   Events enqueued!
    //   clicked!
    //   clicked on ::MyWidget!
    //   resized to {100, 200}!
}
