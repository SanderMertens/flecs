#include <exclusive_relations.h>
#include <iostream>

// Type for Platoon relationship
struct Platoon { };

int main(int, char*[]) {
    flecs::world ecs;

    // Register Platoon as exclusive relationship. This ensures that an entity
    // can only belong to a single Platoon.
    ecs.component<Platoon>()
        .add(flecs::Exclusive);

    // Create two platoons
    auto platoon_1 = ecs.entity();
    auto platoon_2 = ecs.entity();

    // Create a unit
    auto unit = ecs.entity();

    // Add unit to platoon 1
    unit.add<Platoon>(platoon_1);

    // Log platoon of unit
    std::cout << "Unit in platoon 1: " << 
        (unit.has<Platoon>(platoon_1) ? "true" : "false") << "\n";
    std::cout << "Unit in platoon 2: " << 
        (unit.has<Platoon>(platoon_2) ? "true" : "false") << "\n\n";

    // Add unit to platoon 2. Because Platoon is an exclusive relationship, this
    // both removes (Platoon, platoon_1) and adds (Platoon, platoon_2) in a
    // single operation.
    unit.add<Platoon>(platoon_2);

    std::cout << "Unit in platoon 1: " << 
        (unit.has<Platoon>(platoon_1) ? "true" : "false") << "\n";
    std::cout << "Unit in platoon 2: " << 
        (unit.has<Platoon>(platoon_2) ? "true" : "false") << "\n";

    // Output:
    //  Unit in platoon 1: true
    //  Unit in platoon 2: false
    //  
    //  Unit in platoon 1: false
    //  Unit in platoon 2: true
}
