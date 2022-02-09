#include <mutate_entity.h>
#include <iostream>
#include <iomanip>

struct Timeout { 
    double value;
};

int main(int, char *[]) {
    flecs::world ecs;

    // System that deletes an entity after a timeout expires
    ecs.system<Timeout>()
        .each([](flecs::iter& it, size_t index, Timeout& t) {
            t.value -= static_cast<double>(it.delta_time());
            if (t.value <= 0) {
                // Delete the entity

                // To make sure that the storage doesn't change while a system
                // is iterating entities, and multiple threads can safely access
                // the data, mutations (like a delete) are added to a command
                // queue and executed when it's safe to do so.

                // When the entity to be mutated is not the same as the entity
                // provided by the system, an additional mut() call is required.
                // See the mutate_entity_handle example.
                auto e = it.entity(index);
                e.destruct();
                std::cout << "Expire: " << e.name() << "deleted!\n";
            }
        });

    // System that prints remaining expiry time
    ecs.system<const Timeout>()
        .each([](flecs::entity e, const Timeout& t) {
            std::cout << "PrintExpire: " << e.name() << " has " << std::fixed
                << std::setprecision(2) << t.value << " seconds left\n";
        });

    // Observer that triggers when entity is actually deleted
    ecs.observer<Timeout>()
        .event(flecs::OnRemove)
        .each([](flecs::entity e, Timeout&) {
            std::cout << "Expired: " << e.name() << " actually deleted\n";
        });


    auto e = ecs.entity("MyEntity")
        .set<Timeout>({3.0});

    ecs.set_target_fps(1);

    while (ecs.progress()) {
        // If entity is no longer alive, exit
        if (!e.is_alive()) {
            break;
        }

        std::cout << "Tick...\n";
    }

    // Output
    //   PrintExpire: MyEntity has 2.00 seconds left
    //   Tick...
    //   PrintExpire: MyEntity has 0.99 seconds left
    //   Tick...
    //   Expire: MyEntitydeleted!
    //   PrintExpire: MyEntity has -0.02 seconds left
    //   Expired: MyEntity actually deleted
}
