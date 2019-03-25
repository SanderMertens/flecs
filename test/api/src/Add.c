#include <include/api.h>

void Add_zero() {
    ecs_world_t *world = ecs_init();

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, 0);
    test_assert(ecs_empty(world, e));
    
    ecs_fini(world);
}

void Add_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_component_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    
    ecs_fini(world);
}

void Add_2_components() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has(world, e, Velocity));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Add_2_components_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    
    ecs_fini(world);
}

void Add_2_components_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    ecs_add(world, e, Velocity);
    ecs_add(world, e, Mass);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));
    
    ecs_fini(world);
}

void Add_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Type_1);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_w_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, Type_1, Type_2);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_3);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Type_1, Velocity);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_1);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type_1);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Velocity, Mass);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type_1);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));

    ecs_fini(world);
}

void Add_type_again_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_type_overlap_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_component_to_nonempty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Velocity);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_component_to_nonempty_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void Add_component_to_nonempty_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Position);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_to_nonempty() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_to_nonempty_again() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    EcsEntity e = ecs_new(world, Type);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    ecs_fini(world);
}

void Add_type_to_nonempty_overlap() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_TYPE(world, Type_1, Position, Velocity);
    ECS_TYPE(world, Type_2, Velocity, Mass);

    EcsEntity e = ecs_new(world, Type_1);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(!ecs_has(world, e, Mass));

    ecs_add(world, e, Type_2);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));
    test_assert(ecs_has(world, e, Mass));

    ecs_fini(world);
}

void Add_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Tag);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void Add_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TYPE(world, Type, Tag);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Type));
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void Add_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);
    ECS_TYPE(world, Type, Tag_1, Tag_2);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Type));
    test_assert(ecs_has(world, e, Tag_1));
    test_assert(ecs_has(world, e, Tag_2));

    ecs_fini(world);
}

void Add_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag_1);
    ECS_TAG(world, Tag_2);
    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Tag_1, Tag_2, Position);

    EcsEntity e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_add(world, e, Type);
    test_assert(ecs_has(world, e, Type));
    test_assert(ecs_has(world, e, Tag_1));
    test_assert(ecs_has(world, e, Tag_2));
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}
