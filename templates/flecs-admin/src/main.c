#include <include/${id base}.h>

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    /* Import admin and civetweb systems. The admin systems are implemented on
     * top of EcsComponentsHttp, which only defines HTTP components. To
     * actually start a web server, we need the Civetweb system which listens
     * for the Http components. */
    ECS_IMPORT(world, EcsSystemsCivetweb, 0);
    ECS_IMPORT(world, EcsSystemsAdmin, 0);

    /* Create a new entity with an initialized EcsAdmin component. This will
     * start the flecs admin web app on port 9090. */
    ecs_set(world, 0, EcsAdmin, {.port = 9090});

    /* Don't run too fast */
    ecs_set_target_fps(world, 60);

    /* Run the application */
    while ( ecs_progress(world, 0));

    return ecs_fini(world);
}
