#include <slots.h>
#include <iostream>

// Prefab hierarchies that use the non-fragmenting Parent storage automatically
// let applications resolve an instantiated child from its prefab child. This
// provides slot-like lookup without adding a relationship to either entity.

int main() {
    flecs::world ecs;

    // Create a prefab hierarchy with non-fragmenting Parent storage.
    flecs::entity SpaceShip = ecs.prefab("SpaceShip");
        flecs::entity Engine = ecs.prefab(flecs::Parent{SpaceShip}, "Engine");

        flecs::entity Cockpit = ecs.prefab(
            flecs::Parent{SpaceShip}, "Cockpit");

    flecs::entity PilotSeat = ecs.prefab(
        flecs::Parent{Cockpit}, "PilotSeat");

    // Create a prefab instance.
    flecs::entity inst = ecs.entity("my_spaceship").is_a(SpaceShip);

    // Get the instantiated children from their prefab children. A nested child
    // is resolved from the corresponding instance parent.
    flecs::entity inst_engine = inst.target(Engine);
    flecs::entity inst_cockpit = inst.target(Cockpit);
    flecs::entity inst_seat = inst_cockpit.target(PilotSeat);
    
    std::cout << std::boolalpha;
    std::cout << "instance engine found:  " << inst_engine.is_valid() << "\n";
    std::cout << "instance cockpit found: " << inst_cockpit.is_valid() << "\n";
    std::cout << "instance seat found:    " << inst_seat.is_valid() << "\n";

    // Output:
    //  instance engine found:  true
    //  instance cockpit found: true
    //  instance seat found:    true
}
