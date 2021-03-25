#include <is_alive.h>

int main(void) {
    ecs_world_t *world = ecs_init();

    /* Create new entity, will be alive */
    ecs_entity_t e1 = ecs_new(world, 0);
    printf("e1 is alive: %d\n", ecs_is_alive(world, e1));

    /* Entity will not be alive after deleting */
    ecs_delete(world, e1);
    printf("e1 is alive: %d\n", ecs_is_alive(world, e1));

    /* Create new entity, will return same id but new generation */
    ecs_entity_t e2 = ecs_new(world, 0);

    /* e2 is alive, but e1 is not because the generation increased */
    printf("e1 is alive: %d\n", ecs_is_alive(world, e1));
    printf("e2 is alive: %d\n", ecs_is_alive(world, e2));

    ecs_fini(world);
}
