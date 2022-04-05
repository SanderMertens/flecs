#include <switch_case.h>
#include <stdio.h>

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

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Create a Movement state machine with 3 states
    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Standing, Walking, Running);

    // Create a Direction state machine with 4 states
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);
    ECS_TYPE(world, Direction, Front, Back, Left, Right);    

    /* Create a query that subscribers for all entities that have a Direction
     * and that are walking */
    ecs_query_t *q = ecs_query_new(world, 
        "CASE | (Movement, Walking), SWITCH | Direction");

    /* Create a few entities with various state combinations */
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_id(world, e1, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_add_id(world, e1, ECS_SWITCH | Direction);
    ecs_add_id(world, e1, ECS_CASE | Front);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_id(world, e2, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Running);
    ecs_add_id(world, e2, ECS_SWITCH | Direction);
    ecs_add_id(world, e2, ECS_CASE | Left);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_id(world, e3, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Running);
    ecs_add_id(world, e3, ECS_SWITCH | Direction);
    ecs_add_id(world, e3, ECS_CASE | Right);

    /* Add Walking to e4. This will remove the Running case */
    ecs_add_id(world, e3, ECS_CASE | Walking);

    // Iterate query as usual
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_iter_next(&it)) {
        ecs_entity_t *movement = ecs_term(&it, ecs_entity_t, 1);
        ecs_entity_t *direction = ecs_term(&it, ecs_entity_t, 2);

        for (int i = 0; i < it.count; i ++) {
            printf("%s: Movement: %s, Direction: %s\n",
                ecs_get_name(world, it.entities[i]),
                ecs_get_name(world, movement[i]),
                ecs_get_name(world, direction[i]));
        }
    }

    // Output:
    //   e3: Movement: Walking, Direction: Back
    //   e1: Movement: Walking, Direction: Front

    /* Cleanup */
    return ecs_fini(world);
}
