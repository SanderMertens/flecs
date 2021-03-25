#include <switch.h>
#include <iostream>

/* This example shows how to use switch and case roles, which let an application
 * build efficient state machines. Without switch and case, an application would
 * have to add a separate tag for each state, while ensuring that the previous
 * state is removed. This is not only less usable, but also not very performant.
 *
 * With tags, different combinations of states can grow quickly, which can
 * scatter entities across many different tables. With switches, entities with
 * different states are still stored in the same table, which makes it much
 * faster to change states, and improves iteration performance.
 *
 * Additionally, adding a case to an entity will remove the previous case, which
 * makes the switch/case combination especially suited for state machihnes.
 */

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    /* Create a Movement state machine with 3 states */
    auto Walking = ecs.entity("Walking");
    auto Running = ecs.entity("Running");
    auto Movement = ecs.type("Movement", 
        "Walking, Running");

    /* Create a Direction state machine with 4 states */
    auto Front = ecs.entity("Front");
    auto Back = ecs.entity("Back");
    auto Left = ecs.entity("Left");
    auto Right = ecs.entity("Right");
    auto Direction = ecs.type("Direction", 
        "Front, Back, Left, Right");

    /* Create a system that subscribes for all entities that have a Direction
     * and that are walking */
    ecs.system<>("Walk", "CASE | Walking, SWITCH | Direction")
        .iter([](const flecs::iter& it) {
            /* Get the column with direction states. This is stored as an array
             * with identifiers to the individual states */
            auto movement = it.term<flecs::entity_t>(1);
            auto direction = it.term<flecs::entity_t>(2);

            for (auto i : it) {
                std::cout << it.entity(i).name() 
                    << ": Movement: " 
                    << flecs::entity(it.world(), movement[i]).name()
                    << ", Direction: "
                    << flecs::entity(it.world(), direction[i]).name()
                    << std::endl;
            }
        });

    /* Create a few entities with various state combinations */
    ecs.entity("e1")
        .add_switch(Movement)
            .add_case(Walking)
        .add_switch(Direction)
            .add_case(Front);

    ecs.entity("e2")
        .add_switch(Movement)
            .add_case(Running)
        .add_switch(Direction)
            .add_case(Left);

    ecs.entity("e3")
        .add_switch(Movement)
            .add_case(Walking)
        .add_switch(Direction)
            .add_case(Right);

    auto e4 = ecs.entity("e4")
        .add_switch(Movement)
            .add_case(Running)
        .add_switch(Direction)
            .add_case(Back);

    /* Add Walking to e4. This will remove the Running case */
    e4.add_case(Walking);

    /* Set target FPS for main loop */
    ecs.set_target_fps(1);

    while (ecs.progress()) { }
}
