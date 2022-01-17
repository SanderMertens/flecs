#include <runtime_component.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world ecs;

    // Create component for a type that isn't known at compile time
    auto position = ecs.component("Position")
        .member<float>("x")
        .member<float>("y");

    // Create entity, set value of position using reflection API
    auto e = ecs.entity();
    void *ptr = e.get_mut(position);

    auto cur = ecs.cursor(position, ptr);
    cur.push();
    cur.set_float(10);
    cur.next();
    cur.set_float(20);
    cur.pop();

    // Convert component to string
    std::cout << ecs.to_expr(position, ptr).c_str() << "\n"; // {x: 10, y: 20}
}
