#include <api.h>

void Lookup_setup() {
    ecs_tracing_enable(-3);
}

void Lookup_lookup() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, MyEntity, 0);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_w_null_name() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, MyEntity, 0);

    /* Ensure this doesn't crash the lookup function */
    ecs_set_name(world, 0, NULL);

    ecs_entity_t lookup = ecs_lookup(world, "MyEntity");
    test_assert(lookup != 0);
    test_assert(lookup == MyEntity);

    ecs_fini(world);
}

void Lookup_lookup_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t lookup = ecs_lookup(world, "Position");
    test_assert(lookup != 0);
    test_assert(lookup == ecs_id(Position));

    ecs_fini(world);
}

void Lookup_lookup_not_found() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t lookup = ecs_lookup(world, "foo");
    test_assert(lookup == 0);

    ecs_fini(world);
}

void Lookup_lookup_child() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent1, 0);
    ECS_ENTITY(world, Parent2, 0);

    ecs_entity_t e1 = ecs_set_name(world, 0, "Child");
    ecs_entity_t e2 = ecs_set_name(world, 0, "Child");

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
    ecs_world_t *world = ecs_init();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_set_name(world, 0, "Entity");
    const char *id = ecs_get_name(world, e);
    test_assert(id != NULL);
    test_str(id, "Entity");

    ecs_fini(world);
}

void Lookup_get_name_no_name() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world, Position);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_get_name_from_empty() {
    ecs_world_t *world = ecs_init();

    /* Ensure this doesn't crash the lookup function */
    ecs_entity_t e = ecs_new(world, 0);
    const char *id = ecs_get_name(world, e);
    test_assert(id == NULL);

    ecs_fini(world);
}

void Lookup_lookup_by_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup(world, "1000");
    test_int(e, 1000);

    ecs_fini(world);
}

void Lookup_lookup_name_w_digit() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "10_id");
    ecs_entity_t e2 = ecs_lookup(world, "10_id");
    test_assert(e == e2);

    ecs_fini(world);
}

void Lookup_lookup_symbol_by_id() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_lookup_symbol(world, "1000", true);
    test_int(e, 1000);

    ecs_fini(world);
}

void Lookup_lookup_symbol_w_digit() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_symbol(world, 0, "10_id");    
    ecs_entity_t e2 = ecs_lookup_symbol(world, "10_id", true);
    test_assert(e == e2);

    ecs_fini(world);
}

void Lookup_lookup_path_w_digit() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_set_name(world, 0, "parent");
    ecs_entity_t e1 = ecs_set_name(world, 0, "10_id");
    ecs_add_pair(world, e1, EcsChildOf, parent);

    ecs_entity_t e2 = ecs_lookup_fullpath(world, "parent.10_id");
    test_assert(e2 == e1);

    ecs_fini(world);
}

void Lookup_set_name_of_existing() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_get_name(world, e) == NULL);

    ecs_set_name(world, e, "Foo");
    test_assert(ecs_get_name(world, e) != NULL);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Lookup_change_name_of_existing() {
    ecs_world_t *world = ecs_init();

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
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_set_name(world, 0, "MyEntity");
    test_assert(e != 0);

    ecs_use(world, e, "MyAlias");

    ecs_entity_t a = ecs_lookup(world, "MyAlias");
    test_assert(a != 0);
    test_assert(a == e);
    
    ecs_fini(world);
}

void Lookup_lookup_scoped_alias() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = ecs_set_name(world, 0, "MyParent");
    test_assert(p != 0);
    ecs_entity_t e = ecs_set_name(world, 0, "MyEntity");
    test_assert(e != 0);
    ecs_add_pair(world, e, EcsChildOf, p);

    ecs_use(world, e, "MyAlias");

    ecs_entity_t a = ecs_lookup(world, "MyAlias");
    test_assert(a != 0);
    test_assert(a == e);
    
    ecs_fini(world);
}

void Lookup_define_duplicate_alias() {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_entity_t e1 = ecs_set_name(world, 0, "MyEntityA");
    test_assert(e1 != 0);
    ecs_entity_t e2 = ecs_set_name(world, 0, "MyEntityB");
    test_assert(e2 != 0);
    
    test_expect_abort(); /* Not allowed to create duplicate aliases */

    ecs_use(world, e1, "MyAlias");
    ecs_use(world, e2, "MyAlias");

}

void Lookup_lookup_null() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_lookup(world, NULL) == 0);

    ecs_fini(world);
}

void Lookup_lookup_symbol_null() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_lookup_symbol(world, NULL, true) == 0);

    ecs_fini(world);
}

void Lookup_lookup_this() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t lookup = ecs_lookup(world, ".");
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_wildcard() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t lookup = ecs_lookup(world, "*");
    test_assert(lookup != 0);
    test_assert(lookup == EcsWildcard);

    ecs_fini(world);
}

void Lookup_lookup_path_this() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, 0, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_path_wildcard() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, 0, "*", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsWildcard);

    ecs_fini(world);
}

void Lookup_lookup_path_this_from_scope() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t scope = ecs_new_id(world);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, scope, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}

void Lookup_lookup_path_wildcard_from_scope() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t scope = ecs_new_id(world);

    ecs_entity_t lookup = ecs_lookup_path_w_sep(world, scope, ".", NULL, NULL, false);
    test_assert(lookup != 0);
    test_assert(lookup == EcsThis);

    ecs_fini(world);
}
