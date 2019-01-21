#include <include/features.h>

typedef struct Position {
    float x;
    float y;
} Position;

typedef float Speed;

void Move(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_data(rows, row, 0);
        Speed *s = ecs_data(rows, row, 1);
        p->x += *s;
        p->y += *s;
        printf("Moved to %f, %f\n", p->x, p->y);
    }
}

void Drag(EcsRows *rows) {
    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Speed *s = ecs_data(rows, row, 0);
        *s *= 0.9;
        printf("Decrease speed to %f\n", *s);
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position type as a component with the world */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Speed);

    /* Register family with Position and Speed */
    ECS_FAMILY(world, Object, Position, Speed);

    /* Register the Move and Drag functions as a systems with the world.*/
    ECS_SYSTEM(world, Move, EcsOnFrame, Position, Speed);
    ECS_SYSTEM(world, Drag, EcsOnFrame, Speed);

    /* Register a feature (family of systems) */
    ECS_FAMILY(world, Movement, Move, Drag);

    /* Create entity, add & initialize Position */
    EcsEntity e = ecs_set(world, 0, Position, {0, 0});
    ecs_set(world, e, Speed, {1});

    /* Call ecs_progress. This will cause Move and Drag to be invoked. */
    ecs_progress(world, 0);

    /* Disable the Movement feature. This will disable both Move and Drag */
    ecs_enable(world, Movement_h, false);

    /* Call ecs_progress again. Notice how no systems are invoked */
    ecs_progress(world, 0);

    /* Re-enable the Movement feature. This will enable both Move and Drag */
    ecs_enable(world, Movement_h, true);

    /* Call ecs_progress again. Notice how both systems are invoked again */
    ecs_progress(world, 0);

    /* Cleanup the world. */
    return ecs_fini(world);
}
