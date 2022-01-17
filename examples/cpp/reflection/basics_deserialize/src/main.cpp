#include <basics_deserialize.h>
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

    // Create entity, set value of position using reflection API
    auto e = ecs.entity();
    Position *ptr = e.get_mut<Position>();

    auto cur = ecs.cursor<Position>(ptr);
    cur.push();          // {
    cur.set_float(10.0); //   10
    cur.next();          //   ,
    cur.set_float(20.0); //   20
    cur.pop();           // }

    std::cout << ecs.to_expr(ptr).c_str() << "\n"; // {x: 10.00, y: 20.00}

    // Use member names before assigning values
    cur = ecs.cursor<Position>(ptr);
    cur.push();        // {
    cur.member("y");   //   y: 
    cur.set_float(10); //   10
    cur.member("x");   //   x: 
    cur.set_float(20); //   20
    cur.pop();         // }

    std::cout << ecs.to_expr(ptr).c_str() << "\n"; // {x: 20.00, y: 10.00}
}
