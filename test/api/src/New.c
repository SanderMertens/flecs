#include <api.h>

void New_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(!ecs_get_type(world, e));

    ecs_fini(world);
}

void New_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_type_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_type_w_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, AND | Type_1, AND | Type_2);

    ecs_entity_t e = ecs_new(world, Type_3);

    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void New_type_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1, Velocity);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void New_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_new_w_entity(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag));
    
    ecs_fini(world);
}

void New_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    ECS_TYPE(world, Type, Tag);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag));
    
    ecs_fini(world);
}

void New_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);

    ECS_TYPE(world, Type, Tag_1, Tag_2);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag_1));
    test_assert(ecs_has_entity(world, e, Tag_1));
    
    ecs_fini(world);
}

void New_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);
    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position, Tag);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_redefine_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = 0;

    {
        ECS_COMPONENT(world, Position);
        c = ecs_entity(Position);
    }

    {
        ECS_COMPONENT(world, Position);
        test_assert(c == ecs_entity(Position));
    }
    
    ecs_fini(world);
}

void New_recycle_id_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e1 == e2);

    ecs_fini(world);
}

void New_recycle_id_w_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_new(world, 0);

    ecs_entity_t e1 = ecs_new_w_entity(world, tag);
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new_w_entity(world, tag);
    test_assert(e2 != 0);
    test_assert(e1 == e2);

    ecs_fini(world);
}

void New_recycle_id_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w_type(world, ecs_type(Position));
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new_w_type(world, ecs_type(Position));
    test_assert(e2 != 0);
    test_assert(e1 == e2);

    ecs_fini(world);
}

void New_recycle_empty_staged_delete() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_staging_begin(world);
    ecs_delete(world, e1);
    ecs_staging_end(world, false);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e1 == e2);

    ecs_fini(world);
}

void New_recycle_staged_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    test_assert(e1 != 0);

    ecs_staging_begin(world);
    ecs_delete(world, e1);
    ecs_staging_end(world, false);

    ecs_entity_t e2 = ecs_new(world, 0);
    test_assert(e2 != 0);
    test_assert(e1 == e2);

    ecs_fini(world);
}

void New_new_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);
    test_assert(!ecs_get_type(world, e));

    ecs_fini(world);
}

void New_new_component_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_component_id(world);
    test_assert(e != 0);
    test_assert(e < ECS_HI_COMPONENT_ID);
    test_assert(!ecs_get_type(world, e));

    ecs_fini(world);
}

void New_new_hi_component_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    ecs_entity_t c;
    do {
        c = ecs_new_component_id(world);
    } while (c < ECS_HI_COMPONENT_ID);

    test_assert(c != e);
    test_assert(c > e);

    ecs_fini(world);
}
