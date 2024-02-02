#include <simple_module.h>

namespace simple {

module::module(flecs::world& ecs) {
    // Register module with world. The module entity will be created with the
    // same hierarchy as the C++ namespaces (e.g. simple.module)
    ecs.module<module>();

    // All contents of the module are created inside the module's namespace, so
    // the Position component will be created as simple.module.Position

    // Component registration is optional, however by registering components
    // inside the module constructor, they will be created inside the scope
    // of the module.
    ecs.component<Position>();
    ecs.component<Velocity>();

    ecs.system<Position, const Velocity>("Move")
        .each([](Position& p, const Velocity& v) {    
            p.x += v.x;
            p.y += v.y;
        });        
}

}
