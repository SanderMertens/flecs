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
