#include <basics.h>
#include <iostream>

// Prefabs are entities that can be used as templates for other entities. They
// are created with a builtin Prefab tag, which by default excludes them from
// queries and systems.
//
// Prefab instances are entities that have an IsA relationship to the prefab.
// The IsA relationship causes instances to inherit the components from the 
// prefab. By default all instances for a prefab share its components. 
//
// Inherited components save memory as they only need to be stored once for all
// prefab instances. They also speed up the creation of prefabs, as inherited
// components don't need to be copied to the instances.
//
// To get a private copy of a component, an instance can add it which is called 
// an override. Overrides can be manual (by using add) or automatic (see the
// auto_override example).
//
// If a prefab has children, adding the IsA relationship instantiates the prefab
// children for the instance (see hierarchy example).

struct Defense {
    double value;
};

int main() {
    flecs::world ecs;

    // Create a SpaceShip prefab with a Defense component.
    flecs::entity SpaceShip = ecs.prefab("SpaceShip")
        .set<Defense>({ 50 });

    // Create a prefab instance
    flecs::entity inst = ecs.entity("my_spaceship").is_a(SpaceShip);

    // Because of the IsA relationship, the instance now shares the Defense
    // component with the prefab, and can be retrieved as a regular component:
    const Defense *d_inst = inst.get<Defense>();
    std::cout << "defense: " << d_inst->value << "\n";

    // Because the component is shared, changing the value on the prefab will
    // also change the value for the instance:
    SpaceShip.set<Defense>({ 100 });
    std::cout << "defense after set: " << d_inst->value << "\n";

    // Prefab components can be iterated like regular components:
    ecs.each([](flecs::entity e, Defense& d) {
        std::cout << e.path() << ": " << d.value << "\n";
    });

    // Output:
    //  defense: 50
    //  defense after set: 100
    //  ::my_spaceship: 100
}
