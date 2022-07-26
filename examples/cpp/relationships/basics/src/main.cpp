#include <basics.h>
#include <iostream>

// Type used for Eats relationship
struct Eats { };

int main(int, char*[]) {
    flecs::world ecs;

    // Entity used for Grows relationship
    auto grows = ecs.entity("Grows");

    // Relationship objects
    auto apples = ecs.entity("Apples");
    auto pears = ecs.entity("Pears");

    // Create an entity with 3 relationships. Relationships are like regular components,
    // but combine two types/identifiers into an (relationship, object) pair.
    auto bob = ecs.entity("Bob")
        // Pairs can be constructed from a type and entity
        .add<Eats>(apples)
        .add<Eats>(pears)
        // Pairs can also be constructed from two entity ids
        .add(grows, pears);

    // Has can be used with relationships as well
    std::cout << "Bob eats apples? " << bob.has<Eats>(apples) << "\n";

    // Wildcards can be used to match relationships
    std::cout << "Bob grows food? " << bob.has(grows, flecs::Wildcard) << "\n";

    // Print the type of the entity. Should output:
    //   (Identifier,Name),(Eats,Apples),(Eats,Pears),(Grows,Pears)
    std::cout << "Bob's type: [" << bob.type().str() << "]\n";

    // Relationships can be iterated for an entity. This iterates (Eats, *):
    bob.each<Eats>([](flecs::entity second) {
        std::cout << "Bob eats " << second.name() << "\n";
    });

    // Iterate by explicitly providing the pair. This iterates (*, Pears):
    bob.each(flecs::Wildcard, pears, [](flecs::id id) {
        std::cout << "Bob " << id.first().name() << " pears\n";
    });
}
