#include <switch_case.h>
#include <iostream>

// This example shows how to use switch and case roles, which let an application
// build efficient state machines. A switch is a type with a set of tags (cases) 
// of which only one can be added to an entity at a time.
//
// When one case is added to an entity, the previous case is removed, which is
// similar to the behavior of an exclusive relationship.
//
// The benefit of the switch/case mechanism is that changing a case does not
// change the entity's table. This makes it more suitable for states that change
// often, as changing tables requires copying all of the entity's components.
//
// The tradeoff is that iterating all entities for a specific case is not as 
// fast as iterating a regular tag, as it relies on a linked list to find all
// entities within a table that have the case.
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
    ecs.type<Movement>()
        .add<Movement::Walking>()
        .add<Movement::Running>();

    ecs.type<Direction>()
        .add<Direction::Front>()
        .add<Direction::Back>()
        .add<Direction::Left>()
        .add<Direction::Right>();

    // Create a query that subscribes for all entities that have a Direction
    // and that are walking
    auto q = ecs.query_builder()
        .term<Movement, Movement::Walking>().role(flecs::Case)
        .term<Direction>().role(flecs::Switch)
        .build();

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

    // Iterate the query
    q.iter([](const flecs::iter& it) {
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

    // Output:
    //   e3: Movement: Walking, Direction: Back
    //   e1: Movement: Walking, Direction: Front

    return 0;
}
