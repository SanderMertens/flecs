#include <world_ser_deser.h>
#include <iostream>
#include <string>

struct Position {
    double x;
    double y;
};

struct Velocity {
    double x;
    double y;
};

// Register components and systems in a module. This excludes them by default
// from the serialized data, and makes it easier to import across worlds.
struct move {
    move(flecs::world& world) {
        world.component<Position>()
            .member<double>("x")
            .member<double>("y");

        world.component<Velocity>()
            .member<double>("x")
            .member<double>("y");

        world.system<Position, const Velocity>("Move")
            .each([](flecs::entity e, Position& p, const Velocity& v) {
                p.x += v.x;
                p.y += v.y;
                std::cout << e.path() << " moved to "
                    << "{x: " << p.x << ", y: " << p.y << "}"
                    << std::endl;
            });
    }
};

int main(int, char *[]) {
    flecs::world world_a;

    world_a.import<move>();

    world_a.entity("ent_1")
        .set<Position>({10, 20})
        .set<Velocity>({1, -1});

    world_a.entity("ent_2")
        .set<Position>({30, 40})
        .set<Velocity>({-1, 1});

    // Serialize world to JSON
    auto json = world_a.to_json();
    std::cout << json << std::endl << std::endl;

    // Output:
    // {
    //     "results": [{
    //         "ids": [
    //             ["my_module.Position"],
    //             ["my_module.Velocity"],
    //             ["flecs.core.Identifier", "flecs.core.Name"]
    //         ],
    //         "entities": ["ent_1", "ent_2"],
    //         "values": [
    //             [{
    //                 "x": 10,
    //                 "y": 20
    //             }, {
    //                 "x": 30,
    //                 "y": 40
    //             }],
    //             [{
    //                 "x": 1,
    //                 "y": -1
    //             }, {
    //                 "x": -1,
    //                 "y": 1
    //             }], 0
    //         ]
    //     }]
    // }

    // Create second world, import same module
    flecs::world world_b;
    world_b.import<move>();

    // Deserialize JSON into second world
    world_b.from_json(json);

    // Run system once for both worlds
    world_a.progress();
    std::cout << std::endl;
    world_b.progress();

    // Output
    //   ::ent_1 moved to {x: 11, y: 19}
    //   ::ent_2 moved to {x: 29, y: 41}
    //   
    //   ::ent_1 moved to {x: 11, y: 19}
    //   ::ent_2 moved to {x: 29, y: 41}
}
