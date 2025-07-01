#include <core.h>

void NonFragmentingChildOf_set_parent_no_ordered_children(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    test_expect_abort();
    ecs_insert(world, ecs_value(EcsParent, {parent}));
}

void NonFragmentingChildOf_set_0_parent(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    test_expect_abort();
    ecs_insert(world, ecs_value(EcsParent, {0}));
}

void NonFragmentingChildOf_set_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    
    const ecs_entities_t children = ecs_get_ordered_children(world, parent);
    test_int(children.count, 1);
    test_int(children.ids[0], child);

    ecs_fini(world);
}

void NonFragmentingChildOf_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child);
    }

    ecs_remove(world, child, EcsParent);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_clear_child(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child);
    }

    ecs_clear(world, child);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_child(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child);
    }

    ecs_delete(world, child);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }

    ecs_fini(world);
}
