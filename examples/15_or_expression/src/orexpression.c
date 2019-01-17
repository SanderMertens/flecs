#include <include/orexpression.h>

typedef struct Vector {
    int32_t x;
    int32_t y;
} Vector;

typedef Vector Position;
typedef Vector Velocity;
typedef uint32_t Speed;

void Move(EcsRows *rows) {
    void *row;

    /* While the column can differ per entity, all entities per callback share
     * the same components. That is because this callback is invoked for every
     * matching table, which group entities per component set. */
    EcsEntity component = ecs_component(rows, 1);

    /* Obtain the Speed handle so we can figure out which component is available
     * on the entities */
    EcsEntity Speed_h = ecs_component(rows, 2);

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);

        if (component == Speed_h) {
            Speed *s = ecs_data(rows, row, 1);
            p->x += *s;
            p->y += *s;
            printf("Moved to %d, %d (Speed)\n", p->x, p->y);
        } else {
            Velocity *v = ecs_data(rows, row, 1);
            p->x += v->x;
            p->y += v->y;
            printf("Moved to %d, %d (Velocity)\n", p->x, p->y);
        }
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Speed);

    /* Register the Move system. This system will match any entity that has the
     * Position system, and the Speed OR Velocity system. Pass in the handle
     * to the Speed component so the callback can determine which component is
     * available. */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Speed | Velocity, ID.Speed);

    /* Create entity with Position and Speed component */
    EcsEntity e1 = ecs_new(world, 0);
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Speed, 1);

    /* Create entity with Position and Velocity component */
    EcsEntity e2 = ecs_new(world, 0);
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e2, Velocity, {1, 2});

    /* Call ecs_progress. Both entities will be matched by the Move system */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
