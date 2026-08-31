#include <script.h>

void Template_template_no_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_no_props(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  Foo"
    LINE "}"
    LINE "Tree ent()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    ecs_fini(world);
}

void Template_template_newline_before_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "template Tree"
    LINE "{"
    LINE "  Foo"
    LINE "}"
    LINE "Tree ent()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    ecs_fini(world);
}

void Template_template_prop(void) {
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
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Template_template_prop_space_colon(void) {
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
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Template_template_2_props(void) {
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
    LINE "template Tree {"
    LINE "  prop width: i32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_w_using(void) {
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
    HEAD "using flecs.meta"
    LINE "Foo {}"
    LINE "template Tree {"
    LINE "  prop width: i32 = 10"
    LINE "  prop height: f32 = 20"
    LINE "  Foo"
    LINE "}"
    LINE "Tree ent()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_i32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_f32_t));

    test_assert(ecs_has_id(world, ent, tree));
    test_assert(ecs_has_id(world, ent, foo));

    const void *ptr = ecs_get_id(world, ent, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 10, height: 20}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Template_template_instance_w_default_values(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 10"
    LINE "  prop height: f32 = 20"
    LINE "}"
    LINE ""
    LINE "e { Tree }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_instance_w_assign_default_values(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 10"
    LINE "  prop height: f32 = 20"
    LINE "}"
    LINE ""
    LINE "e { Tree: {} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_instance_w_overridden_values(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 10"
    LINE "  prop height: f32 = 20"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 30, height: 40} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_w_prop_implicit_type(void) {
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
    LINE "template Tree {"
    LINE "  prop width = 10"
    LINE "  prop height = 20"
    LINE "}"
    LINE ""
    LINE "e { Tree }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_i64_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type, ecs_id(ecs_i64_t));

    test_assert(ecs_has_id(world, e, tree));
    const void *ptr = ecs_get_id(world, e, tree);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_assert(str != NULL);
    test_str(str, "{width: 10, height: 20}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Template_template_w_child(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child { Position: {width * 10 + 1, height * 20 + 2} }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t root_child = ecs_lookup(world, "child");
    test_assert(root_child == 0);

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

void Template_template_w_child_parse_script(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child { Position: {width * 10 + 1, height * 20 + 2} }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
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

void Template_template_w_child_parse_script_twice(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child { Position: {width * 10 + 1, height * 20 + 2} }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    }) != 0);

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
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

void Template_template_w_child_update_after_parse(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child { Position: {width * 10 + 1, height * 20 + 2} }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
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
    LINE "e { Tree: {width: 3, height: 4} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr_update, NULL) == 0);

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

void Template_template_w_nested_child(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child {"
    LINE "    Position: {width, height}"
    LINE "    grand_child { Position: {height, width} }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_w_prefab(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  Prefab base {"
    LINE "    Velocity: {width * 2, height * 3}"
    LINE "  }"
    LINE "  child : base {"
    LINE "    Position: {width, height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_w_prefab_tree(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  Prefab base {"
    LINE "    Velocity: {width * 2, height * 3}"
    LINE "    Prefab child {"
    LINE "      Velocity: {height * 3, width * 2}"
    LINE "    }"
    LINE "  }"
    LINE "  child : base {"
    LINE "    Position: {width, height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e { Tree: {width: 1, height: 2} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_w_nested_template(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child {"
    LINE "    Position: {width, height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  tree_1 { Tree: {-width, -height} }"
    LINE "  tree_2 { Tree: {width + 1, height + 1} }"
    LINE "}"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t forest = ecs_lookup(world, "Forest");
    test_assert(forest != 0);

    test_assert(ecs_lookup(world, "tree_1") == 0);
    test_assert(ecs_lookup(world, "tree_2") == 0);
    test_assert(ecs_lookup(world, "Forest.tree_1.child") == 0);
    test_assert(ecs_lookup(world, "Forest.tree_2.child") == 0);

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
    LINE "f { Forest: {10, 20} }"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr_instance, NULL) == 0);

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

void Template_instantiate_prefab_w_template(void) {
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
    LINE "template Tree {"
    LINE "  prop width: f32 = 0"
    LINE "  prop height: f32 = 0"
    LINE "  child { Position: {width, height} }"
    LINE "}"
    LINE ""
    LINE "Prefab p { Tree: {width: 10, height: 20} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t p = ecs_lookup(world, "p");
    test_assert(p != 0);
    test_assert(ecs_lookup(world, "p.child") == 0);
    test_assert(ecs_lookup(world, "child") == 0);

    const char *expr_instance =
    LINE "e : p {}"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr_instance, NULL) == 0);

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

void Template_template_w_prefab_w_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree {"
    LINE "  prop count: f32 = 0"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop count: f32 = 0"
    LINE ""
    LINE "  Prefab TreePrefab {"
    LINE "    Tree: {count: count}"
    LINE "  }"
    LINE ""
    LINE "  child : TreePrefab {}"
    LINE "}"
    LINE "f { Forest: {10} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t forest = ecs_lookup(world, "Forest");
    test_assert(forest != 0);

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

    ecs_fini(world);
}

void Template_3_templates(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree {"
    LINE "  prop t: f32 = 10"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop f: f32 = 20"
    LINE "}"
    LINE ""
    LINE "template Park {"
    LINE "  prop p: f32 = 30"
    LINE "}"
    LINE ""
    LINE "Tree a()"
    LINE "Forest b()"
    LINE "Park c()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_nested_w_default_var(void) {
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
    LINE "template Tree {"
    LINE "  prop count: f32 = 0"
    LINE "  trunk { Position: {count, count * 2} }"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop count: f32 = 0"
    LINE "  child { Tree: {count:$} }"
    LINE "}"
    LINE "f { Forest: {10} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_w_anonymous(void) {
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
    LINE "template Tree {"
    LINE "  prop count: i32 = 0"
    LINE "  _ { Position: {count, count * 2} }"
    LINE "  _ { Position: {count, count * 2} }"
    LINE "}"
    LINE ""
    LINE "t { Tree: {10} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);
    ecs_entity_t t = ecs_lookup(world, "t");
    test_assert(t != 0);

    {
        ecs_query_t *f = ecs_query(world, {
            .terms = {
                { .id = ecs_childof(t) },
                { .id = ecs_id(Position) },
            }
        });

        ecs_iter_t it = ecs_query_iter(world, f);
        test_int(2, ecs_iter_count(&it));
        ecs_query_fini(f);
    }

    ecs_fini(world);
}

void Template_template_w_anonymous_parse_again(void) {
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
    LINE "template Tree {"
    LINE "  prop count: i32 = 0"
    LINE "  _ { Position: {count, count * 2} }"
    LINE "  _ { Position: {count, count * 2} }"
    LINE "}"
    LINE ""
    LINE "t { Tree: {10} }";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);
        ecs_entity_t t = ecs_lookup(world, "t");
        test_assert(t != 0);

        {
            ecs_query_t *f = ecs_query(world, {
                .terms = {
                    { .id = ecs_childof(t) },
                    { .id = ecs_id(Position) },
                }
            });

            ecs_iter_t it = ecs_query_iter(world, f);
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_assert(ecs_get_name(world, it.entities[0]) == NULL);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 10); test_int(p[0].y, 20);
            test_int(p[1].x, 10); test_int(p[1].y, 20);
            test_bool(false, ecs_query_next(&it));
            ecs_query_fini(f);
        }
    }

    const char *expr_again =
    LINE "t { Tree: {10} }";
    test_assert(ecs_script_run(world, NULL, expr_again, NULL) == 0);

    {
        ecs_entity_t tree = ecs_lookup(world, "Tree");
        test_assert(tree != 0);
        ecs_entity_t t = ecs_lookup(world, "t");
        test_assert(t != 0);

        {
            ecs_query_t *f = ecs_query(world, {
                .terms = {
                    { .id = ecs_childof(t) },
                    { .id = ecs_id(Position) },
                }
            });

            ecs_iter_t it = ecs_query_iter(world, f);
            test_bool(true, ecs_query_next(&it));
            test_int(2, it.count);
            test_assert(ecs_get_name(world, it.entities[0]) == NULL);
            Position *p = ecs_field(&it, Position, 1);
            test_int(p[0].x, 10); test_int(p[0].y, 20);
            test_int(p[1].x, 10); test_int(p[1].y, 20);
            test_bool(false, ecs_query_next(&it));
            ecs_query_fini(f);
        }
    }

    ecs_fini(world);
}

void Template_template_w_composite_prop(void) {
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
    LINE "template Tree {"
    LINE "  prop pos: Position = {10, 20}"
    LINE "  child { Position: pos }"
    LINE "}"
    LINE "t { Tree: {pos: {20, 30}} }"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_template_with_with(void) {
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
    HEAD "Foo {}"
    LINE "template Tree {"
    LINE "  prop count: i32 = 0"
    LINE "  with Foo {"
    LINE "    child { Position: {count, count * 2} }"
    LINE "  }"
    LINE "}"
    LINE "t { Tree: {count: 10} }"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

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

void Template_module_w_template(void) {
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
    LINE "template Tree {\n"
    LINE "  prop count: i32 = 0\n"
    LINE "  child { Position: {count, count * 2} }\n"
    LINE "}\n"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *expr_inst =
    LINE "t { hello.world.Tree: {10} }\n";
    test_assert(ecs_script_run(world, NULL, expr_inst, NULL) == 0);

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

void Template_module_w_nested_template(void) {
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
    LINE "module hello.world"
    LINE "template Tree {"
    LINE "  prop count: i32 = 0"
    LINE "  child { Position: {count, count * 2} }"
    LINE "}"
    LINE "template Forest {"
    LINE "  prop count: i32 = 0"
    LINE "  t { Tree: {count:$} }"
    LINE "}"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *expr_inst =
    LINE "f { hello.world.Forest: {10} }\n";
    test_assert(ecs_script_run(world, NULL, expr_inst, NULL) == 0);

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

void Template_template_w_pair_w_this_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, this)\n"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, ent, Rel, ent));

    ecs_fini(world);
}

void Template_template_w_pair_w_prop_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  prop x: entity = flecs\n"
    LINE "  (Rel, $x)\n"
    LINE "}\n"
    LINE "ent { Foo: {flecs.core} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, ent, Rel, EcsFlecsCore));

    ecs_fini(world);
}

void Template_template_w_pair_w_const_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  const x: entity = flecs\n"
    LINE "  (Rel, $x)\n"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, ent, Rel, EcsFlecs));

    ecs_fini(world);
}

void Template_template_w_pair_scope_w_this_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t Rel = ecs_entity(world, { .name = "Rel" });
    ecs_add_id(world, Rel, EcsPairIsTag);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, this) {\n"
    LINE "    child {}"
    LINE "  }"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");
    ecs_entity_t child = ecs_lookup(world, "ent.child");

    test_assert(foo != 0);
    test_assert(ent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, child, Rel, ent));

    ecs_fini(world);
}

void Template_template_w_pair_scope_w_prop_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  prop x: entity = flecs\n"
    LINE "  (Rel, $x) {\n"
    LINE "    child {}"
    LINE "  }"
    LINE "}\n"
    LINE "ent { Foo: {flecs.core} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");
    ecs_entity_t child = ecs_lookup(world, "ent.child");

    test_assert(foo != 0);
    test_assert(ent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, child, Rel, EcsFlecsCore));

    ecs_fini(world);
}

void Template_template_w_pair_scope_w_const_var(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  prop x: entity = flecs\n"
    LINE "  (Rel, $x) {\n"
    LINE "    child {}"
    LINE "  }"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");
    ecs_entity_t child = ecs_lookup(world, "ent.child");

    test_assert(foo != 0);
    test_assert(ent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, child, Rel, EcsFlecs));

    ecs_fini(world);
}

void Template_template_w_this_var_assigned_to_entity_field(void) {
    ecs_world_t *world = ecs_init();

    typedef struct Ref {
        ecs_entity_t e;
    } Ref;

    ECS_COMPONENT(world, Ref);

    ecs_struct(world, {
        .entity = ecs_id(Ref),
        .members = {{ .name = "e", .type = ecs_id(ecs_entity_t) }}
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  Ref: {this}"
    LINE "}"
    LINE "Foo ent()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    const Ref *r = ecs_get(world, ent, Ref);
    test_assert(r != NULL);
    test_assert(r->e == ent);

    ecs_fini(world);
}

void Template_template_w_this_var_in_component_expr(void) {
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
    HEAD "template Foo {"
    LINE "  Velocity: {this[Position].x + 1, this[Position].y + 2}"
    LINE "}"
    LINE "ent {"
    LINE "  Position: {10, 20}"
    LINE "  Foo: {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    const Velocity *v = ecs_get(world, ent, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 11);
    test_int(v->y, 22);

    ecs_fini(world);
}

void Template_template_w_pair_w_this_kw(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, this)\n"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, ent, Rel, ent));

    ecs_fini(world);
}

void Template_template_w_pair_scope_w_this_kw(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t Rel = ecs_entity(world, { .name = "Rel" });
    ecs_add_id(world, Rel, EcsPairIsTag);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, this) {\n"
    LINE "    child {}"
    LINE "  }"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");
    ecs_entity_t child = ecs_lookup(world, "ent.child");

    test_assert(foo != 0);
    test_assert(ent != 0);
    test_assert(child != 0);

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, child, Rel, ent));

    ecs_fini(world);
}

void Template_template_w_this_kw_assigned_to_entity_field(void) {
    ecs_world_t *world = ecs_init();

    typedef struct Ref {
        ecs_entity_t e;
    } Ref;

    ECS_COMPONENT(world, Ref);

    ecs_struct(world, {
        .entity = ecs_id(Ref),
        .members = {{ .name = "e", .type = ecs_id(ecs_entity_t) }}
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  Ref: {this}"
    LINE "}"
    LINE "Foo ent()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    const Ref *r = ecs_get(world, ent, Ref);
    test_assert(r != NULL);
    test_assert(r->e == ent);

    ecs_fini(world);
}

void Template_template_w_this_kw_in_component_expr(void) {
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
    HEAD "template Foo {"
    LINE "  Velocity: {this[Position].x + 1, this[Position].y + 2}"
    LINE "}"
    LINE "ent {"
    LINE "  Position: {10, 20}"
    LINE "  Foo: {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    const Velocity *v = ecs_get(world, ent, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 11);
    test_int(v->y, 22);

    ecs_fini(world);
}

void Template_template_w_const_w_this_kw_in_component_expr(void) {
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
    HEAD "template Foo {"
    LINE "  Position: {10, 20}"
    LINE "  const px = this[Position].x"
    LINE "  Velocity: {px, 0}"
    LINE "}"
    LINE "ent {"
    LINE "  Foo: {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    const Velocity *v = ecs_get(world, ent, Velocity);
    test_assert(v != NULL);
    test_int(v->x, 10);
    test_int(v->y, 0);

    ecs_fini(world);
}

void Template_template_w_pair_w_unresolved_var_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  ($x, Tgt)\n"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_w_pair_w_unresolved_var_second(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, $x)\n"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_w_pair_scope_w_unresolved_var_first(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, $x) {\n"
    LINE "    child {}"
    LINE "  }"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_w_pair_scope_w_unresolved_var_second(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tgt);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  ($x, Tgt) {\n"
    LINE "    child {}"
    LINE "  }"
    LINE "}\n"
    LINE "ent { Foo: {} }\n"
    LINE "\n";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_prop_without_using_meta(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "height");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Template_hoist_var(void) {
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
    HEAD "using flecs.meta"
    LINE "const v = 10"
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "  Position: {v, height}"
    LINE "}"
    LINE "Tree foo(height: 20)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoist_non_folded_var(void) {
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
    HEAD "fn value() -> i32 { 10 }"
    LINE "const v = value()"
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "  Position: {v, height}"
    LINE "}"
    LINE "Tree foo(height: 20)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoist_component_var(void) {
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
    HEAD "source { Position: {10, 20} }"
    LINE "const v = source[Position].x"
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "  Position: {v, height}"
    LINE "}"
    LINE "Tree foo(height: 30)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 30);

    ecs_fini(world);
}

void Template_hoist_vars_nested(void) {
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
    HEAD "const x = 10"
    LINE "parent {"
    LINE "  const y = 20"
    LINE "  template Tree {"
    LINE "    Position: {x, y}"
    LINE "  }"
    LINE "}"
    LINE "parent.Tree foo()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "parent.Tree");
    test_assert(tree != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoist_vars_nested_w_masked(void) {
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
    HEAD "const x = 10"
    LINE "parent {"
    HEAD "  const x = 30"
    LINE "  const y = 20"
    LINE "  template Tree {"
    LINE "    Position: {x, y}"
    LINE "  }"
    LINE "}"
    LINE "parent.Tree foo()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "parent.Tree");
    test_assert(tree != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_anonymous_template_instance(void) {
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
    HEAD "template Tree {"
    LINE "  Position: {10, 20}"
    LINE "}"
    LINE "Tree() {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_iter_t it = ecs_each_id(world, tree);
    test_bool(true, ecs_each_next(&it));
    test_int(1, it.count);
    ecs_entity_t foo = it.entities[0];
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_anonymous_template_instance_no_scope(void) {
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
    HEAD "template Tree {"
    LINE "  Position: {10, 20}"
    LINE "}"
    LINE "Tree()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_iter_t it = ecs_each_id(world, tree);
    test_bool(true, ecs_each_next(&it));
    test_int(1, it.count);
    ecs_entity_t foo = it.entities[0];
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_anonymous_template_instance_w_prop(void) {
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
    HEAD "template Tree {"
    LINE "  prop height: f32 = 10"
    LINE "  Position: {height, height * 2}"
    LINE "}"
    LINE "Tree() {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_iter_t it = ecs_each_id(world, tree);
    test_bool(true, ecs_each_next(&it));
    test_int(1, it.count);
    ecs_entity_t foo = it.entities[0];
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_anonymous_template_instance_w_prop_no_scope(void) {
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
    HEAD "template Tree {"
    LINE "  prop height: f32 = 10"
    LINE "  Position: {height, height * 2}"
    LINE "}"
    LINE "Tree()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    ecs_iter_t it = ecs_each_id(world, tree);
    test_bool(true, ecs_each_next(&it));
    test_int(1, it.count);
    ecs_entity_t foo = it.entities[0];
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_with_after_template(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "template Foo {}"
    LINE "with Bar {"
    LINE "  withBar {}"
    LINE "}"
    LINE "withoutBar {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t withBar = ecs_lookup(world, "withBar");
    test_assert(withBar != 0);
    ecs_entity_t withoutBar = ecs_lookup(world, "withoutBar");
    test_assert(withoutBar != 0);

    test_assert(ecs_has(world, withBar, Bar));
    test_assert(!ecs_has(world, withoutBar, Bar));

    ecs_fini(world);
}

void Template_with_in_scope_after_template(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "template Foo {}"
    LINE "parent {"
    LINE "  with Bar {"
    LINE "    withBar {}"
    LINE "  }"
    LINE "  withoutBar {}"
    LINE "}"
    LINE "defWithoutBar {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t withBar = ecs_lookup(world, "parent.withBar");
    test_assert(withBar != 0);
    ecs_entity_t withoutBar = ecs_lookup(world, "parent.withoutBar");
    test_assert(withoutBar != 0);
    ecs_entity_t defWithoutBar = ecs_lookup(world, "defWithoutBar");
    test_assert(defWithoutBar != 0);

    test_assert(ecs_has(world, withBar, Bar));
    test_assert(!ecs_has(world, withoutBar, Bar));
    test_assert(!ecs_has(world, defWithoutBar, Bar));

    ecs_fini(world);
}

void Template_prefab_w_template(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Bar);

    const char *expr =
    HEAD "Tag {}"
    LINE ""
    LINE "template Foo {"
    LINE "  Tag"
    LINE "}"
    LINE ""
    LINE "prefab Base {"
    LINE "  Foo: {}"
    LINE "}"
    LINE ""
    LINE "e : Base {}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t base = ecs_lookup(world, "Base");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tag = ecs_lookup(world, "Tag");

    test_assert(e != 0);
    test_assert(base != 0);
    test_assert(foo != 0);
    test_assert(tag != 0);

    test_assert(ecs_has_id(world, e, tag));
    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has_pair(world, e, EcsIsA, base));

    ecs_fini(world);
}

void Template_template_in_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    LINE "parent {"
    LINE "  template Foo {"
    LINE "  }"
    LINE "}"
    LINE "parent.Foo ent";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "Foo") == 0);
    ecs_entity_t foo = ecs_lookup(world, "parent.Foo");
    test_assert(foo != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);
    test_assert(ecs_has_id(world, ent, foo));

    ecs_fini(world);
}

void Template_nested_templates_in_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Frame {"
    LINE "  wall {}"
    LINE "}"
    LINE ""
    LINE "template Room { }"
    LINE ""
    LINE "template House {"
    LINE "  building {"
    LINE "    walls { Frame: {} }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "prefab HousePrefab {"
    LINE "  House: {}"
    LINE "  room {"
    LINE "    Room: {}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e : HousePrefab {}"
    ;

    // ecs_log_set_level(0);
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t frame = ecs_lookup(world, "Frame");
    test_assert(frame != 0);

    ecs_entity_t room = ecs_lookup(world, "Room");
    test_assert(room != 0);

    ecs_entity_t house = ecs_lookup(world, "House");
    test_assert(house != 0);

    ecs_entity_t house_prefab = ecs_lookup(world, "HousePrefab");
    test_assert(house_prefab != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t e_room = ecs_lookup(world, "e.room");
    test_assert(e_room != 0);

    ecs_entity_t e_building = ecs_lookup(world, "e.building");
    test_assert(e_building != 0);

    ecs_entity_t e_building_walls = ecs_lookup(world, "e.building.walls");
    test_assert(e_building_walls != 0);

    ecs_entity_t e_building_walls_wall = ecs_lookup(world, "e.building.walls.wall");
    test_assert(e_building_walls_wall != 0);

    ecs_fini(world);
}

void Template_entity_w_2_template_instances(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Foo {"
    LINE "  child_a {}"
    LINE "}"
    LINE ""
    LINE "template Bar {"
    LINE "  child_b {}"
    LINE "}"
    LINE ""
    LINE "e {"
    LINE "  Foo: {}"
    LINE "  Bar: {}"
    LINE "}"
    ;

    // ecs_log_set_level(0);
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child_a = ecs_lookup(world, "e.child_a");
    test_assert(child_a != 0);

    ecs_entity_t child_b = ecs_lookup(world, "e.child_b");
    test_assert(child_b != 0);

    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has_id(world, e, bar));

    ecs_fini(world);
}

void Template_template_w_prefab_and_instance(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Foo {"
    LINE "  prefab Base"
    LINE ""
    LINE "  child {"
    LINE "    grand_child : Base {}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e { Foo: {} }"
    ;

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, foo));

    ecs_entity_t e_child = ecs_lookup(world, "e.child");
    test_assert(e_child != 0);

    ecs_entity_t grand_child = ecs_lookup(world, "e.child.grand_child");
    test_assert(grand_child != 0);

    ecs_entity_t base = ecs_lookup(world, "e.Base");
    test_assert(base != 0);

    test_assert(ecs_has_pair(world, grand_child, EcsIsA, base));

    ecs_fini(world);
}

void Template_template_w_child_w_var(void) {
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
    HEAD "template Foo {"
    LINE "  const pos: Position = {10, 20}"
    LINE "  child {"
    LINE "    Position: pos"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e { Foo: {} }"
    ;

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, foo));

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_template_w_child_w_prop(void) {
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
    HEAD "template Foo {"
    LINE "  prop pos: Position = {0, 0}"
    LINE "  child {"
    LINE "    Position: pos"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e { Foo: {{10, 20}} }"
    ;

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, foo));

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_fold_const(void) {
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
    HEAD "template Foo {"
    LINE "  prop size: i32 = 10"
    LINE "  const size_h = size / 2"
    LINE "  const size_h_2 = size_h + 2"
    LINE "  Position: {size_h, size_h_2}"
    LINE "}"
    LINE ""
    LINE "Foo e1(6)"
    LINE "Foo e2(10)"
    LINE "Foo e3(16)"
    ;

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    ecs_entity_t e3 = ecs_lookup(world, "e3");
    test_assert(e1 != 0);
    test_assert(e2 != 0);
    test_assert(e3 != 0);
    test_assert(ecs_has_id(world, e1, foo));
    test_assert(ecs_has_id(world, e2, foo));
    test_assert(ecs_has_id(world, e3, foo));

    {
        const Position *p = ecs_get(world, e1, Position);
        test_assert(p != NULL);
        test_int(p->x, 3);
        test_int(p->y, 5);
    }

    {
        const Position *p = ecs_get(world, e2, Position);
        test_assert(p != NULL);
        test_int(p->x, 5);
        test_int(p->y, 7);
    }

    {
        const Position *p = ecs_get(world, e3, Position);
        test_assert(p != NULL);
        test_int(p->x, 8);
        test_int(p->y, 10);
    }

    ecs_fini(world);
}

void Template_bulk_create_template(void) {
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
    HEAD "template Position {"
    LINE "  prop x: f32 = 0"
    LINE "  prop y: f32 = 0"
    LINE "  Velocity: {x + 5, y + 5}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    Position p[] = {
        {10, 20},
        {30, 40}
    };

    void *data[] = {p};

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t) {
        .count = 2,
        .ids = {ecs_id(Position)},
        .data = data
    });

    test_assert(entities[0] != 0);
    test_assert(entities[1] != 0);

    {
        const Velocity *v = ecs_get(world, entities[0], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 15);
        test_int(v->y, 25);
    }

    {
        const Velocity *v = ecs_get(world, entities[1], Velocity);
        test_assert(v != NULL);
        test_int(v->x, 35);
        test_int(v->y, 45);
    }

    ecs_fini(world);
}

void Template_template_w_expr_w_self_ref(void) {
    ecs_world_t *world = ecs_init();

    typedef struct Ref {
        ecs_entity_t e;
    } Ref;

    ECS_COMPONENT(world, Ref);

    ecs_struct(world, {
        .entity = ecs_id(Ref),
        .members = {{ .name = "e", .type = ecs_id(ecs_entity_t) }}
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  a {}"
    LINE "  b { Ref: {a} }"
    LINE "}"
    LINE "Foo ent()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Foo");
    test_assert(tree != 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    ecs_entity_t a = ecs_lookup(world, "ent.a");
    test_assert(a != 0);

    ecs_entity_t b = ecs_lookup(world, "ent.b");
    test_assert(b != 0);

    {
        const Ref *r = ecs_get(world, b, Ref);
        test_assert(r != NULL);
        test_assert(r->e == a);
    }

    ecs_fini(world);
}

void Template_entity_w_assign_with_nested_template(void) {
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
    HEAD "template Foo {"
    LINE "}"
    LINE "template Bar {"
    LINE "  Foo child()"
    LINE "}"
    LINE "with Bar() {"
    LINE "  e { Position: {10, 20} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    test_assert(!ecs_has_id(world, e, foo));
    test_assert(ecs_has_id(world, e, bar));
    test_assert(ecs_has(world, e, Position));

    {
        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    test_assert(ecs_has_id(world, child, foo));
    test_assert(!ecs_has_id(world, child, bar));
    test_assert(!ecs_has(world, child, Position));

    ecs_fini(world);
}

void Template_template_w_for(void) {
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
    HEAD "template Foo {"
    LINE "  for i in 0..2 {"
    LINE"     const t = i"
    LINE "    \"child_$i\" { Position: {t, t + 2} }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t child_0 = ecs_lookup(world, "e.child_0");
    test_assert(child_0 != 0);
    ecs_entity_t child_1 = ecs_lookup(world, "e.child_1");
    test_assert(child_1 != 0);

    test_assert(ecs_has_id(world, e, foo));
    test_assert(!ecs_has(world, e, Position));

    test_assert(ecs_has(world, child_0, Position));
    test_assert(ecs_has(world, child_1, Position));

    {
        const Position *p = ecs_get(world, child_0, Position);
        test_assert(p != NULL);
        test_int(p->x, 0);
        test_int(p->y, 2);
    }

    {
        const Position *p = ecs_get(world, child_1, Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 3);
    }

    ecs_fini(world);
}

void Template_template_w_component_w_undefined_identifier(void) {
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
    HEAD "template Foo {"
    LINE "  Position: P"
    LINE "}";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_w_child_component_w_undefined_identifier(void) {
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
    HEAD "template Foo {"
    LINE "  foo {"
    LINE "    Position: P"
    LINE "  }"
    LINE "}";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_w_anonymous_child_component_w_undefined_identifier(void) {
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
    HEAD "template Foo {"
    LINE "  _ {"
    LINE "    Position: P"
    LINE "  }"
    LINE "}";

    ecs_log_set_level(-4);

    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

static int on_foo_invoked = 0;

static void on_foo(ecs_iter_t *it) {
    on_foo_invoked ++;
}

void Template_clear_script_w_template_w_on_remove_observer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }},
        .events = { EcsOnRemove },
        .callback = on_foo
    });

    const char *expr =
    HEAD "e { Position: {10, 20} }"
    LINE ""
    LINE "parent {"
    LINE "  template Bar { }"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .code = expr
    });

    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t parent = ecs_lookup(world, "parent");
    test_assert(parent != 0);
    ecs_entity_t bar = ecs_lookup(world, "parent.Bar");
    test_assert(bar != 0);
    test_assert(ecs_has(world, e, Position));

    test_int(on_foo_invoked, 0);

    ecs_script_clear(world, s, 0);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, bar));

    test_int(on_foo_invoked, 1);

    ecs_fini(world);
}

void Template_clear_script_w_template_w_on_remove_observer_added_after(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }},
        .events = { EcsOnRemove },
        .callback = on_foo
    });

    const char *expr =
    HEAD "e { }"
    LINE ""
    LINE "parent {"
    LINE "  template Bar { }"
    LINE "}";

    ecs_entity_t s = ecs_script(world, {
        .code = expr
    });

    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t parent = ecs_lookup(world, "parent");
    test_assert(parent != 0);
    ecs_entity_t bar = ecs_lookup(world, "parent.Bar");
    test_assert(bar != 0);

    ecs_add(world, e, Position);

    test_int(on_foo_invoked, 0);

    ecs_script_clear(world, s, 0);

    test_assert(!ecs_is_alive(world, e));
    test_assert(!ecs_is_alive(world, parent));
    test_assert(!ecs_is_alive(world, bar));

    test_int(on_foo_invoked, 1);

    ecs_fini(world);
}

void Template_component_w_assign_add(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "  Position: {y += height / 2}"
    LINE "}"
    LINE ""
    HEAD "Foo {"
    LINE "  Position: {10, 20}"
    LINE "  Tree: {6}"
    LINE "}"
    ;

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, NULL, NULL);
    test_assert(s != NULL);

    test_assert(ecs_script_eval(s, NULL, NULL) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 23);

    ecs_script_free(s);

    ecs_fini(world);
}

void Template_component_w_assign_mul(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, {.name = "Position"}),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "  Position: {y *= height / 2}"
    LINE "}"
    LINE ""
    HEAD "Foo {"
    LINE "  Position: {10, 20}"
    LINE "  Tree: {6}"
    LINE "}"
    ;

    ecs_script_t *s = ecs_script_parse(world, NULL, expr, NULL, NULL);
    test_assert(s != NULL);

    test_assert(ecs_script_eval(s, NULL, NULL) == 0);
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *ptr = ecs_get(world, foo, Position);
    test_assert(ptr != NULL);
    test_int(ptr->x, 10);
    test_int(ptr->y, 60);

    ecs_script_free(s);

    ecs_fini(world);
}

void Template_prop_after_const(void) {
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
    LINE "template Tree {"
    LINE "  const x: i32 = 10"
    LINE "  prop y: f32 = 20"
    LINE "  Position: {x, y}"
    LINE "}"
    LINE "Tree e(30)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_const_from_prop(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    LINE "template Tree {"
    LINE "  prop x = 10"
    LINE "  const y = x"
    LINE "  i32: {y}"
    LINE "}"
    LINE "Tree e(30)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const int32_t *ptr = ecs_get(world, e, ecs_i32_t);
    test_assert(ptr != NULL);
    test_int(*ptr, 30);

    ecs_fini(world);
}

void Template_redefine_nested_template_w_prefab(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Foo {"
    LINE "  foo_child {}"
    LINE "}"
    LINE ""
    LINE "template Bar {"
    LINE "  prefab Base {"
    LINE "    Foo: {}"
    LINE "  }"
    LINE ""
    LINE "  (IsA, Base)"
    LINE "}"
    LINE ""
    LINE "e { Bar: { } }"
    LINE "e { Bar: { } }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t base = ecs_lookup(world, "e.Base");
    test_assert(base != 0);

    ecs_entity_t foo_child = ecs_lookup(world, "e.foo_child");
    test_assert(foo_child != 0);

    test_assert(ecs_has_id(world, e, bar));
    test_assert(ecs_has_pair(world, e, EcsIsA, base));
    test_assert(ecs_has_id(world, e, foo));

    ecs_fini(world);
}

void Template_redefine_nested_template_w_prefab_2(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Foo {"
    LINE "}"
    LINE ""
    LINE "template Bar {"
    LINE "  prefab Base {"
    LINE "    Foo: {}"
    LINE "  }"
    LINE ""
    LINE "  child : Base {}"
    LINE "}"
    LINE ""
    LINE "e { Bar: { } }"
    LINE "e { Bar: { } }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t base = ecs_lookup(world, "e.Base");
    test_assert(base != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    test_assert(ecs_has_id(world, e, bar));
    test_assert(ecs_has_pair(world, child, EcsIsA, base));
    test_assert(ecs_has_id(world, child, foo));

    ecs_fini(world);
}

void Template_redefine_nested_template_w_prefab_3(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Foo {"
    LINE "  foo_child {}"
    LINE "}"
    LINE ""
    LINE "template Bar {"
    LINE "  prefab Base {"
    LINE "    Foo: {}"
    LINE "  }"
    LINE ""
    LINE "  child : Base {}"
    LINE "}"
    LINE ""
    LINE "e { Bar: { } }"
    LINE "e { Bar: { } }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t base = ecs_lookup(world, "e.Base");
    test_assert(base != 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t foo_child = ecs_lookup(world, "e.child.foo_child");
    test_assert(foo_child != 0);

    test_assert(ecs_has_id(world, e, bar));
    test_assert(ecs_has_pair(world, child, EcsIsA, base));
    test_assert(ecs_has_id(world, child, foo));

    ecs_fini(world);
}

void Template_template_w_script_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "template Foo {"
    LINE "  prop x: f32 = 0"
    LINE "  prop y: f32 = 0"
    LINE "  Position: {x, y}"
    LINE "}"
    LINE "Foo e(10, 20)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ECS_COMPONENT(world, Position);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has(world, e, Position));

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_template_w_script_pair_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position(x: f32, y: f32)"
    LINE "Tgt {}"
    LINE "template Foo {"
    LINE "  prop x: f32 = 0"
    LINE "  prop y: f32 = 0"
    LINE "  (Position, Tgt): {x, y}"
    LINE "}"
    LINE "Foo e(10, 20)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has_pair(world, e, ecs_id(Position), Tgt));

    const Position *p = ecs_get_pair(world, e, Position, Tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_template_w_prop_and_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "Tag {}"
    LINE ""
    LINE "template Foo {"
    LINE "  prop v = 10"
    LINE "  (Position, Tag): {v, v}"
    LINE "}"
    LINE ""
    LINE "Foo e(10)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_fini(world);
}

void Template_template_w_name_annotation(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "@name Some entity"
    LINE "template Bar {"
    LINE "  Foo"
    LINE "}"
    LINE ""
    LINE "Bar e {}"
    LINE "Bar f {}"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t Bar = ecs_lookup(world, "Bar");
    test_assert(Bar != 0);

    {
        ecs_entity_t e = ecs_lookup(world, "e");
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Foo));
        test_assert(ecs_has_id(world, e, Bar));
        test_str(ecs_doc_get_name(world, e), "Some entity");
    }

    {
        ecs_entity_t e = ecs_lookup(world, "f");
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Foo));
        test_assert(ecs_has_id(world, e, Bar));
        test_str(ecs_doc_get_name(world, e), "Some entity");
    }

    ecs_fini(world);
}

void Template_template_w_name_annotation_update_script(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "@name Some entity"
    LINE "template Bar {"
    LINE "  Foo"
    LINE "}"
    LINE ""
    LINE "Bar e {}"
    LINE "";

    ecs_entity_t script = ecs_script(world, {
        .code = expr
    });

    test_assert(script != 0);

    {
        ecs_entity_t Bar = ecs_lookup(world, "Bar");
        test_assert(Bar != 0);

        {
            ecs_entity_t e = ecs_lookup(world, "e");
            test_assert(e != 0);
            test_assert(ecs_has(world, e, Foo));
            test_assert(ecs_has_id(world, e, Bar));
            test_str(ecs_doc_get_name(world, e), "Some entity");
        }
    }

    const char *expr_2 =
    HEAD "@name Some other entity"
    LINE "template Bar {"
    LINE "  Foo"
    LINE "}"
    LINE ""
    LINE "Bar e {}"
    LINE "";

    ecs_script_update(world, script, 0, expr_2);
    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_str(s->error, NULL);

    {
        ecs_entity_t Bar = ecs_lookup(world, "Bar");
        test_assert(Bar != 0);

        {
            ecs_entity_t e = ecs_lookup(world, "e");
            test_assert(e != 0);
            test_assert(ecs_has(world, e, Foo));
            test_assert(ecs_has_id(world, e, Bar));
            test_str(ecs_doc_get_name(world, e), "Some other entity");
        }
    }

    ecs_fini(world);
}

void Template_template_w_tree_parent(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Foo);

    const char *expr =
    HEAD "@tree Parent"
    LINE "template Bar {"
    LINE "  child {"
    LINE "    Foo"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Bar e {}"
    LINE "Bar f {}"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t Bar = ecs_lookup(world, "Bar");
    test_assert(Bar != 0);

    {
        ecs_entity_t e = ecs_lookup(world, "e");
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Bar));

        ecs_entity_t child = ecs_lookup_child(world, e, "child");
        test_assert(child != 0);
        test_assert(ecs_has(world, child, Foo));

        {
            const EcsParent *p = ecs_get(world, child, EcsParent);
            test_assert(p != NULL);
            test_uint(p->value, e);
        }
    }

    {
        ecs_entity_t e = ecs_lookup(world, "f");
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Bar));

        ecs_entity_t child = ecs_lookup_child(world, e, "child");
        test_assert(child != 0);
        test_assert(ecs_has(world, child, Foo));

        {
            const EcsParent *p = ecs_get(world, child, EcsParent);
            test_assert(p != NULL);
            test_uint(p->value, e);
        }
    }

    ecs_fini(world);
}

void Template_template_w_tree_parent_change_value(void) {
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
    HEAD "@tree Parent"
    LINE "template Bar {"
    LINE "  prop v: i32 = 0"
    LINE "  child {"
    LINE "    Position: {v, v * 2}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Bar e(10)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t Bar = ecs_lookup(world, "Bar");
    test_assert(Bar != 0);

    {
        ecs_entity_t e = ecs_lookup(world, "e");
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Bar));

        ecs_entity_t child = ecs_lookup_child(world, e, "child");
        test_assert(child != 0);
        test_assert(ecs_has(world, child, Position));
        
        {
            const Position *p = ecs_get(world, child, Position);
            test_assert(p != NULL);
            test_int(p->x, 10);
            test_int(p->y, 20);
        }

        {
            const EcsParent *p = ecs_get(world, child, EcsParent);
            test_assert(p != NULL);
            test_uint(p->value, e);
        }

        {
            int32_t value = 20;
            ecs_set_id(world, e, Bar, sizeof(int32_t), &value);
        }
    }

    {
        ecs_entity_t e = ecs_lookup(world, "e");
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Bar));

        ecs_entity_t child = ecs_lookup_child(world, e, "child");
        test_assert(child != 0);
        test_assert(ecs_has(world, child, Position));
        
        {
            const Position *p = ecs_get(world, child, Position);
            test_assert(p != NULL);
            test_int(p->x, 20);
            test_int(p->y, 40);
        }

        {
            const EcsParent *p = ecs_get(world, child, EcsParent);
            test_assert(p != NULL);
            test_uint(p->value, e);
        }
    }

    ecs_fini(world);
}

void Template_template_w_nested_template_w_with(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Frame {"
    LINE "}"
    LINE ""
    LINE "template CityBlock {"
    LINE "  Frame frame() { }"
    LINE "}"
    LINE ""
    LINE "template Cross {"
    LINE "  with CityBlock() {"
    LINE "    CityBlock block() { }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Cross city()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t Cross = ecs_lookup(world, "Cross");
    test_assert(Cross != 0);

    ecs_entity_t city = ecs_lookup(world, "city");
    test_assert(city != 0);

    ecs_entity_t block = ecs_lookup_child(world, city, "block");
    test_assert(block != 0);

    ecs_entity_t frame = ecs_lookup_child(world, block, "frame");
    test_assert(frame != 0);

    ecs_fini(world);
}

void Template_template_w_nested_template_w_with_kind_value(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Frame {"
    LINE "}"
    LINE ""
    LINE "template CityBlock {"
    LINE "  prop value: i32 = 0"
    LINE "  Frame frame() { }"
    LINE "}"
    LINE ""
    LINE "template Cross {"
    LINE "  with CityBlock(10) {"
    LINE "    CityBlock block(20) { }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Cross city()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t CityBlock = ecs_lookup(world, "CityBlock");
    test_assert(CityBlock != 0);

    ecs_entity_t city = ecs_lookup(world, "city");
    test_assert(city != 0);

    ecs_entity_t block = ecs_lookup_child(world, city, "block");
    test_assert(block != 0);

    const int32_t *value = ecs_get_id(world, block, CityBlock);
    test_assert(value != NULL);
    test_int(*value, 20);

    ecs_entity_t frame = ecs_lookup_child(world, block, "frame");
    test_assert(frame != 0);

    ecs_fini(world);
}

void Template_pair_component_w_entity_prop_target(void) {
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
    LINE "template Foo {\n"
    LINE "  prop tgt: entity = flecs\n"
    LINE "  (Position, $tgt): {5, 6}\n"
    LINE "}\n"
    LINE "ent { Foo: {flecs.core} }\n"
    LINE "\n";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t ent = ecs_lookup(world, "ent");

    test_assert(foo != 0);
    test_assert(ent != 0);

    test_assert(ecs_has_id(world, ent, foo));
    test_assert(ecs_has_pair(world, ent, ecs_id(Position), EcsFlecsCore));

    const Position *p = ecs_get_pair(world, ent, Position, EcsFlecsCore);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Template_child_name_from_string_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  prop suffix: string = \"a\""
    LINE "  \"child_$suffix\" {}"
    LINE "}"
    LINE "ent { Foo: {suffix: \"hello\"} }"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    ecs_entity_t child = ecs_lookup(world, "ent.child_hello");
    test_assert(child != 0);

    ecs_fini(world);
}


void Template_template_w_new_expr_in_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "Foo {}"
    LINE "Rel {}"
    LINE "template Tree {"
    LINE "  const helper = new { Foo }"
    LINE "  child {"
    LINE "    (Rel, $helper)"
    LINE "  }"
    LINE "}"
    LINE "Tree e1()"
    LINE "Tree e2()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t rel = ecs_lookup(world, "Rel");
    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    test_assert(e1 != 0);
    test_assert(e2 != 0);

    ecs_entity_t child_1 = ecs_lookup(world, "e1.child");
    ecs_entity_t child_2 = ecs_lookup(world, "e2.child");
    test_assert(child_1 != 0);
    test_assert(child_2 != 0);

    ecs_entity_t helper_1 = ecs_get_target(world, child_1, rel, 0);
    ecs_entity_t helper_2 = ecs_get_target(world, child_2, rel, 0);
    test_assert(helper_1 != 0);
    test_assert(helper_2 != 0);
    test_assert(helper_1 != helper_2);

    test_assert(ecs_has_id(world, helper_1, foo));
    test_assert(ecs_has_id(world, helper_2, foo));
    test_assert(ecs_has_pair(world, helper_1, EcsChildOf, e1));
    test_assert(ecs_has_pair(world, helper_2, EcsChildOf, e2));

    ecs_fini(world);
}

static Position on_position = {};
static int on_position_count = 0;

static void OnPosition(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);
    test_int(it->count, 1);
    on_position = *p;
    on_position_count ++;
}

void Template_template_w_existing_observer(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_observer(world, {
        .query.terms = {{ ecs_id(Position) }},
        .events = { EcsOnSet },
        .callback = OnPosition
    });

    const char *expr =
    HEAD "template Position {"
    LINE "  prop x: f32 = 0"
    LINE "  prop y: f32 = 0"
    LINE "}"
    LINE ""
    LINE "Position e(10, 20)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_int(on_position_count, 1);
    test_int(on_position.x, 10);
    test_int(on_position.y, 20);

    ecs_fini(world);
}

void Template_template_w_prop_w_value_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop value: i32 = 0"
    LINE "  e {"
    LINE "    Position: {value, value * 2}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Foo e(10)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e.e");
    test_assert(e != 0);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_template_w_var_w_value_name(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop x: i32 = 0"
    LINE "  const value = 20"
    LINE "  child {"
    LINE "    Position: {x, value}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Foo e(10)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_template_w_module_qualified_entity_ref(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module foo"
    LINE ""
    LINE "suburban {"
    LINE "  prefab a {}"
    LINE "}"
    LINE ""
    LINE "template Tree {"
    LINE "  (IsA, foo.suburban.a)"
    LINE "}"
    LINE ""
    LINE "foo.Tree inst()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t a = ecs_lookup(world, "foo.suburban.a");
    test_assert(a != 0);

    ecs_entity_t inst = ecs_lookup(world, "foo.inst");
    test_assert(inst != 0);
    test_assert(ecs_has_pair(world, inst, EcsIsA, a));

    ecs_fini(world);
}

void Template_hoisted_var_from_outer_scope(void) {
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
    HEAD "using flecs.meta"
    LINE "const v = 10"
    LINE "template Tree {"
    LINE "  prop height: f32 = 0"
    LINE "  Position: {v, height}"
    LINE "}"
    LINE "Tree foo(height: 20)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoisted_var_from_outer_scopes(void) {
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
    HEAD "const x = 10"
    LINE "parent {"
    LINE "  const y = 20"
    LINE "  template Tree {"
    LINE "    Position: {x, y}"
    LINE "  }"
    LINE "}"
    LINE "parent.Tree foo()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoisted_masked_var_from_outer_scope(void) {
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
    HEAD "const x = 10"
    LINE "parent {"
    HEAD "  const x = 30"
    LINE "  const y = 20"
    LINE "  template Tree {"
    LINE "    Position: {x, y}"
    LINE "  }"
    LINE "}"
    LINE "parent.Tree foo()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoisted_var_instantiate_after_run(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr_1 =
    HEAD "const v = 10"
    LINE "template Tree {"
    LINE "  Position: {v, 20}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr_1, NULL) == 0);

    const char *expr_2 =
    HEAD "Tree foo()";

    test_assert(ecs_script_run(world, NULL, expr_2, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);

    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Template_hoisted_var_w_script_entity(void) {

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    HEAD "tgt {}"
    LINE "const t = tgt"
    LINE "template Tree {"
    LINE "  (Rel, $t)"
    LINE "}"
    LINE "Tree foo()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    ecs_entity_t tgt = ecs_lookup(world, "tgt");
    test_assert(foo != 0);
    test_assert(tgt != 0);

    test_assert(ecs_has_pair(world, foo, Rel, tgt));

    ecs_fini(world);
}

void Template_eval_twice_w_failed_method_call_in_body(void) {

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Apples);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_pair(world, e, Likes, Apples);

    const char *expr =
    HEAD "template Foo {"
    LINE "  const b = e.has(pair(Likes, Apples))"
    LINE "}";

    ecs_log_set_level(-4);

    ecs_script_eval_result_t parse_result = {0};
    ecs_script_t *script = ecs_script_parse(world, NULL, expr, NULL, &parse_result);
    test_assert(script != NULL);
    test_assert(parse_result.error == NULL);

    ecs_script_eval_result_t result_1 = {0};
    test_assert(ecs_script_eval(script, NULL, &result_1) != 0);
    test_assert(result_1.error != NULL);
    test_assert(strstr(result_1.error, "unresolved reference 'pair'") != NULL);
    ecs_os_free(result_1.error);

    ecs_script_eval_result_t result_2 = {0};
    test_assert(ecs_script_eval(script, NULL, &result_2) != 0);
    test_assert(result_2.error != NULL);
    test_assert(strstr(result_2.error, "unresolved reference 'pair'") != NULL);
    ecs_os_free(result_2.error);

    ecs_script_free(script);
    ecs_fini(world);
}

void Template_eval_twice_w_failed_method_call_in_prop_default(void) {

    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Likes);
    ECS_TAG(world, Apples);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_add_pair(world, e, Likes, Apples);

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop b: bool = e.has(pair(Likes, Apples))"
    LINE "}";

    ecs_log_set_level(-4);

    ecs_script_eval_result_t parse_result = {0};
    ecs_script_t *script = ecs_script_parse(world, NULL, expr, NULL, &parse_result);
    test_assert(script != NULL);
    test_assert(parse_result.error == NULL);

    ecs_script_eval_result_t result_1 = {0};
    test_assert(ecs_script_eval(script, NULL, &result_1) != 0);
    test_assert(result_1.error != NULL);
    test_assert(strstr(result_1.error, "unresolved reference 'pair'") != NULL);
    ecs_os_free(result_1.error);

    ecs_script_eval_result_t result_2 = {0};
    test_assert(ecs_script_eval(script, NULL, &result_2) != 0);
    test_assert(result_2.error != NULL);
    test_assert(strstr(result_2.error, "unresolved reference 'pair'") != NULL);
    ecs_os_free(result_2.error);

    ecs_script_free(script);
    ecs_fini(world);
}

void Template_singleton_scope_w_template(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "$ {"
            LINE "  Position: {1, 2}"
            LINE "}"
            LINE "template Foo {"
            LINE "  Position: {3, 4}"
            LINE "}"
            LINE "Foo inst()"
    });
    test_assert(script != 0);

    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error == NULL);

    const Position *p = ecs_get(world, ecs_id(Position), Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_entity_t inst = ecs_lookup(world, "inst");
    test_assert(inst != 0);
    p = ecs_get(world, inst, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Template_annotation_in_template(void) {
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
    HEAD "template Foo {"
    LINE "  @brief A description"
    LINE "  child {"
    LINE "    Position: {1, 2}"
    LINE "  }"
    LINE "  sibling {"
    LINE "    Position: {3, 4}"
    LINE "  }"
    LINE "}"
    LINE "Foo inst_a()"
    LINE "Foo inst_b()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inst_a = ecs_lookup(world, "inst_a");
    test_assert(inst_a != 0);
    ecs_entity_t inst_b = ecs_lookup(world, "inst_b");
    test_assert(inst_b != 0);

    ecs_entity_t child_a = ecs_lookup(world, "inst_a.child");
    test_assert(child_a != 0);
    ecs_entity_t sibling_a = ecs_lookup(world, "inst_a.sibling");
    test_assert(sibling_a != 0);
    ecs_entity_t child_b = ecs_lookup(world, "inst_b.child");
    test_assert(child_b != 0);
    ecs_entity_t sibling_b = ecs_lookup(world, "inst_b.sibling");
    test_assert(sibling_b != 0);

    test_str(ecs_doc_get_brief(world, child_a), "A description");
    test_str(ecs_doc_get_brief(world, child_b), "A description");
    test_str(ecs_doc_get_brief(world, sibling_a), NULL);
    test_str(ecs_doc_get_brief(world, sibling_b), NULL);
    test_str(ecs_doc_get_brief(world, inst_a), NULL);
    test_str(ecs_doc_get_brief(world, inst_b), NULL);

    const Position *p = ecs_get(world, child_a, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, sibling_a, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Template_annotation_in_for_in_template(void) {
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
    HEAD "template Foo {"
    LINE "  for i in 0..2 {"
    LINE "    @brief A description"
    LINE "    \"child_{i}\" {"
    LINE "      Position: {i, 2}"
    LINE "    }"
    LINE "    \"sibling_{i}\" {"
    LINE "      Position: {i, 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo inst()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child_0 = ecs_lookup(world, "inst.child_0");
    test_assert(child_0 != 0);
    ecs_entity_t child_1 = ecs_lookup(world, "inst.child_1");
    test_assert(child_1 != 0);
    ecs_entity_t sibling_0 = ecs_lookup(world, "inst.sibling_0");
    test_assert(sibling_0 != 0);
    ecs_entity_t sibling_1 = ecs_lookup(world, "inst.sibling_1");
    test_assert(sibling_1 != 0);

    test_str(ecs_doc_get_brief(world, child_0), "A description");
    test_str(ecs_doc_get_brief(world, child_1), "A description");
    test_str(ecs_doc_get_brief(world, sibling_0), NULL);
    test_str(ecs_doc_get_brief(world, sibling_1), NULL);

    const Position *p = ecs_get(world, child_0, Position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 2);

    p = ecs_get(world, child_1, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Template_annotation_in_if_in_template(void) {
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
    HEAD "template Foo {"
    LINE "  prop v: bool = true"
    LINE "  if v {"
    LINE "    @brief A description"
    LINE "    child {"
    LINE "      Position: {1, 2}"
    LINE "    }"
    LINE "    sibling {"
    LINE "      Position: {3, 4}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo inst()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "inst.child");
    test_assert(child != 0);
    ecs_entity_t sibling = ecs_lookup(world, "inst.sibling");
    test_assert(sibling != 0);

    test_str(ecs_doc_get_brief(world, child), "A description");
    test_str(ecs_doc_get_brief(world, sibling), NULL);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    p = ecs_get(world, sibling, Position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Template_multiple_templates_dont_leak_child_names(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "module m"
    LINE "template A {"
    LINE "  lamp {}"
    LINE "}"
    LINE "template B {"
    LINE "  bulb {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "m.A") != 0);
    test_assert(ecs_lookup(world, "m.B") != 0);
    test_assert(ecs_lookup(world, "m.lamp") == 0);
    test_assert(ecs_lookup(world, "m.bulb") == 0);
    test_assert(ecs_lookup(world, "lamp") == 0);
    test_assert(ecs_lookup(world, "bulb") == 0);

    ecs_fini(world);
}

void Template_injected_child_order_w_ordered_children(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Cap {"
    LINE "  Position: {0, 0}"
    LINE "  chrome { Position: {1, 1} }"
    LINE "}"
    LINE "row {"
    LINE "  flecs.core.OrderedChildren"
    LINE "  Cap: {}"
    LINE "  a { Position: {0, 0} }"
    LINE "  b { Position: {0, 0} }"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t row = ecs_lookup(world, "row");
    test_assert(row != 0);

    ecs_entity_t chrome = ecs_lookup(world, "row.chrome");
    ecs_entity_t a = ecs_lookup(world, "row.a");
    ecs_entity_t b = ecs_lookup(world, "row.b");
    test_assert(chrome != 0);
    test_assert(a != 0);
    test_assert(b != 0);

    ecs_entities_t children = ecs_get_ordered_children(world, row);
    test_int(children.count, 3);
    test_uint(children.ids[0], chrome);
    test_uint(children.ids[1], a);
    test_uint(children.ids[2], b);

    ecs_fini(world);
}

void Template_template_prop_default_from_const(void) {
    typedef struct {
        ecs_u8_t r;
        ecs_u8_t g;
        ecs_u8_t b;
        ecs_u8_t a;
    } Rgba;

    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Rgba" }),
        .members = {
            {"r", ecs_id(ecs_u8_t)},
            {"g", ecs_id(ecs_u8_t)},
            {"b", ecs_id(ecs_u8_t)},
            {"a", ecs_id(ecs_u8_t)}
        }
    });

    const char *expr =
    HEAD "const White: Rgba = {255, 255, 255, 255}"
    LINE "template Panel {"
    LINE "  prop color: Rgba = White"
    LINE "}"
    LINE "Panel inst()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "Panel");
    test_assert(panel != 0);

    ecs_entity_t inst = ecs_lookup(world, "inst");
    test_assert(inst != 0);

    const Rgba *color = ecs_get_id(world, inst, panel);
    test_assert(color != NULL);
    test_int(color->r, 255);
    test_int(color->g, 255);
    test_int(color->b, 255);
    test_int(color->a, 255);

    ecs_fini(world);
}

void Template_template_root_component_w_string_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  mut label: string = \"a\""
    LINE "  Text: {\"{label}\"}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const ecs_string_t *value = ecs_get_id(world, e, text);
    test_assert(value != NULL);
    test_str(*value, "a");

    ecs_string_t label = "hello";
    ecs_set_id(world, e, mut, sizeof(ecs_string_t), &label);

    value = ecs_get_id(world, e, text);
    test_assert(value != NULL);
    test_str(*value, "hello");

    ecs_fini(world);
}

void Template_template_root_component_after_component_w_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t text_2 = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text2" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  mut label: string = \"a\""
    LINE "  Text: {\"x\"}"
    LINE "  Text2: {label}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const ecs_string_t *value = ecs_get_id(world, e, text);
    test_assert(value != NULL);
    test_str(*value, "x");

    value = ecs_get_id(world, e, text_2);
    test_assert(value != NULL);
    test_str(*value, "a");

    ecs_fini(world);
}

void Template_template_in_singleton_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template T {"
    LINE "  prop v: f32 = 1"
    LINE "  Mass: {v}"
    LINE "}"
    LINE "$ { T: {v: 5} }";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run(world, NULL, expr, &result) != 0);
    test_assert(result.error != NULL);
    test_int(result.line, 5);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Template_template_instantiated_on_itself(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template T {"
    LINE "  prop v: f32 = 1"
    LINE "  Mass: {v}"
    LINE "}"
    LINE "T { T: {v: 5} }";

    ecs_log_set_level(-4);
    ecs_script_eval_result_t result = {0};
    test_assert(ecs_script_run(world, NULL, expr, &result) != 0);
    test_assert(result.error != NULL);
    test_int(result.line, 5);
    ecs_os_free(result.error);

    ecs_fini(world);
}

void Template_template_root_component_w_string_mut_in_nested_if(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t text_2 = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text2" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t text_3 = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text3" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  mut label: string = \"a\""
    LINE "  prop cond: bool = true"
    LINE "  Text: {\"x\"}"
    LINE "  if cond {"
    LINE "    if cond {"
    LINE "      Text2: {label}"
    LINE "    }"
    LINE "  }"
    LINE "  Text3: {label}"
    LINE "}"
    LINE "Foo e(cond: true)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const ecs_string_t *value = ecs_get_id(world, e, text);
    test_assert(value != NULL);
    test_str(*value, "x");

    value = ecs_get_id(world, e, text_2);
    test_assert(value != NULL);
    test_str(*value, "a");

    value = ecs_get_id(world, e, text_3);
    test_assert(value != NULL);
    test_str(*value, "a");

    ecs_string_t label = "hello";
    ecs_set_id(world, e, mut, sizeof(ecs_string_t), &label);

    value = ecs_get_id(world, e, text_2);
    test_assert(value != NULL);
    test_str(*value, "hello");

    value = ecs_get_id(world, e, text_3);
    test_assert(value != NULL);
    test_str(*value, "hello");

    ecs_fini(world);
}

void Template_template_root_component_w_string_mut_in_match_in_if(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t text_2 = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text2" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t text_3 = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text3" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  mut label: string = \"a\""
    LINE "  prop kind: i32 = 1"
    LINE "  Text: {\"x\"}"
    LINE "  if kind == 1 {"
    LINE "    Text2: {match kind {"
    LINE "      1: label"
    LINE "      _: \"z\""
    LINE "    }}"
    LINE "  }"
    LINE "  Text3: {label}"
    LINE "}"
    LINE "Foo e(kind: 1)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const ecs_string_t *value = ecs_get_id(world, e, text);
    test_assert(value != NULL);
    test_str(*value, "x");

    value = ecs_get_id(world, e, text_2);
    test_assert(value != NULL);
    test_str(*value, "a");

    value = ecs_get_id(world, e, text_3);
    test_assert(value != NULL);
    test_str(*value, "a");

    ecs_string_t label = "hello";
    ecs_set_id(world, e, mut, sizeof(ecs_string_t), &label);

    value = ecs_get_id(world, e, text_2);
    test_assert(value != NULL);
    test_str(*value, "hello");

    value = ecs_get_id(world, e, text_3);
    test_assert(value != NULL);
    test_str(*value, "hello");

    ecs_fini(world);
}

void Template_template_props_set_on_multiple_entities_w_bulk_init(void) {
    typedef struct {
        char *label;
    } FooProps;

    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  prop label: string = \"?\""
    LINE "  Text: {\"v_{label}\"}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);

    FooProps props[8];
    int32_t i;
    for (i = 0; i < 8; i ++) {
        char buf[8];
        ecs_os_snprintf(buf, 8, "p%d", i);
        props[i].label = ecs_os_strdup(buf);
    }

    void *data[2] = { props, NULL };

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t){
        .count = 8,
        .ids = { foo },
        .data = data
    });
    test_assert(entities != NULL);

    ecs_entity_t instances[8];
    ecs_os_memcpy_n(instances, entities, ecs_entity_t, 8);

    for (i = 0; i < 8; i ++) {
        char expect[16];
        ecs_os_snprintf(expect, 16, "v_p%d", i);

        const ecs_string_t *value = ecs_get_id(world, instances[i], text);
        test_assert(value != NULL);
        test_str(*value, expect);
    }

    ecs_fini(world);
}

static
void template_prop_large_struct_run(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t large = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Large" }),
        .members = {
            { .name = "x", .type = ecs_id(ecs_i32_t) },
            { .name = "pad", .type = ecs_id(ecs_i32_t), .count = 1024 }
        }
    });
    test_assert(large != 0);

    const char *expr =
    HEAD "template Hud {"
    LINE "  prop v: Large = {x: 10}"
    LINE "  Large: {x: $v.x}"
    LINE "}"
    LINE "inst { Hud: {} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inst = ecs_lookup(world, "inst");
    test_assert(inst != 0);
    test_assert(ecs_get_id(world, inst, large) != NULL);
    test_int(*(int32_t*)ecs_get_id(world, inst, large), 10);

    ecs_fini(world);
}

void Template_template_prop_large_struct_no_leak(void) {
    template_prop_large_struct_run();

    int64_t balance_before = (ecs_os_api_malloc_count +
        ecs_os_api_calloc_count) - ecs_os_api_free_count;

    int32_t i;
    for (i = 0; i < 20; i ++) {
        template_prop_large_struct_run();
    }

    int64_t balance_after = (ecs_os_api_malloc_count +
        ecs_os_api_calloc_count) - ecs_os_api_free_count;
    test_int(0, balance_after - balance_before);
}

void Template_many_templates_w_props_in_script(void) {
    ecs_world_t *world = ecs_init();

    int32_t t, template_count = 12;

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    ecs_strbuf_appendlit(&buf, "struct Value(x: f32, y: f32)\n");
    ecs_strbuf_appendlit(&buf, "Prefab Base {}\n");

    for (t = 0; t < template_count; t ++) {
        ecs_strbuf_append(&buf, "template T%d {\n", t);
        ecs_strbuf_appendlit(&buf, "  prop w: f32 = 2\n");
        ecs_strbuf_appendlit(&buf, "  prop d: f32 = 3\n");
        ecs_strbuf_appendlit(&buf, "  prop h: f32 = 4\n");
        ecs_strbuf_appendlit(&buf, "  prop n: i32 = 2\n");
        ecs_strbuf_appendlit(&buf, "  prop kind: i32 = 0\n");
        ecs_strbuf_appendlit(&buf, "  prop lit: f32 = 1\n");
        ecs_strbuf_appendlit(&buf, "  const scale = w * d * h * lit\n");
        ecs_strbuf_appendlit(&buf, "  const sign = match kind {\n");
        ecs_strbuf_appendlit(&buf, "    0: 1\n");
        ecs_strbuf_appendlit(&buf, "    _: -1\n");
        ecs_strbuf_appendlit(&buf, "  }\n");
        ecs_strbuf_appendlit(&buf, "  for i in 0..n {\n");
        ecs_strbuf_appendlit(&buf, "    \"c_{i}\" : Base {\n");
        ecs_strbuf_appendlit(&buf, "      Value: {scale * i, sign}\n");
        ecs_strbuf_appendlit(&buf, "    }\n");
        ecs_strbuf_appendlit(&buf, "  }\n");
        if (t) {
            ecs_strbuf_append(&buf, "  prev { T%d: {w: w, n: n} }\n", t - 1);
        }
        ecs_strbuf_appendlit(&buf, "}\n");
    }

    ecs_strbuf_append(&buf, "inst { T%d: {w: 2, n: 3, kind: 1} }\n",
        template_count - 1);

    char *expr = ecs_strbuf_get(&buf);
    int result = ecs_script_run(world, NULL, expr, NULL);
    ecs_os_free(expr);
    test_int(result, 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    test_assert(base != 0);

    ecs_entity_t value = ecs_lookup(world, "Value");
    test_assert(value != 0);

    ecs_entity_t inst = ecs_lookup(world, "inst");
    test_assert(inst != 0);

    ecs_entity_t parent = inst;
    for (t = template_count - 1; t >= 0; t --) {
        char name[8];
        int32_t i;
        for (i = 0; i < 3; i ++) {
            ecs_os_snprintf(name, 8, "c_%d", i);
            ecs_entity_t child = ecs_lookup_child(world, parent, name);
            test_assert(child != 0);
            test_assert(ecs_has_pair(world, child, EcsIsA, base));

            const void *ptr = ecs_get_id(world, child, value);
            test_assert(ptr != NULL);
            test_int(((const ecs_f32_t*)ptr)[0], 24 * i);
            test_int(((const ecs_f32_t*)ptr)[1],
                t == (template_count - 1) ? -1 : 1);
        }

        if (t) {
            parent = ecs_lookup_child(world, parent, "prev");
            test_assert(parent != 0);
        }
    }

    ecs_fini(world);
}

void Template_tree_parent_prefab_before_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "@tree Parent"
    LINE "prefab Base {"
    LINE "  child { }"
    LINE "}"
    LINE ""
    LINE "template Tpl {"
    LINE "  prop x: f32 = 0"
    LINE "}"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t base = ecs_lookup(world, "Base");
    test_assert(base != 0);
    ecs_entity_t child = ecs_lookup_child(world, base, "child");
    test_assert(child != 0);
    test_assert(ecs_has(world, child, EcsParent));

    ecs_fini(world);
}

typedef struct {
    ecs_u8_t r, g, b, a;
} Rgba;

typedef struct {
    ecs_f32_t strength;
    Rgba color;
} Emissive;

typedef struct {
    ecs_f32_t r, g, b, a;
} RgbaF;

typedef struct {
    ecs_f32_t strength;
    RgbaF color;
} EmissiveF;

typedef struct {
    ecs_i32_t x, y;
} TemplatePropDefault;

ECS_CTOR(TemplatePropDefault, ptr, {
    ptr->x = 10;
    ptr->y = 20;
})

static
void register_rgba_types(ecs_world_t *world, ecs_entity_t *rgba_out,
    ecs_entity_t *emissive_out)
{
    ecs_entity_t rgba = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Rgba" }),
        .members = {
            {"r", ecs_id(ecs_u8_t)},
            {"g", ecs_id(ecs_u8_t)},
            {"b", ecs_id(ecs_u8_t)},
            {"a", ecs_id(ecs_u8_t)}
        }
    });

    ecs_entity_t emissive = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Emissive" }),
        .members = {
            {"strength", ecs_id(ecs_f32_t)},
            {"color", rgba}
        }
    });

    if (rgba_out) *rgba_out = rgba;
    if (emissive_out) *emissive_out = emissive;
}

static
void register_rgba_f_types(ecs_world_t *world, ecs_entity_t *rgba_out,
    ecs_entity_t *emissive_out)
{
    ecs_entity_t rgba = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "RgbaF" }),
        .members = {
            {"r", ecs_id(ecs_f32_t)},
            {"g", ecs_id(ecs_f32_t)},
            {"b", ecs_id(ecs_f32_t)},
            {"a", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t emissive = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "EmissiveF" }),
        .members = {
            {"strength", ecs_id(ecs_f32_t)},
            {"color", rgba}
        }
    });

    if (rgba_out) *rgba_out = rgba;
    if (emissive_out) *emissive_out = emissive;
}

static
ecs_entity_t register_template_prop_default(ecs_world_t *world)
{
    ECS_COMPONENT(world, TemplatePropDefault);

    ecs_set_hooks(world, TemplatePropDefault, {
        .ctor = ecs_ctor(TemplatePropDefault)
    });

    ecs_struct(world, {
        .entity = ecs_id(TemplatePropDefault),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    return ecs_id(TemplatePropDefault);
}

void Template_template_prop_struct_assign_whole_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0;
    register_rgba_types(world, &rgba, NULL);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255})"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Rgba *c = ecs_get_id(world, panel, rgba);
    test_assert(c != NULL);
    test_int(c->r, 60);
    test_int(c->g, 220);
    test_int(c->b, 255);
    test_int(c->a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_assign_whole_component_no_dollar(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0;
    register_rgba_types(world, &rgba, NULL);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  panel {"
    LINE "    Rgba: color"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255})"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Rgba *c = ecs_get_id(world, panel, rgba);
    test_assert(c != NULL);
    test_int(c->r, 60);
    test_int(c->g, 220);
    test_int(c->b, 255);
    test_int(c->a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_assign_whole_component_default(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0;
    register_rgba_types(world, &rgba, NULL);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "  }"
    LINE "}"
    LINE "Sign s()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Rgba *c = ecs_get_id(world, panel, rgba);
    test_assert(c != NULL);
    test_int(c->r, 255);
    test_int(c->g, 0);
    test_int(c->b, 0);
    test_int(c->a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_in_nested_struct_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "    Emissive: {strength: $strength, color: $color}"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Rgba *c = ecs_get_id(world, panel, rgba);
    test_assert(c != NULL);
    test_int(c->r, 60);
    test_int(c->g, 220);
    test_int(c->b, 255);
    test_int(c->a, 255);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_in_nested_struct_literal_default(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "    Emissive: {strength: $strength, color: $color}"
    LINE "  }"
    LINE "}"
    LINE "Sign s()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 2);
    test_int(e->color.r, 255);
    test_int(e->color.g, 0);
    test_int(e->color.b, 0);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_in_nested_struct_literal_partial_args(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "    Emissive: {strength: $strength, color: $color}"
    LINE "  }"
    LINE "}"
    LINE "Sign s(strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 255);
    test_int(e->color.g, 0);
    test_int(e->color.b, 0);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_f32_members_in_nested_struct_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_f_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: RgbaF = {1, 0, 0, 1}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    RgbaF: $color"
    LINE "    EmissiveF: {strength: $strength, color: $color}"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {0.25, 0.5, 1, 1}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const RgbaF *c = ecs_get_id(world, panel, rgba);
    test_assert(c != NULL);
    test_flt(c->r, 0.25);
    test_flt(c->g, 0.5);
    test_flt(c->b, 1);
    test_flt(c->a, 1);

    const EmissiveF *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_flt(e->strength, 6);
    test_flt(e->color.r, 0.25);
    test_flt(e->color.g, 0.5);
    test_flt(e->color.b, 1);
    test_flt(e->color.a, 1);

    ecs_fini(world);
}

void Template_template_prop_struct_in_struct_literal_same_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  Rgba: $color"
    LINE "  Emissive: {strength: $strength, color: $color}"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t s = ecs_lookup(world, "s");
    test_assert(s != 0);

    const Rgba *c = ecs_get_id(world, s, rgba);
    test_assert(c != NULL);
    test_int(c->r, 60);
    test_int(c->g, 220);
    test_int(c->b, 255);
    test_int(c->a, 255);

    const Emissive *e = ecs_get_id(world, s, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_in_deeply_nested_child(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    inner {"
    LINE "      Rgba: $color"
    LINE "      Emissive: {strength: $strength, color: $color}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inner = ecs_lookup(world, "s.panel.inner");
    test_assert(inner != 0);

    const Rgba *c = ecs_get_id(world, inner, rgba);
    test_assert(c != NULL);
    test_int(c->r, 60);
    test_int(c->g, 220);
    test_int(c->b, 255);
    test_int(c->a, 255);

    const Emissive *e = ecs_get_id(world, inner, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_forwarded_to_nested_template(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Panel {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  Rgba: $color"
    LINE "  Emissive: {strength: $strength, color: $color}"
    LINE "}"
    LINE "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  Panel panel(color: $color, strength: $strength)"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Rgba *c = ecs_get_id(world, panel, rgba);
    test_assert(c != NULL);
    test_int(c->r, 60);
    test_int(c->g, 220);
    test_int(c->b, 255);
    test_int(c->a, 255);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_no_default(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba"
    LINE "  prop strength: f32"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "    Emissive: {strength: $strength, color: $color}"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_typed_no_default_ctor(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value_type = register_template_prop_default(world);

    const char *expr =
    HEAD "template T {"
    LINE "  prop value: TemplatePropDefault"
    LINE "  child {"
    LINE "    TemplatePropDefault: $value"
    LINE "  }"
    LINE "}"
    LINE "T e()"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const TemplatePropDefault *value = ecs_get_id(world, child, value_type);
    test_assert(value != NULL);
    test_int(value->x, 10);
    test_int(value->y, 20);

    ecs_fini(world);
}

void Template_template_prop_typed_no_default_override(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t value_type = register_template_prop_default(world);

    const char *expr =
    HEAD "template T {"
    LINE "  prop value: TemplatePropDefault"
    LINE "  child {"
    LINE "    TemplatePropDefault: $value"
    LINE "  }"
    LINE "}"
    LINE "T e(value: {30, 40})"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const TemplatePropDefault *value = ecs_get_id(world, child, value_type);
    test_assert(value != NULL);
    test_int(value->x, 30);
    test_int(value->y, 40);

    ecs_fini(world);
}

void Template_template_prop_no_default_missing_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template T {"
    LINE "  prop value:"
    LINE "}"
    LINE "";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Template_template_prop_struct_in_nested_struct_literal_w_using(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    ecs_entity_t scope = ecs_entity(world, { .name = "engine" });
    ecs_entity_t prev = ecs_set_scope(world, scope);
    register_rgba_types(world, &rgba, &emissive);
    ecs_set_scope(world, prev);

    const char *expr =
    HEAD "using engine"
    LINE "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    Rgba: $color"
    LINE "    Emissive: {strength: $strength, color: $color}"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_prop_struct_member_expr_in_struct_literal(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t rgba = 0, emissive = 0;
    register_rgba_types(world, &rgba, &emissive);

    const char *expr =
    HEAD "template Sign {"
    LINE "  prop color: Rgba = {255, 0, 0, 255}"
    LINE "  prop strength: f32 = 2"
    LINE "  panel {"
    LINE "    Emissive: {strength: $strength, color: {$color.r, $color.g, $color.b, $color.a}}"
    LINE "  }"
    LINE "}"
    LINE "Sign s(color: {60, 220, 255, 255}, strength: 6)"
    LINE "";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t panel = ecs_lookup(world, "s.panel");
    test_assert(panel != 0);

    const Emissive *e = ecs_get_id(world, panel, emissive);
    test_assert(e != NULL);
    test_int(e->strength, 6);
    test_int(e->color.r, 60);
    test_int(e->color.g, 220);
    test_int(e->color.b, 255);
    test_int(e->color.a, 255);

    ecs_fini(world);
}

void Template_template_props_no_member_entities(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop width: i32 = 10"
    LINE "  mut height: f32 = 20"
    LINE "}"
    LINE "Tree ent(width: 30)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

    const EcsStruct *st = ecs_get(world, tree, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "width");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type, ecs_id(ecs_i32_t));
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->member, 0);

    ecs_entity_t muts = ecs_lookup_child(world, tree, "mut");
    test_assert(muts != 0);

    const EcsStruct *muts_st = ecs_get(world, muts, EcsStruct);
    test_assert(muts_st != NULL);
    test_int(muts_st->members.count, 1);
    test_str(ecs_vec_get_t(&muts_st->members, ecs_member_t, 0)->name, "height");
    test_uint(ecs_vec_get_t(&muts_st->members, ecs_member_t, 0)->type, ecs_id(ecs_f32_t));
    test_uint(ecs_vec_get_t(&muts_st->members, ecs_member_t, 0)->member, 0);

    test_assert(ecs_lookup_child(world, tree, "width") == 0);
    test_assert(ecs_lookup_child(world, muts, "height") == 0);

    ecs_iter_t it = ecs_children(world, tree);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(!ecs_has(world, it.entities[i], EcsMember));
        }
    }

    it = ecs_children(world, muts);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(!ecs_has(world, it.entities[i], EcsMember));
        }
    }

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    const void *ptr = ecs_get_id(world, ent, tree);
    test_assert(ptr != NULL);

    char *str = ecs_ptr_to_expr(world, tree, ptr);
    test_str(str, "{width: 30}");
    ecs_os_free(str);

    ecs_meta_cursor_t cur = ecs_meta_cursor(world, tree, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "width"), 0);
    test_int(ecs_meta_get_int(&cur), 30);
    test_int(ecs_meta_pop(&cur), 0);

    ecs_fini(world);
}

void Template_template_prop_w_component_type_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t existing = ecs_new(world);
    ecs_set(world, existing, Position, {1, 2});

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop pos: Position = {10, 20}"
    LINE "  Position: {$pos.x, $pos.y}"
    LINE "}"
    LINE "Tree ent(pos: {30, 40})";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    const Position *p = ecs_get(world, ent, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Template_template_prop_w_array_of_component_type_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Positions" }),
        .type = ecs_id(Position),
        .count = 3
    });

    ecs_entity_t existing = ecs_new(world);
    ecs_set(world, existing, Position, {1, 2});

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop points: Positions = [{1, 2}, {3, 4}, {5, 6}]"
    LINE "  Position: {$points[1].x, $points[1].y}"
    LINE "}"
    LINE "Tree ent(points: [{10, 20}, {30, 40}, {50, 60}])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    const Position *p = ecs_get(world, ent, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

typedef struct {
    ecs_f32_t values[3];
} TemplateValues;

void Template_template_prop_w_inline_array_member_in_use(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, TemplateValues);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(TemplateValues),
        .members = {
            {"values", ecs_id(ecs_f32_t), .count = 3}
        }
    });

    ecs_entity_t existing = ecs_new(world);
    ecs_set(world, existing, TemplateValues, {{1, 2, 3}});

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop v: TemplateValues = {values: [1, 2, 3]}"
    LINE "  Position: {$v.values[0], $v.values[2]}"
    LINE "}"
    LINE "Tree ent(v: {values: [10, 20, 30]})";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t ent = ecs_lookup(world, "ent");
    test_assert(ent != 0);

    const Position *p = ecs_get(world, ent, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 30);

    ecs_fini(world);
}

typedef struct {
    ecs_f32_t offset;
} TemplateLane;

typedef struct {
    ecs_i32_t lanes;
    TemplateLane lane[4];
} TemplateLight;

static
void template_light_types(ecs_world_t *world) {
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, TemplateLane);
    ECS_COMPONENT(world, TemplateLight);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(TemplateLane),
        .members = {
            {"offset", ecs_id(ecs_f32_t)}
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(TemplateLight),
        .members = {
            {"lanes", ecs_id(ecs_i32_t)},
            {"lane", ecs_id(TemplateLane), .count = 4}
        }
    });
}

void Template_template_base_w_inline_array_member_elem_in_if(void) {
    ecs_world_t *world = ecs_init();

    template_light_types(world);

    const char *expr =
    HEAD "template Light : TemplateLight {"
    LINE "  if lane[0].offset > 0 {"
    LINE "    positive {}"
    LINE "  }"
    LINE "}"
    LINE "Light a(lanes: 1, lane: [{10}, {20}, {30}, {40}])"
    LINE "Light b(lanes: 1, lane: [{0}, {20}, {30}, {40}])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b") != 0);
    test_assert(ecs_lookup(world, "a.positive") != 0);
    test_assert(ecs_lookup(world, "b.positive") == 0);

    ecs_fini(world);
}

void Template_template_base_w_inline_array_member_elem_in_for(void) {
    ecs_world_t *world = ecs_init();

    template_light_types(world);

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Light : TemplateLight {"
    LINE "  for i in 0..lanes {"
    LINE "    \"c_{i}\" { Position: {lane[i].offset, i} }"
    LINE "  }"
    LINE "}"
    LINE "Light a(lanes: 3, lane: [{10}, {20}, {30}, {40}])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t c0 = ecs_lookup(world, "a.c_0");
    ecs_entity_t c1 = ecs_lookup(world, "a.c_1");
    ecs_entity_t c2 = ecs_lookup(world, "a.c_2");
    test_assert(c0 != 0);
    test_assert(c1 != 0);
    test_assert(c2 != 0);
    test_assert(ecs_lookup(world, "a.c_3") == 0);

    const Position *p = ecs_get(world, c0, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 0);

    p = ecs_get(world, c1, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 1);

    p = ecs_get(world, c2, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Template_template_base_w_inline_array_member_reassign(void) {
    ecs_world_t *world = ecs_init();

    template_light_types(world);

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Light : TemplateLight {"
    LINE "  child { Position: {lane[1].offset, lanes} }"
    LINE "}"
    LINE "Light a(lanes: 2, lane: [{10}, {20}, {30}, {40}])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "a.child");
    test_assert(child != 0);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 2);

    const char *update =
    HEAD "Light a(lanes: 3, lane: [{10}, {50}, {30}, {40}])";

    test_assert(ecs_script_run(world, NULL, update, NULL) == 0);

    child = ecs_lookup(world, "a.child");
    test_assert(child != 0);

    p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 50);
    test_int(p->y, 3);

    ecs_fini(world);
}

void Template_template_base_w_inline_array_member_reassign_no_index(void) {
    ecs_world_t *world = ecs_init();

    template_light_types(world);

    ECS_COMPONENT(world, Position);

    const char *expr =
    HEAD "template Light : TemplateLight {"
    LINE "  child { Position: {lanes, 0} }"
    LINE "}"
    LINE "Light a(lanes: 1, lane: [{10}, {20}, {30}, {40}])";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "a.child");
    test_assert(child != 0);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 1);

    const char *update =
    HEAD "Light a(lanes: 4, lane: [{50}, {20}, {30}, {40}])";

    test_assert(ecs_script_run(world, NULL, update, NULL) == 0);

    child = ecs_lookup(world, "a.child");
    test_assert(child != 0);

    p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 4);

    ecs_fini(world);
}
