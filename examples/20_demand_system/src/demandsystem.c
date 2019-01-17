#include <include/demandsystem.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

/* This function is invoked when the ecs_progress function is called. */
void ListEntities(EcsRows *rows) {
    void *row;

    /* This will loop over the 10 entities we created in the main function. */
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);
        Position *p = ecs_data(rows, row, 0);
        printf("Entity %llu: (%d, %d)\n", entity, p->x, p->y);
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position type as a component with the world */
    ECS_COMPONENT(world, Position);

    /* Use Prefab+Family trick to automatically initialize Position */
    ECS_PREFAB(world, ObjectPrefab, Position);
    ECS_FAMILY(world, Object, ObjectPrefab, Position);
    ecs_set(world, ObjectPrefab_h, Position, {0, 0});

    /* List all the entities with a Position component. Instead of running the
     * system every frame, EcsOnDemand requires a user to explicitly invoke the
     * system with ecs_run. */
    ECS_SYSTEM(world, ListEntities, EcsOnDemand, Position);

    /* Create 10 entities with Object family */
    ecs_new_w_count(world, Object_h, 10, NULL);

    /* List entities with on demand system */
    ecs_run(
        world,           /* The world */
        ListEntities_h,  /* The system handle */
        1.0,             /* delta_time */
        0,               /* Component filter */
        NULL);           /* Userdata (can be accessed through rows->param) */

    /* Cleanup the world. */
    return ecs_fini(world);
}
