#include <include/addremove.h>
#include <reflecs/reflecs.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef int32_t Speed;

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);

    EcsHandle e = ecs_new(world, 0);

    /* Whenever components are added or removed from entities, entities
     * move around between different internal tables, which can be expensive.
     * With ecs_add and ecs_commit you can add any number of components while
     * guaranteeing that the entity is moved at most once. */
    ecs_add(world, e, Position_h);
    ecs_add(world, e, Speed_h);
    ecs_commit(world, e);

    /* Notice that the Position and Speed components are not initialized at this
     * point. You'll need to either set them with ecs_set, or create an OnAdd
     * system (see the init_system example). */

    /* Removing components works the same way as adding components */
    ecs_remove(world, e, Position_h);
    ecs_remove(world, e, Speed_h);
    ecs_commit(world, e);

    return ecs_fini(world);
}
