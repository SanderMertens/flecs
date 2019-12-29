#include <simple_module.h>
#include <iostream>

SimpleModule::SimpleModule(flecs::world& world, int flags) 
{
    /* Register module with world */
    flecs::module<SimpleModule>(world, "SimpleModule");

    /* Register components */
    this->position = flecs::component<Position>(world, "Position");
    this->velocity = flecs::component<Velocity>(world, "Velocity");

    /* Register system */
    this->move = flecs::system<Position, Velocity>(world, "Move")
        .action([](const flecs::rows& rows, 
            flecs::column<Position> p, 
            flecs::column<Velocity> v) 
        {    
            for (auto row : rows) {
                p[row].x += v[row].x;
                p[row].y += v[row].y;

                std::cout << "Moved " << rows.entity(row).name() << "to {" <<
                    p[row].x << ", " << p[row].y << "}" << std::endl;
            }
        });
}
