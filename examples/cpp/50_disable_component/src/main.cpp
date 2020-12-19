#include <disable_component.h>
#include <iostream>

struct Position {
    float x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    auto e1 = ecs.entity("e1")
        .set<Position>({10, 20});

    auto e2 = ecs.entity("e2")
        .set<Position>({30, 40});

    auto e3 = ecs.entity("e3")
        .set<Position>({50, 60});

    // All components will show up as enabled
    std::cout << "e1 enabled: " << e1.is_enabled<Position>() << std::endl;
    std::cout << "e2 enabled: " << e2.is_enabled<Position>() << std::endl;
    std::cout << "e3 enabled: " << e3.is_enabled<Position>() << std::endl;

    // Create system that matches all entities with position
    auto s = ecs.system<const Position>()
        .each([](flecs::entity e, const Position& p) {
            std::cout << e.name() << " = {" << p.x << ", " << p.y << "}" << std::endl;
        });

    std::cout << std::endl << "1st run: all components enabled" << std::endl;
    s.run();

    // Enable component of entity 1, disable component of entity 2
    e1.enable<Position>();
    e2.disable<Position>();

    // e2 will now show up as disabled
    std::cout << std::endl;
    std::cout << "e1 enabled: " << e1.is_enabled<Position>() << std::endl;
    std::cout << "e2 enabled: " << e2.is_enabled<Position>() << std::endl;
    std::cout << "e3 enabled: " << e3.is_enabled<Position>() << std::endl;    

    std::cout << std::endl << "2nd run: e2 is disabled" << std::endl;
    s.run();  

    // Print types. Both e1 and e2 will have DISABLED|Position. This does not
    // actually mean that both are disabled. Instead it means that both entities
    // have a bitset that tracks whether the component is disabled.
    std::cout << std::endl;
    std::cout << "e1 type: " << e1.type().str() << std::endl;
    std::cout << "e2 type: " << e2.type().str() << std::endl;
    std::cout << "e3 type: " << e3.type().str() << std::endl;
}
