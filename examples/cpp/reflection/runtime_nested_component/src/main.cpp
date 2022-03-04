#include <runtime_nested_component.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world ecs;

    // Create components for types that aren't known at compile time
    auto point = ecs.component("Point")
        .member<float>("x")
        .member<float>("y");

    auto line = ecs.component("Line")
        .member(point, "start")
        .member(point, "stop");

    // Create entity, set value of position using reflection API
    auto e = ecs.entity();
    void *ptr = e.get_mut(line);

    auto cur = ecs.cursor(line, ptr);
    cur.push();        // {
    cur.push();        //   {
    cur.set_float(10); //     10
    cur.next();        //     ,
    cur.set_float(20); //     20
    cur.pop();         //   }
    cur.next();        //   ,
    cur.push();        //   {
    cur.set_float(30); //     30
    cur.next();        //     ,
    cur.set_float(40); //     40
    cur.pop();         //   }
    cur.pop();         // }

    // Convert component to string
    std::cout << ecs.to_expr(line, ptr).c_str() << "\n"; 
    // {start: {x: 10.00, y: 20.00}, stop: {x: 30.00, y: 40.00}}
}
