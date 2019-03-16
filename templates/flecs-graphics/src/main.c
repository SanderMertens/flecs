#include <include/${id base}.h>

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_init();

    ECS_IMPORT(world, EcsComponentsTransform, ECS_2D);
    ECS_IMPORT(world, EcsComponentsGeometry, ECS_2D);
    ECS_IMPORT(world, EcsComponentsGraphics, ECS_2D);
    ECS_IMPORT(world, EcsSystemsSdl2, ECS_2D);

    EcsEntity canvas = ecs_set(world, 0, EcsCanvas2D, {
        .window = {
            .width = 800,
            .height = 600,
        },
        .title = "Hello ${id}"
    });

    EcsEntity square = ecs_set(world, 0, EcsPosition2D, {0, 0});

    ecs_set(world, square, EcsSquare, {
        .size = 50
    });

    ecs_set_target_fps(world, 60);

    while ( ecs_progress(world, 0));

    return ecs_fini(world);
}
