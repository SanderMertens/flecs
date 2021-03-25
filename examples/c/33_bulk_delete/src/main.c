#include <bulk_delete.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register Position component */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Movable, Position, Velocity);

    /* Create 3 entities with Position */
    ecs_bulk_new(world, Position, 3);

    /* Create 3 entities with Position, Velocity */
    ecs_bulk_new(world, Movable, 3);

    /* There will be 6 entities with Position, 3 with Velocity */
    printf("There are %d entities with Position\n", ecs_count(world, Position));
    printf("There are %d entities with Velocity\n", ecs_count(world, Velocity));

    printf("\n -- Bulk delete all entities with Velocity -- \n\n");

    ecs_bulk_delete(world, &(ecs_filter_t){
        .include = ecs_type(Velocity)
    });

    /* There will be 3 entities with Position, 0 with Velocity */
    printf("There are %d entities with Position\n", ecs_count(world, Position));
    printf("There are %d entities with Velocity\n", ecs_count(world, Velocity));

    /* Cleanup: will invoke OnRemove system */
    return ecs_fini(world);
}
