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
    auto sun = ecs.entity("Sun")
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

        auto earth = ecs.entity("Earth")
            .child_of(sun)
            .add<Position, World>()
            .set<Position, Local>({3, 3});

            ecs.entity("Moon")
                .child_of(earth)
                .add<Position, World>()
                .set<Position, Local>({0.1, 0.1});

    // Create a hierarchical query to compute the global position from the
    // local position and the parent position.
    auto q = ecs.query_builder<const Position, const Position, Position>()
        // Make sure to select the correct world/local positions
        .arg(1).obj<Local>()
        .arg(2).obj<World>()
        .arg(3).obj<World>()

        // Extend the 2nd query argument to select it from the parent
        .arg(2)
            // Get from the parent, in breadth-first order (cascade)
            .set(flecs::Parent | flecs::Cascade)
            // Make term component optional so we also match the root (sun)
            .oper(flecs::Optional)
        // Finalize the query
        .build();

    // Do the transform
    q.iter([](flecs::iter& it, 
        const Position *p, const Position *p_parent, Position *p_out) 
    {
        for (auto i : it) {
            p_out[i].x = p[i].x;
            p_out[i].y = p[i].y;
            if (p_parent) {
                p_out[i].x += p_parent->x;
                p_out[i].y += p_parent->y;
            }
        }
    });

    // Print world positions
    ecs.each([](flecs::entity e, flecs::pair<Position, World> p) {
        std::cout << e.name() << ": {" << p->x << ", " << p->y << "}\n";
    });
}
