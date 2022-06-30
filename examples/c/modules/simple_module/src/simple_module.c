#include <simple_module.h>

ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void SimpleModuleImport(ecs_world_t *world) {
    // Create the module entity. The PascalCase module name is translated to a
    // lower case path for the entity name, like "simple.module".
    ECS_MODULE(world, SimpleModule);

    // All contents of the module are created inside the module's namespace, so
    // the Position component will be created as simple.module.Position

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);
}
