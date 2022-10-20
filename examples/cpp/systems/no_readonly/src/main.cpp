#include <no_readonly.h>
#include <iostream>

struct Waiter { };
struct Plate { };

int main(int, char *[]) {
    flecs::world ecs;

    // Create query to find all waiters without a plate
    auto q_waiter = ecs.query_builder()
        .term<Waiter>()
        .term<Plate>(flecs::Wildcard).not_()
        .build();

    // System that assigns plates to waiter
    ecs.system("AssignPlate")
        .term<Plate>()
        .term<Waiter>(flecs::Wildcard).not_()
        .no_readonly()
        .iter([&](flecs::iter& it) {
            for (auto i : it) {
                flecs::entity plate = it.entity(i);

                // Find an available waiter
                flecs::entity waiter = q_waiter.first();
                if (waiter) {
                    // An available waiter was found, assign a plate to it so 
                    // that the next plate will no longer find it.
                    // The defer_suspend function temporarily suspends deferring 
                    // operations, which ensures that our plate is assigned 
                    // immediately. Even though this is a no_readonly system, 
                    // defering is still enabled by default, as adding/removing 
                    // components to the entities being iterated would intefere 
                    // with the system iterator.
                    it.world().defer_suspend();
                    waiter.add<Plate>(plate);
                    it.world().defer_resume();

                    // Now that defering is resumed, we can safely also add the 
                    // waiter to the plate. We can't do this while defering is 
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
