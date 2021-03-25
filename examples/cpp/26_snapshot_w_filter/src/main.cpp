#include <snapshot_w_filter.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

struct Mass {
    double value;
};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
                p.x += v.x;
                p.y += v.y;
                std::cout << "Moved " << e.name() << " to {" <<
                    p.x << ", " << p.y << "}" << std::endl;
        });

    ecs.entity("E1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    ecs.entity("E2")
        .set<Position>({30, 40})
        .set<Velocity>({1, 1})
        .set<Mass>({1});

    /* Take a snapshot that records the current state of the entity. Filter out
     * any entities that have the 'Mass' component. */
    std::cout << "Take snapshot" << std::endl;
    auto s = ecs.snapshot();
    s.take(flecs::filter(ecs).exclude<Mass>());

    /* Progress the world a few times, updates position */
    ecs.progress();
    ecs.progress();
    ecs.progress();

    /* Restore snapshot. This will only restore the state for entity E1. */
    std::cout << std::endl << "Restore snapshot" << std::endl;
    s.restore();

    /* Progress the world again, note that the state of E1 has been restored */
    ecs.progress();
}
