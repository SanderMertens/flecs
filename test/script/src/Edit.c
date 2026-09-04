#include <script.h>
#include "../../../src/addons/script/script.h"

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void Edit_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

static ecs_entity_t edit_position(ecs_world_t *world) {
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

static ecs_entity_t edit_velocity(ecs_world_t *world) {
    ECS_COMPONENT(world, Velocity);

    ecs_struct(world, {
        .entity = ecs_id(Velocity),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    return ecs_id(Velocity);
}

static void edit_tag(ecs_world_t *world, const char *name) {
    ecs_entity_init(world, &(ecs_entity_desc_t){ .name = name });
}

static ecs_entity_t edit_health(ecs_world_t *world) {
    ecs_entity_t h = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Health" });
    ecs_primitive_init(world, &(ecs_primitive_desc_t){
        .entity = h, .kind = EcsF32 });
    return h;
}

static ecs_script_t* edit_parse(ecs_world_t *world, const char *code) {
    ecs_script_t *script = ecs_script_parse(world, "test", code, &ir_desc, NULL);
    test_assert(script != NULL);
    test_int(0, ecs_script_eval(script, &ir_desc, NULL));
    return script;
}

static char* edit_span(const ecs_script_t *script, ecs_entity_t e) {
    ecs_script_source_t src = {0};
    if (!ecs_script_entity_source(script, e, &src)) {
        return NULL;
    }
    char *result = ecs_os_malloc(src.length + 1);
    ecs_os_memcpy(result, &script->code[src.offset], src.length);
    result[src.length] = '\0';
    return result;
}

void Edit_source_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "foo {}"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo {}");
    ecs_os_free(span);

    span = edit_span(script, ecs_lookup(world, "bar"));
    test_str(span, "bar {}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_scope(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Tag"
    LINE "}"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo {\n  Tag\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_kind(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Kind");

    const char *expr =
    HEAD "Kind foo"
    LINE "Kind bar";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "Kind foo");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_kind_scope(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Kind");

    const char *expr =
    HEAD "Kind foo {"
    LINE "}"
    LINE "Kind bar";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "Kind foo {\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_base(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Kind");
    edit_tag(world, "Base");
    edit_tag(world, "Base2");

    const char *expr =
    HEAD "Kind foo : Base, Base2"
    LINE "Kind bar";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "Kind foo : Base, Base2");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_base_scope(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Base");
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo : Base {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo : Base {\n  Tag\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_paren(void) {
    ecs_world_t *world = ecs_init();

    edit_position(world);

    const char *expr =
    HEAD "Position foo(x: 10, y: 20)"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "Position foo(x: 10, y: 20)");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_paren_scope(void) {
    ecs_world_t *world = ecs_init();

    edit_position(world);

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "Position foo(10, 20) {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "Position foo(10, 20) {\n  Tag\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_anonymous_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t ecs_id(Position) = edit_position(world);

    const char *expr =
    HEAD "{"
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_query_t *q = ecs_query(world, { .terms = {{ ecs_id(Position) }}});
    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    ecs_entity_t e = it.entities[0];
    test_bool(false, ecs_query_next(&it));
    ecs_query_fini(q);

    char *span = edit_span(script, e);
    test_str(span, "{\n  Position: {10, 20}\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_child_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "foo {"
    LINE "  bar {"
    LINE "    zoo {}"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo.bar.zoo"));
    test_str(span, "zoo {}");
    ecs_os_free(span);

    span = edit_span(script, ecs_lookup(world, "foo.bar"));
    test_str(span, "bar {\n    zoo {}\n  }");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_w_trailing_comment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "// leading"
    LINE "foo {} // trailing"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo {}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_in_with_scope(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "with Tag {"
    LINE "  foo {}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo {}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_template_instance(void) {
    ecs_world_t *world = ecs_init();

    edit_position(world);

    const char *expr =
    HEAD "template Ship {"
    LINE "  prop size: f32 = 10"
    LINE "  child {"
    LINE "    Position: {$size, 0}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Ship foo(size: 20)";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "Ship foo(size: 20)");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_template_body_entity(void) {
    ecs_world_t *world = ecs_init();

    edit_position(world);

    const char *expr =
    HEAD "template Ship {"
    LINE "  child {"
    LINE "    Position: {10, 20}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Ship foo()";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_entity_t child = ecs_lookup(world, "foo.child");
    test_assert(child != 0);
    test_bool(false, ecs_script_entity_source(script, child, NULL));

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_for_loop_entity(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "for i in 0..3 {"
    LINE "  \"e_{i}\" {}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_entity_t e = ecs_lookup(world, "e_1");
    test_assert(e != 0);
    test_bool(false, ecs_script_entity_source(script, e, NULL));

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_foreign_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = edit_parse(world, "foo {}");

    ecs_entity_t e = ecs_entity(world, { .name = "other" });
    test_bool(false, ecs_script_entity_source(script, e, NULL));

    ecs_script_t *script2 = edit_parse(world, "zoo {}");
    test_bool(false, ecs_script_entity_source(
        script, ecs_lookup(world, "zoo"), NULL));
    test_bool(true, ecs_script_entity_source(
        script2, ecs_lookup(world, "zoo"), NULL));

    ecs_script_free(script2);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_unknown_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = edit_parse(world, "foo {}");

    test_bool(false, ecs_script_entity_source(script, 0, NULL));
    test_bool(false, ecs_script_entity_source(script, 100000, NULL));

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_entity_declared_twice(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "TagA");
    edit_tag(world, "TagB");

    const char *expr =
    HEAD "foo {"
    LINE "  TagA"
    LINE "}"
    LINE "foo {"
    LINE "  TagB"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo {\n  TagA\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

typedef struct edit_file_t {
    const char *name;
    const char *content;
    size_t pos;
} edit_file_t;

static edit_file_t edit_files[4];
static ecs_os_api_fopen_t edit_default_fopen = NULL;
static ecs_os_api_fread_t edit_default_fread = NULL;
static ecs_os_api_fclose_t edit_default_fclose = NULL;

static edit_file_t* edit_file_get(FILE *file) {
    int32_t i;
    for (i = 0; i < 4; i ++) {
        if (file == (FILE*)&edit_files[i]) {
            return &edit_files[i];
        }
    }
    return NULL;
}

static FILE* edit_fopen(const char *file, const char *mode) {
    int32_t i;
    for (i = 0; i < 4; i ++) {
        if (edit_files[i].name && !strcmp(file, edit_files[i].name)) {
            edit_files[i].pos = 0;
            return (FILE*)&edit_files[i];
        }
    }
    return edit_default_fopen(file, mode);
}

static size_t edit_fread(void *ptr, size_t size, size_t count, FILE *file) {
    edit_file_t *f = edit_file_get(file);
    size_t remaining, requested;
    if (!f) {
        return edit_default_fread(ptr, size, count, file);
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

static void edit_fclose(FILE *file) {
    if (edit_file_get(file)) {
        return;
    }
    edit_default_fclose(file);
}

static void edit_files_init(void) {
    ecs_os_set_api_defaults();
    edit_default_fopen = ecs_os_api.fopen_;
    edit_default_fread = ecs_os_api.fread_;
    edit_default_fclose = ecs_os_api.fclose_;

    ecs_os_api_t api = ecs_os_api;
    api.fopen_ = edit_fopen;
    api.fread_ = edit_fread;
    api.fclose_ = edit_fclose;
    ecs_os_set_api(&api);
}

static void edit_files_fini(void) {
    ecs_os_api_t api = ecs_os_api;
    api.fopen_ = edit_default_fopen;
    api.fread_ = edit_default_fread;
    api.fclose_ = edit_default_fclose;
    ecs_os_set_api(&api);
    memset(edit_files, 0, sizeof(edit_files));
}

void Edit_source_include_entity(void) {
    edit_files_init();
    edit_files[0].name = "included.flecs";
    edit_files[0].content = "included_entity {}\n";

    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "include included.flecs"
    LINE "own_entity {}";

    ecs_script_t *script = edit_parse(world, expr);

    test_assert(ecs_lookup(world, "included_entity") != 0);
    test_assert(ecs_lookup(world, "own_entity") != 0);

    test_bool(true, ecs_script_entity_source(
        script, ecs_lookup(world, "own_entity"), NULL));
    test_bool(false, ecs_script_entity_source(
        script, ecs_lookup(world, "included_entity"), NULL));

    ecs_script_free(script);
    ecs_fini(world);

    edit_files_fini();
}

void Edit_source_managed_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
            HEAD "foo {"
            LINE "  bar {}"
            LINE "}"
    });

    test_assert(s != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    test_assert(sc != NULL);
    test_assert(sc->script != NULL);

    char *span = edit_span(sc->script, ecs_lookup(world, "foo.bar"));
    test_str(span, "bar {}");
    ecs_os_free(span);

    ecs_fini(world);
}

void Edit_source_line_column(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Kind");

    const char *expr =
    HEAD "Kind foo"
    LINE ""
    LINE "  bar {"
    LINE "  }";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_source_t src = {0};
    test_bool(true, ecs_script_entity_source(
        script, ecs_lookup(world, "foo"), &src));
    test_int(1, src.line);
    test_int(1, src.column);
    test_int(0, src.offset);
    test_int(8, src.length);
    test_bool(false, src.has_scope);

    test_bool(true, ecs_script_entity_source(
        script, ecs_lookup(world, "bar"), &src));
    test_int(3, src.line);
    test_int(3, src.column);
    test_bool(true, src.has_scope);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_source_component_stmt_ends(void) {
    ecs_world_t *world = ecs_init();

    edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {10, 20}"
    LINE "}"
    LINE "bar {"
    LINE "  Position: {"
    LINE "    x: 10,"
    LINE "    y: 20"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    char *span = edit_span(script, ecs_lookup(world, "foo"));
    test_str(span, "foo {\n  Position: {10, 20}\n}");
    ecs_os_free(span);

    span = edit_span(script, ecs_lookup(world, "bar"));
    test_str(span, "bar {\n  Position: {\n    x: 10,\n    y: 20\n  }\n}");
    ecs_os_free(span);

    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_named_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {x: 10, y: 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_positional_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {30, 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_empty_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {30, 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_partial_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {y: 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_multi_line_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {"
    LINE "    x: 10,"
    LINE "    y: 20"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_value_form(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t h = edit_health(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Health: 10"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    float v = 20;
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), h, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Health: 20"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_collection_form(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {10, 20}"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_set_expr(
        edits, ecs_lookup(world, "foo"), p, "[1, 2]"));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: [1, 2]"
        LINE "  Tag"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_paren_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "Position foo(x: 10, y: 20)";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, "Position foo(x: 30, y: 40)");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_preserves_comments(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "// header comment"
    LINE "foo { // entity comment"
    LINE "  /* before */ Position: {10, 20} // after"
    LINE "  Tag"
    LINE "}"
    LINE "// footer comment";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "// header comment"
        LINE "foo { // entity comment"
        LINE "  /* before */ Position: {30, 40} // after"
        LINE "  Tag"
        LINE "}"
        LINE "// footer comment");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_preserves_blank_lines_and_indent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD ""
    LINE "foo {"
    LINE ""
    LINE "      Position: {10, 20}"
    LINE ""
    LINE "}"
    LINE "";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD ""
        LINE "foo {"
        LINE ""
        LINE "      Position: {30, 40}"
        LINE ""
        LINE "}"
        LINE "");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_tabs_indent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    ecs_entity_t v_id = edit_velocity(world);

    const char *expr =
    HEAD "foo {"
    LINE "\tPosition: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    Velocity vel = {1, 2};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), v_id, &vel));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "\tPosition: {30, 40}"
        LINE "\tVelocity: {x: 1, y: 2}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_append_to_existing_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Tag"
        LINE "  Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_append_to_empty_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "    Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_append_no_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Kind");

    const char *expr =
    HEAD "parent {"
    LINE "  Kind foo"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "parent.foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "parent {"
        LINE "  Kind foo {"
        LINE "      Position: {x: 30, y: 40}"
        LINE "  }"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_append_w_using(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t scope = ecs_entity(world, { .name = "game" });
    ecs_entity_t prev = ecs_set_scope(world, scope);
    ecs_entity_t p = edit_position(world);
    ecs_set_scope(world, prev);

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "using game"
    LINE ""
    LINE "foo {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "using game"
        LINE ""
        LINE "foo {"
        LINE "  Tag"
        LINE "  Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_append_w_module(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "module game"
    LINE ""
    LINE "foo {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "game.foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "module game"
        LINE ""
        LINE "foo {"
        LINE "  Tag"
        LINE "  Position: {x: 30, y: 40}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_expr(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_set_expr(
        edits, ecs_lookup(world, "foo"), p, "{x: 1, y: 2}"));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {x: 1, y: 2}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_expr_append(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_set_expr(
        edits, ecs_lookup(world, "foo"), p, "{1, 2}"));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Tag"
        LINE "  Position: {1, 2}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_nested_child(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "a {"
    LINE "  b {"
    LINE "    c {"
    LINE "      Position: {10, 20}"
    LINE "    }"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "a.b.c"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "a {"
        LINE "  b {"
        LINE "    c {"
        LINE "      Position: {1, 2}"
        LINE "    }"
        LINE "  }"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_anonymous_entity(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "{"
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_query_t *q = ecs_query(world, { .terms = {{ p }}});
    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    ecs_entity_t e = it.entities[0];
    ecs_iter_fini(&it);
    ecs_query_fini(q);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(0, ecs_script_edits_set(edits, e, p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "{"
        LINE "  Position: {1, 2}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_in_with_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "with Tag {"
    LINE "  foo {"
    LINE "    Position: {10, 20}"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "with Tag {"
        LINE "  foo {"
        LINE "    Position: {1, 2}"
        LINE "  }"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_float_roundtrip(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {0, 0}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1.0f / 3.0f, -12345.678f};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);

    ecs_world_t *world2 = ecs_init();
    ecs_entity_t p2 = edit_position(world2);
    test_int(0, ecs_script_run_w_desc(world2, "test", result, &ir_desc, NULL));

    ecs_entity_t foo = ecs_lookup(world2, "foo");
    test_assert(foo != 0);
    const Position *ptr = ecs_get_id(world2, foo, p2);
    test_assert(ptr != NULL);
    test_assert(ptr->x == v.x);
    test_assert(ptr->y == v.y);

    ecs_fini(world2);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_template_body_entity_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "template Ship {"
    LINE "  child {"
    LINE "    Position: {10, 20}"
    LINE "  }"
    LINE "}"
    LINE ""
    LINE "Ship foo()";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_entity_t child = ecs_lookup(world, "foo.child");
    test_assert(child != 0);

    ecs_log_set_level(-4);
    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(-1, ecs_script_edits_set(edits, child, p, &v));
    test_int(-1, ecs_script_edits_delete(edits, child));
    ecs_log_set_level(0);

    char *result = ecs_script_edits_apply(edits);
    test_str(result, expr);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_for_loop_entity_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "for i in 0..3 {"
    LINE "  \"e_{i}\" {"
    LINE "    Position: {10, 20}"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_entity_t e = ecs_lookup(world, "e_1");
    test_assert(e != 0);

    ecs_log_set_level(-4);
    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(-1, ecs_script_edits_set(edits, e, p, &v));
    ecs_log_set_level(0);

    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_foreign_entity_fails(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    ecs_script_t *script = edit_parse(world, "foo {}");

    ecs_entity_t other = ecs_entity(world, { .name = "other" });

    ecs_log_set_level(-4);
    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(-1, ecs_script_edits_set(edits, other, p, &v));
    test_int(-1, ecs_script_edits_remove(edits, other, p));
    test_int(-1, ecs_script_edits_delete(edits, other));
    ecs_log_set_level(0);

    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_twice_same_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v1 = {1, 2};
    Position v2 = {3, 4};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v1));
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v2));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {3, 4}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_remove_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {10, 20}"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_remove(edits, ecs_lookup(world, "foo"), p));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Tag"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_remove_component_multi_line(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Position: {"
    LINE "    x: 10,"
    LINE "    y: 20"
    LINE "  }"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_remove(edits, ecs_lookup(world, "foo"), p));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Tag"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_remove_missing_component(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_remove(edits, ecs_lookup(world, "foo"), p));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, expr);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_remove_tag(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Tag");
    edit_tag(world, "Other");

    const char *expr =
    HEAD "foo {"
    LINE "  Tag"
    LINE "  Other"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_remove(edits,
        ecs_lookup(world, "foo"), ecs_lookup(world, "Tag")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Other"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_first(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Tag"
    LINE "}"
    LINE "bar {}"
    LINE "zoo {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "foo")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "bar {}"
        LINE "zoo {}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_middle(void) {
    ecs_world_t *world = ecs_init();

    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {}"
    LINE "bar {"
    LINE "  Tag"
    LINE "}"
    LINE "zoo {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "bar")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {}"
        LINE "zoo {}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_last(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "foo {}"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "bar")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, "foo {}\n");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_nested_child(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "a {"
    LINE "  b {"
    LINE "    c {}"
    LINE "    d {}"
    LINE "  }"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "a.b.c")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "a {"
        LINE "  b {"
        LINE "    d {}"
        LINE "  }"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_entity_w_edits_inside(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "a {"
    LINE "  b {"
    LINE "    Position: {10, 20}"
    LINE "  }"
    LINE "}"
    LINE "c {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "a.b"), p, &v));
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "a")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, "c {}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_w_trailing_comment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "foo {} // comment"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "foo")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, "bar {}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_delete_preserves_preceding_comment(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "// keep me"
    LINE "foo {}"
    LINE "bar {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "foo")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "// keep me"
        LINE "bar {}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_multiple_edits_one_apply(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    ecs_entity_t vel = edit_velocity(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "a {"
    LINE "  Position: {1, 2}"
    LINE "}"
    LINE "b {"
    LINE "  Position: {3, 4}"
    LINE "}"
    LINE "c {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position pa = {10, 20};
    Position pb = {30, 40};
    Velocity vc = {5, 6};
    test_int(0, ecs_script_edits_set(edits, ecs_lookup(world, "a"), p, &pa));
    test_int(0, ecs_script_edits_set(edits, ecs_lookup(world, "b"), p, &pb));
    test_int(0, ecs_script_edits_set(edits, ecs_lookup(world, "c"), vel, &vc));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "a {"
        LINE "  Position: {10, 20}"
        LINE "}"
        LINE "b {"
        LINE "  Position: {30, 40}"
        LINE "}"
        LINE "c {"
        LINE "  Tag"
        LINE "  Velocity: {x: 5, y: 6}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_apply_twice_same_result(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);

    const char *expr =
    HEAD "a {"
    LINE "  Position: {1, 2}"
    LINE "}"
    LINE "b {}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position pa = {10, 20};
    test_int(0, ecs_script_edits_set(edits, ecs_lookup(world, "a"), p, &pa));
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "b")));

    char *first = ecs_script_edits_apply(edits);
    char *second = ecs_script_edits_apply(edits);
    test_str(first, second);
    test_str(first,
        HEAD "a {"
        LINE "  Position: {10, 20}"
        LINE "}"
        LINE "");

    ecs_os_free(first);
    ecs_os_free(second);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_apply_result_runs(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "a {"
    LINE "  Position: {1, 2}"
    LINE "}"
    LINE "b {"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position pa = {10, 20};
    Position pb = {30, 40};
    test_int(0, ecs_script_edits_set(edits, ecs_lookup(world, "a"), p, &pa));
    test_int(0, ecs_script_edits_set(edits, ecs_lookup(world, "b"), p, &pb));

    char *result = ecs_script_edits_apply(edits);
    test_assert(result != NULL);

    ecs_world_t *world2 = ecs_init();
    ecs_entity_t p2 = edit_position(world2);
    edit_tag(world2, "Tag");
    test_int(0, ecs_script_run_w_desc(world2, "test", result, &ir_desc, NULL));

    const Position *ptr = ecs_get_id(world2, ecs_lookup(world2, "a"), p2);
    test_assert(ptr != NULL);
    test_int(10, ptr->x);
    test_int(20, ptr->y);

    ptr = ecs_get_id(world2, ecs_lookup(world2, "b"), p2);
    test_assert(ptr != NULL);
    test_int(30, ptr->x);
    test_int(40, ptr->y);

    ecs_fini(world2);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_apply_deleted_entity_gone(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code =
            HEAD "a {}"
            LINE "b {}"
            LINE "c {}"
    });

    test_assert(s != 0);
    test_assert(ecs_lookup(world, "b") != 0);

    const EcsScript *sc = ecs_get(world, s, EcsScript);
    ecs_script_edits_t *edits = ecs_script_edits_new(sc->script);
    test_int(0, ecs_script_edits_delete(edits, ecs_lookup(world, "b")));

    char *result = ecs_script_edits_apply(edits);
    test_str(result, "a {}\nc {}");
    ecs_script_edits_free(edits);

    test_int(0, ecs_script_update(world, s, 0, result));

    test_assert(ecs_lookup(world, "a") != 0);
    test_assert(ecs_lookup(world, "b") == 0);
    test_assert(ecs_lookup(world, "c") != 0);

    ecs_os_free(result);
    ecs_fini(world);
}

void Edit_apply_no_edits(void) {
    ecs_world_t *world = ecs_init();

    const char *expr =
    HEAD "foo {} // comment"
    LINE ""
    LINE "bar {"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    char *result = ecs_script_edits_apply(edits);
    test_str(result, expr);

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_in_module_script(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t scope = ecs_entity(world, { .name = "game" });
    ecs_entity_t prev = ecs_set_scope(world, scope);
    ecs_entity_t p = edit_position(world);
    ecs_set_scope(world, prev);

    const char *expr =
    HEAD "module game"
    LINE ""
    LINE "foo {"
    LINE "  Position: {10, 20}"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {1, 2};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "game.foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "module game"
        LINE ""
        LINE "foo {"
        LINE "  Position: {1, 2}"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}

void Edit_set_replaces_tag_stmt(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p = edit_position(world);
    edit_tag(world, "Tag");

    const char *expr =
    HEAD "foo {"
    LINE "  Position"
    LINE "  Tag"
    LINE "}";

    ecs_script_t *script = edit_parse(world, expr);

    ecs_script_edits_t *edits = ecs_script_edits_new(script);
    Position v = {30, 40};
    test_int(0, ecs_script_edits_set(
        edits, ecs_lookup(world, "foo"), p, &v));

    char *result = ecs_script_edits_apply(edits);
    test_str(result,
        HEAD "foo {"
        LINE "  Position: {x: 30, y: 40}"
        LINE "  Tag"
        LINE "}");

    ecs_os_free(result);
    ecs_script_edits_free(edits);
    ecs_script_free(script);
    ecs_fini(world);
}
