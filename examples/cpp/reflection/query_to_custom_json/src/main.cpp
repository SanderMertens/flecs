#include <query_to_custom_json.h>
#include <iostream>

// Same example as query_to_json, but with customized serializer parameters

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Mass {
    float value;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register components with reflection data
    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    ecs.component<Velocity>()
        .member<float>("x")
        .member<float>("y");

    ecs.component<Mass>()
        .member<float>("value");

    ecs.entity("a").set<Position>({10, 20}).set<Velocity>({1, 2});
    ecs.entity("b").set<Position>({20, 30}).set<Velocity>({2, 3});
    ecs.entity("c").set<Position>({30, 40}).set<Velocity>({3, 4}).set<Mass>({10});
    ecs.entity("d").set<Position>({30, 40}).set<Velocity>({4, 5}).set<Mass>({20});

    // Query for components
    auto q = ecs.query<Position, const Velocity>();

    // Serialize query to JSON. Customize serializer to only serialize entity
    // names and component values.
    flecs::iter_to_json_desc_t desc = {};
    desc.serialize_entities = true;
    desc.serialize_values = true;
    std::cout << q.iter().to_json(&desc).c_str() << "\n";

    // Iterator returns 2 sets of results, one for each table.

    // {
    //   "results": [{
    //     "entities": ["a", "b"],
    //     "values": [
    //       [{
    //         "x": 10.00,
    //         "y": 20.00
    //       }, {
    //         "x": 20.00,
    //         "y": 30.00
    //       }],
    //       [{
    //         "x": 1.00,
    //         "y": 2.00
    //       }, {
    //         "x": 2.00,
    //         "y": 3.00
    //       }]
    //     ]
    //   }, {
    //     "entities": ["c", "d"],
    //     "values": [
    //       [{
    //         "x": 30.00,
    //         "y": 40.00
    //       }, {
    //         "x": 30.00,
    //         "y": 40.00
    //       }],
    //       [{
    //         "x": 3.00,
    //         "y": 4.00
    //       }, {
    //         "x": 4.00,
    //         "y": 5.00
    //       }]
    //     ]
    //   }]
    // }
}
