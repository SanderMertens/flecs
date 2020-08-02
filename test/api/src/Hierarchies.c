#include <api.h>

void Hierarchies_empty_scope() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_get_scope(world) == 0);

    ecs_fini(world);
}

void Hierarchies_get_parent() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Child, 0);

    ecs_entity_t e = ecs_get_parent_w_entity(world, Child, 0);
    test_assert(e == 0);

    ecs_fini(world);
}

void Hierarchies_get_parent_from_nested() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, Child, CHILDOF | Scope);

    ecs_entity_t e = ecs_get_parent_w_entity(world, Child, 0);
    test_assert(e == Scope);

    ecs_fini(world);
}

void Hierarchies_get_parent_from_nested_2() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, ChildScope, CHILDOF | Scope);
    ECS_ENTITY(world, Child, CHILDOF | Scope.ChildScope);

    ecs_entity_t e = ecs_get_parent_w_entity(world, Child, 0);
    test_assert(e == ChildScope);

    ecs_fini(world);
}

void Hierarchies_get_parent_from_root() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, Child, CHILDOF | Scope);

    ecs_entity_t e = ecs_get_parent_w_entity(world, 0, 0);
    test_assert(e == 0);

    ecs_fini(world);
}

void Hierarchies_delete_children() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);

    ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | e);
    test_assert(child != 0);

    ecs_delete(world, e);

    test_assert(ecs_get_type(world, child) == NULL);

    ecs_fini(world);
}

void Hierarchies_tree_iter_empty() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    ecs_iter_t it = ecs_scope_iter(world, Parent);
    test_assert( !ecs_scope_next(&it));

    ecs_fini(world);
}

void Hierarchies_tree_iter_1_table() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    ECS_ENTITY(world, Child1, CHILDOF | Parent);
    ECS_ENTITY(world, Child2, CHILDOF | Parent);
    ECS_ENTITY(world, Child3, CHILDOF | Parent);

    ecs_iter_t it = ecs_scope_iter(world, Parent);
    test_assert( ecs_scope_next(&it) == true);
    test_int( it.count, 3);

    test_assert(it.entities[0] == Child1);
    test_assert(it.entities[1] == Child2);
    test_assert(it.entities[2] == Child3);

    test_assert( !ecs_scope_next(&it));

    ecs_fini(world);
}

void Hierarchies_tree_iter_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, 0);

    ECS_ENTITY(world, Child1, CHILDOF | Parent);
    ECS_ENTITY(world, Child2, CHILDOF | Parent);
    ECS_ENTITY(world, Child3, CHILDOF | Parent, Position);
    ECS_ENTITY(world, Child4, CHILDOF | Parent, Position);

    ecs_iter_t it = ecs_scope_iter(world, Parent);
    test_assert( ecs_scope_next(&it) == true);
    test_int( it.count, 2);
    test_assert(it.entities[0] == Child1);
    test_assert(it.entities[1] == Child2);

    test_assert( ecs_scope_next(&it) == true);
    test_int( it.count, 2);
    test_assert(it.entities[0] == Child3);
    test_assert(it.entities[1] == Child4);

    test_assert( !ecs_scope_next(&it));

    ecs_fini(world);
}

void Hierarchies_path_depth_0() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    char *path = ecs_get_fullpath(world, Parent);
    test_str(path, "Parent");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_depth_1() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    char *path = ecs_get_fullpath(world, Child);
    test_str(path, "Parent.Child");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_depth_2() {
   ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    char *path = ecs_get_fullpath(world, GrandChild);
    test_str(path, "Parent.Child.GrandChild");
    free(path);

    ecs_fini(world);
}

void Hierarchies_rel_path_from_root() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    char *path = ecs_get_path(world, 0, Parent);
    test_str(path, "Parent");
    free(path);

    ecs_fini(world);
}

void Hierarchies_rel_path_from_self() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    char *path = ecs_get_path(world, Parent, Parent);
    test_str(path, "");
    free(path);

    ecs_fini(world);
}

void Hierarchies_rel_path_depth_1() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    char *path = ecs_get_path(world, Parent, Child);
    test_str(path, "Child");
    free(path);

    ecs_fini(world);
}

void Hierarchies_rel_path_depth_2() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    char *path = ecs_get_path(world, Parent, GrandChild);
    test_str(path, "Child.GrandChild");
    free(path);

    ecs_fini(world);
}

void Hierarchies_rel_path_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Parent2, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    char *path = ecs_get_path(world, Parent2, Child);
    test_str(path, "Parent.Child");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_for_component_0() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, 0);

    char *path = ecs_get_path_w_sep(world, 0, Parent, ecs_entity(Position), ".", NULL);
    test_str(path, "Parent");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_for_component_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Parent2, Position);
    ECS_ENTITY(world, Child, CHILDOF | Parent, CHILDOF | Parent2);

    char *path = ecs_get_path_w_sep(world, 0, Child, ecs_entity(Position), ".", NULL);
    test_str(path, "Parent2.Child");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_for_component_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Parent2, Position);
    ECS_ENTITY(world, Child1, CHILDOF | Parent);
    ECS_ENTITY(world, Child2, Position, CHILDOF | Parent2);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child1, CHILDOF | Parent2.Child2);

    char *path = ecs_get_path_w_sep(world, 0, GrandChild, ecs_entity(Position), ".", NULL);
    test_str(path, "Parent2.Child2.GrandChild");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_custom_sep() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    char *path = ecs_get_path_w_sep(world, 0, Child, 0, "::", NULL);
    test_str(path, "Parent::Child");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_custom_prefix() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    char *path = ecs_get_path_w_sep(world, 0, Child, 0,  "::", "::");
    test_str(path, "::Parent::Child");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_prefix_rel_match() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    char *path = ecs_get_path_w_sep(world, Parent, GrandChild, 0, "::", "::");
    test_str(path, "Child::GrandChild");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_prefix_rel_no_match() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Parent2, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    char *path = ecs_get_path_w_sep(world, Parent2, GrandChild, 0, "::", "::");
    test_str(path, "::Parent::Child::GrandChild");
    free(path);

    ecs_fini(world);
}

void Hierarchies_path_w_number() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_w_entity(world, ECS_CHILDOF | 1000);
    ecs_set(world, e, EcsName, {"Foo"});

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "1000.Foo");
    free(path);

    ecs_fini(world);
}


void Hierarchies_lookup_depth_0() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Parent");
    test_assert(e == Parent);

    ecs_fini(world);
}

void Hierarchies_lookup_depth_1() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Parent.Child");
    test_assert(e == Child);

    ecs_fini(world);
}

void Hierarchies_lookup_depth_2() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Parent.Child.GrandChild");
    test_assert(e == GrandChild);

    ecs_fini(world);
}

void Hierarchies_lookup_rel_0() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    ecs_entity_t e = ecs_lookup_path(world, Parent, "Child");
    test_assert(e == Child);

    ecs_fini(world);
}

void Hierarchies_lookup_rel_1() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    ecs_entity_t e = ecs_lookup_path(world, Parent, "Child.GrandChild");
    test_assert(e == GrandChild);

    ecs_fini(world);
}

void Hierarchies_lookup_rel_2() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);
    ECS_ENTITY(world, GrandGrandChild, CHILDOF | Parent.Child.GrandChild);

    ecs_entity_t e = ecs_lookup_path(world, Parent, "Child.GrandChild.GrandGrandChild");
    test_assert(e == GrandGrandChild);

    ecs_fini(world);
}

void Hierarchies_lookup_custom_sep() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    ecs_entity_t e = ecs_lookup_path_w_sep(world, 0, "Parent::Child", "::", NULL);
    test_assert(e == Child);

    ecs_fini(world);
}

void Hierarchies_lookup_custom_prefix() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    ecs_entity_t e = ecs_lookup_path_w_sep(world, 0, "::Parent::Child", "::", "::");
    test_assert(e == Child);

    ecs_fini(world);
}

void Hierarchies_lookup_custom_prefix_from_root() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    ecs_entity_t e = ecs_lookup_path_w_sep(world, Parent, "::Parent::Child", "::", "::");
    test_assert(e == Child);

    ecs_fini(world);
}

void Hierarchies_lookup_self() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);

    ecs_entity_t e = ecs_lookup_path(world, Parent, "");
    test_assert(e == Parent);

    ecs_fini(world);
}

void Hierarchies_lookup_number() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Parent2, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);
    ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child);

    ecs_entity_t e = ecs_new_w_entity(world, ECS_CHILDOF | 1000);
    ecs_set(world, e, EcsName, {"Foo"});

    ecs_entity_t c = ecs_lookup_path(world, 0, "1000.Foo");
    test_assert(e == c);

    ecs_fini(world);
}

void Hierarchies_scope_set() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);

    ecs_entity_t old_scope = ecs_set_scope(world, Scope);
    test_assert(old_scope == 0);

    ecs_entity_t scope = ecs_get_scope(world);
    test_assert(scope == Scope);

    ecs_fini(world);
}

void Hierarchies_scope_set_w_new() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);

    ecs_entity_t old_scope = ecs_set_scope(world, Scope);
    test_assert(old_scope == 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, ECS_CHILDOF | Scope));

    old_scope = ecs_set_scope(world, 0);
    test_assert(old_scope == Scope);

    e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_fini(world);
}

void Hierarchies_scope_set_w_new_staged() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);

    bool is_staged = ecs_staging_begin(world);
    test_assert(is_staged == false);

    ecs_entity_t old_scope = ecs_set_scope(world, Scope);
    test_assert(old_scope == 0);

    ecs_entity_t e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_has_entity(world, e, ECS_CHILDOF | Scope));

    old_scope = ecs_set_scope(world, 0);
    test_assert(old_scope == Scope);

    e = ecs_new(world, 0);
    test_assert(e != 0);
    test_assert(ecs_get_type(world, e) == NULL);

    ecs_staging_end(world, false);

    ecs_fini(world);
}

void Hierarchies_scope_set_again() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, ChildScope, 0);

    ecs_entity_t old_scope = ecs_set_scope(world, Scope);
    test_assert(old_scope == 0);

    old_scope = ecs_set_scope(world, ChildScope);
    test_assert(old_scope == Scope);

    ecs_fini(world);
}

void Hierarchies_scope_set_w_lookup() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, Child, CHILDOF | Scope);

    test_assert( ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t old_scope = ecs_set_scope(world, Scope);
    test_assert(old_scope == 0);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Child");
    test_assert(e != 0);
    test_assert(e == Child);

    old_scope = ecs_set_scope(world, 0);
    test_assert(old_scope == Scope);

    test_assert( ecs_lookup_fullpath(world, "Child") == 0);

    ecs_fini(world);
}

void Hierarchies_lookup_in_parent_from_scope() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, ChildScope, CHILDOF | Scope);
    ECS_ENTITY(world, Child, CHILDOF | Scope);

    test_assert( ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t old_scope = ecs_set_scope(world, ChildScope);
    test_assert(old_scope == 0);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Child");
    test_assert(e != 0);
    test_assert(e == Child);

    old_scope = ecs_set_scope(world, 0);
    test_assert(old_scope == ChildScope);

    test_assert( ecs_lookup_fullpath(world, "Child") == 0);

    ecs_fini(world);
}

void Hierarchies_lookup_in_root_from_scope() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);
    ECS_ENTITY(world, ChildScope, CHILDOF | Scope);
    ECS_ENTITY(world, Child, 0);

    ecs_entity_t old_scope = ecs_set_scope(world, ChildScope);
    test_assert(old_scope == 0);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Child");
    test_assert(e != 0);
    test_assert(e == Child);

    old_scope = ecs_set_scope(world, 0);
    test_assert(old_scope == ChildScope);

    ecs_fini(world);
}


void Hierarchies_scope_component() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Scope, 0);

    ecs_entity_t old_scope = ecs_set_scope(world, Scope);
    test_assert(old_scope == 0);

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_lookup_fullpath(world, "Position");
    test_assert(e != 0);
    test_assert(e == ecs_entity(Position));

    old_scope = ecs_set_scope(world, 0);
    test_assert(old_scope == Scope);

    e = ecs_lookup_fullpath(world, "Position");
    test_assert(e == 0);

    ecs_fini(world);
}

void Hierarchies_fullpath_for_core() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Parent, 0);
    ECS_ENTITY(world, Child, CHILDOF | Parent);

    char *path = ecs_get_fullpath(world, ecs_entity(EcsComponent));
    test_str(path, "Component");
    free(path);

    ecs_fini(world);
}


void Hierarchies_new_from_path_depth_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(ecs_vector_count(type), 1);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "foo");
    free(path);

    ecs_fini(world);
}

void Hierarchies_new_from_path_depth_1() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo.bar");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "bar");

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(ecs_vector_count(type), 2);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "foo.bar");
    free(path);

    ecs_fini(world);
}

void Hierarchies_new_from_path_depth_2() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo.bar.hello");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "hello");

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(ecs_vector_count(type), 2);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "foo.bar.hello");
    free(path);

    ecs_fini(world);
}


void Hierarchies_new_from_path_existing_depth_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");

    ecs_entity_t f = ecs_new_from_path(world, 0, "foo");
    test_assert(f != 0);
    test_assert(f == e);
    test_str(ecs_get_name(world, e), "foo");

    ecs_fini(world);
}

void Hierarchies_new_from_path_existing_depth_1() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo.bar");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "bar");

    ecs_entity_t f = ecs_new_from_path(world, 0, "foo.bar");
    test_assert(f != 0);
    test_assert(f == e);
    test_str(ecs_get_name(world, e), "bar");

    ecs_fini(world);
}

void Hierarchies_new_from_path_existing_depth_2() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo.bar.hello");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "hello");

    ecs_entity_t f = ecs_new_from_path(world, 0, "foo.bar.hello");
    test_assert(f != 0);
    test_assert(f == e);
    test_str(ecs_get_name(world, e), "hello");

    ecs_fini(world);
}

void Hierarchies_add_path_depth_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t id = ecs_new(world, 0);
    test_assert(id != 0);

    ecs_entity_t e = ecs_add_path(world, id, 0, "foo");
    test_assert(e != 0);
    test_assert(e == id);
    test_str(ecs_get_name(world, e), "foo");

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(ecs_vector_count(type), 1);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "foo");
    free(path);

    ecs_fini(world);
}

void Hierarchies_add_path_depth_1() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t id = ecs_new(world, 0);
    test_assert(id != 0);

    ecs_entity_t e = ecs_add_path(world, id, 0, "foo.bar");
    test_assert(e != 0);
    test_assert(e == id);
    test_str(ecs_get_name(world, e), "bar");

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(ecs_vector_count(type), 2);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "foo.bar");
    free(path);

    ecs_fini(world);
}

void Hierarchies_add_path_depth_2() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t id = ecs_new(world, 0);
    test_assert(id != 0);

    ecs_entity_t e = ecs_add_path(world, id, 0, "foo.bar.hello");
    test_assert(e != 0);
    test_assert(e == id);
    test_str(ecs_get_name(world, e), "hello");

    ecs_type_t type = ecs_get_type(world, e);
    test_assert(type != NULL);
    test_int(ecs_vector_count(type), 2);

    char *path = ecs_get_fullpath(world, e);
    test_str(path, "foo.bar.hello");
    free(path);

    ecs_fini(world);
}

void Hierarchies_add_path_existing_depth_0() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "foo");    

    ecs_entity_t id = ecs_new(world, 0);
    test_assert(id != 0);

    ecs_entity_t f = ecs_add_path(world, id, 0, "foo");
    test_assert(f != 0);
    test_assert(f != id);
    test_assert(f == e);
    test_str(ecs_get_name(world, e), "foo");

    ecs_fini(world);
}

void Hierarchies_add_path_existing_depth_1() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo.bar");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "bar");    

    ecs_entity_t id = ecs_new(world, 0);
    test_assert(id != 0);

    ecs_entity_t f = ecs_add_path(world, id, 0, "foo.bar");
    test_assert(f != 0);
    test_assert(f != id);
    test_assert(f == e);
    test_str(ecs_get_name(world, e), "bar");

    ecs_fini(world);
}

void Hierarchies_add_path_existing_depth_2() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_new_from_path(world, 0, "foo.bar.hello");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "hello");    

    ecs_entity_t id = ecs_new(world, 0);
    test_assert(id != 0);

    ecs_entity_t f = ecs_add_path(world, id, 0, "foo.bar.hello");
    test_assert(f != 0);
    test_assert(f != id);
    test_assert(f == e);
    test_str(ecs_get_name(world, e), "hello");

    ecs_fini(world);
}

void Hierarchies_new_w_child_in_root() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t scope = ecs_new(world, 0);
    ecs_entity_t parent = ecs_new(world, 0);
    
    ecs_set_scope(world, scope);

    ecs_entity_t child = ecs_new_w_entity(world, ECS_CHILDOF | parent);
    test_assert( ecs_has_entity(world, child, ECS_CHILDOF | parent));
    test_assert( !ecs_has_entity(world, child, ECS_CHILDOF | scope));

    ecs_entity_t child_2 = ecs_new(world, 0);
    test_assert( !ecs_has_entity(world, child_2, ECS_CHILDOF | parent));
    test_assert( ecs_has_entity(world, child_2, ECS_CHILDOF | scope));

    ecs_fini(world);
}