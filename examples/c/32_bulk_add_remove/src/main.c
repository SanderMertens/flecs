#include <bulk_add_remove.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

/* Typedefs can be used as component types */
typedef Vector2D Position;
typedef Vector2D Velocity;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register Position component */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Movable, Position, Velocity);

    /* Create 3 entities with Position */
    ecs_new_w_count(world, Position, 3);

    /* There will be 3 entities with Position, 0 with Velocity */
    printf("There are %d entities with Position\n", ecs_count(world, Position));
    printf("There are %d entities with Velocity\n", ecs_count(world, Velocity));

    /* Bulk-add Velocity to all entities with Position */
    ecs_add_remove_w_filter(world, Velocity, 0, &(ecs_filter_t) {
        .include = ecs_type(Position)
    });

    printf("\n -- Bulk add Velocity -- \n\n");

    /* There will be 3 entities with Position and Velocity */
    printf("There are %d entities with Position, Velocity\n", ecs_count(world, Movable));

    /* Bulk-remove Position from all entities with Velocity */
    ecs_add_remove_w_filter(world, 0, Position, &(ecs_filter_t) {
        .include = ecs_type(Velocity)
    });

    printf("\n -- Bulk remove Position -- \n\n");

    /* There will be 0 entities with Position, 3 with Velocity */
    printf("There are %d entities with Position\n", ecs_count(world, Position));
    printf("There are %d entities with Velocity\n", ecs_count(world, Velocity));

    /* Cleanup: will invoke OnRemove system */
    return ecs_fini(world);
}
