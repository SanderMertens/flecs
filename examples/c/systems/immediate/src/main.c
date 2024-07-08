#include <immediate.h>
#include <stdio.h>

// When an application calls ecs_progress(), the world is put in readonly mode.
// This ensures that systems (on multiple threads) can safely iterate 
// components, without having to worry about components moving around while 
// they're being read. This has as side effect that any operations (like adding
// or removing components) are not visible until the end of the frame (see the
// sync_point example for more details).
// Sometimes this is not what you want, and you need a change to be visible
// immediately. For these use cases, applications can use an immediate system.
// This temporarily takes the world out of readonly mode, so a system can make
// changes that are directly visible.
// Because they mutate the world directly, immediate systems are never ran on
// more than one thread, and no other systems are ran at the same time.

ECS_DECLARE(Waiter);
ECS_DECLARE(Plate);

// System that assigns plates to waiter. By making this system immediate
// plate assignments are assigned directly (not deferred) to waiters, which 
// ensures that we won't assign plates to the same waiter more than once.
void AssignPlate(ecs_iter_t *it) {
    ecs_world_t *ecs = it->world;
    ecs_query_t *q_waiter = it->ctx; // Get query from system context

    // Iterate unassigned plates
    for (int i = 0; i < it->count; i ++) {
        ecs_entity_t plate = it->entities[i];

        // Find an available waiter
        ecs_iter_t qit = ecs_query_iter(ecs, q_waiter);
        ecs_entity_t waiter = ecs_iter_first(&qit); // Get first matching entity
        if (waiter) {
            // An available waiter was found, assign a plate to it so that the
            // next plate will no longer find it.
            // The defer_suspend function temporarily suspends deferring 
            // operations, which ensures that our plate is assigned immediately.
            // Even though this is an immediate system, deferring is still
            // enabled by default, as adding/removing components to the entities
            // being iterated would interfere with the system iterator.
            ecs_defer_suspend(ecs);
            ecs_add_pair(ecs, waiter, Plate, plate);
            ecs_defer_resume(ecs);

            // Now that deferring is resumed, we can safely also add the waiter
            // to the plate. We can't do this while deferring is suspended,
            // because the plate is the entity we're currently iterating, and
            // we don't want to move it to a different table while we're 
            // iterating it.
            ecs_add_pair(ecs, plate, Waiter, waiter);

            printf("Assigned %s to %s!\n",
                ecs_get_name(ecs, waiter),
                ecs_get_name(ecs, plate));

            // Note that we use defer suspend/resume here instead of end/begin.
            // This ensures that we don't merge the enqueued commands for the
            // plates, which shouldn't be merged before we're done iterating.
        } else {
            // No available waiters, can't assign the plate
        }
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG_DEFINE(ecs, Waiter);
    ECS_TAG_DEFINE(ecs, Plate);

    // Create query to find all waiters without a plate
    ecs_query_t *q_waiter = ecs_query(ecs, {
        .terms = {
            { .id = Waiter },
            { .id = ecs_pair(Plate, EcsWildcard), .oper = EcsNot }
        }
    });

    // Find all plates without a Waiter
    ecs_system(ecs, {
        .entity = ecs_entity(ecs, {
            .name = "AssignPlate",
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { .id = Plate },
            { .id = ecs_pair(Waiter, EcsWildcard), .oper = EcsNot }
        },
        .callback = AssignPlate,
        .ctx = q_waiter,
        .immediate = true // disable readonly mode for this system
    });

    // Create a few plates and waiters
    ecs_entity_t waiter_1 = ecs_entity(ecs, {
        .name = "waiter_1",
        .add = ecs_ids( Waiter )
    });
    /* ecs_entity_t waiter_2 = */ ecs_entity(ecs, { // silence unused warning
        .name = "waiter_2",
        .add = ecs_ids( Waiter )
    });
    /* ecs_entity_t waiter_3 = */ ecs_entity(ecs, {
        .name = "waiter_3",
        .add = ecs_ids( Waiter )
    });

    /* ecs_entity_t plate_1 = */ ecs_entity(ecs, {
        .name = "plate_1",
        .add = ecs_ids( Plate )
    });
    ecs_entity_t plate_2 = ecs_entity(ecs, {
        .name = "plate_2",
        .add = ecs_ids( Plate, ecs_pair(Waiter, waiter_1) ) // already assigned
    });
    /* ecs_entity_t plate_3 = */ ecs_entity(ecs, {
        .name = "plate_3",
        .add = ecs_ids( Plate )
    });

    // Also assign plate_2 to waiter_1
    ecs_add_pair(ecs, waiter_1, Plate, plate_2);

    // Run the system
    ecs_progress(ecs, 0);

    return ecs_fini(ecs);
    
    // Output:
    //  Assigned waiter_3 to plate_1!
    //  Assigned waiter_2 to plate_3!
}
