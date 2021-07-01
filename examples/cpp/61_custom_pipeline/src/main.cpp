#include <custom_pipeline.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world ecs;

    // Create three custom phases for the pipeline
    auto PreFrame = ecs.entity();
    auto OnFrame = ecs.entity();
    auto PostFrame = ecs.entity();

    // Create the pipeline with the three custom phases
    flecs::pipeline pip = ecs.pipeline("MyPipeline")
        .add(PreFrame)
        .add(OnFrame)
        .add(PostFrame);

    // Make the world use the custom pipeline. After this the pipeline should
    // no longer be modified.
    ecs.set_pipeline(pip);

    // Define a system for each of the phases. Define them in opposite order so
    // we can see the pipeline orders them correctly.
    ecs.system<>()
        .kind(PostFrame)
        .iter([&](flecs::iter) {
            std::cout << "- PostFrame" << std::endl;
        });

    ecs.system<>()
        .kind(OnFrame)
        .iter([&](flecs::iter) {
            std::cout << "- OnFrame" << std::endl;
        });

    ecs.system<>()
        .kind(PreFrame)
        .iter([&](flecs::iter) {
            std::cout << "- PreFrame" << std::endl;
        });

    // Run the pipeline once
    std::cout << "Begin" << std::endl;
    ecs.progress();
    std::cout << "End" << std::endl;
}
