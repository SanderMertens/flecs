#include <world_iter.h>
#include <iostream>

/* Component types */
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

    ecs.entity("E1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity("E2")
        .set<Position>({30, 40})
        .set<Velocity>({3, 4});

    // 1: Iterate all entities with Position, Velocity in the world
    ecs.each([](flecs::entity e, Position& p, Velocity& v) {
        std::cout << "Matched " << e.name() << ": Position = {" 
                  << p.x << ", " << p.y << "} Velocity = {"
                  << v.x << ", " << v.y << "}" << std::endl;
    });

    // 2: Iterate all entities in the world
    for (auto it : ecs) {
        flecs::type table_type = it.table_type();
        std::cout << "Iterating table [" << table_type.str() << "]" 
                  << " (" << it.count() << " entities)" << std::endl;
    }
}
