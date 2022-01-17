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
    std::cout << ecs.to_json(ptr).c_str() << "\n"; // {"x":10, "y":20}

    // Convert entity to JSON
    std::cout << e.to_json().c_str() << "\n";
    // {
    //   "path": "ent", 
    //   "type":[
    //     {"pred":"Position", "value":{"x":10.00, "y":20.00}}, 
    //     {"pred":"Identifier", "obj":"Name"}
    //   ]
    // }
}
