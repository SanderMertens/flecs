#include <bulk_add_remove.h>
#include <iostream>

/* Component types */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    // Create 3 entities with Position
    flecs::entity_range(world, 3)
        .add<Position>();

    // There will be 3 entities with Position and 0 with Velocity
    std::cout << "There are " << world.count<Position>() << " entities with Position" << std::endl;
    std::cout << "There are " << world.count<Velocity>() << " entities with Position" << std::endl;

    auto Position_filter = flecs::filter(world).include<Position>();
    auto Velocity_filter = flecs::filter(world).include<Velocity>();
    auto PV_filter = flecs::filter(world).include<Position>().include<Velocity>();

    // Bulk add Velocity to all entities with Position
    std::cout << std::endl << "-- Bulk add Velocity --" << std::endl << std::endl;
    world.add<Velocity>( Position_filter );

    std::cout << "There are " 
        << world.count( PV_filter ) 
        << " entities with Position, Velocity" 
        << std::endl;

    // Bulk remove Position from all entities with Velocity
    std::cout << std::endl << "-- Bulk remove Position --" << std::endl << std::endl;
    world.remove<Position>( Velocity_filter );

    // There will be 0 entities with Position and 3 with Velocity
    std::cout << "There are " << world.count<Position>() << " entities with Position" << std::endl;
    std::cout << "There are " << world.count<Velocity>() << " entities with Velocity" << std::endl;    
}
