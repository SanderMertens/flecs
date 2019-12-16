#include <optional.h>

typedef float Health;
typedef float Stamina;
typedef float Mana;

void Regenerate(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Health, health, 1);
    ECS_COLUMN(rows, Stamina, stamina, 2);
    ECS_COLUMN(rows, Mana, mana, 3);

    for (int i = 0; i < rows->count; i ++) {
        if (health) {
            health[i] ++;
            printf("%d: process health\n", (int)rows->entities[i]);
        }

        if (stamina) {
            stamina[i] ++;
            printf("%d: process stamina\n", (int)rows->entities[i]);
        }

        if (mana) {
            mana[i] ++;
            printf("%d: process mana\n", (int)rows->entities[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Health);
    ECS_COMPONENT(world, Stamina);
    ECS_COMPONENT(world, Mana);

    ECS_SYSTEM(world, Regenerate, EcsOnUpdate, ?Health, ?Stamina, ?Mana);

    /* Create three entities that will all match with the Regenerate system */
    ecs_new(world, Health);
    ecs_new(world, Stamina);
    ecs_new(world, Mana);

    /* Run systems */
    ecs_progress(world, 0);

    /* Cleanup */
    return ecs_fini(world);
}
