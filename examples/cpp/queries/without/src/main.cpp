#include <without.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Npc { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create a query for Position, !Npc. By adding the Npc component using the
    // "without" method, the component is not a part of the query type, and as a
    // result does not become part of the function signatures of each and iter.
    // This is useful for things like tags, which because they don't have a 
    // value are less useful to pass to the each/iter functions as argument.
    //
    // The without method is short for:
    //   .term<Npc>().not_()
    auto q = ecs.query_builder<Position>()
        .without<Npc>()
        .build();

    // Create a few test entities for the Position query
    ecs.entity("e1")
        .set<Position>({10, 20});

    ecs.entity("e2")
        .set<Position>({10, 20});

    // This entity will not match as it has Npc
    ecs.entity("e3")
        .set<Position>({10, 20})
        .add<Npc>();


    // Note how the Npc tag is not part of the each signature
    q.each([](flecs::entity e, Position& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
    });

    // Output:
    //  e1: {10, 20}
    //  e2: {10, 20}
}
