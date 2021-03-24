#include <snapshot.h>
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

    ecs.system<Position, const Velocity>()
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
                p.x += v.x;
                p.y += v.y;
                std::cout << "Moved " << e.name() << " to {" <<
                    p.x << ", " << p.y << "}" << std::endl;
        });

    ecs.entity("MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    /* Take a snapshot of the world */
    std::cout << "Take snapshot" << std::endl;
    auto s = ecs.snapshot();
    s.take();

    /* Progress the world a few times, updates position */
    ecs.progress();
    ecs.progress();
    ecs.progress();

    /* Restore snapshot */
    std::cout << std::endl << "Restore snapshot" << std::endl;
    s.restore();

    ecs.progress();
}
