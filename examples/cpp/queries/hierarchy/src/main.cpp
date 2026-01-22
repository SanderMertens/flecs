#include <hierarchy.h>
#include <iostream>

// Simple transform location with 2D location
struct Transform {
    double x, y;
};

// Create two distinct types for world and local transform
struct WorldTransform : Transform { };
struct LocalTransform : Transform { };

int main(int, char *[]) {
    flecs::world ecs;

    // Whenever we add LocalTransform, also add WorldTransform
    ecs.component<LocalTransform>()
        .add(flecs::With, ecs.component<WorldTransform>());

    // Create a hierarchy. For an explanation see the entities/hierarchy example
    flecs::entity sun = ecs.entity("Sun")
        .set(LocalTransform{1, 1});

        ecs.entity(sun, "Mercury")
            .set(LocalTransform{1, 1});

        ecs.entity(sun, "Venus")
            .set(LocalTransform{2, 2});

        flecs::entity earth = ecs.entity(sun, "Earth")
            .set(LocalTransform{3, 3});

            ecs.entity(earth, "Moon")
                .set(LocalTransform{0.1, 0.1});

    // Create a hierarchical query to compute the global position from the
    // local position and the parent position. The three template arguments are:
    //  - Local position
    //  - World parent position (* = optional, so we match root entities)
    //  - World position
    flecs::query<const LocalTransform, const WorldTransform*, WorldTransform> q = 
        ecs.query_builder<const LocalTransform, const WorldTransform*, WorldTransform>()
            // Select the second query argument (const WorldTransform) from the
            // parent entity with .parent(). The cascade() modifier ensures that
            // the query iterates parents before children.
            .term_at(1).parent().cascade()
            // Finalize the query
            .build();

    // Run the query
    q.each([](
        const LocalTransform& t_local,
        const WorldTransform *t_parent, 
        WorldTransform& t_world) 
    {
        t_world.x = t_local.x;
        t_world.y = t_local.y;
        if (t_parent) {
            t_world.x += t_parent->x;
            t_world.y += t_parent->y;
        }
    });

    // Print world positions
    ecs.each([](flecs::entity e, WorldTransform& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });

    // Output:
    //   Sun: {1, 1}
    //   Mercury: {2, 2}
    //   Venus: {3, 3}
    //   Earth: {4, 4}
    //   Moon: {4.1, 4.1}
}
