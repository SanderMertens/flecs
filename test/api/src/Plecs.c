#include <api.h>

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
