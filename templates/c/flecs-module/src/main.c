#include <${id underscore}.h>

void ${id pascalcase}Import(
    ecs_world_t *world,
    int flags)
{
    ECS_MODULE(world, ${id pascalcase});

    /*
     * ECS_COMPONENT(world, MyComponent);
     * ECS_TAG(world, MyTag);
     * 
     * ECS_EXPORT_COMPONENT(MyComponent);
     * ECS_EXPORT_ENTITY(MyTag);
     */
}