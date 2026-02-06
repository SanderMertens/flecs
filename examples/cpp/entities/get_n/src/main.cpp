#include <get_n.h>
#include <iostream>

/* This code shows how to get multiple components in a single command */

struct Position {
    double x, y;
};

struct Mass {
    double value;
};

struct Velocity {
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create new entity, set Position and Mass component
    flecs::entity e = ecs.entity()
        .insert([](Position& p, Mass& m) {
            p.x = 10;
            p.y = 20;
            m.value = 100;
        });

    {
        auto [pos, mass] = e.get_mut_n<Position, Mass>();
        pos.x += 5;
        mass.value += 3;
        std::cout << "Position: {" << pos.x << ", " << pos.y << "}\n";
        std::cout << "Mass: {" << mass.value << "}\n";
        // Output:
        //  Position: {15, 20}
        //  Mass: {103}
    }
    std::cout << "\n";
    {
        const auto [pos, mass] = e.get_n<Position, Mass>();
        // pos.x += 5; // Will not compile here!
        // mass.value += 3; // Will not compile here!
        std::cout << "Position: {" << pos.x << ", " << pos.y << "}\n";
        std::cout << "Mass: {" << mass.value << "}\n";
        // Output:
        //  Position: {15, 20}
        //  Mass: {100}
    }
    std::cout << "\n";
    // It's possible to try_get_n with absent component that will return null
    {
        const auto [pos, velocity, mass] = e.try_get_n<Position, Velocity, Mass>();

        std::cout << "Position: {" << pos->x << ", " << pos->y << "}\n";
        if (velocity) {
            std::cout << "Velocity: {" << velocity->x << ", " << velocity->y << "}\n";
        } else {
            std::cout << "Velocity: not found\n";
        }
        std::cout << "Mass: {" << mass->value << "}\n";
        // Output:
        //  Position: {15, 20}
        //  Velocity: not found
        //  Mass: {100}
    }
    
    return 0;
}
