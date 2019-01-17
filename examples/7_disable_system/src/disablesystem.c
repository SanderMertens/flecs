#include <include/disablesystem.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

/* This function is invoked when the ecs_progress function is called. */
void Move(EcsRows *rows) {
    void *row;

    /* This will loop over the 10 entities we created in the main function. */
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        p->x += 1;
        p->y += 2;
        printf("Moved to %d, %d\n", p->x, p->y);
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position type as a component with the world */
    ECS_COMPONENT(world, Position);

    /* Register the Move function as a system with the world.
     * - EcsOnFrame causes the sytem to be invoked when ecs_progress is called
     * - The system will match every entity that has the Position component */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position);

    /* Create entity, add & initialize Position */
    EcsEntity e = ecs_new(world, 0);
    ecs_set(world, e, Position, {0, 0});

    /* Call ecs_progress. This will cause the Move system to be invoked. */
    ecs_progress(world, 0);

    /* Disable the move system */
    ecs_enable(world, Move_h, false);

    /* Call ecs_progress again. Notice how the Move system is not invoked */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
