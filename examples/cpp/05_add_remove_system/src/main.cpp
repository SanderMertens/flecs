#include <add_remove_system.h>
#include <iostream>

struct Position {
    float x;
    float y;
};

/* This system will be called when Position is added */
void AddPosition(flecs::entity e,  Position& p) {
    p.x = 10;
    p.y = 20;
    std::cout << "Position added" << std::endl;
}

/* This system will be called when Position is removed */
void RemovePosition(flecs::entity e, Position& p) {
    std::cout << "Position removed -> {" 
            << p.x << ", " << p.y << "}" << std::endl;
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");

    /* Register two systems that are executed when Position is added or removed */
    flecs::system<Position>(world).kind(flecs::OnAdd).each(AddPosition);
    flecs::system<Position>(world).kind(flecs::OnRemove).each(RemovePosition);

    flecs::entity e(world);

    /* Add Position. This will trigger the OnAdd system */
    e.add<Position>();

    /* Remove Position. This will trigger the OnRemove system. */
    e.remove<Position>();

    /* Add Position again. This will retrigger the OnAdd system */
    e.add<Position>();

    /* Add Position again. This does not retrigger the OnAdd system since the
     * entity already has Position */
    e.add<Position>();
}
