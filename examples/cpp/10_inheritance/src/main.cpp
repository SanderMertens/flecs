#include <inheritance.h>
#include <iostream>

/* Component types */
struct Position {
    float x;
    float y;
};

struct Force {
    float x;
    float y;
};

struct Mass {
    float value;
};

/* Implement a move system with support for shared columns */
void Move(flecs::iter& it, flecs::column<Position> p, flecs::column<Force> f, flecs::column<Mass> m) {

    for (auto row : it) {
        /* Explicitly check if the Mass column is shared or not. If the column
         * is shared, each entity that is currently iterated over shared the
         * same base, and thus the same Mass value. This means that rather than
         * accessing m as an array, we should access it as a single value. */
        if (m.is_shared()) {
            /* Mass is shared, use as single value */
            p[row].x += f[row].x / m->value;
            p[row].y += f[row].y / m->value;
        } else {
            /* Mass is not shared, use as array */
            p[row].x += f[row].x / m[row].value;
            p[row].y += f[row].y / m[row].value;
        }

        /* Print something to the console so we can see the system is being
         * invoked */
        std::cout << it.entity(row).name() << " moved to {.x = "
            << p[row].x << ", .y = "
            << p[row].y << "}" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    /* Register components */
    flecs::component<Position>(world, "Position");
    flecs::component<Force>(world, "Force");
    flecs::component<Mass>(world, "Mass");

    /* Define a system called Move that is executed every frame, and subscribes
     * for the 'Position', 'Force' and 'Mass' components. The Mass component
     * will be either shared or owned. */
    flecs::system<Position, Force, Mass>(world).action(Move);

    /* Demonstrate that a system can also use 'each' to abstract away from the
     * difference between shared and owned components */
    flecs::system<Mass>(world).each(
        [](flecs::entity e, Mass& m) {
            std::cout << e.name() << ": Mass = " << m.value << std::endl;
        });

    /* Create two base entities */
    auto LightEntity = flecs::entity(world, "LightEntity").set<Mass>({100});
    auto HeavyEntity = flecs::entity(world, "HeavyEntity").set<Mass>({200});

    /* Create an entity which does not share Mass from a base */
    flecs::entity(world, "MyEntity")
        .set<Position>({0, 0})
        .set<Force>({10, 10})
        .set<Mass>({50});

    /* Create entities which share the Mass component from a base */
    flecs::entity(world, "MyInstance1")
        .add_instanceof(LightEntity)
        .set<Position>({0, 0})
        .set<Force>({10, 10});

    flecs::entity(world, "MyInstance2")
        .add_instanceof(HeavyEntity)
        .set<Position>({0, 0})
        .set<Force>({10, 10}); 

    flecs::entity(world, "MyInstance3")
        .add_instanceof(HeavyEntity)
        .set<Position>({0, 0})
        .set<Force>({10, 10});                

    world.set_target_fps(1);

    std::cout << "Application inheritance is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (world.progress()) { }
}
