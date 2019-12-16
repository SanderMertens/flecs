#include <inheritance.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Force;
typedef float Mass;

/* Implement a simple move system */
void Move(ecs_rows_t *rows) {
    /* Get the two columns from the system signature */
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Force, v, 2);
    ECS_COLUMN(rows, Mass, m, 3);

    for (int i = 0; i < rows->count; i ++) {
        if (ecs_is_shared(rows, 3)) {
            p[i].x += v[i].x / m[0];
            p[i].y += v[i].y / m[0];
        } else {
            p[i].x += v[i].x / m[i];
            p[i].y += v[i].y / m[i];
        }

        /* Print something to the console so we can see the system is being
         * invoked */
        printf("%s moved to {.x = %f, .y = %f}\n",
            ecs_get_id(rows->world, rows->entities[i]),
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Force);
    ECS_COMPONENT(world, Mass);

    /* Define a system called Move that is executed every frame, and subscribes
     * for the 'Position', 'Force' and 'Mass' components. The Mass component
     * will be either shared or owned. */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Force, Mass);

    /* Create two base entities */
    ECS_ENTITY(world, HeavyEntity, Mass);
    ecs_set(world, HeavyEntity, Mass, {100});

    ECS_ENTITY(world, LightEntity, Mass);
    ecs_set(world, LightEntity, Mass, {10});

    /* Create regular entity with Position, Force and Mass */
    ECS_ENTITY(world, Instance0, Position, Force, Mass);
    ecs_set(world, Instance0, Position, {0, 0});
    ecs_set(world, Instance0, Force, {10, 10});
    ecs_set(world, Instance0, Mass, {2});

    /* Create instances which share the Mass component from a base */
    ECS_ENTITY(world, Instance1, Position, Force, INSTANCEOF | LightEntity);
    ecs_set(world, Instance1, Position, {0, 0});
    ecs_set(world, Instance1, Force, {10, 10});

    ECS_ENTITY(world, Instance2, Position, Force, INSTANCEOF | LightEntity);
    ecs_set(world, Instance2, Position, {0, 0});
    ecs_set(world, Instance2, Force, {10, 10});

    ECS_ENTITY(world, Instance3, Position, Force, INSTANCEOF | HeavyEntity);
    ecs_set(world, Instance3, Position, {0, 0});
    ecs_set(world, Instance3, Force, {10, 10});

    ECS_ENTITY(world, Instance4, Position, Force, INSTANCEOF | HeavyEntity);
    ecs_set(world, Instance4, Position, {0, 0});
    ecs_set(world, Instance4, Force, {10, 10});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application move_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0)) {
        printf("-----\n");
    }

    /* Cleanup */
    return ecs_fini(world);
}
