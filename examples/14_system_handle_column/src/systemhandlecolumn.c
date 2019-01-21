#include <include/systemhandlecolumn.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Speed;

void AddSpeed(EcsRows *rows) {
    /* Get the Speed handle from the HANDLE column */
    EcsEntity Speed_h = ecs_component(rows, 1);

    /* Initialize position and add Speed to every entity */
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        p->x = 0;
        p->y = 0;

        EcsEntity entity = ecs_entity(rows, row, ECS_ROW_ENTITY);

        /* ecs_set is a macro that requires the Speed_h variable. Try removing
         * the line that declares Speed_h; the code won't compile. */
        ecs_set(rows->world, entity, Speed, {1});
    }
}

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        Speed *s = ecs_data(rows, row, 1);
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

    /* Register the AddSpeed system. The ID.Object column will pass the
     * Speed handle to the system so it can be used by the callback. */
    ECS_SYSTEM(world, AddSpeed, EcsOnAdd, Position, ID.Speed);

    /* Register the Move system. */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Speed);

    /* Create entity with Position component */
    ecs_new(world, Position_h);

    /* At this point the AddSpeed system has been invoked, so our entities have
     * both Position and Speed, which will match them with the Move system. */

    /* Call ecs_progress */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
