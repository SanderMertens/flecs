#include <${id underscore}.h>

#define MOVE_SPEED (5)

void MoveSquare(ecs_iter_t *it) {
    EcsInput *input = ecs_term(it, EcsInput, 1);
    EcsPosition2D *position = ecs_term(it, EcsPosition2D, 2);

    int x_v = input->keys[ECS_KEY_D].state || input->keys[ECS_KEY_RIGHT].state;
        x_v -= input->keys[ECS_KEY_A].state || input->keys[ECS_KEY_LEFT].state;
    int y_v = input->keys[ECS_KEY_S].state || input->keys[ECS_KEY_DOWN].state;
        y_v -= input->keys[ECS_KEY_W].state || input->keys[ECS_KEY_UP].state;

    position->x += x_v * MOVE_SPEED;
    position->y += y_v * MOVE_SPEED;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_IMPORT(world, FlecsComponentsTransform);
    ECS_IMPORT(world, FlecsComponentsGeometry);
    ECS_IMPORT(world, FlecsComponentsGraphics);
    ECS_IMPORT(world, FlecsComponentsInput);
    ECS_IMPORT(world, FlecsSystemsSdl2);

    /* Define entity for square */
    ECS_ENTITY(world, Square, EcsPosition2D, EcsSquare, EcsRgb);

    /* Define system that moves the square on keyboard input */
    ECS_SYSTEM(world, MoveSquare, EcsOnUpdate, EcsInput, Square.EcsPosition2D);

    /* Initialize canvas */
    ecs_set(world, 0, EcsCanvas2D, {
        .window = { .width = 800, .height = 600 }, .title = "Hello ${id}!" 
    });

    /* Initialize square */
    ecs_set(world, Square, EcsPosition2D, {0, 0});
    ecs_set(world, Square, EcsSquare, { .size = 50 });
    ecs_set(world, Square, EcsRgb, { .r = 0, .g = 0, .b = 255, .a = 255 });

    /* Enter main loop */
    ecs_set_target_fps(world, 60);

    while ( ecs_progress(world, 0));

    return ecs_fini(world);
}
