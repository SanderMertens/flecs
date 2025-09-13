#include <core.h>

void OrderedChildren_iter_no_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_iter_t it = ecs_children(world, parent);
    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void OrderedChildren_children_1_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    ecs_iter_t it = ecs_children(world, parent);
    test_bool(true, ecs_children_next(&it));
    test_int(3, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(e3, it.entities[2]);

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void OrderedChildren_children_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    ecs_iter_t it = ecs_children(world, parent);
    test_bool(true, ecs_children_next(&it));
    test_int(3, it.count);
    test_uint(e1, it.entities[0]);
    test_uint(e2, it.entities[1]);
    test_uint(e3, it.entities[2]);

    test_bool(false, ecs_children_next(&it));

    ecs_fini(world);
}

void OrderedChildren_children_1_table_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e1);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e3);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_1_table_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e1);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e3);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_1_table_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e2, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e1, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_1_table_remove_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e2, EcsChildOf, EcsWildcard);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e1, EcsChildOf, EcsWildcard);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e3, EcsChildOf, EcsWildcard);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_1_table_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e1, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e3, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_1_table_reparent_ordered(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_add_id(world, parent_2, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e1, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e3, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    {
        ecs_iter_t it = ecs_children(world, parent_2);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e1, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_2_tables_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e1);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_delete(world, e3);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_2_tables_clear(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e1);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_clear(world, e3);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_2_tables_remove_parent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e2, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e1, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e3, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_2_tables_remove_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e2, EcsChildOf, EcsWildcard);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e1, EcsChildOf, EcsWildcard);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_pair(world, e3, EcsChildOf, EcsWildcard);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_2_tables_reparent(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e1, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e3, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_children_2_tables_reparent_ordered(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_entity_t parent_2 = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);
    ecs_add_id(world, parent_2, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e2, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e1, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(1, it.count);
        test_uint(e3, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_add_pair(world, e3, EcsChildOf, parent_2);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    {
        ecs_iter_t it = ecs_children(world, parent_2);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e1, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_delete_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_delete(world, parent);
    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_fini(world);
}

void OrderedChildren_delete_with_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_delete_with(world, ecs_childof(parent));
    test_assert(ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_delete_with_tag_all_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    ecs_delete_with(world, Foo);
    test_assert(ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_delete_with_tag_some_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e4 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e4, Bar);
    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e1, Foo);

    ecs_delete_with(world, Foo);
    test_assert(ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));
    test_assert(ecs_is_alive(world, e4));

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(2, it.count);
        test_uint(e2, it.entities[0]);
        test_uint(e4, it.entities[1]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_add_remove_ordered_children_after_in_use(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e1, Foo);
    ecs_add(world, e2, Bar);
    ecs_add(world, e3, Foo);

    ecs_add_id(world, parent, EcsOrderedChildren);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);
        test_uint(e2, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_entity_t children[] = {e1, e2, e3};

    ecs_set_child_order(world, parent, children, 3);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_remove_id(world, parent, EcsOrderedChildren);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_int(2, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e3, it.entities[1]);

        test_bool(true, ecs_children_next(&it));
        test_int(1, it.count);
        test_uint(e2, it.entities[0]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_add_remove_ordered_children_no_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_entity_t parent = ecs_new(world);

    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_remove_id(world, parent, EcsOrderedChildren);

    ecs_fini(world);

    test_assert(true);
}

void OrderedChildren_change_order_no_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_set_child_order(world, parent, NULL, 0);

    test_assert(true); // no crash

    ecs_fini(world);
}

void OrderedChildren_change_order_mismatching_child_count(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    /* ecs_entity_t e2 = */ ecs_new_w_pair(world, EcsChildOf, parent);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_entity_t children[] = {e1};

    test_expect_abort();
    ecs_set_child_order(world, parent, children, 1);
}

void OrderedChildren_change_order_mismatching_child_id(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    /* ecs_entity_t e2 = */ ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e4 = ecs_new(world);

    ecs_entity_t children[] = {e3, e4, e1};

    test_expect_abort();
    ecs_set_child_order(world, parent, children, 3);
}

void OrderedChildren_change_order_double_child_id(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    /* ecs_entity_t e2 = */ ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_entity_t children[] = {e3, e3, e1};

    test_expect_abort();
    ecs_set_child_order(world, parent, children, 3);
}

void OrderedChildren_change_order_null_children_nonzero_count(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    /* ecs_entity_t e1 = */ ecs_new_w_pair(world, EcsChildOf, parent);
    /* ecs_entity_t e2 = */ ecs_new_w_pair(world, EcsChildOf, parent);
    /* ecs_entity_t e3 = */ ecs_new_w_pair(world, EcsChildOf, parent);

    test_expect_abort();
    ecs_set_child_order(world, parent, NULL, 3);
}

void OrderedChildren_change_order_not_null_children_zero_count(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t children[] = {10};

    test_expect_abort();
    ecs_set_child_order(world, parent, children, 0);
}

void OrderedChildren_change_order_on_parent_without_ordered_children(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    test_expect_abort();
    ecs_set_child_order(world, parent, NULL, 0);
}

void OrderedChildren_change_order(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e1, it.entities[0]);
        test_uint(e2, it.entities[1]);
        test_uint(e3, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_entity_t children[] = {e3, e1, e2};
    ecs_set_child_order(world, parent, children, 3);

    {
        ecs_iter_t it = ecs_children(world, parent);
        test_bool(true, ecs_children_next(&it));
        test_assert(it.table == NULL);
        test_int(3, it.count);
        test_uint(e3, it.entities[0]);
        test_uint(e1, it.entities[1]);
        test_uint(e2, it.entities[2]);
        test_bool(false, ecs_children_next(&it));
    }

    ecs_fini(world);
}

void OrderedChildren_get_ordered_children_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entities_t entities = ecs_get_ordered_children(world, parent);
    test_int(entities.count, 0);
    test_int(entities.alive_count, 0);

    ecs_fini(world);
}

void OrderedChildren_get_ordered_children_invalid(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new(world);

    test_expect_abort();
    ecs_get_ordered_children(world, parent);
}

void OrderedChildren_get_ordered_children(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    ecs_entities_t entities = ecs_get_ordered_children(world, parent);
    test_int(entities.count, 3);
    test_int(entities.alive_count, 3);
    test_int(entities.ids[0], e1);
    test_int(entities.ids[1], e2);
    test_int(entities.ids[2], e3);

    ecs_fini(world);
}

void OrderedChildren_get_ordered_children_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t parent = ecs_new(world);
    ecs_add_id(world, parent, EcsOrderedChildren);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, parent);

    ecs_add(world, e3, Foo);
    ecs_add(world, e2, Foo);
    ecs_add(world, e1, Foo);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    ecs_entities_t entities = ecs_get_ordered_children(stage, parent);
    test_int(entities.count, 3);
    test_int(entities.alive_count, 3);
    test_int(entities.ids[0], e1);
    test_int(entities.ids[1], e2);
    test_int(entities.ids[2], e3);

    ecs_fini(world);
}

void OrderedChildren_ordered_children_w_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, e, EcsOrderedChildren);

    ecs_entity(world, { .parent = e, .name = "a" });
    ecs_entity(world, { .parent = e, .name = "b" });
    ecs_entity(world, { .parent = e, .name = "c" });

    ecs_entities_t children = ecs_get_ordered_children(world, e);
    test_int(children.count, 3);
    test_str(ecs_get_name(world, children.ids[0]), "a");
    test_str(ecs_get_name(world, children.ids[1]), "b");
    test_str(ecs_get_name(world, children.ids[2]), "c");

    ecs_delete(world, e);

    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") == 0);
    test_assert(ecs_lookup(world, "c") == 0);

    ecs_fini(world);
}

void OrderedChildren_get_ordered_children_from_prefab_instance_no_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_id(world, i, EcsOrderedChildren));

    ecs_entities_t children = ecs_get_ordered_children(world, i);
    test_int(children.count, 0);

    ecs_fini(world);
}

void OrderedChildren_get_ordered_children_from_prefab_instance_3_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity(world, { .parent = p, .name = "a" });
    ecs_entity(world, { .parent = p, .name = "b" });
    ecs_entity(world, { .parent = p, .name = "c" });

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_id(world, i, EcsOrderedChildren));

    ecs_entities_t children = ecs_get_ordered_children(world, i);
    test_int(children.count, 3);
    test_str(ecs_get_name(world, children.ids[0]), "a");
    test_str(ecs_get_name(world, children.ids[1]), "b");
    test_str(ecs_get_name(world, children.ids[2]), "c");

    ecs_fini(world);
}

void OrderedChildren_get_ordered_children_from_prefab_instance_nested_children(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_new_w_id(world, EcsPrefab);
    ecs_add_id(world, p, EcsOrderedChildren);

    ecs_entity(world, { .parent = p, .name = "a" });
    ecs_entity(world, { .parent = p, .name = "b" });
    ecs_entity_t c = ecs_entity(world, { .parent = p, .name = "c" });
    ecs_add_id(world, c, EcsOrderedChildren);
    ecs_entity(world, { .parent = c, .name = "x" });
    ecs_entity(world, { .parent = c, .name = "y" });

    ecs_entity_t i = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(ecs_has_id(world, i, EcsOrderedChildren));

    ecs_entities_t children = ecs_get_ordered_children(world, i);
    test_int(children.count, 3);
    test_str(ecs_get_name(world, children.ids[0]), "a");
    test_str(ecs_get_name(world, children.ids[1]), "b");
    test_str(ecs_get_name(world, children.ids[2]), "c");

    ecs_entities_t grandchildren = ecs_get_ordered_children(world, children.ids[2]);
    test_int(grandchildren.count, 2);
    test_str(ecs_get_name(world, grandchildren.ids[0]), "x");
    test_str(ecs_get_name(world, grandchildren.ids[1]), "y");

    ecs_fini(world);
}
