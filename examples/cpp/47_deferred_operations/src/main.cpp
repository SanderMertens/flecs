#include <deferred_operations.h>
#include <iostream>

struct Position {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    /* Create OnSet system so we can see when Velocity is actually set */
    ecs.system<Position>()
        .kind(flecs::OnSet)
        .each([](Position& p) {
            std::cout << "Position set to {" << p.x << ", " << p.y << "}" 
                      << std::endl;
        });

    // Defer operations until end of defer statement
    std::cout << "Defer begin" << std::endl;
    ecs.defer([&]{
        ecs.entity().set<Position>({10, 20});
        ecs.entity().set<Position>({20, 30});
        ecs.entity().set<Position>({30, 40});

        // After the function exits, the deferred operations will be processed
        // and the OnSet system will be called.
        std::cout << "Operations enqueued" << std::endl;
    });
    std::cout << "Defer end" << std::endl;
}
