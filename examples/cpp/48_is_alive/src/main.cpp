#include <is_alive.h>
#include <iostream>

int main() {
    flecs::world ecs;

    // Create new entity, will be alive
    auto e1 = ecs.entity();
    std::cout << "e1 alive: " << e1.is_alive() << std::endl;
    
    // Entity will not be alive after deleting
    e1.destruct();
    std::cout << "e1 alive: " << e1.is_alive() << std::endl;

    // Create new entity, will return same id but new generation
    auto e2 = ecs.entity();

    // e2 is alive, but e1 is not because the generation increased
    std::cout << "e1 alive: " << e1.is_alive() << std::endl;
    std::cout << "e2 alive: " << e2.is_alive() << std::endl;
}
