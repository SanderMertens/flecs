#include <override.h>

/* Component types */
typedef float Mass;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Mass);

    /* Create base entity */
    ecs_entity_t base = ecs_new(world, 0);
    ecs_set(world, base, Mass, {10});

    /* Create instances which share the Mass component from a base */
    ecs_entity_t instance = ecs_new_w_entity(world, ECS_INSTANCEOF | base);

    /* Print value before overriding Mass. The component is not owned, as it is
     * shared with the base entity. */
    printf("Before overriding:\n");
    printf("instance: %f (owned = %u)\n", *ecs_get(world, instance, Mass), ecs_owns(world, instance, Mass, true));

    /* Override Mass of instance, which will give instance a private copy of the
     * Mass component. */
    ecs_set(world, instance, Mass, {20});

    /* Print values after overriding Mass. The value of Mass for instance_1 will
     * be the value assigned in the override (20). Instance now owns Mass,
     * confirming it has a private copy of the component. */
    printf("\nAfter overriding:\n");
    printf("instance: %f (owned = %u)\n", *ecs_get(world, instance, Mass), ecs_owns(world, instance, Mass, true));

    /* Remove override of Mass. This will remove the private copy of the Mass
     * component from instance. */
    ecs_remove(world, instance, Mass);

    /* Print value after removing the override Mass. The component is no longer
     * owned, as the instance is again sharing the component with base. */
    printf("\nAfter removing override:\n");
    printf("instance: %f (owned = %u)\n", *ecs_get(world, instance, Mass), ecs_owns(world, instance, Mass, true));

    /* Cleanup */
    return ecs_fini(world);
}
