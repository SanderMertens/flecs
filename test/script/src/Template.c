#include <script.h>

void Template_template_no_scope(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Tree";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t tree = ecs_lookup(world, "Tree");
    test_assert(tree != 0);

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
    LINE "  prop height = flecs.meta.f32: 0"
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
    LINE "  prop height = flecs.meta.f32: 0"
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
    LINE "  prop width = flecs.meta.i32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
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
    LINE "  prop width = i32: 10"
    LINE "  prop height = f32: 20"
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
    LINE "  prop width = flecs.meta.f32: 10"
    LINE "  prop height = flecs.meta.f32: 20"
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
    LINE "  prop width = flecs.meta.f32: 10"
    LINE "  prop height = flecs.meta.f32: 20"
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
    LINE "  prop width = flecs.meta.f32: 10"
    LINE "  prop height = flecs.meta.f32: 20"
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
    LINE "  prop width: 10"
    LINE "  prop height: 20"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child { Position: {$width * 10 + 1, $height * 20 + 2} }"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child { Position: {$width * 10 + 1, $height * 20 + 2} }"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child { Position: {$width * 10 + 1, $height * 20 + 2} }"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child { Position: {$width * 10 + 1, $height * 20 + 2} }"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child {"
    LINE "    Position: {$width, $height}"
    LINE "    grand_child { Position: {$height, $width} }"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  Prefab base {"
    LINE "    Velocity: {$width * 2, $height * 3}"
    LINE "  }"
    LINE "  child : base {"
    LINE "    Position: {$width, $height}"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  Prefab base {"
    LINE "    Velocity: {$width * 2, $height * 3}"
    LINE "    Prefab child {"
    LINE "      Velocity: {$height * 3, $width * 2}"
    LINE "    }"
    LINE "  }"
    LINE "  child : base {"
    LINE "    Position: {$width, $height}"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child {"
    LINE "    Position: {$width, $height}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  tree_1 { Tree: {-$width, -$height} }"
    LINE "  tree_2 { Tree: {$width + 1, $height + 1} }"
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
    LINE "  prop width = flecs.meta.f32: 0"
    LINE "  prop height = flecs.meta.f32: 0"
    LINE "  child { Position: {$width, $height} }"
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
    LINE "  prop count = flecs.meta.f32: 0"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop count = flecs.meta.f32: 0"
    LINE ""
    LINE "  Prefab TreePrefab {"
    LINE "    Tree: {count: $count}"
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
    LINE "  prop t = flecs.meta.f32: 10"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop f = flecs.meta.f32: 20"
    LINE "}"
    LINE ""
    LINE "template Park {"
    LINE "  prop p = flecs.meta.f32: 30"
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
    LINE "  prop count = flecs.meta.f32: 0"
    LINE "  trunk { Position: {$count, $count * 2} }"
    LINE "}"
    LINE ""
    LINE "template Forest {"
    LINE "  prop count = flecs.meta.f32: 0"
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
    LINE "  prop count = flecs.meta.i32: 0"
    LINE "  _ { Position: {$count, $count * 2} }"
    LINE "  _ { Position: {$count, $count * 2} }"
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
    LINE "  prop count = flecs.meta.i32: 0"
    LINE "  _ { Position: {$count, $count * 2} }"
    LINE "  _ { Position: {$count, $count * 2} }"
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
    LINE "  prop pos = Position: {10, 20}"
    LINE "  child { $pos }"
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
    LINE "  prop count = flecs.meta.i32: 0"
    LINE "  with Foo {"
    LINE "    child { Position: {$count, $count * 2} }"
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
    LINE "  prop count = flecs.meta.i32: 0\n"
    LINE "  child { Position: {$count, $count * 2} }\n"
    LINE "}\n"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *expr_inst =
    LINE "t = hello.world.Tree: {10}\n";
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
    LINE "  prop count = flecs.meta.i32: 0"
    LINE "  child { Position: {$count, $count * 2} }"
    LINE "}"
    LINE "template Forest {"
    LINE "  prop count = flecs.meta.i32: 0"
    LINE "  t { Tree: {count:$} }"
    LINE "}"
    LINE "";
    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    const char *expr_inst =
    LINE "f = hello.world.Forest: {10}\n";
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
    LINE "  (Rel, $this)\n"
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
    LINE "  prop x = flecs.meta.entity: flecs\n"
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
    LINE "  const x = flecs.meta.entity: flecs\n"
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

    ECS_ENTITY(world, Rel, PairIsTag);

    const char *expr =
    LINE "template Foo {\n"
    LINE "  (Rel, $this) {\n"
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
    LINE "  prop x = flecs.meta.entity: flecs\n"
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
    LINE "  prop x = flecs.meta.entity: flecs\n"
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
    LINE "  prop height = f32: 0"
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
    LINE "const v: 10"
    LINE "template Tree {"
    LINE "  prop height = f32: 0"
    LINE "  Position: {$v, $height}"
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
    HEAD "const x: 10"
    LINE "parent {"
    LINE "  const y: 20"
    LINE "  template Tree {"
    LINE "    Position: {$x, $y}"
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
    HEAD "const x: 10"
    LINE "parent {"
    HEAD "  const x: 30"
    LINE "  const y: 20"
    LINE "  template Tree {"
    LINE "    Position: {$x, $y}"
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
    LINE "  prop height = f32: 10"
    LINE "  Position: {$height, $height * 2}"
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
    LINE "  prop height = f32: 10"
    LINE "  Position: {$height, $height * 2}"
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
    LINE "    walls = Frame: {}"
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
    LINE "e = Foo: {}"
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

void Template_template_w_with_var(void) {
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
    LINE "  const pos = Position: {10, 20}"
    LINE ""
    LINE "  with $pos {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e = Foo: {}"
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

void Template_template_w_with_prop(void) {
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
    LINE "  prop pos = Position: {10, 20}"
    LINE ""
    LINE "  with $pos {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e = Foo: {"
    LINE "  pos: {30, 40}"
    LINE "}"
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
    test_int(p->x, 30);
    test_int(p->y, 40);

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
    LINE "  const pos = Position: {10, 20}"
    LINE "  child {"
    LINE "    $pos"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e = Foo: {}"
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
    LINE "  prop pos = Position: {0, 0}"
    LINE "  child {"
    LINE "    $pos"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "e = Foo: {{10, 20}}"
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
    LINE "  prop size = i32: 10"
    LINE "  const size_h: $size / 2"
    LINE "  const size_h_2: $size_h + 2"
    LINE "  Position: {$size_h, $size_h_2}"
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
    LINE "  prop x = f32: 0"
    LINE "  prop y = f32: 0"
    LINE "  Velocity: {$x + 5, $y + 5}"
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
    LINE "  b = Ref: {a}"
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
    LINE "  e = Position: {10, 20}"
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
    LINE"     const t: $i"
    LINE "    \"child_$i\" = Position: {$t, $t + 2}"
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

static
void on_foo(ecs_iter_t *it) {
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
    LINE "  prop height = f32: 0"
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
    LINE "  prop height = f32: 0"
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
    LINE "  const x = i32: 10"
    LINE "  prop y = f32: 20"
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
    LINE "  prop x: 10"
    LINE "  const y: x"
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
    LINE "e = Bar: { }"
    LINE "e = Bar: { }";

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
    LINE "e = Bar: { }"
    LINE "e = Bar: { }";

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
    LINE "e = Bar: { }"
    LINE "e = Bar: { }";

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
    HEAD "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE "template Foo {"
    LINE "  prop x = f32: 0"
    LINE "  prop y = f32: 0"
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
    HEAD "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE "Tgt {}"
    LINE "template Foo {"
    LINE "  prop x = f32: 0"
    LINE "  prop y = f32: 0"
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
