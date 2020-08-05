#include <api.h>

void Switch_get_case_empty() {
    ecs_world_t *world = ecs_init();

    ECS_TYPE(world, Type, 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t case_id = ecs_get_case(world, e, Type);
    test_int(case_id, 0);

    ecs_fini(world);
}

void Switch_get_case_no_switch() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, 0);    

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    ecs_entity_t case_id = ecs_get_case(world, e, Type);
    test_int(case_id, 0);

    ecs_fini(world);
}

void Switch_get_case_unset() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Type, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_entity_t case_id = ecs_get_case(world, e, Type);
    test_int(case_id, 0);

    ecs_fini(world);
}

void Switch_get_case_set() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Type, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_add_entity(world, e, ECS_CASE | Walking);
    test_assert( ecs_has_entity(world, e, ECS_CASE | Walking));

    ecs_entity_t case_id = ecs_get_case(world, e, Type);
    test_int(case_id, Walking);

    ecs_fini(world);
}

void Switch_get_case_change() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Type, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_add_entity(world, e, ECS_CASE | Walking);
    test_assert( ecs_has_entity(world, e, ECS_CASE | Walking));

    ecs_add_entity(world, e, ECS_CASE | Running);
    test_assert( !ecs_has_entity(world, e, ECS_CASE | Walking));
    test_assert( ecs_has_entity(world, e, ECS_CASE | Running));

    ecs_entity_t case_id = ecs_get_case(world, e, Type);
    test_int(case_id, Running);

    ecs_fini(world);
}

void Switch_new_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_TYPE(world, Type, SWITCH | Movement, CASE | Running);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);

    test_assert( ecs_has_entity(world, e, ECS_CASE | Running));
    ecs_entity_t case_id = ecs_get_case(world, e, Movement);
    test_int(case_id, Running);

    ecs_fini(world);
}

void Switch_add_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_TYPE(world, Type, SWITCH | Movement, CASE | Running);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);

    test_assert( ecs_has_entity(world, e, ECS_CASE | Running));
    ecs_entity_t case_id = ecs_get_case(world, e, Movement);
    test_int(case_id, Running);

    ecs_fini(world);
}

