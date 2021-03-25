#include <delta_time.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

void Move(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);
    Velocity *v = ecs_term(it, Velocity, 2);

    for (int i = 0; i < it->count; i ++) {
        /* Use delta_time to update the entity proportionally to the amount of
         * time that has passed inbetween frames. */
        p[i].x += v[i].x * (double)it->delta_time;
        p[i].y += v[i].y * (double)it->delta_time;

        printf("%s moved to {.x = %f, .y = %f}\n",
            ecs_get_name(it->world, it->entities[i]),
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
