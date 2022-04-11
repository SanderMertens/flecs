#include <multi_set_get.h>
#include <iostream>

/* This code shows how to get & set multiple components in a single command */

struct Position {
    double x, y;
};

struct Mass {
    double value;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create new entity, set Position and Mass component
    auto e = ecs.entity()
        .set([](Position& p, Mass& m) {
            p.x = 10;
            p.y = 20;
            m.value = 100;
        });

    // Print values of Position and Mass component
    bool found = e.get([](const Position& p, const Mass& m) {
        std::cout << "Position: {" << p.x << ", " << p.y << "}\n"
                  << "Mass: {" << m.value << "}\n";
    });

    std::cout << "Components found: " << (found ? "true" : "false") << "\n";

    // Output:
    //  Position: {10, 20}
    //  Mass: {100}
    //  Components found: true
    
    return 0;
}
