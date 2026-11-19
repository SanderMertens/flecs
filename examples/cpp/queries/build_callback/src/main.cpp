#include <build_callback.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

struct Npc { };

int main(int, char *[]) {
    flecs::world ecs;

    // The build method accepts a function that configures the builder. This
    // makes it possible to add terms as separate statements, which can be
    // easier to read than one long chain of method calls. Because the builder
    // is passed as auto, template methods are called with the template keyword.
    flecs::query<Position> q = ecs.query_builder<Position>()
        .build([](auto& b) {
            b.template with<Velocity>().optional();
            b.template with<Npc>();
        });

    // Create a few test entities for the Position, Npc query
    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2})
        .add<Npc>();

    ecs.entity("e2")
        .set<Position>({10, 20})
        .add<Npc>();

    // This entity will not match as it does not have Npc
    ecs.entity("e3")
        .set<Position>({10, 20});

    q.each([](flecs::entity e, Position& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });

    // Output:
    //  e1: {10, 20}
    //  e2: {10, 20}
}
