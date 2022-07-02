#include <basics.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Walking { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create an entity with name Bob
    auto bob = ecs.entity("Bob")
        // The set operation finds or creates a component, and sets it.
        // Components are automatically registered with the world.
        .set<Position>({10, 20}) 
        // The add operation adds a component without setting a value. This is
        // useful for tags, or when adding a component with its default value.
        .add<Walking>();

    // Get the value for the Position component
    const Position* ptr = bob.get<Position>();
    std::cout << "{" << ptr->x << ", " << ptr->y << "}" << "\n";

    // Overwrite the value of the Position component
    bob.set<Position>({20, 30});

    // Create another named entity
    auto alice = ecs.entity("Alice")
        .set<Position>({10, 20});

    // Add a tag after entity is created
    alice.add<Walking>();

    // Print all of the components the entity has. This will output:
    //    Position, Walking, (Identifier,Name)
    std::cout << "[" << alice.type().str() << "]" << "\n";

    // Remove tag
    alice.remove<Walking>();

    // Iterate all entities with Position
    ecs.each([](flecs::entity e, Position& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}" << "\n";
    });

    // Output
    //  {10, 20}
    //  [Position, Walking, (Identifier,Name)]
    //  Alice: {10, 20}
    //  Bob: {20, 30}
}
