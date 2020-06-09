#include <snapshot.h>
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

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world world(argc, argv);

    flecs::component<Position>(world, "Position");
    flecs::component<Velocity>(world, "Velocity");

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

    flecs::entity(world, "MyEntity")
        .set<Position>({0, 0})
        .set<Velocity>({1, 1});

    /* Take a snapshot of the world */
    std::cout << "Take snapshot" << std::endl;
    flecs::snapshot s(world);
    s.take();

    /* Progress the world a few times, updates position */
    world.progress();
    world.progress();
    world.progress();

    /* Restore snapshot */
    std::cout << std::endl << "Restore snapshot" << std::endl;
    s.restore();

    world.progress();
}
