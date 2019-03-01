#include <include/initsystem.h>

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

/* This function is invoked when a Position component is added to an entity. */
void InitPosition(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        p->x += 10;
        p->y += 20;
        printf("Position initialized (%d, %d)\n", p->x, p->y);
    }
}

/* This function is invoked when a Position component is removed from an entity. */
void DeinitPosition(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        printf("Position deinitialized (%d, %d)\n", p->x, p->y);
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

    /* Register the InitPosition function as a system with the world.
     * - EcsOnAdd causes the system to be invoked when a new component is added
     * - The system will match every entity that has the Position component */
    ECS_SYSTEM(world, InitPosition, EcsOnAdd, Position);
    ECS_SYSTEM(world, DeinitPosition, EcsOnRemove, Position);

    /* Create 10 entities with the Position component. The component will be
     * automatically initialized by the InitPosition system.
     *
     * Notice the _h suffix. Every component, system and type has a unique
     * handle that must be used to refer to the entity. Variables with the _h
     * suffix contain these handles and are automatically declared. The suffix
     * ensures that there are no collisions with type- or function names. */
    ecs_new_w_count(world, Position_h, 10, NULL);

    /* Call ecs_progress. This will cause the Move system to be invoked. */
    ecs_progress(world, 0);

    /* Cleanup the world. Note how when the world is deinitialized, all
     * components are removed, and matching OnRemove systems are invoked. */
    return ecs_fini(world);
}
