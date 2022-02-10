#include <nested_set_member.h>
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

    ecs.component<Point>()
        .member<float>("x")
        .member<float>("y");

    ecs.component<Line>()
        .member<Point>("start")
        .member<Point>("stop");

    // Create entity, set value of Line using reflection API
    auto e = ecs.entity();
    Line *ptr = e.get_mut<Line>();

    auto cur = ecs.cursor<Line>(ptr);
    cur.push();          // {
    cur.member("start"); //   start:
    cur.push();          //   {
    cur.member("x");     //     x:
    cur.set_float(10);   //     10
    cur.member("y");     //     y:
    cur.set_float(20);   //     20
    cur.pop();           //   }
    cur.member("stop");  //   stop:
    cur.push();          //   {
    cur.member("x");     //     x:
    cur.set_float(30);   //     30
    cur.member("y");     //     y:
    cur.set_float(40);   //     40
    cur.pop();           //   }
    cur.pop();           // }

    // Convert component to string
    std::cout << ecs.to_expr(ptr).c_str() << "\n";
    // {start: {x: 10.00, y: 20.00}, stop: {x: 30.00, y: 40.00}}
}
