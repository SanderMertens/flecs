#include <union.h>
#include <stdio.h>

// This example shows how to use union relationships. Union relationships behave
// much like exclusive relationships in that entities can have only one instance
// and that adding an instance removes the previous instance.
//
// What makes union relationships stand out is that changing the relationship
// target doesn't change the archetype of an entity. This allows for quick
// switching of tags, which can be useful when encoding state machines in ECS.
//
// There is a tradeoff, and that is that because a single archetype can contain
// entities with multiple targets, queries need to do a bit of extra work to
// only return the requested target. 

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Create a Movement state machine with 3 states
    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    // Create a Direction state machine with 4 states
    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);

    // Create a query that subscribers for all entities that have a Direction
    // and that are walking.
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {
            { .id = ecs_pair(Movement, Walking) },
            { .id = ecs_pair(Direction, EcsWildcard) }
        }
    });

    // Create a few entities with various state combinations
    ecs_entity_t e1 = ecs_new_entity(world, "e1");
    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e1, Direction, Front);

    ecs_entity_t e2 = ecs_new_entity(world, "e2");
    ecs_add_pair(world, e2, Movement, Running);
    ecs_add_pair(world, e2, Direction, Left);

    ecs_entity_t e3 = ecs_new_entity(world, "e3");
    ecs_add_pair(world, e3, Movement, Running);
    ecs_add_pair(world, e3, Direction, Right);

    // Add Walking to e3. This will remove the Running case
    ecs_add_pair(world, e3, Movement, Walking);

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

    // Cleanup
    return ecs_fini(world);
}
