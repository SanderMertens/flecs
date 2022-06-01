#include <explorer.h>
#include <iostream>

using mass = flecs::units::mass;

struct Mass {
    double value;
};

int main(int argc, char *argv[]) {
    // Passing in the command line arguments will allow the explorer to display
    // the application name.
    flecs::world world(argc, argv);

    world.import<flecs::units>();
    world.import<flecs::monitor>(); // Collect statistics periodically

    // Mass component
    world.component<Mass>()
        .member<double, mass::KiloGrams>("value");

    // Simple hierarchy
    auto Sun = world.entity("Sun")
        .set<Mass>({1.988500e31});

    auto Earth = world.scope(Sun).entity("Earth")
        .set<Mass>({5.9722e24});

    world.scope(Earth).entity("Moon")
        .set<Mass>({7.34767309e22});
    
    // Run application with REST interface. When the application is running,
    // navigate to https://flecs.dev/explorer to inspect it!
    //
    // See docs/RestApi.md#explorer for more information.
    return world.app().enable_rest().run();
}
