#include <immediate.h>
#include <iostream>

// When an application calls world.progress(), the world is put in readonly mode.
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

struct Waiter { };
struct Plate { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create query to find all waiters without a plate
    flecs::query<> q_waiter = ecs.query_builder()
        .with<Waiter>()
        .without<Plate>(flecs::Wildcard)
        .build();

    // System that assigns plates to waiter. By making this system immediate
    // plate assignments are assigned directly (not deferred) to waiters, which 
    // ensures that we won't assign plates to the same waiter more than once.
    ecs.system("AssignPlate")
        .with<Plate>()
        .without<Waiter>(flecs::Wildcard)
        .immediate()
        .each([&](flecs::iter& it, size_t i) {
            flecs::entity plate = it.entity(i);

            // Find an available waiter
            flecs::entity waiter = q_waiter.first();
            if (waiter) {
                // An available waiter was found, assign a plate to it so 
                // that the next plate will no longer find it.
                // The defer_suspend function temporarily suspends deferring 
                // operations, which ensures that our plate is assigned 
                // immediately. Even though this is an immediate system,
                // deferring is still enabled by default, as adding/removing
                // components to the entities being iterated would interfere
                // with the system iterator.
                it.world().defer_suspend();
                waiter.add<Plate>(plate);
                it.world().defer_resume();

                // Now that deferring is resumed, we can safely also add the
                // waiter to the plate. We can't do this while deferring is
                // suspended, because the plate is the entity we're 
                // currently iterating, and we don't want to move it to a 
                // different table while we're iterating it.
                plate.add<Waiter>(waiter);

                std::cout << "Assigned " 
                    << waiter.name() << " to " 
                    << plate.name() << "!\n";
            } else {
                // No available waiters, can't assign the plate
            }
        });

    // Create a few plates and waiters
    flecs::entity waiter_1 = ecs.entity("waiter_1").add<Waiter>();
    ecs.entity("waiter_2").add<Waiter>();
    ecs.entity("waiter_3").add<Waiter>();

    ecs.entity("plate_1").add<Plate>();
    flecs::entity plate_2 = ecs.entity("plate_2").add<Plate>();
    ecs.entity("plate_3").add<Plate>();

    waiter_1.add<Plate>(plate_2);
    plate_2.add<Waiter>(waiter_1);

    // run systems
    ecs.progress();

    // Output:
    //  Assigned waiter_3 to plate_1!
    //  Assigned waiter_2 to plate_3!
}
