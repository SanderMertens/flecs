/* 
 * Tests generated from: docs/ComponentTraits.md
 * Contains 1 test(s) from documentation code snippets
 */

#include <flecs.h>
#include <iostream>
#include <cassert>

// (No hardcoded component or world setup is injected)

// Test function declarations
void CanToggle_trait_01();

// Test function implementations

void CanToggle_trait_01() {
    flecs::world world;
    world.component<Position>().add(flecs::CanToggle);

    flecs::entity e = world.entity().set(Position{10, 20});

    e.disable<Position>(); // Disable component
    assert(!e.enabled<Position>());

    e.enable<Position>(); // Enable component
    assert(e.enabled<Position>());
}

// Test runner for this file
void run_componenttraits_tests() {
    std::cout << "Running componenttraits tests..." << std::endl;
    
    CanToggle_trait_01();
    
    std::cout << "All componenttraits tests passed!" << std::endl;
}
