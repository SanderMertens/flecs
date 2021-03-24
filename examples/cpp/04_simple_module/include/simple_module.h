#ifndef SIMPLE_MODULE_H
#define SIMPLE_MODULE_H

/* This generated file contains includes for project dependencies */
#include "simple_module/bake_config.h"
#include <iostream>

struct simple_module {
    // Define types inside module scope. This is not mandatory, but ensures
    // that their fully qualified Flecs name matches the C++ type name. It also
    // ensures that type names cannot clash between modules.

    struct Position {
        double x, y;
    };

    struct Velocity {
        double x, y;
    };

    simple_module(flecs::world& ecs) {
        /* Register module with world */
        ecs.module<simple_module>();

        /* Register components */
        ecs.component<Position>();
        ecs.component<Velocity>();

        /* Register system */
        ecs.system<Position, const Velocity>("Move")
            .each([](flecs::entity e, Position& p, const Velocity& v) {    
                p.x += v.x;
                p.y += v.y;

                std::cout << "Moved " << e.name() << " to {" <<
                    p.x << ", " << p.y << "}" << std::endl;
            });        
    }
};

#endif
