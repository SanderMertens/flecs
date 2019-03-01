#include <include/timemgmt.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef float Speed;

/* Move system is invoked every time ecs_progress is called */
void Move(EcsRows *rows) {
    void *row;

    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        Speed *s = ecs_data(rows, row, 1);

        /* Scale progress with delta_time. This way, objects are updated at a
         * constant speed, regardless of how fast the CPU is */
        p->x += *s * rows->delta_time;
        p->y += *s * rows->delta_time;

        printf("Moved to %f, %f (delta_time = %f)\n", p->x, p->y, rows->delta_time);
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
    ECS_TYPE(world, Object, Position, Speed);

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

    /* Specify 0 for delta_time. This causes ecs_progress to automatically
     * measure the time passed since the last frame */
    ecs_progress(world, 0);

    /* Specify a custom non-zero value for delta_time. Here ecs_progress will
     * not measure time, but will pass the provided value to systems. The unit
     * of the value is seconds. */
    ecs_progress(world, 0.5);
    ecs_progress(world, 1.0);

    /* Cleanup the world */
    return ecs_fini(world);
}
