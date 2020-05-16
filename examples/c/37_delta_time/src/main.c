#include <delta_time.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D Velocity;

void Move(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    for (int i = 0; i < rows->count; i ++) {
        /* Use delta_time to update the entity proportionally to the amount of
         * time that has passed inbetween frames. */
        p[i].x += v[i].x * rows->delta_time;
        p[i].y += v[i].y * rows->delta_time;

        printf("%s moved to {.x = %f, .y = %f}\n",
            ecs_get_name(rows->world, rows->entities[i]),
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    ECS_ENTITY(world, MyEntity, Position, Velocity);

    ecs_set(world, MyEntity, Position, {0, 0});
    ecs_set(world, MyEntity, Velocity, {1, 1});

    /* Set target FPS for main loop to 60 frames per second. The specified FPS
     * is not a guarantee, which is why applications should use delta_time to
     * progress a simulation, which contains the actual time passed. */
    ecs_set_target_fps(world, 60);

    printf("Application delta_time is running, press CTRL-C to exit...\n");

    /* Run systems. The 0 is the delta_time parameter that will be used for the
     * frame. If the application provides 0, flecs will measure the delta_time
     * automatically inbetween frames.
     *
     * For deterministic simulations, applications will want to pass in a fixed
     * value that does not rely on the system clock. */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
