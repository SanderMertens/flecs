#include <script.h>

void Mut_declaration(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.i32: 10"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    test_assert(ecs_has(world, mut, EcsComponent));
    test_assert(ecs_has_pair(world, foo, EcsWith, mut));

    const EcsStruct *st = ecs_get(world, mut, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "value");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type,
        ecs_id(ecs_i32_t));

    test_assert(ecs_get(world, foo, EcsStruct) == NULL);

    ecs_fini(world);
}

void Mut_two_members(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut x = flecs.meta.i32: 10"
    LINE "  mut y = flecs.meta.f32: 20"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);

    const EcsStruct *st = ecs_get(world, mut, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 0)->name, "x");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type,
        ecs_id(ecs_i32_t));
    test_str(ecs_vec_get_t(&st->members, ecs_member_t, 1)->name, "y");
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type,
        ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Mut_implicit_type(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    const EcsStruct *st = ecs_get(world, mut, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type,
        ecs_id(ecs_i64_t));

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const int64_t *value = ecs_get_id(world, e, mut);
    test_assert(value != NULL);
    test_int(*value, 10);

    ecs_fini(world);
}

void Mut_instance_w_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut x = flecs.meta.i32: 10"
    LINE "  mut y = flecs.meta.i32: 20"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, foo));
    test_assert(ecs_has_id(world, e, mut));

    const int32_t *value = ecs_get_id(world, e, mut);
    test_assert(value != NULL);
    test_int(value[0], 10);
    test_int(value[1], 20);

    ecs_fini(world);
}

void Mut_instance_w_props_and_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  prop width = flecs.meta.i32: 10"
    LINE "  mut selected = flecs.meta.bool: true"
    LINE "  prop height = flecs.meta.i32: 20"
    LINE "  mut count = flecs.meta.i32: 30"
    LINE "}"
    LINE "Foo e(40, 50)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const EcsStruct *foo_st = ecs_get(world, foo, EcsStruct);
    test_assert(foo_st != NULL);
    test_int(foo_st->members.count, 2);
    test_str(ecs_vec_get_t(&foo_st->members, ecs_member_t, 0)->name, "width");
    test_str(ecs_vec_get_t(&foo_st->members, ecs_member_t, 1)->name, "height");

    const EcsStruct *mut_st = ecs_get(world, mut, EcsStruct);
    test_assert(mut_st != NULL);
    test_int(mut_st->members.count, 2);
    test_str(ecs_vec_get_t(&mut_st->members, ecs_member_t, 0)->name,
        "selected");
    test_str(ecs_vec_get_t(&mut_st->members, ecs_member_t, 1)->name, "count");

    const int32_t *props = ecs_get_id(world, e, foo);
    test_assert(props != NULL);
    test_int(props[0], 40);
    test_int(props[1], 50);

    const void *mut_ptr = ecs_get_id(world, e, mut);
    test_assert(mut_ptr != NULL);
    char *str = ecs_ptr_to_expr(world, mut, mut_ptr);
    test_assert(str != NULL);
    test_str(str, "{selected: true, count: 30}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Mut_value_in_template_body(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, $value * 2}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_value_w_prop_in_template_body(void) {
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
    LINE "template Foo {"
    LINE "  mut x = flecs.meta.f32: 10"
    LINE "  prop scale = flecs.meta.f32: 2"
    LINE "  mut y = flecs.meta.f32: 20"
    LINE "  Position: {$x * $scale, $y * $scale}"
    LINE "}"
    LINE "Foo e(3)";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}

void Mut_modified_reinstantiates(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, $value * 2}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    float *value = ecs_get_mut_id(world, e, mut);
    test_assert(value != NULL);
    *value = 30;
    ecs_modified_id(world, e, mut);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 60);

    ecs_fini(world);
}

void Mut_set_reinstantiates(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, $value * 2}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    float value = 40;
    ecs_set_id(world, e, mut, sizeof(float), &value);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 40);
    test_int(p->y, 80);

    ecs_fini(world);
}

void Mut_modified_reinstantiates_only_instance(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, 0}"
    LINE "}"
    LINE "Foo e1()"
    LINE "Foo e2()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e1 = ecs_lookup(world, "e1");
    test_assert(e1 != 0);
    ecs_entity_t e2 = ecs_lookup(world, "e2");
    test_assert(e2 != 0);

    float *value = ecs_get_mut_id(world, e1, mut);
    test_assert(value != NULL);
    *value = 20;
    ecs_modified_id(world, e1, mut);

    const Position *p1 = ecs_get(world, e1, Position);
    test_assert(p1 != NULL);
    test_int(p1->x, 20);
    const Position *p2 = ecs_get(world, e2, Position);
    test_assert(p2 != NULL);
    test_int(p2->x, 10);

    ecs_fini(world);
}

void Mut_deferred_modified_reinstantiates(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, $value * 2}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_defer_begin(world);
    float *value = ecs_get_mut_id(world, e, mut);
    test_assert(value != NULL);
    *value = 50;
    ecs_modified_id(world, e, mut);
    ecs_defer_end(world);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 50);
    test_int(p->y, 100);

    ecs_fini(world);
}

void Mut_bulk_create(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.i32: 10"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);

    const ecs_entity_t *entities = ecs_bulk_init(world, &(ecs_bulk_desc_t) {
        .count = 3,
        .ids = { foo }
    });
    test_assert(entities != NULL);

    int32_t i;
    for (i = 0; i < 3; i ++) {
        test_assert(ecs_has_id(world, entities[i], foo));
        const int32_t *value = ecs_get_id(world, entities[i], mut);
        test_assert(value != NULL);
        test_int(*value, 10);
    }

    ecs_fini(world);
}

void Mut_prop_update_preserves_mut(void) {
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
    LINE "template Foo {"
    LINE "  prop scale = flecs.meta.f32: 2"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value * $scale, 0}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    float *value = ecs_get_mut_id(world, e, mut);
    test_assert(value != NULL);
    *value = 3;
    ecs_modified_id(world, e, mut);

    float scale = 4;
    ecs_set_id(world, e, foo, sizeof(float), &scale);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 12);
    const float *state = ecs_get_id(world, e, mut);
    test_assert(state != NULL);
    test_int(*state, 3);

    ecs_fini(world);
}

void Mut_mut_component_without_template(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, 0}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);

    ecs_entity_t e = ecs_new(world);
    float value = 20;
    ecs_set_id(world, e, mut, sizeof(float), &value);
    test_assert(ecs_has_id(world, e, mut));
    test_assert(!ecs_has_id(world, e, foo));
    test_assert(!ecs_has(world, e, Position));

    ecs_fini(world);
}

void Mut_string_default(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut text = flecs.meta.string: \"hello\""
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const ecs_string_t *text = ecs_get_id(world, e, mut);
    test_assert(text != NULL);
    test_str(*text, "hello");

    ecs_fini(world);
}

void Mut_const_from_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "  const result: value * 2"
    LINE "  flecs.meta.i64: {$result}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const int64_t *result = ecs_get(world, e, ecs_i64_t);
    test_assert(result != NULL);
    test_int(*result, 20);

    ecs_fini(world);
}

void Mut_script_update_reinstantiates(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  Position: {$value, 0}"
    LINE "}"
    LINE "Foo e()"
    LINE "e { Foo.mut: {20} }";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    ecs_fini(world);
}

void Mut_not_exposed_as_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "}"
    LINE "Foo e(20)";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_outside_template(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "mut value = flecs.meta.i32: 10";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_after_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  const value: 10"
    LINE "  mut state: 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_redeclare_prop_as_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  prop value: 10"
    LINE "  mut value: 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_redeclare_mut_as_prop(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "  prop value: 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_using(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "using flecs.meta"
    LINE "template Foo {"
    LINE "  mut x = i32: 10"
    LINE "  mut y = f32: 20"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    const EcsStruct *st = ecs_get(world, mut, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 2);
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type,
        ecs_id(ecs_i32_t));
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 1)->type,
        ecs_id(ecs_f32_t));

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    char *str = ecs_ptr_to_expr(world, mut, ptr);
    test_assert(str != NULL);
    test_str(str, "{x: 10, y: 20}");
    ecs_os_free(str);

    ecs_fini(world);
}

void Mut_type_without_using_meta(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value = f32: 10"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    const EcsStruct *st = ecs_get(world, mut, EcsStruct);
    test_assert(st != NULL);
    test_uint(ecs_vec_get_t(&st->members, ecs_member_t, 0)->type,
        ecs_id(ecs_f32_t));

    ecs_fini(world);
}

void Mut_composite(void) {
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
    LINE "template Foo {"
    LINE "  mut pos = Position: {10, 20}"
    LINE "  child { Position: $pos }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    Position *state = ecs_get_mut_id(world, e, mut);
    test_assert(state != NULL);
    state->x = 30;
    state->y = 40;
    ecs_modified_id(world, e, mut);

    child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_fini(world);
}

void Mut_with_mut(void) {
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
    LINE "  mut pos = Position: {10, 20}"
    LINE "  with $pos {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_entity_pair(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {"
    LINE "  mut target = flecs.meta.entity: flecs.core"
    LINE "  (Rel, $target)"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, Rel, EcsFlecsCore));

    ecs_entity_t *target = ecs_get_mut_id(world, e, mut);
    test_assert(target != NULL);
    *target = EcsFlecs;
    ecs_modified_id(world, e, mut);

    test_assert(ecs_has_pair(world, e, Rel, EcsFlecs));

    ecs_fini(world);
}

void Mut_entity_pair_scope(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Rel);

    const char *expr =
    LINE "template Foo {"
    LINE "  mut target = flecs.meta.entity: flecs.core"
    LINE "  (Rel, $target) {"
    LINE "    child {}"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_assert(ecs_has_pair(world, child, Rel, EcsFlecsCore));

    ecs_fini(world);
}

void Mut_pair_component_entity_target(void) {
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
    LINE "template Foo {"
    LINE "  mut target = flecs.meta.entity: flecs.core"
    LINE "  (Position, $target): {5, 6}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get_pair(world, e, Position, EcsFlecsCore);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 6);

    ecs_fini(world);
}

void Mut_anonymous_instance(void) {
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
    LINE "  mut value = f32: 10"
    LINE "  Position: {$value, $value * 2}"
    LINE "}"
    LINE "Foo()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_iter_t it = ecs_each_id(world, foo);
    test_bool(true, ecs_each_next(&it));
    test_int(it.count, 1);
    ecs_entity_t e = it.entities[0];
    test_assert(ecs_has_id(world, e, mut));
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_managed_script_twice(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  child { Position: {$value, $value * 2} }"
    LINE "}"
    LINE "Foo e()";

    ecs_entity_t script = ecs_entity(world, { .name = "main" });
    test_assert(ecs_script(world, {
        .entity = script,
        .code = expr
    }) != 0);
    test_assert(ecs_script(world, {
        .entity = script,
        .code = expr
    }) != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    const EcsStruct *st = ecs_get(world, mut, EcsStruct);
    test_assert(st != NULL);
    test_int(st->members.count, 1);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, mut));
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_module(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "module hello.world"
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.i32: 10"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);
    test_assert(ecs_script_run(world, NULL,
        "hello.world.Foo e()", NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "hello.world.Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const int32_t *value = ecs_get_id(world, e, mut);
    test_assert(value != NULL);
    test_int(*value, 10);

    ecs_fini(world);
}

void Mut_multiple_templates(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.i32: 10"
    LINE "}"
    LINE "template Bar {"
    LINE "  mut value = flecs.meta.i32: 20"
    LINE "}"
    LINE "template Baz {"
    LINE "  mut value = flecs.meta.i32: 30"
    LINE "}"
    LINE "Foo a()"
    LINE "Bar b()"
    LINE "Baz c()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t bar = ecs_lookup(world, "Bar");
    ecs_entity_t baz = ecs_lookup(world, "Baz");
    test_assert(foo != 0);
    test_assert(bar != 0);
    test_assert(baz != 0);
    ecs_entity_t foo_mut = ecs_lookup_child(world, foo, "mut");
    ecs_entity_t bar_mut = ecs_lookup_child(world, bar, "mut");
    ecs_entity_t baz_mut = ecs_lookup_child(world, baz, "mut");
    test_assert(foo_mut != 0);
    test_assert(bar_mut != 0);
    test_assert(baz_mut != 0);
    test_assert(foo_mut != bar_mut);
    test_assert(foo_mut != baz_mut);
    test_assert(bar_mut != baz_mut);

    const int32_t *a = ecs_get_id(world, ecs_lookup(world, "a"), foo_mut);
    const int32_t *b = ecs_get_id(world, ecs_lookup(world, "b"), bar_mut);
    const int32_t *c = ecs_get_id(world, ecs_lookup(world, "c"), baz_mut);
    test_assert(a != NULL);
    test_assert(b != NULL);
    test_assert(c != NULL);
    test_int(*a, 10);
    test_int(*b, 20);
    test_int(*c, 30);

    ecs_fini(world);
}

void Mut_anonymous_children(void) {
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
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.i32: 10"
    LINE "  _ { Position: {$value, 0} }"
    LINE "  _ { Position: {$value, 0} }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    int32_t *value = ecs_get_mut_id(world, e, mut);
    test_assert(value != NULL);
    *value = 20;
    ecs_modified_id(world, e, mut);

    ecs_query_t *q = ecs_query(world, {
        .terms = {
            { .id = ecs_childof(e) },
            { .id = ecs_id(Position) }
        }
    });
    ecs_iter_t it = ecs_query_iter(world, q);
    test_int(ecs_iter_count(&it), 2);
    ecs_iter_t verify = ecs_query_iter(world, q);
    while (ecs_query_next(&verify)) {
        const Position *p = ecs_field(&verify, Position, 1);
        int32_t i;
        for (i = 0; i < verify.count; i ++) {
            test_int(p[i].x, 20);
        }
    }
    ecs_query_fini(q);

    ecs_fini(world);
}

void Mut_fold_const(void) {
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
    LINE "  mut size = i32: 10"
    LINE "  const half: $size / 2"
    LINE "  const adjusted: $half + 2"
    LINE "  Position: {$half, $adjusted}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    int32_t *size = ecs_get_mut_id(world, e, mut);
    test_assert(size != NULL);
    *size = 16;
    ecs_modified_id(world, e, mut);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 8);
    test_int(p->y, 10);

    ecs_fini(world);
}

void Mut_assign_add(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  mut value = f32: 6"
    LINE "  Position: {y += $value / 2}"
    LINE "}"
    HEAD "e {"
    LINE "  Position: {10, 20}"
    LINE "  Foo: {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 23);

    ecs_fini(world);
}

void Mut_assign_mul(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    const char *expr =
    HEAD "template Foo {"
    LINE "  mut value = f32: 6"
    LINE "  Position: {y *= $value / 2}"
    LINE "}"
    HEAD "e {"
    LINE "  Position: {10, 20}"
    LINE "  Foo: {}"
    LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 60);

    ecs_fini(world);
}

void Mut_script_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE "template Foo {"
    LINE "  mut x = f32: 10"
    LINE "  mut y = f32: 20"
    LINE "  Position: {$x, $y}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ECS_COMPONENT(world, Position);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_script_pair_component(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "struct Position {"
    LINE "  x = f32"
    LINE "  y = f32"
    LINE "}"
    LINE "Target {}"
    LINE "template Foo {"
    LINE "  mut x = f32: 10"
    LINE "  mut y = f32: 20"
    LINE "  (Position, Target): {$x, $y}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ECS_COMPONENT(world, Position);
    ecs_entity_t target = ecs_lookup(world, "Target");
    test_assert(target != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get_id(
        world, e, ecs_pair(ecs_id(Position), target));
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_tree_parent(void) {
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
    LINE "template Foo {"
    LINE "  mut value = i32: 10"
    LINE "  child { Position: {$value, $value * 2} }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t child = ecs_lookup_child(world, e, "child");
    test_assert(child != 0);

    int32_t *value = ecs_get_mut_id(world, e, mut);
    test_assert(value != NULL);
    *value = 20;
    ecs_modified_id(world, e, mut);

    child = ecs_lookup_child(world, e, "child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 40);
    const EcsParent *parent = ecs_get(world, child, EcsParent);
    test_assert(parent != NULL);
    test_uint(parent->value, e);

    ecs_fini(world);
}

void Mut_child_name_from_string(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut suffix = flecs.meta.string: \"a\""
    LINE "  \"child_$suffix\" {}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_lookup(world, "e.child_a") != 0);

    ecs_string_t *suffix = ecs_get_mut_id(world, e, mut);
    test_assert(suffix != NULL);
    ecs_os_free(*suffix);
    *suffix = ecs_os_strdup("hello");
    ecs_modified_id(world, e, mut);

    test_assert(ecs_lookup(world, "e.child_a") == 0);
    test_assert(ecs_lookup(world, "e.child_hello") != 0);

    ecs_fini(world);
}

void Mut_default_component(void) {
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
    HEAD "DefaultChildComponent Holder(Position)"
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 5"
    LINE "  Holder holder {"
    LINE "    child = $value, 20"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.holder.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_default_component_nested_if(void) {
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
    HEAD "DefaultChildComponent Holder(Position)"
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 5"
    LINE "  Holder holder {"
    LINE "    if true {"
    LINE "      if true {"
    LINE "        child = $value, 20"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.holder.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_default_component_nested_for(void) {
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
    HEAD "DefaultChildComponent Holder(Position)"
    LINE "template Foo {"
    LINE "  mut value = flecs.meta.f32: 5"
    LINE "  Holder holder {"
    LINE "    for i in 0..2 {"
    LINE "      for j in 0..2 {"
    LINE "        \"child_{$i}_{$j}\" = $value, 20"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    int32_t i, j;
    for (i = 0; i < 2; i ++) {
        for (j = 0; j < 2; j ++) {
            char name[64];
            ecs_os_snprintf(name, 64, "e.holder.child_%d_%d", i, j);
            ecs_entity_t child = ecs_lookup(world, name);
            test_assert(child != 0);
            const Position *p = ecs_get(world, child, Position);
            test_assert(p != NULL);
            test_int(p->x, 5);
            test_int(p->y, 20);
        }
    }

    ecs_fini(world);
}

void Mut_value_name(void) {
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
    LINE "  mut value = i32: 10"
    LINE "  child { Position: {value, value * 2} }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_const_value_name(void) {
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
    LINE "  mut x = i32: 10"
    LINE "  const value: 20"
    LINE "  child { Position: {x, value} }"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_hoist_var(void) {
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
    LINE "const global: 10"
    LINE "template Foo {"
    LINE "  mut value = f32: 20"
    LINE "  Position: {$global, $value}"
    LINE "}"
    LINE "Foo e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_nested_template(void) {
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
    LINE "template Inner {"
    LINE "  mut value = flecs.meta.f32: 10"
    LINE "  child { Position: {$value, $value * 2} }"
    LINE "}"
    LINE "template Outer {"
    LINE "  Inner inner()"
    LINE "}"
    LINE "Outer e()";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    ecs_entity_t inner_type = ecs_lookup(world, "Inner");
    test_assert(inner_type != 0);
    ecs_entity_t mut = ecs_lookup_child(world, inner_type, "mut");
    test_assert(mut != 0);
    ecs_entity_t inner = ecs_lookup(world, "e.inner");
    test_assert(inner != 0);
    test_assert(ecs_has_id(world, inner, mut));
    ecs_entity_t child = ecs_lookup(world, "e.inner.child");
    test_assert(child != 0);
    const Position *p = ecs_get(world, child, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Mut_redeclare_mut_as_mut(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "  mut value: 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_redeclare_mut_as_const(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "  const value: 20"
    LINE "}";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_managed_script_twice_after_low_id_exhaustion(void) {
    ecs_world_t *world = ecs_init();

    int32_t i;
    for (i = 0; i < 300; i ++) {
        test_assert(ecs_component_init(world, &(ecs_component_desc_t){
            .type.size = sizeof(int32_t),
            .type.alignment = ECS_ALIGNOF(int32_t)
        }) != 0);
    }

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 10"
    LINE "  i64: {value}"
    LINE "}"
    LINE "Foo e()";

    ecs_entity_t script = ecs_entity(world, { .name = "main" });
    test_assert(ecs_script(world, {
        .entity = script,
        .code = expr
    }) != 0);
    test_assert(ecs_script(world, {
        .entity = script,
        .code = expr
    }) != 0);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    ecs_entity_t mut = ecs_lookup_child(world, foo, "mut");
    test_assert(mut != 0);
    test_assert((mut >> 32) != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    const int64_t *state = ecs_get_id(world, e, mut);
    test_assert(state != NULL);
    test_int(*state, 10);
    const int64_t *value = ecs_get(world, e, ecs_i64_t);
    test_assert(value != NULL);
    test_int(*value, 10);

    ecs_fini(world);
}

void Mut_eval_error(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 0"
    LINE "  i64: {10 / value}"
    LINE "}"
    LINE "Foo e()";

    ecs_log_set_level(-4);
    test_assert(ecs_script_run(world, NULL, expr, NULL) != 0);

    ecs_fini(world);
}

void Mut_eval_error_w_runtime(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    LINE "template Foo {"
    LINE "  mut value: 0"
    LINE "  i64: {10 / value}"
    LINE "}"
    LINE "Foo e()";

    ecs_script_t *script = ecs_script_parse(world, NULL, expr, NULL, NULL);
    test_assert(script != NULL);

    ecs_script_runtime_t *runtime = ecs_script_runtime_new();
    ecs_script_eval_desc_t desc = { .runtime = runtime };

    ecs_log_set_level(-4);
    test_assert(ecs_script_eval(script, &desc, NULL) != 0);

    ecs_script_runtime_free(runtime);
    ecs_script_free(script);
    ecs_fini(world);
}
