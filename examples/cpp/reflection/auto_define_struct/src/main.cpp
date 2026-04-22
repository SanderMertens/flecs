#include <auto_define_struct.h>
#include <iostream>

// Capture reflection data for Position
ECS_STRUCT(Position, {
    float x;
    float y;
});

int main(int, char *[]) {
    flecs::world ecs;

    // Serialize as usual. Component registration automatically detects the
    // reflection data. 
    Position value = {10, 20};
    std::cout << ecs.to_json(&value) << "\n"; // {"x":10, "y":20}
}
