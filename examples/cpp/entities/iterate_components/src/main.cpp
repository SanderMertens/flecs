#include <iterate_components.h>
#include <iostream>

// Ordinary components
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

// Tag
struct Human { };

// Two tags used to create a pair
struct Eats { };
struct Apples { };

void iterate_components(flecs::entity e) {
    // 1. The easiest way to print the components is to use type::str
    std::cout << e.type().str() << "\n\n";

    // 2. To get individual component ids, use entity::each
    std::int32_t i = 0;
    e.each([&](flecs::id id) {
        std::cout << i++ << ": " << id.str() << "\n";
    });
    std::cout << "\n";

    // 3. we can also inspect and print the ids in our own way. This is a
    // bit more complicated as we need to handle the edge cases of what can be
    // encoded in an id, but provides the most flexibility.
    i = 0;
    e.each([&](flecs::id id) {
        std::cout << i++ << ": ";

        flecs::id role = id.role();
        if (role) {
            std::cout << "role: " << role.role_str() << ", ";
        }

        if (id.is_pair()) {
            // If id is a pair, extract & print both parts of the pair
            flecs::entity rel = id.first();
            flecs::entity target = id.second();
            std::cout << "rel: " << rel.name() << ", " << "obj: " << target.name();
        } else {
            // Id contains a regular entity. Strip role before printing.
            flecs::entity comp = id.entity();
            std::cout << "entity: " << comp.name();
        }

        std::cout << "\n";
    });

    std::cout << "\n\n";
}

int main(int, char *[]) {
    flecs::world ecs;

    auto bob = ecs.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 1})
        .add<Human>()
        .add<Eats, Apples>();

    std::cout << "Bob's components:\n";
    iterate_components(bob);

    // We can use the same function to iterate the components of a component
    std::cout << "Position's components:\n";
    iterate_components(ecs.component<Position>());
}

// Output:

// Bob's components:
// Position, Velocity, Human, (Eats,Apples)
// 
// 0: Position
// 1: Velocity
// 2: Human
// 3: (Eats,Apples)
// 
// 0: entity: Position
// 1: entity: Velocity
// 2: entity: Human
// 3: role: PAIR, rel: Eats, obj: Apples
// 
// 
// Position's components:
// Component, (Identifier,Name), (Identifier,Symbol), (OnDelete,Panic)
// 
// 0: Component
// 1: (Identifier,Name)
// 2: (Identifier,Symbol)
// 3: (OnDelete,Panic)
// 
// 0: entity: Component
// 1: role: PAIR, rel: Identifier, obj: Name
// 2: role: PAIR, rel: Identifier, obj: Symbol
// 3: role: PAIR, rel: OnDelete, obj: Panic
