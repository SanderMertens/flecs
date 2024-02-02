#include <time_interval.h>
#include <stdio.h>

// This example shows how to run a system at a specified time interval.

void Tick(ecs_iter_t *it) {
    printf("%s\n", ecs_get_name(it->world, it->system));
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create two systems that are running at different time intervals
    ecs_system(ecs, {
        .entity = ecs_entity(ecs, { 
            .name = "Tick",
            .add = { ecs_dependson(EcsOnUpdate) } // run in OnUpdate phase
        }),
        .callback = Tick,
        .interval = 1.0  // time in seconds
    });

    ecs_system(ecs, {
        .entity = ecs_entity(ecs, { 
            .name = "FastTick",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .callback = Tick,
        .interval = 0.5
    });

    // Run the main loop at 60 FPS
    ecs_set_target_fps(ecs, 60);

    while (ecs_progress(ecs, 0)) { }

    return ecs_fini(ecs);

    // Output
    //  FastTick
    //  Tick
    //  FastTick
    //  FastTick
    //  Tick
    //  FastTick
    //  FastTick
    //  Tick
    //  FastTick
    //  FastTick
    //  ...
}
