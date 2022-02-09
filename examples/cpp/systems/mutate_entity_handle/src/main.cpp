#include <mutate_entity_handle.h>
#include <iostream>
#include <iomanip>

// This example is the same as the mutate_entity example, but instead stores the
// handle of the to be deleted entity in a component.

struct Timeout { 
    flecs::entity to_delete;
    double value;
};

struct Tag { };

int main(int, char *[]) {
    flecs::world ecs;

    ecs.system<Timeout>()
        .each([](flecs::iter& it, size_t, Timeout& t) {
            t.value -= static_cast<double>(it.delta_time());
            if (t.value <= 0) {
                // Delete the entity

                // To make sure the delete operation is enqueued (see 
                // mutate_entity example for more details) we need to provide it
                // with a mutable context (stage) using the mut() function. If
                // we don't provide a mutable context, the operation will be
                // attempted on the context stored in the flecs::entity object,
                // which would throw a readonly error.

                // To catch these errors at compile time, replace the type of
                // to_delete with flecs::entity_view. This class does not have 
                // any methods for mutating the entity, which forces the code to
                // first call mut().

                // The it.world() function can be used to provide the context:
                //   t.to_delete.mut(it.world()).destruct();
                // 
                // The current entity can also be used to provide context. This 
                // is useful for functions that accept a flecs::entity:
                //   t.to_delete.mut(it.entity(index)).destruct();
                //
                // A shortcut is to use the iterator directly:
                t.to_delete.mut(it).destruct();
                std::cout << "Expire: " << t.to_delete.name() << " deleted!\n";
            }
        });

    ecs.system<const Timeout>()
        .each([](const Timeout& t) {
            std::cout << "PrintExpire: " << t.to_delete.name() << " has " 
                << std::fixed << std::setprecision(2) << t.value 
                << " seconds left\n";
        });

    ecs.observer<Tag>()
        .event(flecs::OnRemove)
        .each([](flecs::entity e, Tag) {
            std::cout << "Expired: " << e.name() << " actually deleted\n";
        });

    // Add Tag so we can get notified when entity is actually deleted
    auto to_delete = ecs.entity("ToDelete")
        .add<Tag>();

    ecs.entity("MyEntity")
        .set<Timeout>({to_delete, 3.0});

    ecs.set_target_fps(1);

    while (ecs.progress()) {
        // If entity is no longer alive, exit
        if (!to_delete.is_alive()) {
            break;
        }

        std::cout << "Tick...\n";
    }

    // Output
    //  PrintExpire: ToDelete has 2.00 seconds left
    //  Tick...
    //  PrintExpire: ToDelete has 0.98 seconds left
    //  Tick...
    //  Expire: ToDelete deleted!
    //  PrintExpire: ToDelete has -0.03 seconds left
    //  Expired: ToDelete actually deleted
}
