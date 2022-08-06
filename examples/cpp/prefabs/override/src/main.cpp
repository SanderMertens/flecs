#include <override.h>
#include <iostream>

// Overriding makes it possible for a prefab instance to obtain a private copy
// of an inherited component. To override a component the regular add operation
// is used. The overridden component will be initialized with the value of the
// inherited component.
//
// In some cases a prefab instance should always have a private copy of an 
// inherited component. This can be achieved with an auto override which can be
// added to a prefab. Components with an auto override are automatically 
// overridden when the prefab is instantiated.

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

    // Attack and Damage are properties that can be shared across many 
    // spaceships. This saves memory, and speeds up prefab creation as we don't
    // have to copy the values of Attack and Defense to private components.
    flecs::entity SpaceShip = ecs.entity("SpaceShip")
        .set<Attack>({ 75 })
        .set<Defense>({ 100 });

    // Damage is a property that is private to a spaceship, so add an auto
    // override for it. This ensures that each prefab instance will have a
    // private copy of the component.
    SpaceShip.set_override<Damage>({ 0 });

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
    //  damage: 0
}
