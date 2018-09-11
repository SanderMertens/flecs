#include "reflecs.h"

EcsEntity* ecs_component_new(
    EcsWorld *world,
    const char *id,
    size_t size)
{
    EcsEntity *result = ecs_new(world, id);
    EcsComponent *component_data = _ecs_add(result, world->component);
    component_data->size = size;
    return result;
}
