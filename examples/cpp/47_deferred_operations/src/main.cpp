#include <deferred_operations.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    /* Create OnSet system so we can see when Velocity is actually set */
    ecs.system<Velocity>()
        .kind(flecs::OnSet)
        .each([](flecs::entity, Velocity& v) {
            std::cout << "Velocity set to {" << v.x << ", " << v.y << "}" 
                      << std::endl;
        });

    // Create 3 entities with position
    ecs.entity().add<Position>();
    ecs.entity().add<Position>();
    ecs.entity().add<Position>();

    // Create a query for Position to set Velocity for each entity with Position.
    // Because adding a component changes the underlying data structures, we
    // need to defer the operations until we have finished iterating.
    auto q = ecs.query<Position>();

    std::cout << "Defer begin" << std::endl;
    ecs.defer_begin();

    q.each([](flecs::entity e, Position&) {
        e.set<Velocity>({1, 2});
    });

    std::cout << "Defer end" << std::endl;
    ecs.defer_end();
}
