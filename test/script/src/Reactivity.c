#include <script.h>

static void reactivity_reentrant_set(ecs_iter_t *it) {
    ecs_entity_t *ctx = it->ctx;
    ecs_set_id(it->world, ctx[0], ctx[1],
        sizeof(Mass), &(Mass){40});
}

void Reactivity_external_inputs_are_isolated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source_a = ecs_entity(world, { .name = "source_a" });
    ecs_entity_t source_b = ecs_entity(world, { .name = "source_b" });
    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){10});
    ecs_set_id(world, source_b, mass, sizeof(Mass), &(Mass){20});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "a { Position: {source_a[Mass].value, 0} }"
            LINE "b { Position: {source_b[Mass].value, 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);
    ecs_set_id(world, b, position, sizeof(Position), &(Position){99, 100});

    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){30});

    test_uint(ecs_lookup(world, "a"), a);
    test_uint(ecs_lookup(world, "b"), b);
    const Position *pa = ecs_get_id(world, a, position);
    const Position *pb = ecs_get_id(world, b, position);
    test_assert(pa != NULL);
    test_assert(pb != NULL);
    test_int(pa->x, 30);
    test_int(pb->x, 99);
    test_int(pb->y, 100);

    ecs_fini(world);
}

void Reactivity_script_update_recreates_observers(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source_a = ecs_entity(world, { .name = "source_a" });
    ecs_entity_t source_b = ecs_entity(world, { .name = "source_b" });
    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){10});
    ecs_set_id(world, source_b, mass, sizeof(Mass), &(Mass){20});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "a { Position: {source_a[Mass].value, 0} }"
            LINE "b { Position: {source_b[Mass].value, 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t old_observers[2] = {0};
    int32_t old_observer_count = 0;
    ecs_iter_t it = ecs_children(world, script);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_has_id(world, it.entities[i], EcsObserver)) {
                test_assert(old_observer_count < 2);
                old_observers[old_observer_count ++] = it.entities[i];
            }
        }
    }
    test_int(old_observer_count, 2);

    test_int(ecs_script_update(world, script, 0,
        HEAD "b { Position: {source_b[Mass].value, 0} }"
        LINE "a { Position: {source_a[Mass].value, 0} }"), 0);

    test_assert(!ecs_is_alive(world, old_observers[0]));
    test_assert(!ecs_is_alive(world, old_observers[1]));

    ecs_entity_t new_observers[2] = {0};
    int32_t new_observer_count = 0;
    it = ecs_children(world, script);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_has_id(world, it.entities[i], EcsObserver)) {
                test_assert(new_observer_count < 2);
                new_observers[new_observer_count ++] = it.entities[i];
            }
        }
    }
    test_int(new_observer_count, 2);
    test_assert(new_observers[0] != old_observers[0]);
    test_assert(new_observers[0] != old_observers[1]);
    test_assert(new_observers[1] != old_observers[0]);
    test_assert(new_observers[1] != old_observers[1]);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);
    ecs_set_id(world, b, position, sizeof(Position), &(Position){99, 100});

    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){30});

    const Position *pa = ecs_get_id(world, a, position);
    const Position *pb = ecs_get_id(world, b, position);
    test_assert(pa != NULL);
    test_assert(pb != NULL);
    test_int(pa->x, 30);
    test_int(pb->x, 99);
    test_int(pb->y, 100);

    ecs_fini(world);
}

void Reactivity_annotation_follows_dependent_statement(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source_a = ecs_entity(world, { .name = "source_a" });
    ecs_entity_t source_b = ecs_entity(world, { .name = "source_b" });
    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){10});
    ecs_set_id(world, source_b, mass, sizeof(Mass), &(Mass){20});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "@brief Only A"
            LINE "a { Position: {source_a[Mass].value, 0} }"
            LINE "b { Position: {source_b[Mass].value, 0} }"
    });
    test_assert(script != 0);
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);
    test_str(ecs_doc_get_brief(world, a), "Only A");
    test_str(ecs_doc_get_brief(world, b), NULL);

    ecs_set_id(world, source_b, mass, sizeof(Mass), &(Mass){30});

    test_str(ecs_doc_get_brief(world, a), "Only A");
    test_str(ecs_doc_get_brief(world, b), NULL);

    ecs_fini(world);
}

void Reactivity_reentrant_input_change_is_reevaluated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source_a = ecs_entity(world, { .name = "source_a" });
    ecs_entity_t source_b = ecs_entity(world, { .name = "source_b" });
    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){10});
    ecs_set_id(world, source_b, mass, sizeof(Mass), &(Mass){20});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "a { Position: {source_a[Mass].value, 0} }"
            LINE "b { Position: {source_b[Mass].value, 0} }"
    });
    test_assert(script != 0);
    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    ecs_entity_t ctx[2] = {source_b, mass};
    ecs_observer(world, {
        .query.terms = {{
            .id = position,
            .src.id = a
        }},
        .events = { EcsOnSet },
        .callback = reactivity_reentrant_set,
        .ctx = &ctx
    });

    ecs_set_id(world, source_a, mass, sizeof(Mass), &(Mass){30});

    const Mass *m = ecs_get_id(world, source_b, mass);
    test_assert(m != NULL);
    test_int(m->value, 40);
    const Position *p = ecs_get_id(world, b, position);
    test_assert(p != NULL);
    test_int(p->x, 40);

    ecs_fini(world);
}

void Reactivity_failed_update_is_atomic(void) {
    test_quarantine("18 Aug 2026");

    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){1, 0});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "first { PositionI: {source[PositionI].x, 0} }"
            LINE "second { PositionI: {10 / (source[PositionI].x - 2), 0} }"
    });
    test_assert(script != 0);
    ecs_entity_t first = ecs_lookup(world, "first");
    ecs_entity_t second = ecs_lookup(world, "second");
    test_assert(first != 0);
    test_assert(second != 0);

    const PositionI *p = ecs_get_id(world, first, position_i);
    test_assert(p != NULL);
    test_int(p->x, 1);

    p = ecs_get_id(world, second, position_i);
    test_assert(p != NULL);
    test_int(p->x, -10);

    ecs_log_set_level(-4);
    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){2, 0});

    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error != NULL);

    p = ecs_get_id(world, first, position_i);
    test_assert(p != NULL);
    test_int(p->x, 1);

    p = ecs_get_id(world, second, position_i);
    test_assert(p != NULL);
    test_int(p->x, -10);

    ecs_fini(world);
}

void Reactivity_extern_variables_are_isolated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t constant = ecs_const_var(world, {
        .name = "constant",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    ecs_entity_t mutable = ecs_mut_var(world, {
        .name = "mutable",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){20}
    });
    test_assert(constant != 0);
    test_assert(mutable != 0);

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "constant_output { Position: {constant, 0} }"
            LINE "mutable_output { Position: {mutable, 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t constant_output = ecs_lookup(world, "constant_output");
    ecs_entity_t mutable_output = ecs_lookup(world, "mutable_output");
    test_assert(constant_output != 0);
    test_assert(mutable_output != 0);
    ecs_set_id(world, mutable_output, position,
        sizeof(Position), &(Position){99, 0});

    EcsScriptConstVar *constant_value = ecs_ensure(
        world, constant, EcsScriptConstVar);
    *(ecs_f32_t*)constant_value->value.ptr = 30;
    ecs_modified(world, constant, EcsScriptConstVar);

    const Position *p = ecs_get_id(world, constant_output, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    p = ecs_get_id(world, mutable_output, position);
    test_assert(p != NULL);
    test_int(p->x, 99);

    ecs_set_id(world, constant_output, position,
        sizeof(Position), &(Position){77, 0});
    EcsScriptMutVar *mutable_value = ecs_ensure(
        world, mutable, EcsScriptMutVar);
    *(ecs_f32_t*)mutable_value->value.ptr = 40;
    ecs_modified(world, mutable, EcsScriptMutVar);

    p = ecs_get_id(world, constant_output, position);
    test_assert(p != NULL);
    test_int(p->x, 77);
    p = ecs_get_id(world, mutable_output, position);
    test_assert(p != NULL);
    test_int(p->x, 40);
    test_uint(ecs_lookup(world, "constant_output"), constant_output);
    test_uint(ecs_lookup(world, "mutable_output"), mutable_output);

    ecs_fini(world);
}

void Reactivity_const_dependency_is_transitive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const value = source[Mass].value"
            LINE "item { Position: {value * 2, 0} }"
    });
    test_assert(script != 0);
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){15});

    test_uint(ecs_lookup(world, "item"), item);
    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 30);

    ecs_fini(world);
}

void Reactivity_if_inherits_branch_dependencies(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_entity_t payload = ecs_entity(world, { .name = "payload" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});
    ecs_set_id(world, payload, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if condition[Mass].value > 0 {"
            LINE "  item { Position: {payload[Mass].value, 0} }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);

    ecs_set_id(world, payload, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "item"), item);
    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    ecs_fini(world);
}

void Reactivity_if_cleans_up_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if condition[Mass].value > 0 {"
            LINE "  branch_true {}"
            LINE "} else {"
            LINE "  branch_false {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t branch_true = ecs_lookup(world, "branch_true");
    test_assert(branch_true != 0);
    test_assert(ecs_lookup(world, "branch_false") == 0);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    ecs_entity_t branch_false = ecs_lookup(world, "branch_false");
    test_assert(branch_false != 0);
    test_assert(!ecs_is_alive(world, branch_true));
    test_assert(ecs_lookup(world, "branch_true") == 0);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    test_assert(!ecs_is_alive(world, branch_false));
    test_assert(ecs_lookup(world, "branch_true") != 0);
    test_assert(ecs_lookup(world, "branch_false") == 0);

    ecs_fini(world);
}

void Reactivity_if_cleans_up_components(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "item {"
            LINE "  if condition[Mass].value > 0 {"
            LINE "    Position: {10, 20}"
            LINE "  } else {"
            LINE "    Velocity: {30, 40}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    test_assert(ecs_has_id(world, item, position));
    test_assert(!ecs_has_id(world, item, velocity));

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    test_uint(ecs_lookup(world, "item"), item);
    test_assert(!ecs_has_id(world, item, position));
    test_assert(ecs_has_id(world, item, velocity));

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    test_uint(ecs_lookup(world, "item"), item);
    test_assert(ecs_has_id(world, item, position));
    test_assert(!ecs_has_id(world, item, velocity));

    ecs_fini(world);
}

void Reactivity_if_cleans_up_singleton_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if condition[Mass].value > 0 {"
            LINE "  $ { Position: {10, 20} }"
            LINE "}"
    });
    test_assert(script != 0);
    test_assert(ecs_has_id(world, position, position));

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    test_assert(ecs_is_alive(world, position));
    test_assert(!ecs_has_id(world, position, position));

    ecs_fini(world);
}

void Reactivity_mutually_exclusive_component_owner(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "item {"
            LINE "  if condition[Mass].value > 0 {"
            LINE "    Position: {10, 20}"
            LINE "  } else {"
            LINE "    Position: {30, 40}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Reactivity_mutually_exclusive_component_owner_three_branches(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t selector = ecs_entity(world, { .name = "selector" });
    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){0, 0});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "item {"
            LINE "  if selector[PositionI].x == 0 {"
            LINE "    Position: {10, 20}"
            LINE "  } else if selector[PositionI].x == 1 {"
            LINE "    Position: {30, 40}"
            LINE "  } else {"
            LINE "    Velocity: {50, 60}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);

    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(!ecs_has_id(world, item, velocity));

    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){1, 0});

    p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);
    test_assert(!ecs_has_id(world, item, velocity));

    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){2, 0});

    test_assert(!ecs_has_id(world, item, position));
    const Velocity *v = ecs_get_id(world, item, velocity);
    test_assert(v != NULL);
    test_int(v->x, 50);
    test_int(v->y, 60);

    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){1, 0});

    p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    test_int(p->y, 40);
    test_assert(!ecs_has_id(world, item, velocity));

    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){0, 0});

    p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
    test_assert(!ecs_has_id(world, item, velocity));

    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){2, 0});

    test_assert(!ecs_has_id(world, item, position));
    v = ecs_get_id(world, item, velocity);
    test_assert(v != NULL);
    test_int(v->x, 50);
    test_int(v->y, 60);

    ecs_fini(world);
}

void Reactivity_non_exclusive_component_owner_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t selector = ecs_entity(world, { .name = "selector" });
    ecs_set_id(world, selector, position_i,
        sizeof(PositionI), &(PositionI){0, 0});

    ecs_log_set_level(-4);
    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if selector[PositionI].x == 0 {"
            LINE "  item { Position: {10, 20} }"
            LINE "}"
            LINE "if selector[PositionI].x == 1 {"
            LINE "  item { Position: {30, 40} }"
            LINE "}"
    });
    test_assert(script != 0);
    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error != NULL);
    test_assert(strstr(script_data->error,
        "component can only be created in one scope or "
        "mutually exclusive scopes") != NULL);

    ecs_fini(world);
}

void Reactivity_template_non_exclusive_component_owner_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_log_set_level(-4);
    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Panel {"
            LINE "  prop first: bool = true"
            LINE "  prop second: bool = false"
            LINE "  if first {"
            LINE "    Position: {10, 20}"
            LINE "  }"
            LINE "  if second {"
            LINE "    Position: {30, 40}"
            LINE "  }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);
    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error != NULL);
    test_assert(strstr(script_data->error,
        "component can only be created in one scope or "
        "mutually exclusive scopes") != NULL);

    ecs_fini(world);
}

void Reactivity_partial_assignment_does_not_own_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t owner = ecs_entity(world, { .name = "owner" });
    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, owner, mass, sizeof(Mass), &(Mass){1});
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "item {"
            LINE "  Position: {owner[Mass].value, 2}"
            LINE "  if condition[Mass].value > 0 {"
            LINE "    Position: {x: 10}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 2);

    ecs_set_id(world, owner, mass, sizeof(Mass), &(Mass){5});

    p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Reactivity_for_clears_previous_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){2, 0});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[PositionI].x {"
            LINE "  \"item_$i\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t old_0 = ecs_lookup(world, "item_0");
    ecs_entity_t old_1 = ecs_lookup(world, "item_1");
    test_assert(old_0 != 0);
    test_assert(old_1 != 0);

    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){3, 0});

    ecs_entity_t new_0 = ecs_lookup(world, "item_0");
    ecs_entity_t new_1 = ecs_lookup(world, "item_1");
    ecs_entity_t new_2 = ecs_lookup(world, "item_2");
    test_assert(new_0 != 0);
    test_assert(new_1 != 0);
    test_assert(new_2 != 0);
    test_assert(!ecs_is_alive(world, old_0));
    test_assert(!ecs_is_alive(world, old_1));
    test_assert(new_0 != old_0);
    test_assert(new_1 != old_1);

    ecs_fini(world);
}

void Reactivity_nested_for_clears_previous_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){2, 0});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[PositionI].x {"
            LINE "  for j in 0..2 {"
            LINE "    \"item_{i}_{j}\" {}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t old_00 = ecs_lookup(world, "item_0_0");
    ecs_entity_t old_01 = ecs_lookup(world, "item_0_1");
    ecs_entity_t old_10 = ecs_lookup(world, "item_1_0");
    ecs_entity_t old_11 = ecs_lookup(world, "item_1_1");
    test_assert(old_00 != 0);
    test_assert(old_01 != 0);
    test_assert(old_10 != 0);
    test_assert(old_11 != 0);

    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){0, 0});

    test_assert(!ecs_is_alive(world, old_00));
    test_assert(!ecs_is_alive(world, old_01));
    test_assert(!ecs_is_alive(world, old_10));
    test_assert(!ecs_is_alive(world, old_11));
    test_assert(ecs_lookup(world, "item_0_0") == 0);
    test_assert(ecs_lookup(world, "item_0_1") == 0);
    test_assert(ecs_lookup(world, "item_1_0") == 0);
    test_assert(ecs_lookup(world, "item_1_1") == 0);

    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){1, 0});

    test_assert(ecs_lookup(world, "item_0_0") != 0);
    test_assert(ecs_lookup(world, "item_0_1") != 0);
    test_assert(ecs_lookup(world, "item_1_0") == 0);
    test_assert(ecs_lookup(world, "item_1_1") == 0);

    ecs_fini(world);
}

void Reactivity_inactive_for_cleans_up_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){1, 0});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if source[PositionI].x > 0 {"
            LINE "  for i in 0..2 {"
            LINE "    \"item_$i\" {}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t old_0 = ecs_lookup(world, "item_0");
    ecs_entity_t old_1 = ecs_lookup(world, "item_1");
    test_assert(old_0 != 0);
    test_assert(old_1 != 0);

    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){0, 0});

    test_assert(!ecs_is_alive(world, old_0));
    test_assert(!ecs_is_alive(world, old_1));
    test_assert(ecs_lookup(world, "item_0") == 0);
    test_assert(ecs_lookup(world, "item_1") == 0);

    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){1, 0});

    test_assert(ecs_lookup(world, "item_0") != 0);
    test_assert(ecs_lookup(world, "item_1") != 0);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_reevaluation(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "holder {"
            LINE "  entity: {new { Position: {source[Mass].value, 0} }}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t holder = ecs_lookup(world, "holder");
    test_assert(holder != 0);
    const ecs_entity_t *value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    ecs_entity_t created = *value;
    test_assert(created != 0);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    test_uint(*value, created);
    test_assert(ecs_is_alive(world, created));
    const Position *p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    ecs_fini(world);
}

void Reactivity_template_props_are_isolated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Panel {"
            LINE "  prop width: f32 = 10"
            LINE "  prop height: f32 = 20"
            LINE "  width_child { Position: {width, 0} }"
            LINE "  height_child { Position: {height, 0} }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);
    ecs_entity_t panel = ecs_lookup(world, "Panel");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t width_child = ecs_lookup(world, "instance.width_child");
    ecs_entity_t height_child = ecs_lookup(world, "instance.height_child");
    test_assert(panel != 0);
    test_assert(instance != 0);
    test_assert(width_child != 0);
    test_assert(height_child != 0);
    ecs_set_id(world, height_child, position,
        sizeof(Position), &(Position){99, 100});

    struct {
        ecs_f32_t width;
        ecs_f32_t height;
    } props = {30, 20};
    ecs_set_id(world, instance, panel, sizeof(props), &props);

    test_uint(ecs_lookup(world, "instance.width_child"), width_child);
    test_uint(ecs_lookup(world, "instance.height_child"), height_child);
    const Position *p = ecs_get_id(world, width_child, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    p = ecs_get_id(world, height_child, position);
    test_assert(p != NULL);
    test_int(p->x, 99);
    test_int(p->y, 100);

    ecs_fini(world);
}

void Reactivity_template_same_prop_value_skips(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Panel {"
            LINE "  prop width: f32 = 10"
            LINE "  child { Position: {width, 0} }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);
    ecs_entity_t panel = ecs_lookup(world, "Panel");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t child = ecs_lookup(world, "instance.child");
    test_assert(panel != 0);
    test_assert(instance != 0);
    test_assert(child != 0);
    ecs_set_id(world, child, position,
        sizeof(Position), &(Position){99, 100});

    ecs_f32_t width = 10;
    ecs_set_id(world, instance, panel, sizeof(width), &width);

    test_uint(ecs_lookup(world, "instance.child"), child);
    const Position *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_int(p->x, 99);
    test_int(p->y, 100);

    ecs_fini(world);
}

void Reactivity_template_muts_are_isolated(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Panel {"
            LINE "  mut width: f32 = 10"
            LINE "  mut height: f32 = 20"
            LINE "  width_child { Position: {width, 0} }"
            LINE "  height_child { Position: {height, 0} }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);
    ecs_entity_t panel = ecs_lookup(world, "Panel");
    ecs_entity_t mut = ecs_lookup_child(world, panel, "mut");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t width_child = ecs_lookup(world, "instance.width_child");
    ecs_entity_t height_child = ecs_lookup(world, "instance.height_child");
    test_assert(panel != 0);
    test_assert(mut != 0);
    test_assert(instance != 0);
    test_assert(width_child != 0);
    test_assert(height_child != 0);
    ecs_set_id(world, height_child, position,
        sizeof(Position), &(Position){99, 100});

    struct {
        ecs_f32_t width;
        ecs_f32_t height;
    } state = {30, 20};
    ecs_set_id(world, instance, mut, sizeof(state), &state);

    test_uint(ecs_lookup(world, "instance.width_child"), width_child);
    test_uint(ecs_lookup(world, "instance.height_child"), height_child);
    const Position *p = ecs_get_id(world, width_child, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    p = ecs_get_id(world, height_child, position);
    test_assert(p != NULL);
    test_int(p->x, 99);
    test_int(p->y, 100);

    ecs_fini(world);
}

void Reactivity_template_const_dependency_is_transitive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Panel {"
            LINE "  prop value: f32 = 10"
            LINE "  const doubled = value * 2"
            LINE "  child { Position: {doubled, 0} }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);
    ecs_entity_t panel = ecs_lookup(world, "Panel");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t child = ecs_lookup(world, "instance.child");
    test_assert(panel != 0);
    test_assert(instance != 0);
    test_assert(child != 0);

    ecs_f32_t value = 15;
    ecs_set_id(world, instance, panel, sizeof(value), &value);

    test_uint(ecs_lookup(world, "instance.child"), child);
    const Position *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_int(p->x, 30);

    ecs_fini(world);
}

void Reactivity_template_if_cleans_up_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Switch {"
            LINE "  prop enabled: bool = true"
            LINE "  if enabled {"
            LINE "    branch_true {}"
            LINE "  } else {"
            LINE "    branch_false {}"
            LINE "  }"
            LINE "}"
            LINE "Switch instance()"
    });
    test_assert(script != 0);
    ecs_entity_t switch_type = ecs_lookup(world, "Switch");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t branch_true = ecs_lookup(world, "instance.branch_true");
    test_assert(switch_type != 0);
    test_assert(instance != 0);
    test_assert(branch_true != 0);
    test_assert(ecs_lookup(world, "instance.branch_false") == 0);

    bool enabled = false;
    ecs_set_id(world, instance, switch_type, sizeof(enabled), &enabled);

    ecs_entity_t branch_false = ecs_lookup(world, "instance.branch_false");
    test_assert(branch_false != 0);
    test_assert(!ecs_is_alive(world, branch_true));
    test_assert(ecs_lookup(world, "instance.branch_true") == 0);

    enabled = true;
    ecs_set_id(world, instance, switch_type, sizeof(enabled), &enabled);

    test_assert(!ecs_is_alive(world, branch_false));
    test_assert(ecs_lookup(world, "instance.branch_true") != 0);
    test_assert(ecs_lookup(world, "instance.branch_false") == 0);

    ecs_fini(world);
}

void Reactivity_template_if_cleans_up_components(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Switch {"
            LINE "  prop enabled: bool = true"
            LINE "  if enabled {"
            LINE "    Position: {10, 20}"
            LINE "  } else {"
            LINE "    Velocity: {30, 40}"
            LINE "  }"
            LINE "}"
            LINE "Switch instance()"
    });
    test_assert(script != 0);
    ecs_entity_t switch_type = ecs_lookup(world, "Switch");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    test_assert(switch_type != 0);
    test_assert(instance != 0);
    test_assert(ecs_has_id(world, instance, position));
    test_assert(!ecs_has_id(world, instance, velocity));

    bool enabled = false;
    ecs_set_id(world, instance, switch_type, sizeof(enabled), &enabled);

    test_assert(!ecs_has_id(world, instance, position));
    test_assert(ecs_has_id(world, instance, velocity));

    enabled = true;
    ecs_set_id(world, instance, switch_type, sizeof(enabled), &enabled);

    test_assert(ecs_has_id(world, instance, position));
    test_assert(!ecs_has_id(world, instance, velocity));

    ecs_fini(world);
}

void Reactivity_template_owner_cleans_up_instance_state(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Content {"
            LINE "  Position: {10, 20}"
            LINE "  child {}"
            LINE "}"
            LINE "instance {"
            LINE "  if condition[Mass].value > 0 {"
            LINE "    Content: {}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t content = ecs_lookup(world, "Content");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t child = ecs_lookup(world, "instance.child");
    test_assert(content != 0);
    test_assert(instance != 0);
    test_assert(child != 0);
    test_assert(ecs_has_id(world, instance, content));
    test_assert(ecs_has_id(world, instance, position));

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    test_uint(ecs_lookup(world, "instance"), instance);
    test_assert(!ecs_has_id(world, instance, content));
    test_assert(!ecs_has_id(world, instance, position));
    test_assert(!ecs_is_alive(world, child));
    test_assert(ecs_lookup(world, "instance.child") == 0);

    ecs_fini(world);
}

void Reactivity_template_for_clears_previous_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template List {"
            LINE "  prop count: i32 = 2"
            LINE "  for i in 0..count {"
            LINE "    \"child_$i\" {}"
            LINE "  }"
            LINE "}"
            LINE "List instance()"
    });
    test_assert(script != 0);
    ecs_entity_t list = ecs_lookup(world, "List");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t old_0 = ecs_lookup(world, "instance.child_0");
    ecs_entity_t old_1 = ecs_lookup(world, "instance.child_1");
    test_assert(list != 0);
    test_assert(instance != 0);
    test_assert(old_0 != 0);
    test_assert(old_1 != 0);

    int32_t count = 3;
    ecs_set_id(world, instance, list, sizeof(count), &count);

    ecs_entity_t new_0 = ecs_lookup(world, "instance.child_0");
    ecs_entity_t new_1 = ecs_lookup(world, "instance.child_1");
    ecs_entity_t new_2 = ecs_lookup(world, "instance.child_2");
    test_assert(new_0 != 0);
    test_assert(new_1 != 0);
    test_assert(new_2 != 0);
    test_assert(!ecs_is_alive(world, old_0));
    test_assert(!ecs_is_alive(world, old_1));
    test_assert(new_0 != old_0);
    test_assert(new_1 != old_1);

    ecs_fini(world);
}

void Reactivity_template_inactive_for_cleans_up_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template List {"
            LINE "  prop enabled: bool = true"
            LINE "  if enabled {"
            LINE "    for i in 0..2 {"
            LINE "      \"child_$i\" {}"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "List instance()"
    });
    test_assert(script != 0);
    ecs_entity_t list = ecs_lookup(world, "List");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    ecs_entity_t old_0 = ecs_lookup(world, "instance.child_0");
    ecs_entity_t old_1 = ecs_lookup(world, "instance.child_1");
    test_assert(list != 0);
    test_assert(instance != 0);
    test_assert(old_0 != 0);
    test_assert(old_1 != 0);

    bool enabled = false;
    ecs_set_id(world, instance, list, sizeof(enabled), &enabled);

    test_assert(!ecs_is_alive(world, old_0));
    test_assert(!ecs_is_alive(world, old_1));
    test_assert(ecs_lookup(world, "instance.child_0") == 0);
    test_assert(ecs_lookup(world, "instance.child_1") == 0);

    enabled = true;
    ecs_set_id(world, instance, list, sizeof(enabled), &enabled);

    test_assert(ecs_lookup(world, "instance.child_0") != 0);
    test_assert(ecs_lookup(world, "instance.child_1") != 0);

    ecs_fini(world);
}

void Reactivity_template_instances_have_private_slots(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Switch {"
            LINE "  prop enabled: bool = true"
            LINE "  if enabled {"
            LINE "    child { Position: {10, 20} }"
            LINE "  } else {"
            LINE "    other { Velocity: {30, 40} }"
            LINE "  }"
            LINE "}"
            LINE "Switch first()"
            LINE "Switch second()"
    });
    test_assert(script != 0);
    ecs_entity_t switch_type = ecs_lookup(world, "Switch");
    ecs_entity_t first = ecs_lookup(world, "first");
    ecs_entity_t second_child = ecs_lookup(world, "second.child");
    test_assert(switch_type != 0);
    test_assert(first != 0);
    test_assert(second_child != 0);
    test_assert(ecs_has_id(world, second_child, position));
    test_assert(!ecs_has_id(world, second_child, velocity));

    bool enabled = false;
    ecs_set_id(world, first, switch_type, sizeof(enabled), &enabled);

    test_assert(ecs_lookup(world, "first.child") == 0);
    test_assert(ecs_lookup(world, "first.other") != 0);
    test_uint(ecs_lookup(world, "second.child"), second_child);
    test_assert(ecs_is_alive(world, second_child));
    test_assert(ecs_has_id(world, second_child, position));
    test_assert(!ecs_has_id(world, second_child, velocity));

    ecs_fini(world);
}

void Reactivity_template_capture_is_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const captured = source[Mass].value"
            LINE "template Panel {"
            LINE "  child { Position: {captured, 0} }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);
    ecs_entity_t child = ecs_lookup(world, "instance.child");
    test_assert(child != 0);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "instance.child"), child);
    const Position *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    ecs_fini(world);
}

void Reactivity_sixty_four_inputs(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t sources[64];
    ecs_strbuf_t code = ECS_STRBUF_INIT;
    int32_t i;
    for (i = 0; i < 64; i ++) {
        char name[32];
        ecs_os_snprintf(name, sizeof(name), "source_%d", i);
        sources[i] = ecs_entity(world, { .name = name });
        ecs_set_id(world, sources[i], mass, sizeof(Mass), &(Mass){i});
        ecs_strbuf_append(&code,
            "output_%d { Position: {source_%d[Mass].value, 0} }\n", i, i);
    }
    char *expr = ecs_strbuf_get(&code);

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(script != 0);
    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error == NULL);

    ecs_set_id(world, sources[63], mass, sizeof(Mass), &(Mass){100});

    ecs_entity_t output = ecs_lookup(world, "output_63");
    test_assert(output != 0);
    const Position *p = ecs_get_id(world, output, position);
    test_assert(p != NULL);
    test_int(p->x, 100);

    ecs_os_free(expr);
    ecs_fini(world);
}

void Reactivity_sixty_five_inputs_fail(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    ecs_strbuf_t code = ECS_STRBUF_INIT;
    int32_t i;
    for (i = 0; i < 65; i ++) {
        char name[32];
        ecs_os_snprintf(name, sizeof(name), "source_%d", i);
        ecs_entity_t source = ecs_entity(world, { .name = name });
        ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){i});
        ecs_strbuf_append(&code,
            "output_%d { i32: {source_%d[Mass].value} }\n", i, i);
    }
    char *expr = ecs_strbuf_get(&code);

    ecs_log_set_level(-4);
    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code = expr
    });
    test_assert(script != 0);
    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error != NULL);
    test_assert(strstr(script_data->error,
        "script cannot have more than 64 reactive inputs") != NULL);

    ecs_os_free(expr);
    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "holder {"
            LINE "  entity: {new { Position: {3, 4} }}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t holder = ecs_lookup(world, "holder");
    test_assert(holder != 0);
    const ecs_entity_t *value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    ecs_entity_t created = *value;
    test_assert(created != 0);
    const Position *p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    ecs_entity_t reactive = ecs_lookup(world, "reactive");
    test_assert(reactive != 0);
    p = ecs_get_id(world, reactive, position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    test_uint(*value, created);
    test_assert(ecs_is_alive(world, created));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}
