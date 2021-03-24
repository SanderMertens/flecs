#include <set_system.h>
#include <iostream>

struct Position {
    double x, y;
};

/* This system will be called when Position is added */
void AddPosition(flecs::entity,  Position& p) {
    p.x = 10;
    p.y = 20;
    std::cout << "Position added" << std::endl;
}

/* This system will be called when Position is set */
void SetPosition(flecs::entity,  Position& p) {
    std::cout << "Position set -> {" 
              << p.x << ", " << p.y << "}" << std::endl;
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* Register two systems that are executed when Position is added or set */
    ecs.system<Position>().kind(flecs::OnAdd).each(AddPosition);
    ecs.system<Position>().kind(flecs::OnSet).each(SetPosition);

    auto e = ecs.entity();

    /* Add Position. Because we have an OnAdd system, flecs assumes a valid 
     * value will be assigned to Position, and therefore the OnSet system is 
     * invoked directly after the OnAdd system. */
    e.add<Position>();

    /* Set Position. This will trigger the OnSet system. */
    e.set<Position>({30, 40});

    /* Set Position again. This will retrigger the OnSet system */
    e.set<Position>({50, 60});
}
