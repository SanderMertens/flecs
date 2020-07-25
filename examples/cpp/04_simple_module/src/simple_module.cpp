#include <simple_module.h>
#include <iostream>

SimpleModule::SimpleModule(flecs::world& world)
{
    /* Register module with world */
    flecs::module<SimpleModule>(world, "SimpleModule");

    /* Register components */
    this->position = flecs::component<Position>(world, "Position");
    this->velocity = flecs::component<Velocity>(world, "Velocity");

    /* Register system */
    this->move = flecs::system<Position, Velocity>(world, "Move")
        .each([](flecs::entity e, Position& p, Velocity& v) {    
            p.x += v.x;
            p.y += v.y;

            std::cout << "Moved " << e.name() << " to {" <<
                p.x << ", " << p.y << "}" << std::endl;
        });
}
