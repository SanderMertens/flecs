#include <set_system.h>
#include <iostream>

struct Position {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* This system will be called when Position is set */
    ecs.system<Position>()
        .kind(flecs::OnSet)
        .each([](Position& p) {
            std::cout << "OnSet {" << p.x << ", " << p.y << "}" << std::endl;
        });

    auto e = ecs.entity();

    /* Add Position. This will not trigger the OnSet system */
    e.add<Position>();

    /* Set Position. This will trigger the OnSet system. */
    e.set<Position>({30, 40});

    /* Set Position again. This will retrigger the OnSet system */
    e.set<Position>({50, 60});
}
