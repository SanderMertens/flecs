#include <include/nestedprefab.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Speed;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_column(rows, row, 0);
        Speed *s = ecs_column(rows, row, 1);

        p->x += *s;
        p->y += *s;
        printf("Moved to %d, %d\n", p->x, p->y);
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);

    /* Move system. For a system callback it is transparent whether a component
     * comes from a prefab or from an entity. */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Speed);

    /* Prefabs can be nested, which allows applications to define hierarchies
     * of entity "classes". */
    ECS_PREFAB(world, Vehicle, Position);
    ECS_PREFAB(world, CarPrefab, Vehicle, Speed);
    ECS_PREFAB(world, PlanePrefab, Vehicle, Speed);

    /* Create two families that include prefabs and override Position. */
    ECS_FAMILY(world, Car, CarPrefab, Position);
    ECS_FAMILY(world, Plane, PlanePrefab, Position);

    /* Initialize Position once, which initializes both Car and Plane prefabs */
    ecs_set(world, Vehicle_h, Position, {0, 0});

    /* Initialize prefab speeds */
    ecs_set(world, CarPrefab_h, Speed, 50);
    ecs_set(world, PlanePrefab_h, Speed, 500);

    /* Create five cars */
    ecs_new_w_count(world, Car_h, 5, NULL);

    /* Create five planes */
    ecs_new_w_count(world, Plane_h, 5, NULL);

    /* Recap: the entities we just created have two components, Position and
     * Speed. The Speed component is shared with the prefab. Each entity has its
     * own private Position component, since the family overrides it. The
     * Position value for each entity was initialized from the Vehicle prefab.
     */

    /* Call ecs_progress */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
