#include <script.h>

static
void times_two(
    const ecs_function_ctx_t *ctx,
    int argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    *(ecs_f32_t*)result->ptr = *(ecs_f32_t*)argv[0].ptr * 2;
}

static
ecs_entity_t define_mass(ecs_world_t *world) {
    return ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
}

static
ecs_entity_t define_position(ecs_world_t *world) {
    return ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
}

static
ecs_entity_t define_position_i(ecs_world_t *world) {
    return ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
}

void Refs_ref_in_component_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_const_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const m: e[Mass]"
            LINE "foo {"
            LINE "  Position: {$m.value, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_with_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "with Position(e[Mass].value, 0) {"
            LINE "  foo {}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, PositionI, {1, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: match e[PositionI].x {"
            LINE "    1: {100, 0}"
            LINE "    2: {200, 0}"
            LINE "    _: {0, 0}"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 100);
    }

    ecs_set(world, e, PositionI, {2, 0});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 200);
    }

    ecs_fini(world);
}

void Refs_ref_in_if_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, PositionI, {1, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if e[PositionI].x > 0 {"
            LINE "  foo {}"
            LINE "} else {"
            LINE "  bar {}"
            LINE "}"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") == 0);

    ecs_set(world, e, PositionI, {0, 0});

    test_assert(ecs_lookup(world, "foo") == 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_fini(world);
}

void Refs_non_managed_script_no_update(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    const char *expr =
        HEAD "foo {"
        LINE "  Position: {e[Mass].value, 0}"
        LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void Refs_set_after_managed_script_deleted(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_delete(world, s);
    test_assert(!ecs_is_alive(world, s));

    ecs_set(world, e, Mass, {30});

    {
        const Mass *m = ecs_get(world, e, Mass);
        test_assert(m != NULL);
        test_int(m->value, 30);
    }

    ecs_fini(world);
}

void Refs_ref_in_for_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, PositionI, {2, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..e[PositionI].x {"
            LINE "  \"e_{$i}\" {"
            LINE "    Position: {$i, 0}"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") == 0);

    ecs_set(world, e, PositionI, {3, 0});

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") != 0);
    test_assert(ecs_lookup(world, "e_3") == 0);

    ecs_fini(world);
}

void Refs_global_const_var_in_component_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {$v, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_with_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "with Position($v, 0) {"
            LINE "  foo {}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &(ecs_i32_t){1}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: match $v {"
            LINE "    1: {100, 0}"
            LINE "    2: {200, 0}"
            LINE "    _: {0, 0}"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 100);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_i32_t*)cv->value.ptr = 2;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 200);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_if_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &(ecs_i32_t){1}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if $v > 0 {"
            LINE "  foo {}"
            LINE "} else {"
            LINE "  bar {}"
            LINE "}"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "foo") != 0);
    test_assert(ecs_lookup(world, "bar") == 0);

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_i32_t*)cv->value.ptr = 0;
    ecs_modified(world, v, EcsScriptConstVar);

    test_assert(ecs_lookup(world, "foo") == 0);
    test_assert(ecs_lookup(world, "bar") != 0);

    ecs_fini(world);
}

void Refs_global_const_var_in_for_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &(ecs_i32_t){2}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..$v {"
            LINE "  \"e_{$i}\" {"
            LINE "    Position: {$i, 0}"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") == 0);

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_i32_t*)cv->value.ptr = 3;
    ecs_modified(world, v, EcsScriptConstVar);

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") != 0);
    test_assert(ecs_lookup(world, "e_3") == 0);

    ecs_fini(world);
}

void Refs_global_const_var_non_managed_script_no_update(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    const char *expr =
        HEAD "foo {"
        LINE "  Position: {$v, 0}"
        LINE "}";

    test_assert(ecs_script_run(world, NULL, expr, NULL) == 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void Refs_global_const_var_set_after_managed_script_deleted(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {$v, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_delete(world, s);
    test_assert(!ecs_is_alive(world, s));

    cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 30;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_value_t val = ecs_const_var_get(world, v);
        test_assert(val.ptr != NULL);
        test_int(*(ecs_f32_t*)val.ptr, 30);
    }

    ecs_fini(world);
}

void Refs_global_const_var_modified(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {$v, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_value_t val = ecs_const_var_get(world, v);
    test_assert(val.ptr != NULL);
    *(ecs_f32_t*)val.ptr = 20;
    ecs_const_var_modified(world, v);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {times_two(e[Mass].value), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_set(world, e, Mass, {30});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 60);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {times_two($v), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 30;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 60);
    }

    ecs_fini(world);
}

void Refs_ref_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  entity: { new { Position: {e[Mass].value, 0} } }"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  entity: { new { Position: {$v, 0} } }"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_function_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  entity: { new { Position: {times_two(e[Mass].value), 0} } }"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_set(world, e, Mass, {30});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 60);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_function_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  entity: { new { Position: {times_two($v), 0} } }"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 30;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const ecs_entity_t *ptr = ecs_get(world, foo, ecs_entity_t);
        test_assert(ptr != NULL);
        ecs_entity_t x = *ptr;
        test_assert(x != 0);
        const Position *p = ecs_get(world, x, Position);
        test_assert(p != NULL);
        test_int(p->x, 60);
    }

    ecs_fini(world);
}

void Refs_ref_in_script_function_body(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "fn get_mass() -> f32 { e[Mass].value }"
            LINE "foo {"
            LINE "  Position: {get_mass(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_script_function_body(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "fn get_v() -> f32 { $v }"
            LINE "foo {"
            LINE "  Position: {get_v(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_assigned_to_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: e[Position]"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    ecs_set(world, e, Position, {30, 40});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

void Refs_global_const_var_assigned_to_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(Position),
        .value = &(Position){10, 20}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: $v"
            LINE "}"
    });
    test_assert(s != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 20);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    ((Position*)cv->value.ptr)->x = 30;
    ((Position*)cv->value.ptr)->y = 40;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 30);
        test_int(p->y, 40);
    }

    ecs_fini(world);
}

static
void SetMass(ecs_iter_t *it) {
    ecs_entity_t e = ecs_lookup(it->world, "e");
    ecs_entity_t mass = ecs_lookup(it->world, "Mass");
    ecs_set_id(it->world, e, mass, sizeof(float), &(float){ 20 });
}

void Refs_reeval_hierarchy(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ECS_TAG(world, TagP1);
    ECS_TAG(world, TagP2);
    ECS_TAG(world, TagC1);
    ECS_TAG(world, TagC2);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "gp {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "  p1 {"
            LINE "    TagP1"
            LINE "    c1 { TagC1 }"
            LINE "  }"
            LINE "  p2 {"
            LINE "    TagP2"
            LINE "    c2 { TagC2 }"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "gp") != 0);
    test_assert(ecs_lookup(world, "gp.p1.c1") != 0);
    test_assert(ecs_lookup(world, "gp.p2.c2") != 0);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .callback = SetMass
    });

    ecs_progress(world, 0);

    test_assert(ecs_lookup(world, "gp") != 0);
    test_assert(ecs_lookup(world, "gp.p1.c1") != 0);
    test_assert(ecs_lookup(world, "gp.p2.c2") != 0);

    ecs_fini(world);
}

void Refs_reeval_prefab_in_branch(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_entity_t assets = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "assets" }),
        .code =
            HEAD "Prefab base_prefab {"
            LINE "  Position: {1, 2}"
            LINE "  turret { Position: {3, 4} }"
            LINE "}"
    });
    test_assert(assets != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if e[Mass].value < 15 {"
            LINE "  world_root {"
            LINE "    base : base_prefab {"
            LINE "      Position: {e[Mass].value, 0}"
            LINE "      slot { Position: {7, 8} }"
            LINE "    }"
            LINE "  }"
            LINE "} else {"
            LINE "  replacement { Position: {e[Mass].value, 0} }"
            LINE "}"
    });
    test_assert(s != 0);

    int32_t i;
    for (i = 0; i < 8; i ++) {
        ecs_set(world, e, Mass, {20 + i});
    }

    ecs_fini(world);
}

static
void RefDummyObserver(ecs_iter_t *it);

void Refs_progress_reeval_prefab_w_isa_observer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_observer(world, {
        .query.terms = {{
            .id = ecs_id(Position),
            .src.id = EcsUp,
            .trav = EcsIsA
        }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = RefDummyObserver
    });

    ecs_entity_t assets = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "assets" }),
        .code =
            HEAD "Prefab base_prefab {"
            LINE "  Position: {1, 2}"
            LINE "  turret { Position: {3, 4} }"
            LINE "}"
    });
    test_assert(assets != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "@tree Parent"
            LINE "world_root {"
            LINE "  base : base_prefab {"
            LINE "    Position: {e[Mass].value, 0}"
            LINE "    slot { Position: {7, 8} }"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .callback = SetMass
    });

    int32_t i;
    for (i = 0; i < 2; i ++) {
        ecs_progress(world, 0);
    }

    ecs_fini(world);
}

static
void RefDummyObserver(ecs_iter_t *it) {
    (void)it;
}

void Refs_reeval_prefab_w_isa_observer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_observer(world, {
        .query.terms = {{
            .id = ecs_id(Position),
            .src.id = EcsUp,
            .trav = EcsIsA
        }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = RefDummyObserver
    });

    ecs_entity_t assets = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "assets" }),
        .code =
            HEAD "Prefab base_prefab {"
            LINE "  Position: {1, 2}"
            LINE "  turret { Position: {3, 4} }"
            LINE "}"
    });
    test_assert(assets != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "@tree Parent"
            LINE "world_root {"
            LINE "  base : base_prefab {"
            LINE "    Position: {e[Mass].value, 0}"
            LINE "    slot { Position: {7, 8} }"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_set(world, e, Mass, {20});
    ecs_set(world, e, Mass, {21});

    ecs_fini(world);
}

void Refs_reeval_prefab_delete_with_inherited_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);
    ecs_add_pair(world, ecs_id(Position), EcsOnInstantiate, EcsInherit);

    ecs_observer(world, {
        .query.terms = {{
            .id = ecs_id(Position),
            .src.id = EcsUp,
            .trav = EcsIsA
        }},
        .events = { EcsOnAdd, EcsOnRemove },
        .callback = RefDummyObserver
    });

    ecs_entity_t assets = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "assets" }),
        .code =
            HEAD "Prefab base_prefab {"
            LINE "  Position: {1, 2}"
            LINE "  turret { Position: {3, 4} }"
            LINE "}"
    });
    test_assert(assets != 0);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "@tree Parent"
            LINE "world_root {"
            LINE "  base : base_prefab {"
            LINE "    Position: {e[Mass].value, 0}"
            LINE "    slot { Position: {7, 8} }"
            LINE "  }"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_set(world, e, Mass, {20});
    ecs_set(world, e, Mass, {21});

    ecs_delete_with(world, ecs_id(Position));

    ecs_fini(world);
}

void Refs_ref_in_template_not_instantiated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {100, 0}"
            LINE "}"
            LINE "template Bar {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    {
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 100);
    }

    ecs_set(world, foo, Position, {999, 0});

    ecs_set(world, e, Mass, {20});

    {
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 999);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_component_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    ecs_entity_t inst3 = ecs_lookup(world, "inst3");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);
    test_assert(inst3 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_assert(p3 != NULL);
        test_int(p1->x, 10);
        test_int(p2->x, 10);
        test_int(p3->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_assert(p3 != NULL);
        test_int(p1->x, 20);
        test_int(p2->x, 20);
        test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_component_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {$v, 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    ecs_entity_t inst3 = ecs_lookup(world, "inst3");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);
    test_assert(inst3 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_assert(p3 != NULL);
        test_int(p1->x, 10);
        test_int(p2->x, 10);
        test_int(p3->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_assert(p3 != NULL);
        test_int(p1->x, 20);
        test_int(p2->x, 20);
        test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_with_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  with Position(e[Mass].value, 0) {"
            LINE "    foo {}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, PositionI, {1, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: match e[PositionI].x {"
            LINE "    1: {100, 0}"
            LINE "    2: {200, 0}"
            LINE "    _: {0, 0}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 100); test_int(p2->x, 100); test_int(p3->x, 100);
    }

    ecs_set(world, e, PositionI, {2, 0});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 200); test_int(p2->x, 200); test_int(p3->x, 200);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_if_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, PositionI, {1, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  if e[PositionI].x > 0 {"
            LINE "    foo {}"
            LINE "  } else {"
            LINE "    bar {}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.foo") != 0);
    test_assert(ecs_lookup(world, "inst2.foo") != 0);
    test_assert(ecs_lookup(world, "inst3.foo") != 0);
    test_assert(ecs_lookup(world, "inst1.bar") == 0);
    test_assert(ecs_lookup(world, "inst2.bar") == 0);
    test_assert(ecs_lookup(world, "inst3.bar") == 0);

    ecs_set(world, e, PositionI, {0, 0});

    test_assert(ecs_lookup(world, "inst1.foo") == 0);
    test_assert(ecs_lookup(world, "inst2.foo") == 0);
    test_assert(ecs_lookup(world, "inst3.foo") == 0);
    test_assert(ecs_lookup(world, "inst1.bar") != 0);
    test_assert(ecs_lookup(world, "inst2.bar") != 0);
    test_assert(ecs_lookup(world, "inst3.bar") != 0);

    ecs_fini(world);
}

void Refs_ref_in_template_for_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, PositionI, {2, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  for i in 0..e[PositionI].x {"
            LINE "    \"e_{$i}\" {}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.e_0") != 0);
    test_assert(ecs_lookup(world, "inst1.e_1") != 0);
    test_assert(ecs_lookup(world, "inst1.e_2") == 0);
    test_assert(ecs_lookup(world, "inst3.e_1") != 0);
    test_assert(ecs_lookup(world, "inst3.e_2") == 0);

    ecs_set(world, e, PositionI, {3, 0});

    test_assert(ecs_lookup(world, "inst1.e_2") != 0);
    test_assert(ecs_lookup(world, "inst2.e_2") != 0);
    test_assert(ecs_lookup(world, "inst3.e_2") != 0);
    test_assert(ecs_lookup(world, "inst1.e_3") == 0);

    ecs_fini(world);
}

void Refs_ref_in_template_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {times_two(e[Mass].value), 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_set(world, e, Mass, {30});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 60); test_int(p3->x, 60);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  entity: { new { Position: {e[Mass].value, 0} } }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_function_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  entity: { new { Position: {times_two(e[Mass].value), 0} } }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_set(world, e, Mass, {30});

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 60); test_int(p3->x, 60);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_script_function_body(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "fn get_mass() -> f32 { e[Mass].value }"
            LINE "template Bar {"
            LINE "  Position: {get_mass(), 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_ref_in_template_assigned_to_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Position, {10, 20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: e[Position]"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    ecs_set(world, e, Position, {30, 40});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 30); test_int(p3->x, 30);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_with_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  with Position($v, 0) {"
            LINE "    foo {}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &(ecs_i32_t){1}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: match $v {"
            LINE "    1: {100, 0}"
            LINE "    2: {200, 0}"
            LINE "    _: {0, 0}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 100); test_int(p2->x, 100); test_int(p3->x, 100);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_i32_t*)cv->value.ptr = 2;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 200); test_int(p2->x, 200); test_int(p3->x, 200);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_if_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &(ecs_i32_t){1}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  if $v > 0 {"
            LINE "    foo {}"
            LINE "  } else {"
            LINE "    bar {}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.foo") != 0);
    test_assert(ecs_lookup(world, "inst2.foo") != 0);
    test_assert(ecs_lookup(world, "inst3.foo") != 0);
    test_assert(ecs_lookup(world, "inst1.bar") == 0);

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_i32_t*)cv->value.ptr = 0;
    ecs_modified(world, v, EcsScriptConstVar);

    test_assert(ecs_lookup(world, "inst1.foo") == 0);
    test_assert(ecs_lookup(world, "inst2.foo") == 0);
    test_assert(ecs_lookup(world, "inst3.foo") == 0);
    test_assert(ecs_lookup(world, "inst1.bar") != 0);
    test_assert(ecs_lookup(world, "inst2.bar") != 0);
    test_assert(ecs_lookup(world, "inst3.bar") != 0);

    ecs_fini(world);
}

void Refs_global_const_var_in_template_for_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_i32_t),
        .value = &(ecs_i32_t){2}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  for i in 0..$v {"
            LINE "    \"e_{$i}\" {}"
            LINE "  }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.e_0") != 0);
    test_assert(ecs_lookup(world, "inst1.e_1") != 0);
    test_assert(ecs_lookup(world, "inst1.e_2") == 0);
    test_assert(ecs_lookup(world, "inst3.e_1") != 0);

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_i32_t*)cv->value.ptr = 3;
    ecs_modified(world, v, EcsScriptConstVar);

    test_assert(ecs_lookup(world, "inst1.e_2") != 0);
    test_assert(ecs_lookup(world, "inst2.e_2") != 0);
    test_assert(ecs_lookup(world, "inst3.e_2") != 0);
    test_assert(ecs_lookup(world, "inst1.e_3") == 0);

    ecs_fini(world);
}

void Refs_global_const_var_in_template_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {times_two($v), 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 30;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 60); test_int(p3->x, 60);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  entity: { new { Position: {$v, 0} } }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_function_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  entity: { new { Position: {times_two($v), 0} } }"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 30;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const ecs_entity_t *x1 = ecs_get(world, ecs_lookup(world, "inst1"), ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, ecs_lookup(world, "inst2"), ecs_entity_t);
        const ecs_entity_t *x3 = ecs_get(world, ecs_lookup(world, "inst3"), ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL); test_assert(x3 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        const Position *p3 = ecs_get(world, *x3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 60); test_int(p3->x, 60);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_script_function_body(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "fn get_v() -> f32 { $v }"
            LINE "template Bar {"
            LINE "  Position: {get_v(), 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 20); test_int(p3->x, 20);
    }

    ecs_fini(world);
}

void Refs_global_const_var_in_template_assigned_to_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t v = ecs_const_var(world, {
        .name = "v",
        .type = ecs_id(Position),
        .value = &(Position){10, 20}
    });
    test_assert(v != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: $v"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 10); test_int(p3->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    ((Position*)cv->value.ptr)->x = 30;
    ((Position*)cv->value.ptr)->y = 40;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2"), Position);
        const Position *p3 = ecs_get(world, ecs_lookup(world, "inst3"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 30); test_int(p3->x, 30);
    }

    ecs_fini(world);
}

void Refs_template_ref_observer_lifecycle(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e = ecs_entity(world, { .name = "e" });
    ecs_set(world, e, Mass, {10});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "}"
            LINE "Bar inst1()"
    });
    test_assert(s != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    test_assert(inst1 != 0);
    test_int(ecs_get(world, inst1, Position)->x, 10);

    ecs_set(world, e, Mass, {20});
    test_int(ecs_get(world, inst1, Position)->x, 20);

    int32_t observer_count = 0;
    ecs_iter_t cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    ecs_delete(world, inst1);
    test_assert(!ecs_is_alive(world, inst1));

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 0);

    ecs_set(world, e, Mass, {30});

    test_assert(ecs_script_run(world, "reinstantiate", "Bar inst2()", NULL) == 0);
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst2 != 0);
    test_assert(ecs_get(world, inst2, Position) != NULL);
    test_int(ecs_get(world, inst2, Position)->x, 30);

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    ecs_set(world, e, Mass, {40});
    test_int(ecs_get(world, inst2, Position)->x, 40);

    ecs_fini(world);
}

void Refs_this_ref_in_template_component_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {$this[Mass].value, 0}"
            LINE "}"
            LINE "inst1 { Mass: {10} Bar: {} }"
            LINE "inst2 { Mass: {20} Bar: {} }"
            LINE "inst3 { Mass: {30} Bar: {} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    ecs_entity_t inst3 = ecs_lookup(world, "inst3");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);
    test_assert(inst3 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 20); test_int(p3->x, 30);
    }

    ecs_set(world, inst2, Mass, {40});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL); test_assert(p3 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 40); test_int(p3->x, 30);
    }

    ecs_fini(world);
}

void Refs_this_ref_in_template_with_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  with Position($this[Mass].value, 0) {"
            LINE "    foo {}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { Mass: {10} Bar: {} }"
            LINE "inst2 { Mass: {20} Bar: {} }"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 20);
    }

    ecs_set(world, ecs_lookup(world, "inst1"), Mass, {30});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 20);
    }

    ecs_fini(world);
}

void Refs_this_ref_in_template_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: match $this[PositionI].x {"
            LINE "    1: {100, 0}"
            LINE "    2: {200, 0}"
            LINE "    _: {0, 0}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { PositionI: {1, 0} Bar: {} }"
            LINE "inst2 { PositionI: {2, 0} Bar: {} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 100); test_int(p2->x, 200);
    }

    ecs_set(world, inst1, PositionI, {2, 0});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 200); test_int(p2->x, 200);
    }

    ecs_fini(world);
}

void Refs_this_ref_in_template_if_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  if $this[PositionI].x > 0 {"
            LINE "    foo {}"
            LINE "  } else {"
            LINE "    bar {}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { PositionI: {1, 0} Bar: {} }"
            LINE "inst2 { PositionI: {0, 0} Bar: {} }"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.foo") != 0);
    test_assert(ecs_lookup(world, "inst1.bar") == 0);
    test_assert(ecs_lookup(world, "inst2.foo") == 0);
    test_assert(ecs_lookup(world, "inst2.bar") != 0);

    ecs_set(world, ecs_lookup(world, "inst1"), PositionI, {0, 0});

    test_assert(ecs_lookup(world, "inst1.foo") == 0);
    test_assert(ecs_lookup(world, "inst1.bar") != 0);
    test_assert(ecs_lookup(world, "inst2.foo") == 0);
    test_assert(ecs_lookup(world, "inst2.bar") != 0);

    ecs_fini(world);
}

void Refs_this_ref_in_template_for_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  for i in 0..$this[PositionI].x {"
            LINE "    \"e_{$i}\" {}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { PositionI: {2, 0} Bar: {} }"
            LINE "inst2 { PositionI: {1, 0} Bar: {} }"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.e_0") != 0);
    test_assert(ecs_lookup(world, "inst1.e_1") != 0);
    test_assert(ecs_lookup(world, "inst1.e_2") == 0);
    test_assert(ecs_lookup(world, "inst2.e_0") != 0);
    test_assert(ecs_lookup(world, "inst2.e_1") == 0);

    ecs_set(world, ecs_lookup(world, "inst1"), PositionI, {3, 0});

    test_assert(ecs_lookup(world, "inst1.e_2") != 0);
    test_assert(ecs_lookup(world, "inst1.e_3") == 0);
    test_assert(ecs_lookup(world, "inst2.e_1") == 0);

    ecs_fini(world);
}

void Refs_this_ref_in_template_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {times_two($this[Mass].value), 0}"
            LINE "}"
            LINE "inst1 { Mass: {10} Bar: {} }"
            LINE "inst2 { Mass: {20} Bar: {} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 40);
    }

    ecs_set(world, inst1, Mass, {30});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 40);
    }

    ecs_fini(world);
}

void Refs_this_ref_in_template_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  entity: { new { Position: {$this[Mass].value, 0} } }"
            LINE "}"
            LINE "inst1 { Mass: {10} Bar: {} }"
            LINE "inst2 { Mass: {20} Bar: {} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 20);
    }

    ecs_set(world, inst1, Mass, {30});

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 20);
    }

    ecs_fini(world);
}

void Refs_this_ref_in_template_function_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  entity: { new { Position: {times_two($this[Mass].value), 0} } }"
            LINE "}"
            LINE "inst1 { Mass: {10} Bar: {} }"
            LINE "inst2 { Mass: {20} Bar: {} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 40);
    }

    ecs_set(world, inst1, Mass, {30});

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 40);
    }

    ecs_fini(world);
}

void Refs_this_ref_in_template_assigned_to_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  foo = Position: $this[Position]"
            LINE "}"
            LINE "inst1 { Position: {10, 20} Bar: {} }"
            LINE "inst2 { Position: {30, 40} Bar: {} }"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p1->y, 20);
        test_int(p2->x, 30); test_int(p2->y, 40);
    }

    ecs_set(world, ecs_lookup(world, "inst1"), Position, {50, 60});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 50); test_int(p1->y, 60);
        test_int(p2->x, 30); test_int(p2->y, 40);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_component_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  Position: {$target[Mass].value, 0}"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 20);
    }

    ecs_set(world, e1, Mass, {30});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 20);
    }

    ecs_set(world, e2, Mass, {40});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 40);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_with_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  with Position($target[Mass].value, 0) {"
            LINE "    foo {}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 20);
    }

    ecs_set(world, e1, Mass, {30});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 20);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_match_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, PositionI, {1, 0});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, PositionI, {2, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  Position: match $target[PositionI].x {"
            LINE "    1: {100, 0}"
            LINE "    2: {200, 0}"
            LINE "    _: {0, 0}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 100); test_int(p2->x, 200);
    }

    ecs_set(world, e1, PositionI, {2, 0});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 200); test_int(p2->x, 200);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_if_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, PositionI, {1, 0});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, PositionI, {0, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  if $target[PositionI].x > 0 {"
            LINE "    foo {}"
            LINE "  } else {"
            LINE "    bar {}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.foo") != 0);
    test_assert(ecs_lookup(world, "inst1.bar") == 0);
    test_assert(ecs_lookup(world, "inst2.foo") == 0);
    test_assert(ecs_lookup(world, "inst2.bar") != 0);

    ecs_set(world, e1, PositionI, {0, 0});

    test_assert(ecs_lookup(world, "inst1.foo") == 0);
    test_assert(ecs_lookup(world, "inst1.bar") != 0);
    test_assert(ecs_lookup(world, "inst2.foo") == 0);
    test_assert(ecs_lookup(world, "inst2.bar") != 0);

    ecs_fini(world);
}

void Refs_prop_ref_in_template_for_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(PositionI) = define_position_i(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, PositionI, {2, 0});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, PositionI, {1, 0});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  for i in 0..$target[PositionI].x {"
            LINE "    \"e_{$i}\" {}"
            LINE "  }"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    test_assert(ecs_lookup(world, "inst1.e_0") != 0);
    test_assert(ecs_lookup(world, "inst1.e_1") != 0);
    test_assert(ecs_lookup(world, "inst1.e_2") == 0);
    test_assert(ecs_lookup(world, "inst2.e_0") != 0);
    test_assert(ecs_lookup(world, "inst2.e_1") == 0);

    ecs_set(world, e1, PositionI, {3, 0});

    test_assert(ecs_lookup(world, "inst1.e_2") != 0);
    test_assert(ecs_lookup(world, "inst1.e_3") == 0);
    test_assert(ecs_lookup(world, "inst2.e_1") == 0);

    ecs_fini(world);
}

void Refs_prop_ref_in_template_function(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  Position: {times_two($target[Mass].value), 0}"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 40);
    }

    ecs_set(world, e1, Mass, {30});

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 40);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  entity: { new { Position: {$target[Mass].value, 0} } }"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p2->x, 20);
    }

    ecs_set(world, e1, Mass, {30});

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 30); test_int(p2->x, 20);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_function_in_new_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_function(world, {
        .name = "times_two",
        .return_type = ecs_id(ecs_f32_t),
        .params = {{ "x", ecs_id(ecs_f32_t) }},
        .callback = times_two
    });

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  entity: { new { Position: {times_two($target[Mass].value), 0} } }"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 20); test_int(p2->x, 40);
    }

    ecs_set(world, e1, Mass, {30});

    {
        const ecs_entity_t *x1 = ecs_get(world, inst1, ecs_entity_t);
        const ecs_entity_t *x2 = ecs_get(world, inst2, ecs_entity_t);
        test_assert(x1 != NULL); test_assert(x2 != NULL);
        const Position *p1 = ecs_get(world, *x1, Position);
        const Position *p2 = ecs_get(world, *x2, Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 60); test_int(p2->x, 40);
    }

    ecs_fini(world);
}

void Refs_prop_ref_in_template_assigned_to_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Position, {10, 20});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Position, {30, 40});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  foo = Position: $target[Position]"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
            LINE "inst2 { Bar: {target: e2} }"
    });
    test_assert(s != 0);

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 10); test_int(p1->y, 20);
        test_int(p2->x, 30); test_int(p2->y, 40);
    }

    ecs_set(world, e1, Position, {50, 60});

    {
        const Position *p1 = ecs_get(world, ecs_lookup(world, "inst1.foo"), Position);
        const Position *p2 = ecs_get(world, ecs_lookup(world, "inst2.foo"), Position);
        test_assert(p1 != NULL); test_assert(p2 != NULL);
        test_int(p1->x, 50); test_int(p1->y, 60);
        test_int(p2->x, 30); test_int(p2->y, 40);
    }

    ecs_fini(world);
}

void Refs_template_prop_ref_observer_lifecycle(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  Position: {$target[Mass].value, 0}"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
    });
    test_assert(s != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    test_assert(inst1 != 0);
    test_int(ecs_get(world, inst1, Position)->x, 10);

    ecs_set(world, e1, Mass, {30});
    test_int(ecs_get(world, inst1, Position)->x, 30);

    int32_t observer_count = 0;
    ecs_iter_t cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    test_assert(ecs_script_run(world, "instantiate2",
        "inst2 { Bar: {target: e2} }", NULL) == 0);
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst2 != 0);
    test_int(ecs_get(world, inst2, Position)->x, 20);

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 2);

    ecs_delete(world, inst1);
    test_assert(!ecs_is_alive(world, inst1));

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    ecs_set(world, e2, Mass, {40});
    test_int(ecs_get(world, inst2, Position)->x, 40);

    ecs_delete(world, inst2);
    test_assert(!ecs_is_alive(world, inst2));

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 0);

    ecs_fini(world);
}

void Refs_template_prop_ref_retarget(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t e1 = ecs_entity(world, { .name = "e1" });
    ecs_set(world, e1, Mass, {10});
    ecs_entity_t e2 = ecs_entity(world, { .name = "e2" });
    ecs_set(world, e2, Mass, {20});

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  prop target = flecs.meta.entity: flecs"
            LINE "  Position: {$target[Mass].value, 0}"
            LINE "}"
            LINE "inst1 { Bar: {target: e1} }"
    });
    test_assert(s != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    test_assert(inst1 != 0);
    test_int(ecs_get(world, inst1, Position)->x, 10);

    ecs_set_id(world, inst1, bar, sizeof(ecs_entity_t), &e2);
    test_int(ecs_get(world, inst1, Position)->x, 20);

    ecs_set(world, e1, Mass, {99});
    test_int(ecs_get(world, inst1, Position)->x, 20);

    ecs_set(world, e2, Mass, {30});
    test_int(ecs_get(world, inst1, Position)->x, 30);

    int32_t observer_count = 0;
    ecs_iter_t cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    ecs_fini(world);
}

void Refs_template_this_ref_observer_lifecycle(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Bar {"
            LINE "  Position: {$this[Mass].value, 0}"
            LINE "}"
            LINE "inst1 { Mass: {10} Bar: {} }"
    });
    test_assert(s != 0);

    ecs_entity_t bar = ecs_lookup(world, "Bar");
    test_assert(bar != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    test_assert(inst1 != 0);
    test_int(ecs_get(world, inst1, Position)->x, 10);

    ecs_set(world, inst1, Mass, {20});
    test_int(ecs_get(world, inst1, Position)->x, 20);

    int32_t observer_count = 0;
    ecs_iter_t cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    test_assert(ecs_script_run(world, "instantiate2",
        "inst2 { Mass: {30} Bar: {} }", NULL) == 0);
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    test_assert(inst2 != 0);
    test_int(ecs_get(world, inst2, Position)->x, 30);

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 2);

    ecs_delete(world, inst1);
    test_assert(!ecs_is_alive(world, inst1));

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 1);

    ecs_set(world, inst2, Mass, {40});
    test_int(ecs_get(world, inst2, Position)->x, 40);

    ecs_delete(world, inst2);
    test_assert(!ecs_is_alive(world, inst2));

    observer_count = 0;
    cit = ecs_children(world, bar);
    while (ecs_children_next(&cit)) {
        int32_t i;
        for (i = 0; i < cit.count; i ++) {
            if (ecs_has_id(world, cit.entities[i], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 0);

    ecs_fini(world);
}

void Refs_global_const_var_declared_in_same_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "export const v = f32: 10"
            LINE "foo {"
            LINE "  Position: {$v, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t v = ecs_lookup(world, "v");
    test_assert(v != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10); // intentional: we don't do partial script updates on an external change
    }

    ecs_fini(world);
}

void Refs_global_const_var_declared_in_same_script_w_fn(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "export const v = f32: 10"
            LINE "fn get_v() -> f32 { $v }"
            LINE "foo {"
            LINE "  Position: {get_v(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t v = ecs_lookup(world, "v");
    test_assert(v != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_fini(world);
}

void Refs_global_const_var_declared_in_same_script_w_fn_other_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t f = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "func" }),
        .code =
            HEAD "export const v = f32: 10"
            LINE "fn get_v() -> f32 { $v }"
            LINE "A {}"
    });
    test_assert(f != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {get_v(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t v = ecs_lookup(world, "v");
    test_assert(v != 0);

    ecs_entity_t A = ecs_lookup(world, "A");
    test_assert(A != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    test_assert(A == ecs_lookup(world, "A"));

    ecs_fini(world);
}

void Refs_global_const_var_declared_in_same_script_w_fn_other_scripts(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t var = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "var" }),
        .code =
            HEAD "export const v = f32: 10"
            LINE "A {}"
    });
    test_assert(var != 0);

    ecs_entity_t f = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "func" }),
        .code =
            HEAD "fn get_v() -> f32 { $v }"
            LINE "B {}"
    });
    test_assert(f != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {get_v(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t v = ecs_lookup(world, "v");
    test_assert(v != 0);

    ecs_entity_t A = ecs_lookup(world, "A");
    test_assert(A != 0);

    ecs_entity_t B = ecs_lookup(world, "B");
    test_assert(B != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    test_assert(A == ecs_lookup(world, "A"));
    test_assert(B == ecs_lookup(world, "B"));

    ecs_fini(world);
}

void Refs_ref_declared_in_same_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "e {"
            LINE "  Mass: {10}"
            LINE "}"
            LINE "foo {"
            LINE "  Position: {e[Mass].value, 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    test_assert(e == ecs_lookup(world, "e"));

    ecs_fini(world);
}

void Refs_ref_declared_in_same_script_w_fn(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "e {"
            LINE "  Mass: {10}"
            LINE "}"
            LINE "fn get_mass() -> f32 { e[Mass].value }"
            LINE "foo {"
            LINE "  Position: {get_mass(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    test_assert(e == ecs_lookup(world, "e"));

    ecs_fini(world);
}

void Refs_ref_declared_in_same_script_w_fn_other_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t f = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "func" }),
        .code =
            HEAD "e {"
            LINE "  Mass: {10}"
            LINE "}"
            LINE "fn get_mass() -> f32 { e[Mass].value }"
            LINE "A {}"
    });
    test_assert(f != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {get_mass(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t A = ecs_lookup(world, "A");
    test_assert(A != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    test_assert(e == ecs_lookup(world, "e"));
    test_assert(A == ecs_lookup(world, "A"));

    ecs_fini(world);
}

void Refs_ref_declared_in_same_script_w_fn_other_scripts(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Mass) = define_mass(world);
    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t ent = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "ent" }),
        .code =
            HEAD "e {"
            LINE "  Mass: {10}"
            LINE "}"
            LINE "A {}"
    });
    test_assert(ent != 0);

    ecs_entity_t f = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "func" }),
        .code =
            HEAD "fn get_mass() -> f32 { e[Mass].value }"
            LINE "B {}"
    });
    test_assert(f != 0);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {get_mass(), 0}"
            LINE "}"
    });
    test_assert(s != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    ecs_entity_t A = ecs_lookup(world, "A");
    test_assert(A != 0);

    ecs_entity_t B = ecs_lookup(world, "B");
    test_assert(B != 0);

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set(world, e, Mass, {20});

    {
        ecs_entity_t foo = ecs_lookup(world, "foo");
        test_assert(foo != 0);
        const Position *p = ecs_get(world, foo, Position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    test_assert(e == ecs_lookup(world, "e"));
    test_assert(A == ecs_lookup(world, "A"));
    test_assert(B == ecs_lookup(world, "B"));

    ecs_fini(world);
}

void Refs_global_const_var_declared_in_same_script_w_template(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = define_position(world);

    ecs_entity_t s = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "export const v = f32: 10"
            LINE "template Bar {"
            LINE "  Position: {$v, 0}"
            LINE "}"
            LINE "Bar inst1()"
            LINE "Bar inst2()"
            LINE "Bar inst3()"
            LINE "A {}"
    });
    test_assert(s != 0);

    ecs_entity_t v = ecs_lookup(world, "v");
    test_assert(v != 0);

    ecs_entity_t A = ecs_lookup(world, "A");
    test_assert(A != 0);

    ecs_entity_t inst1 = ecs_lookup(world, "inst1");
    ecs_entity_t inst2 = ecs_lookup(world, "inst2");
    ecs_entity_t inst3 = ecs_lookup(world, "inst3");
    test_assert(inst1 != 0);
    test_assert(inst2 != 0);
    test_assert(inst3 != 0);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_assert(p3 != NULL);
        test_int(p1->x, 10);
        test_int(p2->x, 10);
        test_int(p3->x, 10);
    }

    EcsScriptConstVar *cv = ecs_ensure(world, v, EcsScriptConstVar);
    *(ecs_f32_t*)cv->value.ptr = 20;
    ecs_modified(world, v, EcsScriptConstVar);

    {
        const Position *p1 = ecs_get(world, inst1, Position);
        const Position *p2 = ecs_get(world, inst2, Position);
        const Position *p3 = ecs_get(world, inst3, Position);
        test_assert(p1 != NULL);
        test_assert(p2 != NULL);
        test_assert(p3 != NULL);
        test_int(p1->x, 20);
        test_int(p2->x, 20);
        test_int(p3->x, 20);
    }

    test_assert(v == ecs_lookup(world, "v"));
    test_assert(A == ecs_lookup(world, "A"));

    ecs_fini(world);
}

