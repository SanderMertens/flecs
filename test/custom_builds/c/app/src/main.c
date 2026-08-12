#include <app.h>

static int frame_count = 0;

static void CountFrames(ecs_iter_t *it) {
    (void)it;
    frame_count ++;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ecs_system(world, {
        .phase = EcsOnUpdate,
        .callback = CountFrames
    });

    int result = ecs_app_run(world, &(ecs_app_desc_t){
        .frames = 2
    });

    assert(frame_count == 2);

    return result;
}
