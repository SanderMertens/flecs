#include <filter_iter.h>
#include <iostream>

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});

    ecs.entity("e2")
        .set<Position>({30, 40})
        .set<Velocity>({1, 1});

    ecs.entity("e3")
        .set<Position>({40, 50});

    auto f = ecs.filter<Position, Velocity>();

    f.each([&](flecs::entity e, Position& p, Velocity&) {
        std::cout << "Matched " << e.name() << ": {" 
                  << p.x << ", " << p.y << "}" << std::endl;
    });
}
