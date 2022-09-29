#include <addons.h>

static int dummy_frame_action(
    ecs_world_t *world,
    const ecs_app_desc_t *desc)
{
    return 10;
}

void App_app_w_frame_action() {
    ecs_world_t *world = ecs_init();

    test_int(ecs_app_set_frame_action(dummy_frame_action), 0);

    int result = ecs_app_run(world, &(ecs_app_desc_t){ 0 });
    test_int(result, 10);

    ecs_fini(world);
}

static void Sys(ecs_iter_t *it) {
    ecs_quit(it->world);
}

void App_app_w_default_frame_action() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Sys, EcsOnUpdate, 0);

    int result = ecs_app_run(world, &(ecs_app_desc_t){ 0 });
    test_int(result, 0);

    ecs_fini(world);
}

static
void Dummy(ecs_iter_t *it) {
    ecs_quit(it->world);
}

void App_app_w_set_threads() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, 0);

    test_int(ecs_get_stage_count(world), 1);

    ecs_set_threads(world, 2);

    test_int(ecs_get_stage_count(world), 2);

    ecs_app_run(world, &(ecs_app_desc_t) { 0 });

    test_int(ecs_get_stage_count(world), 2);

    ecs_fini(world);
}

void App_app_w_set_target_fps() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, 0);

    const ecs_world_info_t *info = ecs_get_world_info(world);

    test_int(info->target_fps, 0);

    ecs_set_target_fps(world, 60);

    test_int(info->target_fps, 60);

    ecs_app_run(world, &(ecs_app_desc_t) { 0 });

    test_int(info->target_fps, 60);

    ecs_fini(world);
}

static int sys_invoked = 0;

static void SysCount(ecs_iter_t *it) {
    sys_invoked ++;
}

void App_app_w_set_frames() {
    ecs_world_t *world = ecs_init();

    ECS_SYSTEM(world, SysCount, EcsOnUpdate, 0);

    ecs_app_run(world, &(ecs_app_desc_t) {
        .frames = 100
    });

    test_int(sys_invoked, 100);

    ecs_fini(world);
}
