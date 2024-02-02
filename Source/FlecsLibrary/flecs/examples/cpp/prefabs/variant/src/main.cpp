#include <variant.h>
#include <iostream>

/* Prefabs can inherit from each other, which creates prefab variants. With
 * variants applications can reuse a commmon set of components and specialize it
 * by adding or overriding components on the variant. */

struct Attack { 
    double value; 
};

struct Defense { 
    double value; 
};

struct FreightCapacity { 
    double value; 
};

struct ImpulseSpeed { 
    double value; 
};

struct Position { 
    double x;
    double y;
};

int main() {
    flecs::world ecs;

    // Create a base prefab for SpaceShips.
    flecs::entity SpaceShip = ecs.prefab("SpaceShip")
        .set<ImpulseSpeed>({ 50 })
        .set<Defense>({ 25 });

        // Create a Freighter variant which inherits from SpaceShip
        flecs::entity Freighter = ecs.prefab("Freighter")
            .is_a(SpaceShip)
            .set<FreightCapacity>({ 100 })
            .set<Defense>({ 50 });

            // Create a MammotFreighter variant which inherits from Freighter
            flecs::entity MammothFreighter = ecs.prefab("MammothFreighter")
                .is_a(Freighter)
                .set<FreightCapacity>({ 500 });

        // Create a Frigate variant which inherits from SpaceShip
        ecs.prefab("Frigate")
            .is_a(SpaceShip)
            .set<Attack>({ 100 })
            .set<Defense>({ 75 })
            .set<ImpulseSpeed>({ 125 });

    // Create an instance of the MammothFreighter. This entity will inherit the
    // ImpulseSpeed from SpaceShip, Defense from Freighter and FreightCapacity
    // from MammothFreighter.
    flecs::entity inst = ecs.entity("my_freighter").is_a(MammothFreighter);

    // Add a private Position component.
    inst.set<Position>({ 10, 20});

    // Instances can override inherited components to give them a private copy
    // of the component. This freighter got an armor upgrade:
    inst.set<Defense>({ 100 });

    // Queries can match components from multiple levels of inheritance
    ecs.each([](flecs::entity e, 
        Position& p, ImpulseSpeed& s, Defense& d, FreightCapacity& c) {
            std::cout << e.name() << ":\n";
            std::cout << " - position: " << p.x << ", " << p.y << "\n";
            std::cout << " - impulse speed: " << s.value << "\n";
            std::cout << " - defense: " << d.value << "\n";
            std::cout << " - capacity: " << c.value << "\n";
        });

    // Output:
    //   my_freighter:
    //    - position: 10, 20
    //    - impulse speed: 50
    //    - defense: 100
    //    - capacity: 500
}
