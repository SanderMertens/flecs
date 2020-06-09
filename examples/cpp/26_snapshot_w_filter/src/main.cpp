#include <snapshot_w_filter.h>
#include <iostream>

/* Component types */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Mass {
    float value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");
    flecs::component<Mass>(world, "Mass");

    flecs::system<Position, Velocity>(world)
        .action([](const flecs::iter& it, 
            flecs::column<Position> p, 
            flecs::column<Velocity> v) 
        {    
            for (auto row : it) {
                p[row].x += v[row].x;
                p[row].y += v[row].y;

                std::cout << "Moved " << it.entity(row).name() << " to {" <<
                    p[row].x << ", " << p[row].y << "}" << std::endl;
            }
        });

    flecs::entity(world, "E1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    flecs::entity(world, "E2")
        .set<Position>({30, 40})
        .set<Velocity>({1, 1})
        .set<Mass>({1});

    /* Take a snapshot that records the current state of the entity. Filter out
     * any entities that have the 'Mass' component. */
    std::cout << "Take snapshot" << std::endl;
    flecs::snapshot s(world);
    s.take(flecs::filter(world).exclude<Mass>());

    /* Progress the world a few times, updates position */
    world.progress();
    world.progress();
    world.progress();

    /* Restore snapshot. This will only restore the state for entity E1. */
    std::cout << std::endl << "Restore snapshot" << std::endl;
    s.restore();

    /* Progress the world again, note that the state of E1 has been restored */
    world.progress();
}
