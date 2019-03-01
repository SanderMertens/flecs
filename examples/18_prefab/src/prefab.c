#include <include/prefab.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Speed;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);

        /* The Speed component is actually defined on the prefab, but for the
         * callback it looks just like an ordinary component. This allows this
         * system to be reused for entities with and without prefabs. */
        Speed *s = ecs_data(rows, row, 1);

        /* Try to change the value of the Speed component here. You'll notice
         * that it changes the values for all entities created with the same
         * prefab, not just for this entity */

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

    /* Create two prefabs. Prefabs are entities of which its component values
     * can be shared across many entities. A common usecase for prefabs is when
     * there are different "classes" of entities with different values for
     * components.
     *
     * In addition to being useful, prefabs are also very memory efficient.
     * Component values of a prefab are only stored once in memory, but can
     * be reused by many entities. */
    ECS_PREFAB(world, CarPrefab, Position, Speed);
    ECS_PREFAB(world, PlanePrefab, Position, Speed);

    /* Create two families that include prefabs and override Position. Entities
     * can selectively override components of prefabs which allows the entity to
     * set component values that are not shared with the prefab.
     *
     * By adding both the prefab (which has the Position component) and the
     * Position component in a type, entities that use this type
     * automatically override the Position component.
     *
     * When a component is overridden, the value from the prefab is copied into
     * the entity. This makes prefabs also a useful tool for initializing
     * entity values. */
    ECS_TYPE(world, Car, CarPrefab, Position);
    ECS_TYPE(world, Plane, PlanePrefab, Position);

    /* Initialize prefab position. Even though the Position will be overriden by
     * the entities, setting the value on the prefab will ensure that the
     * entities are initialized with valid data. */
    ecs_set(world, CarPrefab_h, Position, {0, 0});
    ecs_set(world, PlanePrefab_h, Position, {0, 0});

    /* Initialize prefab speeds */
    ecs_set(world, CarPrefab_h, Speed, {50});
    ecs_set(world, PlanePrefab_h, Speed, {500});

    /* Create five cars */
    ecs_new_w_count(world, Car_h, 5, NULL);

    /* Create five planes */
    ecs_new_w_count(world, Plane_h, 5, NULL);

    /* Recap: the entities we just created have two components, Position and
     * Speed. The Speed component is shared with the prefab. Each entity has its
     * own private Position component, since the type overrides it. The
     * Position value for each entity was initialized from the prefab. */

    /* Call ecs_progress */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
