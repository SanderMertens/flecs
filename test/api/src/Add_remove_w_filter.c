#include <api.h>

void Add_remove_w_filter_remove_1_no_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Mass, NULL);

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 0);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_1));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_2_no_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass, Rotation);
    ECS_TYPE(world, ToRemove, Mass, Rotation);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, ToRemove, NULL);

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 0);
    test_int( ecs_count(world, Rotation), 0);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_1));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_1_to_empty_no_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 3);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Mass, NULL);

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 3);
    test_int( ecs_count(world, Type_2), 0);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_2_to_empty_no_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass, Rotation);
    ECS_TYPE(world, ToRemove, Mass, Rotation);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 3);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, ToRemove, NULL);

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 3);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 0);
    test_int( ecs_count(world, Rotation), 0);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_1_all_no_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 3);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Type_2, NULL);

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 0);
    test_int( ecs_count(world, Type_2), 0);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == 0);
    test_assert( ecs_get_type(world, e_1 + 1) == 0);
    test_assert( ecs_get_type(world, e_1 + 2) == 0);

    ecs_fini(world);
}

void Add_remove_w_filter_remove_2_all_no_filter() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass, Rotation);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 3);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Type_2, NULL);

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 0);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 0);
    test_int( ecs_count(world, Rotation), 0);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == 0);
    test_assert( ecs_get_type(world, e_1 + 1) == 0);
    test_assert( ecs_get_type(world, e_1 + 2) == 0);

    ecs_fini(world);
}

void Add_remove_w_filter_remove_1_exclude_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Mass);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);
    ecs_entity_t e_3 = ecs_new_w_count(world, Type_3, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 6);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Mass, &(ecs_filter_t){
        .exclude = ecs_type(Velocity)
    });

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Position), 9);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_2));

    test_assert( ecs_get_type(world, e_3) == ecs_type(Position));
    test_assert( ecs_get_type(world, e_3 + 1) == ecs_type(Position));
    test_assert( ecs_get_type(world, e_3 + 2) == ecs_type(Position));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_1_exclude_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Rotation, Mass);
    ECS_TYPE(world, Type_4, Position, Mass);
    ECS_TYPE(world, Exclude, Velocity, Rotation);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);
    ecs_entity_t e_3 = ecs_new_w_count(world, Type_3, 3);
    ecs_entity_t e_4 = ecs_new_w_count(world, Type_4, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Type_4), 9);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Mass, &(ecs_filter_t){
        .exclude = ecs_type(Exclude)
    });

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Type_4), 6);
    test_int( ecs_count(world, Position), 12);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_2));

    test_assert( ecs_get_type(world, e_3) == ecs_type(Type_3));
    test_assert( ecs_get_type(world, e_3 + 1) == ecs_type(Type_3));
    test_assert( ecs_get_type(world, e_3 + 2) == ecs_type(Type_3));

    test_assert( ecs_get_type(world, e_4) == ecs_type(Position));
    test_assert( ecs_get_type(world, e_4 + 1) == ecs_type(Position));
    test_assert( ecs_get_type(world, e_4 + 2) == ecs_type(Position));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_1_include_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Mass);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);
    ecs_entity_t e_3 = ecs_new_w_count(world, Type_3, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 6);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Mass, &(ecs_filter_t){
        .include = ecs_type(Velocity)
    });

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Position), 9);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_3) == ecs_type(Type_3));
    test_assert( ecs_get_type(world, e_3 + 1) == ecs_type(Type_3));
    test_assert( ecs_get_type(world, e_3 + 2) == ecs_type(Type_3));

    ecs_fini(world);
}

void Add_remove_w_filter_remove_1_include_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Rotation, Mass);
    ECS_TYPE(world, Type_4, Position, Mass);
    ECS_TYPE(world, Type_5, Position, Rotation);
    ECS_TYPE(world, Include, Velocity, Rotation);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);
    ecs_entity_t e_3 = ecs_new_w_count(world, Type_3, 3);
    ecs_entity_t e_4 = ecs_new_w_count(world, Type_4, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Type_4), 9);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, 0, Mass, (&(ecs_filter_t){
        .include = ecs_type(Include),
        .include_kind = EcsMatchAny
    }));

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Type_3), 0);
    test_int( ecs_count(world, Type_4), 3);
    test_int( ecs_count(world, Position), 12);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_1));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_1));

    test_assert( ecs_get_type(world, e_3) == ecs_type(Type_5));
    test_assert( ecs_get_type(world, e_3 + 1) == ecs_type(Type_5));
    test_assert( ecs_get_type(world, e_3 + 2) == ecs_type(Type_5));

    test_assert( ecs_get_type(world, e_4) == ecs_type(Type_4));
    test_assert( ecs_get_type(world, e_4 + 1) == ecs_type(Type_4));
    test_assert( ecs_get_type(world, e_4 + 2) == ecs_type(Type_4));

    ecs_fini(world);
}

void Add_remove_w_filter_add_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, Mass, 0, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 6);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_2));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_2));

    ecs_fini(world);
}

void Add_remove_w_filter_add_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass, Rotation);
    ECS_TYPE(world, ToAdd, Mass, Rotation);

    ecs_entity_t e_1 = ecs_new_w_count(world, Type_1, 3);
    ecs_entity_t e_2 = ecs_new_w_count(world, Type_2, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);

    /* Remove component Mass */
    ecs_add_remove_w_filter(world, ToAdd, 0, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    /* ecs_count tests if the number of entities in the tables is correct */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 6);
    test_int( ecs_count(world, Mass), 6);
    test_int( ecs_count(world, Rotation), 6);

    /* ecs_get_type tests if the entity index is properly updated */
    test_assert( ecs_get_type(world, e_1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_1 + 1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_1 + 2) == ecs_type(Type_2));

    test_assert( ecs_get_type(world, e_2) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 1) == ecs_type(Type_2));
    test_assert( ecs_get_type(world, e_2 + 2) == ecs_type(Type_2));

    ecs_fini(world);
}
