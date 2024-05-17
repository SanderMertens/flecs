#include <override.h>
#include <iostream>

// When an entity is instantiated from a prefab, components are by default 
// copied from the prefab to the instance. This behavior can be customized with
// the OnInstantiate trait, which has three options:
// 
// - Override (copy to instance)
// - Inherit (inherit from prefab)
// - DontInherit (don't copy or inherit)
//
// When a component is inheritable, it can be overridden manually by adding the
// component to the instance, which also copies the value from the prefab 
// component. Additionally, when creating a prefab it is possible to flag a 
// component as "auto override", which can change the behavior for a specific 
// prefab from "inherit" to "override".
//
// This example shows how these different features can be used.

struct Attack {
    double value;
};

struct Defense {
    double value;
};

struct Damage {
    double value;
};

int main() {
    flecs::world ecs;

    // Change the instantiation behavior for Attack and Defense to inherit.
    ecs.component<Attack>().add(flecs::OnInstantiate, flecs::Inherit);
    ecs.component<Defense>().add(flecs::OnInstantiate, flecs::Inherit);

    // Attack and Damage are properties that can be shared across many 
    // spaceships. This saves memory, and speeds up prefab creation as we don't
    // have to copy the values of Attack and Defense to private components.
    flecs::entity SpaceShip = ecs.prefab("SpaceShip")
        .set<Attack>({ 75 })
        .set<Defense>({ 100 })
        .set<Damage>({ 50 });

    // Create a prefab instance.
    flecs::entity inst = ecs.entity("my_spaceship").is_a(SpaceShip);

    // The entity will now have a private copy of the Damage component, but not
    // of the Attack and Defense components. We can see this when we look at the
    // type of the instance:
    std::cout << inst.type().str() << "\n";

    // Even though Attack was not automatically overridden, we can always 
    // override it manually afterwards by adding it:
    inst.add<Attack>();

    // The Attack component now shows up in the entity type:
    std::cout << inst.type().str() << "\n";

    // We can get all components on the instance, regardless of whether they
    // are overridden or not. Note that the overridden components (Attack and
    // Damage) are initialized with the values from the prefab component:
    const Attack *a = inst.get<Attack>();
    const Defense *d = inst.get<Defense>();
    const Damage *dmg = inst.get<Damage>();

    std::cout << "attack: " << a->value << "\n";
    std::cout << "defense: " << d->value << "\n";
    std::cout << "damage: " << dmg->value << "\n";

    // Output:
    //  Damage, (Identifier,Name), (IsA,SpaceShip)
    //  Attack, Damage, (Identifier,Name), (IsA,SpaceShip)
    //  attack: 75
    //  defense: 100
    //  damage: 50
}
