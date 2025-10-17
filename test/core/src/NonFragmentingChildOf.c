#include <core.h>

void NonFragmentingChildOf_set_parent_no_ordered_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    test_assert(ecs_has_id(world, parent, EcsOrderedChildren));

    const ecs_entities_t children = ecs_get_ordered_children(world, parent);
    test_int(children.count, 1);
    test_int(children.ids[0], child);

    ecs_fini(world);
}

void NonFragmentingChildOf_add_ordered_children_before_set_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t child = ecs_insert(world, ecs_value(EcsParent, {parent}));

    test_assert(ecs_has_id(world, parent, EcsOrderedChildren));

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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c);
    test_assert(table != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->first_entity, c);
    test_int(pr->count, 1);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_set_parent_after_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_new_w_id(world, Foo);

    {
        ecs_table_t *table = ecs_get_table(world, c);
        test_assert(table != NULL);

        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
        test_assert(pr == NULL);
    }

    ecs_set(world, c, EcsParent, {p});

    {
        ecs_table_t *table = ecs_get_table(world, c);
        test_assert(table != NULL);

        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
        test_assert(pr != NULL);

        test_uint(pr->first_entity, c);
        test_int(pr->count, 1);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_add(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->first_entity, c);
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

        test_uint(pr->first_entity, c);
        test_int(pr->count, 1);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_remove(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->first_entity, c);
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

        test_uint(pr->first_entity, c);
        test_int(pr->count, 1);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_remove_all(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->first_entity, c);
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

        test_uint(pr->first_entity, c);
        test_int(pr->count, 1);
    }

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_new_w(world, Foo);
    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->first_entity, c);
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

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_after_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c = ecs_new(world);
    ecs_table_t *root = ecs_get_table(world, c);

    ecs_set(world, c, EcsParent, {p});
    ecs_table_t *table_a = ecs_get_table(world, c);
    test_assert(table_a != NULL);

    {
        const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table_a);
        test_assert(pr != NULL);

        test_uint(pr->first_entity, c);
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

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 2);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 2);

    ecs_remove(world, c1, EcsParent);

    test_uint(pr->first_entity, c2);
    test_int(pr->count, 1);

    ecs_remove(world, c2, EcsParent);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_delete_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 2);

    ecs_delete(world, c1);

    test_uint(pr->first_entity, c2);
    test_int(pr->count, 1);

    ecs_delete(world, c2);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_remove_parent_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 2);

    ecs_remove(world, c2, EcsParent);

    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_remove(world, c1, EcsParent);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_table_child_count_n_children_delete_children_reverse(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    const ecs_component_record_t *cr = flecs_components_get(world, ecs_childof(p));
    test_assert(cr != NULL);

    ecs_entity_t c1 = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_table_t *table = ecs_get_table(world, c1);
    test_assert(table != NULL);

    const ecs_parent_record_t *pr = flecs_component_get_parent_record(cr, table);
    test_assert(pr != NULL);
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_entity_t c2 = ecs_insert(world, ecs_value(EcsParent, {p}));
    test_uint(pr->first_entity, c1);
    test_int(pr->count, 2);

    ecs_delete(world, c2);

    test_uint(pr->first_entity, c1);
    test_int(pr->count, 1);

    ecs_delete(world, c1);

    test_assert(flecs_component_get_parent_record(cr, table) == NULL);

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_set(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_nested_parent_set(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t c = ecs_insert(world, ecs_value(EcsParent, {p}));
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity_t gc = ecs_insert(world, ecs_value(EcsParent, {c}));

    test_assert(ecs_has_id(world, c, ecs_value_pair(EcsParentDepth, 1)));
    test_assert(ecs_has_id(world, gc, ecs_value_pair(EcsParentDepth, 2)));

    ecs_fini(world);
}

void NonFragmentingChildOf_depth_after_parent_replace(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_set(world, p2, EcsParent, {p1});
    ecs_entity_t p3 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p3 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p1 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_entity_t p2 = ecs_new_w_id(world, EcsOrderedChildren);
    ecs_set(world, p2, EcsParent, {p1});
    ecs_entity_t p3 = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

    ecs_entity_t p = ecs_new_w_id(world, EcsOrderedChildren);
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

void NonFragmentingChildOf_observer_order_after_delete(void) {
    // Implement testcase
}

void NonFragmentingChildOf_observer_order_after_deferred_delete(void) {
    // Implement testcase
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
