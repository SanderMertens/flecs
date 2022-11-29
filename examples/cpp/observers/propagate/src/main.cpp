#include <propagate.h>
#include <iostream>

// Events are propagated along relationship edges. This means that observers can
// listen for events from a parent or prefab, like triggering when a component
// inherited from a prefab was set.
//
// Event propagation happens automatically when an observer contains a filter
// with the EcsUp flag set (indicating upwards traversal). Observers use the 
// same matching logic as queries: if a query with upwards traversal matches an
// entity, so will an observer.
//
// Events are only propagated along acyclic relationship edges.

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create observer that listens for events from both self and parent
    ecs.observer<Position, Position>()
        .term_at(2).parent() // select 2nd Position from parent
        .event(flecs::OnSet)
        .each([](flecs::iter& it, size_t i, Position& p_self, Position& p_parent) {
            std::cout << " - " << it.event().name() << ": " 
                << it.event_id().str() << ": "
                << it.entity(i).name() << ": "
                << "self: {" << p_self.x << ", " << p_self.y << "}, "
                << "parent: {" << p_parent.x << ", " << p_parent.y << "}\n";
        });

    // Create entity and parent
    flecs::entity p = ecs.entity("p");
    flecs::entity e = ecs.entity("e").child_of(p);

    // Set Position on entity. This doesn't trigger the observer yet, since the
    // parent doesn't have Position yet.
    e.set<Position>({10, 20});

    // Set Position on parent. This event will be propagated and trigger the 
    // observer, as the observer query now matches.
    p.set<Position>({1, 2});

    // Output
    //   - OnSet: Position: e: self: {10, 20}, parent: {1, 2}
}
