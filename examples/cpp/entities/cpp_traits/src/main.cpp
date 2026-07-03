#include <cpp_traits.h>
#include <iostream>

// The dont_fragment member registers the component with the DontFragment
// trait, which stores the component in a sparse storage that doesn't fragment
// tables (see the ComponentTraits manual).
struct Position {
    static constexpr bool dont_fragment = true;
    double x, y;
};

// The on_instantiate member registers the component with an OnInstantiate
// policy. Mass is registered with (OnInstantiate, Inherit), which makes
// instances share the component with their prefab instead of copying it.
struct Mass {
    static constexpr auto on_instantiate = flecs::on_instantiate::inherit;
    double value;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Traits are applied on component registration (implicit or explicit).
    flecs::entity pos = ecs.component<Position>();
    std::cout << "Position has DontFragment: "
        << pos.has(flecs::DontFragment) << "\n";

    // Because Mass is inheritable, instances share it with the prefab.
    flecs::entity base = ecs.prefab("Spaceship").set<Mass>({100});
    flecs::entity inst = ecs.entity("MySpaceship").is_a(base);
    std::cout << "MySpaceship owns Mass: " << inst.owns<Mass>() << "\n";
    std::cout << "MySpaceship mass: " << inst.get<Mass>().value << "\n";

    inst.set<Position>({10, 20});

    // When all components in a query have the dont_fragment trait, the query
    // returns a flecs::sparse_query, which iterates the sparse component
    // storages directly and is faster than a regular query.
    flecs::sparse_query<Position> q = ecs.query<Position>();

    q.each([](flecs::entity e, Position& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}" << "\n";
    });

    // The same goes for world.each() which also uses the optimized iteration
    // mode when all components have the dont_fragment trait:

    ecs.each([](flecs::entity e, Position& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}" << "\n";
    });

    // Output
    //  Position has DontFragment: 1
    //  Mass has (OnInstantiate, Inherit): 1
    //  MySpaceship owns Mass: 0
    //  MySpaceship mass: 100
    //  MySpaceship: {10, 20}
    //  MySpaceship: {10, 20}
}
