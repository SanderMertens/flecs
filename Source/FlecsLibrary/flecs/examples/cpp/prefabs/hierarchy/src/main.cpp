#include <hierarchy.h>
#include <iostream>

// When a prefab has children, they are instantiated for an instance when the
// IsA relationship to the prefab is added.

int main() {
    flecs::world ecs;

    // Create a prefab hierarchy.
    flecs::entity SpaceShip = ecs.prefab("SpaceShip");
        ecs.prefab("Engine").child_of(SpaceShip);
        ecs.prefab("Cockpit").child_of(SpaceShip);

    // Instantiate the prefab. This also creates an Engine and Cockpit child
    // for the instance.
    flecs::entity inst = ecs.entity("my_spaceship").is_a(SpaceShip);
    flecs::entity inst_engine = inst.lookup("Engine");
    flecs::entity inst_cockpit = inst.lookup("Cockpit");

    std::cout << "instance engine:  " << inst_engine.path() << "\n";
    std::cout << "instance cockpit: " << inst_cockpit.path() << "\n";

    // Output:
    //   instance engine:  ::my_spaceship::Engine
    //   instance cockpit: ::my_spaceship::Cockpit
}
