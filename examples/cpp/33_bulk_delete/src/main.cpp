#include <bulk_delete.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create 3 entities with Position
    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>();

    // Create 3 entities with Position, Velocity
    ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>().add<Velocity>();
    ecs.entity().add<Position>().add<Velocity>();

    // There will be 6 entities with Position and 3 with Velocity
    std::cout << "There are " << ecs.count<Position>() << " entities with Position" << std::endl;
    std::cout << "There are " << ecs.count<Velocity>() << " entities with Velocity" << std::endl;

    auto Velocity_filter = flecs::filter(ecs).include<Velocity>();

    // Bulk delete entities with Velocity
    std::cout << std::endl << "-- Bulk delete entities with Velocity --" << std::endl << std::endl;
    ecs.delete_entities( Velocity_filter );

    // There will be 0 entities with Position and 3 with Velocity
    std::cout << "There are " << ecs.count<Position>() << " entities with Position" << std::endl;
    std::cout << "There are " << ecs.count<Velocity>() << " entities with Velocity" << std::endl;    
}
