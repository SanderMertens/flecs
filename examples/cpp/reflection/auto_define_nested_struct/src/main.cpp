#include <auto_define_nested_struct.h>
#include <iostream>

// Capture reflection data for Point and Line.
ECS_STRUCT(Point, {
    int32_t x;
    int32_t y;
});

ECS_STRUCT(Line, {
    Point start;
    Point stop;
});

int main(int, char *[]) {
    flecs::world ecs;

    // Point won't be detected by automatic component registration, so register
    // it explicitly.
    ecs.component<Point>();

    // Serialize as usual. Component registration automatically detects the
    // reflection data for Line.
    Line value = {{1, 2}, {3, 4}};
    std::cout << ecs.to_json(&value) << "\n"; // {"start":{"x":1, "y":2}, "stop":{"x":3, "y":4}}
}
