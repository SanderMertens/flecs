#include <core.h>

void NonFragmentingChildOf_set_parent_no_ordered_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    const ecs_entities_t children = ecs_get_ordered_children(world, parent);
    test_int(children.count, 1);
    test_int(children.ids[0], child);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_ordered_children_before_set_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    const ecs_entities_t children = ecs_get_ordered_children(world, parent);
    test_int(children.count, 1);
    test_int(children.ids[0], child);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_ordered_children_after_set_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    ecs_add_id(world, parent, EcsOrderedChildren);

    test_assert(ecs_has_id(world, parent, EcsOrderedChildren));

    const ecs_entities_t children = ecs_get_ordered_children(world, parent);
    test_int(children.count, 1);
    test_int(children.ids[0], child);

    ecs_fini(world);
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

    ecs_entity_t child = ecs_new(world);
    ecs_set(world, child, EcsParent, { parent });
    
    const ecs_entities_t children = ecs_get_ordered_children(world, parent);
    test_int(children.count, 1);
    test_int(children.ids[0], child);

    ecs_fini(world);
}

void NonFragmentingChildOf_insert_parent(void) {
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

void NonFragmentingChildOf_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 1);
        test_int(children.ids[0], child);
    } {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_b);
        test_int(children.count, 0);
    }

    ecs_set(world, child, EcsParent, {parent_b});

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 0);
    } {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_b);
        test_int(children.count, 1);
        test_int(children.ids[0], child);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_set_parent_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_remove_parent_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
    }

    ecs_remove(world, child_a, EcsParent);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_b);
    }

    ecs_remove(world, child_b, EcsParent);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_clear_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
    }

    ecs_clear(world, child_a);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_b);
    }

    ecs_clear(world, child_b);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
    }

    ecs_delete(world, child_a);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_b);
    }

    ecs_delete(world, child_b);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_reparent_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_a });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
    }

    ecs_set(world, child_a, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 1);
        test_int(children.ids[0], child_b);
    }
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_b);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_set(world, child_b, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 0);
    }
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_b);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_delete(world, parent_a);

    test_assert(!ecs_is_alive(world, parent_a));
    test_assert(!ecs_is_alive(world, child_a));
    test_assert(ecs_is_alive(world, child_b));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_parent_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_a_1 = ecs_new(world);
    ecs_set(world, child_a_1, EcsParent, { parent_a });
    ecs_entity_t child_a_2 = ecs_new(world);
    ecs_set(world, child_a_2, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a_1);
        test_int(children.ids[1], child_a_2);
    }

    ecs_delete(world, parent_a);

    test_assert(!ecs_is_alive(world, parent_a));
    test_assert(!ecs_is_alive(world, child_a_1));
    test_assert(!ecs_is_alive(world, child_a_2));
    test_assert(ecs_is_alive(world, child_b));

    ecs_fini(world);
}

void NonFragmentingChildOf_mixed_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);

    ecs_entity_t child_a_1 = ecs_new(world);
    ecs_set(world, child_a_1, EcsParent, { parent_a });
    ecs_entity_t child_a_2 = ecs_new(world);
    ecs_add_pair(world, child_a_2, EcsChildOf, parent_a);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a_1);
        test_int(children.ids[1], child_a_2);
    }

    ecs_delete(world, parent_a);

    test_assert(!ecs_is_alive(world, parent_a));
    test_assert(!ecs_is_alive(world, child_a_1));
    test_assert(!ecs_is_alive(world, child_a_2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_parent_w_mixed_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_a_1 = ecs_new(world);
    ecs_set(world, child_a_1, EcsParent, { parent_a });
    ecs_entity_t child_a_2 = ecs_new(world);
    ecs_add_pair(world, child_a_2, EcsChildOf, parent_a);

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 2);
        test_int(children.ids[0], child_a_1);
        test_int(children.ids[1], child_a_2);
    }

    ecs_delete(world, parent_a);

    test_assert(!ecs_is_alive(world, parent_a));
    test_assert(!ecs_is_alive(world, child_a_1));
    test_assert(!ecs_is_alive(world, child_a_2));
    test_assert(ecs_is_alive(world, child_b));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_nested_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root = ecs_new(world);
    ecs_add_id(world, root, EcsOrderedChildren);
    
    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_set(world, parent_a, EcsParent, { root });

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, root);
        test_int(children.count, 1);
        test_int(children.ids[0], parent_a);
    }
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_delete(world, root);

    test_assert(!ecs_is_alive(world, root));
    test_assert(!ecs_is_alive(world, parent_a));
    test_assert(!ecs_is_alive(world, child_a));
    test_assert(ecs_is_alive(world, parent_b));
    test_assert(ecs_is_alive(world, child_b));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_nested_mixed_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root = ecs_new(world);
    ecs_add_id(world, root, EcsOrderedChildren);
    
    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_set(world, parent_a, EcsParent, { root });

    ecs_entity_t child_a = ecs_new(world);
    ecs_add_pair(world, child_a, EcsChildOf, parent_a );

    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);
    ecs_add_pair(world, parent_b, EcsChildOf, root);

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, root);
        test_int(children.count, 2);
        test_int(children.ids[0], parent_a);
        test_int(children.ids[1], parent_b);
    }
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_a);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }
    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent_b);
        test_int(children.count, 1);
        test_int(children.ids[0], child_b);
    }

    ecs_delete(world, root);

    test_assert(!ecs_is_alive(world, root));
    test_assert(!ecs_is_alive(world, parent_a));
    test_assert(!ecs_is_alive(world, child_a));
    test_assert(!ecs_is_alive(world, parent_b));
    test_assert(!ecs_is_alive(world, child_b));

    ecs_fini(world);
}

void NonFragmentingChildOf_remove_ordered_children_trait_w_children(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent } );

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    test_expect_abort();
    ecs_remove_id(world, parent, EcsOrderedChildren);
}

void NonFragmentingChildOf_remove_ordered_children_trait_w_deleted_child(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent } );

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 1);
        test_int(children.ids[0], child_a);
    }

    ecs_delete(world, child_a);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }
    
    ecs_remove_id(world, parent, EcsOrderedChildren);

    test_assert(!ecs_has_id(world, parent, EcsOrderedChildren));
    
    ecs_fini(world);
}

void NonFragmentingChildOf_remove_ordered_children_trait_w_deleted_children(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent } );
    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent } );
    ecs_entity_t child_c = ecs_new(world);
    ecs_set(world, child_c, EcsParent, { parent } );

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 3);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
        test_int(children.ids[2], child_c);
    }

    ecs_delete(world, child_a);
    ecs_delete(world, child_b);
    ecs_delete(world, child_c);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }
    
    ecs_remove_id(world, parent, EcsOrderedChildren);

    test_assert(!ecs_has_id(world, parent, EcsOrderedChildren));
    
    ecs_fini(world);
}

void NonFragmentingChildOf_remove_ordered_children_trait_w_deleted_children_reverse(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent } );
    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent } );
    ecs_entity_t child_c = ecs_new(world);
    ecs_set(world, child_c, EcsParent, { parent } );

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 3);
        test_int(children.ids[0], child_a);
        test_int(children.ids[1], child_b);
        test_int(children.ids[2], child_c);
    }

    ecs_delete(world, child_c);
    ecs_delete(world, child_b);
    ecs_delete(world, child_a);

    {
        const ecs_entities_t children = ecs_get_ordered_children(world, parent);
        test_int(children.count, 0);
    }
    
    ecs_remove_id(world, parent, EcsOrderedChildren);

    test_assert(!ecs_has_id(world, parent, EcsOrderedChildren));
    
    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent_2(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_a = ecs_new_w(world, Position);
    ecs_entity_t parent_b = ecs_new_w(world, Position);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent_4(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);

    ecs_entity_t child_a = ecs_new_w(world, Position);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new_w(world, Position);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent_5(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);

    ecs_entity_t child_a = ecs_entity(world, { .name = "a" });
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_entity(world, { .name = "b" });
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent_6(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);

    ecs_entity_t child_a = ecs_entity(world, { .name = "a" });
    ecs_add(world, child_a, Position);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_entity(world, { .name = "b" });
    ecs_add(world, child_b, Position);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_parent_7(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);

    ecs_entity_t child_a = ecs_entity(world, { .name = "a" });
    ecs_add(world, child_a, Position);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_entity(world, { .name = "b" });
    ecs_add(world, child_b, Position);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_parent(world, child_a) == parent_a);
    test_assert(ecs_get_parent(world, child_b) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_target(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_add_id(world, parent_a, EcsOrderedChildren);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_add_id(world, parent_b, EcsOrderedChildren);

    ecs_entity_t child_a = ecs_new(world);
    ecs_set(world, child_a, EcsParent, { parent_a });

    ecs_entity_t child_b = ecs_new(world);
    ecs_set(world, child_b, EcsParent, { parent_b });

    test_assert(ecs_get_target(world, child_a, EcsChildOf, 0) == parent_a);
    test_assert(ecs_get_target(world, child_b, EcsChildOf, 0) == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c);
    test_assert(table != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->entity, c);
    test_int(pr->count, 1);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 1);
    test_uint(entities.ids[0], c);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c_2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_table_t *t1 = ecs_get_table(world, c_1);
    test_assert(t1 == ecs_get_table(world, c_2));

    const ecs_parent_record_t *pr1 = flecs_component_get_parent_record(cr, t1);
    test_assert(pr1 != NULL);
    test_int(pr1->count, 2);

    ecs_add(world, c_1, Foo);
    ecs_add(world, c_2, Foo);

    ecs_table_t *t2 = ecs_get_table(world, c_1);
    test_assert(t2 == ecs_get_table(world, c_2));
    test_assert(t1 != t2);

    const ecs_parent_record_t *pr2 = flecs_component_get_parent_record(cr, t2);
    test_assert(pr2 != NULL);
    test_assert(pr2 != pr1);
    test_int(pr2->count, 2);
    test_assert(flecs_component_get_parent_record(cr, t1) == NULL);

    ecs_entity_t parent = ecs_new(world);
    ecs_add(world, parent, Bar);

    ecs_add_pair(world, p, EcsChildOf, parent);

    ecs_table_t *t3 = ecs_get_table(world, c_1);
    test_assert(t3 == ecs_get_table(world, c_2));
    test_assert(t3 != t2);

    const ecs_parent_record_t *pr3 = flecs_component_get_parent_record(cr, t3);
    test_assert(pr3 != NULL);
    test_assert(pr3 != pr1);
    test_int(pr3->count, 2);
    test_assert(flecs_component_get_parent_record(cr, t1) == NULL);
    test_assert(flecs_component_get_parent_record(cr, t2) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 2);
    test_uint(entities.ids[0], c_1);
    test_uint(entities.ids[1], c_2);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_set_parent_after_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr == NULL);

    ecs_entity_t c = ecs_new_w_id(world, Foo);

    {
        ecs_table_t *table = ecs_get_table(world, c);
        test_assert(table != NULL);
    }

    ecs_set(world, c, EcsParent, {p});

    cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    {
        ecs_table_t *table = ecs_get_table(world, c);
        test_assert(table != NULL);

        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 1);
    test_uint(entities.ids[0], c);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_add(world, c, Foo);

    ecs_table_t *table_b = ecs_get_table(world, c);
    test_assert(table_b != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr == NULL);
    }

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_b);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 1);
    test_uint(entities.ids[0], c);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_remove(world, c, Foo);
    test_assert(!ecs_has(world, c, Foo));

    ecs_table_t *table_b = ecs_get_table(world, c);
    test_assert(table_b != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr == NULL);
    }

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_b);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 1);
    test_uint(entities.ids[0], c);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_remove_all(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_remove_all(world, Foo);
    test_assert(!ecs_has(world, c, Foo));

    ecs_table_t *table_b = ecs_get_table(world, c);
    test_assert(table_b != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr == NULL);
    }

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_b);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 1);
    test_uint(entities.ids[0], c);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_clear(world, c);

    ecs_table_t *table_b = ecs_get_table(world, c);
    test_assert(table_b != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr == NULL);
    }

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_b);
        test_assert(pr == NULL);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new(world);
    ecs_table_t *root = ecs_get_table(world, c);

    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->entity, c);
        test_int(pr->count, 1);
    }

    ecs_delete(world, c);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr == NULL);
    }

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, root);
        test_assert(pr == NULL);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->entity, 0);
    test_int(pr->count, 2);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 2);
    test_uint(entities.ids[0], c1);
    test_uint(entities.ids[1], c2);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->entity, 0);
    test_int(pr->count, 2);

    ecs_remove(world, c1, EcsParent);

    test_uint(pr->entity, 0);
    test_int(pr->count, 1);

    ecs_remove(world, c2, EcsParent);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_delete_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->entity, 0);
    test_int(pr->count, 2);

    ecs_delete(world, c1);

    test_uint(pr->entity, 0);
    test_int(pr->count, 1);

    ecs_delete(world, c2);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_remove_parent_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->entity, 0);
    test_int(pr->count, 2);

    ecs_remove(world, c2, EcsParent);

    test_uint(pr->entity, 0);
    test_int(pr->count, 1);

    ecs_remove(world, c1, EcsParent);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_delete_children_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->entity, 0);
    test_int(pr->count, 2);

    ecs_delete(world, c2);

    test_uint(pr->entity, 0);
    test_int(pr->count, 1);

    ecs_delete(world, c1);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, p);
    test_int(entities.count, 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_set(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_nested_parent_set(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_replace(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_set(world, c, EcsParent, {p2});

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_replace_different_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, EcsParent, {p1});

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_set(world, c, EcsParent, {p2});

    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_remove(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, EcsParent, {p1});

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_remove(world, c, EcsParent);

    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(!ecs_has_id(world, c, ecs_pair(EcsParentDepth, EcsWildcard)));

    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 1)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_set_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_set(world, p1, EcsParent, {p2});

    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, EcsParent, {p1});
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_remove(world, p2, EcsParent);

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t p3 = ecs_new(world);
    ecs_set(world, p3, EcsParent, {p1});

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p3}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_set(world, p3, EcsParent, {p2});

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_reparent_different_depth(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, EcsParent, {p1});
    ecs_entity_t p3 = ecs_new(world);
    ecs_set(world, p3, EcsParent, {p1});

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p3}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_set(world, p3, EcsParent, {p2});

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 4)));
    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_set_parent_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p1}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));
    ecs_add_id(world, gc, EcsOrderedChildren);
    ecs_entity_t ggc = ecs_insert(world, ecs_value(EcsParent, {gc}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_set(world, p1, EcsParent, {p2});

    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    test_assert(!ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 4)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_remove_parent_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, EcsParent, {p1});
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));
    ecs_add_id(world, gc, EcsOrderedChildren);
    ecs_entity_t ggc = ecs_insert(world, ecs_value(EcsParent, {gc}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 4)));

    ecs_remove(world, p2, EcsParent);

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(!ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 4)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_reparent_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_entity_t p3 = ecs_new(world);
    ecs_set(world, p3, EcsParent, {p1});

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p3}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));
    ecs_add_id(world, gc, EcsOrderedChildren);
    ecs_entity_t ggc = ecs_insert(world, ecs_value(EcsParent, {gc}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 4)));

    ecs_set(world, p3, EcsParent, {p2});

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 4)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_reparent_different_depth_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);
    ecs_set(world, p2, EcsParent, {p1});
    ecs_entity_t p3 = ecs_new(world);
    ecs_set(world, p3, EcsParent, {p1});

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p3}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));
    ecs_add_id(world, gc, EcsOrderedChildren);
    ecs_entity_t ggc = ecs_insert(world, ecs_value(EcsParent, {gc}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    ecs_set(world, p3, EcsParent, {p2});

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 3)));
    test_assert(!ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 2)));

    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 4)));
    test_assert(!ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 3)));

    test_assert(ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 5)));
    test_assert(!ecs_has_id(world, ggc, ecs_value_pair(EcsParentDepth, 4)));

    ecs_fini(world);
}

void NonFragmentingChildOf_defer_delete_parent_and_base(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t b = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, c_1, EcsIsA, b);

    ecs_defer_begin(world);
    ecs_delete(world, p);
    ecs_delete(world, b);
    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, b));
    test_assert(ecs_is_alive(world, c_1));
    ecs_defer_end(world);
    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, b));
    test_assert(!ecs_is_alive(world, c_1));

    ecs_fini(world);
}

void NonFragmentingChildOf_defer_delete_parent_and_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Bar);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c_1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add(world, c_1, Bar);

    ecs_defer_begin(world);
    ecs_delete(world, p);
    ecs_delete(world, Bar);
    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, Bar));
    test_assert(ecs_is_alive(world, c_1));

    ecs_defer_end(world);
    test_assert(!ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, Bar));
    test_assert(!ecs_is_alive(world, c_1));

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new_w(world, Foo);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_1 = ecs_new_w(world, Foo);
    ecs_entity_t parent_2 = ecs_new_w(world, Foo);
    ecs_entity_t child_1 = ecs_insert(world, ecs_value(EcsParent, {parent_1}));
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_2}));

    ecs_entity_t src = ecs_get_target_for(world, child_1, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_1);

    src = ecs_get_target_for(world, child_2, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_2);

    ecs_fini(world);
}

void NonFragmentingChildOf_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new_w(world, Foo);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_entity_t src = 0;
    ecs_id_t id = 0;
    ecs_table_record_t *tr = NULL;
    test_assert(
        ecs_search_relation_for_entity(
            world, child, Foo, ecs_pair(EcsChildOf, EcsWildcard), true, 
                NULL, &src, &id, &tr) != -1);

    test_assert(src != 0);
    test_assert(src == parent);
    test_assert(id == Foo);
    test_assert(tr->hdr.table == ecs_get_table(world, parent));

    ecs_fini(world);
}

void NonFragmentingChildOf_search_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_1 = ecs_new_w(world, Foo);
    ecs_entity_t parent_2 = ecs_new_w(world, Foo);
    ecs_entity_t child_1 = ecs_insert(world, ecs_value(EcsParent, {parent_1}));
    ecs_entity_t child_2 = ecs_insert(world, ecs_value(EcsParent, {parent_2}));

    ecs_entity_t src = 0;
    ecs_id_t id = 0;
    ecs_table_record_t *tr = NULL;
    test_assert(
        ecs_search_relation_for_entity(
            world, child_1, Foo, ecs_pair(EcsChildOf, EcsWildcard), true, 
                NULL, &src, &id, &tr) != -1);

    test_assert(src != 0);
    test_assert(src == parent_1);
    test_assert(id == Foo);
    test_assert(tr->hdr.table == ecs_get_table(world, parent_1));

    test_assert(
        ecs_search_relation_for_entity(
            world, child_2, Foo, ecs_pair(EcsChildOf, EcsWildcard), true, 
                NULL, &src, &id, &tr) != -1);

    test_assert(src != 0);
    test_assert(src == parent_2);
    test_assert(id == Foo);
    test_assert(tr->hdr.table == ecs_get_table(world, parent_2));

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_2_lvls_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_2_lvls_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_childof_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_childof_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_parent_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_parent_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new_w(world, Foo);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_2_lvls_parent_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_add(world, parent_b, Foo);
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_self_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_add(world, child, Foo);

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == child);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_2_lvls_parent_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_2_lvls_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_childof_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_childof_parent_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_parent_childof_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_3_lvls_parent_parent_inherited(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Foo, (OnInstantiate, Inherit));

    ecs_entity_t base = ecs_new_w(world, Foo);
    ecs_entity_t parent_a = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for(world, child, EcsChildOf, Foo);
    test_assert(src != 0);
    test_assert(src == base);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard_2_lvls_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard_2_lvls_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_b}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard_3_lvls_childof_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard_3_lvls_childof_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t parent_b = ecs_new_w_pair(world, EcsChildOf, parent_a);
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard_3_lvls_parent_childof(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t parent_c = ecs_new_w_pair(world, EcsChildOf, parent_b);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_target_for_wildcard_3_lvls_parent_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    ecs_entity_t parent_a = ecs_new_w_pair(world, Rel, Tgt);
    ecs_entity_t parent_b = ecs_insert(world, ecs_value(EcsParent, {parent_a}));
    ecs_entity_t parent_c = ecs_insert(world, ecs_value(EcsParent, {parent_b}));
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent_c}));

    ecs_entity_t src = ecs_get_target_for_id(
        world, child, EcsChildOf, ecs_pair(Rel, EcsWildcard));
    test_assert(src != 0);
    test_assert(src == parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_parent_has_traversable_flag(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_record_t *r = ecs_record_find(world, parent);
    test_assert(r != NULL);
    test_bool(false, r->row & EcsEntityIsTraversable);

    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));
    test_bool(true, r->row & EcsEntityIsTraversable);

    ecs_record_t *child_r = ecs_record_find(world, child);
    test_assert(child_r != NULL);
    test_bool(false, child_r->row & EcsEntityIsTraversable);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_2_ordered_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root_a = ecs_new(world);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {root_a}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {root_a}));

    ecs_entities_t ent = ecs_get_ordered_children(world, root_a);
    test_int(ent.count, 2);
    test_uint(ent.ids[0], e1);
    test_uint(ent.ids[1], e2);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_2_ordered_children_mixed(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root_a = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, root_a);
    ecs_entity_t e2 = ecs_insert(world, ecs_value(EcsParent, {root_a}));

    ecs_entities_t ent = ecs_get_ordered_children(world, root_a);
    test_int(ent.count, 2);
    test_uint(ent.ids[0], e1);
    test_uint(ent.ids[1], e2);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_2_ordered_children_mixed_2(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root_a = ecs_new(world);

    ecs_entity_t e1 = ecs_insert(world, ecs_value(EcsParent, {root_a}));
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, root_a);

    ecs_entities_t ent = ecs_get_ordered_children(world, root_a);
    test_int(ent.count, 2);
    test_uint(ent.ids[0], e1);
    test_uint(ent.ids[1], e2);

    ecs_fini(world);
}

void NonFragmentingChildOf_3_levels_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1_1 = ecs_insert(world, ecs_value(EcsParent, {p0}));
    ecs_entity_t p1_2 = ecs_insert(world, ecs_value(EcsParent, {p0}));
    ecs_entity_t p2_1 = ecs_insert(world, ecs_value(EcsParent, {p1_1}));
    ecs_entity_t p2_2 = ecs_insert(world, ecs_value(EcsParent, {p1_2}));
    ecs_entity_t p3_1 = ecs_insert(world, ecs_value(EcsParent, {p2_1}));
    ecs_entity_t p3_2 = ecs_insert(world, ecs_value(EcsParent, {p2_2}));

    test_uint(p0, ecs_get_target(world, p1_1, EcsChildOf, 0));
    test_uint(p0, ecs_get_target(world, p1_2, EcsChildOf, 0));
    test_uint(p1_1, ecs_get_target(world, p2_1, EcsChildOf, 0));
    test_uint(p1_2, ecs_get_target(world, p2_2, EcsChildOf, 0));
    test_uint(p2_1, ecs_get_target(world, p3_1, EcsChildOf, 0));
    test_uint(p2_2, ecs_get_target(world, p3_2, EcsChildOf, 0));

    ecs_fini(world);
}

static int delete_count = 0;

static void CheckDeleteOrder(ecs_iter_t *it) {
    test_int(1, it->count);

    /* Make sure entity's parent is still alive */
    ecs_entity_t e = it->entities[0];
    ecs_entity_t parent = ecs_get_parent(it->world, e);
    test_assert(parent != 0);
    test_assert(ecs_is_alive(it->world, parent));

    /* Make sure children are deleted before entity */
    ecs_iter_t cit = ecs_children(it->world, e);
    test_bool(ecs_children_next(&cit), false);

    delete_count ++;
}

void NonFragmentingChildOf_3_levels_2_children_cleanup_order_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1_1 = ecs_insert(world, ecs_value(EcsParent, {p0}));
    ecs_entity_t p1_2 = ecs_insert(world, ecs_value(EcsParent, {p0}));
    ecs_entity_t p2_1 = ecs_insert(world, ecs_value(EcsParent, {p1_1}));
    ecs_entity_t p2_2 = ecs_insert(world, ecs_value(EcsParent, {p1_2}));
    ecs_entity_t p3_1 = ecs_insert(world, ecs_value(EcsParent, {p2_1}));
    ecs_entity_t p3_2 = ecs_insert(world, ecs_value(EcsParent, {p2_2}));

    ecs_add(world, p1_1, Foo);
    ecs_add(world, p1_2, Foo);
    ecs_add(world, p2_1, Foo);
    ecs_add(world, p2_2, Foo);
    ecs_add(world, p3_1, Foo);
    ecs_add(world, p3_2, Foo);

    ecs_observer(world, {
        .query.terms = {{ Foo }},
        .events = {EcsOnRemove},
        .callback = CheckDeleteOrder
    });

    ecs_delete(world, p0);

    test_int(delete_count, 6);

    ecs_fini(world);
}

void NonFragmentingChildOf_3_levels_2_children_cleanup_order_after_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p0 = ecs_new(world);
    ecs_entity_t p1_1 = ecs_insert(world, ecs_value(EcsParent, {p0}));
    ecs_entity_t p1_2 = ecs_insert(world, ecs_value(EcsParent, {p0}));
    ecs_entity_t p2_1 = ecs_insert(world, ecs_value(EcsParent, {p1_1}));
    ecs_entity_t p2_2 = ecs_insert(world, ecs_value(EcsParent, {p1_2}));
    ecs_entity_t p3_1 = ecs_insert(world, ecs_value(EcsParent, {p2_1}));
    ecs_entity_t p3_2 = ecs_insert(world, ecs_value(EcsParent, {p2_2}));

    ecs_add(world, p1_1, Foo);
    ecs_add(world, p1_2, Foo);
    ecs_add(world, p2_1, Foo);
    ecs_add(world, p2_2, Foo);
    ecs_add(world, p3_1, Foo);
    ecs_add(world, p3_2, Foo);

    ecs_observer(world, {
        .query.terms = {{ Foo }},
        .events = {EcsOnRemove},
        .callback = CheckDeleteOrder
    });

    ecs_fini(world);

    test_int(delete_count, 6);
}

void NonFragmentingChildOf_named_children_same_table_w_same_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_entity(world, { .name = "parent_a" });
    ecs_entity_t parent_b = ecs_entity(world, { .name = "parent_b" });

    ecs_entity_t child_a = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent_a }) )
    });

    ecs_entity_t child_b = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent_b }) )
    });

    test_assert(child_a != child_b);

    test_assert(ecs_lookup(world, "parent_a") == parent_a);
    test_assert(ecs_lookup(world, "parent_b") == parent_b);
    test_assert(ecs_lookup(world, "parent_a.child") == child_a);
    test_assert(ecs_lookup(world, "parent_b.child") == child_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_parent_to_entity_w_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});

    ecs_table_t *table = ecs_get_table(world, c);

    ecs_component_record_t *root_cr = flecs_components_get(world, ecs_childof(0));
    test_assert(root_cr != NULL);

    ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    test_assert(flecs_component_get_parent_record(root_cr, table) == NULL);

    ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_int(pr->count, 1);
    test_uint(pr->entity, c);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_parent_to_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, c, EcsParent, {p});

    ecs_table_t *table = ecs_get_table(world, c);

    ecs_component_record_t *root_cr = flecs_components_get(world, ecs_childof(0));
    test_assert(root_cr != NULL);

    ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    test_assert(flecs_component_get_parent_record(root_cr, table) == NULL);

    ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_int(pr->count, 1);
    test_uint(pr->entity, c);

    ecs_fini(world);
}

void NonFragmentingChildOf_reparent_to_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_add_id(world, c, EcsPrefab);

    ecs_table_t *table = ecs_get_table(world, c);

    ecs_component_record_t *root_cr = flecs_components_get(world, ecs_childof(0));
    test_assert(root_cr != NULL);

    ecs_component_record_t *cr_1 = flecs_components_get(world, ecs_childof(p_1));
    test_assert(cr_1 != NULL);

    test_assert(flecs_component_get_parent_record(root_cr, table) == NULL);

    ecs_parent_record_t *pr_1 = flecs_component_get_parent_record(cr_1, table);
    test_assert(pr_1 != NULL);
    test_int(pr_1->count, 1);
    test_uint(pr_1->entity, c);

    ecs_set(world, c, EcsParent, {p_2});

    ecs_component_record_t *cr_2 = flecs_components_get(world, ecs_childof(p_2));
    test_assert(cr_2 != NULL);

    ecs_parent_record_t *pr_2 = flecs_component_get_parent_record(cr_2, table);
    test_assert(pr_2 != NULL);
    test_int(pr_2->count, 1);
    test_uint(pr_2->entity, c);

    test_assert(flecs_component_get_parent_record(cr_1, table) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_parent_to_prefab_after_add_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);
    ecs_entity_t p_2 = ecs_new(world);
    ecs_entity_t c = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, c, EcsParent, {p_1});

    ecs_table_t *table = ecs_get_table(world, c);

    ecs_component_record_t *root_cr = flecs_components_get(world, ecs_childof(0));
    test_assert(root_cr != NULL);

    ecs_component_record_t *cr_1 = flecs_components_get(world, ecs_childof(p_1));
    test_assert(cr_1 != NULL);

    test_assert(flecs_component_get_parent_record(root_cr, table) == NULL);

    ecs_parent_record_t *pr_1 = flecs_component_get_parent_record(cr_1, table);
    test_assert(pr_1 != NULL);
    test_int(pr_1->count, 1);
    test_uint(pr_1->entity, c);

    ecs_set(world, c, EcsParent, {p_2});

    ecs_component_record_t *cr_2 = flecs_components_get(world, ecs_childof(p_2));
    test_assert(cr_2 != NULL);

    ecs_parent_record_t *pr_2 = flecs_component_get_parent_record(cr_2, table);
    test_assert(pr_2 != NULL);
    test_int(pr_2->count, 1);
    test_uint(pr_2->entity, c);

    test_assert(flecs_component_get_parent_record(cr_1, table) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_create_delete_mixed_tree(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p_1 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p_1);
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c4 = ecs_new_w_pair(world, EcsChildOf, p_1);

    ecs_delete(world, p_1);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, c3));
    test_assert(!ecs_is_alive(world, c4));

    ecs_fini(world);
}

void NonFragmentingChildOf_create_delete_tree_w_tgt_of_other_rel_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t p_1 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {c2}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {c3}));

    ecs_entity_t e = ecs_new_w_pair(world, Rel, c1);

    ecs_delete(world, p_1);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, c3));
    test_assert(!ecs_is_alive(world, c4));

    test_assert(!ecs_has_pair(world, e, Rel, c1));
    test_assert(!ecs_has_pair(world, e, Rel, EcsWildcard));

    ecs_fini(world);
}

void NonFragmentingChildOf_create_delete_tree_w_tgt_of_other_rel_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t p_1 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {c2}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {c3}));

    ecs_entity_t e = ecs_new_w_pair(world, Rel, c2);

    ecs_delete(world, p_1);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, c3));
    test_assert(!ecs_is_alive(world, c4));

    test_assert(!ecs_has_pair(world, e, Rel, c2));
    test_assert(!ecs_has_pair(world, e, Rel, EcsWildcard));

    ecs_fini(world);
}

void NonFragmentingChildOf_create_delete_tree_w_tgt_of_other_rel_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t p_1 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {c2}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {c3}));

    ecs_entity_t e = ecs_new_w_pair(world, Rel, c3);

    ecs_delete(world, p_1);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, c3));
    test_assert(!ecs_is_alive(world, c4));

    test_assert(!ecs_has_pair(world, e, Rel, c3));
    test_assert(!ecs_has_pair(world, e, Rel, EcsWildcard));

    ecs_fini(world);
}

void NonFragmentingChildOf_create_delete_tree_w_tgt_of_other_rel_4(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t p_1 = ecs_new(world);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p_1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {c2}));
    ecs_entity_t c4 = ecs_insert(world, ecs_value(EcsParent, {c3}));

    ecs_entity_t e = ecs_new_w_pair(world, Rel, c4);

    ecs_delete(world, p_1);

    test_assert(!ecs_is_alive(world, p_1));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, c3));
    test_assert(!ecs_is_alive(world, c4));

    test_assert(!ecs_has_pair(world, e, Rel, c4));
    test_assert(!ecs_has_pair(world, e, Rel, EcsWildcard));

    ecs_fini(world);
}

void NonFragmentingChildOf_deep_hierarchy(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t root = ecs_new(world), cur = root;
    for (int i = 0; i < FLECS_DAG_DEPTH_MAX; i ++) {
        ecs_entity_t e = ecs_new(world);
        ecs_set(world, e, EcsParent, {cur});
        cur = e;
    }

    test_int(ecs_count(world, EcsParent), FLECS_DAG_DEPTH_MAX);

    ecs_delete(world, root);

    test_int(ecs_count(world, EcsParent), 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup(world, "parent.child") == child);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });
    ecs_entity_t grandchild = ecs_entity(world, { 
        .name = "grandchild", 
        .set = ecs_values( ecs_value(EcsParent, { child }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup(world, "parent.child") == child);
    test_assert(ecs_lookup(world, "parent.child.grandchild") == grandchild);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_from(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup_from(world, parent, "child") == child);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_from_2_lvls(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });
    ecs_entity_t grandchild = ecs_entity(world, { 
        .name = "grandchild", 
        .set = ecs_values( ecs_value(EcsParent, { child }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup_from(world, parent, "child") == child);
    test_assert(ecs_lookup_from(world, parent, "child.grandchild") == grandchild);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_after_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent_a = ecs_entity(world, { .name = "parent_a" });
    ecs_entity_t parent_b = ecs_entity(world, { .name = "parent_b" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent_a }) )
    });

    test_assert(ecs_lookup(world, "parent_a") == parent_a);
    test_assert(ecs_lookup(world, "parent_a.child") == child);
    test_assert(ecs_lookup(world, "parent_b.child") == 0);

    ecs_set(world, child, EcsParent, { parent_b });

    test_assert(ecs_lookup(world, "parent_a.child") == 0);
    test_assert(ecs_lookup(world, "parent_b.child") == child);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_after_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup(world, "parent.child") == child);

    ecs_remove(world, child, EcsParent);

    test_assert(ecs_lookup(world, "parent.child") == 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup(world, "parent.child") == child);

    ecs_clear(world, child);

    test_assert(ecs_lookup(world, "parent.child") == 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_lookup_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_entity(world, { 
        .name = "child", 
        .set = ecs_values( ecs_value(EcsParent, { parent }) )
    });

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup(world, "parent.child") == child);

    ecs_delete(world, child);

    test_assert(ecs_lookup(world, "parent.child") == 0);

    ecs_fini(world);
}

void NonFragmentingChildOf_set_name_after_create(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    ecs_set_name(world, child, "child");

    test_assert(ecs_lookup(world, "parent") == parent);
    test_assert(ecs_lookup(world, "parent.child") != 0);
    test_assert(ecs_lookup(world, "parent.child") == child);
    test_assert(ecs_lookup_child(world, parent, "child") != 0);
    test_assert(ecs_lookup_child(world, parent, "child") == child);
    
    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_child_has_prefab_w_insert(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));

    test_assert(ecs_has_id(world, c, EcsPrefab));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_child_has_prefab_w_new_child(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_parent(world, p, NULL);

    test_assert(ecs_has_id(world, c, EcsPrefab));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_child_has_prefab_w_set(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new(world);
    ecs_set(world, c, EcsParent, {p});

    test_assert(ecs_has_id(world, c, EcsPrefab));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_child_nested_has_prefab_w_insert(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, EcsPrefab));
    test_assert(ecs_has_id(world, gc, EcsPrefab));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_child_nested_has_prefab_w_new_child(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new_w_parent(world, p, NULL);
    ecs_entity_t gc = ecs_new_w_parent(world, c, NULL);

    test_assert(ecs_has_id(world, c, EcsPrefab));
    test_assert(ecs_has_id(world, gc, EcsPrefab));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_child_nested_has_prefab_w_set(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_new(world);
    ecs_set(world, c, EcsParent, {p});
    ecs_entity_t gc = ecs_new(world);
    ecs_set(world, gc, EcsParent, {p});

    test_assert(ecs_has_id(world, c, EcsPrefab));
    test_assert(ecs_has_id(world, gc, EcsPrefab));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_variant_w_children(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child = ecs_new_w_parent(world, base, NULL);
    ecs_entity_t variant = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, variant, EcsIsA, base);

    test_assert(ecs_has_id(world, base, EcsPrefab));
    test_assert(ecs_has_id(world, base_child, EcsPrefab));
    test_assert(ecs_has_id(world, variant, EcsPrefab));

    ecs_entities_t entities = ecs_get_ordered_children(world, variant);
    test_int(entities.count, 1);
    test_assert(ecs_has_id(world, entities.ids[0], EcsPrefab));
    test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));

    ecs_fini(world);
}

void NonFragmentingChildOf_prefab_variant_w_nested_children(void) {
    ecs_world_t *world = ecs_mini();
    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t base_child = ecs_new_w_parent(world, base, NULL);
    ecs_entity_t base_grandchild = ecs_new_w_parent(world, base_child, NULL);
    ecs_entity_t variant = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, variant, EcsIsA, base);

    test_assert(ecs_has_id(world, base, EcsPrefab));
    test_assert(ecs_has_id(world, base_child, EcsPrefab));
    test_assert(ecs_has_id(world, base_grandchild, EcsPrefab));
    test_assert(ecs_has_id(world, variant, EcsPrefab));

    ecs_entities_t entities = ecs_get_ordered_children(world, variant);
    test_int(entities.count, 1);
    test_assert(ecs_has_id(world, entities.ids[0], EcsPrefab));
    test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));

    ecs_entities_t grand_children = ecs_get_ordered_children(world, entities.ids[0]);
    test_int(grand_children.count, 1);
    test_assert(ecs_has_id(world, grand_children.ids[0], EcsPrefab));
    test_assert(ecs_has_pair(world, grand_children.ids[0], EcsIsA, base_grandchild));

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_1_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    {
        const Position *p = ecs_get(world, i, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 1);
    
    ecs_entity_t i_c = entities.ids[0];
    test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
    {
        const EcsParent *i_cp = ecs_get(world, i_c, EcsParent);
        test_assert(i_cp != NULL);
        test_uint(i_cp->value, i);
    }
    {
        const Position *p = ecs_get(world, i_c, Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_2_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});
        ecs_entity_t p_c_1 = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c_1, Position, {11, 21});
        ecs_entity_t p_c_2 = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c_2, Position, {12, 22});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 2);
    
    test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, p_c_1));
    test_assert(ecs_has_pair(world, entities.ids[1], EcsIsA, p_c_2));

    {
        const EcsParent *i_cp = ecs_get(world, entities.ids[0], EcsParent);
        test_assert(i_cp != NULL);
        test_uint(i_cp->value, i);
    } {
        const EcsParent *i_cp = ecs_get(world, entities.ids[1], EcsParent);
        test_assert(i_cp != NULL);
        test_uint(i_cp->value, i);
    }

    {
        const Position *p = ecs_get(world, entities.ids[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    } {
        const Position *p = ecs_get(world, entities.ids[1], Position);
        test_assert(p != NULL);
        test_int(p->x, 12);
        test_int(p->y, 22);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_3_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});
        ecs_entity_t p_c_1 = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c_1, Position, {11, 21});
        ecs_entity_t p_c_2 = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c_2, Position, {12, 22});
        ecs_entity_t p_c_3 = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c_3, Position, {13, 23});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 3);
    
    test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, p_c_1));
    test_assert(ecs_has_pair(world, entities.ids[1], EcsIsA, p_c_2));
    test_assert(ecs_has_pair(world, entities.ids[2], EcsIsA, p_c_3));

    {
        const EcsParent *i_cp = ecs_get(world, entities.ids[0], EcsParent);
        test_assert(i_cp != NULL);
        test_uint(i_cp->value, i);
    } {
        const EcsParent *i_cp = ecs_get(world, entities.ids[1], EcsParent);
        test_assert(i_cp != NULL);
        test_uint(i_cp->value, i);
    } {
        const EcsParent *i_cp = ecs_get(world, entities.ids[2], EcsParent);
        test_assert(i_cp != NULL);
        test_uint(i_cp->value, i);
    }

    {
        const Position *p = ecs_get(world, entities.ids[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    } {
        const Position *p = ecs_get(world, entities.ids[1], Position);
        test_assert(p != NULL);
        test_int(p->x, 12);
        test_int(p->y, 22);
    } {
        const Position *p = ecs_get(world, entities.ids[2], Position);
        test_assert(p != NULL);
        test_int(p->x, 13);
        test_int(p->y, 23);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_w_dont_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsDontInherit);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});
        ecs_set(world, p_c, Velocity, {1, 2});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 1);
    
    ecs_entity_t i_c = entities.ids[0];
    test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
    test_assert(ecs_get_parent(world, i_c) == i);
    test_assert(!ecs_has(world, i_c, Velocity));
    {
        const Position *p = ecs_get(world, entities.ids[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_w_inherit(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});
        ecs_set(world, p_c, Velocity, {1, 2});

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 1);
    
    ecs_entity_t i_c = entities.ids[0];
    test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
    test_assert(ecs_get_parent(world, i_c) == i);
    test_assert(ecs_has(world, i_c, Velocity));
    test_assert(!ecs_owns(world, i_c, Velocity));
    {
        const Position *p = ecs_get(world, entities.ids[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    } {
        const Velocity *v = ecs_get(world, entities.ids[0], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_w_auto_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});
        ecs_set(world, p_c, Velocity, {1, 2});
        ecs_auto_override(world, p_c, Velocity);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 1);
    
    ecs_entity_t i_c = entities.ids[0];
    test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
    test_assert(ecs_get_parent(world, i_c) == i);
    test_assert(ecs_has(world, i_c, Velocity));
    test_assert(ecs_owns(world, i_c, Velocity));
    {
        const Position *p = ecs_get(world, entities.ids[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    } {
        const Velocity *v = ecs_get(world, entities.ids[0], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_w_isa(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_pair(world, ecs_id(Velocity), EcsOnInstantiate, EcsInherit);

    ecs_entity_t base = ecs_new(world);
    ecs_set(world, base, Velocity, {1, 2});

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});
        ecs_add_pair(world, p_c, EcsIsA, base);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(entities.count, 1);
    
    ecs_entity_t i_c = entities.ids[0];
    test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
    test_assert(ecs_get_parent(world, i_c) == i);
    test_assert(ecs_has(world, i_c, Velocity));
    test_assert(!ecs_owns(world, i_c, Velocity));
    test_assert(ecs_has_pair(world, i_c, EcsIsA, base));
    test_assert(!ecs_owns_pair(world, i_c, EcsIsA, base));
    {
        const Position *p = ecs_get(world, entities.ids[0], Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    } {
        const Velocity *v = ecs_get(world, entities.ids[0], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 1);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_twice(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});

    ecs_entity_t i1 = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(!ecs_has(world, i1, EcsTreeSpawner));
    {
        {
            const Position *p = ecs_get(world, i1, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        ecs_entities_t entities = ecs_get_ordered_children(world, i1);
        test_int(entities.count, 1);
        
        ecs_entity_t i_c = entities.ids[0];
        test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
        {
            const EcsParent *i_cp = ecs_get(world, i_c, EcsParent);
            test_assert(i_cp != NULL);
            test_uint(i_cp->value, i1);
        }
        {
            const Position *p = ecs_get(world, i_c, Position);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
    }

    ecs_entity_t i2 = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(!ecs_has(world, i2, EcsTreeSpawner));
    test_assert(ecs_get_table(world, i1) == ecs_get_table(world, i2));
    {
        {
            const Position *p = ecs_get(world, i2, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        ecs_entities_t entities = ecs_get_ordered_children(world, i2);
        test_int(entities.count, 1);
        
        ecs_entity_t i_c = entities.ids[0];
        test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
        {
            const EcsParent *i_cp = ecs_get(world, i_c, EcsParent);
            test_assert(i_cp != NULL);
            test_uint(i_cp->value, i2);
        }
        {
            const Position *p = ecs_get(world, i_c, Position);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_twice_different_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_set(world, p, Position, {10, 20});
        ecs_entity_t p_c = ecs_insert(world, ecs_value(EcsParent, { p }));
        ecs_set(world, p_c, Position, {11, 21});

    ecs_entity_t i1 = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(!ecs_has(world, i1, EcsTreeSpawner));
    {
        {
            const Position *p = ecs_get(world, i1, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        ecs_entities_t entities = ecs_get_ordered_children(world, i1);
        test_int(entities.count, 1);
        
        ecs_entity_t i_c = entities.ids[0];
        test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
        {
            const EcsParent *i_cp = ecs_get(world, i_c, EcsParent);
            test_assert(i_cp != NULL);
            test_uint(i_cp->value, i1);
        }
        {
            const Position *p = ecs_get(world, i_c, Position);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
    }

    ecs_entity_t i2 = ecs_new_w(world, Foo);
    ecs_add_pair(world, i2, EcsIsA, p);
    test_assert(!ecs_has(world, i2, EcsTreeSpawner));
    test_assert(ecs_get_table(world, i1) != ecs_get_table(world, i2));
    {
        {
            const Position *p = ecs_get(world, i2, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        ecs_entities_t entities = ecs_get_ordered_children(world, i2);
        test_int(entities.count, 1);
        
        ecs_entity_t i_c = entities.ids[0];
        test_assert(ecs_has_pair(world, i_c, EcsIsA, p_c));
        {
            const EcsParent *i_cp = ecs_get(world, i_c, EcsParent);
            test_assert(i_cp != NULL);
            test_uint(i_cp->value, i2);
        }
        {
            const Position *p = ecs_get(world, i_c, Position);
            test_assert(p != NULL);
            test_int(p->x, 11);
            test_int(p->y, 21);
        }
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_at_depth_1_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t i = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, i, EcsIsA, base);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_has_id(world, i, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_at_depth_2_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t p2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t i = ecs_insert(world, ecs_value(EcsParent, {p2}));
    ecs_add_pair(world, i, EcsIsA, base);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_has_id(world, i, ecs_value_pair(EcsParentDepth, 2)));
    test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_at_depth_n_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new(world);
    int32_t depth = FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE + 1;
    for (int i = 0; i < depth; i ++) {
        p = ecs_insert(world, ecs_value(EcsParent, {p}));
    }
    ecs_entity_t i = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_pair(world, i, EcsIsA, base);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_has_id(world, i, ecs_value_pair(EcsParentDepth, depth + 1)));
    test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, depth + 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_at_depth_1_w_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t i = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add_pair(world, i, EcsIsA, base);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_at_depth_2_w_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new(world);
    p = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t i = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add_pair(world, i, EcsIsA, base);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 3)));

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_tree_at_depth_n_w_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new(world);
    int32_t depth = FLECS_TREE_SPAWNER_DEPTH_CACHE_SIZE + 1;
    for (int i = 0; i < depth; i ++) {
        p = ecs_insert(world, ecs_value(EcsParent, {p}));
    }
    ecs_entity_t i = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_add_pair(world, i, EcsIsA, base);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, depth + 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_reparent_instantiated_tree_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);

    {
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 1)));
    }

    ecs_entity_t p = ecs_new(world);
    ecs_set(world, i, EcsParent, {p});

    {
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 2)));
    }

    ecs_remove(world, i, EcsParent);

    {
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 1)));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_reparent_instantiated_tree_w_childof(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);

    {
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 1)));
    }

    ecs_entity_t p = ecs_new(world);
    ecs_add_pair(world, i, EcsChildOf, p);

    {
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 2)));
    }

    ecs_remove_pair(world, i, EcsChildOf, EcsWildcard);

    {
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_assert(ecs_has_id(world, entities.ids[0], ecs_value_pair(EcsParentDepth, 1)));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_instance_after_remove_all_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_add(world, c, Foo);

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 1);
        test_assert(ecs_has(world, entities.ids[0], Foo));

        ecs_remove_all(world, Foo);

        test_assert(!ecs_has(world, entities.ids[0], Foo));
    }

    test_assert(ecs_is_alive(world, c));
    test_assert(ecs_has(world, c, Foo));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 1);
        test_assert(ecs_has(world, entities.ids[0], Foo));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_instance_after_delete_with_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_add(world, c1, Foo);
    /* ecs_entity_t c2 = */ ecs_insert(world, ecs_value(EcsParent, {base}));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 2);
        test_assert(ecs_has(world, entities.ids[0], Foo));
        test_assert(!ecs_has(world, entities.ids[1], Foo));

        ecs_delete_with(world, Foo);

        entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 1);
        test_assert(ecs_is_alive(world, entities.ids[0]));
        test_assert(!ecs_has(world, entities.ids[0], Foo));
    }

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 2);
        test_assert(ecs_has(world, entities.ids[0], Foo));
        test_assert(!ecs_has(world, entities.ids[1], Foo));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_instance_after_shrink(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_add(world, c1, Foo);
    /* ecs_entity_t c2 = */ ecs_insert(world, ecs_value(EcsParent, {base}));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 2);
        test_assert(ecs_has(world, entities.ids[0], Foo));
        test_assert(!ecs_has(world, entities.ids[1], Foo));

        ecs_delete(world, i);
    }

    ecs_shrink(world);

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 2);
        test_assert(ecs_has(world, entities.ids[0], Foo));
        test_assert(!ecs_has(world, entities.ids[1], Foo));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_instantiate_instance_after_delete_empty_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_add(world, c1, Foo);
    /* ecs_entity_t c2 = */ ecs_insert(world, ecs_value(EcsParent, {base}));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 2);
        test_assert(ecs_has(world, entities.ids[0], Foo));
        test_assert(!ecs_has(world, entities.ids[1], Foo));

        ecs_delete(world, i);
    }

    ecs_delete_empty_tables(world, &(ecs_delete_empty_tables_desc_t){ .delete_generation = 1 });
    ecs_delete_empty_tables(world, &(ecs_delete_empty_tables_desc_t){ .delete_generation = 1 });

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(entities.count, 2);
        test_assert(ecs_has(world, entities.ids[0], Foo));
        test_assert(!ecs_has(world, entities.ids[1], Foo));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_get_instance_child_from_prefab(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_get_target(world, i, c1, 0) == entities.ids[0]);
    test_assert(ecs_get_target(world, i, c2, 0) == entities.ids[1]);
    test_assert(ecs_get_target(world, i, c3, 0) == entities.ids[2]);

    ecs_fini(world);
}

void NonFragmentingChildOf_get_instance_child_from_prefab_variant(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_entity_t c3 = ecs_insert(world, ecs_value(EcsParent, {base}));

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, p, EcsIsA, base);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_assert(ecs_get_target(world, i, c1, 0) == entities.ids[0]);
    test_assert(ecs_get_target(world, i, c2, 0) == entities.ids[1]);
    test_assert(ecs_get_target(world, i, c3, 0) == entities.ids[2]);

    ecs_fini(world);
}

void NonFragmentingChildOf_named_prefab_hierarchy(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set_name(world, base, "Base");

    ecs_entity_t base_child = ecs_insert(world, ecs_value(EcsParent, {base}));
    ecs_set_name(world, base_child, "BaseChild");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_get_name(world, i) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(1, entities.count);
    test_assert(ecs_get_name(world, entities.ids[0]) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_named_prefab_variant_hierarchy(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set_name(world, base, "Base");

    ecs_entity_t base_child = ecs_new_w_parent(world, base, NULL);
    ecs_set_name(world, base_child, "BaseChild");
    ecs_add(world, base_child, Foo);

    ecs_entity_t variant = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, variant, EcsIsA, base);

    ecs_entity_t variant_child = ecs_new_w_parent(world, variant, NULL);
    ecs_set_name(world, variant_child, "VariantChild");

    ecs_entity_t variant_base_child = ecs_lookup_child(world, variant, "BaseChild");
    test_assert(variant_base_child != 0);
    test_assert(variant_base_child != base_child);
    test_assert(ecs_has(world, variant_base_child, Foo));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        test_assert(ecs_get_name(world, i) == NULL);

        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(1, entities.count);
        test_assert(ecs_get_name(world, entities.ids[0]) == NULL);
        test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));
    }

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, variant);
        test_assert(ecs_get_name(world, i) == NULL);

        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(2, entities.count);
        test_assert(ecs_get_name(world, entities.ids[0]) == NULL);
        test_assert(ecs_get_name(world, entities.ids[1]) == NULL);
        test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));
        test_assert(ecs_has_pair(world, entities.ids[1], EcsIsA, variant_child));
        test_assert(ecs_has(world, entities.ids[0], Foo));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_named_prefab_hierarchy_new_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set_name(world, base, "Base");

    ecs_entity_t base_child = ecs_new_w_parent(world, base, "BaseChild");

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_get_name(world, i) == NULL);

    ecs_entities_t entities = ecs_get_ordered_children(world, i);
    test_int(1, entities.count);
    test_assert(ecs_get_name(world, entities.ids[0]) == NULL);
    test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));

    ecs_fini(world);
}

void NonFragmentingChildOf_named_prefab_variant_hierarchy_new_w_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t base = ecs_new_w_id(world, EcsPrefab);
    ecs_set_name(world, base, "Base");

    ecs_entity_t base_child = ecs_new_w_parent(world, base, "BaseChild");
    ecs_add(world, base_child, Foo);

    ecs_entity_t variant = ecs_new_w_id(world, EcsPrefab);
    ecs_add_pair(world, variant, EcsIsA, base);

    ecs_entity_t variant_base_child = ecs_lookup_child(world, variant, "BaseChild");
    test_assert(variant_base_child != 0);
    test_assert(variant_base_child != base_child);
    test_assert(ecs_has(world, variant_base_child, Foo));

    ecs_entity_t variant_child = ecs_new_w_parent(world, variant, "VariantChild");
    test_assert(variant_base_child == ecs_new_w_parent(world, variant, "BaseChild"));

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, base);
        test_assert(ecs_get_name(world, i) == NULL);

        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(1, entities.count);
        test_assert(ecs_get_name(world, entities.ids[0]) == NULL);
        test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));
    }

    {
        ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, variant);
        test_assert(ecs_get_name(world, i) == NULL);

        ecs_entities_t entities = ecs_get_ordered_children(world, i);
        test_int(2, entities.count);
        test_assert(ecs_get_name(world, entities.ids[0]) == NULL);
        test_assert(ecs_get_name(world, entities.ids[1]) == NULL);
        test_assert(ecs_has_pair(world, entities.ids[0], EcsIsA, base_child));
        test_assert(ecs_has_pair(world, entities.ids[1], EcsIsA, variant_child));
        test_assert(ecs_has(world, entities.ids[0], Foo));
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent_nested(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc1 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc2 = ecs_insert(world, ecs_value(EcsParent, {c2}));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(!ecs_is_alive(world, gc2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent_nested_2(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc1 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc2 = ecs_insert(world, ecs_value(EcsParent, {c2}));

    ecs_delete_with(world, ecs_childof(c1));

    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, c1));
    test_assert(ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(ecs_is_alive(world, gc2));

    ecs_delete_with(world, ecs_childof(c2));

    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, c1));
    test_assert(ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(!ecs_is_alive(world, gc2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent_nested_3(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc1 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc2 = ecs_insert(world, ecs_value(EcsParent, {c2}));

    ecs_delete_with(world, ecs_childof(c1));

    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, c1));
    test_assert(ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(ecs_is_alive(world, gc2));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(!ecs_is_alive(world, gc2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent_nested_4(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new(world);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc1 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc2 = ecs_insert(world, ecs_value(EcsParent, {c2}));

    ecs_delete_with(world, ecs_childof(c1));

    test_assert(ecs_is_alive(world, p));
    test_assert(ecs_is_alive(world, c1));
    test_assert(ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(ecs_is_alive(world, gc2));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(!ecs_is_alive(world, gc2));

    ecs_fini(world);
}

static void DummyObserver(ecs_iter_t *it) { }

void NonFragmentingChildOf_delete_with_parent_w_up_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_observer(world, {
        .query.terms = {{ Foo, .src.id = EcsUp }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = DummyObserver
    });

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent_nested_w_up_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_observer(world, {
        .query.terms = {{ Foo, .src.id = EcsUp }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = DummyObserver
    });

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc1 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_entity_t gc2 = ecs_insert(world, ecs_value(EcsParent, {c2}));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(!ecs_is_alive(world, gc2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_with_parent_mixed_nested_w_up_observer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_observer(world, {
        .query.terms = {{ Foo, .src.id = EcsUp }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = DummyObserver
    });

    ecs_entity_t p = ecs_new_w(world, Foo);
    ecs_entity_t c1 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc1 = ecs_insert(world, ecs_value(EcsParent, {c1}));
    ecs_entity_t c2 = ecs_new_w_pair(world, EcsChildOf, p);
    ecs_entity_t gc2 = ecs_insert(world, ecs_value(EcsParent, {c2}));

    ecs_delete_with(world, ecs_childof(p));

    test_assert(ecs_is_alive(world, p));
    test_assert(!ecs_is_alive(world, c1));
    test_assert(!ecs_is_alive(world, c2));
    test_assert(!ecs_is_alive(world, gc1));
    test_assert(!ecs_is_alive(world, gc2));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_tree_1(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, AlsoKill);

    ecs_entity_t container = ecs_new(world);
    ecs_entity_t a = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, EcsParent, {a});
    
    ecs_add_pair(world, a, AlsoKill, e);
    
    ecs_entity_t e_1 = ecs_new(world);
    ecs_set(world, e_1, EcsParent, {e});

    ecs_entity_t e_2 = ecs_new(world);
    ecs_set(world, e_2, EcsParent, {e});

    ecs_entity_t e_3 = ecs_new(world);
    ecs_set(world, e_3, EcsParent, {e});

    ecs_add_pair(world, a, EcsChildOf, container);

    ecs_delete(world, e);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));
    
    test_assert(ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, container));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_tree_2(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, AlsoKill);

    ecs_entity_t container = ecs_new(world);
    ecs_entity_t a = ecs_new(world);

    ecs_entity_t e = ecs_new(world);
    ecs_set(world, e, EcsParent, {a});
    
    ecs_add_pair(world, a, AlsoKill, e);
    
    ecs_entity_t e_1 = ecs_new(world);
    ecs_set(world, e_1, EcsParent, {e});

    ecs_entity_t e_2 = ecs_new(world);
    ecs_set(world, e_2, EcsParent, {e});

    ecs_entity_t e_3 = ecs_new(world);
    ecs_set(world, e_3, EcsParent, {e});

    ecs_defer_begin(world);
    ecs_add_pair(world, a, EcsChildOf, container);
    ecs_delete(world, e);
    ecs_defer_end(world);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, e_1));
    test_assert(!ecs_is_alive(world, e_2));
    test_assert(!ecs_is_alive(world, e_3));
    
    test_assert(ecs_is_alive(world, a));
    test_assert(ecs_is_alive(world, container));

    ecs_fini(world);
}

static
void on_remove_rel(ecs_iter_t* it)
{
    const ecs_id_t id = it->ids[0];
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t also_kill_ent = ecs_pair_second(it->world, id);
        ecs_delete(it->world, also_kill_ent);
    }
}

void NonFragmentingChildOf_delete_tree_3(void) {
    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Rel);

    ecs_observer(world, {
        .query.terms = {{ ecs_pair(Rel, EcsWildcard) }},
        .events = {EcsOnRemove},
        .callback = on_remove_rel
    });

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t e = ecs_new(world);
    ecs_entity_t e_1 = ecs_new(world);

    ecs_add(world, e, Foo);

    ecs_set(world, e, EcsParent, {a});

    ecs_add_pair(world, a, ecs_id(Rel), e);

    ecs_set(world, e_1, EcsParent, {e});

    ecs_delete(world, e);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, e_1));
    test_assert(ecs_is_alive(world, a));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_tree_4(void) {
    install_test_abort();

    ecs_world_t* world = ecs_mini();

    ECS_TAG(world, Rel);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t e = ecs_new(world);

    ecs_set(world, e, EcsParent, {a});

    test_expect_abort(); // cycle
    ecs_set(world, a, EcsParent, {e});
}

void NonFragmentingChildOf_delete_tree_5(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new_w_parent(world, a, NULL);
    ecs_entity_t c = ecs_new_w_parent(world, b, NULL);

    ecs_add_id(world, b, EcsOrderedChildren);

    ecs_delete(world, a);

    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));
    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void NonFragmentingChildOf_delete_tree_6(void) {
    ecs_world_t* world = ecs_mini();
    
    ECS_TAG(world, Rel);

    ecs_entity_t a = ecs_new(world);
    ecs_entity_t b = ecs_new_w_parent(world, a, NULL);
    ecs_add_pair(world, a, Rel, b);

    ecs_entity_t c = ecs_new_w_parent(world, b, NULL);

    ecs_add_id(world, b, EcsOrderedChildren);

    ecs_delete(world, a);

    test_assert(!ecs_is_alive(world, a));
    test_assert(!ecs_is_alive(world, b));
    test_assert(!ecs_is_alive(world, c));

    ecs_fini(world);
}

void NonFragmentingChildOf_add_parent_to_childof_child(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent_a);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent_b));

    ecs_set(world, child, EcsParent, { parent_b });

    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_b));
    test_assert(ecs_has_pair(world, child, EcsParentDepth, EcsWildcard));

    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent_b);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_childof_to_parent_child(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t parent_b = ecs_new(world);
    ecs_entity_t child = ecs_new_w_parent(world, parent_a, NULL);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent_b));

    ecs_add_pair(world, child, EcsChildOf, parent_b);

    test_assert(!ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_b));
    test_assert(!ecs_has_pair(world, child, EcsParentDepth, EcsWildcard));

    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_parent_to_childof_child_same_parent(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent_a);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(!ecs_has(world, child, EcsParent));

    ecs_set(world, child, EcsParent, { parent_a });

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(ecs_has_pair(world, child, EcsParentDepth, EcsWildcard));

    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent_a);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_childof_to_parent_child_same_parent(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent_a = ecs_new(world);
    ecs_entity_t child = ecs_new_w_parent(world, parent_a, NULL);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(ecs_has(world, child, EcsParent));

    ecs_add_pair(world, child, EcsChildOf, parent_a);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent_a));
    test_assert(!ecs_has_pair(world, child, EcsParentDepth, EcsWildcard));

    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_defer_new_w_parent(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_defer_begin(world);
    ecs_entity_t child = ecs_new_w_parent(world, parent, NULL);
    test_assert(child != 0);
    ecs_defer_end(world);

    test_assert(ecs_has(world, child, EcsParent));
    
    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_defer_new_w_parent_w_name(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_defer_begin(world);
    ecs_entity_t child = ecs_new_w_parent(world, parent, "foo");
    test_assert(child != 0);
    ecs_defer_end(world);

    test_assert(ecs_has(world, child, EcsParent));
    test_str(ecs_get_name(world, child), "foo");
    
    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_defer_new_w_parent_w_same_name_twice(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_defer_begin(world);
    ecs_entity_t child = ecs_new_w_parent(world, parent, "foo");
    test_assert(child != 0);
    test_assert(child == ecs_new_w_parent(world, parent, "foo"));
    ecs_defer_end(world);

    test_assert(ecs_has(world, child, EcsParent));
    test_str(ecs_get_name(world, child), "foo");
    
    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_new_w_parent_w_same_name_twice(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t child = ecs_new_w_parent(world, parent, "foo");
    test_assert(child != 0);
    test_assert(child == ecs_new_w_parent(world, parent, "foo"));
    
    test_str(ecs_get_name(world, child), "foo");
    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_new_w_parent_from_stage(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_world_t *s = ecs_get_stage(world, 0);
    test_assert(s != NULL);
    test_assert(s != world);

    ecs_entity_t child = ecs_new_w_parent(s, parent, NULL);

    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent);

    ecs_fini(world);
}

void NonFragmentingChildOf_new_w_parent_w_name_from_stage(void) {
    ecs_world_t* world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    ecs_world_t *s = ecs_get_stage(world, 0);
    test_assert(s != NULL);
    test_assert(s != world);

    ecs_entity_t child = ecs_new_w_parent(s, parent, "foo");

    test_str(ecs_get_name(world, child), "foo");
    const EcsParent *p = ecs_get(world, child, EcsParent);
    test_assert(p != NULL);
    test_uint(p->value, parent);

    ecs_fini(world);
}
