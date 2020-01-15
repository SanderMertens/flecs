#include <set_system.h>
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

/* This system will be called when Position is set */
void SetPosition(flecs::entity e,  Position& p) {
    std::cout << "Position set -> {" 
              << p.x << ", " << p.y << "}" << std::endl;
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");

    /* Register two systems that are executed when Position is added or set */
    flecs::system<Position>(world).kind(flecs::OnAdd).each(AddPosition);
    flecs::system<Position>(world).kind(flecs::OnSet).each(SetPosition);

    flecs::entity e(world);

    /* Add Position. Because we have an OnAdd system, flecs assumes a valid 
     * value will be assigned to Position, and therefore the OnSet system is 
     * invoked directly after the OnAdd system. */
    e.add<Position>();

    /* Set Position. This will trigger the OnSet system. */
    e.set<Position>({30, 40});

    /* Set Position again. This will retrigger the OnSet system */
    e.set<Position>({50, 60});
}
