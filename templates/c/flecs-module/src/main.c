#include <${id underscore}.h>

void ${id pascalcase}Import(
    ecs_world_t *world)
{
    ECS_MODULE(world, ${id pascalcase});

    /*
     * ECS_COMPONENT(world, MyComponent);
     * ECS_ENTITY(world, MyTag, 0);
     * 
     * ECS_EXPORT_COMPONENT(MyComponent);
     * ECS_EXPORT_ENTITY(MyTag);
     */
}