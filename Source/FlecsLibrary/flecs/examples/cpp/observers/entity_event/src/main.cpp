#include <iostream>
#include <entity_event.h>

// Entity events are events that are emitted and observed for a specific entity.
// They are a thin wrapper around regular observers, which match against queries
// instead of single entities. While they work similarly under the hood, entity
// events provide a much simpler API.
//
// An entity event only needs two pieces of data:
// - The entity on which to emit the event
// - The event to emit

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

    // Emit the Click event
    widget.emit<Click>();

    // Emit the Resize event
    widget.emit<Resize>({100, 200});

    // Output
    //   clicked!
    //   clicked on ::MyWidget!
    //   resized to {100, 200}!
}
