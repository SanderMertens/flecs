#include <auto_override.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Velocity;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Create entity with default values for Position and Velocity. Add the
     * EcsDisabled tag to ensure the entity will not be matched by systems,
     * since it is only used to provide initial component values. */
    ECS_ENTITY(world, Base, Position, Velocity, EcsDisabled);
    ecs_set(world, Base, Position, {10, 20});
    ecs_set(world, Base, Velocity, {30, 40});

    /* This type inherits from Base, as well as adding Position and Velocity as
     * private components. This will automatically override the components as an
     * entity with this type is created, which will initialize the private
     * values with the values of the Base entity. This is a common approach to
     * creating entities with an initialized set of components. */
    ECS_TYPE(world, Movable, INSTANCEOF | Base, Position, Velocity);

    /* Create new entity with type */
    ecs_entity_t e = ecs_new(world, Movable);

    /* Get pointers to component values */
    Position *p = ecs_get_ptr(world, e, Position);
    Velocity *v = ecs_get_ptr(world, e, Velocity);

    /* Print values of entity */
    printf("Position: {%f, %f} (owned = %d)\n", p->x, p->y, ecs_has_owned(world, e, Position));
    printf("Velocity: {%f, %f} (owned = %d)\n", v->x, v->y, ecs_has_owned(world, e, Velocity));

    /* Cleanup */
    return ecs_fini(world);
}
