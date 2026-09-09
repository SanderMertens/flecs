#include <flecs.h>
#include <emscripten.h>

static void playground_frame(void *world) {
    ecs_progress(world, 0);
}

static int playground_run(ecs_world_t *world, ecs_app_desc_t *desc) {
    (void)desc;
    emscripten_set_main_loop_arg(playground_frame, world, 0, false);
    return 0;
}

int main(void) {
    ecs_world_t *world = ecs_init();

    ECS_IMPORT(world, FlecsUnits);
    ECS_IMPORT(world, FlecsScriptMath);
    ECS_IMPORT(world, FlecsScriptPlatform);
    ECS_IMPORT(world, FlecsAlerts);
    ECS_IMPORT(world, FlecsMetrics);

    ecs_script(world, {
        .entity = ecs_entity(world, { .name = "playground" }),
        .code = ""
    });

    ecs_app_set_run_action(playground_run);

    return ecs_app_run(world, &(ecs_app_desc_t){
        .enable_rest = true,
        .enable_stats = true
    });
}
