#include <inheritance_api.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Force;
typedef float Mass;

/* Implement a simple move system */
void Move(ecs_iter_t *it) {
    /* Get the two columns from the system signature */
    ECS_COLUMN(it, Position, p, 1);
    ECS_COLUMN(it, Force, v, 2);
    ECS_COLUMN(it, Mass, m, 3);

    for (int i = 0; i < it->count; i ++) {
        if (!ecs_is_owned(it, 3)) {
            p[i].x += v[i].x / m[0];
            p[i].y += v[i].y / m[0];
        } else {
            p[i].x += v[i].x / m[i];
            p[i].y += v[i].y / m[i];
        }

        /* Print something to the console so we can see the system is being
         * invoked */
        printf("%s moved to {.x = %f, .y = %f}\n",
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
    ECS_COMPONENT(world, Force);
    ECS_COMPONENT(world, Mass);

    /* Define a system called Move that is executed every frame, and subscribes
     * for the 'Position', 'Force' and 'Mass' components. The Mass component
     * will be either shared or owned. */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Force, Mass);

    /* Create two base entities */
    ecs_entity_t HeavyEntity = ecs_new(world, 0);
    ecs_set(world, HeavyEntity, Mass, {100});

    ecs_entity_t LightEntity = ecs_new(world, 0);
    ecs_set(world, LightEntity, Mass, {10});

    /* Create regular entity with Position, Force and Mass */
    ecs_entity_t Instance0 = ecs_new(world, 0);
    ecs_set(world, Instance0, EcsName, {"Instance0"});
    ecs_set(world, Instance0, Position, {0, 0});
    ecs_set(world, Instance0, Force, {10, 10});
    ecs_set(world, Instance0, Mass, {2});

    /* Create instances which share the Mass component from a base */
    ecs_entity_t Instance1 = ecs_new_w_entity(world, ECS_INSTANCEOF | LightEntity);
    ecs_set(world, Instance1, EcsName, {"Instance1"});
    ecs_set(world, Instance1, Position, {0, 0});
    ecs_set(world, Instance1, Force, {10, 10});

    ecs_entity_t Instance2 = ecs_new_w_entity(world, ECS_INSTANCEOF | LightEntity);
    ecs_set(world, Instance2, EcsName, {"Instance2"});
    ecs_set(world, Instance2, Position, {0, 0});
    ecs_set(world, Instance2, Force, {10, 10});

    ecs_entity_t Instance3 = ecs_new_w_entity(world, ECS_INSTANCEOF | HeavyEntity);
    ecs_set(world, Instance3, EcsName, {"Instance3"});
    ecs_set(world, Instance3, Position, {0, 0});
    ecs_set(world, Instance3, Force, {10, 10});

    ecs_entity_t Instance4 = ecs_new_w_entity(world, ECS_INSTANCEOF | HeavyEntity);
    ecs_set(world, Instance4, EcsName, {"Instance4"});
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
