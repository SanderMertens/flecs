#include <helloworld.h>

typedef struct Position {
    float x;
    float y;
} Position;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register a component with the world. */
    ECS_COMPONENT(world, Position);

    /* Create a new empty entity  */
    ECS_ENTITY(world, MyEntity, 0);

    /* Set the Position component on the entity */
    ecs_set(world, MyEntity, Position, {10, 20});

    /* Get the Position component */
    const Position *p = ecs_get(world, MyEntity, Position);

    printf("Position of %s is {%f, %f}\n", 
        ecs_get_name(world, MyEntity), p->x, p->y);

    /* Cleanup */
    return ecs_fini(world);
}
