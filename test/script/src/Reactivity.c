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

    ecs_entity_t first = ecs_mut_var(world, {
        .name = "first",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){10}
    });
    ecs_entity_t second = ecs_mut_var(world, {
        .name = "second",
        .type = ecs_id(ecs_f32_t),
        .value = &(ecs_f32_t){20}
    });
    test_assert(first != 0);
    test_assert(second != 0);

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "first_output { Position: {first, 0} }"
            LINE "second_output { Position: {second, 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t first_output = ecs_lookup(world, "first_output");
    ecs_entity_t second_output = ecs_lookup(world, "second_output");
    test_assert(first_output != 0);
    test_assert(second_output != 0);
    ecs_set_id(world, second_output, position,
        sizeof(Position), &(Position){99, 0});

    EcsScriptMutVar *first_value = ecs_ensure(
        world, first, EcsScriptMutVar);
    *(ecs_f32_t*)first_value->value.ptr = 30;
    ecs_modified(world, first, EcsScriptMutVar);

    const Position *p = ecs_get_id(world, first_output, position);
    test_assert(p != NULL);
    test_int(p->x, 30);
    p = ecs_get_id(world, second_output, position);
    test_assert(p != NULL);
    test_int(p->x, 99);

    ecs_set_id(world, first_output, position,
        sizeof(Position), &(Position){77, 0});
    EcsScriptMutVar *second_value = ecs_ensure(
        world, second, EcsScriptMutVar);
    *(ecs_f32_t*)second_value->value.ptr = 40;
    ecs_modified(world, second, EcsScriptMutVar);

    p = ecs_get_id(world, first_output, position);
    test_assert(p != NULL);
    test_int(p->x, 77);
    p = ecs_get_id(world, second_output, position);
    test_assert(p != NULL);
    test_int(p->x, 40);
    test_uint(ecs_lookup(world, "first_output"), first_output);
    test_uint(ecs_lookup(world, "second_output"), second_output);

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

void Reactivity_new_entity_not_duplicated_on_reevaluation(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "holder {"
            LINE "  entity: {new { Position: {source[Mass].value, 4} Marker }}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    int32_t count = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            count += it.count;
            created = it.entities[0];
        }
    }
    test_int(count, 1);
    test_assert(created != 0);

    ecs_entity_t holder = ecs_lookup(world, "holder");
    test_assert(holder != 0);
    const ecs_entity_t *value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    test_uint(*value, created);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    count = 0;
    ecs_entity_t recreated = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            count += it.count;
            recreated = it.entities[0];
        }
    }
    test_int(count, 1);
    test_uint(recreated, created);

    value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    test_uint(*value, created);

    const Position *p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_child_reclaimed_on_reevaluation(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "holder {"
            LINE "  entity: {new {"
            LINE "    Position: {source[Mass].value, 4}"
            LINE "    Marker"
            LINE "    if source[Mass].value < 15 {"
            LINE "      child {}"
            LINE "    }"
            LINE "  }}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
    test_assert(created != 0);

    ecs_entity_t child = ecs_lookup_child(world, created, "child");
    test_assert(child != 0);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_assert(ecs_is_alive(world, created));
    const Position *p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    test_assert(!ecs_is_alive(world, child));
    test_assert(ecs_lookup_child(world, created, "child") == 0);

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

void Reactivity_with_scope_is_reactive(void) {
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
    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "with Tag {"
            LINE "  a { Position: {source[Mass].value, 0} }"
            LINE "  b { Position: {5, 0} }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(ecs_has_id(world, a, tag));
    test_assert(ecs_has_id(world, b, tag));

    ecs_set_id(world, b, position, sizeof(Position), &(Position){99, 100});

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "a"), a);
    test_uint(ecs_lookup(world, "b"), b);
    test_assert(ecs_has_id(world, a, tag));
    test_assert(ecs_has_id(world, b, tag));

    const Position *pa = ecs_get_id(world, a, position);
    const Position *pb = ecs_get_id(world, b, position);
    test_assert(pa != NULL);
    test_assert(pb != NULL);
    test_int(pa->x, 20);
    test_int(pb->x, 99);

    ecs_fini(world);
}

void Reactivity_with_expression_is_reactive(void) {
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
            HEAD "with Position(source[Mass].value, 0) {"
            LINE "  a {}"
            LINE "  b {}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);
    {
        const Position *pa = ecs_get_id(world, a, position);
        const Position *pb = ecs_get_id(world, b, position);
        test_assert(pa != NULL);
        test_assert(pb != NULL);
        test_int(pa->x, 10);
        test_int(pb->x, 10);
    }

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "a"), a);
    test_uint(ecs_lookup(world, "b"), b);
    {
        const Position *pa = ecs_get_id(world, a, position);
        const Position *pb = ecs_get_id(world, b, position);
        test_assert(pa != NULL);
        test_assert(pb != NULL);
        test_int(pa->x, 20);
        test_int(pb->x, 20);
    }

    ecs_fini(world);
}

void Reactivity_pair_scope_is_reactive(void) {
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
    ecs_entity_t rel = ecs_entity(world, { .name = "Rel" });
    ecs_entity_t tgt = ecs_entity(world, { .name = "Tgt" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "(Rel, Tgt) {"
            LINE "  a { Position: {source[Mass].value, 0} }"
            LINE "  b { Position: {5, 0} }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);
    test_assert(ecs_has_pair(world, a, rel, tgt));
    test_assert(ecs_has_pair(world, b, rel, tgt));

    ecs_set_id(world, b, position, sizeof(Position), &(Position){99, 100});

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "a"), a);
    test_uint(ecs_lookup(world, "b"), b);
    test_assert(ecs_has_pair(world, a, rel, tgt));
    test_assert(ecs_has_pair(world, b, rel, tgt));

    const Position *pa = ecs_get_id(world, a, position);
    const Position *pb = ecs_get_id(world, b, position);
    test_assert(pa != NULL);
    test_assert(pb != NULL);
    test_int(pa->x, 20);
    test_int(pb->x, 99);

    ecs_fini(world);
}

void Reactivity_nested_template_is_reactive(void) {
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
            HEAD "template Inner {"
            LINE "  prop v: f32 = 0"
            LINE "  Position: {v, 1}"
            LINE "}"
            LINE "template Outer {"
            LINE "  prop w: f32 = 0"
            LINE "  child { Inner: {w} }"
            LINE "}"
            LINE "o { Outer: {source[Mass].value} }"
    });
    test_assert(script != 0);

    ecs_entity_t child = ecs_lookup(world, "o.child");
    test_assert(child != 0);
    {
        const Position *p = ecs_get_id(world, child, position);
        test_assert(p != NULL);
        test_int(p->x, 10);
        test_int(p->y, 1);
    }

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "o.child"), child);
    {
        const Position *p = ecs_get_id(world, child, position);
        test_assert(p != NULL);
        test_int(p->x, 20);
        test_int(p->y, 1);
    }

    ecs_fini(world);
}

void Reactivity_template_with_scope_is_reactive(void) {
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
    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template T {"
            LINE "  prop v: f32 = 0"
            LINE "  with Tag {"
            LINE "    child { Position: {v, 1} }"
            LINE "  }"
            LINE "}"
            LINE "t { T: {source[Mass].value} }"
    });
    test_assert(script != 0);

    ecs_entity_t child = ecs_lookup(world, "t.child");
    test_assert(child != 0);
    test_assert(ecs_has_id(world, child, tag));
    {
        const Position *p = ecs_get_id(world, child, position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "t.child"), child);
    test_assert(ecs_has_id(world, child, tag));
    {
        const Position *p = ecs_get_id(world, child, position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_fini(world);
}

static ecs_script_future_t *reactivity_futures[8];
static int32_t reactivity_future_count;

static void reactivity_store_future(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    reactivity_futures[reactivity_future_count ++] = future;
}

static void reactivity_cancel_future(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)future;
}

void Reactivity_try_in_managed_script_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "try {"
            LINE "  a {}"
            LINE "} catch {"
            LINE "  b {}"
            LINE "}"
    });

    test_assert(script != 0);
    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") == 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);

    ecs_fini(world);
}

void Reactivity_try_catch_scope_dependencies(void) {
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

    reactivity_future_count = 0;
    ecs_os_zeromem(reactivity_futures);

    ecs_async_function(world, {
        .name = "fail",
        .return_type = ecs_id(ecs_i32_t),
        .callback = reactivity_store_future,
        .cancel = reactivity_cancel_future
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "try {"
        LINE "  in_try { Position: {source[Mass].value, 0} }"
        LINE "  await fail()"
        LINE "  not_reached { Position: {1, 0} }"
        LINE "} catch {"
        LINE "  in_catch { Position: {source[Mass].value * 2, 0} }"
        LINE "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_assert(task != NULL);
    test_int(ecs_script_task_resume(task, NULL), EcsScriptTaskPending);
    test_int(reactivity_future_count, 1);

    test_int(ecs_script_future_reject(reactivity_futures[0], "nope"), 0);
    ecs_script_future_release(reactivity_futures[0]);

    test_int(ecs_script_task_resume(task, NULL), EcsScriptTaskDone);

    ecs_entity_t in_try = ecs_lookup(world, "in_try");
    ecs_entity_t in_catch = ecs_lookup(world, "in_catch");
    test_assert(in_try != 0);
    test_assert(in_catch != 0);
    test_assert(ecs_lookup(world, "not_reached") == 0);

    const Position *pt = ecs_get_id(world, in_try, position);
    const Position *pc = ecs_get_id(world, in_catch, position);
    test_assert(pt != NULL);
    test_assert(pc != NULL);
    test_int(pt->x, 10);
    test_int(pc->x, 20);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Reactivity_await_expression_dependencies(void) {
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

    reactivity_future_count = 0;
    ecs_os_zeromem(reactivity_futures);

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { "amount", ecs_id(ecs_i32_t) }
        },
        .callback = reactivity_store_future,
        .cancel = reactivity_cancel_future
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "const value = await fetch(source[Mass].value)"
        LINE "item { Position: {value, 0} }", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_assert(task != NULL);
    test_int(ecs_script_task_resume(task, NULL), EcsScriptTaskPending);
    test_int(reactivity_future_count, 1);
    test_assert(ecs_lookup(world, "item") == 0);

    ecs_value_t value = ecs_value(ecs_i32_t, {42});
    test_int(ecs_script_future_resolve(reactivity_futures[0], &value), 0);
    ecs_script_future_release(reactivity_futures[0]);

    test_int(ecs_script_task_resume(task, NULL), EcsScriptTaskDone);

    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 42);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

static void reactivity_doubled_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    *(ecs_f32_t*)result->ptr = *(ecs_f32_t*)argv[0].ptr * 2;
}

void Reactivity_element_expr_is_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t index = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Index" }),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_array(world, {
        .entity = ecs_entity(world, { .name = "Arr" }),
        .type = ecs_id(ecs_i32_t),
        .count = 3
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){0});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const a: Arr = [10, 20, 30]"
            LINE "const i = source[Index].value"
            LINE "item { Position: {a[i], 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    {
        const Position *p = ecs_get_id(world, item, position);
        test_assert(p != NULL);
        test_int(p->x, 10);
    }

    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){2});

    test_uint(ecs_lookup(world, "item"), item);
    {
        const Position *p = ecs_get_id(world, item, position);
        test_assert(p != NULL);
        test_int(p->x, 30);
    }

    ecs_fini(world);
}

void Reactivity_range_expr_is_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t index = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Index" }),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
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
    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const n = source[Index].value"
            LINE "for i in [0..n] {"
            LINE "  \"e_{i}\" { Position: {i, 0} }"
            LINE "}"
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") == 0);

    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){3});

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    ecs_entity_t e2 = ecs_lookup(world, "e_2");
    test_assert(e2 != 0);
    const Position *p = ecs_get_id(world, e2, position);
    test_assert(p != NULL);
    test_int(p->x, 2);

    ecs_fini(world);
}

void Reactivity_swizzle_expr_is_reactive(void) {
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
            HEAD "const p: Position = {source[Mass].value, 7}"
            LINE "item { Position: p.yx }"
    });
    test_assert(script != 0);

    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    {
        const Position *p = ecs_get_id(world, item, position);
        test_assert(p != NULL);
        test_int(p->x, 7);
        test_int(p->y, 10);
    }

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "item"), item);
    {
        const Position *p = ecs_get_id(world, item, position);
        test_assert(p != NULL);
        test_int(p->x, 7);
        test_int(p->y, 20);
    }

    ecs_fini(world);
}

void Reactivity_has_expr_is_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t flag = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Flag" }),
        .members = {
            {"value", ecs_id(ecs_bool_t)}
        }
    });

    ecs_entity_t without = ecs_entity(world, { .name = "without" });
    ecs_entity_t with = ecs_entity(world, { .name = "with_pos" });
    ecs_set_id(world, with, position, sizeof(Position), &(Position){1, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template T {"
            LINE "  prop other: entity = 0"
            LINE "  child { Flag: {other?[Position]} }"
            LINE "}"
            LINE "t { T: {without} }"
    });
    test_assert(script != 0);

    ecs_entity_t t = ecs_lookup(world, "t");
    ecs_entity_t tmpl = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "t.child");
    test_assert(t != 0);
    test_assert(tmpl != 0);
    test_assert(child != 0);
    {
        const bool *f = ecs_get_id(world, child, flag);
        test_assert(f != NULL);
        test_bool(*f, false);
    }

    ecs_set_id(world, t, tmpl, sizeof(ecs_entity_t), &with);

    test_uint(ecs_lookup(world, "t.child"), child);
    {
        const bool *f = ecs_get_id(world, child, flag);
        test_assert(f != NULL);
        test_bool(*f, true);
    }

    ecs_fini(world);
}

void Reactivity_method_expr_is_reactive(void) {
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

    test_assert(ecs_method(world, {
        .name = "doubled",
        .parent = ecs_id(ecs_f32_t),
        .return_type = ecs_id(ecs_f32_t),
        .callback = reactivity_doubled_callback
    }) != 0);

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const v = source[Mass].value"
            LINE "item { Position: {v.doubled(), 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    {
        const Position *p = ecs_get_id(world, item, position);
        test_assert(p != NULL);
        test_int(p->x, 20);
    }

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){15});

    test_uint(ecs_lookup(world, "item"), item);
    {
        const Position *p = ecs_get_id(world, item, position);
        test_assert(p != NULL);
        test_int(p->x, 30);
    }

    ecs_fini(world);
}

void Reactivity_interpolated_string_width_is_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t index = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Index" }),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t label = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Label" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){12});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const w = source[Index].value"
            LINE "const v = 7.5"
            LINE "item { Label: {\"[{v:w}]\"} }"
    });
    test_assert(script != 0);

    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    {
        char *const *l = ecs_get_id(world, item, label);
        test_assert(l != NULL);
        test_str(*l, "[    7.500000]");
    }

    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){14});

    test_uint(ecs_lookup(world, "item"), item);
    {
        char *const *l = ecs_get_id(world, item, label);
        test_assert(l != NULL);
        test_str(*l, "[      7.500000]");
    }

    ecs_fini(world);
}

void Reactivity_interpolated_string_precision_is_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t index = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Index" }),
        .members = {
            {"value", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t label = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Label" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "const p = source[Index].value"
            LINE "const v = 1.5"
            LINE "item { Label: {\"[{v:.p}]\"} }"
    });
    test_assert(script != 0);

    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);
    {
        char *const *l = ecs_get_id(world, item, label);
        test_assert(l != NULL);
        test_str(*l, "[1.5]");
    }

    ecs_set_id(world, source, index, sizeof(int32_t), &(int32_t){3});

    test_uint(ecs_lookup(world, "item"), item);
    {
        char *const *l = ecs_get_id(world, item, label);
        test_assert(l != NULL);
        test_str(*l, "[1.500]");
    }

    ecs_fini(world);
}

void Reactivity_parse_failure_clears_observers(void) {
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
            HEAD "item { Position: {source[Mass].value, 0} }"
    });
    test_assert(script != 0);

    ecs_entity_t observers[8] = {0};
    int32_t observer_count = 0;
    ecs_iter_t it = ecs_children(world, script);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            if (ecs_has_id(world, it.entities[i], EcsObserver)) {
                test_assert(observer_count < 8);
                observers[observer_count ++] = it.entities[i];
            }
        }
    }
    test_int(observer_count, 1);

    ecs_log_set_level(-4);
    test_int(ecs_script_update(world, script, 0,
        HEAD "item { Position: {"), -1);
    ecs_log_set_level(-1);

    int32_t i;
    for (i = 0; i < observer_count; i ++) {
        test_assert(!ecs_is_alive(world, observers[i]));
    }

    observer_count = 0;
    it = ecs_children(world, script);
    while (ecs_children_next(&it)) {
        int32_t j;
        for (j = 0; j < it.count; j ++) {
            if (ecs_has_id(world, it.entities[j], EcsObserver)) {
                observer_count ++;
            }
        }
    }
    test_int(observer_count, 0);

    /* A parse error leaves the entities from the last successful run in
     * place, but the script must no longer react to source changes. */
    ecs_entity_t item = ecs_lookup(world, "item");
    test_assert(item != 0);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    test_uint(ecs_lookup(world, "item"), item);
    const Position *p = ecs_get_id(world, item, position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);
    test_assert(s->script == NULL);

    ecs_fini(world);
}

void Reactivity_for_preserves_unchanged_entities(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
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

    ecs_set_id(world, old_0, mass, sizeof(Mass), &(Mass){42});

    ecs_set_id(world, source, position_i,
        sizeof(PositionI), &(PositionI){3, 0});

    test_assert(ecs_is_alive(world, old_0));
    test_assert(ecs_is_alive(world, old_1));
    test_uint(ecs_lookup(world, "item_0"), old_0);
    test_uint(ecs_lookup(world, "item_1"), old_1);
    test_assert(ecs_lookup(world, "item_2") != 0);

    const Mass *m = ecs_get_id(world, old_0, mass);
    test_assert(m != NULL);
    test_int(m->value, 42);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_nested_initializer(void) {
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
    ecs_entity_t inner = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Inner" }),
        .members = {
            {"e", ecs_id(ecs_entity_t)}
        }
    });
    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Outer" }),
        .members = {
            {"inner", inner}
        }
    });
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "holder {"
            LINE "  Outer: {inner: {e: new {"
            LINE "    Position: {3, 4}"
            LINE "    Marker"
            LINE "  }}}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
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

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_function_arg(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "fn ident(e: entity) -> entity { e }"
            LINE "reactive { Position: {source[Mass].value, 0} }"
            LINE "holder {"
            LINE "  entity: {ident(new {"
            LINE "    Position: {3, 4}"
            LINE "    Marker"
            LINE "  })}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
    test_assert(created != 0);

    ecs_entity_t holder = ecs_lookup(world, "holder");
    test_assert(holder != 0);
    const ecs_entity_t *value = ecs_get(world, holder, ecs_entity_t);
    test_assert(value != NULL);
    test_uint(*value, created);

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

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_interpolated_string(void) {
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
    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Label" }),
        .members = {
            {"value", ecs_id(ecs_string_t)}
        }
    });
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "holder {"
            LINE "  Label: {\"{new { Position: {3, 4} Marker }}\"}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
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

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_component_expr(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "holder {"
            LINE "  Position: {(new { Position: {3, 4} Marker })[Position].x, 0}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
    test_assert(created != 0);

    ecs_entity_t holder = ecs_lookup(world, "holder");
    test_assert(holder != 0);
    const Position *p = ecs_get_id(world, holder, position);
    test_assert(p != NULL);
    test_int(p->x, 3);

    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){20});

    ecs_entity_t reactive = ecs_lookup(world, "reactive");
    test_assert(reactive != 0);
    p = ecs_get_id(world, reactive, position);
    test_assert(p != NULL);
    test_int(p->x, 20);

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_template_prop(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "template T {"
            LINE "  prop e: entity = 0"
            LINE "}"
            LINE "T inst(new { Position: {3, 4} Marker })"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
    test_assert(created != 0);

    ecs_entity_t inst = ecs_lookup(world, "inst");
    test_assert(inst != 0);

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

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_entity_name(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "\"holder_{new { Position: {3, 4} Marker }}\" {}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
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

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_new_entity_survives_skipped_statement_in_if_condition(void) {
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
    ecs_entity_t marker = ecs_entity(world, { .name = "Marker" });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, mass, sizeof(Mass), &(Mass){10});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "reactive { Position: {source[Mass].value, 0} }"
            LINE "if (new { Position: {3, 4} Marker }) != 0 {"
            LINE "  holder {}"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t created = 0;
    {
        ecs_iter_t it = ecs_each_id(world, marker);
        while (ecs_each_next(&it)) {
            test_int(it.count, 1);
            created = it.entities[0];
        }
    }
    test_assert(created != 0);

    test_assert(ecs_lookup(world, "holder") != 0);

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

    test_assert(ecs_is_alive(world, created));
    test_assert(ecs_has_id(world, created, marker));
    p = ecs_get_id(world, created, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    ecs_fini(world);
}

void Reactivity_for_keyed_entity_survives_collection_change(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3});

    test_assert(ecs_is_alive(world, row_10));
    test_assert(ecs_is_alive(world, row_20));
    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);

    ecs_entity_t row_5 = ecs_lookup(world, "row_5");
    test_assert(row_5 != 0);
    test_assert(row_5 != row_10);
    test_assert(row_5 != row_20);

    ecs_fini(world);
}

void Reactivity_for_keyed_external_component_survives(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    test_assert(row_10 != 0);
    ecs_set_id(world, row_10, mass, sizeof(Mass), &(Mass){42});

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    const Mass *m = ecs_get_id(world, row_10, mass);
    test_assert(m != NULL);
    test_int(m->value, 42);

    ecs_fini(world);
}

void Reactivity_for_keyed_removed_key_is_deleted(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}, 3});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    ecs_entity_t row_30 = ecs_lookup(world, "row_30");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    test_assert(row_30 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 30}, 2});

    test_assert(!ecs_is_alive(world, row_20));
    test_uint(ecs_lookup(world, "row_20"), 0);
    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_30"), row_30);

    ecs_fini(world);
}

void Reactivity_for_keyed_new_key_is_added(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);

    ecs_entity_t row_30 = ecs_lookup(world, "row_30");
    test_assert(row_30 != 0);
    test_assert(row_30 != row_10);
    test_assert(row_30 != row_20);

    ecs_fini(world);
}

void Reactivity_for_keyed_reorder_preserves_identity(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}, 3});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    ecs_entity_t row_30 = ecs_lookup(world, "row_30");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    test_assert(row_30 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{30, 20, 10}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    test_uint(ecs_lookup(world, "row_30"), row_30);

    ecs_fini(world);
}

void Reactivity_for_keyed_unkeyed_entity_is_recreated(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "  _ { Velocity: {1, 0} }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);

    ecs_entity_t unkeyed[2] = {0};
    int32_t unkeyed_count = 0;
    ecs_iter_t it = ecs_each_id(world, velocity);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(unkeyed_count < 2);
            unkeyed[unkeyed_count ++] = it.entities[i];
        }
    }
    test_int(unkeyed_count, 2);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);

    test_assert(!ecs_is_alive(world, unkeyed[0]));
    test_assert(!ecs_is_alive(world, unkeyed[1]));

    ecs_fini(world);
}

void Reactivity_for_keyed_mixed_external_components(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "  _ { Velocity: {1, 0} }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    ecs_set_id(world, row_10, mass, sizeof(Mass), &(Mass){42});

    ecs_entity_t unkeyed[2] = {0};
    int32_t unkeyed_count = 0;
    ecs_iter_t it = ecs_each_id(world, velocity);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(unkeyed_count < 2);
            unkeyed[unkeyed_count ++] = it.entities[i];
        }
    }
    test_int(unkeyed_count, 2);
    ecs_set_id(world, unkeyed[0], mass, sizeof(Mass), &(Mass){7});

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    const Mass *m = ecs_get_id(world, row_10, mass);
    test_assert(m != NULL);
    test_int(m->value, 42);

    test_assert(!ecs_is_alive(world, unkeyed[0]));
    test_assert(!ecs_is_alive(world, unkeyed[1]));

    ecs_fini(world);
}

void Reactivity_for_keyed_multiple_keys_per_iteration(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "  \"label_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    ecs_entity_t label_10 = ecs_lookup(world, "label_10");
    ecs_entity_t label_20 = ecs_lookup(world, "label_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    test_assert(label_10 != 0);
    test_assert(label_20 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    test_uint(ecs_lookup(world, "label_10"), label_10);
    test_uint(ecs_lookup(world, "label_20"), label_20);
    test_assert(ecs_lookup(world, "row_5") != 0);
    test_assert(ecs_lookup(world, "label_5") != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{20, 10, 5}, 3});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    test_uint(ecs_lookup(world, "label_10"), label_10);
    test_uint(ecs_lookup(world, "label_20"), label_20);

    ecs_fini(world);
}

void Reactivity_for_keyed_multiple_keys_removed_together(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}, 3});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {}"
            LINE "  \"label_{id}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    ecs_entity_t row_30 = ecs_lookup(world, "row_30");
    ecs_entity_t label_20 = ecs_lookup(world, "label_20");
    ecs_entity_t label_30 = ecs_lookup(world, "label_30");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    test_assert(row_30 != 0);
    test_assert(label_20 != 0);
    test_assert(label_30 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 30}, 2});

    test_assert(!ecs_is_alive(world, row_20));
    test_assert(!ecs_is_alive(world, label_20));
    test_uint(ecs_lookup(world, "row_20"), 0);
    test_uint(ecs_lookup(world, "label_20"), 0);
    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_30"), row_30);
    test_uint(ecs_lookup(world, "label_30"), label_30);

    ecs_fini(world);
}

void Reactivity_for_unkeyed_only_entities_are_recreated(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  _ { Velocity: {id, 0} }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t old[2] = {0};
    int32_t old_count = 0;
    ecs_iter_t it = ecs_each_id(world, velocity);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(old_count < 2);
            old[old_count ++] = it.entities[i];
        }
    }
    test_int(old_count, 2);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3});

    test_assert(!ecs_is_alive(world, old[0]));
    test_assert(!ecs_is_alive(world, old[1]));

    int32_t new_count = 0;
    it = ecs_each_id(world, velocity);
    while (ecs_each_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != old[0]);
            test_assert(it.entities[i] != old[1]);
            new_count ++;
        }
    }
    test_int(new_count, 3);

    ecs_fini(world);
}

void Reactivity_for_keyed_outer_condition_toggles_rows(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t count;
        ecs_i32_t show;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)},
            {"show", ecs_id(ecs_i32_t)}
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
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20}, 2, 1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "if source[Ids].show > 0 {"
            LINE "  for i in 0..source[Ids].count {"
            LINE "    const id = source[Ids].values[i]"
            LINE "    \"row_{id}\" { Position: {id, 0} }"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3, 1});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3, 0});

    test_assert(!ecs_is_alive(world, row_10));
    test_assert(!ecs_is_alive(world, row_20));
    test_uint(ecs_lookup(world, "row_10"), 0);
    test_uint(ecs_lookup(world, "row_20"), 0);

    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{5, 10, 20}, 3, 1});

    ecs_entity_t new_10 = ecs_lookup(world, "row_10");
    test_assert(new_10 != 0);
    const Position *p = ecs_get_id(world, new_10, position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_assert(ecs_lookup(world, "row_5") != 0);
    test_assert(ecs_lookup(world, "row_20") != 0);

    ecs_fini(world);
}

void Reactivity_for_keyed_per_item_condition_toggles_row(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t visible[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"visible", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
        }
    });
    ecs_entity_t mass = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Mass" }),
        .members = {
            {"value", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids),
        &(Ids){{10, 20, 30}, {1, 1, 1}, 3});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  if source[Ids].visible[i] > 0 {"
            LINE "    \"row_{id}\" {}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);
    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    ecs_entity_t row_30 = ecs_lookup(world, "row_30");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    test_assert(row_30 != 0);
    ecs_set_id(world, row_20, mass, sizeof(Mass), &(Mass){42});

    ecs_set_id(world, source, ids, sizeof(Ids),
        &(Ids){{10, 20, 30}, {0, 1, 1}, 3});

    test_assert(!ecs_is_alive(world, row_10));
    test_uint(ecs_lookup(world, "row_10"), 0);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    test_uint(ecs_lookup(world, "row_30"), row_30);

    const Mass *m = ecs_get_id(world, row_20, mass);
    test_assert(m != NULL);
    test_int(m->value, 42);

    ecs_set_id(world, source, ids, sizeof(Ids),
        &(Ids){{10, 20, 30}, {1, 1, 1}, 3});

    test_assert(ecs_lookup(world, "row_10") != 0);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    test_uint(ecs_lookup(world, "row_30"), row_30);

    ecs_fini(world);
}

void Reactivity_conditional_component_on_named_entity(void) {
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
    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {1, 2}"
            LINE "  if condition[Mass].value > 0 {"
            LINE "    Velocity: {3, 4}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    test_assert(ecs_has_id(world, foo, velocity));
    ecs_set_id(world, foo, position_i,
        sizeof(PositionI), &(PositionI){7, 8});

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    test_uint(ecs_lookup(world, "foo"), foo);
    test_assert(ecs_is_alive(world, foo));
    test_assert(!ecs_has_id(world, foo, velocity));

    const Position *p = ecs_get_id(world, foo, position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    const PositionI *pi = ecs_get_id(world, foo, position_i);
    test_assert(pi != NULL);
    test_int(pi->x, 7);
    test_int(pi->y, 8);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    test_uint(ecs_lookup(world, "foo"), foo);
    const Velocity *v = ecs_get_id(world, foo, velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    p = ecs_get_id(world, foo, position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Reactivity_conditional_component_on_keyed_for_entity(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[4];
        ecs_i32_t active[4];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 4},
            {"active", ecs_id(ecs_i32_t), .count = 4},
            {"count", ecs_id(ecs_i32_t)}
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

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids),
        &(Ids){{10, 20}, {1, 1}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  const id = source[Ids].values[i]"
            LINE "  \"row_{id}\" {"
            LINE "    Position: {id, 0}"
            LINE "    if source[Ids].active[i] > 0 {"
            LINE "      Velocity: {3, 4}"
            LINE "    }"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t row_10 = ecs_lookup(world, "row_10");
    ecs_entity_t row_20 = ecs_lookup(world, "row_20");
    test_assert(row_10 != 0);
    test_assert(row_20 != 0);
    test_assert(ecs_has_id(world, row_10, velocity));
    test_assert(ecs_has_id(world, row_20, velocity));

    ecs_set_id(world, source, ids, sizeof(Ids),
        &(Ids){{10, 20}, {0, 1}, 2});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    test_uint(ecs_lookup(world, "row_20"), row_20);
    test_assert(!ecs_has_id(world, row_10, velocity));
    test_assert(ecs_has_id(world, row_20, velocity));

    const Position *p = ecs_get_id(world, row_10, position);
    test_assert(p != NULL);
    test_int(p->x, 10);

    ecs_set_id(world, source, ids, sizeof(Ids),
        &(Ids){{10, 20}, {1, 1}, 2});

    test_uint(ecs_lookup(world, "row_10"), row_10);
    const Velocity *v = ecs_get_id(world, row_10, velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    ecs_fini(world);
}

void Reactivity_conditional_component_on_template_instance(void) {
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
    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "template Panel {"
            LINE "  prop hover: bool = false"
            LINE "  Position: {1, 2}"
            LINE "  if hover {"
            LINE "    Velocity: {3, 4}"
            LINE "  }"
            LINE "}"
            LINE "Panel instance()"
    });
    test_assert(script != 0);

    ecs_entity_t panel = ecs_lookup(world, "Panel");
    ecs_entity_t instance = ecs_lookup(world, "instance");
    test_assert(panel != 0);
    test_assert(instance != 0);
    test_assert(!ecs_has_id(world, instance, velocity));
    ecs_set_id(world, instance, position_i,
        sizeof(PositionI), &(PositionI){7, 8});

    struct {
        ecs_bool_t hover;
    } props = { true };
    ecs_set_id(world, instance, panel, sizeof(props), &props);

    test_uint(ecs_lookup(world, "instance"), instance);
    const Velocity *v = ecs_get_id(world, instance, velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);

    const PositionI *pi = ecs_get_id(world, instance, position_i);
    test_assert(pi != NULL);
    test_int(pi->x, 7);
    test_int(pi->y, 8);

    props.hover = false;
    ecs_set_id(world, instance, panel, sizeof(props), &props);

    test_uint(ecs_lookup(world, "instance"), instance);
    test_assert(!ecs_has_id(world, instance, velocity));

    const Position *p = ecs_get_id(world, instance, position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Reactivity_two_conditional_components_toggle_independently(void) {
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
    ecs_entity_t velocity = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Velocity" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });
    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t cond_a = ecs_entity(world, { .name = "cond_a" });
    ecs_entity_t cond_b = ecs_entity(world, { .name = "cond_b" });
    ecs_set_id(world, cond_a, mass, sizeof(Mass), &(Mass){1});
    ecs_set_id(world, cond_b, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "foo {"
            LINE "  Position: {1, 2}"
            LINE "  if cond_a[Mass].value > 0 {"
            LINE "    Velocity: {3, 4}"
            LINE "  }"
            LINE "  if cond_b[Mass].value > 0 {"
            LINE "    PositionI: {5, 6}"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t foo = ecs_lookup(world, "foo");
    test_assert(foo != 0);
    test_assert(ecs_has_id(world, foo, velocity));
    test_assert(ecs_has_id(world, foo, position_i));

    ecs_set_id(world, cond_a, mass, sizeof(Mass), &(Mass){-1});

    test_uint(ecs_lookup(world, "foo"), foo);
    test_assert(!ecs_has_id(world, foo, velocity));
    const PositionI *pi = ecs_get_id(world, foo, position_i);
    test_assert(pi != NULL);
    test_int(pi->x, 5);
    test_int(pi->y, 6);

    ecs_set_id(world, cond_b, mass, sizeof(Mass), &(Mass){-1});

    test_uint(ecs_lookup(world, "foo"), foo);
    test_assert(!ecs_has_id(world, foo, velocity));
    test_assert(!ecs_has_id(world, foo, position_i));

    ecs_set_id(world, cond_a, mass, sizeof(Mass), &(Mass){1});

    test_uint(ecs_lookup(world, "foo"), foo);
    const Velocity *v = ecs_get_id(world, foo, velocity);
    test_assert(v != NULL);
    test_int(v->x, 3);
    test_int(v->y, 4);
    test_assert(!ecs_has_id(world, foo, position_i));

    ecs_fini(world);
}

void Reactivity_conditional_component_on_child_entity(void) {
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
    ecs_entity_t position_i = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "PositionI" }),
        .members = {
            {"x", ecs_id(ecs_i32_t)},
            {"y", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t condition = ecs_entity(world, { .name = "condition" });
    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "parent {"
            LINE "  Position: {1, 2}"
            LINE "  child {"
            LINE "    Position: {3, 4}"
            LINE "    if condition[Mass].value > 0 {"
            LINE "      Velocity: {5, 6}"
            LINE "    }"
            LINE "  }"
            LINE "}"
    });
    test_assert(script != 0);

    ecs_entity_t parent = ecs_lookup(world, "parent");
    ecs_entity_t child = ecs_lookup(world, "parent.child");
    test_assert(parent != 0);
    test_assert(child != 0);
    test_assert(ecs_has_id(world, child, velocity));
    ecs_set_id(world, child, position_i,
        sizeof(PositionI), &(PositionI){7, 8});

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){-1});

    test_uint(ecs_lookup(world, "parent"), parent);
    test_uint(ecs_lookup(world, "parent.child"), child);
    test_assert(!ecs_has_id(world, child, velocity));

    const Position *p = ecs_get_id(world, child, position);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);

    const PositionI *pi = ecs_get_id(world, child, position_i);
    test_assert(pi != NULL);
    test_int(pi->x, 7);
    test_int(pi->y, 8);

    ecs_set_id(world, condition, mass, sizeof(Mass), &(Mass){1});

    test_uint(ecs_lookup(world, "parent"), parent);
    test_uint(ecs_lookup(world, "parent.child"), child);
    const Velocity *v = ecs_get_id(world, child, velocity);
    test_assert(v != NULL);
    test_int(v->x, 5);
    test_int(v->y, 6);

    p = ecs_get_id(world, parent, position);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Reactivity_interpolated_name_w_indexed_expr(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[3];
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 3}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "\"row_{source[Ids].values[1]}\" {}"
    });
    test_assert(script != 0);
    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error == NULL);

    test_assert(ecs_lookup(world, "row_20") != 0);

    ecs_fini(world);
}

void Reactivity_interpolated_name_w_indexed_expr_in_for(void) {
    ecs_world_t *world = ecs_init();

    typedef struct {
        ecs_i32_t values[3];
        ecs_i32_t count;
    } Ids;

    ecs_entity_t ids = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ids" }),
        .members = {
            {"values", ecs_id(ecs_i32_t), .count = 3},
            {"count", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_set_id(world, source, ids, sizeof(Ids), &(Ids){{10, 20, 30}, 2});

    ecs_entity_t script = ecs_script(world, {
        .entity = ecs_entity(world, { .name = "main" }),
        .code =
            HEAD "for i in 0..source[Ids].count {"
            LINE "  \"row_{source[Ids].values[i]}\" {}"
            LINE "}"
    });
    test_assert(script != 0);
    const EcsScript *script_data = ecs_get(world, script, EcsScript);
    test_assert(script_data != NULL);
    test_assert(script_data->error == NULL);

    test_assert(ecs_lookup(world, "row_10") != 0);
    test_assert(ecs_lookup(world, "row_20") != 0);
    test_uint(ecs_lookup(world, "row_30"), 0);

    ecs_fini(world);
}
