#include <include/api.h>

void Internals_deactivate_table() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_table() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_table() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_reactive() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    /* Add entities of same type to trigger table reactivation */
    ECS_ENTITY(world, e_3, Position);
    ECS_ENTITY(world, e_4, Position);

    test_assert(true);

    ecs_fini(world);
}

void Internals_activate_deactivate_activate_other() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_SYSTEM(world, Iter, EcsOnFrame, Position);

    /* Add entities after system definition to trigger table activation */
    ECS_ENTITY(world, e_1, Position);
    ECS_ENTITY(world, e_2, Position);

    /* System is now matched with archetype of entities. Delete entities to
     * deactivate table for system */
    ecs_delete(world, e_1);
    ecs_delete(world, e_2);

    /* Add entities of different type type to trigger new table activation */
    ECS_ENTITY(world, e_3, Position, Velocity);
    ECS_ENTITY(world, e_4, Position, Velocity);

    test_assert(true);

    ecs_fini(world);
}
