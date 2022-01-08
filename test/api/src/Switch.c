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

    ecs_entity_t e = ecs_new_w_id(world, ECS_SWITCH | Type);
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

    ecs_entity_t e = ecs_new_w_id(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_add_id(world, e, ECS_CASE | Walking);
    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));

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

    ecs_entity_t e = ecs_new_w_id(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_add_id(world, e, ECS_CASE | Walking);
    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));

    ecs_add_id(world, e, ECS_CASE | Running);
    test_assert( !ecs_has_id(world, e, ECS_CASE | Walking));
    test_assert( ecs_has_id(world, e, ECS_CASE | Running));

    ecs_entity_t case_id = ecs_get_case(world, e, Type);
    test_int(case_id, Running);

    ecs_fini(world);
}

void Switch_remove_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e != 0);

    ecs_add_id(world, e, ECS_CASE | Walking);

    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));
    ecs_entity_t case_id = ecs_get_case(world, e, Movement);
    test_int(case_id, Walking);

    ecs_remove_id(world, e, ECS_CASE | Walking);

    test_assert( !ecs_has_id(world, e, ECS_CASE | Walking));
    case_id = ecs_get_case(world, e, Movement);
    test_int(case_id, 0);

    ecs_fini(world);
}

void Switch_remove_last() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Walking, Running);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e1 != 0);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e2 != 0);
    ecs_add_id(world, e2, ECS_CASE | Walking);
    ecs_add(world, e2, Tag);

    ecs_remove(world, e2, Tag);

    test_assert(!ecs_has(world, e2, Tag));
    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));

    test_assert(ecs_has(world, e1, Tag));
    test_assert(ecs_has_id(world, e1, ECS_CASE | Walking));

    ecs_fini(world);
}

void Switch_delete_first() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Walking, Running);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e1 != 0);
    ecs_add_id(world, e1, ECS_CASE | Walking);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e2 != 0);
    ecs_add_id(world, e2, ECS_CASE | Walking);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e3 != 0);
    ecs_add_id(world, e3, ECS_CASE | Walking);

    ecs_delete(world, e1);

    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e3, ECS_CASE | Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_delete_last() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Walking, Running);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e1 != 0);
    ecs_add_id(world, e1, ECS_CASE | Walking);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e2 != 0);
    ecs_add_id(world, e2, ECS_CASE | Walking);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e3 != 0);
    ecs_add_id(world, e3, ECS_CASE | Walking);

    ecs_delete(world, e3);

    test_assert(ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_delete_first_last() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Walking, Running);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e1 != 0);
    ecs_add_id(world, e1, ECS_CASE | Walking);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e2 != 0);
    ecs_add_id(world, e2, ECS_CASE | Walking);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e3 != 0);
    ecs_add_id(world, e3, ECS_CASE | Walking);

    ecs_delete(world, e1);
    ecs_delete(world, e3);

    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_3_entities_same_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_ENTITY(world, e1, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, CASE | (Movement, Running));
    ECS_ENTITY(world, e3, CASE | (Movement, Running));

    test_assert( ecs_has_id(world, e1, ECS_CASE | Running));
    test_assert( ecs_has_id(world, e2, ECS_CASE | Running));
    test_assert( ecs_has_id(world, e3, ECS_CASE | Running));

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

    ECS_ENTITY(world, e1, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, CASE | (Movement, Running));

    ecs_add_id(world, e2, ECS_CASE | Jumping);
    test_assert( ecs_has_id(world, e1, ECS_CASE | Running));
    test_assert( ecs_has_id(world, e2, ECS_CASE | Jumping));

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

    ECS_ENTITY(world, e1, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, CASE | (Movement, Running));
    ECS_ENTITY(world, e3, CASE | (Movement, Running));

    ecs_add_id(world, e1, ECS_CASE | Walking);
    test_assert( ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert( ecs_has_id(world, e2, ECS_CASE | Running));
    test_assert( ecs_has_id(world, e3, ECS_CASE | Running));

    test_int(ecs_get_case(world, e1, Movement), Walking);
    test_int(ecs_get_case(world, e2, Movement), Running);
    test_int(ecs_get_case(world, e3, Movement), Running);

    ecs_add_id(world, e2, ECS_CASE | Jumping);
    test_assert( ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert( ecs_has_id(world, e2, ECS_CASE | Jumping));
    test_assert( ecs_has_id(world, e3, ECS_CASE | Running));

    test_int(ecs_get_case(world, e1, Movement), Walking);
    test_int(ecs_get_case(world, e2, Movement), Jumping);
    test_int(ecs_get_case(world, e3, Movement), Running); 

    ecs_add_id(world, e3, ECS_CASE | Walking);
    test_assert( ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert( ecs_has_id(world, e2, ECS_CASE | Jumping));
    test_assert( ecs_has_id(world, e3, ECS_CASE | Walking));

    test_int(ecs_get_case(world, e1, Movement), Walking);
    test_int(ecs_get_case(world, e2, Movement), Jumping);
    test_int(ecs_get_case(world, e3, Movement), Walking);        

    ecs_fini(world);
}

static
void MatchSwitch(ecs_iter_t *it) {
    ecs_entity_t *movement = ecs_term(it, ecs_entity_t, 1);
    test_assert(movement != NULL);
    probe_iter(it);
}

void Switch_query_switch() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, MatchSwitch, EcsOnUpdate, SWITCH | Movement);

    ECS_ENTITY(world, e1, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, CASE | (Movement, Walking));
    ECS_ENTITY(world, e3, CASE | (Movement, Running));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, MatchSwitch);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.c[0][0], ECS_SWITCH | Movement);
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Switch_query_1_case_1_type() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, MatchSwitch, EcsOnUpdate, CASE | (Movement, Running));

    ECS_ENTITY(world, e1, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, CASE | (Movement, Walking));
    ECS_ENTITY(world, e3, CASE | (Movement, Running));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, MatchSwitch);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.e[1], e1);
    test_int(ctx.c[0][0], ecs_case(Movement, Running));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Switch_query_1_case_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, MatchSwitch, EcsOnUpdate, CASE | (Movement, Running));

    ECS_ENTITY(world, e1, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, CASE | (Movement, Walking));
    ECS_ENTITY(world, e3, CASE | (Movement, Running));
    ECS_ENTITY(world, e4, CASE | (Movement, Walking), Position);
    ECS_ENTITY(world, e5, CASE | (Movement, Running), Position);
    ECS_ENTITY(world, e6, CASE | (Movement, Walking), Position);
    ECS_ENTITY(world, e7, CASE | (Movement, Running), Position);

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 4);
    test_int(ctx.system, MatchSwitch);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e3);
    test_int(ctx.e[1], e1);
    test_int(ctx.e[2], e7);
    test_int(ctx.e[3], e5);    
    test_int(ctx.c[0][0], ecs_case(Movement, Running));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Switch_query_2_cases_1_type() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);
    ECS_TYPE(world, Direction, Front, Back, Left, Right);

    ECS_SYSTEM(world, MatchSwitch, EcsOnUpdate, CASE | (Movement, Running), CASE | (Direction, Front));

    ECS_ENTITY(world, e1, 
        CASE | (Movement, Running),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e2, 
        CASE | (Movement, Walking),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e3, 
        CASE | (Movement, Running),
        CASE | (Direction, Back));
    ECS_ENTITY(world, e4, 
        CASE | (Movement, Running),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e5, 
        CASE | (Movement, Walking),
        CASE | (Direction, Front));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, MatchSwitch);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e1);
    test_int(ctx.c[0][0], ecs_case(Movement, Running));
    test_int(ctx.c[0][1], ecs_case(Direction, Front));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Switch_query_2_cases_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);
    ECS_TYPE(world, Direction, Front, Back, Left, Right);

    ECS_SYSTEM(world, MatchSwitch, EcsOnUpdate, CASE | (Movement, Running), CASE | (Direction, Front));

    ECS_ENTITY(world, e1, 
        CASE | (Movement, Running),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e2, 
        CASE | (Movement, Walking),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e3, 
        CASE | (Movement, Running),
        CASE | (Direction, Back));
    ECS_ENTITY(world, e4, 
        CASE | (Movement, Running),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e5, 
        CASE | (Movement, Walking),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e6, 
        Position,
        CASE | (Movement, Walking),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e7, 
        Position,
        CASE | (Movement, Running),
        CASE | (Direction, Front));
    ECS_ENTITY(world, e8, 
        Position,
        CASE | (Movement, Walking),
        CASE | (Direction, Front));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 1);

    test_int(ctx.count, 3);
    test_int(ctx.invoked, 3);
    test_int(ctx.system, MatchSwitch);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e1);
    test_int(ctx.e[2], e7);
    test_int(ctx.c[0][0], ecs_case(Movement, Running));
    test_int(ctx.c[0][1], ecs_case(Direction, Front));
    test_int(ctx.s[0][0], 0);

    ecs_fini(world);
}

void Switch_query_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_ENTITY(world, e1, CASE | (Movement, Walking));
    ECS_ENTITY(world, e2, CASE | (Movement, Walking));
    ECS_ENTITY(world, e3, CASE | (Movement, Running));
    ECS_ENTITY(world, e4, CASE | (Movement, Running));
    ECS_ENTITY(world, e5, CASE | (Movement, Running));
    ECS_ENTITY(world, e6, CASE | (Movement, Jumping));
    ECS_ENTITY(world, e7, CASE | (Movement, Jumping));

    ecs_query_t *q_walking = ecs_query_new(world, "CASE | (Movement, Walking)");
    ecs_query_t *q_running = ecs_query_new(world, "CASE | (Movement, Running)");
    ecs_query_t *q_jumping = ecs_query_new(world, "CASE | (Movement, Jumping)");

    /* Verify all queries are correctly matched */
    ecs_iter_t it = ecs_query_iter(world, q_walking);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e2);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e1);
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_running);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e5);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e4);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e3);    
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_jumping);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e7);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e6);
    test_assert(!ecs_query_next(&it));

    ecs_remove_id(world, e4, ECS_CASE | Running);
    test_assert(!ecs_has_id(world, e4, ECS_CASE | Running));
    ecs_entity_t c = ecs_get_case(world, e4, Movement);
    test_int(c, 0);

    /* Verify queries are still correctly matched, now excluding e4 */
    it = ecs_query_iter(world, q_walking);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e2);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e1);
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_running);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e5);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e3);    
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_jumping);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e7);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e6);
    test_assert(!ecs_query_next(&it));    

    ecs_add_id(world, e4, ECS_CASE | Running);
    test_assert(ecs_has_id(world, e4, ECS_CASE | Running));
    c = ecs_get_case(world, e4, Movement);
    test_int(c, Running);

    /* Verify e4 is now matched again */
    it = ecs_query_iter(world, q_walking);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e2);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e1);
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_running);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e4);      
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e5);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e3);      
    test_assert(!ecs_query_next(&it));

    it = ecs_query_iter(world, q_jumping);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e7);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1); test_int(it.entities[0], e6);
    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

static
void AddSwitch(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t movement = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(world, it->entities[i], ECS_SWITCH | movement);
    }
}

void Switch_add_switch_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, AddSwitch, EcsOnUpdate, Position, Movement());

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);

    ecs_progress(world, 0);

    test_assert(ecs_has_id(world, e1, ECS_SWITCH | Movement));
    test_assert(ecs_has_id(world, e2, ECS_SWITCH | Movement));
    test_assert(ecs_has_id(world, e3, ECS_SWITCH | Movement));

    ecs_fini(world);
}

static
void SetCase(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t sw_case = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(world, it->entities[i], ECS_CASE | sw_case);
    }
}

void Switch_add_case_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Walking());

    ECS_ENTITY(world, e1, Position, SWITCH | Movement);
    ECS_ENTITY(world, e2, Position, SWITCH | Movement);
    ECS_ENTITY(world, e3, Position, SWITCH | Movement);

    ecs_progress(world, 0);

    test_assert(ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e3, ECS_CASE | Walking));

    ecs_fini(world);
}

void Switch_change_case_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Walking());

    ECS_ENTITY(world, e1, Position, CASE | (Movement, Running));
    ECS_ENTITY(world, e2, Position, CASE | (Movement, Running));
    ECS_ENTITY(world, e3, Position, CASE | (Movement, Running));

    ecs_progress(world, 0);

    test_assert(!ecs_has_id(world, e1, ECS_CASE | Running));
    test_assert(!ecs_has_id(world, e2, ECS_CASE | Running));
    test_assert(!ecs_has_id(world, e3, ECS_CASE | Running));

    test_assert(ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e3, ECS_CASE | Walking));

    ecs_fini(world);
}

void Switch_change_one_case_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Jumping(), CASE | (Movement, Walking));

    ECS_ENTITY(world, e0, Position, CASE | (Movement, Jumping));
    ECS_ENTITY(world, e1, Position, CASE | (Movement, Walking));
    ECS_ENTITY(world, e2, Position, CASE | (Movement, Running));
    ECS_ENTITY(world, e3, Position, CASE | (Movement, Walking));
    ECS_ENTITY(world, e4, Position, CASE | (Movement, Running));
    ECS_ENTITY(world, e5, Position, CASE | (Movement, Jumping));

    ecs_progress(world, 0);

    test_assert(ecs_has_id(world, e0, ECS_CASE | Jumping));
    test_assert(ecs_has_id(world, e1, ECS_CASE | Jumping));
    test_assert(ecs_has_id(world, e2, ECS_CASE | Running));
    test_assert(ecs_has_id(world, e3, ECS_CASE | Jumping));
    test_assert(ecs_has_id(world, e4, ECS_CASE | Running));
    test_assert(ecs_has_id(world, e5, ECS_CASE | Jumping));

    ecs_fini(world);
}

static
void RemoveSwitch(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t movement = ecs_term_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove_id(world, it->entities[i], ECS_SWITCH | movement);
    }
}

void Switch_remove_switch_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ECS_SYSTEM(world, RemoveSwitch, EcsOnUpdate, SWITCH | Movement);

    ECS_ENTITY(world, e1, Position, SWITCH | Movement);
    ECS_ENTITY(world, e2, Position, SWITCH | Movement);
    ECS_ENTITY(world, e3, Position, SWITCH | Movement);

    ecs_progress(world, 0);

    test_assert(!ecs_has_id(world, e1, ECS_SWITCH | Movement));
    test_assert(!ecs_has_id(world, e2, ECS_SWITCH | Movement));
    test_assert(!ecs_has_id(world, e3, ECS_SWITCH | Movement));

    ecs_fini(world);
}

void Switch_switch_no_match_for_case() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_id(world, e, ECS_SWITCH | Movement);
    ecs_add_id(world, e, ECS_CASE | Walking);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Running)");
    ecs_iter_t it = ecs_query_iter(world, q);

    int count = 0;
    while (ecs_query_next(&it)) {
        count ++;
    }

    test_assert(count == 0);

    ecs_fini(world);
}

void Switch_empty_entity_has_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(!ecs_has_id(world, e, ECS_CASE | Walking));

    ecs_fini(world);
}

void Switch_zero_entity_has_case() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);

    test_expect_abort();

    test_assert(!ecs_has_id(world, 0, ECS_CASE | Walking));
}

void Switch_add_to_entity_w_switch() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new_w_id(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_add_id(world, e, ECS_CASE | Walking);
    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));
    test_int(ecs_get_case(world, e, Type), Walking);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));
    test_int(ecs_get_case(world, e, Type), Walking);

    ecs_fini(world);
}

void Switch_add_pair_to_entity_w_switch() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ECS_TYPE(world, Type, Walking, Running, Jumping);

    ecs_entity_t e = ecs_new_w_id(world, ECS_SWITCH | Type);
    test_assert(e != 0);

    ecs_add_id(world, e, ECS_CASE | Walking);
    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));
    test_int(ecs_get_case(world, e, Type), Walking);

    ecs_entity_t pair_id = ecs_pair(ecs_id(Position), Pair);
    ecs_add_id(world, e, pair_id);
    test_assert(ecs_has_id(world, e, pair_id));
    test_assert( ecs_has_id(world, e, ECS_CASE | Walking));
    test_int(ecs_get_case(world, e, Type), Walking);

    ecs_fini(world);
}

static
int compare_position(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2, const void *ptr2) {
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void Switch_sort() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TAG(world, Sitting);
    ECS_TYPE(world, Type, Walking, Running, Jumping, Sitting);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {2, 2});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {0, 2});

    ecs_add_id(world, e1, ECS_SWITCH | Type);
    ecs_add_id(world, e2, ECS_SWITCH | Type);
    ecs_add_id(world, e3, ECS_SWITCH | Type);
    ecs_add_id(world, e4, ECS_SWITCH | Type);

    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_add_id(world, e2, ECS_CASE | Running);
    ecs_add_id(world, e3, ECS_CASE | Jumping);
    ecs_add_id(world, e4, ECS_CASE | Sitting);
    
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t) {
        .filter.terms = {{ ecs_id(Position) }},
        .order_by_component = ecs_id(Position), 
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 4);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e3);
    test_assert(it.entities[2] == e2);
    test_assert(it.entities[3] == e1);

    /* Entities will have shuffled around, ensure cases got shuffled too */
    test_int(ecs_get_case(world, e1, Type), Walking);
    test_int(ecs_get_case(world, e2, Type), Running);
    test_int(ecs_get_case(world, e3, Type), Jumping);
    test_int(ecs_get_case(world, e4, Type), Sitting);

    ecs_fini(world);
}

void Switch_recycled_tags() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Standing, Walking, Running);

    test_assert(Standing > UINT32_MAX);
    test_assert(Walking > UINT32_MAX);
    test_assert(Running > UINT32_MAX);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_id(world, e, ECS_SWITCH | Movement);
    ecs_add_id(world, e, ECS_CASE | Standing);

    test_assert(ecs_has_id(world, e, ECS_SWITCH | Movement));
    test_assert(ecs_has_id(world, e, ECS_CASE | Standing));

    ecs_fini(world);
}

void Switch_query_recycled_tags() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TYPE(world, Movement, Standing, Walking, Running);

    test_assert(Standing > UINT32_MAX);
    test_assert(Walking > UINT32_MAX);
    test_assert(Running > UINT32_MAX);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_id(world, e, ECS_SWITCH | Movement);
    ecs_add_id(world, e, ECS_CASE | Standing);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Standing)");
    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    ecs_entity_t *cases = ecs_term(&it, ecs_entity_t, 1);
    test_assert(cases != NULL);
    test_assert(cases[0] == Standing);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_single_case() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TYPE(world, Movement, Walking);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e1 != 0);
    ecs_add_id(world, e1, ECS_CASE | Walking);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e2 != 0);
    ecs_add_id(world, e2, ECS_CASE | Walking);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e3 != 0);
    ecs_add_id(world, e3, ECS_CASE | Walking);

    test_assert(ecs_has_id(world, e1, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e3, ECS_CASE | Walking));

    ecs_delete(world, e1);

    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));
    test_assert(ecs_has_id(world, e3, ECS_CASE | Walking));

    ecs_delete(world, e3);

    test_assert(ecs_has_id(world, e2, ECS_CASE | Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_match_switch_on_base_instance() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);

    ecs_query_t *q = ecs_query_new(world, "CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_id(world);
    test_assert(base != 0);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    test_assert(e1 != 0);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add_id(world, e1, ECS_CASE | Walking);

    ecs_entity_t 
    case_id = ecs_get_case(world, e1, Movement);
    test_int(case_id, Walking);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void Switch_switch_w_bitset_query() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, false);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_inv() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_2_elems() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e0 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e0, ECS_CASE | Walking);
    ecs_set(world, e0, Position, {11, 22});
    ecs_enable_component(world, e0, Position, false);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_2_elems_skip() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e0 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e0, ECS_CASE | Walking);
    ecs_set(world, e0, Position, {11, 22});
    ecs_enable_component(world, e0, Position, false);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_elems_interleaved() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, false);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Walking);
    ecs_set(world, e3, Position, {13, 23});
    ecs_enable_component(world, e3, Position, false);

    ecs_entity_t e4 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e4, ECS_CASE | Running);
    ecs_set(world, e4, Position, {13, 23});
    ecs_enable_component(world, e4, Position, true);

    ecs_entity_t e5 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e5, ECS_CASE | Walking);
    ecs_set(world, e5, Position, {13, 23});
    ecs_enable_component(world, e5, Position, true);

    ecs_entity_t e6 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e6, ECS_CASE | Running);
    ecs_set(world, e6, Position, {13, 23});
    ecs_enable_component(world, e6, Position, true);

    ecs_entity_t e7 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e7, ECS_CASE | Walking);
    ecs_set(world, e7, Position, {13, 23});
    ecs_enable_component(world, e7, Position, false);

    ecs_entity_t e8 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e8, ECS_CASE | Walking);
    ecs_set(world, e8, Position, {13, 23});
    ecs_enable_component(world, e8, Position, true);

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_elems_interleaved_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TYPE(world, Movement, Walking, Running, Jumping);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Position, CASE | (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1, ECS_CASE | Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, false);

    ecs_entity_t e2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2, ECS_CASE | Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3, ECS_CASE | Walking);
    ecs_set(world, e3, Position, {13, 23});
    ecs_enable_component(world, e3, Position, false);

    ecs_entity_t e4 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e4, ECS_CASE | Running);
    ecs_set(world, e4, Position, {13, 23});
    ecs_enable_component(world, e4, Position, true);

    ecs_entity_t e5 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e5, ECS_CASE | Walking);
    ecs_set(world, e5, Position, {13, 23});
    ecs_enable_component(world, e5, Position, true);

    ecs_entity_t e6 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e6, ECS_CASE | Running);
    ecs_set(world, e6, Position, {13, 23});
    ecs_enable_component(world, e6, Position, true);

    ecs_entity_t e7 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e7, ECS_CASE | Walking);
    ecs_set(world, e7, Position, {13, 23});
    ecs_enable_component(world, e7, Position, false);

    ecs_entity_t e8 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e8, ECS_CASE | Walking);
    ecs_set(world, e8, Position, {13, 23});
    ecs_enable_component(world, e8, Position, true);


    ecs_entity_t e1_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e1_2, ECS_CASE | Walking);
    ecs_set(world, e1_2, Position, {10, 20});
    ecs_enable_component(world, e1_2, Position, false);
    ecs_add_id(world, e1_2, Tag);

    ecs_entity_t e2_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e2_2, ECS_CASE | Running);
    ecs_set(world, e2_2, Position, {11, 22});
    ecs_enable_component(world, e2_2, Position, true);
    ecs_add_id(world, e2_2, Tag);

    ecs_entity_t e3_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e3_2, ECS_CASE | Walking);
    ecs_set(world, e3_2, Position, {13, 23});
    ecs_enable_component(world, e3_2, Position, false);
    ecs_add_id(world, e3_2, Tag);

    ecs_entity_t e4_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e4_2, ECS_CASE | Running);
    ecs_set(world, e4_2, Position, {13, 23});
    ecs_enable_component(world, e4_2, Position, true);
    ecs_add_id(world, e4_2, Tag);

    ecs_entity_t e5_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e5_2, ECS_CASE | Walking);
    ecs_set(world, e5_2, Position, {13, 23});
    ecs_enable_component(world, e5_2, Position, true);
    ecs_add_id(world, e5_2, Tag);

    ecs_entity_t e6_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e6_2, ECS_CASE | Running);
    ecs_set(world, e6_2, Position, {13, 23});
    ecs_enable_component(world, e6_2, Position, true);
    ecs_add_id(world, e6_2, Tag);

    ecs_entity_t e7_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e7_2, ECS_CASE | Walking);
    ecs_set(world, e7_2, Position, {13, 23});
    ecs_enable_component(world, e7_2, Position, false);
    ecs_add_id(world, e7_2, Tag);

    ecs_entity_t e8_2 = ecs_new_w_id(world, ECS_SWITCH | Movement);
    ecs_add_id(world, e8_2, ECS_CASE | Walking);
    ecs_set(world, e8_2, Position, {13, 23});
    ecs_enable_component(world, e8_2, Position, true);
    ecs_add_id(world, e8_2, Tag);

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5_2);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8_2);
    p = ecs_term(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_term(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}
