#include <include/entityid.h>
#include <reflecs/reflecs.h>

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Create empty entity */
    EcsHandle e = ecs_new(world, 0);

    /* Set the id of the entity */
    ecs_set(world, e, EcsId, "MyEntity");

    /* Get the id of the entity */
    printf("Entity id = %s (ecs_id)\n", ecs_id(world, e));

    /* Get the id of the entity with ecs_get */
    printf("Entity id = %s (ecs_get)\n", ecs_get(world, e, EcsId));

    /* Cleanup the world. */
    return ecs_fini(world);
}
