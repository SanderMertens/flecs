#include <add_remove_system.h>
#include <iostream>

struct Position {
    double x, y;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    /* This system will be called when Position is added */
    ecs.system<Position>()
        .kind(flecs::OnAdd)
        .each([](Position& p) {
            p.x = 10;
            p.y = 20;
            std::cout << "OnAdd" << std::endl;
        });

    /* This system will be called when Position is removed */
    ecs.system<Position>()
        .kind(flecs::OnRemove)
        .each([](Position& p) {
            std::cout << "OnRemove {" << p.x << ", " << p.y << "}" << std::endl;
        });

    auto e = ecs.entity();

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
