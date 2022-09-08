#include <api.h>

void Switch_get_case_no_switch() {
    ecs_world_t *world = ecs_mini();
    
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, Rel, Union);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);

    test_uint(ecs_get_target(world, e, Rel, 0), 0);

    ecs_fini(world);
}

void Switch_get_case_set() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_fini(world);
}

void Switch_get_case_change() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_add_pair(world, e, Movement, Running);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Running);

    ecs_add_pair(world, e, Movement, Jumping);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Jumping);

    ecs_fini(world);
}

void Switch_remove_case() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_remove_pair(world, e, Movement, Walking);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), 0);

    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void Switch_remove_last() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Tag);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);
    ecs_add(world, e1, Tag);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);
    ecs_add(world, e2, Tag);

    ecs_remove(world, e2, Tag);

    test_assert(!ecs_has(world, e2, Tag));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));

    test_assert(ecs_has(world, e1, Tag));
    test_assert(ecs_has_pair(world, e1, Movement, Walking));

    ecs_fini(world);
}

void Switch_delete_first() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    ecs_delete(world, e1);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

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
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    ecs_delete(world, e3);

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));

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
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    ecs_delete(world, e1);
    ecs_delete(world, e3);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_3_entities_same_case() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Running));
    ECS_ENTITY(world, e3, (Movement, Running));

    test_assert( ecs_has_pair(world, e1, Movement, Running));
    test_assert( ecs_has_pair(world, e2, Movement, Running));
    test_assert( ecs_has_pair(world, e3, Movement, Running));

    test_int(ecs_get_target(world, e1, Movement, 0), Running);
    test_int(ecs_get_target(world, e2, Movement, 0), Running);
    test_int(ecs_get_target(world, e3, Movement, 0), Running);

    ecs_fini(world);
}

void Switch_2_entities_1_change_case() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Running));

    ecs_add_pair(world, e2, Movement, Jumping);
    test_assert( ecs_has_pair(world, e1, Movement, Running));
    test_assert( ecs_has_pair(world, e2, Movement, Jumping));

    test_int(ecs_get_target(world, e1, Movement, 0), Running);
    test_int(ecs_get_target(world, e2, Movement, 0), Jumping);

    ecs_fini(world);
}

void Switch_3_entities_change_case() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Running));
    ECS_ENTITY(world, e3, (Movement, Running));

    ecs_add_pair(world, e1, Movement, Walking);
    test_assert( ecs_has_pair(world, e1, Movement, Walking));
    test_assert( ecs_has_pair(world, e2, Movement, Running));
    test_assert( ecs_has_pair(world, e3, Movement, Running));

    test_int(ecs_get_target(world, e1, Movement, 0), Walking);
    test_int(ecs_get_target(world, e2, Movement, 0), Running);
    test_int(ecs_get_target(world, e3, Movement, 0), Running);

    ecs_add_pair(world, e2, Movement, Jumping);
    test_assert( ecs_has_pair(world, e1, Movement, Walking));
    test_assert( ecs_has_pair(world, e2, Movement, Jumping));
    test_assert( ecs_has_pair(world, e3, Movement, Running));

    test_int(ecs_get_target(world, e1, Movement, 0), Walking);
    test_int(ecs_get_target(world, e2, Movement, 0), Jumping);
    test_int(ecs_get_target(world, e3, Movement, 0), Running); 

    ecs_add_pair(world, e3, Movement, Walking);
    test_assert( ecs_has_pair(world, e1, Movement, Walking));
    test_assert( ecs_has_pair(world, e2, Movement, Jumping));
    test_assert( ecs_has_pair(world, e3, Movement, Walking));

    test_int(ecs_get_target(world, e1, Movement, 0), Walking);
    test_int(ecs_get_target(world, e2, Movement, 0), Jumping);
    test_int(ecs_get_target(world, e3, Movement, 0), Walking);

    ecs_fini(world);
}

static
void MatchSwitch(ecs_iter_t *it) {
    ecs_entity_t *movement = ecs_field(it, ecs_entity_t, 1);
    test_assert(movement != NULL);
    probe_iter(it);
}

void Switch_query_switch() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query_new(world, "(Movement, *)");
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 3);
    test_uint(it.entities[0], e1);
    test_uint(it.entities[1], e2);
    test_uint(it.entities[2], e3);
    test_uint(ecs_pair(Movement, EcsWildcard), ecs_field_id(&it, 1));
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_query_1_case_1_type() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Running)");
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(it.ids[0], ecs_pair(Movement, Running));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.ids[0], ecs_pair(Movement, Running));
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_query_1_case_2_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Running)");
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));
    ECS_ENTITY(world, e4, (Movement, Walking), Position);
    ECS_ENTITY(world, e5, (Movement, Running), Position);
    ECS_ENTITY(world, e6, (Movement, Walking), Position);
    ECS_ENTITY(world, e7, (Movement, Running), Position);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e3);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e7);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e5);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));    

    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_query_2_cases_1_type() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Running), (Direction, Front)");
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e2, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e3, (Movement, Running), (Direction, Back));
    ECS_ENTITY(world, e4, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e5, (Movement, Walking), (Direction, Front));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 2));
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_query_2_cases_2_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Running), (Direction, Front)");
    test_assert(q != NULL);

    ECS_ENTITY(world, e1, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e2, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e3, (Movement, Running), (Direction, Back));
    ECS_ENTITY(world, e4, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e5, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e6, Position, (Movement, Walking), (Direction, Front));
    ECS_ENTITY(world, e7, Position, (Movement, Running), (Direction, Front));
    ECS_ENTITY(world, e8, Position, (Movement, Walking), (Direction, Front));

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e4);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(it.count, 1);
    test_uint(it.entities[0], e7);
    test_uint(ecs_pair(Movement, Running), ecs_field_id(&it, 1));
    test_uint(ecs_pair(Direction, Front), ecs_field_id(&it, 2));
    test_bool(false, ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_query_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, e1, (Movement, Walking));
    ECS_ENTITY(world, e2, (Movement, Walking));
    ECS_ENTITY(world, e3, (Movement, Running));
    ECS_ENTITY(world, e4, (Movement, Running));
    ECS_ENTITY(world, e5, (Movement, Running));
    ECS_ENTITY(world, e6, (Movement, Jumping));
    ECS_ENTITY(world, e7, (Movement, Jumping));

    ecs_query_t *q_walking = ecs_query_new(world, "(Movement, Walking)");
    ecs_query_t *q_running = ecs_query_new(world, "(Movement, Running)");
    ecs_query_t *q_jumping = ecs_query_new(world, "(Movement, Jumping)");

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

    ecs_remove_pair(world, e4, Movement, Running);
    test_assert(!ecs_has_pair(world, e4, Movement, Running));
    ecs_entity_t c = ecs_get_target(world, e4, Movement, 0);
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

    ecs_add_pair(world, e4, Movement, Running);
    test_assert(ecs_has_pair(world, e4, Movement, Running));
    c = ecs_get_target(world, e4, Movement, 0);
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
void SetCase(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t id = ecs_field_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_add_id(world, it->entities[i], id);
    }
}

void Switch_add_case_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Movement(0, Walking));

    ECS_ENTITY(world, e1, Position);
    ECS_ENTITY(world, e2, Position);
    ECS_ENTITY(world, e3, Position);

    ecs_progress(world, 0);

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_fini(world);
}

void Switch_change_case_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Movement(0, Walking));

    ECS_ENTITY(world, e1, Position, (Movement, Running));
    ECS_ENTITY(world, e2, Position, (Movement, Running));
    ECS_ENTITY(world, e3, Position, (Movement, Running));

    ecs_progress(world, 0);

    test_assert(!ecs_has_pair(world, e1, Movement, Running));
    test_assert(!ecs_has_pair(world, e2, Movement, Running));
    test_assert(!ecs_has_pair(world, e3, Movement, Running));

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_fini(world);
}

void Switch_change_one_case_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, SetCase, EcsOnUpdate, Position, Movement(0, Jumping), (Movement, Walking));

    ECS_ENTITY(world, e0, Position, (Movement, Jumping));
    ECS_ENTITY(world, e1, Position, (Movement, Walking));
    ECS_ENTITY(world, e2, Position, (Movement, Running));
    ECS_ENTITY(world, e3, Position, (Movement, Walking));
    ECS_ENTITY(world, e4, Position, (Movement, Running));
    ECS_ENTITY(world, e5, Position, (Movement, Jumping));

    ecs_progress(world, 0);

    test_assert(ecs_has_pair(world, e0, Movement, Jumping));
    test_assert(ecs_has_pair(world, e1, Movement, Jumping));
    test_assert(ecs_has_pair(world, e2, Movement, Running));
    test_assert(ecs_has_pair(world, e3, Movement, Jumping));
    test_assert(ecs_has_pair(world, e4, Movement, Running));
    test_assert(ecs_has_pair(world, e5, Movement, Jumping));

    ecs_fini(world);
}

static
void RemoveSwitch(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_entity_t id = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_remove_id(world, it->entities[i], id);
    }
}

void Switch_remove_switch_in_stage() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_SYSTEM(world, RemoveSwitch, EcsOnUpdate, (Movement, *));

    ECS_ENTITY(world, e1, Position, (Movement, Walking));
    ECS_ENTITY(world, e2, Position, (Movement, Walking));
    ECS_ENTITY(world, e3, Position, (Movement, Walking));

    ecs_progress(world, 0);

    test_assert(!ecs_has_pair(world, e1, Movement, Walking));
    test_assert(!ecs_has_pair(world, e2, Movement, Walking));
    test_assert(!ecs_has_pair(world, e3, Movement, Walking));

    test_assert(!ecs_has_pair(world, e1, Movement, EcsWildcard));
    test_assert(!ecs_has_pair(world, e2, Movement, EcsWildcard));
    test_assert(!ecs_has_pair(world, e3, Movement, EcsWildcard));

    ecs_fini(world);
}

void Switch_switch_no_match_for_case() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_new_w_pair(world, Movement, Walking);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Running)");
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

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(!ecs_has_pair(world, e, Movement, Walking));

    ecs_fini(world);
}

void Switch_zero_entity_has_case() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);

    test_expect_abort();

    test_assert(!ecs_has_pair(world, 0, Movement, Walking));
}

void Switch_add_to_entity_w_switch() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_add(world, e, Position);
    test_assert( ecs_has(world, e, Position));
    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_fini(world);
}

void Switch_add_pair_to_entity_w_switch() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Pair);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);
    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_entity_t pair_id = ecs_pair(ecs_id(Position), Pair);
    ecs_add_id(world, e, pair_id);
    test_assert(ecs_has_id(world, e, pair_id));
    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_int(ecs_get_target(world, e, Movement, 0), Walking);

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

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_TAG(world, Sitting);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {2, 2});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {0, 2});

    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e2, Movement, Running);
    ecs_add_pair(world, e3, Movement, Jumping);
    ecs_add_pair(world, e4, Movement, Sitting);
    
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
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
    test_assert(!ecs_query_next(&it));

    /* Entities will have shuffled around, ensure cases got shuffled too */
    test_uint(ecs_get_target(world, e1, Movement, 0), Walking);
    test_uint(ecs_get_target(world, e2, Movement, 0), Running);
    test_uint(ecs_get_target(world, e3, Movement, 0), Jumping);
    test_uint(ecs_get_target(world, e4, Movement, 0), Sitting);

    ecs_fini(world);
}

void Switch_recycled_tags() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    test_assert(Standing > UINT32_MAX);
    test_assert(Walking > UINT32_MAX);
    test_assert(Running > UINT32_MAX);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Standing);
    test_assert(ecs_has_pair(world, e, Movement, Standing));
    test_assert(!ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert(!ecs_has_pair(world, e, Movement, Standing));
    test_assert(ecs_has_pair(world, e, Movement, Walking));
    test_assert(!ecs_has_pair(world, e, Movement, Running));

    ecs_fini(world);
}

void Switch_query_recycled_tags() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_entity_t e2 = ecs_new_id(world);
    ecs_entity_t e3 = ecs_new_id(world);

    ecs_delete(world, e1);
    ecs_delete(world, e2);
    ecs_delete(world, e3);

    ECS_TAG(world, Standing);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);

    test_assert(Standing > UINT32_MAX);
    test_assert(Walking > UINT32_MAX);
    test_assert(Running > UINT32_MAX);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Movement, Standing);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Standing)");
    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e);

    ecs_entity_t *cases = ecs_field(&it, ecs_entity_t, 1);
    test_assert(cases != NULL);
    test_assert(ecs_get_alive(world, cases[0]) == Standing);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_single_case() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e3 != 0);

    test_assert(ecs_has_pair(world, e1, Movement, Walking));
    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_delete(world, e1);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));
    test_assert(ecs_has_pair(world, e3, Movement, Walking));

    ecs_delete(world, e3);

    test_assert(ecs_has_pair(world, e2, Movement, Walking));

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_match_switch_on_base_instance() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t base = ecs_new_id(world);
    test_assert(base != 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e1 != 0);
    ecs_add_pair(world, e1, EcsIsA, base);

    test_uint(ecs_get_target(world, e1, Movement, 0), Walking);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

void Switch_switch_w_bitset_query() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, false);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    test_bool(ecs_is_enabled_component(world, e1, Position), true);
    test_bool(ecs_is_enabled_component(world, e2, Position), false);

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_inv() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_2_elems() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e0 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e0, Position, {11, 22});
    ecs_enable_component(world, e0, Position, false);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_2_elems_skip() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, true);

    ecs_entity_t e0 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e0, Position, {11, 22});
    ecs_enable_component(world, e0, Position, false);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 22);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_elems_interleaved() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position, (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, false);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});
    ecs_enable_component(world, e3, Position, false);

    ecs_entity_t e4 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e4, Position, {13, 23});
    ecs_enable_component(world, e4, Position, true);

    ecs_entity_t e5 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e5, Position, {13, 23});
    ecs_enable_component(world, e5, Position, true);

    ecs_entity_t e6 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e6, Position, {13, 23});
    ecs_enable_component(world, e6, Position, true);

    ecs_entity_t e7 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e7, Position, {13, 23});
    ecs_enable_component(world, e7, Position, false);

    ecs_entity_t e8 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e8, Position, {13, 23});
    ecs_enable_component(world, e8, Position, true);

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_switch_w_bitset_query_elems_interleaved_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_new(world, "Position, (Movement, Walking)");
    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1, Position, {10, 20});
    ecs_enable_component(world, e1, Position, false);

    ecs_entity_t e2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2, Position, {11, 22});
    ecs_enable_component(world, e2, Position, true);

    ecs_entity_t e3 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3, Position, {13, 23});
    ecs_enable_component(world, e3, Position, false);

    ecs_entity_t e4 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e4, Position, {13, 23});
    ecs_enable_component(world, e4, Position, true);

    ecs_entity_t e5 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e5, Position, {13, 23});
    ecs_enable_component(world, e5, Position, true);

    ecs_entity_t e6 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e6, Position, {13, 23});
    ecs_enable_component(world, e6, Position, true);

    ecs_entity_t e7 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e7, Position, {13, 23});
    ecs_enable_component(world, e7, Position, false);

    ecs_entity_t e8 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e8, Position, {13, 23});
    ecs_enable_component(world, e8, Position, true);


    ecs_entity_t e1_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e1_2, Position, {10, 20});
    ecs_enable_component(world, e1_2, Position, false);
    ecs_add_id(world, e1_2, Tag);

    ecs_entity_t e2_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e2_2, Position, {11, 22});
    ecs_enable_component(world, e2_2, Position, true);
    ecs_add_id(world, e2_2, Tag);

    ecs_entity_t e3_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e3_2, Position, {13, 23});
    ecs_enable_component(world, e3_2, Position, false);
    ecs_add_id(world, e3_2, Tag);

    ecs_entity_t e4_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e4_2, Position, {13, 23});
    ecs_enable_component(world, e4_2, Position, true);
    ecs_add_id(world, e4_2, Tag);

    ecs_entity_t e5_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e5_2, Position, {13, 23});
    ecs_enable_component(world, e5_2, Position, true);
    ecs_add_id(world, e5_2, Tag);

    ecs_entity_t e6_2 = ecs_new_w_pair(world, Movement, Running);
    ecs_set(world, e6_2, Position, {13, 23});
    ecs_enable_component(world, e6_2, Position, true);
    ecs_add_id(world, e6_2, Tag);

    ecs_entity_t e7_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e7_2, Position, {13, 23});
    ecs_enable_component(world, e7_2, Position, false);
    ecs_add_id(world, e7_2, Tag);

    ecs_entity_t e8_2 = ecs_new_w_pair(world, Movement, Walking);
    ecs_set(world, e8_2, Position, {13, 23});
    ecs_enable_component(world, e8_2, Position, true);
    ecs_add_id(world, e8_2, Tag);

    Position *p;
    ecs_entity_t *c;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e5_2);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_int(it.entities[0], e8_2);
    p = ecs_field(&it, Position, 1);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 23);
    c = ecs_field(&it, ecs_entity_t, 2);
    test_assert(c != NULL);
    test_assert(c[0] == Walking);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Switch_has_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_id(world);
    test_assert( !ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_add_pair(world, e, Movement, Walking);
    test_assert( ecs_has_pair(world, e, Movement, EcsWildcard));

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_assert( ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_fini(world);
}

void Switch_remove_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), Walking);

    ecs_remove_pair(world, e, Movement, EcsWildcard);

    test_assert( !ecs_has_pair(world, e, Movement, Walking));
    test_assert( !ecs_has_pair(world, e, Movement, Running));
    test_assert( !ecs_has_pair(world, e, Movement, Jumping));
    test_uint(ecs_get_target(world, e, Movement, 0), 0);
    test_assert( !ecs_has_pair(world, e, Movement, EcsWildcard));

    ecs_fini(world);
}

void Switch_same_table_after_change() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert(e != 0);

    ecs_table_t *table = ecs_get_table(world, e);
    test_assert(table != NULL);

    ecs_add_pair(world, e, Movement, Running);
    test_assert(ecs_get_table(world, e) == table);

    ecs_add_pair(world, e, Movement, Jumping);
    test_assert(ecs_get_table(world, e) == table);
    
    ecs_remove_pair(world, e, Movement, EcsWildcard);
    test_assert(ecs_get_table(world, e) != table);
    test_assert(ecs_get_table(world, e) == NULL);

    ecs_fini(world);
}

void Switch_component_relation() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Front);
    ECS_TAG(world, Back);
    ECS_TAG(world, Left);
    ECS_TAG(world, Right);
    
    ecs_set(world, Movement, EcsComponent, { .size = 1, .alignment = 1 });
    ecs_set(world, Direction, EcsComponent, { .size = 1, .alignment = 1 });

    ecs_entity_t e1 = ecs_new_id(world);
    ecs_add_pair(world, e1, Movement, Walking);
    ecs_add_pair(world, e1, Direction, Front);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_pair(world, e2, Movement, Running);
    ecs_add_pair(world, e2, Direction, Left);

    ecs_query_t *q = ecs_query_new(world, "(Movement, Walking), (Direction, *)");
    test_assert(q != NULL);

    ecs_entity_t *m;
    ecs_entity_t *d;

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_uint(it.entities[0], e1);
    test_uint(it.ids[0], ecs_pair(Movement, Walking));
    test_uint(it.ids[1], ecs_pair(Direction, EcsWildcard));
    m = ecs_field(&it, ecs_entity_t, 1);
    d = ecs_field(&it, ecs_entity_t, 2);
    test_uint(m[0], Walking);
    test_uint(d[0], Front);

    test_bool(ecs_query_next(&it), false);

    ecs_fini(world);
}

static
void Trigger(ecs_iter_t *it) {
    probe_system_w_ctx(it, it->ctx);
}

void Switch_delete_case_trigger_after_delete_switch() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    Probe ctx = {0};

    ecs_observer_init(world, &(ecs_observer_desc_t){
        .filter.terms[0] = { .id = ecs_pair(Movement, Walking), .src.flags = EcsSelf },
        .events = {EcsOnAdd},
        .callback = Trigger,
        .ctx = &ctx
    });

    ecs_entity_t e = ecs_new_w_pair(world, Movement, Walking);
    test_assert( ecs_has_pair(world, e, Movement, Walking));

    ecs_delete(world, Movement);

    ecs_fini(world);
}

void Switch_add_2() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Up);
    ECS_TAG(world, Down);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Movement, Walking);
    ecs_add_pair(world, e, Direction, Up);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( ecs_has_pair(world, e, Direction, Up));

    test_assert(ecs_get_target(world, e, Movement, 0) == Walking);
    test_assert(ecs_get_target(world, e, Direction, 0) == Up);

    ecs_fini(world);
}

void Switch_add_2_reverse() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ECS_ENTITY(world, Direction, Union);
    ECS_TAG(world, Up);
    ECS_TAG(world, Down);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Direction, Up);
    ecs_add_pair(world, e, Movement, Walking);

    test_assert( ecs_has_pair(world, e, Movement, Walking));
    test_assert( ecs_has_pair(world, e, Direction, Up));

    test_assert(ecs_get_target(world, e, Movement, 0) == Walking);
    test_assert(ecs_get_target(world, e, Direction, 0) == Up);

    ecs_fini(world);
}

void Switch_add_switch_to_prefab_instance() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Movement, Union);
    ECS_TAG(world, Walking);
    ECS_TAG(world, Running);
    ECS_TAG(world, Jumping);

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_pair(world, inst, Movement, Walking);

    test_assert(ecs_has_pair(world, inst, Movement, Walking));
    test_assert(ecs_get_target(world, inst, Movement, 0) == Walking);

    ecs_fini(world);
}
