#include <entity_event.h>
#include <iostream>

// Entity events are events that are emitted and observed for a specific entity.
// They are a thin wrapper around regular observers, which match against queries
// instead of single entities. While they work similarly under the hood, entity
// events provide a much simpler API.
//
// An entity event only needs two pieces of data:
// - The entity on which to emit the event
// - The event to emit

// The event to emit.
struct OnClick { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create a widget entity
    flecs::entity widget = ecs.entity("MyWidget");

    // Observe the OnClick event on the widget entity
    widget.observe<OnClick>([](flecs::iter& it) {
        // The event source can be obtained with it.src(1). This allows the same
        // event function to be used for different entities.
        std::cout << "clicked on " << it.src(1).path() << "!\n";
    });

    // Emit the OnClick event for the widget
    widget.emit<OnClick>();

    // Output
    //   clicked on ::MyWidget!
}
