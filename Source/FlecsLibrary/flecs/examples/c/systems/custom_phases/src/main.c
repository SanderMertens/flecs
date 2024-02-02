#include <custom_phases.h>
#include <stdio.h>

// This application demonstrates how to use custom phases for systems. The 
// default pipeline will automatically run systems for custom phases as long as 
// they have the EcsPhase tag.

// Dummy system
void Sys(ecs_iter_t *it) {
    printf("system %s\n", ecs_get_name(it->world, it->system));
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Create two custom phases that branch off of EcsOnUpdate. Note that the
    // phases have the Phase tag, which is necessary for the builtin pipeline
    // to discover which systems it should run.
    ecs_entity_t Physics = ecs_new_w_id(ecs, EcsPhase);
    ecs_entity_t Collisions = ecs_new_w_id(ecs, EcsPhase);
    ecs_add_pair(ecs, Physics, EcsDependsOn, EcsOnUpdate);
    ecs_add_pair(ecs, Collisions, EcsDependsOn, Physics);

    // Create 3 dummy systems.
    ecs_system(ecs, {
        .entity = ecs_entity(ecs, {
            .name = "CollisionSystem",
            .add = { ecs_dependson(Collisions) }
        }),
        .callback = Sys
    });

    ecs_system(ecs, {
        .entity = ecs_entity(ecs, {
            .name = "PhysicsSystem",
            .add = { ecs_dependson(Physics) }
        }),
        .callback = Sys
    });

    ecs_system(ecs, {
        .entity = ecs_entity(ecs, {
            .name = "GameSystem",
            .add = { ecs_dependson(EcsOnUpdate) }
        }),
        .callback = Sys
    });

    // Run pipeline
    ecs_progress(ecs, 0);

    return ecs_fini(ecs);

    // Output
    //   system GameSystem
    //   system PhysicsSystem
    //   system CollisionSystem
}
