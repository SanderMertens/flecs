#include <basics_json.h>
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
    auto e = ecs.entity("ent")
        .set<Position>({10, 20});

    // Convert position component to JSON string
    const Position *ptr = e.get<Position>();
    std::cout << ecs.to_json(ptr) << "\n"; // {"x":10, "y":20}

    // Convert entity to JSON
    flecs::entity_to_json_desc_t desc;
    desc.serialize_path = true;
    desc.serialize_values = true;
    std::cout << e.to_json(&desc) << "\n";

    // {
    //     "path":"ent", 
    //     "ids":[["Position"]], 
    //     "values":[{
    //         "x":10, 
    //         "y":20
    //     }]
    // }
}
