#include <no_macros.h>

/* Component types */
typedef struct {
    double x, y;
} Position, Velocity;

/* Implement a simple move system */
void Move(ecs_iter_t *it) {
    /* Get the two columns from the system signature */
    Position *p = ecs_column_w_size(it, sizeof(Position), 1);
    Velocity *v = ecs_column_w_size(it, sizeof(Velocity), 2);

    for (int i = 0; i < it->count; i ++) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;

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
    ecs_entity_t pos_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity.name = "Position",
        .size = sizeof(Position),
        .alignment = ECS_ALIGNOF(Position)
    });

    ecs_entity_t vel_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity.name = "Velocity",
        .size = sizeof(Velocity),
        .alignment = ECS_ALIGNOF(Velocity)
    });    
    
    /* Register system */
    ecs_new_system(world, 0, "Move", EcsOnUpdate, "Position, Velocity", Move);

    /* Create entity with name MyEntity and components position & velocity */
    ecs_entity_t MyEntity = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "MyEntity",
        .add = {pos_id, vel_id}
    });

    /* Set values for entity. */
    ecs_set_ptr_w_id(world, MyEntity, pos_id, sizeof(Position), &(Position){0, 0});
    ecs_set_ptr_w_id(world, MyEntity, vel_id, sizeof(Velocity), &(Velocity){1, 1});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application move_system is running, press CTRL-C to exit...\n");

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
