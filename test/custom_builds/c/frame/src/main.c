#include <frame.h>

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_ftime_t dt = ecs_frame_begin(world, 0.5f);
    assert(dt > 0.4f);
    assert(dt < 0.6f);
    ecs_frame_end(world);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    assert(info->frame_count_total == 1);

    return ecs_fini(world);
}
