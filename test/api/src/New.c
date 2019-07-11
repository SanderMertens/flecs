#include <api.h>

void New_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_is_empty(world, e));

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
    ECS_TYPE(world, Type_2, Type_1);

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
    ECS_TYPE(world, Type_3, Type_1, Type_2);

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
    ECS_TYPE(world, Type_2, Type_1, Velocity);

    ecs_entity_t e = ecs_new(world, Type_2);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void New_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    
    ecs_fini(world);
}

void New_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ECS_TYPE(world, Type, Tag);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    
    ecs_fini(world);
}

void New_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);

    ECS_TYPE(world, Type, Tag_1, Tag_2);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag_1));
    test_assert(ecs_has(world, e, Tag_1));
    
    ecs_fini(world);
}

void New_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ECS_TYPE(world, Type, Position, Tag);

    ecs_entity_t e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void New_redefine_component() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t c = 0;
    ecs_type_t t = NULL;

    {
        ECS_COMPONENT(world, Position);
        c = ecs_entity(Position);
        t = ecs_type(Position);
    }

    {
        ECS_COMPONENT(world, Position);
        test_assert(c == ecs_entity(Position));
        test_assert(t == ecs_type(Position));
    }
    
    ecs_fini(world);
}
