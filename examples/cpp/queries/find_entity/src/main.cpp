#include <find_entity.h>
#include <iostream>

struct Position {
    int x, y;
};

int main() {
    flecs::world ecs;

    ecs.entity("e1").set<Position>({10, 20});
    ecs.entity("e2").set<Position>({20, 30});

    // Create a simple query for component Position
    flecs::query<Position> q = ecs.query<Position>();

    // Find the entity for which Position.x is 20
    flecs::entity e = q.find([](Position& p) {
        return p.x == 20;
    });

    if (e) {
        std::cout << "Found entity " << e.path() << std::endl;
    } else {
        std::cout << "No entity found" << std::endl;
    }

    // Output
    //  Found entity ::e2
}
