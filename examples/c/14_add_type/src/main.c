#include <add_type.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create a type with both Position and Velocity. This allows applications
     * to create entities with a specific type, or add multiple components in a
     * single operation. This is much more efficient than adding each component
     * individually to an entity */
    ECS_TYPE(world, Movable, Position, Velocity);

    /* Create new entity with type */
    ecs_entity_t e = ecs_new(world, Movable);

    /* Test if entity has the components */
    printf("After new with type:\n");
    printf("Has Position? %u\n", ecs_has(world, e, Position));
    printf("Has Velocity? %u\n", ecs_has(world, e, Velocity));

    /* Remove both components in one operation */
    ecs_remove(world, e, Movable);

    /* Test if entity has the components */
    printf("\nAfter remove with type:\n");
    printf("Has Position? %u\n", ecs_has(world, e, Position));
    printf("Has Velocity? %u\n", ecs_has(world, e, Velocity));

    /* Cleanup */
    return ecs_fini(world);
}
