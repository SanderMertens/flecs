#include <api.h>
#include <stdio.h>

void Lookup_setup() {
    ecs_log_set_level(-3);
}

void Lookup_lookup() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyEntity, 0);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_w_null_name() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, MyEntity, 0);

    /* Ensure this doesn't crash the lookup function */
    ecs_set_name(world, 0, NULL);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_after_name_reset() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_entity(world, "foo");
    ecs_entity_t lookup = ecs_lookup(world, "foo");
    test_assert(e == lookup);

    ecs_set_name(world, e, NULL);
    test_assert(ecs_lookup(world, "foo") == 0);

    ecs_set_name(world, e, "foo");
    lookup = ecs_lookup(world, "foo");
    test_assert(e == lookup);

    ecs_fini(world);
}

void Lookup_lookup_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t lookup = ecs_lookup(world, "Position");
    test_assert(lookup != 0);
    test_assert(lookup == ecs_id(Position));

    ecs_fini(world);
}

void Lookup_lookup_not_found() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "foo");
    test_assert(lookup == 0);

    ecs_fini(world);
}

void Lookup_lookup_child() {
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

void Lookup_get_name() {
    ecs_world_t *world = ecs_mini();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_set_name(world, 0, "Entity");
    const char *id = ecs_get_name(world, e);
    test_assert(id != NULL);
    test_str(id, "Entity");

    ecs_fini(world);
}

void Lookup_get_name_no_name() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world, Position);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_get_name_from_empty() {
    ecs_world_t *world = ecs_mini();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world, 0);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_lookup_by_id() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_lookup(world, "1000");
    test_int(e, 1000);

    ecs_fini(world);
}

void Lookup_lookup_recycled_by_id() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    test_assert(e != 0);

    char buf[20];
    sprintf(buf, "%u", (uint32_t)e);
    
    ecs_entity_t l = ecs_lookup(world, buf);
    test_assert(l != 0);
    test_assert(l == e);

    ecs_delete(world, e);
    ecs_entity_t r = ecs_new_id(world);
    test_assert((uint32_t)r == e);
    test_assert(r != e);

    l = ecs_lookup(world, buf);
    test_assert(l == r);

    ecs_fini(world);
}

void Lookup_lookup_name_w_digit() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "10_id");
    ecs_entity_t e2 = ecs_lookup(world, "10_id");
    test_assert(e == e2);

    ecs_fini(world);
}

void Lookup_lookup_symbol_by_id() {
    ecs_world_t *world = ecs_mini();

    ecs_ensure(world, 1000);
    ecs_entity_t e = ecs_lookup_symbol(world, "1000", true);
    test_int(e, 1000);

    ecs_fini(world);
}

void Lookup_lookup_symbol_w_digit() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_symbol(world, 0, "10_id");    
    ecs_entity_t e2 = ecs_lookup_symbol(world, "10_id", true);
    test_assert(e == e2);

    ecs_fini(world);
}

void Lookup_lookup_path_w_digit() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "parent");
    ecs_entity_t e1 = ecs_set_name(world, 0, "10_id");
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_lookup_fullpath(world, "parent.10_id");
    test_assert(e2 == e1);

    ecs_fini(world);
}

void Lookup_set_name_of_existing() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_get_name(world, e) == NULL);

    ecs_set_name(world, e, "Foo");
    test_assert(ecs_get_name(world, e) != NULL);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Lookup_change_name_of_existing() {
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

void Lookup_lookup_alias() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_set_name(world, 0, "MyEntity");
    test_assert(e != 0);

    ecs_set_alias(world, e, "MyAlias");

    ecs_entity_t a = ecs_lookup(world, "MyAlias");
    test_assert(a != 0);
    test_assert(a == e);
    
    ecs_fini(world);
}

void Lookup_lookup_scoped_alias() {
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

void Lookup_define_duplicate_alias() {
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

void Lookup_lookup_null() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_lookup(world, NULL) == 0);

    ecs_fini(world);
}

void Lookup_lookup_symbol_null() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_lookup_symbol(world, NULL, true) == 0);

    ecs_fini(world);
}

void Lookup_lookup_this() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, ".");
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_wildcard() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "*");
    test_assert(lookup != 0);
    test_assert(lookup == EcsWildcard);

    ecs_fini(world);
}

void Lookup_lookup_any() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup(world, "_");
    test_assert(lookup != 0);
    test_assert(lookup == EcsAny);

    ecs_fini(world);
}

void Lookup_lookup_path_this() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, 0, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_path_wildcard() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, 0, "*", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsWildcard);

    ecs_fini(world);
}

void Lookup_lookup_path_this_from_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_new_id(world);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, scope, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_path_wildcard_from_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t scope = ecs_new_id(world);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, scope, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_resolve_builtin_symbols() {
    ecs_world_t *world = ecs_mini();

    test_assert(ecs_lookup_symbol(world, "EcsComponent", false) == ecs_id(EcsComponent));
    test_assert(ecs_lookup_symbol(world, "EcsIdentifier", false) == ecs_id(EcsIdentifier));

    test_assert(ecs_lookup_symbol(world, "EcsName", false) == EcsName);
    test_assert(ecs_lookup_symbol(world, "EcsSymbol", false) == EcsSymbol);

    ecs_fini(world);
}

void Lookup_lookup_from_scope_staged() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_set_name(world, 0, "Child");
    ecs_add_pair(world, child, EcsChildOf, parent);

    ecs_readonly_begin(world);
    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(ecs_set_scope(stage, parent) == 0);

    test_assert(ecs_lookup_path(stage, 0, "Child") == child);
    test_assert(ecs_lookup_fullpath(stage, "Child") == child);

    test_assert(ecs_lookup_path(stage, parent, "Child") == child);
    test_assert(ecs_lookup_fullpath(stage, "Child") == child);

    test_assert(ecs_set_scope(stage, 0) == parent);
    ecs_readonly_end(world);

    ecs_fini(world);
}

void Lookup_lookup_core() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_lookup_fullpath(world, "Component");
    test_assert(c != 0);
    test_assert(c == ecs_id(EcsComponent));

    ecs_fini(world);
}

void Lookup_lookup_core_from_stage() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t c = ecs_lookup_fullpath(world, "Component");
    test_assert(c != 0);
    test_assert(c == ecs_id(EcsComponent));

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world);
    ecs_entity_t d = ecs_lookup_fullpath(stage, "Component");
    test_assert(d != 0);
    test_assert(d == ecs_id(EcsComponent));
    ecs_readonly_end(world);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Component");
    test_assert(e != 0);
    test_assert(e == ecs_id(EcsComponent));

    ecs_fini(world);
}

void Lookup_lookup_custom_search_path() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, child, "Child");
    
    test_assert(ecs_lookup_fullpath(world, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(world, "Child") == 0);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child") == child);

    ecs_entity_t lookup_path[] = { parent, 0 };
    ecs_entity_t *old_path = ecs_set_lookup_path(world, lookup_path);

    test_assert(ecs_lookup_fullpath(world, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(world, "Child") == child);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child") == child);

    ecs_set_lookup_path(world, old_path);

    ecs_fini(world);
}

void Lookup_lookup_custom_search_path_from_stage() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, child, "Child");
    
    test_assert(ecs_lookup_fullpath(world, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(world, "Child") == 0);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child") == child);

    ecs_entity_t lookup_path[] = { parent, 0 };
    ecs_entity_t *old_path = ecs_set_lookup_path(world, lookup_path);

    ecs_world_t *stage = ecs_get_stage(world, 0);
    ecs_readonly_begin(world);
    test_assert(ecs_lookup_fullpath(stage, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(stage, "Child") == child);
    test_assert(ecs_lookup_fullpath(stage, "Parent.Child") == child);
    ecs_readonly_end(world);

    ecs_set_lookup_path(world, old_path);

    ecs_fini(world);
}

void Lookup_lookup_custom_search_path_n_elems() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_set_name(world, 0, "Parent");
    ecs_entity_t child = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_set_name(world, child, "Child");
    ecs_entity_t grand_child = ecs_new_w_pair(world, EcsChildOf, child);
    ecs_set_name(world, grand_child, "Child");
    
    test_assert(ecs_lookup_fullpath(world, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(world, "Child") == 0);
    test_assert(ecs_lookup_fullpath(world, "Child.Child") == 0);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child") == child);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child.Child") == grand_child);

    ecs_entity_t lookup_path[] = { child, parent, 0 };
    ecs_entity_t *old_path = ecs_set_lookup_path(world, lookup_path);

    test_assert(ecs_lookup_fullpath(world, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(world, "Child") == child);
    test_assert(ecs_lookup_fullpath(world, "Child.Child") == grand_child);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child") == child);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child.Child") == grand_child);

    lookup_path[0] = parent;
    lookup_path[1] = child;

    test_assert(ecs_lookup_fullpath(world, "Parent") == parent);
    test_assert(ecs_lookup_fullpath(world, "Child") == grand_child);
    test_assert(ecs_lookup_fullpath(world, "Child.Child") == grand_child);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child") == child);
    test_assert(ecs_lookup_fullpath(world, "Parent.Child.Child") == grand_child);

    ecs_set_lookup_path(world, old_path);

    ecs_fini(world);
}

void Lookup_set_same_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_entity(world, "MyName");
    test_assert(e != 0);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));

    ecs_set_name(world, e, "MyName");
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));
    
    ecs_fini(world);
}

void Lookup_set_same_name_after_reparenting() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "parent");
    ecs_entity_t e = ecs_new_entity(world, "MyName");
    test_assert(e != 0);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup(world, "MyName"));

    ecs_add_pair(world, e, EcsChildOf, parent);
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup_fullpath(world, "parent.MyName"));

    ecs_set_name(world, e, "MyName");
    test_str("MyName", ecs_get_name(world, e));
    test_uint(e, ecs_lookup_fullpath(world, "parent.MyName"));
    
    ecs_fini(world);
}

void Lookup_defer_set_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_entity(world, "Foo");
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

void Lookup_defer_set_same_name() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_entity(world, "MyName");
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

void Lookup_lookup_invalid_digit() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t child = ecs_lookup(world, "111111111111");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_child_invalid_digit() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "p");
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup_child(world, parent, "111111111111");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_digit_from_wrong_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "p");
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup_fullpath(world, "p.1");
    test_assert(child == 0);

    ecs_fini(world);
}

void Lookup_lookup_core_entity_from_wrong_scope() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t parent = ecs_new_entity(world, "p");
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup_fullpath(world, "p.10.Component");
    test_assert(child == 0);

    ecs_fini(world);
}
