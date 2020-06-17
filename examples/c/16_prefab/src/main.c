#include <prefab.h>

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

    /* Create a prefab. Prefabs are entities that are solely intended as
     * templates for other entities. Prefabs are by default not matched with
     * systems. In that way they are similar to regular entities with the
     * EcsDisbled tag, except that they have more features which are 
     * demonstrated in the nested_prefab example. */
    ECS_PREFAB(world, BasePrefab, Position, Velocity);
    ecs_set(world, BasePrefab, Position, {10, 20});
    ecs_set(world, BasePrefab, Velocity, {30, 40});

    /* Use the same technique as used in the auto_override example to create a
     * type that causes components to be automatically overriden from the base.
     * Note that prefabs use inheritance. */
    ECS_TYPE(world, Base, INSTANCEOF | BasePrefab, Position, Velocity);

    /* Create new entity with type */
    ecs_entity_t e = ecs_new(world, Base);

    /* Get pointers to component values */
    const Position *p = ecs_get(world, e, Position);
    const Velocity *v = ecs_get(world, e, Velocity);

    /* Print values of entity */
    printf("Position: {%f, %f} (owned = %d)\n", p->x, p->y, ecs_owns(world, e, Position, true));
    printf("Velocity: {%f, %f} (owned = %d)\n", v->x, v->y, ecs_owns(world, e, Velocity, true));

    /* Cleanup */
    return ecs_fini(world);
}
