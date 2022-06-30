#include <simple_module.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world ecs;

    ecs.import<simple::module>();

    // Create system that uses component from module
    ecs.system<const simple::Position>("PrintPosition")
        .each([](const simple::Position& p) {
            std::cout << "p = {" << p.x << ", " << p.y << "} (system)\n";
        });

    // Create entity with imported components
    flecs::entity e = ecs.entity()
        .set<simple::Position>({10, 20})
        .set<simple::Velocity>({1, 1});

    // Call progress which runs imported Move system
    ecs.progress();

    // Use component from module in operation
    e.get([](const simple::Position& p) {
        std::cout << "p = {" << p.x << ", " << p.y << "} (get)\n";
    });

    // Output:
    //   p = {11.000000, 22.000000} (system)
    //   p = {11.000000, 22.000000} (get)
}