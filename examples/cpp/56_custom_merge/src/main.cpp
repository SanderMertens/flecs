#include <custom_merge.h>
#include <iostream>

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create three stages. 
    ecs.set_stages(3);

    // By default stages are merged automatically when calling staging_end(). 
    // This behavior can be overridden so that an application can control when
    // each stage is merged, and in what order.
    //
    // The set_automerge() function enables or disables automatic merging for
    // all current stages in the world, and sets the default for new stages.
    ecs.set_automerge(false);

    flecs::world stage_1 = ecs.get_stage(0);
    flecs::world stage_2 = ecs.get_stage(1);
    flecs::world stage_3 = ecs.get_stage(2);

    // Enable automerging just for stage 1
    stage_1.set_automerge(true);

    // Begin staging. Note that this also demonstrates a very minimal example of
    // staging, without frame_begin() and frame_end() functions. Also note that
    // we need to enable staging before we may write to a stage.
    ecs.staging_begin();

    // Create an entity in each stage
    auto e1 = stage_1.entity("e_stage_1");
    auto e2 = stage_2.entity("e_stage_2");
    auto e3 = stage_3.entity("e_stage_3");

    // End staging. This will only merge the stages for which automerging has
    // been enabled, which in this case is just stage_1.
    ecs.staging_end();

    // Show that only e1 has a name
    std::cout << "e1: \"" << e1.name() << "\", "
        << "e2: \"" << e2.name() << "\", "
        << "e3: \"" << e3.name() << "\"" << std::endl;

    // Now merge the remaining stages. We could either call merge() on the world
    // which merges all stages, or we could merge each individual stage. The
    // latter gives us more control over the order in which stages are merged.
    // Note that merging is only allowed when staging is disabled.
    stage_3.merge();
    stage_2.merge();

    // All entities are now merged
    std::cout << "e1: \"" << e1.name() << "\", "
        << "e2: \"" << e2.name() << "\", "
        << "e3: \"" << e3.name() << "\"" << std::endl;
}
