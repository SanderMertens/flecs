#include <is_alive.h>
#include <iostream>

int main() {
    flecs::world ecs;

    // Create new entity, will be alive
    auto e_1 = ecs.entity();
    std::cout << "e_1 alive: " << e_1.is_alive() << std::endl;
    
    // Entity will not be alive after deleting
    e_1.destruct();
    std::cout << "e_1 alive: " << e_1.is_alive() << std::endl;

    // Create new entity, will return same id but new generation
    auto e_2 = ecs.entity();

    // e_2 is alive, but e_1 is not because the generation increased
    std::cout << "e_1 alive: " << e_1.is_alive() << std::endl;
    std::cout << "e_2 alive: " << e_2.is_alive() << std::endl;
}
