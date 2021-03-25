#include <add_type.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    auto Movable = ecs.type("Movable")
        .add<Position>()
        .add<Velocity>();

    auto e = ecs.entity()
        .add(Movable);

    /* Test if entity has the components */
    std::cout << "After adding the type:" << std::endl;
    std::cout << "Has Position? " << e.has<Position>() << std::endl;
    std::cout << "Has Velocity? " << e.has<Velocity>() << std::endl;

    e.remove(Movable);

    std::cout << "After removing the type:" << std::endl;
    std::cout << "Has Position? " << e.has<Position>() << std::endl;
    std::cout << "Has Velocity? " << e.has<Velocity>() << std::endl;
}
