#include <custom_phases_no_builtin.h>
#include <iostream>

// This application demonstrates how to use custom phases for systems. The 
// default pipeline will automatically run systems for custom phases as long as 
// they have the flecs::Phase tag.

// Dummy system
void Sys(flecs::iter& it) {
    std::cout << "system " << it.system().name() << "\n";
}

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create three custom phases. Note that the phases have the Phase tag, 
    // which is necessary for the builtin pipeline to discover which systems it 
    // should run.
    auto Update = ecs.entity().add(flecs::Phase);
    auto Physics = ecs.entity().add(flecs::Phase).depends_on(Update);
    auto Collisions = ecs.entity().add(flecs::Phase).depends_on(Physics);

    // Create 3 dummy systems.
    ecs.system("CollisionSystem")
        .kind(Collisions)
        .iter(Sys);

    ecs.system("PhysicsSystem")
        .kind(Physics)
        .iter(Sys);

    ecs.system("GameSystem")
        .kind(flecs::OnUpdate)
        .iter(Sys);

    // Run pipeline
    ecs.progress();

    // Output
    //   system GameSystem
    //   system PhysicsSystem
    //   system CollisionSystem
}
