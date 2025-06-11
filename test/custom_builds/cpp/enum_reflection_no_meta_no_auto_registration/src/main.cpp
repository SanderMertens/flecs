#include <enum_reflection_no_meta_no_auto_registration.h>
#include <iostream>

// Test if enum reflection works without the meta addon

enum Color {
    Red, Green, Blue
};

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    flecs::entity c = world.component<Color>();

    if (!world.is_alive(flecs::Constant)) {
        std::cout << "Constant tag is not alive\n";
        return -1;
    }

    {
        flecs::entity constant = c.lookup("Red");
        if (!constant) {
            std::cout << "Constant not found\n";
            return -1;
        }

        using U = std::underlying_type<Color>::type;

        if (!constant.has(flecs::Constant, world.id<U>())) {
            std::cout << "Constant pair not found on constant\n";
            return -1;
        }

        const U& v = constant.get_second<U>(flecs::Constant);
        if (v != Red) {
            std::cout << "Constant has wrong value\n";
            return -1;
        }
    }

    {
        flecs::entity constant = c.lookup("Green");
        if (!constant) {
            std::cout << "Constant not found\n";
            return -1;
        }

        using U = std::underlying_type<Color>::type;

        if (!constant.has(flecs::Constant, world.id<U>())) {
            std::cout << "Constant pair not found on constant\n";
            return -1;
        }

        const U& v = constant.get_second<U>(flecs::Constant);
        if (v != Green) {
            std::cout << "Constant has wrong value\n";
            return -1;
        }
    }

    {
        flecs::entity constant = c.lookup("Blue");
        if (!constant) {
            std::cout << "Constant not found\n";
            return -1;
        }

        using U = std::underlying_type<Color>::type;

        if (!constant.has(flecs::Constant, world.id<U>())) {
            std::cout << "Constant pair not found on constant\n";
            return -1;
        }

        const U& v = constant.get_second<U>(flecs::Constant);
        if (v != Blue) {
            std::cout << "Constant has wrong value\n";
            return -1;
        }
    }

    flecs::entity e = world.entity();

    e.add(Color::Red);
    if (!e.has(Color::Red)) {
        std::cout << "Entity doesn't have Color::Red\n";
        return -1;
    }

    e.add(Color::Green);
    if (!e.has(Color::Green)) {
        std::cout << "Entity doesn't have Color::Green\n";
        return -1;
    }

    if (e.has(Color::Red)) {
        std::cout << "Entity has Color::Red\n";
        return -1;
    }

    return 0;
}
