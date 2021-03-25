#include <periodic_system.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
    }
}

void PrintPosition(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("%s position = {.x = %f, .y = %f}\n",
            ecs_get_name(it->world, it->entities[i]),
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Define a system called Move that is executed every frame, and subscribes
     * for the 'Position' and 'Velocity' components */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    /* Create a system that prints the position of an entity once per second */
    ECS_SYSTEM(world, PrintPosition, EcsOnUpdate, Position);
    ecs_set_interval(world, PrintPosition, 1.0);

    /* Create an entity with Position and Velocity. Creating an entity with the
     * ECS_ENTITY macro is an easy way to create entities with multiple
     * components. Additionally, entities created with this macro can be looked
     * up by their name ('MyEntity'). */
    ECS_ENTITY(world, MyEntity, Position, Velocity);

    /* Initialize values for the entity */
    ecs_set(world, MyEntity, Position, {0, 0});
    ecs_set(world, MyEntity, Velocity, {1, 1});

    /* Run at 60FPS */
    ecs_set_target_fps(world, 60);

    printf("Application move_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
