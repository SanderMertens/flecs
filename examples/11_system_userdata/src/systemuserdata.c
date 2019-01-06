#include <include/systemuserdata.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

typedef uint32_t Counter;

void Move(EcsRows *rows) {
    /* We can obtain the Counter column outside of the loop since the component
     * is added to the system, and not on individual entities. */
    Counter *count = ecs_column(rows, NULL, 0);
    (*count) ++;

    void *row;
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_column(rows, row, 1);
        p->x += 1;
        p->y += 2;
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position type as a component with the world */
    ECS_COMPONENT(world, Position);

    /* Register the Counter type as a component with the world. We will use this
     * component as userdata for the system */
    ECS_COMPONENT(world, Counter);

    /* Register the Move system. The SYSTEM.Counter column automatically adds
     * the Counter component to the Move system. It can then be accessed in the
     * System callback just like any other column. You can add as many
     * components to a system as you like. */
    ECS_SYSTEM(world, Move, EcsOnFrame, SYSTEM.Counter, Position);

    /* Create entity, add & initialize Position */
    EcsHandle e = ecs_new(world, 0);
    ecs_set(world, e, Position, {0, 0});

    /* Set system context */
    ecs_set(world, Move_h, Counter, 0);

    /* Call ecs_progress 10 times */
    int i;
    for (i = 0; i < 10; i ++) {
        ecs_progress(world, 0);
    }

    /* Will print 10 */
    printf("count = %d\n", ecs_get(world, Move_h, Counter));

    /* Cleanup the world. */
    return ecs_fini(world);
}
