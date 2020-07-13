#include <helloworld.h>
#include <iostream>

struct Position {
    float x;
    float y;
};

int main(int argc, char *argv[]) {
    // Create the world, pass arguments for overriding the number of threads,fps
    // or for starting the admin dashboard (see flecs.h for details).
    flecs::world world(argc, argv);

    // Register a component with the world. The name argument is mandatory as it
    // ensures a consistent component id across modules and libraries.
    flecs::component<Position>(world, "Position");

    // Create a new empty entity. Entity names are optional.
    auto e = flecs::entity(world, "MyEntity");

    // Set the Position component on the entity.
    e.set<Position>({10, 20});

    // Get the Position component.
    const Position *p = e.get<Position>();

    std::cout << "Position of " << e.name() << " is {" 
              << p->x << ", " << p->y << "}" 
              << std::endl;

    return 0;
}
