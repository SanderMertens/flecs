#include <core.h>
#include <stdio.h>

void Lookup_setup(void) {
    ecs_log_set_level(-3);
}

void Lookup_lookup(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyEntity, 0);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_w_null_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyEntity, 0);

    /* Ensure this doesn't crash the lookup function */
    ecs_set_name(world, 0, NULL);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_after_name_reset(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "foo" });
    ecs_entity_t lookup = ecs_lookup(world, "foo");
    test_assert(e == lookup);

    ecs_set_name(world, e, NULL);
    test_assert(ecs_lookup(world, "foo") == 0);

    ecs_set_name(world, e, "foo");
    lookup = ecs_lookup(world, "foo");
    test_assert(e == lookup);

    ecs_fini(world);
}

void Lookup_lookup_component(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t lookup = ecs_lookup(world, "Position");
    test_assert(lookup != 0);
    test_assert(lookup == ecs_id(Position));

    ecs_fini(world);
}

void Lookup_lookup_not_found(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "foo");
    test_assert(lookup == 0);

    ecs_fini(world);
}

void Lookup_lookup_child(void) {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Parent1, 0);
    ECS_ENTITY(world, Parent2, 0);

    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, Parent1);
    ecs_set_name(world, e1, "Child");
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, Parent2);
    ecs_set_name(world, e2, "Child");

    ecs_add_pair(world, e1, EcsChildOf, Parent1);
    ecs_add_pair(world, e2, EcsChildOf, Parent2);

    ecs_entity_t lookup = ecs_lookup_child(world, Parent1, "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e1);

    lookup = ecs_lookup_child(world, Parent2, "Child");
    test_assert(lookup != 0);
    test_assert(lookup == e2);

    ecs_fini(world);
}

void Lookup_get_name(void) {
    ecs_world_t *world = ecs_mini();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_set_name(world, 0, "Entity");
    const char *id = ecs_get_name(world, e);
    test_assert(id != NULL);
    test_str(id, "Entity");

    ecs_fini(world);
}

void Lookup_get_name_no_name(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new_w(world, Position);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_get_name_from_empty(void) {
    ecs_world_t *world = ecs_mini();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_lookup_by_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup(world, "#1000");
    test_int(e, 0);

    ecs_make_alive(world, 1000);

    e = ecs_lookup(world, "#1000");
    test_int(e, 1000);

    ecs_fini(world);
}

void Lookup_lookup_path_anonymous_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 1000);

    ecs_entity_t e = ecs_entity(world, {
        .name = "foo",
        .parent = 1000
    });
    test_assert(e != 0);

    test_assert(ecs_has_pair(world, e, EcsChildOf, 1000));
    test_assert(ecs_lookup(world, "#1000") == 1000);
    test_assert(ecs_lookup(world, "#1000.foo") == e);

    ecs_fini(world);
}

void Lookup_lookup_path_0_parent(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "foo",
    });
    test_assert(e != 0);

    test_assert(ecs_lookup(world, "#0.foo") == e);

    ecs_fini(world);
}

void Lookup_lookup_path_0_parent_w_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, {
        .name = "parent",
    });
    test_assert(p != 0);

    ecs_entity_t e = ecs_entity(world, {
        .name = "foo",
    });
    test_assert(e != 0);

    ecs_entity_t c = ecs_entity(world, {
        .name = "foo",
        .parent = p
    });
    test_assert(c != 0);
    test_assert(e != c);

    test_assert(ecs_lookup(world, "foo") == e);
    test_assert(ecs_lookup(world, "#0.foo") == e);
    test_assert(ecs_lookup(world, "parent.foo") == c);

    ecs_set_scope(world, p);

    test_assert(ecs_lookup(world, "foo") == c);
    test_assert(ecs_lookup(world, "#0.foo") == e);
    test_assert(ecs_lookup(world, "parent.foo") == c);

    ecs_set_scope(world, 0);

    ecs_fini(world);
}

void Lookup_lookup_recycled_by_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);

    char buf[20];
    sprintf(buf, "#%u", (uint32_t)e);
    
    ecs_entity_t l = ecs_lookup(world, buf);
    test_assert(l != 0);
    test_assert(l == e);

    ecs_delete(world, e);
    ecs_entity_t r = ecs_new(world);
    test_assert((uint32_t)r == e);
    test_assert(r != e);

    l = ecs_lookup(world, buf);
    test_assert(l == r);

    ecs_fini(world);
}

void Lookup_lookup_name_w_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "10_id");
    ecs_entity_t e2 = ecs_lookup(world, "10_id");
    test_assert(e == e2);

    ecs_fini(world);
}

void Lookup_lookup_symbol_by_id(void) {
    ecs_world_t *world = ecs_mini();

    ecs_make_alive(world, 1000);
    ecs_entity_t e = ecs_lookup_symbol(world, "#1000", true, true);
    test_int(e, 1000);

    ecs_fini(world);
}

void Lookup_lookup_symbol_w_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_symbol(world, 0, "10_id");    
    ecs_entity_t e2 = ecs_lookup_symbol(world, "10_id", true, true);
    test_assert(e == e2);

    ecs_fini(world);
}

void Lookup_lookup_path_w_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "parent");
    ecs_entity_t e1 = ecs_set_name(world, 0, "10_id");
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_lookup(world, "parent.10_id");
    test_assert(e2 == e1);

    ecs_fini(world);
}

void Lookup_lookup_name_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_set_name(world, 0, "hello world");
    ecs_entity_t e2 = ecs_set_name(world, 0, "hello mars");
    test_assert(e1 != e2);
    test_str(ecs_get_name(world, e1), "hello world");
    test_str(ecs_get_name(world, e2), "hello mars");
    
    ecs_fini(world);
}

void Lookup_lookup_path_w_spaces(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_set_name(world, 0, "hello world");
    ecs_entity_t e2 = ecs_set_name(world, 0, "hello mars");
    ecs_add_pair(world, e2, EcsChildOf, e1);
    
    test_str(ecs_get_name(world, e1), "hello world");
    test_str(ecs_get_name(world, e2), "hello mars");

    test_assert(e2 == ecs_lookup(world, "hello world.hello mars"));
    
    ecs_fini(world);
}

void Lookup_lookup_number(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "10");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "10");

    test_uint(ecs_lookup(world, "10"), e);

    ecs_fini(world);
}

void Lookup_lookup_number_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_set_name(world, 0, "10");
    ecs_entity_t e = ecs_set_name(world, 0, "20");
    ecs_add_pair(world, e, EcsChildOf, p);
    
    test_str(ecs_get_name(world, p), "10");
    test_str(ecs_get_name(world, e), "20");

    test_uint(ecs_lookup(world, "20"), 0);
    test_uint(ecs_lookup(world, "10"), p);
    test_uint(ecs_lookup(world, "10.20"), e);

    ecs_fini(world);
}

void Lookup_lookup_number_0(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "0");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "0");

    test_uint(ecs_lookup(world, "0"), e);

    ecs_fini(world);
}

void Lookup_set_name_of_existing(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world);
    test_assert(e != 0);
    test_assert(ecs_get_name(world, e) == NULL);

    ecs_set_name(world, e, "Foo");
    test_assert(ecs_get_name(world, e) != NULL);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Lookup_change_name_of_existing(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "Foo");
    test_assert(e != 0);
    test_assert(ecs_get_name(world, e) != NULL);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_set_name(world, e, "Bar");
    test_assert(ecs_get_name(world, e) != NULL);
    test_str(ecs_get_name(world, e), "Bar");

    ecs_fini(world);
}

void Lookup_lookup_alias(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "MyEntity");
    test_assert(e != 0);

    ecs_set_alias(world, e, "MyAlias");

    ecs_entity_t a = ecs_lookup(world, "MyAlias");
    test_assert(a != 0);
    test_assert(a == e);
    
    ecs_fini(world);
}

void Lookup_lookup_scoped_alias(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_set_name(world, 0, "MyParent");
    test_assert(p != 0);
    ecs_entity_t e = ecs_set_name(world, 0, "MyEntity");
    test_assert(e != 0);
    ecs_add_pair(world, e, EcsChildOf, p);

    ecs_set_alias(world, e, "MyAlias");

    ecs_entity_t a = ecs_lookup(world, "MyAlias");
    test_assert(a != 0);
    test_assert(a == e);
    
    ecs_fini(world);
}

void Lookup_define_duplicate_alias(void) {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ecs_entity_t e1 = ecs_set_name(world, 0, "MyEntityA");
    test_assert(e1 != 0);
    ecs_entity_t e2 = ecs_set_name(world, 0, "MyEntityB");
    test_assert(e2 != 0);
    
    test_expect_abort(); /* Not allowed to create duplicate aliases */

    ecs_set_alias(world, e1, "MyAlias");
    ecs_set_alias(world, e2, "MyAlias");

}

void Lookup_lookup_null(void) {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_lookup(world, NULL) == 0);

    ecs_fini(world);
}

void Lookup_lookup_symbol_null(void) {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_lookup_symbol(world, NULL, true, true) == 0);

    ecs_fini(world);
}

void Lookup_lookup_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, ".");
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "*");
    test_assert(lookup != 0);
    test_assert(lookup == EcsWildcard);

    ecs_fini(world);
}

void Lookup_lookup_any(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "_");
    test_assert(lookup != 0);
    test_assert(lookup == EcsAny);

    ecs_fini(world);
}

void Lookup_lookup_variable(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "$");
    test_assert(lookup != 0);
    test_assert(lookup == EcsVariable);

    ecs_fini(world);
}

void Lookup_lookup_path_this(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, 0, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_path_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, 0, "*", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsWildcard);

    ecs_fini(world);
}

void Lookup_lookup_path_this_from_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_new(world);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, scope, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_path_wildcard_from_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_new(world);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, scope, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_resolve_builtin_symbols(void) {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_lookup_symbol(world, "EcsComponent", false, true) == ecs_id(EcsComponent));
    test_assert(ecs_lookup_symbol(world, "EcsIdentifier", false, true) == ecs_id(EcsIdentifier));

    test_assert(ecs_lookup_symbol(world, "EcsName", false, true) == EcsName);
    test_assert(ecs_lookup_symbol(world, "EcsSymbol", false, true) == EcsSymbol);

    ecs_fini(world);
}

void Lookup_lookup_from_scope_staged(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_set_name(world, 0, "Child");
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_readonly_begin(world, false);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(ecs_set_scope(stage, parent) == 0);

    test_assert(ecs_lookup_from(stage, 0, "Child") == child);
    test_assert(ecs_lookup(stage, "Child") == child);

    test_assert(ecs_lookup_from(stage, parent, "Child") == child);
    test_assert(ecs_lookup(stage, "Child") == child);

    test_assert(ecs_set_scope(stage, 0) == parent);
    ecs_readonly_end(world);

    ecs_fini(world);
}

void Lookup_lookup_core(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_lookup(world, "Component");
    test_assert(c != 0);
    test_assert(c == ecs_id(EcsComponent));

    ecs_fini(world);
}

void Lookup_lookup_core_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_lookup(world, "Component");
    test_assert(c != 0);
    test_assert(c == ecs_id(EcsComponent));

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);
    ecs_entity_t d = ecs_lookup(stage, "Component");
    test_assert(d != 0);
    test_assert(d == ecs_id(EcsComponent));
    ecs_readonly_end(world);

    ecs_entity_t e = ecs_lookup(world, "Component");
    test_assert(e != 0);
    test_assert(e == ecs_id(EcsComponent));

    ecs_fini(world);
}

void Lookup_lookup_custom_search_path(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, child, "Child");
    
    test_assert(ecs_lookup(world, "Parent") == parent);
    test_assert(ecs_lookup(world, "Child") == 0);
    test_assert(ecs_lookup(world, "Parent.Child") == child);

    ecs_entity_t lookup_path[] = { parent, 0 };
    ecs_entity_t *old_path = ecs_set_lookup_path(world, lookup_path);

    test_assert(ecs_lookup(world, "Parent") == parent);
    test_assert(ecs_lookup(world, "Child") == child);
    test_assert(ecs_lookup(world, "Parent.Child") == child);

    ecs_set_lookup_path(world, old_path);

    ecs_fini(world);
}

void Lookup_lookup_custom_search_path_from_stage(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, child, "Child");
    
    test_assert(ecs_lookup(world, "Parent") == parent);
    test_assert(ecs_lookup(world, "Child") == 0);
    test_assert(ecs_lookup(world, "Parent.Child") == child);

    ecs_entity_t lookup_path[] = { parent, 0 };
    ecs_entity_t *old_path = ecs_set_lookup_path(world, lookup_path);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world, false);
    test_assert(ecs_lookup(stage, "Parent") == parent);
    test_assert(ecs_lookup(stage, "Child") == child);
    test_assert(ecs_lookup(stage, "Parent.Child") == child);
    ecs_readonly_end(world);

    ecs_set_lookup_path(world, old_path);

    ecs_fini(world);
}

void Lookup_lookup_custom_search_path_n_elems(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, child, "Child");
    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_set_name(world, grand_child, "Child");
    
    test_assert(ecs_lookup(world, "Parent") == parent);
    test_assert(ecs_lookup(world, "Child") == 0);
    test_assert(ecs_lookup(world, "Child.Child") == 0);
    test_assert(ecs_lookup(world, "Parent.Child") == child);
    test_assert(ecs_lookup(world, "Parent.Child.Child") == grand_child);

    ecs_entity_t lookup_path[] = { child, parent, 0 };
    ecs_entity_t *old_path = ecs_set_lookup_path(world, lookup_path);

    test_assert(ecs_lookup(world, "Parent") == parent);
    test_assert(ecs_lookup(world, "Child") == child);
    test_assert(ecs_lookup(world, "Child.Child") == grand_child);
    test_assert(ecs_lookup(world, "Parent.Child") == child);
    test_assert(ecs_lookup(world, "Parent.Child.Child") == grand_child);

    lookup_path[0] = parent;
    lookup_path[1] = child;

    test_assert(ecs_lookup(world, "Parent") == parent);
    test_assert(ecs_lookup(world, "Child") == grand_child);
    test_assert(ecs_lookup(world, "Child.Child") == grand_child);
    test_assert(ecs_lookup(world, "Parent.Child") == child);
    test_assert(ecs_lookup(world, "Parent.Child.Child") == grand_child);

    ecs_set_lookup_path(world, old_path);

    ecs_fini(world);
}

void Lookup_set_same_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "MyName" });
    test_assert(e != 0);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));

    ecs_set_name(world, e, "MyName");
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));
    
    ecs_fini(world);
}

void Lookup_set_same_name_after_reparenting(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    ecs_entity_t e = ecs_entity(world, { .name = "MyName" });
    test_assert(e != 0);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));

    ecs_add_pair(world, e, EcsChildOf, parent);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "parent.MyName"));

    ecs_set_name(world, e, "MyName");
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "parent.MyName"));
    
    ecs_fini(world);
}

void Lookup_defer_set_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "Foo" });
    test_assert(e != 0);
    test_str("Foo", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "Foo"));

    ecs_defer_begin(world);
    ecs_set_name(world, e, "Bar");
    ecs_defer_end(world);

    test_str("Bar", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "Bar"));
    
    ecs_fini(world);
}

void Lookup_defer_set_same_name(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { .name = "MyName" });
    test_assert(e != 0);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));

    ecs_defer_begin(world);
    ecs_set_name(world, e, "MyName");
    ecs_defer_end(world);
    
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));
    
    ecs_fini(world);
}

void Lookup_lookup_invalid_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t child = ecs_lookup(world, "111111111111");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_child_invalid_digit(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "p" });
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup_child(world, parent, "111111111111");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_digit_from_wrong_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "p" });
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup(world, "p.1");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_core_entity_from_wrong_scope(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "p" });
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup(world, "p.10.Component");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_alias_w_number(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_entity(world, { .name = "p" });
    test_assert(parent != 0);
    ecs_set_alias(world, parent, "10p");

    test_assert(parent == ecs_lookup(world, "p"));
    test_assert(parent == ecs_lookup(world, "10p"));

    ecs_fini(world);
}

void Lookup_lookup_symbol_path(void) {
    ecs_world_t *world = ecs_mini();
    
    ecs_entity_t foo = ecs_entity(world, { .name = "foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "bar" });
    ecs_add_pair(world, bar, EcsChildOf, foo);

    ecs_entity_t e = ecs_set_symbol(world, 0, "foo.bar");

    test_assert(ecs_lookup_symbol(world, "foo.bar", false, false) == e);
    test_assert(ecs_lookup_symbol(world, "foo.bar", true, false) == bar);

    ecs_delete(world, bar);

    test_assert(ecs_lookup_symbol(world, "foo.bar", false, false) == e);
    test_assert(ecs_lookup_symbol(world, "foo.bar", true, false) == e);

    ecs_fini(world);
}

void Lookup_lookup_name_escaped_sep(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, {
        .name = "foo.bar",
        .sep = ""
    });
    test_assert(e != 0);

    test_assert(ecs_lookup(world, "foo.bar") == 0);
    test_assert(ecs_lookup(world, "foo\\.bar") == e);

    ecs_fini(world);
}

void Lookup_lookup_path_escaped_sep(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { .name = "parent" });

    ecs_entity_t e = ecs_entity(world, {
        .name = "foo.bar",
        .parent = p,
        .sep = ""
    });
    test_assert(e != 0);

    test_assert(ecs_lookup(world, "parent.foo.bar") == 0);
    test_assert(ecs_lookup(world, "parent.foo\\.bar") == e);

    ecs_fini(world);
}

void Lookup_lookup_name_63_chars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { 
        .name = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij" });

    test_assert(e != 0);

    test_assert(ecs_lookup(world, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij") == e);

    ecs_fini(world);
}

void Lookup_lookup_name_64_chars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { 
        .name = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk" });

    test_assert(e != 0);

    test_assert(ecs_lookup(world, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk") == e);

    ecs_fini(world);
}

void Lookup_lookup_name_65_chars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_entity(world, { 
        .name = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl" });

    test_assert(e != 0);

    test_assert(ecs_lookup(world, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl") == e);

    ecs_fini(world);
}

void Lookup_lookup_path_63_chars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { 
        .name = "Abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij" });

    ecs_entity_t e = ecs_entity(world, { 
        .name = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij" });
    ecs_add_pair(world, e, EcsChildOf, p);

    test_assert(e != 0);

    test_assert(ecs_lookup(world, "Abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij."
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghij") == e);

    ecs_fini(world);
}

void Lookup_lookup_path_64_chars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { 
        .name = "Abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk" });

    ecs_entity_t e = ecs_entity(world, { 
        .name = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk" });
    ecs_add_pair(world, e, EcsChildOf, p);

    test_assert(e != 0);

    test_assert(ecs_lookup(world, "Abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk."
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijk") == e);

    ecs_fini(world);
}

void Lookup_lookup_path_65_chars(void) {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t p = ecs_entity(world, { 
        .name = "Abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl" });

    ecs_entity_t e = ecs_entity(world, { 
        .name = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl" });
    ecs_add_pair(world, e, EcsChildOf, p);

    test_assert(e != 0);

    test_assert(ecs_lookup(world, "Abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl."
        "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkl") == e);

    ecs_fini(world);
}
