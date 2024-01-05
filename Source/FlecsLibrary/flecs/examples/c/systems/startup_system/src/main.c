#include <startup_system.h>
#include <stdio.h>

// Startup systems are systems registered with the EcsOnStart phase, and are
// only ran during the first frame. Just like with regular phases, custom phases
// can depend on the EcsOnStart phase (see custom_phases example). Phases that 
// depend on EcsOnStart are also only ran during the first frame.
//
// Other than that, startup systems behave just like regular systems (they can
// match components, can introduce merge points), with as only exception that
// they are guaranteed to always run on the main thread. 

void Startup(ecs_iter_t *it) {
    printf("%s\n", ecs_get_name(it->world, it->system));
}

void Update(ecs_iter_t *it) {
    printf("%s\n", ecs_get_name(it->world, it->system));
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Startup system
    ECS_SYSTEM(ecs, Startup, EcsOnStart, 0);

    // Regular system
    ECS_SYSTEM(ecs, Update, EcsOnUpdate, 0);

    // First frame. This runs both the Startup and Update systems
    ecs_progress(ecs, 0.0f);

    // Second frame. This runs only the Update system
    ecs_progress(ecs, 0.0f);

    return ecs_fini(ecs);

    // Output
    //   Startup
    //   Update
    //   Update
}
