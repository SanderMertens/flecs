#include <nested_struct.h>
#include <iostream>

struct Point {
    float x;
    float y;
};

struct Line {
    Point start;
    Point stop;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register components with reflection data
    ecs.component<Point>()
        .member<float>("x")
        .member<float>("y");

    ecs.component<Line>()
        .member<Point>("start")
        .member<Point>("stop");

    // Create entity with Position as usual
    auto e = ecs.entity()
        .set<Line>({{10, 20}, {30, 40}});

    // Convert position component to flecs expression string
    const Line *ptr = e.get<Line>();
    std::cout << ecs.to_expr(ptr).c_str() << std::endl;
    // {start: {x: 10.00, y: 20.00}, stop: {x: 30.00, y: 40.00}}
}
