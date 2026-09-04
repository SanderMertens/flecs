#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void EditTemplate_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

static ecs_entity_t et_position(ecs_world_t *world) {
    ECS_COMPONENT(world, Position);

    ecs_struct(world, {
        .entity = ecs_id(Position),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    return ecs_id(Position);
}

static void et_tag(ecs_world_t *world, const char *name) {
    ecs_entity_init(world, &(ecs_entity_desc_t){ .name = name });
}

static ecs_script_t* et_parse(ecs_world_t *world, const char *code) {
    ecs_script_t *script = ecs_script_parse(world, "test", code, &ir_desc, NULL);
    test_assert(script != NULL);
    test_int(0, ecs_script_eval(script, &ir_desc, NULL));
    return script;
}

static char* et_span(const ecs_script_t *script, ecs_entity_t e) {
    ecs_script_source_t src = {0};
    if (!ecs_script_entity_source(script, e, &src)) {
        return NULL;
    }
    char *result = ecs_os_malloc(src.length + 1);
    ecs_os_memcpy(result, &script->code[src.offset], src.length);
    result[src.length] = '\0';
    return result;
}

typedef struct et_file_t {
    const char *name;
    const char *content;
    size_t pos;
} et_file_t;

static et_file_t et_files[4];
static ecs_os_api_fopen_t et_default_fopen = NULL;
static ecs_os_api_fread_t et_default_fread = NULL;
static ecs_os_api_fclose_t et_default_fclose = NULL;

static et_file_t* et_file_get(FILE *file) {
    int32_t i;
    for (i = 0; i < 4; i ++) {
        if (file == (FILE*)&et_files[i]) {
            return &et_files[i];
        }
    }
    return NULL;
}

static FILE* et_fopen(const char *file, const char *mode) {
    int32_t i;
    for (i = 0; i < 4; i ++) {
        if (et_files[i].name && !strcmp(file, et_files[i].name)) {
            et_files[i].pos = 0;
            return (FILE*)&et_files[i];
        }
    }
    return et_default_fopen(file, mode);
}

static size_t et_fread(void *ptr, size_t size, size_t count, FILE *file) {
    et_file_t *f = et_file_get(file);
    size_t remaining, requested;
    if (!f) {
        return et_default_fread(ptr, size, count, file);
    }
    remaining = strlen(f->content) - f->pos;
    requested = size * count;
    if (requested > remaining) {
        requested = remaining;
    }
    memcpy(ptr, f->content + f->pos, requested);
    f->pos += requested;
    return requested;
}

static void et_fclose(FILE *file) {
    if (et_file_get(file)) {
        return;
    }
    et_default_fclose(file);
}

static void et_files_init(void) {
    ecs_os_set_api_defaults();
    et_default_fopen = ecs_os_api.fopen_;
    et_default_fread = ecs_os_api.fread_;
    et_default_fclose = ecs_os_api.fclose_;

    ecs_os_api_t api = ecs_os_api;
    api.fopen_ = et_fopen;
    api.fread_ = et_fread;
    api.fclose_ = et_fclose;
    ecs_os_set_api(&api);
}

static void et_files_fini(void) {
    ecs_os_api_t api = ecs_os_api;
    api.fopen_ = et_default_fopen;
    api.fread_ = et_default_fread;
    api.fclose_ = et_default_fclose;
    ecs_os_set_api(&api);
    memset(et_files, 0, sizeof(et_files));
}

void EditTemplate_source_body_entity(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, trunk, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);
    test_bool(true, src.has_scope);
    test_int(2, src.line);
    test_int(3, src.column);

    char *span = et_span(script, trunk);
    test_str(span, "trunk {\n    Position: {0, 1}\n  }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_no_scope(void) {
    ecs_world_t *world = ecs_init();

    et_tag(world, "Tag");

    const char *expr =
    HEAD "template Tree {"
    LINE "  Tag trunk"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, trunk, &src));
    test_bool(false, src.has_scope);

    char *span = et_span(script, trunk);
    test_str(span, "Tag trunk");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_nested(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    knot {"
    LINE "      Position: {2, 3}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t knot = ecs_lookup(world, "t1.trunk.knot");
    test_assert(knot != 0);

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, knot, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    char *span = et_span(script, knot);
    test_str(span, "knot {\n      Position: {2, 3}\n    }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  _ { Position: {1, 2} }"
    LINE "  _ { Position: {3, 4} }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t t1 = ecs_lookup(world, "t1");
    test_assert(t1 != 0);

    ecs_entity_t first = 0, second = 0;
    ecs_iter_t it = ecs_children(world, t1);
    while (ecs_children_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            const float *ptr = ecs_get_id(world, it.entities[i], p);
            test_assert(ptr != NULL);
            if (ptr[0] == 1) {
                first = it.entities[i];
            } else if (ptr[0] == 3) {
                second = it.entities[i];
            }
        }
    }

    test_assert(first != 0);
    test_assert(second != 0);

    char *span = et_span(script, first);
    test_str(span, "_ { Position: {1, 2} }");
    ecs_os_free(span);

    span = et_span(script, second);
    test_str(span, "_ { Position: {3, 4} }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_in_with_scope(void) {
    ecs_world_t *world = ecs_init();

    et_tag(world, "Tag");
    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  with Tag {"
    LINE "    trunk {"
    LINE "      Position: {0, 1}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    char *span = et_span(script, trunk);
    test_str(span, "trunk {\n      Position: {0, 1}\n    }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_in_if_scope(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop big: bool = true"
    LINE "  if big {"
    LINE "    trunk {"
    LINE "      Position: {0, 1}"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    char *span = et_span(script, trunk);
    test_str(span, "trunk {\n      Position: {0, 1}\n    }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_for_loop_entity(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  for i in 0..2 {"
    LINE "    \"child_$i\" { Position: {i, 0} }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t child = ecs_lookup(world, "t1.child_1");
    test_assert(child != 0);

    test_bool(false, ecs_script_entity_source(script, child, NULL));
    test_uint(0, ecs_script_entity_owner(world, child));

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_instance_stmt_has_no_template(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk { Position: {0, 1} }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(
        script, ecs_lookup(world, "t1"), &src));
    test_uint(0, src.template_);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_two_instances(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk { Position: {0, 1} }"
    LINE "}"
    LINE ""
    LINE "Tree t1()"
    LINE "Tree t2()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_source_t src1 = {0}, src2 = {0};
    test_bool(true, ecs_script_entity_source(
        script, ecs_lookup(world, "t1.trunk"), &src1));
    test_bool(true, ecs_script_entity_source(
        script, ecs_lookup(world, "t2.trunk"), &src2));

    test_int(src1.offset, src2.offset);
    test_int(src1.length, src2.length);
    test_uint(src1.template_, src2.template_);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_nested_template_instance(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Knot {"
    LINE "  bump { Position: {5, 6} }"
    LINE "}"
    LINE ""
    LINE "template Tree {"
    LINE "  Knot knot1()"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t knot1 = ecs_lookup(world, "t1.knot1");
    test_assert(knot1 != 0);

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, knot1, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    char *span = et_span(script, knot1);
    test_str(span, "Knot knot1()");
    ecs_os_free(span);

    ecs_entity_t bump = ecs_lookup(world, "t1.knot1.bump");
    test_assert(bump != 0);

    src = (ecs_script_source_t){0};
    test_bool(true, ecs_script_entity_source(script, bump, &src));
    test_uint(ecs_lookup(world, "Knot"), src.template_);

    span = et_span(script, bump);
    test_str(span, "bump { Position: {5, 6} }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_other_script(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_script_t *script = et_parse(world,
        HEAD "template Tree {"
        LINE "  trunk { Position: {0, 1} }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_script_t *other = et_parse(world, "foo {}");

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    test_bool(true, ecs_script_entity_source(script, trunk, NULL));
    test_bool(false, ecs_script_entity_source(other, trunk, NULL));

    ecs_script_free(other);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_set_body_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "t1.trunk"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {10, 20}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_set_body_entity_replaces_prop_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop x: f32 = 0"
    LINE "  trunk {"
    LINE "    Position: {x, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1(x: 3)";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "t1.trunk"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  prop x: f32 = 0"
        LINE "  trunk {"
        LINE "    Position: {10, 20}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1(x: 3)");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_set_body_entity_empty_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "t1.trunk"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "      Position: {x: 10, y: 20}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_set_body_entity_via_either_instance(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()"
    LINE "Tree t2()";

    ecs_script_t *script = et_parse(world, expr);

    Position v = {10, 20};

    ecs_script_edits_t *edits1 = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_set(
        edits1, ecs_lookup(world, "t1.trunk"), p, &v));
    char *result1 = ecs_script_edits_apply(edits1);

    ecs_script_edits_t *edits2 = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_set(
        edits2, ecs_lookup(world, "t2.trunk"), p, &v));
    char *result2 = ecs_script_edits_apply(edits2);

    test_str(result1, result2);

    ecs_os_free(result1);
    ecs_os_free(result2);
    ecs_script_edits_free(edits1);
    ecs_script_edits_free(edits2);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_delete_body_entity(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "  leaves {"
    LINE "    Position: {0, 2}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(
        edits, ecs_lookup(world, "t1.trunk")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  leaves {"
        LINE "    Position: {0, 2}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_remove_body_entity_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    et_tag(world, "Tag");

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Tag"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_remove(
        edits, ecs_lookup(world, "t1.trunk"), p));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Tag"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_apply_update_all_instances(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {0, 1}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()"
        LINE "Tree t2()"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "t1.trunk"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);
    ecs_script_edits_free(edits);

    test_int(0, ecs_script_update(world, s, 0, result));
    ecs_os_free(result);

    {
        const float *ptr = ecs_get_id(world, ecs_lookup(world, "t1.trunk"), p);
        test_assert(ptr != NULL);
        test_int(10, ptr[0]);
        test_int(20, ptr[1]);
    }

    {
        const float *ptr = ecs_get_id(world, ecs_lookup(world, "t2.trunk"), p);
        test_assert(ptr != NULL);
        test_int(10, ptr[0]);
        test_int(20, ptr[1]);
    }

    ecs_fini(world);
}

void EditTemplate_apply_update_instance_override_wins(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {0, 1}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()"
        LINE "Tree t2() {"
        LINE "  trunk {"
        LINE "    Position: {9, 9}"
        LINE "  }"
        LINE "}"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "t1.trunk"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);
    ecs_script_edits_free(edits);

    test_int(0, ecs_script_update(world, s, 0, result));
    ecs_os_free(result);

    {
        const float *ptr = ecs_get_id(world, ecs_lookup(world, "t1.trunk"), p);
        test_assert(ptr != NULL);
        test_int(10, ptr[0]);
        test_int(20, ptr[1]);
    }

    {
        const float *ptr = ecs_get_id(world, ecs_lookup(world, "t2.trunk"), p);
        test_assert(ptr != NULL);
        test_int(9, ptr[0]);
        test_int(9, ptr[1]);
    }

    ecs_fini(world);
}

void EditTemplate_apply_update_delete_removes_from_all(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {0, 1}"
        LINE "  }"
        LINE "  leaves {"
        LINE "    Position: {0, 2}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()"
        LINE "Tree t2()"
    });

    test_assert(s != 0);
    test_assert(ecs_lookup(world, "t1.trunk") != 0);
    test_assert(ecs_lookup(world, "t2.trunk") != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
    test_int(0, ecs_script_edits_delete(
        edits, ecs_lookup(world, "t2.trunk")));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);
    ecs_script_edits_free(edits);

    test_int(0, ecs_script_update(world, s, 0, result));
    ecs_os_free(result);

    test_assert(ecs_lookup(world, "t1.trunk") == 0);
    test_assert(ecs_lookup(world, "t2.trunk") == 0);
    test_assert(ecs_lookup(world, "t1.leaves") != 0);
    test_assert(ecs_lookup(world, "t2.leaves") != 0);

    ecs_fini(world);
}

void EditTemplate_entity_owner_body_entity(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {0, 1}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()"
    });

    test_assert(s != 0);

    test_uint(s, ecs_script_entity_owner(world, ecs_lookup(world, "t1")));
    test_uint(s, ecs_script_entity_owner(world, ecs_lookup(world, "t1.trunk")));

    ecs_fini(world);
}

void EditTemplate_entity_owner_body_entity_unmanaged(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_script_t *script = et_parse(world,
        HEAD "template Tree {"
        LINE "  trunk { Position: {0, 1} }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    test_uint(0, ecs_script_entity_owner(world, ecs_lookup(world, "t1")));
    test_uint(0, ecs_script_entity_owner(world, ecs_lookup(world, "t1.trunk")));

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_template_in_included_file(void) {
    et_files_init();
    et_files[0].name = "scene/templates.flecs";
    et_files[0].content =
        "template Tree {\n"
        "  trunk {\n"
        "    Position: {0, 1}\n"
        "  }\n"
        "}\n";
    et_files[1].name = "scene/main.flecs";
    et_files[1].content =
        "include templates.flecs\n"
        "Tree t1()\n";

    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t s = ecs_script(world, {
        .ir = ir_enabled, .filename = "scene/main.flecs" });
    test_assert(s != 0);

    ecs_entity_t inc = ecs_lookup_path_w_sep(
        world, 0, "scene/templates.flecs", "/", NULL, false);
    test_assert(inc != 0);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    const EcsScript *psc = ecs_get(world, s, EcsScript);
    const EcsScript *isc = ecs_get(world, inc, EcsScript);
    test_assert(psc != NULL);
    test_assert(isc != NULL);

    test_bool(false, ecs_script_entity_source(psc->script, trunk, NULL));
    test_bool(true, ecs_script_entity_source(isc->script, trunk, NULL));

    test_uint(inc, ecs_script_entity_owner(world, trunk));
    test_uint(s, ecs_script_entity_owner(world, ecs_lookup(world, "t1")));

    ecs_fini(world);
    et_files_fini();
}

void EditTemplate_edit_template_in_included_file(void) {
    et_files_init();
    et_files[0].name = "scene/templates.flecs";
    et_files[0].content =
        "template Tree {\n"
        "  trunk {\n"
        "    Position: {0, 1}\n"
        "  }\n"
        "}\n";
    et_files[1].name = "scene/main.flecs";
    et_files[1].content =
        "include templates.flecs\n"
        "Tree t1()\n";

    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    ecs_entity_t s = ecs_script(world, {
        .ir = ir_enabled, .filename = "scene/main.flecs" });
    test_assert(s != 0);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    ecs_entity_t owner = ecs_script_entity_owner(world, trunk);
    test_assert(owner != 0);
    test_assert(owner != s);

    const EcsScript *osc = ecs_get(world, owner, EcsScript);
    test_assert(osc != NULL);

    ecs_script_edits_t *edits = ecs_script_edits_new(osc->script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(edits, trunk, p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        "template Tree {\n"
        "  trunk {\n"
        "    Position: {10, 20}\n"
        "  }\n"
        "}\n");
    ecs_script_edits_free(edits);

    const EcsScript *psc = ecs_get(world, s, EcsScript);
    test_assert(psc != NULL);
    test_str(psc->script->code, et_files[1].content);

    test_int(0, ecs_script_update(world, owner, 0, result));
    test_int(0, ecs_script_update(world, s, 0, et_files[1].content));
    ecs_os_free(result);

    ecs_entity_t new_trunk = ecs_lookup(world, "t1.trunk");
    test_assert(new_trunk != 0);
    const float *ptr = ecs_get_id(world, new_trunk, p);
    test_assert(ptr != NULL);
    test_int(10, ptr[0]);
    test_int(20, ptr[1]);

    ecs_fini(world);
    et_files_fini();
}

void EditTemplate_source_body_entity_computed_name(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop name: string = \"trunk\""
    LINE "  \"$name\" {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    test_bool(false, ecs_script_entity_source(script, trunk, NULL));
    test_uint(0, ecs_script_entity_owner(world, trunk));

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_edit_body_for_loop_entity_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  for i in 0..2 {"
    LINE "    \"child_$i\" { Position: {i, 0} }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t child = ecs_lookup(world, "t1.child_1");
    test_assert(child != 0);

    ecs_log_set_level(-4);
    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(-1, ecs_script_edits_set(edits, child, p, &v));
    test_int(-1, ecs_script_edits_remove(edits, child, p));
    test_int(-1, ecs_script_edits_delete(edits, child));
    ecs_log_set_level(-1);

    char *result = ecs_script_edits_apply(edits);
    test_str(result, expr);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

static bool et_has_template(
    ecs_world_t *world,
    const char *path,
    const char *template_name)
{
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    ecs_entity_t t = ecs_lookup(world, template_name);
    test_assert(t != 0);
    return ecs_has_pair(world, e, EcsScriptTemplate, t);
}

void EditTemplate_body_entity_template_pair_in_nested_scopes(void) {
    ecs_world_t *world = ecs_init();

    et_tag(world, "Tag");
    et_tag(world, "Rel");
    et_tag(world, "Obj");
    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop big: bool = true"
    LINE "  body {"
    LINE "    Position: {0, 1}"
    LINE "    if big {"
    LINE "      shell {"
    LINE "        Position: {1, 2}"
    LINE "        base { Position: {2, 3} }"
    LINE "        for i in 0..2 {"
    LINE "          \"cut_$i\" { Position: {3, 4} }"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "    with Tag {"
    LINE "      trim { Position: {4, 5} }"
    LINE "    }"
    LINE "    (Rel, Obj) {"
    LINE "      knob { Position: {5, 6} }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    test_bool(true, et_has_template(world, "t1.body", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.shell", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.shell.base", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.shell.cut_0", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.shell.cut_1", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.trim", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.knob", "Tree"));

    test_assert(ecs_has_id(world, ecs_lookup(world, "t1.body.trim"),
        ecs_lookup(world, "Tag")));
    test_assert(ecs_has_pair(world, ecs_lookup(world, "t1.body.knob"),
        ecs_lookup(world, "Rel"), EcsWildcard));

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_nested_in_if_scope(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop big: bool = true"
    LINE "  body {"
    LINE "    if big {"
    LINE "      shell {"
    LINE "        Position: {1, 2}"
    LINE "        base { Position: {2, 3} }"
    LINE "        for i in 0..2 {"
    LINE "          \"cut_$i\" { Position: {3, 4} }"
    LINE "        }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t shell = ecs_lookup(world, "t1.body.shell");
    test_assert(shell != 0);

    test_bool(true, et_has_template(world, "t1.body.shell", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.shell.base", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.shell.cut_0", "Tree"));

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, shell, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    char *span = et_span(script, shell);
    test_str(span,
        "shell {\n"
        "        Position: {1, 2}\n"
        "        base { Position: {2, 3} }\n"
        "        for i in 0..2 {\n"
        "          \"cut_$i\" { Position: {3, 4} }\n"
        "        }\n"
        "      }");
    ecs_os_free(span);

    ecs_entity_t base = ecs_lookup(world, "t1.body.shell.base");
    test_assert(base != 0);

    src = (ecs_script_source_t){0};
    test_bool(true, ecs_script_entity_source(script, base, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    span = et_span(script, base);
    test_str(span, "base { Position: {2, 3} }");
    ecs_os_free(span);

    ecs_entity_t cut = ecs_lookup(world, "t1.body.shell.cut_0");
    test_assert(cut != 0);
    test_bool(false, ecs_script_entity_source(script, cut, NULL));
    test_uint(0, ecs_script_entity_owner(world, cut));

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_in_else_scope(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  prop big: bool = true"
    LINE "  body {"
    LINE "    if big {"
    LINE "      shell { Position: {1, 2} }"
    LINE "    } else {"
    LINE "      hollow { Position: {3, 4} }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1(big: true)"
    LINE "Tree t2(big: false)";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t shell = ecs_lookup(world, "t1.body.shell");
    test_assert(shell != 0);
    test_assert(ecs_lookup(world, "t1.body.hollow") == 0);

    ecs_entity_t hollow = ecs_lookup(world, "t2.body.hollow");
    test_assert(hollow != 0);
    test_assert(ecs_lookup(world, "t2.body.shell") == 0);

    test_bool(true, et_has_template(world, "t1.body.shell", "Tree"));
    test_bool(true, et_has_template(world, "t2.body.hollow", "Tree"));

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, shell, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    char *span = et_span(script, shell);
    test_str(span, "shell { Position: {1, 2} }");
    ecs_os_free(span);

    src = (ecs_script_source_t){0};
    test_bool(true, ecs_script_entity_source(script, hollow, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    span = et_span(script, hollow);
    test_str(span, "hollow { Position: {3, 4} }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_source_body_entity_in_with_scope_nested(void) {
    ecs_world_t *world = ecs_init();

    et_tag(world, "Tag");
    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  body {"
    LINE "    with Tag {"
    LINE "      trunk {"
    LINE "        Position: {0, 1}"
    LINE "        knot { Position: {2, 3} }"
    LINE "      }"
    LINE "    }"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t knot = ecs_lookup(world, "t1.body.trunk.knot");
    test_assert(knot != 0);

    test_bool(true, et_has_template(world, "t1.body.trunk", "Tree"));
    test_bool(true, et_has_template(world, "t1.body.trunk.knot", "Tree"));

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(script, knot, &src));
    test_uint(ecs_lookup(world, "Tree"), src.template_);

    char *span = et_span(script, knot);
    test_str(span, "knot { Position: {2, 3} }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_entity_owner_nested_template_component_assignment(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Roof {"
        LINE "  prop hip: bool = false"
        LINE "  body {"
        LINE "    if hip {"
        LINE "      shell {"
        LINE "        Position: {1, 2}"
        LINE "        base { Position: {2, 3} }"
        LINE "      }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "template House {"
        LINE "  roof {"
        LINE "    Roof: {hip: true}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "House h1()"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_entity_t roof = ecs_lookup(world, "h1.roof");
    ecs_entity_t body = ecs_lookup(world, "h1.roof.body");
    ecs_entity_t shell = ecs_lookup(world, "h1.roof.body.shell");
    ecs_entity_t base = ecs_lookup(world, "h1.roof.body.shell.base");
    test_assert(roof != 0);
    test_assert(body != 0);
    test_assert(shell != 0);
    test_assert(base != 0);

    test_bool(true, et_has_template(world, "h1.roof", "House"));
    test_bool(true, et_has_template(world, "h1.roof.body", "Roof"));
    test_bool(true, et_has_template(world, "h1.roof.body.shell", "Roof"));
    test_bool(true, et_has_template(world, "h1.roof.body.shell.base", "Roof"));

    test_uint(s, ecs_script_entity_owner(world, roof));
    test_uint(s, ecs_script_entity_owner(world, body));
    test_uint(s, ecs_script_entity_owner(world, shell));
    test_uint(s, ecs_script_entity_owner(world, base));

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(sc->script, roof, &src));
    test_uint(ecs_lookup(world, "House"), src.template_);

    src = (ecs_script_source_t){0};
    test_bool(true, ecs_script_entity_source(sc->script, shell, &src));
    test_uint(ecs_lookup(world, "Roof"), src.template_);

    char *span = et_span(sc->script, shell);
    test_str(span,
        "shell {\n"
        "        Position: {1, 2}\n"
        "        base { Position: {2, 3} }\n"
        "      }");
    ecs_os_free(span);

    ecs_fini(world);
}

void EditTemplate_entity_owner_body_entity_in_for_loop_rows(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Roof {"
        LINE "  prop hip: bool = false"
        LINE "  body {"
        LINE "    if hip {"
        LINE "      shell { Position: {1, 2} }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "template House {"
        LINE "  roof {"
        LINE "    Roof: {hip: true}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "template Row {"
        LINE "  prop n: i32 = 2"
        LINE "  for i in 0..n {"
        LINE "    \"h$i\" {"
        LINE "      House: {}"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Row r1()"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_entity_t h0 = ecs_lookup(world, "r1.h0");
    ecs_entity_t h1 = ecs_lookup(world, "r1.h1");
    test_assert(h0 != 0);
    test_assert(h1 != 0);

    test_bool(true, et_has_template(world, "r1.h0", "Row"));
    test_bool(true, et_has_template(world, "r1.h1", "Row"));

    test_bool(false, ecs_script_entity_source(sc->script, h0, NULL));
    test_uint(0, ecs_script_entity_owner(world, h0));

    ecs_entity_t shell = ecs_lookup(world, "r1.h0.roof.body.shell");
    test_assert(shell != 0);
    test_assert(ecs_lookup(world, "r1.h1.roof.body.shell") != 0);

    test_bool(true, et_has_template(world, "r1.h0.roof", "House"));
    test_bool(true, et_has_template(world, "r1.h0.roof.body", "Roof"));
    test_bool(true, et_has_template(world, "r1.h0.roof.body.shell", "Roof"));

    test_uint(s, ecs_script_entity_owner(world, ecs_lookup(world, "r1.h0.roof")));
    test_uint(s, ecs_script_entity_owner(world, shell));

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(sc->script, shell, &src));
    test_uint(ecs_lookup(world, "Roof"), src.template_);

    ecs_fini(world);
}

void EditTemplate_source_body_entity_not_in_scene_script(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t kit = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Roof {"
        LINE "  prop hip: bool = false"
        LINE "  body {"
        LINE "    if hip {"
        LINE "      shell {"
        LINE "        Position: {1, 2}"
        LINE "        base { Position: {2, 3} }"
        LINE "      }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "template House {"
        LINE "  roof {"
        LINE "    Roof: {hip: true}"
        LINE "  }"
        LINE "}"
    });

    test_assert(kit != 0);

    ecs_entity_t scene = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "h1 {"
        LINE "  House: {}"
        LINE "}"
    });

    test_assert(scene != 0);

    const EcsScript *kit_sc = ecs_get(world, kit, EcsScript);
    const EcsScript *scene_sc = ecs_get(world, scene, EcsScript);
    test_assert(kit_sc != NULL);
    test_assert(scene_sc != NULL);

    ecs_entity_t h1 = ecs_lookup(world, "h1");
    ecs_entity_t roof = ecs_lookup(world, "h1.roof");
    ecs_entity_t body = ecs_lookup(world, "h1.roof.body");
    ecs_entity_t shell = ecs_lookup(world, "h1.roof.body.shell");
    ecs_entity_t base = ecs_lookup(world, "h1.roof.body.shell.base");
    test_assert(h1 != 0);
    test_assert(roof != 0);
    test_assert(body != 0);
    test_assert(shell != 0);
    test_assert(base != 0);

    test_uint(scene, ecs_get_target(world, shell, ecs_id(EcsScript), 0));

    test_bool(true, et_has_template(world, "h1.roof", "House"));
    test_bool(true, et_has_template(world, "h1.roof.body", "Roof"));
    test_bool(true, et_has_template(world, "h1.roof.body.shell", "Roof"));
    test_bool(true, et_has_template(world, "h1.roof.body.shell.base", "Roof"));

    test_bool(false, ecs_script_entity_source(scene_sc->script, roof, NULL));
    test_bool(false, ecs_script_entity_source(scene_sc->script, body, NULL));
    test_bool(false, ecs_script_entity_source(scene_sc->script, shell, NULL));
    test_bool(false, ecs_script_entity_source(scene_sc->script, base, NULL));

    test_bool(true, ecs_script_entity_source(scene_sc->script, h1, NULL));
    test_uint(scene, ecs_script_entity_owner(world, h1));

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(kit_sc->script, shell, &src));
    test_uint(ecs_lookup(world, "Roof"), src.template_);

    test_uint(kit, ecs_script_entity_owner(world, roof));
    test_uint(kit, ecs_script_entity_owner(world, body));
    test_uint(kit, ecs_script_entity_owner(world, shell));
    test_uint(kit, ecs_script_entity_owner(world, base));

    ecs_fini(world);
}

void EditTemplate_apply_update_body_entity_in_if_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Tree {"
        LINE "  prop big: bool = true"
        LINE "  body {"
        LINE "    if big {"
        LINE "      shell {"
        LINE "        Position: {0, 1}"
        LINE "      }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()"
        LINE "Tree t2()"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
    Position v = {10, 20};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "t1.body.shell"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);
    ecs_script_edits_free(edits);

    test_int(0, ecs_script_update(world, s, 0, result));
    ecs_os_free(result);

    {
        const float *ptr = ecs_get_id(
            world, ecs_lookup(world, "t1.body.shell"), p);
        test_assert(ptr != NULL);
        test_int(10, ptr[0]);
        test_int(20, ptr[1]);
    }

    {
        const float *ptr = ecs_get_id(
            world, ecs_lookup(world, "t2.body.shell"), p);
        test_assert(ptr != NULL);
        test_int(10, ptr[0]);
        test_int(20, ptr[1]);
    }

    test_bool(true, et_has_template(world, "t1.body.shell", "Tree"));
    test_bool(true, et_has_template(world, "t2.body.shell", "Tree"));

    ecs_fini(world);
}

void EditTemplate_delete_body_entity_recorded_before_delete(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "  leaves {"
    LINE "    Position: {0, 2}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, trunk));
    test_int(1, ecs_script_edits_count(edits));

    ecs_delete(world, trunk);
    test_assert(!ecs_is_alive(world, trunk));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  leaves {"
        LINE "    Position: {0, 2}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_clear_delete_body_entity(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "  leaves {"
    LINE "    Position: {0, 2}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, trunk));
    test_int(1, ecs_script_edits_count(edits));

    ecs_delete(world, trunk);

    test_int(0, ecs_script_edits_clear(edits, trunk, 0));
    test_int(0, ecs_script_edits_count(edits));
    test_int(-1, ecs_script_edits_clear(edits, trunk, 0));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, expr);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_clear_set_body_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = et_position(world);

    const char *expr =
    HEAD "template Tree {"
    LINE "  trunk {"
    LINE "    Position: {0, 1}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Tree t1()";

    ecs_script_t *script = et_parse(world, expr);

    ecs_entity_t trunk = ecs_lookup(world, "t1.trunk");
    test_assert(trunk != 0);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v1 = {10, 20};
    test_int(0, ecs_script_edits_set(edits, trunk, p, &v1));
    test_int(1, ecs_script_edits_count(edits));

    test_int(0, ecs_script_edits_clear(edits, trunk, p));
    test_int(0, ecs_script_edits_count(edits));

    char *unchanged = ecs_script_edits_apply(edits);
    test_str(unchanged, expr);
    ecs_os_free(unchanged);

    Position v2 = {30, 40};
    test_int(0, ecs_script_edits_set(edits, trunk, p, &v2));
    test_int(1, ecs_script_edits_count(edits));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {30, 40}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void EditTemplate_apply_update_delete_body_entity_recorded_before_delete(void) {
    ecs_world_t *world = ecs_init();

    et_position(world);

    ecs_entity_t s = ecs_script(world, { .ir = ir_enabled, .code =
        HEAD "template Tree {"
        LINE "  trunk {"
        LINE "    Position: {0, 1}"
        LINE "  }"
        LINE "  leaves {"
        LINE "    Position: {0, 2}"
        LINE "  }"
        LINE "}"
        LINE ""
        LINE "Tree t1()"
        LINE "Tree t2()"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);

    ecs_entity_t trunk = ecs_lookup(world, "t2.trunk");
    test_assert(trunk != 0);

    ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
    test_int(0, ecs_script_edits_delete(edits, trunk));

    ecs_delete(world, trunk);
    test_assert(!ecs_is_alive(world, trunk));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);
    ecs_script_edits_free(edits);

    test_int(0, ecs_script_update(world, s, 0, result));
    ecs_os_free(result);

    test_assert(ecs_lookup(world, "t1.trunk") == 0);
    test_assert(ecs_lookup(world, "t2.trunk") == 0);
    test_assert(ecs_lookup(world, "t1.leaves") != 0);
    test_assert(ecs_lookup(world, "t2.leaves") != 0);

    ecs_fini(world);
}
