#include <prefab.h>
#include <iostream>

struct Attack { double value; };
struct Defense { double value; };
struct FreightCapacity { double value; };
struct ImpulseSpeed { double value; };
struct HasFTL { };

struct Position { double x = 0; double y = 0; };

int main(int, char *[]) {
    flecs::world ecs;

    // Create a prefab hierarchy. Prefabs are entities that by default are 
    // ignored by queries.

    auto spaceship = ecs.prefab("Spaceship")
        // Add components to prefab entity as usual
        .set<ImpulseSpeed>({50})
        .set<Defense>({50})

        // By default components in an inheritance hierarchy are shared between
        // entities. The override function ensures that instances have a private
        // copy of the component.
        .override<Position>();

        auto freighter = ecs.prefab("Freighter")
            // Short for .add(flecs::IsA, spaceship). This ensures the entity
            // inherits all components from spaceship.
            .is_a(spaceship)
            .add<HasFTL>()
            .set<FreightCapacity>({100})
            .set<Defense>({100});

            auto mammoth_freighter = ecs.prefab("MammothFreighter")
                .is_a(freighter)
                .set<FreightCapacity>({500})
                .set<Defense>({300});

        ecs.prefab("Frigate")
            .is_a(spaceship)
            .add<HasFTL>()
            .set<Attack>({100})
            .set<Defense>({75})
            .set<ImpulseSpeed>({125});

    // Create a regular entity from a prefab.
    // The instance will have a private copy of the Position component, because
    // of the override in the spaceship entity. All other components are shared.
    auto inst = ecs.entity("my_mammoth_freighter")
        .is_a(mammoth_freighter);

    // Inspect the type of the entity. This outputs:
    //    Position,(Identifier,Name),(IsA,MammothFreighter)
    std::cout << "Instance type: [" << inst.type().str() << "]\n";

    // Even though the instance doesn't have a private copy of ImpulseSpeed, we
    // can still get it using the regular API (outputs 50)
    const ImpulseSpeed *ptr = inst.get<ImpulseSpeed>();
    std::cout << "Impulse speed: " << ptr->value << "\n";

    // Prefab components can be iterated just like regular components:
    ecs.each([](flecs::entity e, ImpulseSpeed& is, Position& p) {
        p.x += is.value;
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n"; 
    });
}
