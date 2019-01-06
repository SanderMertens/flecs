#include <include/worlduserdata.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

/* This function is invoked when the ecs_progress function is called. */
void Move(EcsRows *rows) {
    void *row;

    /* Obtain world context, increase count */
    uint32_t *count = ecs_get_context(rows->world);
    (*count) ++;

    /* This will loop over the 10 entities we created in the main function. */
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {
        Position *p = ecs_column(rows, row, 0);
        p->x += 1;
        p->y += 2;
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position type as a component with the world */
    ECS_COMPONENT(world, Position);

    /* Register the Move function as a system with the world */
    ECS_SYSTEM(world, Move, EcsOnFrame, Position);

    /* Create entity, add & initialize Position */
    EcsHandle e = ecs_new(world, 0);
    ecs_set(world, e, Position, {0, 0});

    /* Set world context. This data can be accessed through the world object */
    uint32_t count = 0;
    ecs_set_context(world, &count);

    /* Call ecs_progress 10 times */
    int i;
    for (i = 0; i < 10; i ++) {
        ecs_progress(world, 0);
    }

    /* Will print 10 */
    printf("count = %d\n", count);

    /* Cleanup the world. */
    return ecs_fini(world);
}
