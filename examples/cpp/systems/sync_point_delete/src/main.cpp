#include <sync_point_delete.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // This example shows how to annotate systems that delete entities, in a way
    // that allows the scheduler to to correctly insert sync points. See the
    // sync_point example for more details on sync points.
    //
    // While annotating a system for a delete operation follows the same 
    // design as other operations, one key difference is that a system often 
    // does not know which components a to be deleted entity has. This makes it
    // impossible to annotate the system in advance for specific components.
    //
    // To ensure the scheduler is still able to insert the correct sync points,
    // a system can use a wildcard to indicate that any component could be
    // modified by the system, which forces the scheduler to insert a sync.

    // Basic move system.
    ecs.system<Position, const Velocity>("Move")
        .each([](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    // Delete entities when p.x >= 3. Add wildcard annotation to indicate any
    // component could be written by the system. Position itself is added as 
    // const, since inside the system we're only reading it.
    //
    // Note how the system uses the same annotations as the sync_point example.
    ecs.system<const Position>("DeleteEntity")
        .term(flecs::Wildcard).inout(flecs::Out).set(flecs::Nothing)
        .each([](flecs::entity e, const Position& p) {
            if (p.x >= 3) {
                std::cout << "Delete entity " << e.name() << "\n";
                e.destruct();
            }
        });

    // Print resulting Position. Note that this system will never print entities
    // that have been deleted by the previous system.
    ecs.system<const Position>("PrintPosition")
        .each([](flecs::entity e, const Position& p) {
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
        });

    // Create a few test entities for a Position, Velocity query
    ecs.entity("e1")
        .set<Position>({0, 0})
        .set<Velocity>({1, 2});

    ecs.entity("e2")
        .set<Position>({1, 2})
        .set<Velocity>({1, 2});

    // Run systems. Debug logging enables us to see the generated schedule
    flecs::log::set_level(1);
    while (ecs.progress()) {
        if (!ecs.count<Position>()) {
            break; // No more entities left with Position
        }
    }
    flecs::log::set_level(-1); // Restore so we don't get world cleanup logs

    // Output:
    //  info: pipeline rebuild:
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system Move
    //  info: | | system DeleteEntity
    //  info: | | merge
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system PrintPosition
    //  info: | | merge
    //  e1: {1, 2}
    //  e2: {2, 4}
    //  Delete entity e2
    //  e1: {2, 4}
    //  Delete entity e1
    //  info: system Move deactivated
    //  info: system DeleteEntity deactivated
    //  info: system PrintPosition deactivated
    //
    // Removing the wildcard annotation from the DeleteEntity system will
    // remove the first sync point.
    //
    // Note how after both entities are deleted, all three systems are 
    // deactivated. This happens when there are no matching entities for a
    // system. A deactivated system is not ran by the scheduler, which reduces
    // overhead.
}
