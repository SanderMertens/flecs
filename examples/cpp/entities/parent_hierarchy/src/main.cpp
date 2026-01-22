#include <parent_hierarchy.h>
#include <iostream>

// ChildOf hierarchies are optimized for scenarios with few parents that have
// lots of children. Parent hierarchies are optimized for scenarios with lots of
// parents that have a small number of children. See the documentation for more
// information:
//   https://www.flecs.dev/flecs/md_docs_2HierarchiesManual.html#hierarchy-storage
//
// This example is the same as the hierarchy example, but instead of using
// ChildOf hierarchies it uses Parent hierarchies.

struct Position {
    double x, y;
};

struct Star { };
struct Planet { };
struct Moon { };

void iterate_tree(flecs::entity e, Position p_parent = {0, 0}) {
    // Print hierarchical name of entity & the entity type
    std::cout << e.path() << " [" << e.type().str() << "]\n";

    // Get entity position
    const Position& p = e.get<Position>();

    // Calculate actual position
    Position p_actual = {p.x + p_parent.x, p.y + p_parent.y};
    std::cout << "{" << p_actual.x << ", " << p_actual.y << "}\n\n";

    // Iterate children recursively
    e.children([&](flecs::entity child) {
        iterate_tree(child, p_actual);
    });
}

int main(int, char *[]) {
    flecs::world ecs;

    // Create a simple hierarchy.
    // Hierarchies use ECS relationships and the builtin flecs::ChildOf relationship to
    // create entities as children of other entities.

    flecs::entity sun = ecs.entity("Sun")
        .add<Star>()
        .set<Position>({1, 1});

        ecs.entity(flecs::Parent{sun}, "Mercury")
            .add<Planet>()
            .set<Position>({1, 1});

        ecs.entity(flecs::Parent{sun}, "Venus")
            .add<Planet>()
            .set<Position>({2, 2});

        flecs::entity earth = ecs.entity(flecs::Parent{sun}, "Earth")
            .add<Planet>()
            .set<Position>({3, 3});

            flecs::entity moon = ecs.entity(flecs::Parent{earth}, "Moon")
                .add<Moon>()
                .set<Position>({0.1, 0.1});

    // Is the Moon a child of Earth?
    std::cout << "Child of Earth? " << moon.has(flecs::ChildOf, earth) << "\n\n";

    // Lookup the moon by name
    flecs::entity e = ecs.lookup("Sun::Earth::Moon");
    std::cout << "Moon found: " << e.path() << "\n\n";

    // Do a depth-first walk of the tree
    iterate_tree(sun);

    // Output:
    //   Child of Earth? 1
    //   
    //   Moon found: ::Sun::Earth::Moon
    //   
    //   ::Sun [Star, Position, (Identifier,Name)]
    //   {1, 1}
    //   
    //   ::Sun::Mercury [Parent, Position, Planet, (Identifier,Name), (ParentDepth,@1)]
    //   {2, 2}
    //   
    //   ::Sun::Venus [Parent, Position, Planet, (Identifier,Name), (ParentDepth,@1)]
    //   {3, 3}
    //   
    //   ::Sun::Earth [Parent, Position, Planet, (Identifier,Name), (ParentDepth,@1)]
    //   {4, 4}
    //   
    //   ::Sun::Earth::Moon [Parent, Position, Moon, (Identifier,Name), (ParentDepth,@2)]
    //   {4.1, 4.1}
}
