#include <api.h>

void Container_child() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_child_w_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent, Position);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));
    test_assert( ecs_has(world, child, Position));

    ecs_fini(world);
}

void Container_child_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent, Type);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));
    test_assert( ecs_has(world, child, Type));
    test_assert( ecs_has(world, child, Position));

    ecs_fini(world);
}

void Container_contains_w_empty_parent() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    test_assert( !ecs_contains(world, parent, child));
    test_assert( !ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_child_as_parent() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t child = ecs_new(world, 0);

    test_assert( !ecs_contains(world, child, child));
    test_assert( !ecs_has(world, child, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_0_as_child() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);

    test_assert( !ecs_contains(world, parent, 0));
    test_assert( !ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_0_as_parent() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t child = ecs_new(world, 0);

    test_assert( !ecs_contains(world, 0, child));
    test_assert( !ecs_has(world, child, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_0_for_both() {
    ecs_world_t *world = ecs_init();

    test_assert( !ecs_contains(world, 0, 0));

    ecs_fini(world);
}

void Container_adopt() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    ecs_adopt(world, child, parent);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_again() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    ecs_adopt(world, child, parent);
    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_adopt(world, child, parent);
    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_twice() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_1 = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    ecs_adopt(world, child, parent_1);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_entity_t parent_2 = ecs_new(world, 0);

    ecs_adopt(world, child, parent_2);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_orphan_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    ecs_orphan(world, child, parent);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( !ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_orphan() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_orphan(world, child, parent);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_orphan_again() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_orphan(world, child, parent);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_orphan(world, child, parent);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_orphan_twice() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_1 = ecs_new(world, 0);
    ecs_entity_t parent_2 = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent_1, 0);

    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( !ecs_has(world, parent_2, EcsContainer));

    ecs_adopt(world, child, parent_2);
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, child, parent_1);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, child, parent_2);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( !ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_orphan() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_1 = ecs_new(world, 0);
    ecs_entity_t parent_2 = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    ecs_adopt(world, child, parent_1);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_adopt(world, child, parent_2);
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, child, parent_1);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_orphan(world, child, parent_2);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( !ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_2_orphan_1() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent_1 = ecs_new(world, 0);
    ecs_entity_t parent_2 = ecs_new(world, 0);
    ecs_entity_t child = ecs_new(world, 0);

    ecs_adopt(world, child, parent_1);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_adopt(world, child, parent_2);
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, child, parent_2);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( !ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_get_ptr_container() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child(world, parent, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_type_t Tparent = ecs_type_from_entity(world, parent);
    test_assert(Tparent != 0);
    test_assert( ecs_has(world, child, parent));
    test_assert( ecs_get_ptr(world, child, parent) == NULL);

    ecs_fini(world);
}

void Container_child_w_count_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child_w_count(world, parent, Position, 10);

    test_assert( ecs_has(world, parent, EcsContainer));

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_contains(world, parent, child + i));
        test_assert( ecs_has(world, child + i, Position));
    }

    ecs_fini(world);
}

void Container_child_w_count_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t child = ecs_new_child_w_count(world, parent, Type, 10);

    test_assert( ecs_has(world, parent, EcsContainer));

    int i;
    for (i = 0; i < 10; i ++) {    
        test_assert( ecs_contains(world, parent, child + i));
        test_assert( ecs_has(world, child + i, Type));
        test_assert( ecs_has(world, child + i, Position));
    }

    ecs_fini(world);
}
