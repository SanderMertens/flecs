#include <on_validate.h>
#include <iostream>

// The on_validate hook is invoked when a component is set, before the on_set
// hook and OnSet observers are invoked. When the hook returns false, the
// on_set hook and OnSet observers are not invoked for the entity. This can be
// used to guard application logic from invalid component values.

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ecs.component<Position>()
        // Returns false when the position is outside of the world bounds.
        .on_validate([](flecs::entity, Position& p) {
            return p.x >= 0 && p.x <= 100 && p.y >= 0 && p.y <= 100;
        })
        .on_set([](flecs::entity e, Position& p) {
            std::cout << e.name() << " set to {"
                << p.x << ", " << p.y << "}" << std::endl;
        });

    flecs::entity e = ecs.entity("Entity");

    // Position is inside world bounds, on_set is invoked
    e.set<Position>({50, 50});

    // Position is outside world bounds, on_set is not invoked
    e.set<Position>({150, 50});

    // Output:
    //   Entity set to {50, 50}
}
