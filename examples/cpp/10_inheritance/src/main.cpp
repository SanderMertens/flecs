#include <inheritance.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Force {
    double x, y;
};

struct Mass {
    double value;
};

/* Implement a move system with support for shared columns */
void Move(const flecs::iter& it, Position *p, Force *f) {
    auto m = it.column<const Mass>(3);

    for (auto row : it) {
        /* Explicitly check if the Mass column is shared or not. If the column
         * is shared, each entity that is currently iterated over shared the
         * same base, and thus the same Mass value. This means that rather than
         * accessing m as an array, we should access it as a single value. */
        if (!it.is_owned(3)) {
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
    flecs::world ecs(argc, argv);

    /* Explicitly define Mass so it can be used from the query string */
    ecs.component<Mass>();

    /* Define a system called Move that is executed every frame, and subscribes
     * for the 'Position', 'Force' and 'Mass' components. The Mass component
     * will be either shared or owned. */
    ecs.system<Position, Force>("Move", "ANY:Mass").iter(Move);

    /* Demonstrate that a system can also use 'each' to abstract away from the
     * difference between shared and owned components */
    ecs.system<Mass>().each(
        [](flecs::entity e, Mass& m) {
            std::cout << e.name() << ": Mass = " << m.value << std::endl;
        });

    /* Create two base entities */
    auto LightEntity = ecs.entity("LightEntity").set<Mass>({100});
    auto HeavyEntity = ecs.entity("HeavyEntity").set<Mass>({200});

    /* Create an entity which does not share Mass from a base */
    ecs.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Force>({10, 10})
        .set<Mass>({50});

    /* Create entities which share the Mass component from a base */
    ecs.entity("MyInstance1")
        .add(flecs::IsA, LightEntity)
        .set<Position>({0, 0})
        .set<Force>({10, 10});

    ecs.entity("MyInstance2")
        .add(flecs::IsA, HeavyEntity)
        .set<Position>({0, 0})
        .set<Force>({10, 10}); 

    ecs.entity("MyInstance3")
        .add(flecs::IsA, HeavyEntity)
        .set<Position>({0, 0})
        .set<Force>({10, 10});                

    ecs.set_target_fps(1);

    std::cout << "Application inheritance is running, press CTRL-C to exit..." << std::endl;

    /* Run systems */
    while (ecs.progress()) { }
}
