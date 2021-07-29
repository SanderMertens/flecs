#include <switch.h>
#include <iostream>

// This example shows how to use switch and case roles, which let an application
// build efficient state machines. Without switch and case, an application would
// have to add a separate tag for each state, while ensuring that the previous
// state is removed. This is not only less usable, but also not very performant.
//
// With tags, different combinations of states can grow quickly, which can
// scatter entities across many different tables. With switches, entities with
// different states are still stored in the same table, which makes it much
// faster to change states, and improves iteration performance.
//
// Additionally, adding a case to an entity will remove the previous case, which
// makes the switch/case combination especially suited for state machihnes.
//

struct Movement {
    struct Walking { };
    struct Running { };
};

struct Direction {
    struct Front { };
    struct Back { };
    struct Left { };
    struct Right { };
};

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Register switch types so flecs knows which cases belong to which switch
    ecs.type().component<Movement>()
        .add<Movement::Walking>()
        .add<Movement::Running>();

    ecs.type().component<Direction>()
        .add<Direction::Front>()
        .add<Direction::Back>()
        .add<Direction::Left>()
        .add<Direction::Right>();

    // Create a system that subscribes for all entities that have a Direction
    // and that are walking
    ecs.system<>("Walk")
        .term<Movement::Walking>().role(flecs::Case)
        .term<Direction>().role(flecs::Switch)
        .iter([](const flecs::iter& it) {
            // Get the column with direction states. This is stored as an array
            // with identifiers to the individual states
            auto movement = it.term<flecs::entity_t>(1);
            auto direction = it.term<flecs::entity_t>(2);

            for (auto i : it) {
                // Movement will always be Walking, Direction can be any state
                std::cout << it.entity(i).name() 
                    << ": Movement: " 
                    << it.world().entity(movement[i]).name()
                    << ", Direction: "
                    << it.world().entity(direction[i]).name()
                    << std::endl;
            }
        });

    // Create a few entities with various state combinations
    ecs.entity("e1")
        .add_switch<Movement>()
            .add_case<Movement::Walking>()
        .add_switch<Direction>()
            .add_case<Direction::Front>();

    ecs.entity("e2")
        .add_switch<Movement>()
            .add_case<Movement::Running>()
        .add_switch<Direction>()
            .add_case<Direction::Left>();

    auto e3 = ecs.entity("e3")
        .add_switch<Movement>()
            .add_case<Movement::Running>()
        .add_switch<Direction>()
            .add_case<Direction::Back>();

    // Add Walking to e4. This will remove the Running case
    e3.add_case<Movement::Walking>();

    // Set target FPS for main loop
    ecs.set_target_fps(1);

    // Run system. e2 is filtered out because it's not in the Walking state
    while (ecs.progress()) { }
}
