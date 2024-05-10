#include <hierarchy.h>
#include <iostream>

struct Position {
    double x, y;
};

// Tags for local/world position
struct Local { };
struct World { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create a hierarchy. For an explanation see the entities/hierarchy example
    flecs::entity sun = ecs.entity("Sun")
        .add<Position, World>()
        .set<Position, Local>({1, 1});

        ecs.entity("Mercury")
            .child_of(sun)
            .add<Position, World>()
            .set<Position, Local>({1, 1});

        ecs.entity("Venus")
            .child_of(sun)
            .add<Position, World>()
            .set<Position, Local>({2, 2});

        flecs::entity earth = ecs.entity("Earth")
            .child_of(sun)
            .add<Position, World>()
            .set<Position, Local>({3, 3});

            ecs.entity("Moon")
                .child_of(earth)
                .add<Position, World>()
                .set<Position, Local>({0.1, 0.1});

    // Create a hierarchical query to compute the global position from the
    // local position and the parent position. The three template arguments are:
    //  - Local position
    //  - Local parent position (* = optional, so we match root entities)
    //  - World position
    flecs::query<const Position, const Position*, Position> q = 
        ecs.query_builder<const Position, const Position*, Position>()
            // Modify terms from template to make sure the query selects the
            // local, world and parent position components.
            .term_at(0).second<Local>()
            .term_at(1).second<World>()
            .term_at(2).second<World>()

            // Extend the 2nd query argument to select it from the parent
            .term_at(1)
                // Get from the parent, in breadth-first order (cascade)
                .parent().cascade()
            // Finalize the query
            .build();

    // Do the transform
    q.each([](const Position& p, const Position *p_parent, Position& p_out) {
        p_out.x = p.x;
        p_out.y = p.y;
        if (p_parent) {
            p_out.x += p_parent->x;
            p_out.y += p_parent->y;
        }
    });

    // Print world positions
    ecs.each([](flecs::entity e, flecs::pair<Position, World> p) {
        std::cout << e.name() << ": {" << p->x << ", " << p->y << "}\n";
    });

    // Output:
    //   Sun: {1, 1}
    //   Mercury: {2, 2}
    //   Venus: {3, 3}
    //   Earth: {4, 4}
    //   Moon: {4.1, 4.1}
}
