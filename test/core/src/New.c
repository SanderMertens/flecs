#include <core.h>

void New_setup(void) {
    ecs_log_set_level(-2);
}

void New_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) != NULL);
    test_int(ecs_get_type(world, e)->count, 0);

    ecs_fini(world);
}

void New_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    ecs_fini(world);
}

void New_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_new_w_id(world, Tag);
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Tag));
    
    ecs_fini(world);
}

void New_redefine_component(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = 0;

    {
        ECS_COMPONENT(world, Position);
        c = ecs_id(Position);
    }

    {
        ECS_COMPONENT(world, Position);
        test_assert(c == ecs_id(Position));
    }
    
    ecs_fini(world);
}

void New_recycle_id_empty(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new(world);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_id_w_entity(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new(world);

    ecs_entity_t e1 = ecs_new_w_id(world, tag);
    test_assert(e1 != 0);
    ecs_delete(world, e1);

    ecs_entity_t e2 = ecs_new_w_id(world, tag);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_empty_staged_delete(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_new(world);
    test_assert(e1 != 0);

    ecs_defer_begin(world);
    ecs_delete(world, e1);
    ecs_defer_end(world);

    ecs_entity_t e2 = ecs_new(world);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_recycle_staged_delete(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new_w(world, Position);
    test_assert(e1 != 0);

    ecs_defer_begin(world);
    ecs_delete(world, e1);
    ecs_defer_end(world);

    ecs_entity_t e2 = ecs_new(world);
    test_assert(e2 != 0);
    test_assert(e1 != e2);
    test_assert((e1 & ECS_ENTITY_MASK) == (e2 & ECS_ENTITY_MASK));

    ecs_fini(world);
}

void New_new_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) != NULL);
    test_int(ecs_get_type(world, e)->count, 0);

    ecs_fini(world);
}

void New_new_component_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_low_id(world);
    test_assert(e != 0);
    test_assert(e < FLECS_HI_COMPONENT_ID);
    test_assert(ecs_get_type(world, e) != NULL);
    test_int(ecs_get_type(world, e)->count, 0);

    ecs_fini(world);
}

void New_new_component_id_skip_used(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new_low_id(world);
    test_assert(e != 0);
    test_assert(e < FLECS_HI_COMPONENT_ID);
    test_assert(ecs_get_type(world, e) != NULL);
    test_int(ecs_get_type(world, e)->count, 0);

    /* Explicitly set an id that is one above the last issued id */
    ecs_make_alive(world, e + 1);
    ecs_add_id(world, e + 1, Foo);

    ecs_entity_t e2 = ecs_new_low_id(world);
    test_assert(e2 != 0);
    test_assert(e2 < FLECS_HI_COMPONENT_ID);
    test_assert(ecs_get_type(world, e2) != NULL);
    test_int(ecs_get_type(world, e2)->count, 0);  
    test_assert(e2 != (e + 1));

    ecs_fini(world);
}

void New_new_component_id_skip_to_hi_id(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_entity_t e = ecs_new_low_id(world);
    test_assert(e != 0);

    /* Use up all low component ids */
    int i;
    for (i = (int)e; i < FLECS_HI_COMPONENT_ID; i ++) {
        ecs_make_alive(world, i);
        ecs_add_id(world, i, Foo);
    }

    ecs_entity_t e2 = ecs_new_low_id(world);
    test_assert(e2 != 0);
    test_assert(e2 > FLECS_HI_COMPONENT_ID);
    test_assert(ecs_get_type(world, e2) != NULL);
    test_int(ecs_get_type(world, e2)->count, 0);

    ecs_entity_t e3 = ecs_new(world);
    test_assert(e3 != e2);
    test_assert(e3 > e2);
    test_assert(ecs_get_type(world, e3) != NULL);
    test_int(ecs_get_type(world, e3)->count, 0);

    ecs_fini(world);
}

void New_new_hi_component_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    ecs_entity_t c;
    do {
        c = ecs_new_low_id(world);
    } while (c < FLECS_HI_COMPONENT_ID);

    test_assert(c != e);
    test_assert(c > e);

    ecs_fini(world);
}

void New_new_w_entity_0(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    test_expect_abort();
    ecs_new_w_id(world, 0);

    ecs_fini(world);
}

ECS_DECLARE(Foo);

void New_create_w_explicit_id_2_worlds(void) {
    ecs_world_t *world_1 = ecs_mini();
    ecs_world_t *world_2 = ecs_mini();

    ecs_entity_t p1 = ecs_set_name(world_1, 0, "Parent");
    ecs_entity_t p2 = ecs_set_name(world_2, 0, "Parent");

    ecs_set_scope(world_1, p1);
    ecs_set_scope(world_2, p2);

    ECS_ENTITY_DEFINE(world_1, Foo, 0);
    ECS_ENTITY_DEFINE(world_2, Foo, 0);

    char *path = ecs_get_path(world_1, Foo);
    test_str(path, "Parent.Foo");
    ecs_os_free(path);

    path = ecs_get_path(world_2, Foo);
    test_str(path, "Parent.Foo");
    ecs_os_free(path);

    ecs_fini(world_1);
    ecs_fini(world_2);
}

void New_new_w_id_0_w_with(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);

    test_expect_abort();
    ecs_new_w_id(world, 0);
}

void New_new_w_id_w_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_id_w_with_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new(world);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_id_w_with_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    
    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Tag2));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_id_w_with_defer_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new(world);
    ecs_set_scope(world, parent);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new_w_id(world, Tag2);
    test_assert(e != 0);
    test_assert(ecs_get_table(world, e) != NULL);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Tag2));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Tag2));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_type_w_with(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_type_w_with_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new(world);
    ecs_set_scope(world, parent);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_type_w_with_defer(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);
    
    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Position));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));

    test_int(ecs_set_with(world, 0), Tag);

    ecs_fini(world);
}

void New_new_w_type_w_with_defer_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_set_with(world, Tag);

    ecs_entity_t parent = ecs_new(world);
    ecs_set_scope(world, parent);

    ecs_defer_begin(world);

    ecs_entity_t e = ecs_new_w(world, Position);
    test_assert(e != 0);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(!ecs_has(world, e, Position));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    ecs_defer_end(world);

    test_assert(!ecs_has(world, e, Tag));
    test_assert(ecs_has(world, e, Position));
    test_assert(!ecs_has_pair(world, e, EcsChildOf, parent));

    test_int(ecs_set_with(world, 0), Tag);
    test_int(ecs_set_scope(world, 0), parent);

    ecs_fini(world);
}

void New_new_w_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_table_t *table = ecs_table_add_id(world, NULL, Foo);
    table = ecs_table_add_id(world, table, Bar);

    ecs_entity_t e = ecs_new_w_table(world, table);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Foo));
    test_assert(ecs_has(world, e, Bar));
    test_assert(table == ecs_get_table(world, e));

    ecs_fini(world);
}

void New_new_w_null_table(void) {
    install_test_abort();
    ecs_world_t *world = ecs_mini();

    test_expect_abort();
    ecs_new_w_table(world, NULL);
}

void New_new_w_table_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_table_t *table = ecs_table_add_id(world, NULL, ecs_id(Position));
    table = ecs_table_add_id(world, table, Bar);

    ecs_entity_t e = ecs_new_w_table(world, table);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Bar));
    test_assert(ecs_get(world, e, Position) != NULL);
    test_assert(table == ecs_get_table(world, e));

    ecs_fini(world);
}

void New_new_w_table_sparse_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_add_id(world, ecs_id(Position), EcsSparse);

    ecs_table_t *table = ecs_table_add_id(world, NULL, ecs_id(Position));
    table = ecs_table_add_id(world, table, Bar);

    ecs_entity_t e = ecs_new_w_table(world, table);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Bar));
    test_assert(ecs_get(world, e, Position) != NULL);
    test_assert(table == ecs_get_table(world, e));

    ecs_fini(world);
}

void New_new_w_table_override(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_entity_t base = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_table_t *table = ecs_table_add_id(world, NULL, ecs_pair(EcsIsA, base));

    ecs_entity_t inst = ecs_new_w_table(world, table);
    test_assert(ecs_has(world, inst, Position));
    test_assert(ecs_has_pair(world, inst, EcsIsA, base));

    const Position *p = ecs_get(world, inst, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}
