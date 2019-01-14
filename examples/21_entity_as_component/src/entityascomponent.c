#include <include/entityascomponent.h>

typedef struct Position {
    int32_t x;
    int32_t y;
} Position;

int main(int argc, char *argv[]) {
    /* An EcsWorld contains all our entities, components and systems */
    EcsWorld *world = ecs_init();

    /* Register the Position type as a component with the world */
    ECS_COMPONENT(world, Position);

    /* Create a new entity, add a regular component */
    EcsHandle e = ecs_new(world, 0);
    ecs_stage_add(world, e, Position_h);
    ecs_commit(world, e);

    /* The above code is straightforward- it adds the Position component to the
     * new (empty) entity. Something you might've noticed is that we store
     * component handles (Position_h) and entity handles (e) with the same
     * datatype (EcsHandle). This is because under the hood, reflecs stores
     * components also as entities.
     *
     * What then, allows a component to be added to an entity? Component
     * entities have a special, builtin component called EcsComponent. Consider
     * this code: */
    printf("e has EcsComponent: %s\n",
        ecs_has(world, e, EcsComponent_h) ? "true" : "false");

    printf("Position has EcsComponent: %s\n",
        ecs_has(world, Position_h, EcsComponent_h) ? "true" : "false");

    /* You'll notice that 'e' does not have EcsComponent, while Position_h does
     * have it. Adding an entity that does not have this component to another
     * entity will result in an error (there are two exceptions to that rule:
     * prefabs and families may also be added with ecs_stage_add).
     *
     * From this follows that if we add EcsComponent to an entity, we should be
     * able to add it to another entity. Let's try it out: */

    EcsHandle ec = ecs_new(world, 0);
    ecs_stage_add(world, ec, EcsComponent_h);
    ecs_commit(world, ec);

    /* Now we should be able to add ec to our previous entity: */
    ecs_stage_add(world, e, ec);
    ecs_commit(world, e);

    /* That worked! Now we can test if 'e' has 'ec': */
    printf("e has ec: %s\n", ecs_has(world, e, ec) ? "true" : "false");

    /* This should have returned true. The ability to add entities to entities
     * is an important primitive which allows us to subdivide entities in
     * interesting ways, like building DAGs or creating indexes on data. See
     * the demand_system_filter and dag examples for applications of this
     * capability. */

    /* Cleanup the world. */
    return ecs_fini(world);
}
