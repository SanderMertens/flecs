#include <helloworld.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    // Create the world, pass arguments for overriding the number of threads,fps
    // or for starting the admin dashboard (see flecs.h for details).
    flecs::world ecs(argc, argv);

    // Create a new empty entity. Entity names are optional.
    auto e = ecs.entity("MyEntity");

    // Set the Position component on the entity.
    e.set<Position>({5, 10});

    // Print the Position component
    const Position *ptr = e.get<Position>();
    std::cout << "Position: " 
              << ptr->x << ", " 
              << ptr->y << std::endl << std::endl;

    // Set both Position and Velocity
    e.set([](Position& p, Velocity& v) {
        p = {10, 20};
        v = {1, 2};
    });

    // Print both Position and Velocity
    e.get([](const Position& p, const Velocity& v) {
        std::cout << "Position: " << p.x << ", " << p.y << std::endl;
        std::cout << "Velocity: " << v.x << ", " << v.y << std::endl;
    });

    return 0;
}
