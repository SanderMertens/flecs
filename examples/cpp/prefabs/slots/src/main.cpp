#include <slots.h>
#include <iostream>

// Slots can be combined with prefab hierarchies to make it easier to access
// the child entities created for an instance.
//
// To create a slot, the SlotOf relationship is added to the child of a prefab,
// with as relationship target the prefab for which to register the slot. When
// the prefab is instantiated, each slot will be added as a relationship pair
// to the instance that looks like this:
//   (PrefabChild, InstanceChild)
//
// For a SpaceShip prefab and an Engine child, that pair would look like this:
//   (SpaceShip.Engine, Instance.Engine)
//
// To get the entity for a slot, an application can use the regular functions
// to inspect relationships and relationship targets (see code).
//
// Slots can be added to any level of a prefab hierarchy, as long as it is above
// (a parent of) the slot itself. When the prefab tree is instantiated, the 
// slots are added to the entities that correspond with the prefab children.
//
// Without slots, an application would have to rely on manually looking up 
// entities by name to get access to the instantiated children, like what the
// hierarchy example does.

int main() {
    flecs::world ecs;

    // Create the same prefab hierarchy as from the hierarchy example, but now
    // with the SlotOf relationship.
    flecs::entity SpaceShip = ecs.prefab("SpaceShip");
        flecs::entity Engine = ecs.prefab("Engine")
            .child_of(SpaceShip)
            .slot_of(SpaceShip);

        flecs::entity Cockpit = ecs.prefab("Cockpit")
            .child_of(SpaceShip)
            .slot_of(SpaceShip);

    // Add an additional child to the Cockpit prefab to demonstrate how 
    // slots can be different from the parent. This slot could have been
    // added to the Cockpit prefab, but instead we register it on the top
    // level SpaceShip prefab.
    flecs::entity PilotSeat = ecs.prefab("PilotSeat")
        .child_of(Cockpit)
        .slot_of(SpaceShip);

    // Create a prefab instance.
    flecs::entity inst = ecs.entity("my_spaceship").is_a(SpaceShip);

    // Get the instantiated entities for the prefab slots
    flecs::entity inst_engine = inst.target(Engine);
    flecs::entity inst_cockpit = inst.target(Cockpit);
    flecs::entity inst_seat = inst.target(PilotSeat);
    
    std::cout << "instance engine:  " << inst_engine.path() << "\n";
    std::cout << "instance cockpit: " << inst_cockpit.path() << "\n";
    std::cout << "instance seat:    " << inst_seat.path() << "\n";

    // Output:
    //  instance engine:  ::my_spaceship::Engine
    //  instance cockpit: ::my_spaceship::Cockpit
    //  instance seat:    ::my_spaceship::Cockpit::PilotSeat
}
