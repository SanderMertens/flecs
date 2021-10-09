#include <api.h>

#define HEAD
#define LINE "\n"

void Plecs_null() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, NULL) == 0);

    ecs_fini(world);
}

void Plecs_empty() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "") == 0);

    ecs_fini(world);
}

void Plecs_space() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, " ") == 0);

    ecs_fini(world);
}

void Plecs_space_newline() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, " \n \n") == 0);

    ecs_fini(world);
}

void Plecs_two_empty_newlines() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n\n") == 0);

    ecs_fini(world);
}

void Plecs_three_empty_newlines() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n\n\n") == 0);

    ecs_fini(world);
}

void Plecs_newline_trailing_space() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n ") == 0);

    ecs_fini(world);
}

void Plecs_newline_trailing_spaces() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n   ") == 0);

    ecs_fini(world);
}

void Plecs_multiple_trailing_newlines() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\n\n\n") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");
    test_int(ecs_vector_count(ecs_get_type(world, foo)), 1);

    ecs_fini(world);
}

void Plecs_invalid_2_identifiers_separated_by_space() {
    ecs_tracing_enable(-4);

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo Bar"
    LINE "Hello";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_entity() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");
    test_int(ecs_vector_count(ecs_get_type(world, foo)), 1);

    ecs_fini(world);
}

void Plecs_entity_w_entity() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo(Subj)") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");

    ecs_entity_t subj = ecs_lookup(world, "Subj");
    test_assert(subj != 0);
    test_str(ecs_get_name(world, subj), "Subj");

    test_assert(ecs_has_id(world, subj, foo));

    ecs_fini(world);
}

void Plecs_entity_w_pair() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Rel(Subj, Obj)") == 0);

    ecs_entity_t rel = ecs_lookup(world, "Rel");
    test_assert(rel != 0);
    test_str(ecs_get_name(world, rel), "Rel");

    ecs_entity_t obj = ecs_lookup(world, "Obj");
    test_assert(obj != 0);
    test_str(ecs_get_name(world, obj), "Obj");

    ecs_entity_t subj = ecs_lookup(world, "Subj");
    test_assert(subj != 0);
    test_str(ecs_get_name(world, subj), "Subj");

    test_assert(ecs_has_pair(world, subj, rel, obj));

    ecs_fini(world);
}

void Plecs_2_entities() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\nBar\n") == 0);

    ecs_entity_t e = ecs_lookup(world, "Foo");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Plecs_2_entities_w_entities() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo(Subj_1)\nBar(Subj_2)\n") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);
    test_str(ecs_get_name(world, bar), "Bar");

    ecs_entity_t subj_1 = ecs_lookup(world, "Subj_1");
    test_assert(subj_1 != 0);
    test_str(ecs_get_name(world, subj_1), "Subj_1");

    ecs_entity_t subj_2 = ecs_lookup(world, "Subj_2");
    test_assert(subj_2 != 0);
    test_str(ecs_get_name(world, subj_2), "Subj_2");

    test_assert(ecs_has_id(world, subj_1, foo));
    test_assert(ecs_has_id(world, subj_2, bar));

    ecs_fini(world);
}

void Plecs_3_entities_w_pairs() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, 
        "Rel_1(Subj_1, Obj_1)\n"
        "Rel_1(Subj_2, Obj_2)\n"
        "Rel_2(Subj_3, Obj_2)\n") == 0);

    ecs_entity_t rel_1 = ecs_lookup(world, "Rel_1");
    test_assert(rel_1 != 0);
    test_str(ecs_get_name(world, rel_1), "Rel_1");

    ecs_entity_t rel_2 = ecs_lookup(world, "Rel_2");
    test_assert(rel_2 != 0);
    test_str(ecs_get_name(world, rel_2), "Rel_2");

    ecs_entity_t obj_1 = ecs_lookup(world, "Obj_1");
    test_assert(obj_1 != 0);
    test_str(ecs_get_name(world, obj_1), "Obj_1");

    ecs_entity_t obj_2 = ecs_lookup(world, "Obj_2");
    test_assert(obj_2 != 0);
    test_str(ecs_get_name(world, obj_2), "Obj_2");

    ecs_entity_t subj_1 = ecs_lookup(world, "Subj_1");
    test_assert(subj_1 != 0);
    test_str(ecs_get_name(world, subj_1), "Subj_1");

    ecs_entity_t subj_2 = ecs_lookup(world, "Subj_2");
    test_assert(subj_2 != 0);
    test_str(ecs_get_name(world, subj_2), "Subj_2");

    ecs_entity_t subj_3 = ecs_lookup(world, "Subj_3");
    test_assert(subj_3 != 0);
    test_str(ecs_get_name(world, subj_3), "Subj_3");  

    test_assert(ecs_has_pair(world, subj_1, rel_1, obj_1));
    test_assert(ecs_has_pair(world, subj_2, rel_1, obj_2));
    test_assert(ecs_has_pair(world, subj_3, rel_2, obj_2));

    ecs_fini(world);
}

void Plecs_line_comment() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "// Foo(Bar)\n") == 0);

    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "Bar") == 0);

    ecs_fini(world);
}

void Plecs_line_comment_before_stmt() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "// Hello(World)\nFoo\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Plecs_line_comment_after_stmt() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\n// Hello(World)\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Plecs_line_comment_between_stmt() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\n// Hello(World)\nBar\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_multiple_line_comment() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "// Hello(World)\n// Boo(Baz)\nFoo") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "World") == 0);
    test_assert(ecs_lookup(world, "Boo") == 0);
    test_assert(ecs_lookup(world, "Baz") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Plecs_line_comment_after_stmt_same_line() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo // Hello(World)\nBar\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,Hello,Worlds") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred_w_subj() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo(Bar),Hello(Worlds)") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup(world, "Worlds");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);

    test_assert(ecs_has_id(world, bar, foo));
    test_assert(ecs_has_id(world, _world, hello));

    ecs_fini(world);
}

void Plecs_comma_separated_pred_w_subj_obj() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo(Bar, Obj1),Hello(Worlds, Obj2)") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup(world, "Worlds");
    ecs_entity_t obj1 = ecs_lookup(world, "Obj1");
    ecs_entity_t obj2 = ecs_lookup(world, "Obj2");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);
    test_assert(obj1 != 0);
    test_assert(obj2 != 0);

    test_assert(ecs_has_pair(world, bar, foo, obj1));
    test_assert(ecs_has_pair(world, _world, hello, obj2));

    ecs_fini(world);
}

void Plecs_comma_separated_pred_trailing_comma() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,Hello,Worlds,") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred_trailing_comma_newline() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,Hello,Worlds,\n") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred_trailing_comma_newline_multiline() {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,\nHello,Worlds,") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_hierarchy_1_child() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_hierarchy_2_children() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " ChildA"
    LINE " ChildB"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "ChildA") == 0);
    test_assert(ecs_lookup_fullpath(world, "ChildB") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child_a = ecs_lookup_fullpath(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup_fullpath(world, "Parent.ChildB");

    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_b, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_hierarchy_1_child_same_line() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { Child }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_hierarchy_2_children_same_line() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { ChildA, ChildB }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "ChildA") == 0);
    test_assert(ecs_lookup_fullpath(world, "ChildB") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child_a = ecs_lookup_fullpath(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup_fullpath(world, "Parent.ChildB");

    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_b, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_entity_after_hierarchy() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(foo != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert(!ecs_has_pair(world, foo, EcsChildOf, parent));
    test_assert(!ecs_has_pair(world, foo, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Plecs_newline_before_scope_open() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "{"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_comment_before_scope_open() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent // Some(Comment)"
    LINE "{"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    test_assert(ecs_lookup_fullpath(world, "Some") == 0);
    test_assert(ecs_lookup_fullpath(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_comment_after_newline_before_scope_open() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "// Some(Comment)"
    LINE "{"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    test_assert(ecs_lookup_fullpath(world, "Some") == 0);
    test_assert(ecs_lookup_fullpath(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_hierarchy_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child {"
    LINE "  GrandChild"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);
    test_assert(ecs_lookup_fullpath(world, "GrandChild") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Parent.Child");
    ecs_entity_t grandchild = ecs_lookup_fullpath(world, "Parent.Child.GrandChild");

    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(grandchild != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, grandchild, EcsChildOf, child));

    ecs_fini(world);
}

void Plecs_hierarchy_2_levels_2_subtrees() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " ChildA {"
    LINE "  GrandChild"
    LINE " }"
    LINE " ChildB {"
    LINE "  GrandChild"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "ChildA") == 0);
    test_assert(ecs_lookup_fullpath(world, "ChildB") == 0);
    test_assert(ecs_lookup_fullpath(world, "GrandChild") == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t child_a = ecs_lookup_fullpath(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup_fullpath(world, "Parent.ChildB");
    ecs_entity_t grandchild_a = ecs_lookup_fullpath(world, "Parent.ChildA.GrandChild");
    ecs_entity_t grandchild_b = ecs_lookup_fullpath(world, "Parent.ChildB.GrandChild");

    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);
    test_assert(child_a != child_b);
    test_assert(grandchild_a != 0);
    test_assert(grandchild_b != 0);
    test_assert(grandchild_a != grandchild_b);

    test_assert(ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_b, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, grandchild_a, EcsChildOf, child_a));
    test_assert(ecs_has_pair(world, grandchild_b, EcsChildOf, child_b));

    ecs_fini(world);
}

void Plecs_missing_end_of_scope() {
    ecs_tracing_enable(-4);
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    test_assert(ecs_get_scope(world) == 0);
    test_assert(ecs_get_with(world) == 0);

    ecs_fini(world);
}

void Plecs_missing_end_of_predicate_scope() {
    ecs_tracing_enable(-4);
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent() {"
    LINE " Child";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    test_assert(ecs_get_scope(world) == 0);
    test_assert(ecs_get_with(world) == 0);

    ecs_fini(world);
}

void Plecs_create_in_scope() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);

    ecs_set_scope(world, Root);

    const char *expr =
    HEAD "Parent { Child }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    ecs_set_scope(world, 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Root.Parent");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Root.Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, parent, EcsChildOf, Root));

    ecs_fini(world);
}

void Plecs_hierarchy_w_pred_subj() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo(Bar) {"
    LINE " Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup_fullpath(world, "Child") == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Bar.Child");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);

    test_assert(ecs_has_id(world, bar, foo));
    test_assert(ecs_has_pair(world, child, EcsChildOf, bar));

    ecs_fini(world);
}

void Plecs_hierarchy_custom_relation() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE " Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Child");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);

    test_assert(!ecs_has_id(world, bar, foo));
    test_assert(!ecs_has_id(world, foo, bar));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, foo));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, bar));
    test_assert(ecs_has_pair(world, child, foo, bar));

    ecs_fini(world);
}

void Plecs_hierarchy_custom_relation_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE "  (Rel, ObjA) {"
    LINE "    Child"
    LINE "    ChildA"
    LINE "  }"
    LINE "  (Rel, ObjB) {"
    LINE "    Child"
    LINE "    ChildB"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t rel = ecs_lookup_fullpath(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup_fullpath(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup_fullpath(world, "ObjB");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Child");
    ecs_entity_t child_a = ecs_lookup_fullpath(world, "ChildA");
    ecs_entity_t child_b = ecs_lookup_fullpath(world, "ChildB");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(rel != 0);
    test_assert(obj_a != 0);
    test_assert(obj_b != 0);
    test_assert(child != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(!ecs_has_id(world, bar, foo));
    test_assert(!ecs_has_id(world, foo, bar));

    test_assert(!ecs_has_id(world, rel, obj_a));
    test_assert(!ecs_has_id(world, obj_a, rel));

    test_assert(!ecs_has_id(world, rel, obj_b));
    test_assert(!ecs_has_id(world, obj_b, rel));

    test_assert(!ecs_has_pair(world, child, EcsChildOf, foo));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, bar));

    test_assert(ecs_has_pair(world, child, rel, obj_a));
    test_assert(ecs_has_pair(world, child, rel, obj_b));

    test_assert(ecs_has_pair(world, child_a, rel, obj_a));
    test_assert(ecs_has_pair(world, child_b, rel, obj_b));

    ecs_fini(world);
}

void Plecs_entity_after_hierarchy_custom_relation() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE " Child"
    LINE "}"
    LINE "Hello";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Child");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);
    test_assert(hello != 0);

    test_assert(!ecs_has_id(world, bar, foo));
    test_assert(!ecs_has_id(world, foo, bar));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, foo));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, bar));
    test_assert(ecs_has_pair(world, child, foo, bar));
    test_assert(!ecs_has_pair(world, hello, foo, bar));

    ecs_fini(world);
}

void Plecs_entity_after_hierarchy_custom_relation_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE "  (Rel, ObjA) {"
    LINE "    Child"
    LINE "    ChildA"
    LINE "  }"
    LINE "  TestA"
    LINE "  (Rel, ObjB) {"
    LINE "    Child"
    LINE "    ChildB"
    LINE "  }"
    LINE "}"
    LINE "TestB";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t rel = ecs_lookup_fullpath(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup_fullpath(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup_fullpath(world, "ObjB");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Child");
    ecs_entity_t child_a = ecs_lookup_fullpath(world, "ChildA");
    ecs_entity_t child_b = ecs_lookup_fullpath(world, "ChildB");

    ecs_entity_t test_a = ecs_lookup_fullpath(world, "TestA");
    ecs_entity_t test_b = ecs_lookup_fullpath(world, "TestB");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(rel != 0);
    test_assert(obj_a != 0);
    test_assert(obj_b != 0);
    test_assert(child != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(test_a != 0);
    test_assert(test_b != 0);

    test_assert(!ecs_has_id(world, bar, foo));
    test_assert(!ecs_has_id(world, foo, bar));

    test_assert(!ecs_has_id(world, rel, obj_a));
    test_assert(!ecs_has_id(world, obj_a, rel));

    test_assert(!ecs_has_id(world, rel, obj_b));
    test_assert(!ecs_has_id(world, obj_b, rel));

    test_assert(!ecs_has_pair(world, child, EcsChildOf, foo));
    test_assert(!ecs_has_pair(world, child, EcsChildOf, bar));

    test_assert(ecs_has_pair(world, child, rel, obj_a));
    test_assert(ecs_has_pair(world, child, rel, obj_b));

    test_assert(ecs_has_pair(world, child_a, rel, obj_a));
    test_assert(ecs_has_pair(world, child_b, rel, obj_b));

    test_assert(ecs_has_pair(world, test_a, foo, bar));
    test_assert(!ecs_has_pair(world, test_a, rel, EcsWildcard));

    test_assert(!ecs_has_pair(world, test_b, foo, bar));
    test_assert(!ecs_has_pair(world, test_b, rel, EcsWildcard));

    ecs_fini(world);
}

void Plecs_hierarchy_custom_relation_apply_to_object() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Rel, ObjA) {"
    LINE " (Rel, ObjB) {"
    LINE "   ObjC"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t rel = ecs_lookup_fullpath(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup_fullpath(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup_fullpath(world, "ObjB");
    ecs_entity_t obj_c = ecs_lookup_fullpath(world, "ObjC");

    test_assert(rel != 0);
    test_assert(obj_a != 0);
    test_assert(obj_b != 0);
    test_assert(obj_c != 0);

    test_assert(ecs_has_pair(world, obj_b, rel, obj_a));
    test_assert(ecs_has_pair(world, obj_c, rel, obj_b));
    test_assert(!ecs_has_pair(world, obj_c, rel, obj_a));

    ecs_fini(world);
}

void Plecs_hierarchy_custom_relation_apply_to_object_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Rel, ObjA) {"
    LINE " (Rel, ObjB) {"
    LINE "   (Rel, ObjC) {"
    LINE "     ObjD"
    LINE "   }"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t rel = ecs_lookup_fullpath(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup_fullpath(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup_fullpath(world, "ObjB");
    ecs_entity_t obj_c = ecs_lookup_fullpath(world, "ObjC");
    ecs_entity_t obj_d = ecs_lookup_fullpath(world, "ObjD");

    test_assert(rel != 0);
    test_assert(obj_a != 0);
    test_assert(obj_b != 0);
    test_assert(obj_c != 0);
    test_assert(obj_d != 0);

    test_assert(ecs_has_pair(world, obj_b, rel, obj_a));
    test_assert(ecs_has_pair(world, obj_c, rel, obj_b));
    test_assert(!ecs_has_pair(world, obj_c, rel, obj_a));
    test_assert(ecs_has_pair(world, obj_d, rel, obj_c));
    test_assert(!ecs_has_pair(world, obj_d, rel, obj_b));
    test_assert(!ecs_has_pair(world, obj_d, rel, obj_a));

    ecs_fini(world);
}

void Plecs_pred_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo() {"
    LINE " Hello"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Foo.Hello");

    test_assert(foo != 0);
    test_assert(hello != 0);

    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, hello, foo));

    ecs_fini(world);
}

void Plecs_pred_scope_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo() {"
    LINE "  Bar() {"
    LINE "    Hello"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Bar.Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);

    test_assert(!ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(ecs_has_pair(world, hello, EcsChildOf, bar));
    test_assert(!ecs_has_id(world, hello, foo));
    test_assert(!ecs_has_id(world, hello, bar));

    test_assert(!ecs_has_pair(world, bar, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, bar, foo));

    ecs_fini(world);
}

void Plecs_pred_scope_inside_with() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag {"
    HEAD " Foo() {"
    LINE "  Hello"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tag = ecs_lookup_fullpath(world, "Tag");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Foo.Hello");

    test_assert(foo != 0);
    test_assert(hello != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, hello, foo));
    test_assert(ecs_has_id(world, hello, tag));

    test_assert(!ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Plecs_pred_scope_nested_w_subj_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    HEAD " Foo() {"
    LINE "  Hello"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup_fullpath(world, "Parent");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Foo.Hello");

    test_assert(parent != 0);
    test_assert(foo != 0);
    test_assert(hello != 0);

    test_assert(!ecs_has_pair(world, hello, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, hello, foo));

    test_assert(!ecs_has_pair(world, foo, EcsChildOf, parent));

    ecs_fini(world);
}


void Plecs_with_tag() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag {"
    LINE "  Foo"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t tag = ecs_lookup_fullpath(world, "Tag");

    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Plecs_with_tag_2_entities() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag {"
    LINE "  Foo"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t tag = ecs_lookup_fullpath(world, "Tag");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));
    test_assert(ecs_has_id(world, bar, tag));

    ecs_fini(world);
}

void Plecs_with_tag_same_line() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag { Foo }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t tag = ecs_lookup_fullpath(world, "Tag");

    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Plecs_with_tag_2_entities_same_line() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag { Foo, Bar }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t tag = ecs_lookup_fullpath(world, "Tag");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));
    test_assert(ecs_has_id(world, bar, tag));

    ecs_fini(world);
}

void Plecs_with_tag_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with TagA {"
    HEAD " with TagB {"
    LINE "  Foo"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t tag_a = ecs_lookup_fullpath(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup_fullpath(world, "TagB");

    test_assert(foo != 0);
    test_assert(tag_a != 0);
    test_assert(tag_b != 0);

    test_assert(ecs_has_id(world, foo, tag_a));
    test_assert(ecs_has_id(world, foo, tag_b));

    ecs_fini(world);
}

void Plecs_with_tag_2_levels_2_subtrees() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with TagA {"
    HEAD " with TagB {"
    LINE "  Foo"
    LINE "  BarA"
    LINE " }"
    HEAD " with TagC {"
    LINE "  Foo"
    LINE "  BarB"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t tag_a = ecs_lookup_fullpath(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup_fullpath(world, "TagB");
    ecs_entity_t tag_c = ecs_lookup_fullpath(world, "TagC");
    ecs_entity_t bar_a = ecs_lookup_fullpath(world, "BarA");
    ecs_entity_t bar_b = ecs_lookup_fullpath(world, "BarB");

    test_assert(foo != 0);
    test_assert(tag_a != 0);
    test_assert(tag_b != 0);
    test_assert(bar_a != 0);
    test_assert(bar_b != 0);

    test_assert(ecs_has_id(world, foo, tag_a));
    test_assert(ecs_has_id(world, foo, tag_b));
    test_assert(ecs_has_id(world, foo, tag_c));

    test_assert(ecs_has_id(world, bar_a, tag_a));
    test_assert(ecs_has_id(world, bar_b, tag_a));

    test_assert(ecs_has_id(world, bar_a, tag_b));
    test_assert(ecs_has_id(world, bar_b, tag_c));

    test_assert(!ecs_has_id(world, bar_b, tag_b));
    test_assert(!ecs_has_id(world, bar_a, tag_c));

    ecs_fini(world);
}

void Plecs_with_n_tags() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with TagA, TagB {"
    LINE " Foo"
    LINE " Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t tag_a = ecs_lookup_fullpath(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup_fullpath(world, "TagB");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(tag_a != 0);
    test_assert(tag_b != 0);

    test_assert(ecs_has_id(world, foo, tag_a));
    test_assert(ecs_has_id(world, foo, tag_b));

    test_assert(ecs_has_id(world, bar, tag_a));
    test_assert(ecs_has_id(world, bar, tag_b));

    ecs_fini(world);
}

void Plecs_with_n_tags_2_levels() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with TagA, TagB {"
    LINE " with TagC, TagD, TagE {"
    LINE "  Foo"
    LINE " }"
    LINE " HelloA"
    LINE " with TagF, TagG, TagH {"
    LINE "  Bar"
    LINE " }"
    LINE " HelloB"
    LINE "}"
    LINE "HelloC";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tag_a = ecs_lookup_fullpath(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup_fullpath(world, "TagB");
    ecs_entity_t tag_c = ecs_lookup_fullpath(world, "TagC");
    ecs_entity_t tag_d = ecs_lookup_fullpath(world, "TagD");
    ecs_entity_t tag_e = ecs_lookup_fullpath(world, "TagE");
    ecs_entity_t tag_f = ecs_lookup_fullpath(world, "TagF");
    ecs_entity_t tag_g = ecs_lookup_fullpath(world, "TagG");
    ecs_entity_t tag_h = ecs_lookup_fullpath(world, "TagH");

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");

    ecs_entity_t hello_a = ecs_lookup_fullpath(world, "HelloA");
    ecs_entity_t hello_b = ecs_lookup_fullpath(world, "HelloB");
    ecs_entity_t hello_c = ecs_lookup_fullpath(world, "HelloC");

    test_assert(tag_a != 0);
    test_assert(tag_b != 0);
    test_assert(tag_a != 0);
    test_assert(tag_b != 0);
    test_assert(tag_a != 0);
    test_assert(tag_b != 0);
    test_assert(tag_b != 0);
    test_assert(tag_b != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello_a != 0);
    test_assert(hello_b != 0);
    test_assert(hello_c != 0);

    test_assert(ecs_has_id(world, foo, tag_a));
    test_assert(ecs_has_id(world, foo, tag_b));
    test_assert(ecs_has_id(world, foo, tag_c));
    test_assert(ecs_has_id(world, foo, tag_d));
    test_assert(ecs_has_id(world, foo, tag_e));
    test_assert(!ecs_has_id(world, foo, tag_f));
    test_assert(!ecs_has_id(world, foo, tag_g));
    test_assert(!ecs_has_id(world, foo, tag_h));

    test_assert(ecs_has_id(world, bar, tag_a));
    test_assert(ecs_has_id(world, bar, tag_b));
    test_assert(!ecs_has_id(world, bar, tag_c));
    test_assert(!ecs_has_id(world, bar, tag_d));
    test_assert(!ecs_has_id(world, bar, tag_e));
    test_assert(ecs_has_id(world, bar, tag_f));
    test_assert(ecs_has_id(world, bar, tag_g));
    test_assert(ecs_has_id(world, bar, tag_h));

    test_assert(ecs_has_id(world, hello_a, tag_a));
    test_assert(ecs_has_id(world, hello_a, tag_b));
    test_assert(!ecs_has_id(world, hello_a, tag_c));
    test_assert(!ecs_has_id(world, hello_a, tag_d));
    test_assert(!ecs_has_id(world, hello_a, tag_e));
    test_assert(!ecs_has_id(world, hello_a, tag_f));
    test_assert(!ecs_has_id(world, hello_a, tag_g));
    test_assert(!ecs_has_id(world, hello_a, tag_h));

    test_assert(ecs_has_id(world, hello_b, tag_a));
    test_assert(ecs_has_id(world, hello_b, tag_b));
    test_assert(!ecs_has_id(world, hello_b, tag_c));
    test_assert(!ecs_has_id(world, hello_b, tag_d));
    test_assert(!ecs_has_id(world, hello_b, tag_e));
    test_assert(!ecs_has_id(world, hello_b, tag_f));
    test_assert(!ecs_has_id(world, hello_b, tag_g));
    test_assert(!ecs_has_id(world, hello_b, tag_h));

    test_assert(!ecs_has_id(world, hello_c, tag_a));
    test_assert(!ecs_has_id(world, hello_c, tag_b));
    test_assert(!ecs_has_id(world, hello_c, tag_c));
    test_assert(!ecs_has_id(world, hello_c, tag_d));
    test_assert(!ecs_has_id(world, hello_c, tag_e));
    test_assert(!ecs_has_id(world, hello_c, tag_f));
    test_assert(!ecs_has_id(world, hello_c, tag_g));
    test_assert(!ecs_has_id(world, hello_c, tag_h));

    ecs_fini(world);
}

void Plecs_with_after_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "E1 { }"
    HEAD "with E2 {"
    LINE "  E3() { }"
    LINE "  E4 { }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup_fullpath(world, "E1");
    ecs_entity_t e2 = ecs_lookup_fullpath(world, "E2");
    ecs_entity_t e3 = ecs_lookup_fullpath(world, "E3");
    ecs_entity_t e4 = ecs_lookup_fullpath(world, "E4");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    test_assert(!ecs_has_pair(world, e2, EcsChildOf, e1));
    test_assert(!ecs_has_pair(world, e3, EcsChildOf, e1));
    test_assert(!ecs_has_pair(world, e4, EcsChildOf, e1));

    test_assert(!ecs_has_id(world, e3, e2));
    test_assert(ecs_has_id(world, e4, e2));

    ecs_fini(world);
}

void Plecs_with_after_with() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with E1 { }"
    HEAD "with E2 {"
    LINE "  E3() { }"
    LINE "  E4 { }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup_fullpath(world, "E1");
    ecs_entity_t e2 = ecs_lookup_fullpath(world, "E2");
    ecs_entity_t e3 = ecs_lookup_fullpath(world, "E3");
    ecs_entity_t e4 = ecs_lookup_fullpath(world, "E4");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    test_assert(!ecs_has_id(world, e2, e1));
    test_assert(!ecs_has_id(world, e3, e1));
    test_assert(!ecs_has_id(world, e4, e1));

    test_assert(!ecs_has_id(world, e3, e2));
    test_assert(ecs_has_id(world, e4, e2));

    ecs_fini(world);
}

void Plecs_scope_inside_with_inside_scope() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Sun {"
    LINE "  with Planet {"
    LINE "    Earth {"
    LINE "      Moon"
    LINE "    }"
    LINE "    Mars"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t sun = ecs_lookup_fullpath(world, "Sun");
    ecs_entity_t planet = ecs_lookup_fullpath(world, "Planet");
    ecs_entity_t earth = ecs_lookup_fullpath(world, "Sun.Earth");
    ecs_entity_t moon = ecs_lookup_fullpath(world, "Sun.Earth.Moon");
    ecs_entity_t mars = ecs_lookup_fullpath(world, "Sun.Mars");

    test_assert(sun != 0);
    test_assert(planet != 0);
    test_assert(earth != 0);
    test_assert(moon != 0);
    test_assert(mars != 0);

    test_assert(ecs_has_id(world, earth, planet));
    test_assert(ecs_has_id(world, moon, planet));
    test_assert(ecs_has_id(world, mars, planet));

    test_assert(ecs_has_pair(world, earth, EcsChildOf, sun));
    test_assert(ecs_has_pair(world, moon, EcsChildOf, earth));
    test_assert(ecs_has_pair(world, mars, EcsChildOf, sun));

    test_assert(!ecs_has_id(world, earth, sun));
    test_assert(!ecs_has_id(world, moon, earth));
    test_assert(!ecs_has_id(world, mars, sun));

    test_assert(!ecs_has_pair(world, planet, EcsChildOf, sun));

    ecs_fini(world);
}

void Plecs_with_inside_scope() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth {"
    LINE "  with Continent {"
    LINE "    Europe"
    LINE "  }"
    LINE "  Europe"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup_fullpath(world, "Earth");
    ecs_entity_t continent = ecs_lookup_fullpath(world, "Continent");
    ecs_entity_t europe = ecs_lookup_fullpath(world, "Earth.Europe");

    test_assert(earth != 0);
    test_assert(continent != 0);
    test_assert(europe != 0);

    test_assert( ecs_has_pair(world, europe, EcsChildOf, earth));
    test_assert( !ecs_has_pair(world, continent, EcsChildOf, earth));
    test_assert( !ecs_has_id(world, europe, earth));
    test_assert( !ecs_has_id(world, continent, earth));

    ecs_fini(world);
}

void Plecs_assignment_empty() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup_fullpath(world, "Earth");

    test_assert(earth != 0);

    ecs_fini(world);
}

void Plecs_assignment_w_1() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "  Planet"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup_fullpath(world, "Earth");
    ecs_entity_t planet = ecs_lookup_fullpath(world, "Planet");

    test_assert(earth != 0);
    test_assert(planet != 0);

    test_assert( ecs_has_id(world, earth, planet));

    ecs_fini(world);
}

void Plecs_assignment_w_2() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "  Planet"
    LINE "  Habitable"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup_fullpath(world, "Earth");
    ecs_entity_t planet = ecs_lookup_fullpath(world, "Planet");
    ecs_entity_t habitable = ecs_lookup_fullpath(world, "Habitable");

    test_assert(earth != 0);
    test_assert(planet != 0);
    test_assert(habitable != 0);

    test_assert( ecs_has_id(world, earth, planet));
    test_assert( ecs_has_id(world, earth, habitable));

    ecs_fini(world);
}

void Plecs_assignment_w_pair() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "  Planet"
    LINE "  (ChildOf, Sun)"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup_fullpath(world, "Sun.Earth");
    ecs_entity_t planet = ecs_lookup_fullpath(world, "Planet");
    ecs_entity_t sun = ecs_lookup_fullpath(world, "Sun");

    test_assert(earth != 0);
    test_assert(planet != 0);
    test_assert(sun != 0);

    test_assert( ecs_has_id(world, earth, planet));
    test_assert( ecs_has_pair(world, earth, EcsChildOf, sun));

    ecs_fini(world);
}

void Plecs_assignment_w_invalid_subject() {
    ecs_tracing_enable(-4);

    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "  Planet(Mars)"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assignment_w_invalid_with() {
    ecs_tracing_enable(-4);
    
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "  with Planet { Mars }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assignment_w_invalid_scope() {
    ecs_tracing_enable(-4);
    
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth = {"
    LINE "  Europe { Netherlands }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_inherit_w_colon() {    
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Foo : Bar";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_fini(world);
}

void Plecs_inherit_w_colon_w_scope() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Foo : Bar { Child }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t child = ecs_lookup_fullpath(world, "Foo.Child");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));
    test_assert(ecs_has_pair(world, child, EcsChildOf, foo));

    ecs_fini(world);
}

void Plecs_inherit_w_colon_w_assign() {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Foo : Bar = { Comp }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t comp = ecs_lookup_fullpath(world, "Comp");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));
    test_assert(ecs_has_id(world, foo, comp));

    ecs_fini(world);
}

void Plecs_assign_component_value() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Position(Foo) = {10, 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_assign_2_component_values() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Velocity",
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Position(Foo) = {10, 20}"
    LINE "Velocity(Foo) = {1, 2}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    test_assert(ecs_has(world, foo, Velocity));

    {
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    }

    {
    const Velocity *ptr = ecs_get(world, foo, Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 1);
    test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Plecs_assign_component_value_in_assign_scope() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo = {"
    LINE " Position = {10, 20}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_assign_2_component_values_in_assign_scope() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Position",
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity.name = "Velocity",
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo = {"
    LINE " Position = {10, 20}"
    LINE " Velocity = {1, 2}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    test_assert(ecs_has(world, foo, Velocity));

    {
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    }

    {
    const Velocity *ptr = ecs_get(world, foo, Velocity);
    test_assert(ptr != NULL);
    test_int(ptr->x, 1);
    test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Plecs_type_and_assign_in_plecs() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct(Position) {"
    LINE "  flecs.meta.Member(x) = {flecs.meta.f32}"
    LINE "  flecs.meta.Member(y) = {flecs.meta.f32}"
    LINE "}"
    LINE ""
    LINE "Position(Foo) = {10, 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup_fullpath(world, "Position");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(ecs_id(Position) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    {
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Plecs_type_and_assign_in_plecs_w_2_members() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct(Position) {"
    LINE "  flecs.meta.Member(x) = {flecs.meta.f32}"
    LINE "  flecs.meta.Member(y) = {flecs.meta.f32}"
    LINE "}"
    LINE ""
    LINE "Position(Foo) = {x: 10, y: 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup_fullpath(world, "Position");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(ecs_id(Position) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    {
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Plecs_type_and_assign_in_plecs_w_3_members() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        float x;
        float y;
        float z;
    } Position3D;

    const char *expr =
    HEAD "flecs.meta.Struct(Position) {"
    LINE "  flecs.meta.Member(x) = {flecs.meta.f32}"
    LINE "  flecs.meta.Member(y) = {flecs.meta.f32}"
    LINE "  flecs.meta.Member(z) = {flecs.meta.f32}"
    LINE "}"
    LINE ""
    LINE "Position(Foo) = {x: 10, y: 20, z: 30}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position3D) = ecs_lookup_fullpath(world, "Position");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(ecs_id(Position3D) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position3D));

    {
    const Position3D *ptr = ecs_get(world, foo, Position3D);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    test_int(ptr->z, 30);
    }

    ecs_fini(world);
}

void Plecs_type_and_assign_in_plecs_w_enum() {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    typedef struct {
        Color value;
    } SomeType;

    const char *expr =
    HEAD "flecs.meta.Enum(Color) {"
    LINE "  flecs.meta.Constant(Red)"
    LINE "  flecs.meta.Constant(Green)"
    LINE "  flecs.meta.Constant(Blue)"
    LINE "}"
    LINE ""
    LINE "flecs.meta.Struct(SomeType) {"
    LINE "  flecs.meta.Member(value) = {Color}"
    LINE "}"
    LINE ""
    LINE "SomeType(Foo) = {value: Blue}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(SomeType) = ecs_lookup_fullpath(world, "SomeType");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(ecs_id(SomeType) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, SomeType));

    {
    const SomeType *ptr = ecs_get(world, foo, SomeType);
    test_assert(ptr != NULL);
    test_int(ptr->value, Blue);
    }

    ecs_fini(world);
}

void Plecs_type_and_assign_in_plecs_w_enum_using_meta() {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    typedef struct {
        Color value;
    } SomeType;

    const char *expr =
    HEAD "using flecs.meta"
    LINE "Enum(Color) {"
    LINE "  Constant(Red)"
    LINE "  Constant(Green)"
    LINE "  Constant(Blue)"
    LINE "}"
    LINE ""
    LINE "Struct(SomeType) {"
    LINE "  Member(value) = {Color}"
    LINE "}"
    LINE ""
    LINE "SomeType(Foo) = {value: Blue}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(SomeType) = ecs_lookup_fullpath(world, "SomeType");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    /* Make sure no meta entities were created in the root */
    test_assert(ecs_lookup_fullpath(world, "Enum") == 0);
    test_assert(ecs_lookup_fullpath(world, "Struct") == 0);
    test_assert(ecs_lookup_fullpath(world, "Constant") == 0);
    test_assert(ecs_lookup_fullpath(world, "Red") == 0);
    test_assert(ecs_lookup_fullpath(world, "Green") == 0);
    test_assert(ecs_lookup_fullpath(world, "Blue") == 0);

    test_assert(ecs_id(SomeType) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, SomeType));

    {
    const SomeType *ptr = ecs_get(world, foo, SomeType);
    test_assert(ptr != NULL);
    test_int(ptr->value, Blue);
    }

    ecs_fini(world);
}

void Plecs_type_and_assign_in_plecs_w_enum_primitive_using_meta() {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE "Enum(Color) {"
    LINE "  Constant(Red)"
    LINE "  Constant(Green)"
    LINE "  Constant(Blue)"
    LINE "}"
    LINE ""
    LINE "Color(Foo) = Blue";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Color) = ecs_lookup_fullpath(world, "Color");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    /* Make sure no meta entities were created in the root */
    test_assert(ecs_lookup_fullpath(world, "Enum") == 0);
    test_assert(ecs_lookup_fullpath(world, "Struct") == 0);
    test_assert(ecs_lookup_fullpath(world, "Constant") == 0);
    test_assert(ecs_lookup_fullpath(world, "Red") == 0);
    test_assert(ecs_lookup_fullpath(world, "Green") == 0);
    test_assert(ecs_lookup_fullpath(world, "Blue") == 0);

    test_assert(ecs_id(Color) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Color));

    {
    const Color *ptr = ecs_get(world, foo, Color);
    test_assert(ptr != NULL);
    test_int(*ptr, Blue);
    }

    ecs_fini(world);
}


void Plecs_type_and_assign_in_plecs_w_enum_primitive_and_struct() {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "Struct(Position) {"
    LINE "  Member(x) = {f32}"
    LINE "  Member(y) = {f32}"
    LINE "}"
    LINE ""
    LINE "Enum(Color) {"
    LINE "  Constant(Red)"
    LINE "  Constant(Green)"
    LINE "  Constant(Blue)"
    LINE "}"
    LINE ""
    LINE "Foo = {"
    LINE "  Position = {x: 10, y: 20}"
    LINE "  Color = Green"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup_fullpath(world, "Position");
    ecs_entity_t ecs_id(Color) = ecs_lookup_fullpath(world, "Color");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    /* Make sure no meta entities were created in the root */
    test_assert(ecs_lookup_fullpath(world, "Enum") == 0);
    test_assert(ecs_lookup_fullpath(world, "Struct") == 0);
    test_assert(ecs_lookup_fullpath(world, "Constant") == 0);
    test_assert(ecs_lookup_fullpath(world, "Red") == 0);
    test_assert(ecs_lookup_fullpath(world, "Green") == 0);
    test_assert(ecs_lookup_fullpath(world, "Blue") == 0);

    test_assert(ecs_id(Position) != 0);
    test_assert(ecs_id(Color) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    test_assert(ecs_has(world, foo, Color));

    {
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    }

    {
    const Color *ptr = ecs_get(world, foo, Color);
    test_assert(ptr != NULL);
    test_int(*ptr, Green);
    }

    ecs_fini(world);
}

void Plecs_open_scope_no_parent() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar"
    LINE "}"
    LINE "{"
    LINE "  Zoo"
    LINE "}"
    LINE "Hello";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup_fullpath(world, "Zoo");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(zoo != 0);
    test_assert(hello != 0);

    test_assert(ecs_has_pair(world, bar, EcsChildOf, foo));
    test_assert(!ecs_has_pair(world, zoo, EcsChildOf, EcsWildcard));
    test_assert(!ecs_has_pair(world, hello, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Plecs_create_subject_in_root_scope_w_resolvable_id() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t tag = ecs_lookup_child(world, 0, "Tag");
    test_assert(tag != 0);
    test_assert(tag != EcsTag);

    ecs_fini(world);
}

void Plecs_create_subject_in_scope_w_resolvable_id() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Tag"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t tag = ecs_lookup_fullpath(world, "Foo.Tag");
    test_assert(tag != 0);
    test_assert(tag != EcsTag);

    test_assert(ecs_has_pair(world, tag, EcsChildOf, foo));

    ecs_fini(world);
}

void Plecs_using_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar"
    LINE "}"
    LINE ""
    LINE "using Foo"
    LINE "Bar(Hello)"
    LINE "Foo.Bar(World)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Foo.Bar");
    ecs_entity_t not_bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Hello");
    ecs_entity_t _world = ecs_lookup_fullpath(world, "World");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);
    test_assert(not_bar == 0);

    test_assert(_world != EcsWorld); /* sanity check, verified by other tests */

    test_assert(ecs_has_pair(world, bar, EcsChildOf, foo));
    test_assert(ecs_has_id(world, hello, bar));
    test_assert(ecs_has_id(world, _world, bar));

    ecs_fini(world);
}

void Plecs_using_nested_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {"
    LINE "    Zoo"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "using Foo.Bar"
    LINE "Zoo(Hello)"
    LINE "Foo.Bar.Zoo(World)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup_fullpath(world, "Foo.Bar.Zoo");
    ecs_entity_t not_bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t not_zoo = ecs_lookup_fullpath(world, "Zoo");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Hello");
    ecs_entity_t _world = ecs_lookup_fullpath(world, "World");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(zoo != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);
    test_assert(not_bar == 0);
    test_assert(not_zoo == 0);

    test_assert(_world != EcsWorld); /* sanity check, verified by other tests */

    test_assert(ecs_has_id(world, hello, zoo));
    test_assert(ecs_has_id(world, _world, zoo));

    ecs_fini(world);
}

void Plecs_using_nested_in_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {"
    LINE "    Zoo"
    LINE "  }"
    LINE "}"
    LINE "{"
    LINE "  using Foo.Bar"
    LINE "  Zoo(Hello)"
    LINE "}"
    LINE "Zoo(World)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");
    ecs_entity_t bar = ecs_lookup_fullpath(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup_fullpath(world, "Foo.Bar.Zoo");
    ecs_entity_t not_bar = ecs_lookup_fullpath(world, "Bar");
    ecs_entity_t zoo_root = ecs_lookup_fullpath(world, "Zoo");
    ecs_entity_t hello = ecs_lookup_fullpath(world, "Hello");
    ecs_entity_t _world = ecs_lookup_fullpath(world, "World");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(zoo != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);
    test_assert(not_bar == 0);
    test_assert(zoo_root != 0);

    test_assert(_world != EcsWorld); /* sanity check, verified by other tests */

    test_assert(ecs_has_id(world, hello, zoo));
    test_assert(ecs_has_id(world, _world, zoo_root));

    ecs_fini(world);
}

void Plecs_using_w_entity_ref_in_value_2_members() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  Member(x) = {f32}" // member type is looked up in flecs.meta
    LINE "  Member(y) = {f32}"
    LINE "}"
    LINE ""
    LINE "Position(Foo) = {x: 10, y: 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup_fullpath(world, "Position");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(ecs_id(Position) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    {
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Plecs_using_w_entity_ref_in_value_3_members() {
    ecs_world_t *world = ecs_init();

    typedef struct {
        float x;
        float y;
        float z;
    } Position3D;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  Member(x) = {f32}" // member type is looked up in flecs.meta
    LINE "  Member(y) = {f32}"
    LINE "  Member(z) = {f32}"
    LINE "}"
    LINE ""
    LINE "Position(Foo) = {x: 10, y: 20, z: 30}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position3D) = ecs_lookup_fullpath(world, "Position");
    ecs_entity_t foo = ecs_lookup_fullpath(world, "Foo");

    test_assert(ecs_id(Position3D) != 0);
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position3D));

    {
    const Position3D *ptr = ecs_get(world, foo, Position3D);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);
    test_int(ptr->z, 30);
    }

    ecs_fini(world);
}

void Plecs_assignment_to_non_component() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Position(Foo) = {x: 10, y: 20}";

    ecs_tracing_enable(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_struct_w_member_w_assignment_to_nothing() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct(Position) {"
    LINE "  flecs.meta.Member(x) = "
    LINE "}";

    ecs_tracing_enable(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_struct_w_member_w_assignment_to_empty_scope() {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct(Position) {"
    LINE "  flecs.meta.Member(x) = {"
    LINE "}";

    ecs_tracing_enable(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}
