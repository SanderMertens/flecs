#include <is_alive.h>

int main(void) {
    ecs_world_t *world = ecs_init();

    /* Create new entity, will be alive */
    ecs_entity_t e_1 = ecs_new(world, 0);
    printf("e_1 is alive: %d\n", ecs_is_alive(world, e_1));

    /* Entity will not be alive after deleting */
    ecs_delete(world, e_1);
    printf("e_1 is alive: %d\n", ecs_is_alive(world, e_1));

    /* Create new entity, will return same id but new generation */
    ecs_entity_t e_2 = ecs_new(world, 0);

    /* e_2 is alive, but e_1 is not because the generation increased */
    printf("e_1 is alive: %d\n", ecs_is_alive(world, e_1));
    printf("e_2 is alive: %d\n", ecs_is_alive(world, e_2));

    ecs_fini(world);
}
