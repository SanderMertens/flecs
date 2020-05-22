#include <no_macros.h>

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

/* Typedefs can be used as component types */
typedef Vector2D Position;
typedef Vector2D Velocity;

/* Implement a simple move system */
void Move(ecs_rows_t *rows) {
    /* Get the two columns from the system signature */
    Position *p = _ecs_column(rows, sizeof(Position), 1);
    Velocity *v = _ecs_column(rows, sizeof(Velocity), 2);

    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;

        /* Print something to the console so we can see the system is being
         * invoked */
        printf("%s moved to {.x = %f, .y = %f}\n",
            ecs_get_name(rows->world, rows->entities[i]),
            p[i].x, p[i].y);
    }
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Register components */
    ecs_entity_t FLECS__EPosition = ecs_new_component(world, "Position", sizeof(Position), ECS_ALIGNOF(Position));
    ecs_entity_t FLECS__EVelocity = ecs_new_component(world, "Velocity", sizeof(Velocity), ECS_ALIGNOF(Velocity));

    /* Register system */
    ecs_new_system(world, "Move", EcsOnUpdate, "Position, Velocity", Move);

    /* Create entity */
    ecs_entity_t MyEntity = ecs_new_w_type(world, 0);

    /* Set entity identifier using builtin component */
    ecs_set_ptr_w_entity(world, MyEntity, FLECS__EEcsName, sizeof(EcsName), &(EcsName){"MyEntity"});

    /* Components are automatically added when doing an ecs_set, but this is for
     * demonstration purposes. The ecs_add_entity operation accepts the
     * component identifier. Alternatively applications can use ecs_add_type,
     * which can add multiple components in one operation. */
    ecs_add_entity(world, MyEntity, FLECS__EPosition);
    ecs_add_entity(world, MyEntity, FLECS__EVelocity);

    /* Set values for entity. */
    ecs_set_ptr_w_entity(world, MyEntity, FLECS__EPosition, sizeof(Position), &(Position){0, 0});
    ecs_set_ptr_w_entity(world, MyEntity, FLECS__EVelocity, sizeof(Velocity), &(Velocity){1, 1});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application move_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
