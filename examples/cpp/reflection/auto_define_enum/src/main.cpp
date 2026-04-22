#include <auto_define_enum.h>
#include <iostream>

// Capture reflection data for Color and Car
ECS_ENUM(Color, {
    Red, Black, White, StainlessSteel
});

ECS_STRUCT(Car, {
    const char *brand;
    Color color;
    float speed;
});

int main(int, char *[]) {
    flecs::world ecs;

    // Color won't be detected by automatic component registration, so register
    // it explicitly.
    ecs.component<Color>();

    // Serialize as usual. Component registration automatically detects the
    // reflection data for Car.
    Car value = {"Delorean", StainlessSteel, 1.21f};
    std::cout << ecs.to_json(&value) << "\n"; // {"brand":"Delorean", "color":"StainlessSteel", "speed":1.2100000381}
}
