#include <api.h>

void Delete_w_type_delete_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_new_w_count(world, Position, 3);
    ecs_new_w_count(world, Mass, 3);

    test_int( ecs_count(world, Position), 3);
    test_int( ecs_count(world, Mass), 3);

    ecs_delete_w_type(world, Position, 0);

    test_int( ecs_count(world, Position), 0);
    test_int( ecs_count(world, Mass), 3);

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Position);
    test_int( ecs_count(world, Position), 1);

    ecs_fini(world);
}

void Delete_w_type_delete_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_new_w_count(world, Type, 3);
    ecs_new_w_count(world, Mass, 3);

    test_int( ecs_count(world, Type), 3);
    test_int( ecs_count(world, Mass), 3);

    ecs_delete_w_type(world, Type, 0);

    test_int( ecs_count(world, Type), 0);
    test_int( ecs_count(world, Mass), 3);

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type);
    test_int( ecs_count(world, Type), 1);

    ecs_fini(world);
}

void Delete_w_type_delete_1_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Position, Velocity);

    ecs_new_w_count(world, Type_1, 3);
    ecs_new_w_count(world, Type_2, 3);
    ecs_new_w_count(world, Mass, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Mass), 3);

    /* Delete both entities of Type_1 and Type_2 */
    ecs_delete_w_type(world, Type_1, 0);

    test_int( ecs_count(world, Type_1), 0);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 3);

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_2);
    test_int( ecs_count(world, Type_1), 2);
    test_int( ecs_count(world, Type_2), 1);

    ecs_fini(world);
}

void Delete_w_type_delete_2_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_new_w_count(world, Type_1, 3);
    ecs_new_w_count(world, Type_2, 3);
    ecs_new_w_count(world, Mass, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 and Type_2 */
    ecs_delete_w_type(world, Type_1, 0);

    test_int( ecs_count(world, Type_1), 0);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 3);

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_2);
    test_int( ecs_count(world, Type_1), 2);
    test_int( ecs_count(world, Type_2), 1);

    ecs_fini(world);
}

void Delete_w_type_delete_except_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Rotation);

    ecs_new_w_count(world, Type_1, 3);
    ecs_new_w_count(world, Type_2, 3);
    ecs_new_w_count(world, Type_3, 3);
    ecs_new_w_count(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 and Type_3 but not Type_3 */
    ecs_delete_w_type(world, Type_1, Mass);

    test_int( ecs_count(world, Type_1), 3); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 has Mass so not deleted */
    test_int( ecs_count(world, Type_3), 0);
    test_int( ecs_count(world, Mass), 6);

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_3);
    test_int( ecs_count(world, Type_1), 5);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 1);

    ecs_fini(world);
}

void Delete_w_type_delete_except_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Rotation);
    ECS_TYPE(world, Except, Mass, Rotation);

    ecs_new_w_count(world, Type_1, 3);
    ecs_new_w_count(world, Type_2, 3);
    ecs_new_w_count(world, Type_3, 3);
    ecs_new_w_count(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 and Type_3 but not Type_3 */
    ecs_delete_w_type(world, Type_1, Except);

    test_int( ecs_count(world, Type_1), 6); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 has Mass so not deleted */
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Mass), 6);

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    test_int( ecs_count(world, Type_1), 7);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    ecs_fini(world);
}
