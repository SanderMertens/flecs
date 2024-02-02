#include <startup_system.h>
#include <iostream>

// Startup systems are systems registered with the EcsOnStart phase, and are
// only ran during the first frame. Just like with regular phases, custom phases
// can depend on the EcsOnStart phase (see custom_phases example). Phases that 
// depend on EcsOnStart are also only ran during the first frame.
//
// Other than that, startup systems behave just like regular systems (they can
// match components, can introduce merge points), with as only exception that
// they are guaranteed to always run on the main thread. 

int main(int, char *[]) {
    flecs::world ecs;

    // Startup system
    ecs.system("Startup")
        .kind(flecs::OnStart)
        .iter([](flecs::iter& it) {
            std::cout << it.system().name() << "\n";
        });

    // Regular system
    ecs.system("Update")
        .iter([](flecs::iter& it) {
            std::cout << it.system().name() << "\n";
        });

    // First frame. This runs both the Startup and Update systems
    ecs.progress();

    // Second frame. This runs only the Update system
    ecs.progress();

    // Output
    //   Startup
    //   Update
    //   Update
}
