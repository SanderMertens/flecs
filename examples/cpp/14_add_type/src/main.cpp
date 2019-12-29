#include <add_type.h>
#include <iostream>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

    auto Movable = flecs::type(world, "Movable")
        .add<Position>()
        .add<Velocity>();

    auto e = flecs::entity(world)
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
