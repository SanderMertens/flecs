#include <simple_system.h>

/* Component type */
typedef struct Message {
    const char *text;
} Message;

/* Must have the same name as the ECS_SYSTEM definition */
void PrintMessage(ecs_iter_t *it) {
    /* Get a pointer to the array of the first column in the system. The order
     * of columns is the same as the one provided in the system signature. */
    ECS_COLUMN(it, Message, msg, 1);
    
    /* Iterate all the messages */
    for (int i = 0; i < it->count; i ++) {
        printf("%s\n", msg[i].text);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Define component */
    ECS_COMPONENT(world, Message);

    /* Define a system called PrintMessage that is executed every frame, and
     * subscribes for the 'Message' component */
    ECS_SYSTEM(world, PrintMessage, EcsOnUpdate, Message);

    /* Create new entity, add the component to the entity */
    ecs_entity_t e = ecs_new(world, Message);
    ecs_set(world, e, Message, {.text = "Hello Flecs!"});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application simple_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
