#include <include/api.h>

void Container_child() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, NULL, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_child_w_id() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, "foo", 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));
    test_str( ecs_id(world, child), "foo");

    ecs_fini(world);
}

void Container_child_w_component() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, "foo", Position);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));
    test_assert( ecs_has(world, child, Position));
    test_str( ecs_id(world, child), "foo");

    ecs_fini(world);
}

void Container_child_w_type() {
    EcsWorld *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Position);

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, "foo", Type);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));
    test_assert( ecs_has(world, child, Type));
    test_assert( ecs_has(world, child, Position));
    test_str( ecs_id(world, child), "foo");

    ecs_fini(world);
}

void Container_contains_w_empty_parent() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    test_assert( !ecs_contains(world, parent, child));
    test_assert( !ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_child_as_parent() {
    EcsWorld *world = ecs_init();

    EcsEntity child = ecs_new(world, 0);

    test_assert( !ecs_contains(world, child, child));
    test_assert( !ecs_has(world, child, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_0_as_child() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);

    test_assert( !ecs_contains(world, parent, 0));
    test_assert( !ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_0_as_parent() {
    EcsWorld *world = ecs_init();

    EcsEntity child = ecs_new(world, 0);

    test_assert( !ecs_contains(world, 0, child));
    test_assert( !ecs_has(world, child, EcsContainer));

    ecs_fini(world);
}

void Container_contains_w_0_for_both() {
    EcsWorld *world = ecs_init();

    test_assert( !ecs_contains(world, 0, 0));

    ecs_fini(world);
}

void Container_adopt() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    ecs_adopt(world, parent, child);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_again() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    ecs_adopt(world, parent, child);
    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_adopt(world, parent, child);
    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_twice() {
    EcsWorld *world = ecs_init();

    EcsEntity parent_1 = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    ecs_adopt(world, parent_1, child);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    EcsEntity parent_2 = ecs_new(world, 0);

    ecs_adopt(world, parent_2, child);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_orphan_empty() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    ecs_orphan(world, parent, child);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( !ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_orphan() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, NULL, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_orphan(world, parent, child);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_orphan_again() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, NULL, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_orphan(world, parent, child);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_orphan(world, parent, child);
    test_assert( !ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    ecs_fini(world);
}

void Container_orphan_twice() {
    EcsWorld *world = ecs_init();

    EcsEntity parent_1 = ecs_new(world, 0);
    EcsEntity parent_2 = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent_1, NULL, 0);

    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( !ecs_has(world, parent_2, EcsContainer));

    ecs_adopt(world, parent_2, child);
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, parent_1, child);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, parent_2, child);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( !ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_orphan() {
    EcsWorld *world = ecs_init();

    EcsEntity parent_1 = ecs_new(world, 0);
    EcsEntity parent_2 = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    ecs_adopt(world, parent_1, child);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_adopt(world, parent_2, child);
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, parent_1, child);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_orphan(world, parent_2, child);
    test_assert( !ecs_contains(world, parent_1, child));
    test_assert( !ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_adopt_2_orphan_1() {
    EcsWorld *world = ecs_init();

    EcsEntity parent_1 = ecs_new(world, 0);
    EcsEntity parent_2 = ecs_new(world, 0);
    EcsEntity child = ecs_new(world, 0);

    ecs_adopt(world, parent_1, child);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( ecs_has(world, parent_1, EcsContainer));

    ecs_adopt(world, parent_2, child);
    test_assert( ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_orphan(world, parent_2, child);
    test_assert( ecs_contains(world, parent_1, child));
    test_assert( !ecs_contains(world, parent_2, child));
    test_assert( ecs_has(world, parent_2, EcsContainer));

    ecs_fini(world);
}

void Container_get_ptr_container() {
    EcsWorld *world = ecs_init();

    EcsEntity parent = ecs_new(world, 0);
    EcsEntity child = ecs_new_child(world, parent, NULL, 0);

    test_assert( ecs_contains(world, parent, child));
    test_assert( ecs_has(world, parent, EcsContainer));

    EcsType Tparent = ecs_type_from_entity(world, parent);
    test_assert(Tparent != 0);
    test_assert( ecs_has(world, child, parent));
    test_assert( ecs_get_ptr(world, child, parent) == NULL);

    ecs_fini(world);
}
