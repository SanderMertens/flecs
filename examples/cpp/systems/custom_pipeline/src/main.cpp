#include <custom_pipeline.h>
#include <iostream>

// Custom pipelines make it possible for applications to override which systems
// are ran by a pipeline and how they are ordered. Pipelines are queries under
// the hood, and custom pipelines override the query used for system matching.

// If you only want to use custom phases in addition or in place of the builtin
// phases see the custom_phases and custom_phases_no_builtin examples, as this 
// does not require using a custom pipeline.

struct Physics {};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create a pipeline that matches systems with Physics. Note that this
    // pipeline does not require the use of phases (see custom_phases) or of the
    // DependsOn relationship.
    flecs::entity pipeline = ecs.pipeline()
        .with(flecs::System) // Mandatory, must always match systems
        .with<Physics>()
        .build();

    // Configure the world to use the custom pipeline
    ecs.set_pipeline(pipeline);

    // Create system with Physics tag
    ecs.system()
        .kind<Physics>()
        .iter([](flecs::iter&) {
            std::cout << "System ran!\n";
        });

    // Runs the pipeline & system
    ecs.progress();

    // Output:
    //   System ran!
}
