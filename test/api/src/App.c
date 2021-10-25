#include <api.h>

static int dummy_frame_action(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    return 10;
}

void App_app_w_frame_action() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_app_set_frame_action(dummy_frame_action), 0);

    int result = ecs_app_run(world, &(ecs_app_desc_t) { });
    test_int(result, 10);
}
