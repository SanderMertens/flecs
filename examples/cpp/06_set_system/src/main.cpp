#include <set_system.h>
#include <iostream>

struct Position {
    double x, y;
};

/* This system will be called when Position is added */
void AddPosition(flecs::entity, Position&) {
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

    /* Add Position. This will not trigger the OnSet system */
    e.add<Position>();

    /* Set Position. This will trigger the OnSet system. */
    e.set<Position>({30, 40});

    /* Set Position again. This will retrigger the OnSet system */
    e.set<Position>({50, 60});
}
