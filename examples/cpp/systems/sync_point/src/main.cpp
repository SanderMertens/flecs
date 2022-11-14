#include <sync_point.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    // System that sets velocity using ecs_set for entities with Position.
    // While systems are progressing, operations like ecs_set are deferred until
    // it is safe to merge. By default this merge happens at the end of the
    // frame, but we can annotate systems to give the scheduler more information
    // about what it's doing, which allows it to insert sync points earlier.
    //
    // Note that sync points are never necessary/inserted for systems that write
    // components provided by their signature, as these writes directly happen
    // in the ECS storage and are never deferred.
    //
    // .inout_none() for Position tells the scheduler that while we 
    // want to match entities with Position, we're not interested in reading or 
    // writing the component value.
    ecs.system("SetVelocity")
        .with<Position>().inout_none()
        .write<Velocity>() // Velocity is written, but shouldn't be matched
        .each([](flecs::entity e) {
            e.set<Velocity>({1, 2});
        });

    // This system reads Velocity, which causes the insertion of a sync point.
    ecs.system<Position, const Velocity>("Move")
        .each([](Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
        });

    // Print resulting Position
    ecs.system<const Position>("PrintPosition")
        .each([](flecs::entity e, const Position& p) {
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
        });

    // Create a few test entities for a Position, Velocity query
    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity("e2")
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    // Run systems. Debug logging enables us to see the generated schedule
    flecs::log::set_level(1);
    ecs.progress();
    flecs::log::set_level(-1); // Restore so we don't get world cleanup logs

    // Output:
    //  info: pipeline rebuild:
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system SetVelocity
    //  info: | | merge
    //  info: | schedule: threading: 0, staging: 1:
    //  info: | | system Move
    //  info: | | system PrintPosition
    //  info: | | merge
    //  e1: {11.000000, 22.000000}
    //  e2: {11.000000, 22.000000}
    //
    // The "merge" lines indicate sync points.
    //
    // Removing '.write<Velocity>()' from the system will remove the first
    // sync point from the schedule.
}
