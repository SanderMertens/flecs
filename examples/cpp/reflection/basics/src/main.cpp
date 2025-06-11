#include <basics.h>
#include <iostream>

struct Position {
    float x;
    float y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register component with reflection data
    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    // Create entity with Position as usual
    flecs::entity e = ecs.entity()
        .set<Position>({10, 20});

    // Convert position component to flecs expression string
    const Position& p = e.get<Position>();
    std::cout << ecs.to_expr(&p).c_str() << "\n"; // {x: 10, y: 20}
}
