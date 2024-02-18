#include <addons.h>

#define HEAD
#define LINE "\n"

void Plecs_null(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, NULL) == 0);

    ecs_fini(world);
}

void Plecs_empty(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "") == 0);

    ecs_fini(world);
}

void Plecs_space(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, " ") == 0);

    ecs_fini(world);
}

void Plecs_space_newline(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, " \n \n") == 0);

    ecs_fini(world);
}

void Plecs_two_empty_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n\n") == 0);

    ecs_fini(world);
}

void Plecs_three_empty_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n\n\n") == 0);

    ecs_fini(world);
}

void Plecs_newline_trailing_space(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n ") == 0);

    ecs_fini(world);
}

void Plecs_newline_trailing_spaces(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "\n   ") == 0);

    ecs_fini(world);
}

void Plecs_multiple_trailing_newlines(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\n\n\n") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");
    test_int(ecs_get_type(world, foo)->count, 1);

    ecs_fini(world);
}

void Plecs_entity(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo") == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_str(ecs_get_name(world, foo), "Foo");
    test_int(ecs_get_type(world, foo)->count, 1);

    ecs_fini(world);
}

void Plecs_entity_w_entity(void) {
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

void Plecs_entity_w_pair(void) {
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

void Plecs_2_entities(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\nBar\n") == 0);

    ecs_entity_t e = ecs_lookup(world, "Foo");
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), "Foo");

    ecs_fini(world);
}

void Plecs_2_entities_w_entities(void) {
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

void Plecs_3_entities_w_pairs(void) {
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

void Plecs_line_comment(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "// Foo(Bar)\n") == 0);

    test_assert(ecs_lookup(world, "Foo") == 0);
    test_assert(ecs_lookup(world, "Bar") == 0);

    ecs_fini(world);
}

void Plecs_line_comment_before_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "// Hello(World)\nFoo\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Plecs_line_comment_after_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\n// Hello(World)\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Plecs_line_comment_between_stmt(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo\n// Hello(World)\nBar\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_multiple_line_comment(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "// Hello(World)\n// Boo(Baz)\nFoo") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Boo") == 0);
    test_assert(ecs_lookup(world, "Baz") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Plecs_line_comment_after_stmt_same_line(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo // Hello(World)\nBar\n") == 0);

    test_assert(ecs_lookup(world, "Hello") == 0);
    test_assert(ecs_lookup_child(world, 0, "World") == 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,Hello,Worlds") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred_w_subj(void) {
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

void Plecs_comma_separated_pred_w_subj_obj(void) {
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

void Plecs_comma_separated_pred_trailing_comma(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,Hello,Worlds,") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred_trailing_comma_newline(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,Hello,Worlds,\n") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_comma_separated_pred_trailing_comma_newline_multiline(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_plecs_from_str(world, NULL, "Foo,Bar,\nHello,Worlds,") == 0);

    test_assert(ecs_lookup(world, "Hello") != 0);
    test_assert(ecs_lookup(world, "Worlds") != 0);
    test_assert(ecs_lookup(world, "Foo") != 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Plecs_hierarchy_1_child(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_hierarchy_2_children(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " ChildA"
    LINE " ChildB"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_1_child_same_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { Child }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_hierarchy_2_children_same_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent { ChildA, ChildB }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_entity_after_hierarchy(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_newline_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "{"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_lookup(world, "Child") == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child = ecs_lookup(world, "Parent.Child");

    test_assert(parent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_pair(world, child, EcsChildOf, parent));

    ecs_fini(world);
}

void Plecs_comment_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent // Some(Comment)"
    LINE "{"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_comment_after_newline_before_scope_open(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent"
    LINE "// Some(Comment)"
    LINE "{"
    LINE " Child"
    LINE "}"
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child {"
    LINE "  GrandChild"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_2_levels_2_subtrees(void) {
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

void Plecs_missing_end_of_scope(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " Child";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    test_assert(ecs_get_scope(world) == 0);
    test_assert(ecs_get_with(world) == 0);

    ecs_fini(world);
}

void Plecs_missing_end_of_predicate_scope(void) {
    ecs_log_set_level(-4);
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent() {"
    LINE " Child";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    test_assert(ecs_get_scope(world) == 0);
    test_assert(ecs_get_with(world) == 0);

    ecs_fini(world);
}

void Plecs_create_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Root);

    ecs_set_scope(world, Root);

    const char *expr =
    HEAD "Parent { Child }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_w_pred_subj(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo(Bar) {"
    LINE " Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_custom_relation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE " Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_custom_relation_2_levels(void) {
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

void Plecs_entity_after_hierarchy_custom_relation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Foo, Bar) {"
    LINE " Child"
    LINE "}"
    LINE "Hello";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_entity_after_hierarchy_custom_relation_2_levels(void) {
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

void Plecs_hierarchy_custom_relation_apply_to_object(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "(Rel, ObjA) {"
    LINE " (Rel, ObjB) {"
    LINE "   ObjC"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_hierarchy_custom_relation_apply_to_object_2_levels(void) {
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

void Plecs_pred_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo() {"
    LINE " Hello"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Foo.Hello");

    test_assert(foo != 0);
    test_assert(hello != 0);

    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, hello, foo));

    ecs_fini(world);
}

void Plecs_pred_scope_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo() {"
    LINE "  Bar() {"
    LINE "    Hello"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Bar.Hello");

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

void Plecs_pred_scope_inside_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag {"
    HEAD " Foo() {"
    LINE "  Hello"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tag = ecs_lookup(world, "Tag");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Foo.Hello");

    test_assert(foo != 0);
    test_assert(hello != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, hello, foo));
    test_assert(ecs_has_id(world, hello, tag));

    test_assert(!ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Plecs_pred_scope_nested_w_subj_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    HEAD " Foo() {"
    LINE "  Hello"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t hello = ecs_lookup(world, "Foo.Hello");

    test_assert(parent != 0);
    test_assert(foo != 0);
    test_assert(hello != 0);

    test_assert(!ecs_has_pair(world, hello, EcsChildOf, parent));
    test_assert(ecs_has_pair(world, hello, EcsChildOf, foo));
    test_assert(!ecs_has_id(world, hello, foo));

    test_assert(!ecs_has_pair(world, foo, EcsChildOf, parent));

    ecs_fini(world);
}


void Plecs_with_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag {"
    LINE "  Foo"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Plecs_with_tag_2_entities(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag {"
    LINE "  Foo"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_with_tag_same_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag { Foo }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, foo, tag));

    ecs_fini(world);
}

void Plecs_with_tag_2_entities_same_line(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with Tag { Foo, Bar }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_with_tag_2_levels(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with TagA {"
    HEAD " with TagB {"
    LINE "  Foo"
    LINE " }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_with_tag_2_levels_2_subtrees(void) {
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

void Plecs_with_n_tags(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with TagA, TagB {"
    LINE " Foo"
    LINE " Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_with_n_tags_2_levels(void) {
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

void Plecs_with_after_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "E1 { }"
    HEAD "with E2 {"
    LINE "  E3() { }"
    LINE "  E4 { }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_with_after_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "with E1 { }"
    HEAD "with E2 {"
    LINE "  E3() { }"
    LINE "  E4 { }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_scope_inside_with_inside_scope(void) {
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

void Plecs_with_inside_scope(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth {"
    LINE "  with Continent {"
    LINE "    Europe"
    LINE "  }"
    LINE "  Europe"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assignment_w_1(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth :- Planet";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup(world, "Earth");
    ecs_entity_t planet = ecs_lookup(world, "Planet");

    test_assert(earth != 0);
    test_assert(planet != 0);

    test_assert( ecs_has_id(world, earth, planet));

    ecs_fini(world);
}

void Plecs_assignment_w_2(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth :- Planet, Habitable";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup(world, "Earth");
    ecs_entity_t planet = ecs_lookup(world, "Planet");
    ecs_entity_t habitable = ecs_lookup(world, "Habitable");

    test_assert(earth != 0);
    test_assert(planet != 0);
    test_assert(habitable != 0);

    test_assert( ecs_has_id(world, earth, planet));
    test_assert( ecs_has_id(world, earth, habitable));

    ecs_fini(world);
}

void Plecs_assignment_w_pair(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth :- Planet, (ChildOf, Sun)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t earth = ecs_lookup(world, "Sun.Earth");
    ecs_entity_t planet = ecs_lookup(world, "Planet");
    ecs_entity_t sun = ecs_lookup(world, "Sun");

    test_assert(earth != 0);
    test_assert(planet != 0);
    test_assert(sun != 0);

    test_assert( ecs_has_id(world, earth, planet));
    test_assert( ecs_has_pair(world, earth, EcsChildOf, sun));

    ecs_fini(world);
}

void Plecs_assignment_w_invalid_subject(void) {
    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth :- Planet(Mars)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assignment_w_invalid_with(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Earth :- with Planet { Mars }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_inherit_w_colon(void) {    
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Foo : Bar";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));

    ecs_fini(world);
}

void Plecs_inherit_w_colon_w_scope(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Foo : Bar { Child }";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t child = ecs_lookup(world, "Foo.Child");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));
    test_assert(ecs_has_pair(world, child, EcsChildOf, foo));

    ecs_fini(world);
}

void Plecs_inherit_w_colon_w_assign(void) {
    ecs_world_t *world = ecs_init();
    
    const char *expr =
    HEAD "Foo : Bar :- Comp";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t comp = ecs_lookup(world, "Comp");

    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert(ecs_has_pair(world, foo, EcsIsA, bar));
    test_assert(ecs_has_id(world, foo, comp));

    ecs_fini(world);
}

void Plecs_assign_component_value(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo :- Position{10, 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_assign_2_component_values(void) {
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
    HEAD "Foo :- Position{10, 20}"
    LINE "Foo :- Velocity {1, 2}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_assign_component_value_in_assign_scope(void) {
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
    LINE " - Position {10, 20}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");

    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);

    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_assign_2_component_values_in_assign_scope(void) {
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
    LINE " - Position{10, 20}"
    LINE " -Velocity {1, 2}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct Position {"
    LINE "  x :- flecs.meta.Member{flecs.meta.f32}"
    LINE "  y :- flecs.meta.Member{flecs.meta.f32}"
    LINE "}"
    LINE
    LINE "Foo :- Position{10, 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs_w_2_members(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct Position {"
    LINE "  x :- flecs.meta.Member{flecs.meta.f32}"
    LINE "  y :- flecs.meta.Member{flecs.meta.f32}"
    LINE "}"
    LINE ""
    LINE "Foo :- Position{x: 10, y: 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs_w_3_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        float x;
        float y;
        float z;
    } Position3D;

    const char *expr =
    HEAD "flecs.meta.Struct Position {"
    LINE "  x :- flecs.meta.Member{flecs.meta.f32}"
    LINE "  y :- flecs.meta.Member{flecs.meta.f32}"
    LINE "  z :- flecs.meta.Member{flecs.meta.f32}"
    LINE "}"
    LINE ""
    LINE "Foo :- Position{x: 10, y: 20, z: 30}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs_w_enum(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    typedef struct {
        Color value;
    } SomeType;

    const char *expr =
    HEAD "flecs.meta.Enum Color {"
    LINE "  flecs.meta.Constant Red"
    LINE "  flecs.meta.Constant Green"
    LINE "  flecs.meta.Constant Blue"
    LINE "}"
    LINE ""
    LINE "flecs.meta.Struct SomeType {"
    LINE "  value :- flecs.meta.Member{Color}"
    LINE "}"
    LINE ""
    LINE "Foo :- SomeType{value: Blue}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs_w_enum_using_meta(void) {
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
    LINE "Enum Color {"
    LINE "  Constant Red"
    LINE "  Constant Green"
    LINE "  Constant Blue"
    LINE "}"
    LINE
    LINE "Struct SomeType {"
    LINE "  value :- Member{Color}"
    LINE "}"
    LINE
    LINE "Foo :- SomeType{value: Blue}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs_w_enum_primitive_using_meta(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE "Enum Color {"
    LINE "  Constant Red"
    LINE "  Constant Green"
    LINE "  Constant Blue"
    LINE "}"
    LINE ""
    LINE "Foo :- Color{Blue}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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


void Plecs_type_and_assign_in_plecs_w_enum_primitive_and_struct(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE 
    LINE "Struct Position {"
    LINE "  x :- Member{f32}"
    LINE "  y :- Member{f32}"
    LINE "}"
    LINE 
    LINE "Enum Color {"
    LINE "  Constant Red"
    LINE "  Constant Green"
    LINE "  Constant Blue"
    LINE "}"
    LINE 
    LINE "Foo {"
    LINE "  - Position {x: 10, y: 20}"
    LINE "  - Color {Green}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_type_and_assign_in_plecs_nested_member(void) {
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
    LINE "Struct Line {"
    LINE "  Member start {"
    LINE "    x :- Member{f32}"
    LINE "    y :- Member{f32}"
    LINE "  }"
    LINE "  Member stop {"
    LINE "    x :- Member{f32}"
    LINE "    y :- Member{f32}"
    LINE "  }"
    LINE "}"
    LINE
    LINE "l :- Line{start: {x: 10, y: 20}, stop: {x: 30, y: 40}}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_dot_assign_nested_member(void) {
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
    LINE "Struct Line {"
    LINE "  Member start {"
    LINE "    x :- Member{f32}"
    LINE "    y :- Member{f32}"
    LINE "  }"
    LINE "  Member stop {"
    LINE "    x :- Member{f32}"
    LINE "    y :- Member{f32}"
    LINE "  }"
    LINE "}"
    LINE
    LINE "l :- Line{start.x: 10, start.y: 20, stop.x: 30, stop.y: 40}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_dot_assign_binary_expr(void) {
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
    LINE "Struct Line {"
    LINE "  Member start {"
    LINE "    x :- Member{f32}"
    LINE "    y :- Member{f32}"
    LINE "  }"
    LINE "  Member stop {"
    LINE "    x :- Member{f32}"
    LINE "    y :- Member{f32}"
    LINE "  }"
    LINE "}"
    LINE
    LINE "l :- Line{start.x: 2 + 8, start.y: 5 + 15, stop.x: 10 + 20, stop.y: 15 + 25}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_open_scope_no_parent(void) {
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

void Plecs_create_subject_in_root_scope_w_resolvable_id(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Tag";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t tag = ecs_lookup_child(world, 0, "Tag");
    test_assert(tag != 0);
    test_assert(tag != EcsTag);

    ecs_fini(world);
}

void Plecs_create_subject_in_scope_w_resolvable_id(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Tag"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t tag = ecs_lookup(world, "Foo.Tag");
    test_assert(tag != 0);
    test_assert(tag != EcsTag);

    test_assert(ecs_has_pair(world, tag, EcsChildOf, foo));

    ecs_fini(world);
}

void Plecs_create_subject_in_scope_w_resolvable_id_using(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  Bar"
    LINE "}"
    LINE
    LINE "using Foo"
    LINE
    LINE "Hello(Bar)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_using_scope(void) {
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
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t not_bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup(world, "World");

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

void Plecs_using_nested_scope(void) {
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
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup(world, "Foo.Bar.Zoo");
    ecs_entity_t not_bar = ecs_lookup(world, "Bar");
    ecs_entity_t not_zoo = ecs_lookup(world, "Zoo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup(world, "World");

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

void Plecs_using_nested_in_scope(void) {
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
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Foo.Bar");
    ecs_entity_t zoo = ecs_lookup(world, "Foo.Bar.Zoo");
    ecs_entity_t not_bar = ecs_lookup(world, "Bar");
    ecs_entity_t zoo_root = ecs_lookup(world, "Zoo");
    ecs_entity_t hello = ecs_lookup(world, "Hello");
    ecs_entity_t _world = ecs_lookup(world, "World");

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

void Plecs_using_with_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE ""
    LINE "Scope {"
    LINE ""
    LINE "Foo {}"
    LINE ""
    LINE "Struct Bar {"
    LINE "  x :- Member{f32}"
    LINE "}"
    LINE ""
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_using_w_entity_ref_in_value_2_members(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct Position {"
    LINE "  x :- Member{f32}" // member type is looked up in flecs.meta
    LINE "  y :- Member{f32}"
    LINE "}"
    LINE ""
    LINE "Foo :- Position{x: 10, y: 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_using_w_entity_ref_in_value_3_members(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        float x;
        float y;
        float z;
    } Position3D;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct Position {"
    LINE "  x :- Member{f32}" // member type is looked up in flecs.meta
    LINE "  y :- Member{f32}"
    LINE "  z :- Member{f32}"
    LINE "}"
    LINE
    LINE "Foo :- Position{x: 10, y: 20, z: 30}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_2_using_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using Foo"
    LINE "using Bar"
    LINE
    LINE "Foo {"
    LINE "  Hello"
    LINE "}"
    LINE
    LINE "Bar {"
    LINE "  TheWorld"
    LINE "}"
    LINE
    LINE "Hello(E1)"
    LINE "TheWorld(E2)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_2_using_in_different_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "Foo {"
    LINE "  Hello"
    LINE "}"
    LINE
    LINE "Bar {"
    LINE "  TheWorld"
    LINE "}"
    LINE
    LINE "E1 {"
    LINE "  using Foo"
    LINE
    LINE "  Hello(Child)"
    LINE "  TheWorld(Child)"
    LINE "}"
    LINE
    LINE "E2 {"
    LINE "  using Bar"
    LINE
    LINE "  Hello(Child)"
    LINE "  TheWorld(Child)"
    LINE "}"
    LINE
    LINE "Hello(RootChild)"
    LINE "TheWorld(RootChild)"
    LINE;

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_assignment_to_non_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo :- Position{x: 10, y: 20}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_struct_w_member_w_assignment_to_nothing(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct Position {"
    LINE "  flecs.meta.Member(x) = "
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_struct_w_member_w_assignment_to_empty_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Struct Position {"
    LINE "  x :- flecs.meta.Member{"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_scope_after_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo :- Position{10, 20} {"
    LINE "  Bar"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assign_after_inherit(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Foo : Position"
    LINE "Bar :- Position{10, 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_multiple_assignments_single_line(void) {
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
    HEAD "Foo :- Position{10, 20}, Velocity{1, 2}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_2_stmts_in_scope_w_no_parent(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "{"
    LINE "Bar { }"
    LINE "Foo"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_scope_after_assign_1_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo :- TagA {"
    LINE "  Bar"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_scope_after_assign_2_tags(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo :- TagA, TagB {"
    LINE "  Bar"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_empty_scope_after_using(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE "{"
    LINE "  Foo"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert( foo != 0);
    test_assert( !ecs_has_pair(world, foo, EcsChildOf, EcsWildcard));

    ecs_fini(world);
}

void Plecs_invalid_nested_assignment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo :-"
    LINE "Bar :- Hello";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_invalid_partial_pair_assignment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo :- (Hello, ";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_empty_assignment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo :-";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_empty_assignment_before_end_of_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "{Foo :-}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assign_tag_to_parent(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  - Bar"
    LINE "  Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_component_to_parent(void) {
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
    LINE "  - Position {10, 20}"
    LINE "  Child"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_to_parent_pair_w_new_entities_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {"
    LINE "  - (Rel, Obj)"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_to_parent_pair_w_existing_entities_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Rel, Obj"
    LINE "Foo {"
    LINE "  - (Rel, Obj)"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_default_child_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "DefaultChildComponent(Foo, Bar)"
    LINE "Foo(Parent) {"
    LINE "  ChildA"
    LINE "  ChildB"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

    test_assert( ecs_has_pair(world, foo, EcsDefaultChildComponent, bar));
    test_assert( ecs_has_id(world, parent, foo));
    test_assert( ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert( ecs_has_pair(world, child_b, EcsChildOf, parent));
    
    test_assert(ecs_has_id(world, child_a, bar));
    test_assert(ecs_has_id(world, child_b, bar));

    ecs_fini(world);
}

void Plecs_default_child_component_w_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "DefaultChildComponent(Foo, Position)"
    LINE "Foo(Parent) {"
    LINE "  ChildA :- {10, 20}"
    LINE "  ChildB :- {10, 20}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    
    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t child_a = ecs_lookup(world, "Parent.ChildA");
    ecs_entity_t child_b = ecs_lookup(world, "Parent.ChildB");

    test_assert(foo != 0);
    test_assert(parent != 0);
    test_assert(child_a != 0);
    test_assert(child_b != 0);

    test_assert( ecs_has_pair(world, foo, EcsDefaultChildComponent, ecs_id(Position)));
    test_assert( ecs_has_id(world, parent, foo));
    test_assert( ecs_has_pair(world, child_a, EcsChildOf, parent));
    test_assert( ecs_has_pair(world, child_b, EcsChildOf, parent));
    
    test_assert(ecs_has(world, child_a, Position));
    test_assert(ecs_has(world, child_b, Position));

    ecs_fini(world);
}

void Plecs_struct_type_w_default_child_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  x :- {f32}"
    LINE "  y :- {f32}"
    LINE "}"
    LINE
    LINE "Foo :- Position{x: 10, y: 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ecs_id(Position) = ecs_lookup(world, "Position");

    test_assert(foo != 0);
    test_assert(ecs_id(Position) != 0);

    test_assert( ecs_has(world, foo, Position));

    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_struct_type_w_default_child_component_nested_member(void) {
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
    LINE "Struct(Line) {"
    LINE "  start {"
    LINE "    x :- {f32}"
    LINE "    y :- {f32}"
    LINE "  }"
    LINE "  stop {"
    LINE "    x :- {f32}"
    LINE "    y :- {f32}"
    LINE "  }"
    LINE "}"
    LINE
    LINE "Foo :- Line{start: {10, 20}, stop: {30, 40}}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_enum_type_w_default_child_component(void) {
    ecs_world_t *world = ecs_init();

    typedef enum {
        Red, Green, Blue
    } Color;

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Enum Color {"
    LINE "  Red, Green, Blue"
    LINE "}"
    LINE
    LINE "Foo :- Color{Green}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_default_type_from_with(void) {
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
    LINE "  e1 :- {10, 20}"
    LINE "  e2 :- {30, 40}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_scope_w_1_subj_and_2_pairs(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE " RelA(Foo, Bar)"
    LINE " RelB(Foo, Bar)"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    ecs_entity_t rel_a = ecs_lookup(world, "RelA");
    ecs_entity_t rel_b = ecs_lookup(world, "RelB");
    ecs_entity_t foo = ecs_lookup(world, "Parent.Foo");
    ecs_entity_t bar = ecs_lookup(world, "Parent.Bar");

    test_assert(parent != 0);
    test_assert(rel_a != 0);
    test_assert(rel_b != 0);
    test_assert(foo != 0);
    test_assert(bar != 0);

    test_assert( ecs_has_pair(world, foo, rel_a, bar));
    test_assert( ecs_has_pair(world, foo, rel_b, bar));

    ecs_fini(world);
}

void Plecs_inherit_from_multiple(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Inst : Foo, Bar";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

// using flecs.meta
// Struct(Position) {
//   x :- {f32}
//   y :- {f32}
// }
// Foo :- (Position, Bar){x: 10, y: 20}

void Plecs_assign_pair_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  x :- {f32}"
    LINE "  y :- {f32}"
    LINE "}"
    LINE
    LINE "Foo :- (Position, Bar){x: 10, y: 20}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_pair_component_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  x :- {f32}"
    LINE "  y :- {f32}"
    LINE "}"
    LINE
    LINE "Parent {"
    LINE "  - (Position, Foo) {x: 10, y: 20}"
    LINE "  - (Position, Bar) {x: 20, y: 30}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_pair_component_in_script(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  x :- {f32}"
    LINE "  y :- {f32}"
    LINE "}"
    LINE
    LINE "Foo :- (Position, Bar){x: 10, y: 20}";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .str = expr
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

void Plecs_assign_pair_component_in_script_update(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE
    LINE "Struct(Position) {"
    LINE "  x :- {f32}"
    LINE "  y :- {f32}"
    LINE "}"
    LINE
    LINE "Foo :- (Position, Bar){x: 10, y: 20}";

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .str = expr
    });
    test_assert(s != 0);

    test_assert(ecs_script_update(world, s, 0, expr, NULL) == 0);

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

void Plecs_set_entity_names(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "foo(self)"
    LINE "foo(parent)"
    LINE "foo(cascade)"
    LINE "foo(down)"
    LINE "foo(up)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
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

void Plecs_oneof(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Enum(Color) {"
    LINE "  flecs.meta.Constant(Red)"
    LINE "  flecs.meta.Constant(Green)"
    LINE "  flecs.meta.Constant(Blue)"
    LINE "}"
    LINE "e :- (Color, Green)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_invalid_oneof(void) {
    ecs_log_set_level(-4);
    
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "flecs.meta.Enum(Color) {"
    LINE "  flecs.meta.Constant(Red)"
    LINE "  flecs.meta.Constant(Green)"
    LINE "  flecs.meta.Constant(Blue)"
    LINE "}"
    LINE "e :- (Color, Foo)";

    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_entity_t color = ecs_lookup(world, "Color");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e = ecs_lookup(world, "e");

    test_assert(color != 0);
    test_assert(foo == 0);
    test_assert(e != 0);

    test_assert( !ecs_has_pair(world, e, color, EcsWildcard));

    ecs_fini(world);
}

void Plecs_brief_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief A description"
    LINE "Foo"
    LINE "Bar"
    LINE ""
    LINE "@brief Another description"
    LINE "Baz";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_name_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@name A name"
    LINE "Foo"
    LINE "Bar"
    LINE ""
    LINE "@name Another name"
    LINE "Baz";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_link_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@link A link"
    LINE "Foo"
    LINE "Bar"
    LINE ""
    LINE "@link Another link"
    LINE "Baz";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_color_annotation(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@color #554433"
    LINE "Foo"
    LINE "Bar"
    LINE ""
    LINE "@color rgb(10, 20, 30, 1.0)"
    LINE "Baz";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_multiple_annotations(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief A description"
    LINE "@name A name"
    LINE "@link A link"
    LINE "@color #554433"
    LINE "Foo"
    LINE "Bar"
    LINE ""
    LINE "@brief Another description"
    LINE "@name Another name"
    LINE "Baz";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_annotation_w_trailing_space(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief   A description  "
    LINE "Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    test_str(ecs_doc_get_brief(world, foo), "A description");

    ecs_fini(world);
}

typedef struct String {
    char *value;
} String;

void Plecs_multiline_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(String) = ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "String"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "Foo :- String{value: `start"
    LINE "Hello World"
    LINE "Foo Bar"
    LINE "Special characters }{\"\"'',"
    LINE "`}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_unterminated_multiline_string(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct_init(world, &(ecs_struct_desc_t){
        .entity = ecs_entity(world, {.name = "String"}),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "Foo :- String{value: `start"
    LINE "Hello World"
    LINE "Foo Bar"
    LINE "Special characters }{\"\"'',"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_annotate_declaration(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@brief A brief description"
    LINE "Foo Bar";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_declaration_w_underscore_name(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo _Bar";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "_Bar");
    test_assert(bar != 0);

    test_assert(!ecs_has_id(world, foo, bar));
    test_assert(ecs_has_id(world, bar, foo));

    ecs_fini(world);
}

void Plecs_anonymous_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "_ :- Foo";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ foo });
    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);
    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Plecs_anonymous_entity_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Parent {"
    LINE "  _ :- Foo"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t parent = ecs_lookup(world, "Parent");
    test_assert(parent != 0);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ foo });
    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);
    test_assert(ecs_has_pair(world, e, EcsChildOf, parent));
    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Plecs_anonymous_declaration(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo _";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_iter_t it = ecs_term_iter(world, &(ecs_term_t){ foo });
    test_assert(ecs_term_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(e != 0);
    test_str(ecs_get_name(world, e), NULL);
    test_assert(!ecs_term_next(&it));

    ecs_fini(world);
}

void Plecs_const_var_int(void) {
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
    LINE "e :- Position{$var_x, $var_y}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_const_var_float(void) {
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
    LINE "e :- Position{$var_x, $var_y}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_flt(p->x, 10.5);
    test_flt(p->y, 20.5);

    ecs_fini(world);
}

void Plecs_const_var_bool(void) {
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
    LINE "e :- Bools{$var_x, $var_y}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Bools));

    const Bools *p = ecs_get(world, e, Bools);
    test_assert(p != NULL);
    test_bool(p->x, true);
    test_bool(p->y, false);

    ecs_fini(world);
}

void Plecs_const_var_string(void) {
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
    LINE "e :- Position{$var_x, $var_y}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_flt(p->x, 10.5);
    test_flt(p->y, 20.5);

    ecs_fini(world);
}

typedef struct Line {
    Position start;
    Position stop;
} Line;

void Plecs_const_var_struct(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t ecs_id(Line) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Line"}),
        .members = {
            {"start", ecs_id(Position)},
            {"stop", ecs_id(Position)}
        }
    });

    const char *expr =
    HEAD "const var_start = Position{10.5, 20.5}"
    LINE "const var_stop = Position{30.5, 40.5}"
    LINE ""
    LINE "e :- Line{start: $var_start, stop: $var_stop}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_const_var_redeclare(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "const var_x = 10"
    LINE "const var_x = 20";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_const_var_scoped(void) {
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
    LINE "a :- Position{$var_x, $var_x}"
    LINE "a {"
    LINE "  const var_x = 20"
    LINE "  const var_y = 30"
    LINE "  b :- Position{$var_x, $var_y}"
    LINE "}"
    LINE "a {"
    LINE "  const var_y = 20"
    LINE "  c :- Position{$var_x, $var_y}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_component_from_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos = Position{10, 20}"
    LINE "a :- $var_pos"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_component_from_var_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos = Position{10, 20}"
    LINE "a {"
    LINE " - $var_pos"
    LINE "}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_scope_w_component_after_const_var(void) {
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
    LINE "  - Position{x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 5);

    ecs_fini(world);
}

void Plecs_component_after_const_add_expr(void) {
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
    LINE "  - Position{x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_component_after_const_sub_expr(void) {
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
    LINE "  - Position{x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_component_after_const_mul_expr(void) {
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
    LINE "  - Position{x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_component_after_const_div_expr(void) {
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
    LINE "  - Position{x: 10, y: $var}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_component_after_const_paren_expr(void) {
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
    LINE "  - Position{$val, $val * 2}"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}

void Plecs_parse_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo"
    LINE "Bar {"
    LINE "  Hello"
    LINE "}";

    ECS_TAG(world, Tag);

    ecs_set_with(world, Tag);
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_parse_with_w_with(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const char *expr =
    HEAD "Foo"
    LINE "with TagB {"
    LINE "  Bar {"
    LINE "    Hello"
    LINE "  }"
    LINE "}";

    ecs_set_with(world, TagA);
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
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

void Plecs_parse_with_w_tag(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    const char *expr =
    HEAD "Foo"
    LINE "with TagB {"
    LINE "  Bar {"
    LINE "    - Hello"
    LINE "  }"
    LINE "}";

    ecs_set_with(world, TagA);
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);
    ecs_set_with(world, 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t hello = ecs_lookup(world, "Hello");

    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(hello != 0);
    
    test_assert(ecs_has(world, foo, TagA));
    test_assert(ecs_has(world, bar, TagA));
    test_assert(ecs_has(world, hello, TagA));
    test_assert(ecs_has_id(world, bar, hello));

    test_assert(!ecs_has(world, TagB, TagA));
    test_assert(!ecs_has(world, foo, TagB));
    test_assert(ecs_has(world, bar, TagB));
    test_assert(!ecs_has(world, hello, TagB));

    ecs_fini(world);
}

void Plecs_parse_with_value(void) {
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
    LINE "with Position{10, 20} {"
    LINE "  Foo"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_parse_with_2_values(void) {
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
    LINE "with Position{10, 20}, Velocity{1, 2} {"
    LINE "  Foo"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_parse_with_2_nested_values(void) {
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
    LINE "with Position{10, 20} {"
    LINE "  with Velocity{1, 2} {"
    LINE "    Foo"
    LINE "    Bar"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_parse_with_var(void) {
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
    LINE "const pos : Position = {10, 20}"
    LINE "with $pos {"
    LINE "  Foo"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_parse_with_2_vars(void) {
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
    LINE "const pos : Position = {10, 20}"
    LINE "const vel : Velocity = {1, 2}"
    LINE "with $pos, $vel {"
    LINE "  Foo"
    LINE "  Bar"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_parse_with_2_nested_vars(void) {
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
    LINE "const pos : Position = {10, 20}"
    LINE "const vel : Velocity = {1, 2}"
    LINE "with $pos, $vel {"
    LINE "  with $vel {"
    LINE "    Foo"
    LINE "    Bar"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_parse_with_var_in_scope(void) {
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
    LINE "  const pos : Position = {10, 20}"
    LINE "  with $pos { "
    LINE "    Foo"
    LINE "    Bar"
    LINE "  }"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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


void Plecs_assign_const_w_expr(void) {
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
    LINE "e :- Position{x: $var, y: $var * 2}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 6);
    test_int(p->y, 12);

    ecs_fini(world);
}

void Plecs_const_w_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    LINE "const var : flecs.meta.i32 = 5 / 2"
    LINE "e :- Position{x: $var, y: $var * 3}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Plecs_assembly_no_scope(void) {
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
    LINE "assembly Tree";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_fini(world);
}

void Plecs_assembly_empty(void) {
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
    LINE "assembly Tree {"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_no_props(void) {
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
    LINE "assembly Tree {"
    LINE "  - Foo"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_prop_no_type(void) {
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
    LINE "assembly Tree {"
    LINE "  prop height"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_prop_no_default(void) {
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
    LINE "assembly Tree {"
    LINE "  prop height: flecs.meta.f32"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_prop(void) {
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
    LINE "assembly Tree {"
    LINE "  prop height: flecs.meta.f32 = 0"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Plecs_assembly_prop_space_colon(void) {
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
    LINE "assembly Tree {"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Plecs_assembly_2_props(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.i32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_i32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Plecs_assembly_instance_w_default_values(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 10"
    LINE "  prop height : flecs.meta.f32 = 20"
    LINE "}"
    LINE ""
    LINE "e :- Tree"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 10, height: 20}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Plecs_assembly_instance_w_assign_default_values(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 10"
    LINE "  prop height : flecs.meta.f32 = 20"
    LINE "}"
    LINE ""
    LINE "e :- Tree{}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 10, height: 20}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Plecs_assembly_instance_w_overridden_values(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 10"
    LINE "  prop height : flecs.meta.f32 = 20"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 30, height: 40}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 30, height: 40}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Plecs_assembly_w_child(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child :- Position{$width * 10 + 1, $height * 20 + 2}"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 1, height: 2}");
    ecs_os_free(str);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 42);

    ecs_fini(world);
}

void Plecs_assembly_w_child_parse_script(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child :- Position{$width * 10 + 1, $height * 20 + 2}"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .str = expr
    }) != 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 1, height: 2}");
    ecs_os_free(str);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 42);

    ecs_fini(world);
}

void Plecs_assembly_w_child_parse_script_twice(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child :- Position{$width * 10 + 1, $height * 20 + 2}"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .str = expr
    }) != 0);

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .str = expr
    }) != 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 1, height: 2}");
    ecs_os_free(str);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 42);

    ecs_fini(world);
}

void Plecs_assembly_w_child_update_after_parse(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child :- Position{$width * 10 + 1, $height * 20 + 2}"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .str = expr
    }) != 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);

        ecs_entity_t e = ecs_lookup(world, "e");
        test_assert(e != 0);

        ecs_entity_t child = ecs_lookup(world, "e.child");
        test_assert(child != 0);
    }

    test_assert(!ecs_is_deferred(world));

    const char *expr_update =
    LINE "e :- Tree{width: 3, height: 4}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr_update) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 3, height: 4}");
    ecs_os_free(str);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 31);
    test_int(p->y, 82);

    ecs_fini(world);
}

void Plecs_assembly_w_nested_child(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child {"
    LINE "    - Position{$width, $height}"
    LINE "    grand_child :- Position{$height, $width}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t grand_child = ecs_lookup(world, "e.child.grand_child");
    test_assert(grand_child != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 1, height: 2}");
    ecs_os_free(str);

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    {
        const Position *p = ecs_get(world, grand_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void Plecs_assembly_w_prefab(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  Prefab base {"
    LINE "    - Velocity{$width * 2, $height * 3}"
    LINE "  }"
    LINE "  child : base {"
    LINE "    - Position{$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t base = ecs_lookup(world, "e.base");
    test_assert(base != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 1, height: 2}");
    ecs_os_free(str);

    test_assert(ecs_has_pair(world, child, EcsIsA, base));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    {
        const Velocity *v = ecs_get(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 6);
    }

    {
        const Velocity *v = ecs_get(world, base, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 6);
    }

    ecs_fini(world);
}

void Plecs_assembly_w_prefab_tree(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  Prefab base {"
    LINE "    - Velocity{$width * 2, $height * 3}"
    LINE "    Prefab child {"
    LINE "      - Velocity{$height * 3, $width * 2}"
    LINE "    }"
    LINE "  }"
    LINE "  child : base {"
    LINE "    - Position{$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e :- Tree{width: 1, height: 2}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t base = ecs_lookup(world, "e.base");
    test_assert(base != 0);

    ecs_entity_t base_child = ecs_lookup(world, "e.base.child");
    test_assert(base_child != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 1, height: 2}");
    ecs_os_free(str);

    test_assert(ecs_has_pair(world, child, EcsIsA, base));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }

    {
        const Velocity *v = ecs_get(world, child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 6);
    }

    {
        const Velocity *v = ecs_get(world, base, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 6);
    }

    {
        const Velocity *v = ecs_get(world, base_child, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 6);
        test_int(v->y, 2);
    }

    ecs_fini(world);
}

void Plecs_assembly_w_nested_assembly(void) {
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

    const char *expr =
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child {"
    LINE "    - Position{$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "assembly Forest {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  tree_1 :- Tree{-$width, -$height}"
    LINE "  tree_2 :- Tree{$width + 1, $height + 1}"
    LINE "}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t forest = ecs_lookup(world, "Forest");
    test_assert(forest != 0);
    test_assert(ecs_lookup(world, "Forest.tree_1") != 0);
    test_assert(ecs_lookup(world, "Forest.tree_2") != 0);
    test_assert(ecs_lookup(world, "Forest.tree_1.child") == 0);
    test_assert(ecs_lookup(world, "Forest.tree_2.child") == 0);
    test_assert(ecs_lookup(world, "tree_1") == 0);
    test_assert(ecs_lookup(world, "tree_2") == 0);

    {
        const EcsStruct *st = ecs_get(world, tree, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 2);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));
    }

    {
        const EcsStruct *st = ecs_get(world, forest, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 2);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));
    }

    test_assert(!ecs_is_deferred(world));

    const char *expr_instance =
    LINE "f :- Forest{10, 20}"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr_instance) == 0);

    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);
    ecs_entity_t f_tree_1 = ecs_lookup(world, "f.tree_1");
    test_assert(f_tree_1 != 0);
    ecs_entity_t f_tree_2 = ecs_lookup(world, "f.tree_2");
    test_assert(f_tree_2 != 0);
    ecs_entity_t f_tree_1_child = ecs_lookup(world, "f.tree_1.child");
    test_assert(f_tree_1_child != 0);
    ecs_entity_t f_tree_2_child = ecs_lookup(world, "f.tree_2.child");
    test_assert(f_tree_2_child != 0);

    {
        const void *ptr = ecs_get_id(world, f, forest);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, forest, ptr);
        test_str(str, "{width: 10, height: 20}");
        ecs_os_free(str);
    }
    {
        const void *ptr = ecs_get_id(world, f_tree_1, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{width: -10, height: -20}");
        ecs_os_free(str);
    }
    {
        const void *ptr = ecs_get_id(world, f_tree_2, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{width: 11, height: 21}");
        ecs_os_free(str);
    }

    {
        const Position *p = ecs_get(world, f_tree_1_child, Position);
        test_assert(p != NULL);
        test_int(p->x, -10);
        test_int(p->y, -20);
    }
    {
        const Position *p = ecs_get(world, f_tree_2_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 11);
        test_int(p->y, 21);
    }

    ecs_fini(world);
}

void Plecs_instantiate_prefab_w_assembly(void) {
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
    LINE "assembly Tree {"
    LINE "  prop width : flecs.meta.f32 = 0"
    LINE "  prop height : flecs.meta.f32 = 0"
    LINE "  child :- Position{$width, $height}"
    LINE "}"
    LINE ""
    LINE "Prefab p :- Tree{width: 10, height: 20}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t p = ecs_lookup(world, "p");
    test_assert(p != 0);
    test_assert(ecs_lookup(world, "p.child") == 0);
    test_assert(ecs_lookup(world, "child") == 0);
    
    const char *expr_instance =
    LINE "e : p"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr_instance) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    test_assert(ecs_lookup(world, "e.child") != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    {
        const void *ptr = ecs_get_id(world, p, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{width: 10, height: 20}");
        ecs_os_free(str);
    }

    test_assert(ecs_has_pair(world, e, EcsIsA, p));

    {
        const Position *p = ecs_get(world, child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Plecs_assembly_w_prefab_w_assembly(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "assembly Tree {"
    LINE "  prop count: flecs.meta.f32 = 0"
    LINE "}"
    LINE ""
    LINE "assembly Forest {"
    LINE "  prop count: flecs.meta.f32 = 0"
    LINE ""
    LINE "  Prefab TreePrefab {"
    LINE "    - Tree{count: $count}"
    LINE "  }"
    LINE ""
    LINE "  child : TreePrefab"
    LINE "}"
    LINE "f :- Forest{10}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t forest = ecs_lookup(world, "Forest");
    test_assert(forest != 0);
    ecs_entity_t tree_prefab = ecs_lookup(world, "Forest.TreePrefab");
    test_assert(tree_prefab != 0);

    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);
    ecs_entity_t child = ecs_lookup(world, "f.child");
    test_assert(child != 0);

    {
        const EcsStruct *st = ecs_get(world, tree, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "count");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }
    
    {
        const EcsStruct *st = ecs_get(world, forest, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "count");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }
    
    {
        const void *ptr = ecs_get_id(world, f, forest);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, forest, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }

    {
        const void *ptr = ecs_get_id(world, tree_prefab, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{count: 0}");
        ecs_os_free(str);
    }

    ecs_fini(world);
}

void Plecs_3_assemblies(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "assembly Tree {"
    LINE "  prop t: flecs.meta.f32 = 10"
    LINE "}"
    LINE ""
    LINE "assembly Forest {"
    LINE "  prop f: flecs.meta.f32 = 20"
    LINE "}"
    LINE ""
    LINE "assembly Park {"
    LINE "  prop p: flecs.meta.f32 = 30"
    LINE "}"
    LINE ""
    LINE "a :- Tree{}"
    LINE "b :- Forest{}"
    LINE "c :- Park{}"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t forest = ecs_lookup(world, "Forest");
    test_assert(forest != 0);
    ecs_entity_t park = ecs_lookup(world, "Park");
    test_assert(park != 0);
    
    ecs_entity_t a = ecs_lookup(world, "a");
    test_assert(a != 0);
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(b != 0);
    ecs_entity_t c = ecs_lookup(world, "c");
    test_assert(c != 0);

    {
        const EcsStruct *st = ecs_get(world, tree, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "t");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }  
    {
        const EcsStruct *st = ecs_get(world, forest, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "f");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }
    {
        const EcsStruct *st = ecs_get(world, park, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "p");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }
    
    {
        const void *ptr = ecs_get_id(world, a, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{t: 10}");
        ecs_os_free(str);
    }
    {
        const void *ptr = ecs_get_id(world, b, forest);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, forest, ptr);
        test_str(str, "{f: 20}");
        ecs_os_free(str);
    }
    {
        const void *ptr = ecs_get_id(world, c, park);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, park, ptr);
        test_str(str, "{p: 30}");
        ecs_os_free(str);
    }

    ecs_fini(world);
}

void Plecs_assembly_nested_w_default_var(void) {
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
    LINE "assembly Tree {"
    LINE "  prop count: flecs.meta.f32 = 0"
    LINE "  trunk :- Position{$count, $count * 2}"
    LINE "}"
    LINE ""
    LINE "assembly Forest {"
    LINE "  prop count: flecs.meta.f32 = 0"
    LINE "  child :- Tree{count:$}"
    LINE "}"
    LINE "f :- Forest{10}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t forest = ecs_lookup(world, "Forest");
    test_assert(forest != 0);

    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);
    ecs_entity_t child = ecs_lookup(world, "f.child");
    test_assert(child != 0);
    ecs_entity_t trunk = ecs_lookup(world, "f.child.trunk");
    test_assert(trunk != 0);

    {
        const EcsStruct *st = ecs_get(world, tree, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "count");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }
    
    {
        const EcsStruct *st = ecs_get(world, forest, EcsStruct);
        test_assert(st != NULL);
        test_int(st->members.count, 1);
        test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "count");
        test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    }

    {
        const void *ptr = ecs_get_id(world, f, forest);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, forest, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }
    {
        const void *ptr = ecs_get_id(world, child, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }

    {
        const Position *p = ecs_get(world, trunk, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Plecs_assembly_w_anonymous(void) {
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
    LINE "assembly Tree {"
    LINE "  prop count: flecs.meta.i32 = 0"
    LINE "  _ :- Position{$count, $count * 2}"
    LINE "  _ :- Position{$count, $count * 2}"
    LINE "}"
    LINE ""
    LINE "t :- Tree{10}";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t t = ecs_lookup(world, "t");
    test_assert(t != 0);

    {
        ecs_filter_t *f = ecs_filter(world, {
            .terms = {
                { .id = ecs_childof(tree) },
                { .id = ecs_id(Position) },
                { .id = EcsPrefab },
            }
        });

        ecs_iter_t it = ecs_filter_iter(world, f);
        test_int(2, ecs_iter_count(&it));
        ecs_filter_fini(f);
    }

    {
        ecs_filter_t *f = ecs_filter(world, {
            .terms = {
                { .id = ecs_childof(t) },
                { .id = ecs_id(Position) },
            }
        });

        ecs_iter_t it = ecs_filter_iter(world, f);
        test_int(2, ecs_iter_count(&it));
        ecs_filter_fini(f);
    }

    ecs_fini(world);
}

void Plecs_assembly_w_anonymous_parse_again(void) {
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
    LINE "assembly Tree {"
    LINE "  prop count: flecs.meta.i32 = 0"
    LINE "  _ :- Position{$count, $count * 2}"
    LINE "  _ :- Position{$count, $count * 2}"
    LINE "}"
    LINE ""
    LINE "t :- Tree{10}";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    const char *expr_again =
    LINE "t :- Tree{10}";
    test_assert(ecs_plecs_from_str(world, NULL, expr_again) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t t = ecs_lookup(world, "t");
    test_assert(t != 0);

    {
        ecs_filter_t *f = ecs_filter(world, {
            .terms = {
                { .id = ecs_childof(tree) },
                { .id = ecs_id(Position) },
                { .id = EcsPrefab },
            }
        });

        ecs_iter_t it = ecs_filter_iter(world, f);
        test_int(2, ecs_iter_count(&it));
        ecs_filter_fini(f);
    }

    {
        ecs_filter_t *f = ecs_filter(world, {
            .terms = {
                { .id = ecs_childof(t) },
                { .id = ecs_id(Position) },
            }
        });

        ecs_iter_t it = ecs_filter_iter(world, f);
        test_int(2, ecs_iter_count(&it));
        ecs_filter_fini(f);
    }

    ecs_fini(world);
}

void Plecs_typed_const_w_composite_type_invalid_assignment(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos : Position = Position{10, 20}"
    LINE "a :- $var_pos"
    LINE "";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_typed_const_w_composite_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos : Position = {10, 20}"
    LINE "a :- $var_pos"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_var_to_typed_const_w_composite_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "const var_pos_a : Position = {10, 20}"
    HEAD "const var_pos_b = $var_pos_a"
    LINE "a :- $var_pos_b"
    LINE "";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assembly_w_composite_prop_invalid_assignment(void) {
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
    LINE "assembly Tree {"
    LINE "  prop pos: Position = Position{10, 20}"
    LINE "  child :- $pos"
    LINE "}"
    LINE ""
    LINE "t :- Tree{pos: {20, 30}}";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_w_composite_prop(void) {
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
    LINE "assembly Tree {"
    LINE "  prop pos: Position = {10, 20}"
    LINE "  child :- $pos"
    LINE "}"
    LINE "t :- Tree{pos: {20, 30}}"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t t = ecs_lookup(world, "t");
    test_assert(t != 0);
    ecs_entity_t t_child = ecs_lookup(world, "t.child");
    test_assert(t_child != 0);

    {
        const void *ptr = ecs_get_id(world, t, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{pos: {x: 20, y: 30}}");
        ecs_os_free(str);
    }

    {
        const Position *p = ecs_get(world, t_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 30);
    }

    ecs_fini(world);
}

void Plecs_assembly_with_with(void) {
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
    LINE "assembly Tree {"
    LINE "  prop count: flecs.meta.i32 = 0"
    LINE "  with Foo {"
    LINE "    child :- Position{$count, $count * 2}"
    LINE "  }"
    LINE "}"
    LINE "t :- Tree{count: 10}"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t t = ecs_lookup(world, "t");
    test_assert(t != 0);
    ecs_entity_t t_child = ecs_lookup(world, "t.child");
    test_assert(t_child != 0);

    {
        const void *ptr = ecs_get_id(world, t, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }

    {
        const Position *p = ecs_get(world, t_child, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Plecs_using_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p1 = ecs_new_entity(world, "foo.p1");
    ecs_entity_t p2 = ecs_new_entity(world, "foo.p2");

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
    LINE "e :- Position{10, 20}\n"
    LINE "e :- Velocity{1, 2}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_single_line_comment_in_value(void) {
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
    LINE "e :- Position{\n"
    LINE "  x: 10\n"
    LINE "  //foo\n"
    LINE "  y: 20\n"
    LINE "}";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_single_line_comment_in_value_after_scope(void) {
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
    LINE "e :- Position{\n"
    LINE "  // foo\n"
    LINE "  x: 10\n"
    LINE "  y: 20\n"
    LINE "}";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_multi_line_comment_in_value(void) {
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
    LINE "e :- Position{\n"
    LINE "  x: 10\n"
    LINE "  /*\n"
    LINE "   * foo\n"
    LINE "   */\n"
    LINE "  y: 20\n"
    LINE "}";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_multi_line_comment_in_value_after_scope(void) {
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
    LINE "e :- Position{\n"
    LINE "  /*\n"
    LINE "   * foo\n"
    LINE "   */\n"
    LINE "  x: 10\n"
    LINE "  y: 20\n"
    LINE "}";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_assert(ecs_has(world, e, Position));

    const Position *ptr = ecs_get(world, e, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 20);

    ecs_fini(world);
}

void Plecs_unterminated_multi_line_comment_in_value(void) {
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
    LINE "e :- Position{\n"
    LINE "  x: 10\n"
    LINE "  /*\n"
    LINE "   * foo\n"
    LINE "  y: 20\n"
    LINE "}";
    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_module_stmt(void) {
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
    LINE "e :- Position{10, 20}\n"
    LINE "e :- Foo\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsModule));

    ecs_entity_t e = ecs_lookup(world, "hello.e");
    test_assert(e != 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
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

void Plecs_nested_module_stmt(void) {
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
    LINE "e :- Position{10, 20}\n"
    LINE "e :- Foo\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t hello = ecs_lookup(world, "hello");
    test_assert(hello != 0);
    test_assert(ecs_has_id(world, hello, EcsModule));
    ecs_entity_t hello_world = ecs_lookup(world, "hello.world");
    test_assert(hello_world != 0);
    test_assert(ecs_has_id(world, hello_world, EcsModule));

    ecs_entity_t e = ecs_lookup(world, "hello.world.e");
    test_assert(e != 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
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

void Plecs_module_stmt_w_scope(void) {
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
    LINE "parent {\n"
    LINE "  e :- Position{10, 20}\n"
    LINE "  e :- Foo\n"
    LINE "}\n"
    LINE "f :- Position{30, 40}\n"
    LINE "f :- Foo\n"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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
    ecs_entity_t foo = ecs_lookup(world, "Foo");
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

void Plecs_module_stmt_w_nested_scope(void) {
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
    LINE "parent {\n"
    LINE "  child {\n"
    LINE "    e :- Position{10, 20}\n"
    LINE "    e :- Foo\n"
    LINE "  }\n"
    LINE "}\n"
    LINE "f :- Position{30, 40}\n"
    LINE "f :- Foo\n"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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
    ecs_entity_t foo = ecs_lookup(world, "Foo");
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

void Plecs_module_w_assembly(void) {
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
    LINE "assembly Tree {\n"
    LINE "  prop count: flecs.meta.i32 = 0\n"
    LINE "  child :- Position{$count, $count * 2}\n"
    LINE "}\n"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    const char *expr_inst =
    LINE "t :- hello.world.Tree{10}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr_inst) == 0);

    ecs_entity_t tree = ecs_lookup(world, "hello.world.Tree");
    test_assert(tree != 0);
    ecs_entity_t t = ecs_lookup(world, "t");
    test_assert(t != 0);
    ecs_entity_t child = ecs_lookup(world, "t.child");
    test_assert(child != 0);

    {
        const void *ptr = ecs_get_id(world, t, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }

    test_assert(ecs_has(world, child, Position));

    {
        const Position *ptr = ecs_get(world, child, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Plecs_module_w_nested_assembly(void) {
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
    LINE "assembly Tree {\n"
    LINE "  prop count: flecs.meta.i32 = 0\n"
    LINE "  child :- Position{$count, $count * 2}\n"
    LINE "}\n"
    LINE "assembly Forest {\n"
    LINE "  prop count: flecs.meta.i32 = 0\n"
    LINE "  t :- Tree{count:$}\n"
    LINE "}\n"
    LINE "";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    const char *expr_inst =
    LINE "f :- hello.world.Forest{10}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr_inst) == 0);

    ecs_entity_t forest = ecs_lookup(world, "hello.world.Forest");
    test_assert(forest != 0);
    ecs_entity_t tree = ecs_lookup(world, "hello.world.Tree");
    test_assert(tree != 0);
    ecs_entity_t f = ecs_lookup(world, "f");
    test_assert(f != 0);
    ecs_entity_t t = ecs_lookup(world, "f.t");
    test_assert(t != 0);
    ecs_entity_t child = ecs_lookup(world, "f.t.child");
    test_assert(child != 0);

    {
        const void *ptr = ecs_get_id(world, f, forest);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, forest, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }
    {
        const void *ptr = ecs_get_id(world, t, tree);
        test_assert(ptr != NULL);
        char *str = ecs_ptr_to_expr(world, tree, ptr);
        test_str(str, "{count: 10}");
        ecs_os_free(str);
    }

    test_assert(ecs_has(world, child, Position));

    {
        const Position *ptr = ecs_get(world, child, Position);
        test_assert(ptr != NULL);
        test_int(ptr->x, 10);
        test_int(ptr->y, 20);
    }

    ecs_fini(world);
}

void Plecs_assign_singleton_tag(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "$ :- Foo\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, foo));

    ecs_fini(world);
}

void Plecs_assign_singleton_component(void) {
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
    LINE "$ :- Position{10, 20}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_has(world, ecs_id(Position), Position));

    const Position *p = ecs_get(world, ecs_id(Position), Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_assign_singleton_tag_w_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "$ {\n"
    LINE "  - Foo\n"
    LINE "}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    test_assert(ecs_has_id(world, foo, foo));

    ecs_fini(world);
}

void Plecs_assign_singleton_2_tags_w_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "$ {\n"
    LINE "  - Foo\n"
    LINE "  - Bar\n"
    LINE "}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    test_assert(ecs_has_id(world, foo, foo));
    test_assert(ecs_has_id(world, bar, bar));

    ecs_fini(world);
}

void Plecs_assign_singleton_component_w_scope(void) {
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
    LINE "  - Position{10, 20}\n"
    LINE "}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    test_assert(ecs_has(world, ecs_id(Position), Position));

    const Position *p = ecs_get(world, ecs_id(Position), Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Plecs_assign_singleton_2_components_w_scope(void) {
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
    LINE "  - Position{10, 20}\n"
    LINE "  - Velocity{1, 2}\n"
    LINE "}\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_with_pair_in_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "Tgt\n"
    LINE "Rel\n"
    LINE "\n"
    LINE "Parent {\n"
    LINE "  with (Rel, Tgt) {\n"
    LINE "    Child\n"
    LINE "  }\n"
    LINE "}\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assembly_redeclare_prop_as_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "assembly Foo {\n"
    LINE "  prop x : flecs.meta.f32 = 10\n"
    LINE "  prop y : flecs.meta.f32 = 10\n"
    LINE "  const y : flecs.meta.f32 = 20\n"
    LINE "}"
    LINE "ent :- Foo{}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_redeclare_prop_as_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "assembly Foo {\n"
    LINE "  prop x : flecs.meta.f32 = 10\n"
    LINE "  prop y : flecs.meta.f32 = 10\n"
    LINE "  prop y : flecs.meta.f32 = 20\n"
    LINE "}"
    LINE "ent :- Foo{}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_assembly_redeclare_const_as_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "assembly Foo {\n"
    LINE "  prop x : flecs.meta.f32 = 10\n"
    LINE "  const y : flecs.meta.f32 = 10\n"
    LINE "  const y : flecs.meta.f32 = 20\n"
    LINE "}"
    LINE "ent :- Foo{}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_add_auto_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    LINE "p :- OVERRIDE|Tag"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t p = ecs_lookup(world, "p");
    test_assert(p != 0);
    test_assert(ecs_has_id(world, p, ECS_OVERRIDE | Tag));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void Plecs_add_auto_override_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "p :- OVERRIDE|(Rel, Tgt)"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t p = ecs_lookup(world, "p");
    test_assert(p != 0);
    test_assert(ecs_has_id(world, p, ECS_OVERRIDE | ecs_pair(Rel, Tgt)));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, Rel, Tgt));

    ecs_fini(world);
}

void Plecs_scope_w_auto_override(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    const char *expr =
    LINE "p {"
    LINE " - OVERRIDE|Tag"
    LINE "}"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t p = ecs_lookup(world, "p");
    test_assert(p != 0);
    test_assert(ecs_has_id(world, p, ECS_OVERRIDE | Tag));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Tag));

    ecs_fini(world);
}

void Plecs_scope_w_auto_override_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "p {"
    LINE " - OVERRIDE|(Rel, Tgt)"
    LINE "}"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t p = ecs_lookup(world, "p");
    test_assert(p != 0);
    test_assert(ecs_has_id(world, p, ECS_OVERRIDE | ecs_pair(Rel, Tgt)));

    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, p);
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, Rel, Tgt));

    ecs_fini(world);
}

void Plecs_pair_w_rel_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "const rel = Rel\n"
    LINE "ent {\n"
    LINE " - ($rel, Tgt)\n"
    LINE "}\n"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_pair(world, ent, Rel, Tgt));

    ecs_fini(world);
}

void Plecs_pair_w_tgt_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "const tgt = Tgt\n"
    LINE "ent {\n"
    LINE " - (Rel, $tgt)\n"
    LINE "}\n"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_pair(world, ent, Rel, Tgt));

    ecs_fini(world);
}

void Plecs_assembly_w_pair_w_this_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "assembly Foo {\n"
    LINE "  prop x : flecs.meta.f32 = 10\n" // dummy prop
    LINE " - (Rel, $this)\n"
    LINE "}\n"
    LINE "ent :- Foo{}\n"
    LINE "\n";

    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, ent, Rel, ent));

    ecs_fini(world);
}

void Plecs_with_value_not_a_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "with Foo{10} {\n"
    LINE "  Bar\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_component_in_with_scope(void) {
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
    LINE "with Position{10, 20} {\n"
    LINE "  - Bar\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_component_in_with_scope_nested(void) {
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
    LINE "with Position{10, 20} {\n"
    LINE "  with Velocity{10, 20} {\n"
    LINE "    - Bar\n"
    LINE "  }\n"
    LINE "}\n"
    LINE "\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) != 0);

    ecs_fini(world);
}

void Plecs_component_in_with_scope_in_scope(void) {
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
    LINE "foo {\n"
    LINE "  - Position{10, 20}\n"
    LINE "  with Position{30, 40} {\n"
    LINE "    - Bar\n"
    LINE "  }\n"
    LINE "}\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

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

void Plecs_assign_after_with_in_scope(void) {
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
    LINE "foo {\n"
    LINE "  - Position{10, 20}\n"
    LINE "  with Position{30, 40} {\n"
    LINE "  }\n"
    LINE "  - Velocity{1, 2}\n"
    LINE "}\n";

    ecs_log_set_level(-4);
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));
    test_assert(ecs_has(world, foo, Velocity));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    const Velocity *v = ecs_get(world, foo, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 1);
    test_int(v->y, 2);

    ecs_fini(world);
}

void Plecs_array_component(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_array(world, {
        .entity = ecs_id(Position),
        .type = ecs_id(ecs_f32_t),
        .count = 2
    });

    const char *expr =
    LINE "foo :- Position[10, 20]\n";
    test_assert(ecs_plecs_from_str(world, NULL, expr) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    test_assert(ecs_has(world, foo, Position));

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}
