#include <script.h>

void Eval_null(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, NULL) == 0);

    ecs_fini(world);
}

void Eval_empty(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "") == 0);

    ecs_fini(world);
}

void Eval_space(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, " ") == 0);

    ecs_fini(world);
}

void Eval_space_newline(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, " \n \n") == 0);

    ecs_fini(world);
}

void Eval_two_empty_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "\n\n") == 0);

    ecs_fini(world);
}

void Eval_three_empty_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "\n\n\n") == 0);

    ecs_fini(world);
}

void Eval_newline_trailing_space(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "\n ") == 0);

    ecs_fini(world);
}

void Eval_newline_trailing_spaces(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "\n   ") == 0);

    ecs_fini(world);
}

void Eval_multiple_trailing_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}\n\n\n") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");
    test_int(ecs_get_type(world, foo)->count, 1);

    ecs_fini(world);
}

void Eval_entity(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");
    test_int(ecs_get_type(world, foo)->count, 1);

    ecs_fini(world);
}

void Eval_entity_w_core_name(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "World{}") == 0);

    ecs_entity_t foo = ecs_lookup_child(world, 0, "World");
    test_assert(foo != 0);
    test_assert(foo != EcsWorld);
    test_str(ecs_get_name(world, foo), "World");
    test_int(ecs_get_type(world, foo)->count, 1);

    ecs_fini(world);
}

void Eval_2_entities(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}\nBar{}\n") == 0);

    ecs_entity_t e = ecs_lookup(world, "Foo");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Eval_line_comment(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "// Foo{}\n") == 0);

    test_assert(ecs_lookup(world, "Foo") == 0);

    ecs_fini(world);
}

void Eval_line_comment_before_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "// Hello\nFoo {}\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_line_comment_after_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}\n// Hello\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_line_comment_between_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}\n// Hello\nBar{}\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Eval_multiple_line_comment(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "// Hello\n// World\nFoo{}") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_multiple_line_comment_w_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "// Hello\n\n// World\n\nFoo{}") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_line_comment_after_stmt_same_line(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{} // Hello\nBar{}\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Eval_line_comment_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent // Some Comment()"
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    test_assert(ecs_lookup(world, "Some") == 0);
    test_assert(ecs_lookup(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_line_comment_after_newline_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "// Some Comment()"
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    test_assert(ecs_lookup(world, "Some") == 0);
    test_assert(ecs_lookup(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_line_comment_after_newline_before_newline_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "// Some Comment()"
    LINE ""
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    test_assert(ecs_lookup(world, "Some") == 0);
    test_assert(ecs_lookup(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_multi_line_comment(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "/* Foo{} */") == 0);

    test_assert(ecs_lookup(world, "Foo") == 0);

    ecs_fini(world);
}

void Eval_multi_line_comment_before_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "/* Hello */\nFoo {}") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_multi_line_comment_after_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}\n/* Hello */") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_multi_line_comment_between_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{}\n /* Hello */ Bar{}\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Eval_multiple_multi_line_comment(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "/* Hello *//* World*/ Foo{}") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_multiple_multi_line_comment_w_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "/* Hello */\n/* World */ Foo{}") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Eval_multi_line_comment_after_stmt_same_line(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "Foo{} /* Hello */\nBar{}\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Eval_multi_line_comment_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent /* Some Comment() */"
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    test_assert(ecs_lookup(world, "Some") == 0);
    test_assert(ecs_lookup(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_multi_line_comment_after_newline_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "/* Some Comment() */"
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    test_assert(ecs_lookup(world, "Some") == 0);
    test_assert(ecs_lookup(world, "Comment") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_multi_line_comment_multiple_lines(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "/* Some{}"
    LINE " * Multi Line{}"
    LINE " * Comment{}"
    LINE " */"
    LINE ""
    LINE "Bar {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    test_assert(ecs_lookup(world, "Some") == 0);
    test_assert(ecs_lookup(world, "Multi") == 0);
    test_assert(ecs_lookup(world, "Line") == 0);
    test_assert(ecs_lookup(world, "Comment") == 0);

    ecs_fini(world);
}

void Eval_hierarchy_1_child(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_hierarchy_2_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " ChildA{}"
    LINE " ChildB{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "ChildA") == 0);
    test_assert(ecs_lookup(world, "ChildB") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");

    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_b, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_hierarchy_1_child_same_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { Child{} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_hierarchy_2_children_same_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { ChildA, ChildB, }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "ChildA") == 0);
    test_assert(ecs_lookup(world, "ChildB") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");

    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_b, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_hierarchy_2_children_same_line_no_trailing_comma(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { ChildA, ChildB }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "ChildA") == 0);
    test_assert(ecs_lookup(world, "ChildB") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");

    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert(ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, child_b, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_entity_after_hierarchy(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(foo != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert(!ecs_has_pair(world, foo, EcsChildOf, parent));
    test_assert(!ecs_has_pair(world, foo, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Eval_newline_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_newline_w_whitespace_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "  {"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_2_newline_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE ""
    LINE "{"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_2_newline_w_whitespace_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "  "
    LINE " {"
    LINE " Child{}"
    LINE "}"
    LINE "Foo{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Eval_hierarchy_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child {"
    LINE "  GrandChild {}"
    LINE " }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);
    test_assert(ecs_lookup(world, "GrandChild") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");
    ecs_entity_t grandchild = ecs_lookup(world, "Parent.Child.GrandChild");

    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(grandchild != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, grandchild, EcsChildOf, child));

    ecs_fini(world);
}

void Eval_hierarchy_2_levels_2_subtrees(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " ChildA {"
    LINE "  GrandChild {}"
    LINE " }"
    LINE " ChildB {"
    LINE "  GrandChild {}"
    LINE " }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "ChildA") == 0);
    test_assert(ecs_lookup(world, "ChildB") == 0);
    test_assert(ecs_lookup(world, "GrandChild") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");
    ecs_entity_t grandchild_a = ecs_lookup(world, "Parent.ChildA.GrandChild");
    ecs_entity_t grandchild_b = ecs_lookup(world, "Parent.ChildB.GrandChild");

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

void Eval_create_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);

    ecs_set_scope(world, Root);

    const char *expr =
    HEAD "Parent { Child{} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_get_scope(world) == Root);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_set_scope(world, 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));
    test_assert(!ecs_has_pair(world, parent, EcsChildOf, Root));

    ecs_fini(world);
}

void Eval_hierarchy_w_pred_subj(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "Foo Bar {"
    LINE " Child{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Bar.Child");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);

    test_assert(ecs_has_id(world, bar, foo));
    test_assert(ecs_has_pair(world, child, EcsChildOf, bar));

    ecs_fini(world);
}

void Eval_hierarchy_custom_relation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE " Child{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Child");

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

void Eval_hierarchy_custom_relation_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE "  (Rel, ObjA) {"
    LINE "    Child{}"
    LINE "    ChildA{}"
    LINE "  }"
    LINE "  (Rel, ObjB) {"
    LINE "    Child{}"
    LINE "    ChildB{}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup(world, "ObjB");
    ecs_entity_t child = ecs_lookup(world, "Child");
    ecs_entity_t child_a = ecs_lookup(world, "ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "ChildB");

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

void Eval_entity_after_hierarchy_custom_relation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE " Child{}"
    LINE "}"
    LINE "Hello{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Child");
    ecs_entity_t hello = ecs_lookup(world, "Hello");

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

void Eval_entity_after_hierarchy_custom_relation_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE "  (Rel, ObjA) {"
    LINE "    Child{}"
    LINE "    ChildA{}"
    LINE "  }"
    LINE "  TestA {}"
    LINE "  (Rel, ObjB) {"
    LINE "    Child{}"
    LINE "    ChildB{}"
    LINE "  }"
    LINE "}"
    LINE "TestB{}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup(world, "ObjB");
    ecs_entity_t child = ecs_lookup(world, "Child");
    ecs_entity_t child_a = ecs_lookup(world, "ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "ChildB");

    ecs_entity_t test_a = ecs_lookup(world, "TestA");
    ecs_entity_t test_b = ecs_lookup(world, "TestB");

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

void Eval_hierarchy_custom_relation_apply_to_object(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Rel, ObjA) {"
    LINE " (Rel, ObjB) {"
    LINE "   ObjC{}"
    LINE " }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup(world, "ObjB");
    ecs_entity_t obj_c = ecs_lookup(world, "ObjC");

    test_assert(rel != 0);
    test_assert(obj_a != 0);
    test_assert(obj_b != 0);
    test_assert(obj_c != 0);

    test_assert(ecs_has_pair(world, obj_b, rel, obj_a));
    test_assert(ecs_has_pair(world, obj_c, rel, obj_b));
    test_assert(!ecs_has_pair(world, obj_c, rel, obj_a));

    ecs_fini(world);
}

void Eval_hierarchy_custom_relation_apply_to_object_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Rel, ObjA) {"
    LINE " (Rel, ObjB) {"
    LINE "   (Rel, ObjC) {"
    LINE "     ObjD{}"
    LINE "   }"
    LINE " }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t obj_a = ecs_lookup(world, "ObjA");
    ecs_entity_t obj_b = ecs_lookup(world, "ObjB");
    ecs_entity_t obj_c = ecs_lookup(world, "ObjC");
    ecs_entity_t obj_d = ecs_lookup(world, "ObjD");

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

void Eval_with_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    HEAD "with Tag {"
    LINE "  Foo{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Eval_with_tag_2_entities(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    HEAD "with Tag {"
    LINE "  Foo{}"
    LINE "  Bar{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));
    test_assert(ecs_has_id(world, bar, tag));

    ecs_fini(world);
}

void Eval_with_tag_same_line(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    HEAD "with Tag { Foo{} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Eval_with_tag_2_entities_same_line(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    HEAD "with Tag { Foo, Bar, }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));
    test_assert(ecs_has_id(world, bar, tag));

    ecs_fini(world);
}

void Eval_with_tag_2_entities_same_line_no_trailing_comma(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    HEAD "with Tag { Foo, Bar }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));
    test_assert(ecs_has_id(world, bar, tag));

    ecs_fini(world);
}

void Eval_with_tag_2_levels(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const char *expr =
    HEAD "with TagA {"
    HEAD " with TagB {"
    LINE "  Foo {}"
    LINE " }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag_a = ecs_lookup(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup(world, "TagB");

    test_assert(foo != 0);
    test_assert(tag_a != 0);
    test_assert(tag_b != 0);

    test_assert(ecs_has_id(world, foo, tag_a));
    test_assert(ecs_has_id(world, foo, tag_b));

    ecs_fini(world);
}

void Eval_with_tag_2_levels_2_subtrees(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    const char *expr =
    HEAD "with TagA {"
    HEAD " with TagB {"
    LINE "  Foo {}"
    LINE "  BarA {}"
    LINE " }"
    HEAD " with TagC {"
    LINE "  Foo {}"
    LINE "  BarB {}"
    LINE " }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag_a = ecs_lookup(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup(world, "TagB");
    ecs_entity_t tag_c = ecs_lookup(world, "TagC");
    ecs_entity_t bar_a = ecs_lookup(world, "BarA");
    ecs_entity_t bar_b = ecs_lookup(world, "BarB");

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

void Eval_with_n_tags(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const char *expr =
    HEAD "with TagA, TagB {"
    LINE " Foo {}"
    LINE " Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t tag_a = ecs_lookup(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup(world, "TagB");

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

void Eval_with_n_tags_2_levels(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);
    ECS_TAG(world, TagF);
    ECS_TAG(world, TagG);
    ECS_TAG(world, TagH);

    const char *expr =
    HEAD "with TagA, TagB {"
    LINE " with TagC, TagD, TagE {"
    LINE "  Foo {}"
    LINE " }"
    LINE " HelloA {}"
    LINE " with TagF, TagG, TagH {"
    LINE "  Bar {}"
    LINE " }"
    LINE " HelloB {}"
    LINE "}"
    LINE "HelloC {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t tag_a = ecs_lookup(world, "TagA");
    ecs_entity_t tag_b = ecs_lookup(world, "TagB");
    ecs_entity_t tag_c = ecs_lookup(world, "TagC");
    ecs_entity_t tag_d = ecs_lookup(world, "TagD");
    ecs_entity_t tag_e = ecs_lookup(world, "TagE");
    ecs_entity_t tag_f = ecs_lookup(world, "TagF");
    ecs_entity_t tag_g = ecs_lookup(world, "TagG");
    ecs_entity_t tag_h = ecs_lookup(world, "TagH");

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    ecs_entity_t hello_a = ecs_lookup(world, "HelloA");
    ecs_entity_t hello_b = ecs_lookup(world, "HelloB");
    ecs_entity_t hello_c = ecs_lookup(world, "HelloC");

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

void Eval_with_after_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, E2);
    ECS_TAG(world, E3);

    const char *expr =
    HEAD "E1 { }"
    HEAD "with E2 {"
    LINE "  E3 _ { }"
    LINE "  E4 { }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup(world, "E1");
    ecs_entity_t e2 = ecs_lookup(world, "E2");
    ecs_entity_t e3 = ecs_lookup(world, "E3");
    ecs_entity_t e4 = ecs_lookup(world, "E4");

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

void Eval_with_after_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, E1);
    ECS_TAG(world, E2);
    ECS_TAG(world, E3);

    const char *expr =
    HEAD "with E1 { }"
    HEAD "with E2 {"
    LINE "  E3 _ { }"
    LINE "  E4 { }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup(world, "E1");
    ecs_entity_t e2 = ecs_lookup(world, "E2");
    ecs_entity_t e3 = ecs_lookup(world, "E3");
    ecs_entity_t e4 = ecs_lookup(world, "E4");

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

void Eval_scope_inside_with_inside_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Planet);
    
    const char *expr =
    HEAD "Sun {"
    LINE "  with Planet {"
    LINE "    Earth {"
    LINE "      Moon {}"
    LINE "    }"
    LINE "    Mars {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t sun = ecs_lookup(world, "Sun");
    ecs_entity_t planet = ecs_lookup(world, "Planet");
    ecs_entity_t earth = ecs_lookup(world, "Sun.Earth");
    ecs_entity_t moon = ecs_lookup(world, "Sun.Earth.Moon");
    ecs_entity_t mars = ecs_lookup(world, "Sun.Mars");

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

void Eval_with_tag_core_name(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "with World {"
    LINE "  Hello {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "Hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsWorld));

    ecs_fini(world);
}

void Eval_with_inside_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Continent);
    
    const char *expr =
    HEAD "Earth {"
    LINE "  with Continent {"
    LINE "    Europe {}"
    LINE "  }"
    LINE "  Europe {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup(world, "Earth");
    ecs_entity_t continent = ecs_lookup(world, "Continent");
    ecs_entity_t europe = ecs_lookup(world, "Earth.Europe");

    test_assert(earth != 0);
    test_assert(continent != 0);
    test_assert(europe != 0);

    test_assert( ecs_has_pair(world, europe, EcsChildOf, earth));
    test_assert( !ecs_has_pair(world, continent, EcsChildOf, earth));
    test_assert( !ecs_has_id(world, europe, earth));
    test_assert( !ecs_has_id(world, continent, earth));

    ecs_fini(world);
}

void Eval_inherit(void) {    
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);
    
    const char *expr =
    HEAD "Foo : Bar";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_fini(world);
}

void Eval_inherit_newline(void) {    
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);
    
    const char *expr =
    HEAD "Foo : Bar\n";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_fini(world);
}

void Eval_inherit_w_colon(void) {    
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);
    
    const char *expr =
    HEAD "Foo : Bar {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_fini(world);
}

void Eval_inherit_w_colon_w_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);
    
    const char *expr =
    HEAD "Foo : Bar { Child{} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Foo.Child");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));
    test_assert(ecs_has_pair(world, child, EcsChildOf, foo));

    ecs_fini(world);
}

void Eval_assign_component_w_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo = Position: {10, 20}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_assign_tag_in_assign_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);
    
    const char *expr =
    HEAD "Foo {"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(ecs_has_id(world, foo, bar));

    ecs_fini(world);
}

void Eval_assign_tag_in_assign_scope_same_line(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);
    
    const char *expr =
    HEAD "Foo { Bar }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(ecs_has_id(world, foo, bar));

    ecs_fini(world);
}

void Eval_assign_tag_in_assign_scope_core_name(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Hello {"
    LINE "  World"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "Hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsWorld));

    ecs_fini(world);
}

void Eval_assign_component_value_in_assign_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  Position: {10, 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_assign_2_component_values_in_assign_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE " Position: {10, 20}"
    LINE " Velocity:{1, 2}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_type_and_assign_in_plecs(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.struct Position {"
    LINE "  x { flecs.meta.member: {flecs.meta.f32} }"
    LINE "  y { flecs.meta.member: {flecs.meta.f32} }"
    LINE "}"
    LINE ""
    LINE "Foo {"
    LINE "  Position: {10, 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_type_and_assign_in_plecs_w_2_members(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.struct Position {"
    LINE "  x { flecs.meta.member: {flecs.meta.f32} }"
    LINE "  y { flecs.meta.member: {flecs.meta.f32} }"
    LINE "}"
    LINE ""
    LINE "Foo {"
    LINE "  Position: {x: 10, y: 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_type_and_assign_in_plecs_w_3_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        float x;
        float y;
        float z;
    } Position3D;

    const char *expr =
    HEAD "flecs.meta.struct Position {"
    LINE "  x { flecs.meta.member: {flecs.meta.f32} }"
    LINE "  y { flecs.meta.member: {flecs.meta.f32} }"
    LINE "  z { flecs.meta.member: {flecs.meta.f32} }"
    LINE "}"
    LINE ""
    LINE "Foo {"
    LINE "  Position: {x: 10, y: 20, z: 30}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position3D) = ecs_lookup(world, "Position");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_type_and_assign_in_plecs_w_enum(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    typedef struct {
        Color value;
    } SomeType;

    const char *expr =
    HEAD "flecs.meta.enum Color {"
    LINE "  flecs.meta.constant Red"
    LINE "  flecs.meta.constant Green"
    LINE "  flecs.meta.constant Blue"
    LINE "}"
    LINE ""
    LINE "flecs.meta.struct SomeType {"
    LINE "  value { flecs.meta.member: {Color} }"
    LINE "}"
    LINE ""
    LINE "Foo {"
    LINE "  SomeType: {value: Blue}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(SomeType) = ecs_lookup(world, "SomeType");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_type_and_assign_in_plecs_w_enum_using_meta(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    typedef struct {
        Color value;
    } SomeType;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "enum Color {"
    LINE "  constant Red"
    LINE "  constant Green"
    LINE "  constant Blue"
    LINE "}"
    LINE
    LINE "struct SomeType {"
    LINE "  value { member: {Color} }"
    LINE "}"
    LINE
    LINE "Foo { SomeType: {value: Blue} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(SomeType) = ecs_lookup(world, "SomeType");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    /* Make sure no meta entities were created in the root */
    test_assert(ecs_lookup(world, "Enum") == 0);
    test_assert(ecs_lookup(world, "Struct") == 0);
    test_assert(ecs_lookup(world, "Constant") == 0);
    test_assert(ecs_lookup(world, "Red") == 0);
    test_assert(ecs_lookup(world, "Green") == 0);
    test_assert(ecs_lookup(world, "Blue") == 0);

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

void Eval_type_and_assign_in_plecs_w_enum_primitive_using_meta(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE "enum Color {"
    LINE "  constant Red"
    LINE "  constant Green"
    LINE "  constant Blue"
    LINE "}"
    LINE ""
    LINE "Foo { Color: {Blue} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Color) = ecs_lookup(world, "Color");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    /* Make sure no meta entities were created in the root */
    test_assert(ecs_lookup(world, "Enum") == 0);
    test_assert(ecs_lookup(world, "Struct") == 0);
    test_assert(ecs_lookup(world, "Constant") == 0);
    test_assert(ecs_lookup(world, "Red") == 0);
    test_assert(ecs_lookup(world, "Green") == 0);
    test_assert(ecs_lookup(world, "Blue") == 0);

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

void Eval_type_and_assign_in_plecs_w_enum_primitive_and_struct(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE 
    LINE "struct Position {"
    LINE "  x { member: {f32} }"
    LINE "  y { member: {f32} }"
    LINE "}"
    LINE 
    LINE "enum Color {"
    LINE "  constant Red"
    LINE "  constant Green"
    LINE "  constant Blue"
    LINE "}"
    LINE 
    LINE "Foo {"
    LINE "  Position: {x: 10, y: 20}"
    LINE "  Color: {Green}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    ecs_entity_t ecs_id(Color) = ecs_lookup(world, "Color");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    /* Make sure no meta entities were created in the root */
    test_assert(ecs_lookup(world, "Enum") == 0);
    test_assert(ecs_lookup(world, "Struct") == 0);
    test_assert(ecs_lookup(world, "Constant") == 0);
    test_assert(ecs_lookup(world, "Red") == 0);
    test_assert(ecs_lookup(world, "Green") == 0);
    test_assert(ecs_lookup(world, "Blue") == 0);

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

void Eval_type_and_assign_in_plecs_nested_member(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        struct {
            float x;
            float y;
        } start;

        struct {
            float x;
            float y;
        } stop;
    } Line;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Line {"
    LINE "  member start {"
    LINE "    x { member: {f32} }"
    LINE "    y { member: {f32} }"
    LINE "  }"
    LINE "  member stop {"
    LINE "    x { member: {f32} }"
    LINE "    y { member: {f32} }"
    LINE "  }"
    LINE "}"
    LINE
    LINE "l { Line: {start: {x: 10, y: 20}, stop: {x: 30, y: 40}} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Line) = ecs_lookup(world, "Line");
    ecs_entity_t l = ecs_lookup(world, "l");

    test_assert(ecs_id(Line) != 0);
    test_assert(l != 0);

    test_assert( ecs_has(world, ecs_id(Line), EcsComponent));
    test_assert( ecs_has(world, ecs_id(Line), EcsStruct));
    test_assert( ecs_has(world, l, Line));

    const Line *ptr = ecs_get(world, l, Line);
    test_assert(ptr != NULL);
    test_int(ptr->start.x, 10);
    test_int(ptr->start.y, 20);
    test_int(ptr->stop.x, 30);
    test_int(ptr->stop.y, 40);
    
    
    ecs_fini(world);
}

void Eval_dot_assign_nested_member(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        struct {
            float x;
            float y;
        } start;

        struct {
            float x;
            float y;
        } stop;
    } Line;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Line {"
    LINE "  member start {"
    LINE "    x { member: {f32} }"
    LINE "    y { member: {f32} }"
    LINE "  }"
    LINE "  member stop {"
    LINE "    x { member: {f32} }"
    LINE "    y { member: {f32} }"
    LINE "  }"
    LINE "}"
    LINE
    LINE "l { Line: {start.x: 10, start.y: 20, stop.x: 30, stop.y: 40} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Line) = ecs_lookup(world, "Line");
    ecs_entity_t l = ecs_lookup(world, "l");

    test_assert(ecs_id(Line) != 0);
    test_assert(l != 0);

    test_assert( ecs_has(world, ecs_id(Line), EcsComponent));
    test_assert( ecs_has(world, ecs_id(Line), EcsStruct));
    test_assert( ecs_has(world, l, Line));

    const Line *ptr = ecs_get(world, l, Line);
    test_assert(ptr != NULL);
    test_int(ptr->start.x, 10);
    test_int(ptr->start.y, 20);
    test_int(ptr->stop.x, 30);
    test_int(ptr->stop.y, 40);
    
    ecs_fini(world);
}

void Eval_dot_assign_binary_expr(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        struct {
            float x;
            float y;
        } start;

        struct {
            float x;
            float y;
        } stop;
    } Line;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Line {"
    LINE "  member start {"
    LINE "    x { member: {f32} }"
    LINE "    y { member: {f32} }"
    LINE "  }"
    LINE "  member stop {"
    LINE "    x { member: {f32} }"
    LINE "    y { member: {f32} }"
    LINE "  }"
    LINE "}"
    LINE
    LINE "l { Line: {start.x: 2 + 8, start.y: 5 + 15, stop.x: 10 + 20, stop.y: 15 + 25} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Line) = ecs_lookup(world, "Line");
    ecs_entity_t l = ecs_lookup(world, "l");

    test_assert(ecs_id(Line) != 0);
    test_assert(l != 0);

    test_assert( ecs_has(world, ecs_id(Line), EcsComponent));
    test_assert( ecs_has(world, ecs_id(Line), EcsStruct));
    test_assert( ecs_has(world, l, Line));

    const Line *ptr = ecs_get(world, l, Line);
    test_assert(ptr != NULL);
    test_int(ptr->start.x, 10);
    test_int(ptr->start.y, 20);
    test_int(ptr->stop.x, 30);
    test_int(ptr->stop.y, 40);
    
    
    ecs_fini(world);
}

void Eval_open_scope_no_parent(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {}"
    LINE "}"
    LINE "{"
    LINE "  Zoo {}"
    LINE "}"
    LINE "Hello {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup(world, "Zoo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(zoo != 0);
    test_assert(hello != 0);

    test_assert(ecs_has_pair(world, bar, EcsChildOf, foo));
    test_assert(!ecs_has_pair(world, zoo, EcsChildOf, EcsWildcard));
    test_assert(!ecs_has_pair(world, hello, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Eval_create_subject_in_root_scope_w_resolvable_id(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t tag = ecs_lookup_child(world, 0, "Tag");
    test_assert(tag != 0);
    test_assert(tag != EcsPairIsTag);

    ecs_fini(world);
}

void Eval_create_subject_in_scope_w_resolvable_id(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Tag {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t tag = ecs_lookup(world, "Foo.Tag");
    test_assert(tag != 0);
    test_assert(tag != EcsPairIsTag);

    test_assert(ecs_has_pair(world, tag, EcsChildOf, foo));

    ecs_fini(world);
}

void Eval_create_subject_in_scope_w_resolvable_id_using(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Hello);

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {}"
    LINE "}"
    LINE
    LINE "using Foo"
    LINE
    LINE "Hello Bar {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t root_bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(root_bar != 0);
    test_assert(hello != 0);

    test_assert(ecs_has_pair(world, bar, EcsChildOf, foo));

    test_assert(ecs_has_id(world, root_bar, hello));
    test_assert(!ecs_has_id(world, bar, hello));

    ecs_fini(world);
}

void Eval_using_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {}"
    LINE "}"
    LINE ""
    LINE "using Foo"
    LINE "Bar Hello {}"
    LINE "Foo.Bar World {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t not_bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup_child(world, 0, "World");

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

void Eval_using_nested_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {"
    LINE "    Zoo {}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "using Foo.Bar"
    LINE "Zoo Hello {}"
    LINE "Foo.Bar.Zoo World {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup(world, "Foo.Bar.Zoo");
    ecs_entity_t not_bar = ecs_lookup(world, "Bar");
    ecs_entity_t not_zoo = ecs_lookup(world, "Zoo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup_child(world, 0, "World");

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

void Eval_using_nested_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Zoo);

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar {"
    LINE "    Zoo {}"
    LINE "  }"
    LINE "}"
    LINE "{"
    LINE "  using Foo.Bar"
    LINE "  Zoo Hello {}"
    LINE "}"
    LINE "Zoo World {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup(world, "Foo.Bar.Zoo");
    ecs_entity_t not_bar = ecs_lookup(world, "Bar");
    ecs_entity_t zoo_root = ecs_lookup(world, "Zoo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup_child(world, 0, "World");

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

void Eval_using_with_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "Scope {"
    LINE ""
    LINE "Foo {}"
    LINE ""
    LINE "struct Bar {"
    LINE "  x { member: {f32} }"
    LINE "}"
    LINE ""
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t scope = ecs_lookup(world, "Scope");
    ecs_entity_t foo = ecs_lookup(world, "Scope.Foo");
    ecs_entity_t bar = ecs_lookup(world, "Scope.Bar");
    ecs_entity_t x = ecs_lookup(world, "Scope.Bar.x");

    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "Bar") == 0);
    test_assert(ecs_lookup(world, "x") == 0);
    test_assert(ecs_lookup(world, "Struct") == 0);
    test_assert(ecs_lookup(world, "Member") == 0);

    test_assert(scope != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(x != 0);

    test_assert(ecs_has_pair(world, foo, EcsChildOf, scope));
    test_assert(ecs_has_pair(world, bar, EcsChildOf, scope));
    test_assert(ecs_has_pair(world, x, EcsChildOf, bar));

    ecs_fini(world);
}

void Eval_using_w_entity_ref_in_value_2_members(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x { member: {f32} }" // member type is looked up in flecs.meta
    LINE "  y { member: {f32} }"
    LINE "}"
    LINE ""
    LINE "Foo { Position: {x: 10, y: 20} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_using_w_entity_ref_in_value_3_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        float x;
        float y;
        float z;
    } Position3D;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x { member: {f32} }" // member type is looked up in flecs.meta
    LINE "  y { member: {f32} }"
    LINE "  z { member: {f32} }"
    LINE "}"
    LINE
    LINE "Foo { Position: {x: 10, y: 20, z: 30} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t ecs_id(Position3D) = ecs_lookup(world, "Position");
    ecs_entity_t foo = ecs_lookup(world, "Foo");

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

void Eval_script_w_only_using(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run(world, NULL, "using flecs") == 0);

    ecs_fini(world);
}

void Eval_2_using_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using Foo"
    LINE "using Bar"
    LINE
    LINE "Foo {"
    LINE "  Hello {}"
    LINE "}"
    LINE
    LINE "Bar {"
    LINE "  TheWorld {}"
    LINE "}"
    LINE
    LINE "Hello E1 {}"
    LINE "TheWorld E2 {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Foo.Hello");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t _world = ecs_lookup(world, "Bar.TheWorld");

    ecs_entity_t e1 = ecs_lookup(world, "E1");
    ecs_entity_t e2 = ecs_lookup(world, "E2");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup(world, "TheWorld") == 0);

    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(ecs_has_pair(world, _world, EcsChildOf, bar));
    test_assert(ecs_has_id(world, e1, hello));
    test_assert(ecs_has_id(world, e2, _world));

    ecs_fini(world);
}

void Eval_2_using_in_different_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TheWorld);
    ECS_TAG(world, Hello);

    const char *expr =
    LINE "Foo {"
    LINE "  Hello {}"
    LINE "}"
    LINE
    LINE "Bar {"
    LINE "  TheWorld {}"
    LINE "}"
    LINE
    LINE "E1 {"
    LINE "  using Foo"
    LINE
    LINE "  Hello Child {}"
    LINE "  TheWorld Child {}"
    LINE "}"
    LINE
    LINE "E2 {"
    LINE "  using Bar"
    LINE
    LINE "  Hello Child {}"
    LINE "  TheWorld Child {}"
    LINE "}"
    LINE
    LINE "Hello RootChild {}"
    LINE "TheWorld RootChild {}"
    LINE;

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Foo.Hello");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t _world = ecs_lookup(world, "Bar.TheWorld");

    ecs_entity_t e1 = ecs_lookup(world, "E1");
    ecs_entity_t e1_child = ecs_lookup(world, "E1.Child");
    ecs_entity_t e2 = ecs_lookup(world, "E2");
    ecs_entity_t e2_child = ecs_lookup(world, "E2.Child");

    ecs_entity_t root_hello = ecs_lookup(world, "Hello");
    ecs_entity_t root_world = ecs_lookup(world, "TheWorld");

    ecs_entity_t root_child = ecs_lookup(world, "RootChild");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    test_assert(_world != 0);
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e1_child != 0);
    test_assert(e2_child != 0);
    test_assert(root_hello != 0);
    test_assert(root_world != 0);
    test_assert(root_world != 0);

    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(ecs_has_pair(world, _world, EcsChildOf, bar));

    test_assert(ecs_has_pair(world, e1_child, EcsChildOf, e1));
    test_assert(ecs_has_pair(world, e2_child, EcsChildOf, e2));

    test_assert(ecs_has_id(world, e1_child, hello));
    test_assert(ecs_has_id(world, e1_child, root_world));

    test_assert(ecs_has_id(world, e2_child, root_hello));
    test_assert(ecs_has_id(world, e2_child, _world));

    test_assert(ecs_has_id(world, root_child, root_hello));
    test_assert(ecs_has_id(world, root_child, root_world));

    ecs_fini(world);
}

void Eval_scope_after_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Position Foo(10, 20) {"
    LINE "  Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");

    test_assert(ecs_has_pair(world, bar, EcsChildOf, foo));

    {
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Eval_assign_after_inherit(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo : Position {}"
    LINE "Bar { Position: {10, 20} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert( ecs_has_pair(world, foo, EcsIsA, ecs_id(Position)));
    test_assert( !ecs_has(world, foo, Position));

    test_assert( !ecs_has_pair(world, bar, EcsIsA, ecs_id(Position)));
    test_assert( ecs_has(world, bar, Position));

    const Position *ptr = ecs_get(world, bar, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_multiple_tags_single_line(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Hello);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "Foo { Hello; Bar }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t hello = ecs_lookup(world, "Hello");
    test_assert(hello != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    test_assert( ecs_has_id(world, foo, hello));
    test_assert( ecs_has_id(world, foo, bar));

    ecs_fini(world);
}

void Eval_multiple_pairs_single_line(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Hello);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "Foo { (Rel, Hello); (Rel, Bar) }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t rel = ecs_lookup(world, "Rel");
    test_assert(rel != 0);

    ecs_entity_t hello = ecs_lookup(world, "Hello");
    test_assert(hello != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    test_assert( ecs_has_pair(world, foo, rel, hello));
    test_assert( ecs_has_pair(world, foo, rel, bar));

    ecs_fini(world);
}

void Eval_multiple_assignments_single_line(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo { Position: {10, 20}; Velocity: {1, 2} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

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

void Eval_multiple_vars_single_line(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const pos = Position: {10, 20}"
    LINE "const vel = Velocity: {1, 2}"
    LINE "Foo { $pos; $vel }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

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

void Eval_2_stmts_in_scope_w_no_parent(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "{"
    LINE "Bar { }"
    LINE "Foo { }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert( !ecs_has_id(world, foo, bar));
    test_assert( !ecs_has_pair(world, foo, EcsChildOf, bar));

    test_assert( !ecs_has_id(world, bar, foo));
    test_assert( !ecs_has_pair(world, bar, EcsChildOf, foo));

    ecs_fini(world);
}

void Eval_empty_scope_after_using(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE "{"
    LINE "  Foo {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert( foo != 0);
    test_assert( !ecs_has_pair(world, foo, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Eval_assign_tag_to_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar"
    LINE "  Child {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Foo.Child");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);

    test_assert( ecs_has_id(world, foo, bar));
    test_assert( !ecs_has_id(world, child, bar));
    test_assert( !ecs_has_pair(world, bar, EcsChildOf, foo));
    test_assert( ecs_has_pair(world, child, EcsChildOf, foo));

    ecs_fini(world);
}

void Eval_assign_component_to_parent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  Position: {10, 20}"
    LINE "  Child {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t child = ecs_lookup(world, "Foo.Child");

    test_assert(foo != 0);
    test_assert(child != 0);

    test_assert( ecs_has(world, foo, Position));
    test_assert( !ecs_has(world, child, Position));
    test_assert( !ecs_has_pair(world, ecs_id(Position), EcsChildOf, foo));
    test_assert( ecs_has_pair(world, child, EcsChildOf, foo));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_assign_to_parent_pair_w_new_entities_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    const char *expr =
    HEAD "Foo {"
    LINE "  (Rel, Obj)"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t obj = ecs_lookup(world, "Obj");

    test_assert(foo != 0);
    test_assert(rel != 0);
    test_assert(obj != 0);

    test_assert( ecs_has_pair(world, foo, rel, obj));
    test_assert( !ecs_has_pair(world, rel, EcsChildOf, EcsWildcard));
    test_assert( !ecs_has_pair(world, obj, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Eval_assign_to_parent_pair_w_existing_entities_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel, Obj"
    LINE "Foo {"
    LINE "  (Rel, Obj)"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t obj = ecs_lookup(world, "Obj");

    test_assert(foo != 0);
    test_assert(rel != 0);
    test_assert(obj != 0);

    test_assert( ecs_has_pair(world, foo, rel, obj));
    test_assert( !ecs_has_pair(world, rel, EcsChildOf, EcsWildcard));
    test_assert( !ecs_has_pair(world, obj, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Eval_default_child_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Bar {}"
    LINE "DefaultChildComponent Foo(Bar)"
    LINE "Foo Parent {"
    LINE "  ChildA,"
    LINE "  ChildB"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    
    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    const EcsDefaultChildComponent *dcc = 
        ecs_get(world, foo, EcsDefaultChildComponent);
    test_assert(dcc != NULL);
    test_uint(dcc->component, bar);

    test_assert( ecs_has_id(world, parent, foo));
    test_assert( ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert( ecs_has_pair(world, child_b, EcsChildOf, parent));
    
    test_assert(ecs_has_id(world, child_a, bar));
    test_assert(ecs_has_id(world, child_b, bar));

    ecs_fini(world);
}

void Eval_default_child_component_w_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "DefaultChildComponent Foo(Position)"
    LINE "Foo Parent {"
    LINE "  ChildA = 10, 20"
    LINE "  ChildB = 10, 20"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    
    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");

    test_assert(foo != 0);
    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);
    
    const EcsDefaultChildComponent *dcc = 
        ecs_get(world, foo, EcsDefaultChildComponent);
    test_assert(dcc != NULL);
    test_uint(dcc->component, ecs_id(Position));

    test_assert( ecs_has_id(world, parent, foo));
    test_assert( ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert( ecs_has_pair(world, child_b, EcsChildOf, parent));
    
    test_assert(ecs_has(world, child_a, Position));
    test_assert(ecs_has(world, child_b, Position));

    ecs_fini(world);
}

void Eval_struct_type_w_default_child_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE "Foo { Position: {x: 10, y: 20} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");
    ecs_entity_t x = ecs_lookup(world, "Position.x");
    ecs_entity_t y = ecs_lookup(world, "Position.y");

    test_assert(foo != 0);
    test_assert(ecs_id(Position) != 0);
    test_assert(x != 0);
    test_assert(y != 0);

    test_assert( ecs_has(world, ecs_id(Position), EcsStruct));
    test_assert( ecs_has(world, x, EcsMember));
    test_assert( ecs_has(world, y, EcsMember));
    test_assert( !ecs_has_pair(world, x, EcsIsA, ecs_id(ecs_f32_t)));
    test_assert( !ecs_has_pair(world, y, EcsIsA, ecs_id(ecs_f32_t)));

    test_assert( ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_struct_type_w_default_child_component_nested_member(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        struct {
            float x;
            float y;
        } start;

        struct {
            float x;
            float y;
        } stop;
    } Line;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Line {"
    LINE "  start {"
    LINE "    x = f32"
    LINE "    y = f32"
    LINE "  }"
    LINE "  stop {"
    LINE "    x = f32"
    LINE "    y = f32"
    LINE "  }"
    LINE "}"
    LINE
    LINE "Foo { Line: {start: {10, 20}, stop: {30, 40}} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ecs_id(Line) = ecs_lookup(world, "Line");

    test_assert(foo != 0);
    test_assert(ecs_id(Line) != 0);

    test_assert( ecs_has(world, foo, Line));

    const Line *ptr = ecs_get(world, foo, Line);
    test_assert(ptr != NULL);
    test_int(ptr->start.x, 10);
    test_int(ptr->start.y, 20);
    test_int(ptr->stop.x, 30);
    test_int(ptr->stop.y, 40);

    ecs_fini(world);
}

void Eval_enum_type_w_default_child_component(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "enum Color {"
    LINE "  Red, Green, Blue"
    LINE "}"
    LINE
    LINE "Foo { Color: {Green} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ecs_id(Color) = ecs_lookup(world, "Color");

    test_assert(foo != 0);
    test_assert(ecs_id(Color) != 0);

    test_assert( ecs_has(world, foo, Color));

    const Color *ptr = ecs_get(world, foo, Color);
    test_assert(ptr != NULL);
    test_int(*ptr, Green);

    ecs_fini(world);
}

void Eval_default_type_from_with(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "with Position {"
    LINE "  e1 = 10, 20"
    LINE "  e2 = 30, 40"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");

    test_assert(e1 != 0);
    test_assert(e2 != 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Eval_default_type_from_nested_with(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "with Velocity {"
    LINE "  e1 = 1, 2"
    LINE "  with Position {"
    LINE "    e2 = 10, 20"
    LINE "    e3 = 30, 40"
    LINE "  }"
    LINE "  e4 = 3, 4"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t e3 = ecs_lookup(world, "e3");
    ecs_entity_t e4 = ecs_lookup(world, "e4");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    const Position *p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e3, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    const Velocity *v = ecs_get(world, e1, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    v = ecs_get(world, e4, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_fini(world);
}

void Eval_default_type_from_with_in_entity_scope_w_default_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Velocity) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Velocity"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo = DefaultChildComponent: {Velocity}"
    LINE "Foo Bar {"
    LINE "  e3 = 1, 2"
    LINE "  with Position {"
    LINE "    e1 = 10, 20"
    LINE "    e2 = 30, 40"
    LINE "  }"
    LINE "  e4 = 2, 3"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup(world, "Bar.e1");
    ecs_entity_t e2 = ecs_lookup(world, "Bar.e2");
    ecs_entity_t e3 = ecs_lookup(world, "Bar.e3");
    ecs_entity_t e4 = ecs_lookup(world, "Bar.e4");

    test_assert(ecs_has(world, e1, Position));
    test_assert(ecs_has(world, e2, Position));
    test_assert(!ecs_has(world, e3, Position));
    test_assert(!ecs_has(world, e4, Position));

    test_assert(!ecs_has(world, e1, Velocity));
    test_assert(!ecs_has(world, e2, Velocity));
    test_assert(ecs_has(world, e3, Velocity));
    test_assert(ecs_has(world, e4, Velocity));

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(e4 != 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    const Velocity *v = ecs_get(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    v = ecs_get(world, e4, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 2);
    test_int(v->y, 3);

    ecs_fini(world);
}

void Eval_default_type_from_entity_scope_in_with(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "using flecs.meta"
    LINE "with Position {"
    LINE "  e1 = 10, 20"
    LINE "  struct Velocity {"
    LINE "    x = f32"
    LINE "    y = f32"
    LINE "  }"
    LINE "  e2 = 30, 40"
    LINE "}"
    LINE "e3 = Velocity: {1, 2}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t e3 = ecs_lookup(world, "e3");
    ecs_entity_t ecs_id(Velocity) = ecs_lookup(world, "Velocity");

    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(ecs_id(Velocity) != 0);

    const Position *p = ecs_get(world, e1, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, e2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    const Velocity *v = ecs_get(world, e3, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Eval_scope_w_1_subj_and_2_pairs(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "Parent {"
    LINE " Foo { (RelA, Bar) }"
    LINE " Foo { (RelB, Bar) }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t rel_a = ecs_lookup(world, "RelA");
    ecs_entity_t rel_b = ecs_lookup(world, "RelB");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t foo = ecs_lookup(world, "Parent.Foo");

    test_assert(parent != 0);
    test_assert(rel_a != 0);
    test_assert(rel_b != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert( ecs_has_pair(world, foo, rel_a, bar));
    test_assert( ecs_has_pair(world, foo, rel_b, bar));

    ecs_fini(world);
}

void Eval_inherit_from_multiple(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "Inst : Foo, Bar {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t inst = ecs_lookup(world, "Inst");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(inst != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, inst, EcsIsA, foo));
    test_assert(ecs_has_pair(world, inst, EcsIsA, bar));

    ecs_fini(world);
}

void Eval_assign_pair_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE
    LINE "Foo { (Position, Bar): {x: 10, y: 20} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(ecs_id(Position) != 0);

    test_assert( ecs_has_pair(world, foo, ecs_id(Position), bar));

    const Position *ptr = ecs_get_pair(world, foo, Position, bar);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_assign_pair_component_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE
    LINE "Parent {"
    LINE "  (Position, Foo): {x: 10, y: 20}"
    LINE "  (Position, Bar): {x: 20, y: 30}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");

    test_assert(parent != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(ecs_id(Position) != 0);

    test_assert( ecs_has_pair(world, parent, ecs_id(Position), foo));
    test_assert( ecs_has_pair(world, parent, ecs_id(Position), bar));

    const Position *
    ptr = ecs_get_pair(world, parent, Position, foo);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ptr = ecs_get_pair(world, parent, Position, bar);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);
    test_int(ptr->y, 30);

    ecs_fini(world);
}

void Eval_assign_pair_component_in_script(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE
    LINE "Foo { (Position, Bar): {x: 10, y: 20} }";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(ecs_id(Position) != 0);

    test_assert( ecs_has_pair(world, foo, ecs_id(Position), bar));

    const Position *ptr = ecs_get_pair(world, foo, Position, bar);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_assign_pair_component_in_script_update(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE
    LINE "Foo { (Position, Bar): {x: 10, y: 20} }";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    test_assert(ecs_script_update(world, s, 0, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(ecs_id(Position) != 0);

    test_assert( ecs_has_pair(world, foo, ecs_id(Position), bar));

    const Position *ptr = ecs_get_pair(world, foo, Position, bar);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_set_entity_names(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, foo);

    const char *expr =
    HEAD "foo self {}"
    LINE "foo parent {}"
    LINE "foo cascade {}"
    LINE "foo down {}"
    LINE "foo up {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t self = ecs_lookup(world, "self");
    ecs_entity_t down = ecs_lookup(world, "down");
    ecs_entity_t up = ecs_lookup(world, "up");
    ecs_entity_t parent = ecs_lookup(world, "parent");
    ecs_entity_t cascade = ecs_lookup(world, "cascade");

    test_assert(foo != 0);
    test_assert(self != 0);
    test_assert(down != 0);
    test_assert(up != 0);
    test_assert(parent != 0);
    test_assert(cascade != 0);

    test_assert( ecs_has_id(world, self, foo));
    test_assert( ecs_has_id(world, down, foo));
    test_assert( ecs_has_id(world, up, foo));
    test_assert( ecs_has_id(world, parent, foo));
    test_assert( ecs_has_id(world, cascade, foo));

    ecs_fini(world);
}

void Eval_oneof(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.enum Color {"
    LINE "  flecs.meta.constant Red"
    LINE "  flecs.meta.constant Green"
    LINE "  flecs.meta.constant Blue"
    LINE "}"
    LINE "e { (Color, Green) }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t color = ecs_lookup(world, "Color");
    ecs_entity_t green = ecs_lookup(world, "Color.Green");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_assert(color != 0);
    test_assert(green != 0);
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "Green") == 0);

    test_assert( ecs_has_id(world, color, EcsOneOf));
    test_assert( ecs_has_id(world, green, EcsConstant));
    test_assert( ecs_has_id(world, e, ecs_pair(color, green)));

    ecs_fini(world);
}

void Eval_brief_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief A description"
    LINE "Foo {}"
    LINE "Bar {}"
    LINE ""
    LINE "@brief Another description"
    LINE "Baz {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t baz = ecs_lookup(world, "Baz");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(baz != 0);

    test_str(ecs_doc_get_brief(world, foo), "A description");
    test_str(ecs_doc_get_brief(world, bar), NULL);
    test_str(ecs_doc_get_brief(world, baz), "Another description");

    ecs_fini(world);
}

void Eval_name_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@name A name"
    LINE "Foo {}"
    LINE "Bar {}"
    LINE ""
    LINE "@name Another name"
    LINE "Baz {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t baz = ecs_lookup(world, "Baz");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(baz != 0);

    test_str(ecs_doc_get_name(world, foo), "A name");
    test_str(ecs_doc_get_name(world, bar), "Bar");
    test_str(ecs_doc_get_name(world, baz), "Another name");

    ecs_fini(world);
}

void Eval_link_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@link A link"
    LINE "Foo {}"
    LINE "Bar {}"
    LINE ""
    LINE "@link Another link"
    LINE "Baz {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t baz = ecs_lookup(world, "Baz");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(baz != 0);

    test_str(ecs_doc_get_link(world, foo), "A link");
    test_str(ecs_doc_get_link(world, bar), NULL);
    test_str(ecs_doc_get_link(world, baz), "Another link");

    ecs_fini(world);
}

void Eval_color_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@color #554433"
    LINE "Foo {}"
    LINE "Bar {}"
    LINE ""
    LINE "@color rgb(10, 20, 30, 1.0)"
    LINE "Baz {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t baz = ecs_lookup(world, "Baz");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(baz != 0);

    test_str(ecs_doc_get_color(world, foo), "#554433");
    test_str(ecs_doc_get_color(world, bar), NULL);
    test_str(ecs_doc_get_color(world, baz), "rgb(10, 20, 30, 1.0)");

    ecs_fini(world);
}

void Eval_multiple_annotations(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief A description"
    LINE "@name A name"
    LINE "@link A link"
    LINE "@color #554433"
    LINE "Foo {}"
    LINE "Bar {}"
    LINE ""
    LINE "@brief Another description"
    LINE "@name Another name"
    LINE "Baz {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t baz = ecs_lookup(world, "Baz");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(baz != 0);

    test_str(ecs_doc_get_brief(world, foo), "A description");
    test_str(ecs_doc_get_name(world, foo), "A name");
    test_str(ecs_doc_get_link(world, foo), "A link");
    test_str(ecs_doc_get_color(world, foo), "#554433");
    test_str(ecs_doc_get_brief(world, bar), NULL);
    test_str(ecs_doc_get_brief(world, baz), "Another description");
    test_str(ecs_doc_get_name(world, baz), "Another name");

    ecs_fini(world);
}

void Eval_annotation_w_trailing_space(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief   A description  "
    LINE "Foo {}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    test_str(ecs_doc_get_brief(world, foo), "A description");

    ecs_fini(world);
}

typedef struct String {
    char *value;
} String;

void Eval_multiline_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(String) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "String"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "Foo { String: {value: `start"
    LINE "Hello World"
    LINE "Foo Bar"
    LINE "Special characters }{\"\"'',"
    LINE "`}}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, String));

    const String *ptr = ecs_get(world, foo, String);
    test_assert(ptr != NULL);
    test_assert(ptr->value != NULL);
    test_str(ptr->value,
        "start\n"
        "Hello World\n"
        "Foo Bar\n"
        "Special characters }{\"\"'',\n"
    );

    ecs_os_free(ptr->value);

    ecs_fini(world);
}

void Eval_annotate_declaration(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "@brief A brief description"
    LINE "Foo Bar";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    test_assert(ecs_has_id(world, bar, foo));

    test_assert(ecs_doc_get_brief(world, foo) == NULL);
    test_assert(ecs_doc_get_brief(world, bar) != NULL);
    test_str(ecs_doc_get_brief(world, bar), "A brief description");

    ecs_fini(world);
}

void Eval_declaration_w_underscore_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "Foo _Bar";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "_Bar");
    test_assert(bar != 0);

    test_assert(!ecs_has_id(world, foo, bar));
    test_assert(ecs_has_id(world, bar, foo));

    ecs_fini(world);
}

void Eval_anonymous_entity(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "_ { Foo }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_iter_t it = ecs_each_id(world, foo );
    test_assert(ecs_each_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);
    test_assert(!ecs_each_next(&it));

    ecs_fini(world);
}

void Eval_anonymous_entity_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "Parent {"
    LINE "  _ { Foo }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t parent = ecs_lookup(world, "Parent");
    test_assert(parent != 0);

    ecs_iter_t it = ecs_each_id(world, foo );
    test_assert(ecs_each_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);
    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_assert(!ecs_each_next(&it));

    ecs_fini(world);
}

void Eval_anonymous_declaration(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "Foo _";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_iter_t it = ecs_each_id(world, foo );
    test_assert(ecs_each_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);
    test_assert(!ecs_each_next(&it));

    ecs_fini(world);
}

void Eval_const_var_int(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_x = 10"
    LINE "const var_y = 20"
    LINE ""
    LINE "e { Position: {$var_x, $var_y} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_var_float(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_x = 10.5"
    LINE "const var_y = 20.5"
    LINE ""
    LINE "e { Position: {$var_x, $var_y} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_flt(p->x, 10.5);
    test_flt(p->y, 20.5);

    ecs_fini(world);
}

void Eval_const_var_bool(void) {
    ecs_world_t *world = ecs_init();

    typedef struct Bools {
        bool x;
        bool y;
    } Bools;

    ecs_entity_t ecs_id(Bools) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Bools"}),
        .members = {
            {"x", ecs_id(ecs_bool_t)},
            {"y", ecs_id(ecs_bool_t)}
        }
    });

    const char *expr =
    HEAD "const var_x = true"
    LINE "const var_y = false"
    LINE ""
    LINE "e { Bools: {$var_x, $var_y} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Bools));

    const Bools *p = ecs_get(world, e, Bools);
    test_assert(p != NULL);
    test_bool(p->x, true);
    test_bool(p->y, false);

    ecs_fini(world);
}

void Eval_const_var_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_x = \"10.5\""
    LINE "const var_y = \"20.5\""
    LINE ""
    LINE "e { Position: {$var_x, $var_y} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_flt(p->x, 10.5);
    test_flt(p->y, 20.5);

    ecs_fini(world);
}

void Eval_const_var_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Point) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Point"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Point)},
            {"stop", ecs_id(Point)}
        }
    });

    const char *expr =
    HEAD "const var_start = Point: {10.5, 20.5}"
    LINE "const var_stop = Point: {30.5, 40.5}"
    LINE ""
    LINE "e { Line: {start: $var_start, stop: $var_stop} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Line));

    const Line *l = ecs_get(world, e, Line);
    test_assert(l != NULL);
    test_flt(l->start.x, 10.5);
    test_flt(l->start.y, 20.5);
    test_flt(l->stop.x, 30.5);
    test_flt(l->stop.y, 40.5);

    ecs_fini(world);
}

void Eval_const_var_scoped(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_x = 10"
    LINE "a { Position: {$var_x, $var_x} }"
    LINE "a {"
    LINE "  const var_x = 20"
    LINE "  const var_y = 30"
    LINE "  b { Position: {$var_x, $var_y} }"
    LINE "}"
    LINE "a {"
    LINE "  const var_y = 20"
    LINE "  c { Position: {$var_x, $var_y} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);
    ecs_entity_t b = ecs_lookup(world, "a.b");
    test_assert(b != 0);
    ecs_entity_t c = ecs_lookup(world, "a.c");
    test_assert(c != 0);

    test_assert(ecs_has(world, a, Position));
    test_assert(ecs_has(world, b, Position));
    test_assert(ecs_has(world, c, Position));
    const Position *p;

    p = ecs_get(world, a, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 10);

    p = ecs_get(world, b, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 30);

    p = ecs_get(world, c, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_assign_component_from_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos = Position: {10, 20}"
    LINE "a { $var_pos }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t var = ecs_lookup(world, "var_pos");
    test_assert(var == 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);

    const Position *p = ecs_get(world, a, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_assign_component_from_var_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos = Position: {10, 20}"
    LINE "a {"
    LINE "  $var_pos"
    LINE "}"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t var = ecs_lookup(world, "var_pos");
    test_assert(var == 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);

    const Position *p = ecs_get(world, a, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_scope_w_component_after_const_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  const var = 5"
    LINE "  Position: {x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Eval_component_after_const_add_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  const var = 5 + 15"
    LINE "  Position: {x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_component_after_const_sub_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  const var = 25 - 5"
    LINE "  Position: {x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_component_after_const_mul_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  const var = 2 * 10"
    LINE "  Position: {x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_component_after_const_div_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo {"
    LINE "  const var = 40 / 2"
    LINE "  Position: {x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_component_after_const_paren_expr(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "e {"
    LINE "  const val = (10 + 20)"
    LINE "  Position: {$val, $val * 2}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}

void Eval_parse_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "Bar {"
    LINE "  Hello {}"
    LINE "}";

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_set_with(world, 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Bar.Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    
    test_assert(ecs_has(world, foo, Tag));
    test_assert(ecs_has(world, bar, Tag));
    test_assert(ecs_has(world, hello, Tag));

    ecs_fini(world);
}

void Eval_parse_with_w_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const char *expr =
    HEAD "Foo {}"
    LINE "with TagB {"
    LINE "  Bar {"
    LINE "    Hello {}"
    LINE "  }"
    LINE "}";

    ecs_set_with(world, TagA);
    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_set_with(world, 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Bar.Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    
    test_assert(ecs_has(world, foo, TagA));
    test_assert(ecs_has(world, bar, TagA));
    test_assert(ecs_has(world, hello, TagA));

    test_assert(!ecs_has(world, TagB, TagA));
    test_assert(!ecs_has(world, foo, TagB));
    test_assert(ecs_has(world, bar, TagB));
    test_assert(ecs_has(world, hello, TagB));

    ecs_fini(world);
}

void Eval_parse_with_w_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, Hello);

    const char *expr =
    HEAD "Foo {}"
    LINE "with TagB {"
    LINE "  Bar {"
    LINE "    Hello"
    LINE "  }"
    LINE "}";

    ecs_set_with(world, TagA);
    test_assert(ecs_script_run(world, NULL, expr) == 0);
    ecs_set_with(world, 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    
    test_assert(ecs_has(world, foo, TagA));
    test_assert(ecs_has(world, bar, TagA));
    test_assert(!ecs_has(world, hello, TagA));
    test_assert(ecs_has_id(world, bar, hello));

    test_assert(!ecs_has(world, TagB, TagA));
    test_assert(!ecs_has(world, foo, TagB));
    test_assert(ecs_has(world, bar, TagB));
    test_assert(!ecs_has(world, hello, TagB));

    ecs_fini(world);
}

void Eval_parse_with_value(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "with Position(10, 20) {"
    LINE "  Foo {}"
    LINE "  Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    
    {
        const Position *ptr = ecs_get(world, bar, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Eval_parse_with_2_values(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "with Position(10, 20), Velocity(1, 2) {"
    LINE "  Foo {}"
    LINE "  Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, bar, Position);
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
    {
        const Velocity *ptr = ecs_get(world, bar, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Eval_parse_with_2_nested_values(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "with Position(10, 20) {"
    LINE "  with Velocity(1, 2) {"
    LINE "    Foo {}"
    LINE "    Bar {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, bar, Position);
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
    {
        const Velocity *ptr = ecs_get(world, bar, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Eval_parse_with_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "const pos = Position: {10, 20}"
    LINE "with $pos {"
    LINE "  Foo {}"
    LINE "  Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, bar, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Eval_parse_with_2_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "const pos = Position: {10, 20}"
    LINE "const vel = Velocity: {1, 2}"
    LINE "with $pos, $vel {"
    LINE "  Foo {}"
    LINE "  Bar {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, bar, Position);
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
    {
        const Velocity *ptr = ecs_get(world, bar, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Eval_parse_with_2_nested_vars(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "const pos = Position: {10, 20}"
    LINE "const vel = Velocity: {1, 2}"
    LINE "with $pos, $vel {"
    LINE "  with $vel {"
    LINE "    Foo {}"
    LINE "    Bar {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, bar, Position);
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
    {
        const Velocity *ptr = ecs_get(world, bar, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Eval_parse_with_var_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "Parent {"
    LINE "  const pos = Position: {10, 20}"
    LINE "  with $pos { "
    LINE "    Foo {}"
    LINE "    Bar {}"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t foo = ecs_lookup(world, "Parent.Foo");
    ecs_entity_t bar = ecs_lookup(world, "Parent.Bar");

    test_assert(parent != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);

    {
        const Position *ptr = ecs_get(world, foo, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, bar, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}


void Eval_assign_const_w_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "const var = 5 + 1"
    LINE "e { Position: {x: $var, y: $var * 2} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 12);

    ecs_fini(world);
}

void Eval_const_w_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "const var = flecs.meta.i32: 5 / 2"
    LINE "e { Position: {x: $var, y: $var * 3} }";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Eval_typed_const_w_composite_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos = Position: {10, 20}"
    LINE "a { $var_pos }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t var = ecs_lookup(world, "var_pos");
    test_assert(var == 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);

    const Position *p = ecs_get(world, a, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_assign_var_to_typed_const_w_composite_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos_a = Position: {10, 20}"
    HEAD "const var_pos_b = $var_pos_a"
    LINE "a { $var_pos_b }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t var = ecs_lookup(world, "var_pos");
    test_assert(var == 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);

    const Position *p = ecs_get(world, a, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_using_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p1 = ecs_entity(world, { .name = "foo.p1" });
    ecs_entity_t p2 = ecs_entity(world, { .name = "foo.p2" });

    ecs_set_scope(world, p1);
    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_set_scope(world, p2);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_set_scope(world, 0);

    const char *expr =
    LINE "using foo.*\n"
    LINE "e { Position: {10, 20} }\n"
    LINE "e { Velocity: {1, 2} }\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t pos = ecs_lookup(world, "foo.p1.Position");
    test_assert(pos == ecs_id(Position));
    ecs_entity_t vel = ecs_lookup(world, "foo.p2.Velocity");
    test_assert(vel == ecs_id(Velocity));
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has(world, e, Velocity));

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Velocity *ptr = ecs_get(world, e, Velocity);
        test_assert(ptr != NULL);
        test_int(ptr->x, 1);
        test_int(ptr->y, 2);
    }

    ecs_fini(world);
}

void Eval_single_line_comment_in_value(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "e {Position: {\n"
    LINE "  x: 10\n"
    LINE "  //foo\n"
    LINE "  y: 20\n"
    LINE "}}";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_single_line_comment_in_value_after_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "e {Position:{\n"
    LINE "  // foo\n"
    LINE "  x: 10\n"
    LINE "  y: 20\n"
    LINE "}}";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_multi_line_comment_in_value(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "e {Position: {\n"
    LINE "  x: 10\n"
    LINE "  /*\n"
    LINE "   * foo\n"
    LINE "   */\n"
    LINE "  y: 20\n"
    LINE "}}";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_multi_line_comment_in_value_after_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "e {Position: {\n"
    LINE "  /*\n"
    LINE "   * foo\n"
    LINE "   */\n"
    LINE "  x: 10\n"
    LINE "  y: 20\n"
    LINE "}}";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Eval_module_stmt(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "module hello\n"
    LINE "Foo {}\n"
    LINE "e { Position: {10, 20} }\n"
    LINE "e { Foo }\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_get_scope(world) == 0);

    ecs_entity_t hello = ecs_lookup(world, "hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsModule));

    ecs_entity_t e = ecs_lookup(world, "hello.e");
    test_assert(e != 0);
    ecs_entity_t foo = ecs_lookup(world, "hello.Foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_id(world, e, foo));

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Eval_nested_module_stmt(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "module hello.world\n"
    LINE "Foo {}\n"
    LINE "e { Position: {10, 20} }\n"
    LINE "e { Foo }\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsModule));
    ecs_entity_t hello_world = ecs_lookup(world, "hello.world");
    test_assert(hello_world != 0);
    test_assert(ecs_has_id(world, hello_world, EcsModule));

    ecs_entity_t e = ecs_lookup(world, "hello.world.e");
    test_assert(e != 0);
    ecs_entity_t foo = ecs_lookup(world, "hello.world.Foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_id(world, e, foo));

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Eval_module_stmt_w_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "module hello.world\n"
    LINE "Foo {}\n"
    LINE "parent {\n"
    LINE "  e { Position: {10, 20} }\n"
    LINE "  e { Foo }\n"
    LINE "}\n"
    LINE "f { Position: {30, 40} }\n"
    LINE "f { Foo }\n"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsModule));
    ecs_entity_t hello_world = ecs_lookup(world, "hello.world");
    test_assert(hello_world != 0);
    test_assert(ecs_has_id(world, hello_world, EcsModule));

    ecs_entity_t parent = ecs_lookup(world, "hello.world.parent");
    test_assert(parent != 0);
    ecs_entity_t e = ecs_lookup(world, "hello.world.parent.e");
    test_assert(e != 0);
    ecs_entity_t f = ecs_lookup(world, "hello.world.f");
    test_assert(f != 0);
    ecs_entity_t foo = ecs_lookup(world, "hello.world.Foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has(world, f, Position));
    test_assert(ecs_has_id(world, f, foo));

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, f, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 30);
        test_int(ptr->y, 40);
    }

    ecs_fini(world);
}

void Eval_module_stmt_w_nested_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "module hello.world\n"
    LINE "Foo {}\n"
    LINE "parent {\n"
    LINE "  child {\n"
    LINE "    e { Position: {10, 20} }\n"
    LINE "    e { Foo }\n"
    LINE "  }\n"
    LINE "}\n"
    LINE "f { Position: {30, 40} }\n"
    LINE "f { Foo }\n"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsModule));
    ecs_entity_t hello_world = ecs_lookup(world, "hello.world");
    test_assert(hello_world != 0);
    test_assert(ecs_has_id(world, hello_world, EcsModule));

    ecs_entity_t parent = ecs_lookup(world, "hello.world.parent");
    test_assert(parent != 0);
    ecs_entity_t child = ecs_lookup(world, "hello.world.parent.child");
    test_assert(child != 0);
    ecs_entity_t e = ecs_lookup(world, "hello.world.parent.child.e");
    test_assert(e != 0);
    ecs_entity_t f = ecs_lookup(world, "hello.world.f");
    test_assert(f != 0);
    ecs_entity_t foo = ecs_lookup(world, "hello.world.Foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, e, Position));
    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has(world, f, Position));
    test_assert(ecs_has_id(world, f, foo));

    {
        const Position *ptr = ecs_get(world, e, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }
    {
        const Position *ptr = ecs_get(world, f, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 30);
        test_int(ptr->y, 40);
    }

    ecs_fini(world);
}

void Eval_assign_singleton_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    LINE "$ { Foo }\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, foo));

    ecs_fini(world);
}

void Eval_assign_singleton_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "$ = Position: {10, 20}\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_has(world, ecs_id(Position), Position));

    const Position *p = ecs_get(world, ecs_id(Position), Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_assign_singleton_tag_w_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    LINE "$ {\n"
    LINE "  Foo\n"
    LINE "}\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, foo));

    ecs_fini(world);
}

void Eval_assign_singleton_2_tags_w_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    const char *expr =
    LINE "$ {\n"
    LINE "  Foo\n"
    LINE "  Bar\n"
    LINE "}\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    test_assert(ecs_has_id(world, foo, foo));
    test_assert(ecs_has_id(world, bar, bar));

    ecs_fini(world);
}

void Eval_assign_singleton_component_w_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "$ {\n"
    LINE "  Position: {10, 20}\n"
    LINE "}\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_has(world, ecs_id(Position), Position));

    const Position *p = ecs_get(world, ecs_id(Position), Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_assign_singleton_2_components_w_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "$ {\n"
    LINE "  Position: {10, 20}\n"
    LINE "  Velocity: {1, 2}\n"
    LINE "}\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    const Position *p = ecs_get(world, ecs_id(Position), Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, ecs_id(Velocity), Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Eval_with_pair_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "Tgt {}\n"
    LINE "Rel {}\n"
    LINE "\n"
    LINE "Parent {\n"
    LINE "  with (Rel, Tgt) {\n"
    LINE "    Child {}\n"
    LINE "  }\n"
    LINE "}\n";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");
    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");

    test_assert(!ecs_lookup(world, "Parent.Rel"));
    test_assert(!ecs_lookup(world, "Parent.Tgt"));

    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(rel != 0);
    test_assert(tgt != 0);

    test_assert(ecs_has_pair(world, child, rel, tgt));

    ecs_fini(world);
}

void Eval_with_pair_component_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "Tgt {}\n"
    LINE "\n"
    LINE "Parent {\n"
    LINE "  with (Position, Tgt)(10, 20) {\n"
    LINE "    Child {}\n"
    LINE "  }\n"
    LINE "}\n";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");

    test_assert(!ecs_lookup(world, "Parent.Rel"));
    test_assert(!ecs_lookup(world, "Parent.Tgt"));

    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(tgt != 0);

    test_assert(ecs_has_pair(world, child, ecs_id(Position), tgt));

    const Position *pos = ecs_get_pair(world, child, Position, tgt);
    test_assert(pos != NULL);
    test_int(pos->x, 10);
    test_int(pos->y, 20);

    ecs_fini(world);
}

void Eval_pair_w_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "const rel = Rel\n"
    LINE "ent {\n"
    LINE "  ($rel, Tgt)\n"
    LINE "}\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_pair(world, ent, Rel, Tgt));

    ecs_fini(world);
}

void Eval_pair_w_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "const tgt = Tgt\n"
    LINE "ent {\n"
    LINE "  (Rel, $tgt)\n"
    LINE "}\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_pair(world, ent, Rel, Tgt));

    ecs_fini(world);
}

void Eval_component_in_with_scope_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Bar);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "foo {\n"
    LINE "  Position: {10, 20}\n"
    LINE "  with Position(30, 40) {\n"
    LINE "    Bar\n"
    LINE "  }\n"
    LINE "}\n";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    test_assert(ecs_has(world, foo, Position));
    test_assert(ecs_has_id(world, foo, bar));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_array_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_array(world, {
        .entity = ecs_id(Position),
        .type = ecs_id(ecs_f32_t),
        .count = 2
    });

    const char *expr =
    LINE "foo { Position: [10, 20] }\n";
    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

ECS_CTOR(Position, ptr, {
    ptr->x = 100;
    ptr->y = 200;
})

static int on_set_position_invoked = 0;

static void on_set_position(ecs_iter_t *it) {
    on_set_position_invoked ++;

    Position *p = ecs_field(it, Position, 0);
    test_assert(p != NULL);
}

void Eval_on_set_w_kind_paren_no_reflection(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    const char *expr =
    HEAD "Position e()";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_on_set_w_kind_paren(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Position e()";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_on_set_w_kind_no_paren(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Position e()";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_on_set_w_kind_no_paren_no_reflection(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    const char *expr =
    HEAD "Position e";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_on_set_w_single_assign(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    const char *expr =
    HEAD "e = Position: {}";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_on_set_w_single_assign_scoped_w_value(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    const char *expr =
    HEAD "e { Position: {} }";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 1);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_on_set_w_single_assign_scoped_no_value(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_set_hooks(world, Position, {
        .ctor = ecs_ctor(Position),
        .on_set = on_set_position
    });

    const char *expr =
    HEAD "e { Position }";

    test_int(on_set_position_invoked, 0);

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(on_set_position_invoked, 0);

    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 100);
    test_int(p->y, 200);

    ecs_fini(world);
}

void Eval_if_true(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "if true {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b") == 0);

    ecs_fini(world);
}

void Eval_if_true_no_else(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "if true {"
    LINE "  a{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") != 0);

    ecs_fini(world);
}

void Eval_if_false(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "if false {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") != 0);

    ecs_fini(world);
}

void Eval_if_10(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "if 10 {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b") == 0);

    ecs_fini(world);
}

void Eval_if_256(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "if 256 {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b") == 0);

    ecs_fini(world);
}

void Eval_if_0(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "if 0 {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") != 0);

    ecs_fini(world);
}

void Eval_if_true_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = true"
    LINE "if $v {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b") == 0);

    ecs_fini(world);
}

void Eval_if_false_var(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const v = false"
    LINE "if $v {"
    LINE "  a{}"
    LINE "} else {"
    LINE "  b{}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") != 0);

    ecs_fini(world);
}

void Eval_if_true_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "e {"
    LINE "  if true {"
    LINE "    Foo"
    LINE "  } else {"
    LINE "    Bar"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Foo));

    ecs_fini(world);
}

void Eval_if_false_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "e {"
    LINE "  if false {"
    LINE "    Foo"
    LINE "  } else {"
    LINE "    Bar"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, Bar));

    ecs_fini(world);
}

void Eval_if_lt(void) {
    ecs_world_t *world = ecs_init();

    {
        const char *expr =
        HEAD "if 2.0 > 3.0 {"
        LINE "  a{}"
        LINE "} else {"
        LINE "  b{}"
        LINE "}";

        test_assert(ecs_script_run(world, NULL, expr) == 0);
        test_assert(ecs_lookup(world, "a") == 0);
        test_assert(ecs_lookup(world, "b") != 0);
    }

    {
        const char *expr =
        HEAD "if 3.0 > 2.0 {"
        LINE "  c{}"
        LINE "} else {"
        LINE "  d{}"
        LINE "}";

        test_assert(ecs_script_run(world, NULL, expr) == 0);
        test_assert(ecs_lookup(world, "c") != 0);
        test_assert(ecs_lookup(world, "d") == 0);
    }

    ecs_fini(world);
}

void Eval_if_lt_const(void) {
    ecs_world_t *world = ecs_init();

    {
        const char *expr =
        HEAD "const v = 2.0"
        LINE "if $v > 3.0 {"
        LINE "  a{}"
        LINE "} else {"
        LINE "  b{}"
        LINE "}";

        test_assert(ecs_script_run(world, NULL, expr) == 0);
        test_assert(ecs_lookup(world, "a") == 0);
        test_assert(ecs_lookup(world, "b") != 0);
    }

    {
        const char *expr =
        HEAD "const v = 3.0"
        LINE "if $v > 2.0 {"
        LINE "  c{}"
        LINE "} else {"
        LINE "  d{}"
        LINE "}";

        test_assert(ecs_script_run(world, NULL, expr) == 0);
        test_assert(ecs_lookup(world, "c") != 0);
        test_assert(ecs_lookup(world, "d") == 0);
    }

    ecs_fini(world);
}

void Eval_isa_in_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module things"
    LINE "Animal {}"
    LINE "Dog {"
    LINE "  (IsA, Animal)"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup_child(world, 0, "IsA") == 0);
    test_assert(ecs_lookup(world, "things.IsA") == 0);

    ecs_entity_t animal = ecs_lookup(world, "things.Animal");
    ecs_entity_t dog = ecs_lookup(world, "things.Dog");

    test_assert(animal != 0);
    test_assert(dog != 0);

    test_assert(ecs_has_pair(world, dog, EcsIsA, animal));

    ecs_fini(world);
}

void Eval_isa_hierarchy(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(IsA, Thing) {"
    LINE "  (IsA, Organism) {"
    LINE "    (IsA, Animal) {"
    LINE "      Dog {}"
    LINE "      Cat {}"
    LINE "    }"
    LINE "    (IsA, Tree) {"
    LINE "      Oak {}"
    LINE "      Maple {}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup_child(world, 0, "IsA") == 0);

    ecs_entity_t thing = ecs_lookup(world, "Thing");
    ecs_entity_t organism = ecs_lookup(world, "Organism");
    ecs_entity_t animal = ecs_lookup(world, "Animal");
    ecs_entity_t dog = ecs_lookup(world, "Dog");
    ecs_entity_t cat = ecs_lookup(world, "Cat");
    ecs_entity_t tree = ecs_lookup(world, "Tree");
    ecs_entity_t oak = ecs_lookup(world, "Oak");
    ecs_entity_t maple = ecs_lookup(world, "Maple");

    test_assert(thing != 0);
    test_assert(organism != 0);
    test_assert(animal != 0);
    test_assert(dog != 0);
    test_assert(cat != 0);
    test_assert(tree != 0);
    test_assert(oak != 0);
    test_assert(maple != 0);

    test_assert(ecs_has_pair(world, organism, EcsIsA, thing));
    test_assert(ecs_has_pair(world, animal, EcsIsA, organism));
    test_assert(ecs_has_pair(world, dog, EcsIsA, animal));
    test_assert(ecs_has_pair(world, cat, EcsIsA, animal));
    test_assert(ecs_has_pair(world, tree, EcsIsA, organism));
    test_assert(ecs_has_pair(world, oak, EcsIsA, tree));
    test_assert(ecs_has_pair(world, maple, EcsIsA, tree));

    ecs_fini(world);
}

void Eval_isa_hierarchy_in_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module things"
    LINE "(IsA, Thing) {"
    LINE "  (IsA, Organism) {"
    LINE "    (IsA, Animal) {"
    LINE "      Dog {}"
    LINE "      Cat {}"
    LINE "    }"
    LINE "    (IsA, Tree) {"
    LINE "      Oak {}"
    LINE "      Maple {}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup_child(world, 0, "IsA") == 0);
    test_assert(ecs_lookup(world, "things.IsA") == 0);

    ecs_entity_t thing = ecs_lookup(world, "things.Thing");
    ecs_entity_t organism = ecs_lookup(world, "things.Organism");
    ecs_entity_t animal = ecs_lookup(world, "things.Animal");
    ecs_entity_t dog = ecs_lookup(world, "things.Dog");
    ecs_entity_t cat = ecs_lookup(world, "things.Cat");
    ecs_entity_t tree = ecs_lookup(world, "things.Tree");
    ecs_entity_t oak = ecs_lookup(world, "things.Oak");
    ecs_entity_t maple = ecs_lookup(world, "things.Maple");

    test_assert(thing != 0);
    test_assert(organism != 0);
    test_assert(animal != 0);
    test_assert(dog != 0);
    test_assert(cat != 0);
    test_assert(tree != 0);
    test_assert(oak != 0);
    test_assert(maple != 0);

    test_assert(ecs_has_pair(world, organism, EcsIsA, thing));
    test_assert(ecs_has_pair(world, animal, EcsIsA, organism));
    test_assert(ecs_has_pair(world, dog, EcsIsA, animal));
    test_assert(ecs_has_pair(world, cat, EcsIsA, animal));
    test_assert(ecs_has_pair(world, tree, EcsIsA, organism));
    test_assert(ecs_has_pair(world, oak, EcsIsA, tree));
    test_assert(ecs_has_pair(world, maple, EcsIsA, tree));

    ecs_fini(world);
}

void Eval_custom_isa_hierarchy_in_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module things"
    LINE "IsA {}"
    LINE "(IsA, Thing) {"
    LINE "  (IsA, Organism) {"
    LINE "    (IsA, Animal) {"
    LINE "      Dog {}"
    LINE "      Cat {}"
    LINE "    }"
    LINE "    (IsA, Tree) {"
    LINE "      Oak {}"
    LINE "      Maple {}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup_child(world, 0, "IsA") == 0);

    ecs_entity_t isa = ecs_lookup(world, "things.IsA");
    ecs_entity_t thing = ecs_lookup(world, "things.Thing");
    ecs_entity_t organism = ecs_lookup(world, "things.Organism");
    ecs_entity_t animal = ecs_lookup(world, "things.Animal");
    ecs_entity_t dog = ecs_lookup(world, "things.Dog");
    ecs_entity_t cat = ecs_lookup(world, "things.Cat");
    ecs_entity_t tree = ecs_lookup(world, "things.Tree");
    ecs_entity_t oak = ecs_lookup(world, "things.Oak");
    ecs_entity_t maple = ecs_lookup(world, "things.Maple");

    test_assert(isa != 0);
    test_assert(thing != 0);
    test_assert(organism != 0);
    test_assert(animal != 0);
    test_assert(dog != 0);
    test_assert(cat != 0);
    test_assert(tree != 0);
    test_assert(oak != 0);
    test_assert(maple != 0);

    test_assert(ecs_has_pair(world, organism, isa, thing));
    test_assert(ecs_has_pair(world, animal, isa, organism));
    test_assert(ecs_has_pair(world, dog, isa, animal));
    test_assert(ecs_has_pair(world, cat, isa, animal));
    test_assert(ecs_has_pair(world, tree, isa, organism));
    test_assert(ecs_has_pair(world, oak, isa, tree));
    test_assert(ecs_has_pair(world, maple, isa, tree));

    ecs_fini(world);
}

void Eval_custom_isa_hierarchy_in_subtree(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module things"
    LINE "(IsA, Thing) {"
    LINE "  IsA {}"
    LINE "  (IsA, Organism) {"
    LINE "    (IsA, Animal) {"
    LINE "      Dog {}"
    LINE "      Cat {}"
    LINE "    }"
    LINE "    (IsA, Tree) {"
    LINE "      Oak {}"
    LINE "      Maple {}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup_child(world, 0, "IsA") == 0);

    ecs_entity_t isa = ecs_lookup(world, "things.IsA");
    ecs_entity_t thing = ecs_lookup(world, "things.Thing");
    ecs_entity_t organism = ecs_lookup(world, "things.Organism");
    ecs_entity_t animal = ecs_lookup(world, "things.Animal");
    ecs_entity_t dog = ecs_lookup(world, "things.Dog");
    ecs_entity_t cat = ecs_lookup(world, "things.Cat");
    ecs_entity_t tree = ecs_lookup(world, "things.Tree");
    ecs_entity_t oak = ecs_lookup(world, "things.Oak");
    ecs_entity_t maple = ecs_lookup(world, "things.Maple");

    test_assert(isa != 0);
    test_assert(thing != 0);
    test_assert(organism != 0);
    test_assert(animal != 0);
    test_assert(dog != 0);
    test_assert(cat != 0);
    test_assert(tree != 0);
    test_assert(oak != 0);
    test_assert(maple != 0);

    test_assert(ecs_has_pair(world, isa, EcsIsA, thing));
    test_assert(ecs_has_pair(world, organism, EcsIsA, thing));
    test_assert(ecs_has_pair(world, animal, isa, organism));
    test_assert(ecs_has_pair(world, dog, isa, animal));
    test_assert(ecs_has_pair(world, cat, isa, animal));
    test_assert(ecs_has_pair(world, tree, isa, organism));
    test_assert(ecs_has_pair(world, oak, isa, tree));
    test_assert(ecs_has_pair(world, maple, isa, tree));

    ecs_fini(world);
}

void Eval_inherit_w_kind(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Prefab Foo"
    LINE "Prefab Bar : Foo";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(foo != 0);
    test_assert(bar != 0);
    ecs_has_id(world, foo, EcsPrefab);
    ecs_has_id(world, bar, EcsPrefab);
    test_assert(ecs_has_pair(world, bar, EcsIsA, foo));

    ecs_fini(world);
}

void Eval_inherit_w_kind_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Prefab Foo"
    LINE "Prefab Bar : Foo {"
    LINE "  Child {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Bar.Child");
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);
    ecs_has_id(world, foo, EcsPrefab);
    ecs_has_id(world, bar, EcsPrefab);
    test_assert(ecs_has_pair(world, bar, EcsIsA, foo));

    ecs_fini(world);
}

void Eval_inherit_w_kind_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Prefab Foo"
    LINE "Position Bar : Foo (10, 20)";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(foo != 0);
    test_assert(bar != 0);
    ecs_has_id(world, foo, EcsPrefab);
    ecs_has(world, bar, Position);
    test_assert(ecs_has_pair(world, bar, EcsIsA, foo));

    const Position *p = ecs_get(world, bar, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_inherit_w_kind_value_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Prefab Foo"
    LINE "Position Bar : Foo (10, 20) {"
    LINE "  Child {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Bar.Child");
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(child != 0);
    ecs_has_id(world, foo, EcsPrefab);
    ecs_has(world, bar, Position);
    test_assert(ecs_has_pair(world, bar, EcsIsA, foo));

    const Position *p = ecs_get(world, bar, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_multiple_inherit_w_kind(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Prefab Hello"
    LINE "Prefab World"
    LINE "Prefab Foo : Hello, World";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t world_ = ecs_lookup(world, "World");
    test_assert(foo != 0);
    test_assert(hello != 0);
    test_assert(world_ != 0);
    test_assert(world_ != EcsWorld);
    ecs_has_id(world, hello, EcsPrefab);
    ecs_has_id(world, world_, EcsPrefab);
    test_assert(ecs_has_pair(world, foo, EcsIsA, hello));
    test_assert(ecs_has_pair(world, foo, EcsIsA, world_));

    ecs_fini(world);
}

void Eval_multiple_inherit_w_kind_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Prefab Hello"
    LINE "Prefab World"
    LINE "Prefab Foo : Hello, World {"
    LINE "  Child {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t world_ = ecs_lookup(world, "World");
    ecs_entity_t child = ecs_lookup(world, "Foo.Child");
    test_assert(foo != 0);
    test_assert(child != 0);
    test_assert(hello != 0);
    test_assert(world_ != 0);
    test_assert(world_ != EcsWorld);
    ecs_has_id(world, hello, EcsPrefab);
    ecs_has_id(world, world_, EcsPrefab);
    test_assert(ecs_has_pair(world, foo, EcsIsA, hello));
    test_assert(ecs_has_pair(world, foo, EcsIsA, world_));

    ecs_fini(world);
}

void Eval_auto_override_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    LINE "Prefab Foo {"
    LINE "  auto_override | Tag"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_has_id(world, foo, EcsPrefab);
    test_assert(ecs_has_id(world, foo, ECS_AUTO_OVERRIDE | Tag));
    test_assert(!ecs_has(world, foo, Tag));

    ecs_fini(world);
}

void Eval_auto_override_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "Prefab Foo {"
    LINE "  auto_override | Position: {10, 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_has_id(world, foo, EcsPrefab);
    test_assert(ecs_has_id(world, foo, ECS_AUTO_OVERRIDE | ecs_id(Position)));
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_auto_override_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "Prefab Foo {"
    LINE "  auto_override | (Rel, Tgt)"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_has_id(world, foo, EcsPrefab);
    test_assert(ecs_has_id(world, foo, ECS_AUTO_OVERRIDE | ecs_pair(Rel, Tgt)));
    test_assert(!ecs_has_pair(world, foo, Rel, Tgt));

    ecs_fini(world);
}

void Eval_auto_override_pair_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "Prefab Foo {"
    LINE "  auto_override | (Position, Tgt): {10, 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_has_id(world, foo, EcsPrefab);
    test_assert(ecs_has_id(world, foo, ECS_AUTO_OVERRIDE | ecs_pair_t(Position, Tgt)));
    test_assert(ecs_has_pair(world, foo, ecs_id(Position), Tgt));

    const Position *p = ecs_get_pair(world, foo, Position, Tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_lowercase_prefab_kind(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    HEAD "prefab Foo {"
    LINE "  Tag"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);
    
    test_assert(ecs_lookup(world, "prefab") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has_id(world, foo, EcsPrefab));
    test_assert(ecs_has_id(world, foo, Tag));

    ecs_fini(world);
}

void Eval_assign_component_to_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "const pos = e[Position]"
    LINE "foo {"
    LINE "  Position: {$pos.y, $pos.x}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);
    test_int(ptr->y, 10);

    ecs_fini(world);
}

void Eval_assign_component_member_to_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "const px = e[Position].x"
    LINE "const py = e[Position].y"
    LINE ""
    LINE "foo {"
    LINE "  Position: {$py, $px}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 20);
    test_int(ptr->y, 10);

    ecs_fini(world);
}

void Eval_prefab_w_slot(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "prefab Turret {"
    LINE "  slot Base {}"
    LINE "  slot Cannon {}"
    LINE "}"
    LINE ""
    LINE "inst : Turret";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t turret = ecs_lookup(world, "Turret");
    test_assert(turret != 0);
    test_assert(ecs_has_id(world, turret, EcsPrefab));
    
    ecs_entity_t base = ecs_lookup(world, "Turret.Base");
    test_assert(base != 0);
    test_assert(ecs_has_id(world, base, EcsPrefab));
    test_assert(ecs_has_pair(world, base, EcsSlotOf, turret));

    ecs_entity_t cannon = ecs_lookup(world, "Turret.Cannon");
    test_assert(cannon != 0);
    test_assert(ecs_has_id(world, cannon, EcsPrefab));
    test_assert(ecs_has_pair(world, cannon, EcsSlotOf, turret));

    ecs_entity_t inst = ecs_lookup(world, "inst");
    test_assert(inst != 0);
    test_assert(ecs_has_pair(world, inst, EcsIsA, turret));

    ecs_entity_t inst_base = ecs_lookup(world, "inst.Base");
    ecs_entity_t inst_cannon = ecs_lookup(world, "inst.Cannon");
    test_assert(inst_base != 0);
    test_assert(inst_cannon != 0);

    test_assert(ecs_has_pair(world, inst, base, inst_base));
    test_assert(ecs_has_pair(world, inst, cannon, inst_cannon));

    ecs_fini(world);
}

void Eval_prefab_w_slot_variant(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "prefab Turret {"
    LINE "  slot Head {}"
    LINE "}"
    LINE "prefab Variant : Turret {"
    LINE "  prefab Head {"
    LINE "    Foo"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t head = ecs_lookup(world, "Turret.Head");
    ecs_entity_t variant = ecs_lookup(world, "Variant");
    test_assert(head != 0);
    test_assert(variant != 0);

    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, variant);
    ecs_entity_t inst_head = ecs_get_target(world, inst, head, 0);
    ecs_entity_t inst_head_lookup = ecs_lookup_child(world, inst, "Head");
    test_assert(inst_head != 0);
    test_assert(inst_head_lookup != 0);
    test_assert(inst_head == inst_head_lookup);

    ecs_fini(world);
}

void Eval_const_w_component_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "const pos = e[Position]"
    LINE "foo {"
    LINE "  $pos"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_w_component_expr_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "parent {"
    LINE "  const pos = e[Position]"
    LINE "  foo {"
    LINE "    $pos"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_w_component_expr_in_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "module parent"
    LINE "const pos = e[Position]"
    LINE "foo {"
    LINE "  $pos"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    test_assert(parent != 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_w_component_in_scope_expr_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position", .parent = parent }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "parent {"
    LINE "  const pos = e[Position]"
    LINE "  foo {"
    LINE "    $pos"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_w_component_in_scope_expr_in_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position", .parent = parent }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "module parent"
    LINE "const pos = e[Position]"
    LINE "foo {"
    LINE "  $pos"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_w_component_and_entity_in_scope_expr_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position", .parent = parent }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e", .parent = parent });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "parent {"
    LINE "  const pos = e[Position]"
    LINE "  foo {"
    LINE "    $pos"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_const_w_component_and_entity_in_scope_expr_in_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t parent = ecs_entity(world, { .name = "parent" });
    test_assert(parent != 0);

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position", .parent = parent }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e", .parent = parent });
    ecs_set(world, e, Position, {10, 20});

    const char *expr =
    HEAD "module parent"
    LINE "const pos = e[Position]"
    LINE "foo {"
    LINE "  $pos"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_path_tag_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Hello.World" });
    test_assert(tag != 0);

    const char *expr =
    HEAD "parent {"
    LINE "  foo {"
    LINE "    Hello.World"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "parent.Hello.World") == 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Eval_path_tag_in_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Hello.World" });
    test_assert(tag != 0);

    const char *expr =
    HEAD "module parent"
    LINE "foo {"
    LINE "  Hello.World"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "parent.Hello.World") == 0);

    ecs_entity_t foo = ecs_lookup(world, "parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Eval_path_tag_in_nested_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Hello.World" });
    test_assert(tag != 0);

    const char *expr =
    HEAD "Hello.parent {"
    LINE "  foo {"
    LINE "    Hello.World"
    LINE "  }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "parent.Hello.World") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Hello.parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Eval_path_tag_in_nested_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tag = ecs_entity(world, { .name = "Hello.World" });
    test_assert(tag != 0);

    const char *expr =
    HEAD "module Hello.parent"
    LINE "foo {"
    LINE "  Hello.World"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Hello.parent.Hello") == 0);
    test_assert(ecs_lookup(world, "Hello.parent.Hello.World") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Hello.parent.foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Eval_dont_inherit_script_pair(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE "Bar {}"
    LINE "prefab Foo {"
    LINE "  Bar"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(foo != 0);
    test_assert(bar != 0);

    ecs_entity_t e = ecs_new(world);
    ecs_add_pair(world, e, EcsIsA, foo);

    test_assert(ecs_has_pair(world, foo, ecs_id(EcsScript), s));
    test_assert(ecs_has_pair(world, bar, ecs_id(EcsScript), s));
    test_assert(!ecs_has_pair(world, e, ecs_id(EcsScript), s));
    test_assert(ecs_has_id(world, e, bar));

    ecs_fini(world);
}

void Eval_update_script_w_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "_ { Position: {x: 10, y: 20} }"
    });
    test_assert(s != 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_script_update(world, s, 0, "_ { Position: {x: 30, y: 40} }");

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_fini(world);
}

void Eval_update_script_w_anonymous_paren(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "Position(x: 10, y: 20)"
    });
    test_assert(s != 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_script_update(world, s, 0, "Position(x: 30, y: 40)");

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_fini(world);
}

void Eval_clear_script(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "e { Position: {x: 10, y: 20} }"
    });
    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        test_uint(e, it.entities[0]);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_script_clear(world, s, 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_fini(world);
}

void Eval_clear_script_w_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "_ { Position: {x: 10, y: 20} }"
    });
    test_assert(s != 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_script_clear(world, s, 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_fini(world);
}

void Eval_clear_script_w_anonymous_paren(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "Position(x: 10, y: 20)"
    });
    test_assert(s != 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(true, ecs_each_next(&it));
        test_int(1, it.count);
        Position *p = ecs_field(&it, Position, 0);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_script_clear(world, s, 0);

    {
        ecs_iter_t it = ecs_each(world, Position);
        test_bool(false, ecs_each_next(&it));
    }

    ecs_fini(world);
}

void Eval_partial_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "Position foo(x: 10); Position foo(y: 20)"
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 20);

    ecs_fini(world);
}

typedef struct Strings {
    char *a;
    char *b;
} Strings;

static int strings_ctor_invoked = 0;
static int strings_dtor_invoked = 0;
static int strings_move_invoked = 0;
static int strings_copy_invoked = 0;

ECS_CTOR(Strings, ptr, {
    ptr->a = NULL;
    ptr->b = NULL;
    strings_ctor_invoked ++;
});

ECS_DTOR(Strings, ptr, {
    ecs_os_free(ptr->a);
    ecs_os_free(ptr->b);
    strings_dtor_invoked ++;
});

ECS_MOVE(Strings, dst, src, {
    ecs_os_free(dst->a);
    ecs_os_free(dst->b);
    dst->a = src->a;
    dst->b = src->b;
    src->a = NULL;
    dst->a = NULL;
    strings_move_invoked ++;
});

ECS_COPY(Strings, dst, src, {
    ecs_os_free(dst->a);
    ecs_os_free(dst->b);
    dst->a = ecs_os_strdup(src->a);
    dst->b = ecs_os_strdup(src->b);
    strings_copy_invoked ++;
});

void Eval_partial_assign_nontrivial(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Strings);

    ecs_set_hooks(world, Strings, {
        .ctor = ecs_ctor(Strings),
        .dtor = ecs_dtor(Strings),
        .move = ecs_move(Strings),
        .copy = ecs_copy(Strings)
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = "Strings foo(a: \"hello\", b: \"world\"); Strings foo(b: \"bar\")"
    });
    test_assert(s != 0);

    test_int(strings_ctor_invoked, 3);
    test_int(strings_dtor_invoked, 2);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    
    const Strings *p = ecs_get(world, foo, Strings);
    test_assert(p != NULL);
    test_str(p->a, NULL);
    test_str(p->b, "bar");

    ecs_fini(world);

    test_int(strings_ctor_invoked, 3);
    test_int(strings_dtor_invoked, 3);
    test_int(strings_move_invoked, 0);
    test_int(strings_copy_invoked, 0);
}

void Eval_partial_assign_with(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "with Position(x: 10) {"
    LINE "  with Position(y: 20) {"
    LINE "     foo {}"
    LINE "  }"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Eval_partial_assign_nontrivial_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Strings);

    ecs_set_hooks(world, Strings, {
        .ctor = ecs_ctor(Strings),
        .dtor = ecs_dtor(Strings),
        .move = ecs_move(Strings),
        .copy = ecs_copy(Strings)
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "with Strings(a: \"hello\", b: \"world\") {"
    LINE "  with Strings(b: \"bar\") {"
    LINE "     foo {}"
    LINE "     bar {}"
    LINE "  }"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    test_int(strings_ctor_invoked, 4);
    test_int(strings_dtor_invoked, 2);
    test_int(strings_move_invoked, 0);
    test_int(strings_copy_invoked, 4);

    {
        ecs_entity_t e = ecs_lookup(world, "foo");
        test_assert(e != 0);
        
        const Strings *p = ecs_get(world, e, Strings);
        test_assert(p != NULL);
        test_str(p->a, NULL);
        test_str(p->b, "bar");
    }
    {
        ecs_entity_t e = ecs_lookup(world, "bar");
        test_assert(e != 0);
        
        const Strings *p = ecs_get(world, e, Strings);
        test_assert(p != NULL);
        test_str(p->a, NULL);
        test_str(p->b, "bar");
    }

    ecs_fini(world);

    test_int(strings_ctor_invoked, 4);
    test_int(strings_dtor_invoked, 4);
    test_int(strings_move_invoked, 0);
    test_int(strings_copy_invoked, 4);
}

typedef struct LargeArray {
    int8_t values[1000];
} LargeArray;

void Eval_partial_assign_with_large_array(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, LargeArray);

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "LargeArray"}),
        .members = {
            {"values", ecs_id(ecs_i8_t), .count = 1000}
        }
    });

    const char *expr =
    HEAD "with LargeArray(values: [1, 2, 3]) {"
    LINE "  foo {}"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    
    const LargeArray *p = ecs_get(world, foo, LargeArray);
    test_assert(p != NULL);
    test_int(p->values[0], 1);
    test_int(p->values[1], 2);
    test_int(p->values[2], 3);
    for (int i = 3; i < 1000; i ++) {
        test_int(p->values[i], 0);
    }

    ecs_fini(world);
}

void Eval_non_trivial_var_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Strings);

    ecs_set_hooks(world, Strings, {
        .ctor = ecs_ctor(Strings),
        .dtor = ecs_dtor(Strings),
        .move = ecs_move(Strings),
        .copy = ecs_copy(Strings)
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "const val = Strings: {\"hello\", \"world\"}"
    LINE "foo { $val }"
    LINE "bar { $val }"
    LINE "";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    test_int(strings_ctor_invoked, 3);
    test_int(strings_dtor_invoked, 1);
    test_int(strings_copy_invoked, 2);
    test_int(strings_move_invoked, 0);

    {
        ecs_entity_t e = ecs_lookup(world, "foo");
        test_assert(e != 0);
        
        const Strings *p = ecs_get(world, e, Strings);
        test_assert(p != NULL);
        test_str(p->a, "hello");
        test_str(p->b, "world");
    }
    {
        ecs_entity_t e = ecs_lookup(world, "bar");
        test_assert(e != 0);
        
        const Strings *p = ecs_get(world, e, Strings);
        test_assert(p != NULL);
        test_str(p->a, "hello");
        test_str(p->b, "world");
    }

    ecs_fini(world);

    test_int(strings_ctor_invoked, 3);
    test_int(strings_dtor_invoked, 3);
    test_int(strings_copy_invoked, 2);
    test_int(strings_move_invoked, 0);
}

void Eval_non_trivial_var_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Strings);

    ecs_set_hooks(world, Strings, {
        .ctor = ecs_ctor(Strings),
        .dtor = ecs_dtor(Strings),
        .move = ecs_move(Strings),
        .copy = ecs_copy(Strings)
    });

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Strings"}),
        .members = {
            {"a", ecs_id(ecs_string_t)},
            {"b", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "const val = Strings: {\"hello\", \"world\"}"
    HEAD "with $val {"
    LINE "  foo {}"
    LINE "  bar {}"
    LINE "}"
    LINE "";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(s != 0);

    test_int(strings_ctor_invoked, 3);
    test_int(strings_dtor_invoked, 1);
    test_int(strings_copy_invoked, 2);
    test_int(strings_move_invoked, 0);

    {
        ecs_entity_t e = ecs_lookup(world, "foo");
        test_assert(e != 0);
        
        const Strings *p = ecs_get(world, e, Strings);
        test_assert(p != NULL);
        test_str(p->a, "hello");
        test_str(p->b, "world");
    }
    {
        ecs_entity_t e = ecs_lookup(world, "bar");
        test_assert(e != 0);
        
        const Strings *p = ecs_get(world, e, Strings);
        test_assert(p != NULL);
        test_str(p->a, "hello");
        test_str(p->b, "world");
    }

    ecs_fini(world);

    test_int(strings_ctor_invoked, 3);
    test_int(strings_dtor_invoked, 3);
    test_int(strings_copy_invoked, 2);
    test_int(strings_move_invoked, 0);
}
