#include <override_init.h>

/* Component types */
typedef struct {
    double x, y;
} Position;

typedef struct {
    double x;
    double y;
} Force;

typedef double Mass;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Mass);

    /* Create base entity. Create entity as disabled, which will prevent it from
     * being matched with systems. This is a common approach to creating
     * entities that are only used as templates for other entities, or in this
     * case, for providing initial component values. */
    ecs_entity_t base = ecs_new_w_entity(world, EcsDisabled);
    ecs_set(world, base, Mass, {10});

    /* Create instances which share the Mass component from a base */
    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);

    /* Add component without setting it. This will initialize the new component
     * with the value from the base, which is a common approach to initializing
     * components with a value when they are added. */
    ecs_add(world, instance, Mass);

    /* Print value of mass. The value will be equal to base, and the instance
     * will own the component. */
    printf("instance: %f (owned = %u)\n", 
        *ecs_get(world, instance, Mass), 
        ecs_owns(world, instance, Mass, true));

    /* Cleanup */
    return ecs_fini(world);
}
