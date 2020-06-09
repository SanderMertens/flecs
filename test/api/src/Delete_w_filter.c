#include <api.h>

void Delete_w_filter_delete_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_bulk_new(world, Position, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Mass, 3);

    test_int( ecs_count(world, Position), 3);
    test_int( ecs_count(world, Mass), 3);

    ecs_bulk_delete(world, &(ecs_filter_t){
        ecs_type(Position)
    });

    test_int( ecs_count(world, Position), 0);
    test_int( ecs_count(world, Mass), 3);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !!ecs_get_type(world, e2));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Position);
    test_int( ecs_count(world, Position), 1);

    ecs_fini(world);
}

void Delete_w_filter_delete_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e1 = ecs_bulk_new(world, Type, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Mass, 3);

    test_int( ecs_count(world, Type), 3);
    test_int( ecs_count(world, Mass), 3);

    ecs_bulk_delete(world, &(ecs_filter_t){
        ecs_type(Type)
    });

    test_int( ecs_count(world, Type), 0);
    test_int( ecs_count(world, Mass), 3);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !!ecs_get_type(world, e2));    

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type);
    test_int( ecs_count(world, Type), 1);

    ecs_fini(world);
}

void Delete_w_filter_delete_1_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Position, Velocity);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Mass), 3);

    /* Delete both entities of Type_1 and Type_2 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        ecs_type(Type_1)
    });

    test_int( ecs_count(world, Type_1), 0);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 3);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !ecs_get_type(world, e2));
    test_assert( !!ecs_get_type(world, e3));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_2);
    test_int( ecs_count(world, Type_1), 2);
    test_int( ecs_count(world, Type_2), 1);

    ecs_fini(world);
}

void Delete_w_filter_delete_2_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_1), 6);
    test_int( ecs_count(world, Type_2), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 and Type_2 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        ecs_type(Type_1)
    });

    test_int( ecs_count(world, Type_1), 0);
    test_int( ecs_count(world, Type_2), 0);
    test_int( ecs_count(world, Mass), 3);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !ecs_get_type(world, e2));
    test_assert( !!ecs_get_type(world, e3));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_2);
    test_int( ecs_count(world, Type_1), 2);
    test_int( ecs_count(world, Type_2), 1);

    ecs_fini(world);
}

void Delete_w_filter_delete_except_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Rotation);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Type_3, 3);
    ecs_entity_t e4 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 and Type_3 but not Type_3 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        .include = ecs_type(Type_1),
        .exclude = ecs_type(Mass)
    });

    test_int( ecs_count(world, Type_1), 3); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 has Mass so not deleted */
    test_int( ecs_count(world, Type_3), 0);
    test_int( ecs_count(world, Mass), 6);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !!ecs_get_type(world, e2));
    test_assert( !ecs_get_type(world, e3));
    test_assert( !!ecs_get_type(world, e4));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_3);
    test_int( ecs_count(world, Type_1), 5);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 1);

    ecs_fini(world);
}

void Delete_w_filter_delete_except_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Rotation);
    ECS_TYPE(world, Except, Mass, Rotation);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Type_3, 3);
    ecs_entity_t e4 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        .include = ecs_type(Type_1),
        .exclude = ecs_type(Except)
    });

    test_int( ecs_count(world, Type_1), 6); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 has Mass so not deleted */
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Mass), 6);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !!ecs_get_type(world, e2));
    test_assert( !!ecs_get_type(world, e3));
    test_assert( !!ecs_get_type(world, e4));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    test_int( ecs_count(world, Type_1), 7);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    ecs_fini(world);
}

void Delete_w_filter_delete_with_any_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Rotation);
    ECS_TYPE(world, Include, Mass, Rotation);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Type_3, 3);
    ecs_entity_t e4 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 3);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 */
    test_int( ecs_count(world, Mass), 6);

    /* Delete both entities of Type_1 and Type_3 but not Type_3 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        .include = ecs_type(Include),
        .include_kind = EcsMatchAny
    });

    test_int( ecs_count(world, Type_1), 3); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 0); /* Type_2 has Mass so not deleted */
    test_int( ecs_count(world, Type_3), 0);
    test_int( ecs_count(world, Mass), 0);

    test_assert( !!ecs_get_type(world, e1));
    test_assert( !ecs_get_type(world, e2));
    test_assert( !ecs_get_type(world, e3));
    test_assert( !ecs_get_type(world, e4));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_2);
    ecs_new(world, Type_3);
    ecs_new(world, Mass);
    test_int( ecs_count(world, Type_1), 5);
    test_int( ecs_count(world, Type_2), 1);
    test_int( ecs_count(world, Type_3), 1);
    test_int( ecs_count(world, Mass), 2);

    ecs_fini(world);
}

void Delete_w_filter_delete_except_all_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Mass, Rotation);
    ECS_TYPE(world, Exclude, Mass, Rotation);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Type_3, 3);
    ecs_entity_t e4 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 6);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 and Type_3 */
    test_int( ecs_count(world, Mass), 9);

    /* Delete everything but Type_3 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        .exclude = ecs_type(Exclude),
        .exclude_kind = EcsMatchAll
    });

    test_int( ecs_count(world, Type_1), 3); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 is superset of Type_3 */
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Mass), 3);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !ecs_get_type(world, e2));
    test_assert( !!ecs_get_type(world, e3));
    test_assert( !ecs_get_type(world, e4));    

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_2);
    ecs_new(world, Mass);

    test_int( ecs_count(world, Type_1), 5);
    test_int( ecs_count(world, Type_2), 4);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Mass), 5);

    ecs_fini(world);
}

void Delete_w_filter_include_exact() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Mass, Rotation);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Type_3, 3);
    ecs_entity_t e4 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 6);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 and Type_3 */
    test_int( ecs_count(world, Mass), 9);

    /* Delete only Type_2 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        .include = ecs_type(Type_2),
        .include_kind = EcsMatchExact
    });

    test_int( ecs_count(world, Type_1), 6); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 is superset of Type_3 */
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Mass), 6);

    test_assert( !!ecs_get_type(world, e1));
    test_assert( !ecs_get_type(world, e2));
    test_assert( !!ecs_get_type(world, e3));
    test_assert( !!ecs_get_type(world, e4));       

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_2);

    test_int( ecs_count(world, Type_1), 7);
    test_int( ecs_count(world, Type_2), 4);
    test_int( ecs_count(world, Type_3), 3);
    test_int( ecs_count(world, Mass), 7);

    ecs_fini(world);
}

void Delete_w_filter_exclude_exact() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Position, Velocity, Mass);
    ECS_TYPE(world, Type_3, Position, Velocity, Mass, Rotation);

    ecs_entity_t e1 = ecs_bulk_new(world, Type_1, 3);
    ecs_entity_t e2 = ecs_bulk_new(world, Type_2, 3);
    ecs_entity_t e3 = ecs_bulk_new(world, Type_3, 3);
    ecs_entity_t e4 = ecs_bulk_new(world, Mass, 3);

    /* Type_1 is superset of Type_2 and Type_3 */
    test_int( ecs_count(world, Type_1), 9);
    test_int( ecs_count(world, Type_2), 6);
    test_int( ecs_count(world, Type_3), 3);

    /* Mass is also in Type_2 and Type_3 */
    test_int( ecs_count(world, Mass), 9);

    /* Delete everything but Type_2 */
    ecs_bulk_delete(world, &(ecs_filter_t){
        .exclude = ecs_type(Type_2),
        .exclude_kind = EcsMatchExact
    });

    test_int( ecs_count(world, Type_1), 3); /* Type_1 is superset of Type_2 */
    test_int( ecs_count(world, Type_2), 3); /* Type_2 is superset of Type_3 */
    test_int( ecs_count(world, Type_3), 0);
    test_int( ecs_count(world, Mass), 3);

    test_assert( !ecs_get_type(world, e1));
    test_assert( !!ecs_get_type(world, e2));
    test_assert( !ecs_get_type(world, e3));
    test_assert( !ecs_get_type(world, e4));    

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Type_1);
    ecs_new(world, Type_3);

    test_int( ecs_count(world, Type_1), 5);
    test_int( ecs_count(world, Type_2), 4);
    test_int( ecs_count(world, Type_3), 1);
    test_int( ecs_count(world, Mass), 4);

    ecs_fini(world);
}

static bool invoked = false;

static
void Dummy(ecs_iter_t *it) {
    invoked = true;
}

void Delete_w_filter_system_activate_test() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_entity_t e1 = ecs_bulk_new(world, Position, 3);

    test_int( ecs_count(world, Position), 3);

    ecs_bulk_delete(world, &(ecs_filter_t){
        ecs_type(Position)
    });

    test_int( ecs_count(world, Position), 0);
    test_assert( !ecs_get_type(world, e1));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Position);
    test_int( ecs_count(world, Position), 1);

    /* Test if system is properly reactivated */
    ecs_progress(world, 0);
    test_bool(invoked, true);

    ecs_fini(world);
}


void Delete_w_filter_skip_builtin_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Dummy, EcsOnUpdate, Position);

    ecs_entity_t e1 = ecs_bulk_new(world, Position, 3);

    test_int( ecs_count(world, Position), 3);

    ecs_bulk_delete(world, NULL);

    test_int( ecs_count(world, Position), 0);
    test_assert( !ecs_get_type(world, e1));

    test_assert(!!ecs_get_type(world, ecs_entity(Position)));
    test_assert(!!ecs_get_type(world, Dummy));

    /* Test if table is left in a state that can be repopulated */
    ecs_new(world, Position);
    test_int( ecs_count(world, Position), 1);

    /* Test if system is properly reactivated */
    ecs_progress(world, 0);
    test_bool(invoked, true);

    ecs_fini(world);
}