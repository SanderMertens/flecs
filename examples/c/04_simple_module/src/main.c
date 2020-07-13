#include <simple_module.h>

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Import SimpleModule module. After this statement, the application is able
     * to use the Position and Velocity components, and the Move system will be
     * loaded and matched against entities with Position, Velocity. */
    ECS_IMPORT(world, SimpleModule);

    /* Create an entity with Position and Velocity */
    ECS_ENTITY(world, MyEntity, simple.module.Position, simple.module.Velocity);

    /* Initialize values for the entity */
    ecs_set(world, MyEntity, Position, {0, 0});
    ecs_set(world, MyEntity, Velocity, {1, 1});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application move_system is running, press CTRL-C to exit...\n");

    /* Run systems, this will run the Move system for MyEntity */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
