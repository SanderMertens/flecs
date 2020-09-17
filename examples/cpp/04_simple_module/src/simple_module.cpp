#include <simple_module.h>
#include <iostream>

SimpleModule::SimpleModule(flecs::world& ecs)
{
    /* Register module with world */
    ecs.module<SimpleModule>();

    /* Register components */
    this->position = ecs.component<Position>();
    this->velocity = ecs.component<Velocity>();

    /* Register system */
    this->move = ecs.system<Position, const Velocity>("Move")
        .each([](flecs::entity e, Position& p, const Velocity& v) {    
            p.x += v.x;
            p.y += v.y;

            std::cout << "Moved " << e.name() << " to {" <<
                p.x << ", " << p.y << "}" << std::endl;
        });
}
