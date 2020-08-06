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

void Switch_bulk_new_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_TYPE(world, Type, SWITCH | Movement, CASE | Running);

    ecs_entity_t e = ecs_bulk_new(world, Type, 100);
    test_assert(e != 0);

    ecs_add(world, e, Type);

    test_assert( ecs_has_entity(world, e, ECS_CASE | Running));
    ecs_entity_t case_id = ecs_get_case(world, e, Movement);
    test_int(case_id, Running);

    ecs_fini(world);
}

void Switch_3_entities_same_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_ENTITY(world, e1, SWITCH | Movement, CASE | Running);
    ECS_ENTITY(world, e2, SWITCH | Movement, CASE | Running);
    ECS_ENTITY(world, e3, SWITCH | Movement, CASE | Running);

    test_assert( ecs_has_entity(world, e1, ECS_CASE | Running));
    test_assert( ecs_has_entity(world, e2, ECS_CASE | Running));
    test_assert( ecs_has_entity(world, e3, ECS_CASE | Running));

    test_int(ecs_get_case(world, e1, Movement), Running);
    test_int(ecs_get_case(world, e2, Movement), Running);
    test_int(ecs_get_case(world, e3, Movement), Running);

    ecs_fini(world);
}

void Switch_2_entities_1_change_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_ENTITY(world, e1, SWITCH | Movement, CASE | Running);
    ECS_ENTITY(world, e2, SWITCH | Movement, CASE | Running);

    ecs_add_entity(world, e2, ECS_CASE | Jumping);
    test_assert( ecs_has_entity(world, e1, ECS_CASE | Running));
    test_assert( ecs_has_entity(world, e2, ECS_CASE | Jumping));

    test_int(ecs_get_case(world, e1, Movement), Running);
    test_int(ecs_get_case(world, e2, Movement), Jumping); 

    ecs_fini(world);
}

void Switch_3_entities_change_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_ENTITY(world, e1, SWITCH | Movement, CASE | Running);
    ECS_ENTITY(world, e2, SWITCH | Movement, CASE | Running);
    ECS_ENTITY(world, e3, SWITCH | Movement, CASE | Running);

    ecs_add_entity(world, e1, ECS_CASE | Walking);
    test_assert( ecs_has_entity(world, e1, ECS_CASE | Walking));
    test_assert( ecs_has_entity(world, e2, ECS_CASE | Running));
    test_assert( ecs_has_entity(world, e3, ECS_CASE | Running));

    test_int(ecs_get_case(world, e1, Movement), Walking);
    test_int(ecs_get_case(world, e2, Movement), Running);
    test_int(ecs_get_case(world, e3, Movement), Running);

    ecs_add_entity(world, e2, ECS_CASE | Jumping);
    test_assert( ecs_has_entity(world, e1, ECS_CASE | Walking));
    test_assert( ecs_has_entity(world, e2, ECS_CASE | Jumping));
    test_assert( ecs_has_entity(world, e3, ECS_CASE | Running));

    test_int(ecs_get_case(world, e1, Movement), Walking);
    test_int(ecs_get_case(world, e2, Movement), Jumping);
    test_int(ecs_get_case(world, e3, Movement), Running); 

    ecs_add_entity(world, e3, ECS_CASE | Walking);
    test_assert( ecs_has_entity(world, e1, ECS_CASE | Walking));
    test_assert( ecs_has_entity(world, e2, ECS_CASE | Jumping));
    test_assert( ecs_has_entity(world, e3, ECS_CASE | Walking));

    test_int(ecs_get_case(world, e1, Movement), Walking);
    test_int(ecs_get_case(world, e2, Movement), Jumping);
    test_int(ecs_get_case(world, e3, Movement), Walking);        

    ecs_fini(world);
}
