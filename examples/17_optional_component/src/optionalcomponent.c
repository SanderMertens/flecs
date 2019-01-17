#include <include/optionalcomponent.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Speed;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        Speed *s = ecs_data(rows, row, 1);

        /* Add Speed if available, otherwise just add 1 */
        if (s) {
            p->x += *s;
            p->y += *s;
            printf("Moved to %d, %d (using Speed)\n", p->x, p->y);
        } else {
            p->x += 1;
            p->y += 1;
            printf("Moved to %d, %d (no Speed)\n", p->x, p->y);
        }
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);

    /* Move system that optionally accepts Speed */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, ?Speed);

    /* Create entity with Position and Speed component */
    EcsEntity e1 = ecs_new(world, 0);
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Speed, 2);

    /* Create entity with just a Position component */
    EcsEntity e2 = ecs_new(world, 0);
    ecs_set(world, e2, Position, {0, 0});

    /* Call ecs_progress. Both entities will be matched by the Move system */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
