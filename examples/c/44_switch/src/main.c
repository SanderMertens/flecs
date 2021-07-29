#include <switch.h>

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

void Walk(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    
    ecs_entity_t *m = ecs_term(it, ecs_entity_t, 1);
    ecs_entity_t *d = ecs_term(it, ecs_entity_t, 2);

    for (int i = 0; i < it->count; i ++) {
        printf("%s: Movement: %s, Direction: %s\n",
            ecs_get_name(world, it->entities[i]),
            ecs_get_name(world, m[i]),
            ecs_get_name(world, d[i]));
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    /* Create a Movement state machine with 3 states */
    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Standing, Walking, Running);

    /* Create a Direction state machine with 4 states */
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);
    ECS_TYPE(world, Direction, Front, Back, Left, Right);    

    /* Create a system that subscribers for all entities that have a Direction
     * and that are walking */
    ECS_SYSTEM(world, Walk, EcsOnUpdate, CASE | Walking, SWITCH | Direction);

    /* Create a few entities with various state combinations */
    ECS_ENTITY(world, e1, 
        SWITCH | Movement, CASE | Walking,
        SWITCH | Direction, CASE | Front);

    ECS_ENTITY(world, e2, 
        SWITCH | Movement, CASE | Running,
        SWITCH | Direction, CASE | Left);

    ECS_ENTITY(world, e3, 
        SWITCH | Movement, CASE | Running,
        SWITCH | Direction, CASE | Right);

    /* Add Walking to e4. This will remove the Running case */
    ecs_add_id(world, e3, ECS_CASE | Walking);

    /* Set target FPS for main loop */
    ecs_set_target_fps(world, 1);

    /* Run systems */
    while ( ecs_progress(world, 0));

    /* Cleanup */
    return ecs_fini(world);
}
