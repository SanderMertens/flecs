#include <system_features.h>

void SystemA(ecs_iter_t *it) {
    printf("System A called!\n");
}

void SystemB(ecs_iter_t *it) {
    printf("System B called!\n");
}

void SystemC(ecs_iter_t *it) {
    printf("System C called!\n");
}

void SystemD(ecs_iter_t *it) {
    printf("System D called!\n");
}

void SystemE(ecs_iter_t *it) {
    printf("System E called!\n");
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Create dummy systems */
    ECS_SYSTEM(world, SystemA, EcsOnUpdate, 0);
    ECS_SYSTEM(world, SystemB, EcsOnUpdate, 0);
    ECS_SYSTEM(world, SystemC, EcsOnUpdate, 0);
    ECS_SYSTEM(world, SystemD, EcsOnUpdate, 0);
    ECS_SYSTEM(world, SystemE, EcsOnUpdate, 0);

    /* Create two features, each with a set of systems. Features are regular
     * types, and the name feature is just a convention to indicate that a type
     * only contains systems. Since systems, just like components, are stored as
     * entities, they can be contained by types. */
    ECS_TYPE(world, Feature1, SystemA, SystemB);
    ECS_TYPE(world, Feature2, SystemC, SystemD);

    /* Create a feature that includes Feature2 and SystemE. Types/features can
     * be organized in hierarchies */
    ECS_TYPE(world, Feature3, Feature2, SystemE);

    /* First, disable Feature1 and Feature3. No systems will be executed. */
    printf("Feature1 disabled, Feature3 disabled:\n");
    ecs_enable(world, Feature1, false);
    ecs_enable(world, Feature3, false);
    ecs_progress(world, 1);

    /* Enable Feature3 */
    printf("\nFeature1 disabled, Feature3 enabled:\n");
    ecs_enable(world, Feature3, true);
    ecs_progress(world, 1);

    /* Disable Feature2. This will disable some of the systems in Feature3 too */
    printf("\nFeature1 disabled, Feature3 partially enabled:\n");
    ecs_enable(world, Feature2, false);
    ecs_progress(world, 1);

    /* Enable Feature1 */
    printf("\nFeature1 enabled, Feature3 partially enabled:\n");
    ecs_enable(world, Feature1, true);
    ecs_progress(world, 1);

    /* Disable SystemE */
    printf("\nDisable SystemE:\n");
    ecs_enable(world, SystemE, false);
    ecs_progress(world, 1);

    /* Cleanup */
    return ecs_fini(world);
}
