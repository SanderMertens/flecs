#include <custom_pipeline.h>
#include <stdio.h>

// Custom pipelines make it possible for applications to override which systems
// are ran by a pipeline and how they are ordered. Pipelines are queries under
// the hood, and custom pipelines override the query used for system matching.

// If you only want to use custom phases in addition or in place of the builtin
// phases see the custom_phases and custom_phases_no_builtin examples, as this 
// does not require using a custom pipeline.

// Dummy system
void Sys(ecs_iter_t *it) {
    (void)it;

    printf("System ran!\n");
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Physics);

    // Create a pipeline that matches systems with Physics. Note that this
    // pipeline does not require the use of phases (see custom_phases) or of the
    // DependsOn relationship. 
    ecs_entity_t pipeline = ecs_pipeline(ecs, {
        .query.filter.terms = {
            { .id = EcsSystem }, // Mandatory, must always match systems
            { .id = Physics }
        }
    });

    // Configure the world to use the custom pipeline
    ecs_set_pipeline(ecs, pipeline);

    // Create system with Physics tag
    ECS_SYSTEM(ecs, Sys, Physics, 0);

    // Runs the pipeline & system
    ecs_progress(ecs, 0);

    return ecs_fini(ecs);

    // Output
    //  System ran!
}
