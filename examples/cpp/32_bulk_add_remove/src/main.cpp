#include <bulk_add_remove.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

struct Mass {
    double value;
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create 3 entities with Position and Mass
    ecs.entity().add<Position>().add<Mass>();
    ecs.entity().add<Position>().add<Mass>();
    ecs.entity().add<Position>().add<Mass>();

    // There will be 3 entities with Position and 0 with Velocity
    std::cout << "There are " << ecs.count<Position>() << " entities with Position" << std::endl;
    std::cout << "There are " << ecs.count<Velocity>() << " entities with Velocity" << std::endl;

    auto Position_filter = flecs::filter(ecs).include<Position>();
    auto Velocity_filter = flecs::filter(ecs).include<Velocity>();
    auto PV_filter = flecs::filter(ecs).include<Position>().include<Velocity>();

    // Bulk add Velocity to all entities with Position
    std::cout << std::endl << "-- Bulk add Velocity --" << std::endl << std::endl;
    ecs.add<Velocity>( Position_filter );

    std::cout << "There are " 
        << ecs.count( PV_filter ) 
        << " entities with Position, Velocity" 
        << std::endl;

    // Bulk remove Position from all entities with Velocity
    std::cout << std::endl << "-- Bulk remove Position --" << std::endl << std::endl;
    ecs.remove<Position>( Velocity_filter );

    // There will be 0 entities with Position and 3 with Velocity
    std::cout << "There are " << ecs.count<Position>() << " entities with Position" << std::endl;
    std::cout << "There are " << ecs.count<Velocity>() << " entities with Velocity" << std::endl;

    // There are three entities with Velocity, Mass
    std::cout << "There are " << ecs.count<Mass>() << " entities with Mass" << std::endl;

    // Bulk remove Mass from all entities (no filter)
    std::cout << std::endl << "-- Bulk remove Mass --" << std::endl << std::endl;
    ecs.remove<Mass>();

    std::cout << "There are " << ecs.count<Mass>() << " entities with Mass" << std::endl;
}
