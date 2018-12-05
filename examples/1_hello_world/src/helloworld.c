#include <include/helloworld.h>
#include <reflecs/reflecs.h>

typedef struct Message {
    int32_t id;
} Message;

/* This function is invoked when the ecs_progress function is called. */
void PrintMessage(EcsRows *rows) {
    void *row;

    /* Iterate over the entities that match the system. In this application
     * there will be only one. */
    for (row = rows->first; row < rows->last; row = ecs_next(rows, row)) {

        /* Obtain message component from first column */
        Message *message = ecs_column(rows, row, 0);

        /* Print a message, and increase message id */
        printf("Hello World %d\n", message->id);
        message->id ++;
    }
}

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Message type as a component with the world */
    ECS_COMPONENT(world, Message);

    /* Register the PrintMessage function as a system with the world.
     * - EcsOnFrame causes the sytem to be invoked when ecs_progress is called
     * - The system will match every entity that has the Message component */
    ECS_SYSTEM(world, PrintMessage, EcsOnFrame, Message);

    /* Create a new entity, add and initialize the Message component */
    EcsHandle e = ecs_new(world, 0);
    ecs_set(world, e, Message, {.id = 0});

    /* Call ecs_progress 10 times. This will cause the system to be invoked 10
     * times as well. Note how the system modifies the component data. */
    int i;
    for (i = 0; i < 10; i ++) {
        ecs_progress(world, 0);
    }

    /* Cleanup the world */
    return ecs_fini(world);
}
