#include <include/families.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef int32_t Speed;

/* Move system is invoked every time ecs_progress is called */
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

/* InitObject system is invoked every time Position and Speed components are added */
void InitObject(EcsRows *rows) {
    void *row;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        Speed *s = ecs_data(rows, row, 1);
        p->x = 0;
        p->y = 0;
        *s = 1;
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position and Speed types as a components with the world */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);

    /* Create a family that includes both Position and Move component */
    ECS_FAMILY(world, Object, Position, Speed);

    /* Register the Move function as a system with the world.
     * - EcsOnFrame causes the sytem to be invoked when ecs_progress is called
     * - The system will match entities that have both Position and Speed */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Speed);

    /* Register the InitObject function as system with the world.
     * - EcsOnAdd causes the system to be invoked when a new component is added
     * - The system will match every entity that adds Position and Speed */
    ECS_SYSTEM(world, InitObject, EcsOnAdd, Position, Speed);

    /* Create a new entity with the Object family */
    ecs_new(world, Object_h);

    /* Call ecs_progress 10 times. */
    int i;
    for (i = 0; i < 10; i ++) {
        ecs_progress(world, 0);
    }

    /* Cleanup the world */
    return ecs_fini(world);
}
