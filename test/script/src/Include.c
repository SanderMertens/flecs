#include <script.h>

typedef struct test_file_t {
    const char *name;
    const char *content;
    size_t pos;
} test_file_t;

#define TEST_FILE_MAX (8)

static test_file_t test_files[TEST_FILE_MAX];

static const char *test_fopen_remap_from;
static const char *test_fopen_remap_to;

static ecs_os_api_fopen_t test_default_fopen = NULL;
static ecs_os_api_fread_t test_default_fread = NULL;
static ecs_os_api_fclose_t test_default_fclose = NULL;

static test_file_t* test_file_get(FILE *file) {
    int32_t i;
    for (i = 0; i < TEST_FILE_MAX; i ++) {
        if (file == (FILE*)&test_files[i]) {
            return &test_files[i];
        }
    }
    return NULL;
}

static FILE* test_fopen(const char *file, const char *mode) {
    int32_t i;
    if (test_fopen_remap_from && !strcmp(file, test_fopen_remap_from)) {
        file = test_fopen_remap_to;
    }
    for (i = 0; i < TEST_FILE_MAX; i ++) {
        if (test_files[i].name && !strcmp(file, test_files[i].name)) {
            test_files[i].pos = 0;
            return (FILE*)&test_files[i];
        }
    }
    return test_default_fopen(file, mode);
}

static size_t test_fread(void *ptr, size_t size, size_t count, FILE *file) {
    test_file_t *f = test_file_get(file);
    size_t remaining, requested;
    if (!f) {
        return test_default_fread(ptr, size, count, file);
    }

    remaining = strlen(f->content) - f->pos;
    requested = size * count;
    if (requested > remaining) {
        requested = remaining;
    }

    memcpy(ptr, &f->content[f->pos], requested);
    f->pos += requested;
    return requested;
}

static void test_fclose(FILE *file) {
    if (!test_file_get(file)) {
        test_default_fclose(file);
    }
}

static void test_files_install_ex(bool with_abort, ecs_os_api_log_t log) {
    memset(test_files, 0, sizeof(test_files));
    test_fopen_remap_from = NULL;
    test_fopen_remap_to = NULL;

    ecs_os_set_api_defaults();
    test_default_fopen = ecs_os_api.fopen_;
    test_default_fread = ecs_os_api.fread_;
    test_default_fclose = ecs_os_api.fclose_;

    ecs_os_api_t os_api = ecs_os_api;
    os_api.fopen_ = test_fopen;
    os_api.fread_ = test_fread;
    os_api.fclose_ = test_fclose;
    if (with_abort) {
        os_api.abort_ = test_abort;
    }
    if (log) {
        os_api.log_ = log;
    }
    ecs_os_set_api(&os_api);

    if (with_abort) {
        ecs_log_set_level(-5);
    }
}

static void test_files_install(void) {
    test_files_install_ex(false, NULL);
}

static void test_file_add(const char *name, const char *content) {
    int32_t i;
    for (i = 0; i < TEST_FILE_MAX; i ++) {
        if (!test_files[i].name) {
            test_files[i].name = name;
            test_files[i].content = content;
            test_files[i].pos = 0;
            return;
        }
    }
    test_assert(false);
}

void Include_include_cycle(void) {
    test_quarantine("1 Sep 2026");

    test_files_install();
    test_file_add("a.flecs", "include b.flecs\nFoo{}\n");
    test_file_add("b.flecs", "include a.flecs\nBar{}\n");

    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_file(world, "a.flecs") != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Include_include_simple(void) {
    test_files_install();
    test_file_add("child.flecs", "Foo{}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);

    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Include_include_subdir(void) {
    test_files_install();
    test_file_add("sub/child.flecs", "Bar{}\n");
    test_file_add("parent.flecs", "include sub/child.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Include_include_missing_file(void) {
    test_files_install_ex(true, NULL);
    test_file_add("parent.flecs", "include does_not_exist.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_parent_dir_not_allowed(void) {
    test_files_install_ex(true, NULL);
    test_file_add("parent.flecs", "include ../other.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_relative_to_current_script(void) {
    test_files_install();
    test_file_add("sub/b.flecs", "B_entity{}\n");
    test_file_add("sub/a.flecs", "include b.flecs\n");
    test_file_add("parent.flecs", "include sub/a.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "B_entity") != 0);

    ecs_fini(world);
}

void Include_include_nested(void) {
    test_files_install();
    test_file_add("c.flecs", "C_entity{}\n");
    test_file_add("b.flecs", "include c.flecs\nB_entity{}\n");
    test_file_add("a.flecs", "include b.flecs\nA_entity{}\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "a.flecs") == 0);
    test_assert(ecs_lookup(world, "A_entity") != 0);
    test_assert(ecs_lookup(world, "B_entity") != 0);
    test_assert(ecs_lookup(world, "C_entity") != 0);

    ecs_fini(world);
}

void Include_include_managed_creates_script_entity(void) {
    test_files_install();
    test_file_add("child.flecs", "ChildEntity{}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "ChildEntity") != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, "child.flecs", "/", NULL, false);
    test_assert(child_script != 0);
    test_assert(ecs_has(world, child_script, EcsScript));

    ecs_fini(world);
}

void Include_include_managed_skips_existing(void) {
    test_files_install();
    test_file_add("child.flecs", "ChildEntity{}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    ecs_entity_t child_entity1 = ecs_lookup(world, "ChildEntity");
    test_assert(child_entity1 != 0);

    test_assert(ecs_script_update(world, script, 0, "include child.flecs\n") == 0);

    ecs_entity_t child_entity2 = ecs_lookup(world, "ChildEntity");
    test_assert(child_entity2 != 0);
    test_assert(child_entity1 == child_entity2);

    ecs_fini(world);
}

void Include_include_managed_nested(void) {
    test_files_install();
    test_file_add("b.flecs", "BEntity{}\n");
    test_file_add("a.flecs", "include b.flecs\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "a.flecs"
    });
    test_assert(script != 0);

    ecs_entity_t b_script = ecs_lookup_path_w_sep(
        world, 0, "b.flecs", "/", NULL, false);
    test_assert(b_script != 0);
    test_assert(ecs_has(world, b_script, EcsScript));

    ecs_fini(world);
}

void Include_include_inline_does_not_create_script_entity(void) {
    test_files_install();
    test_file_add("child.flecs", "ChildInline{}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);

    test_assert(ecs_lookup(world, "ChildInline") != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, "child.flecs", "/", NULL, false);
    if (child_script) {
        test_assert(!ecs_has(world, child_script, EcsScript));
    }

    ecs_fini(world);
}

void Include_include_rejects_absolute_unix_path(void) {
    test_files_install_ex(true, NULL);
    test_file_add("parent.flecs", "include /etc/passwd\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_rejects_windows_drive_letter(void) {
    test_files_install_ex(true, NULL);
    test_file_add("parent.flecs", "include C:/foo.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_with_line_comment(void) {
    test_files_install();
    test_file_add("child.flecs", "LineCommentEntity{}\n");
    test_file_add("parent.flecs", "include child.flecs // load the child\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "LineCommentEntity") != 0);

    ecs_fini(world);
}

void Include_include_with_block_comment(void) {
    test_files_install();
    test_file_add("child.flecs", "BlockCommentEntity{}\n");
    test_file_add("parent.flecs", "include child.flecs /* load child */\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "BlockCommentEntity") != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_template(void) {
    test_files_install_ex(true, NULL);
    test_file_add("child.flecs", "TemplateChild{}\n");
    test_file_add("parent.flecs",
        "template MyTemplate {\n"
        "  include child.flecs\n"
        "}\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_entity_scope(void) {
    test_files_install_ex(true, NULL);
    test_file_add("child.flecs", "Child{}\n");
    test_file_add("parent.flecs",
        "Parent {\n"
        "  include child.flecs\n"
        "}\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_with_scope(void) {
    test_files_install_ex(true, NULL);
    test_file_add("child.flecs", "Child{}\n");
    test_file_add("parent.flecs",
        "Tag {}\n"
        "with Tag {\n"
        "  include child.flecs\n"
        "}\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_if_scope(void) {
    test_files_install_ex(true, NULL);
    test_file_add("child.flecs", "Child{}\n");
    test_file_add("parent.flecs",
        "if true {\n"
        "  include child.flecs\n"
        "}\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_for_scope(void) {
    test_files_install_ex(true, NULL);
    test_file_add("child.flecs", "Child{}\n");
    test_file_add("parent.flecs",
        "for i in 0 .. 1 {\n"
        "  include child.flecs\n"
        "}\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_fini(world);
}

void Include_include_auto_appends_extension(void) {
    test_files_install();
    test_file_add("child.flecs", "AutoExtEntity{}\n");
    test_file_add("parent.flecs", "include child\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "AutoExtEntity") != 0);

    ecs_fini(world);
}

void Include_include_auto_appends_extension_subdir(void) {
    test_files_install();
    test_file_add("sub/child.flecs", "SubAutoExt{}\n");
    test_file_add("parent.flecs", "include sub/child\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "SubAutoExt") != 0);

    ecs_fini(world);
}

void Include_include_keeps_explicit_extension(void) {
    test_files_install();
    test_file_add("child.flecs", "ExplicitExtEntity{}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);
    test_assert(ecs_lookup(world, "ExplicitExtEntity") != 0);

    ecs_fini(world);
}

void Include_include_auto_appends_extension_managed(void) {
    test_files_install();
    test_file_add("child.flecs", "ManagedAutoExt{}\n");
    test_file_add("parent.flecs", "include child\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "ManagedAutoExt") != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, "child.flecs", "/", NULL, false);
    test_assert(child_script != 0);
    test_assert(ecs_has(world, child_script, EcsScript));

    ecs_fini(world);
}

void Include_fopen_override_remaps_filename(void) {
    test_files_install();
    test_file_add("actual.flecs", "RemappedEntity{}\n");
    test_fopen_remap_from = "requested.flecs";
    test_fopen_remap_to = "actual.flecs";

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "requested.flecs") == 0);
    test_assert(ecs_lookup(world, "RemappedEntity") != 0);

    ecs_fini(world);
}

static int include_log_error_count = 0;
static int include_log_error_level = 0;

static void include_log_error_callback(
    int32_t level,
    const char *file,
    int32_t line,
    const char *msg)
{
    (void)file;
    (void)line;
    (void)msg;
    if (level <= -3) {
        include_log_error_count ++;
        include_log_error_level = level;
    }
}

void Include_include_managed_eval_error_logged(void) {
    test_files_install_ex(false, include_log_error_callback);
    test_file_add("child.flecs", "e {\n Foo: {}\n}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_log_set_level(-2);

    include_log_error_count = 0;
    include_log_error_level = 0;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(include_log_error_count > 0);
    test_int(include_log_error_level, -3);

    ecs_fini(world);
}

void Include_include_managed_eval_error_set_on_script(void) {
    test_files_install();
    test_file_add("child.flecs", "e {\n Foo: {}\n}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, "child.flecs", "/", NULL, false);
    test_assert(child_script != 0);
    test_assert(ecs_has(world, child_script, EcsScript));

    const EcsScript *s = ecs_get(world, child_script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);

    ecs_fini(world);
}

void Include_include_using_not_visible_in_parent(void) {
    test_files_install();
    test_file_add("child.flecs",
        "using Foo\n"
        "child_e { Bar }\n");
    test_file_add("parent.flecs",
        "include child.flecs\n"
        "parent_e { Bar }\n");

    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar", .parent = foo });

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    ecs_entity_t child_e = ecs_lookup(world, "child_e");
    test_assert(child_e != 0);
    test_assert(ecs_has_id(world, child_e, bar));

    ecs_entity_t parent_e = ecs_lookup(world, "parent_e");
    test_assert(parent_e == 0 || !ecs_has_id(world, parent_e, bar));

    ecs_fini(world);
}

void Include_include_managed_using_not_visible_in_parent(void) {
    test_files_install();
    test_file_add("child.flecs",
        "using Foo\n"
        "child_e { Bar }\n");
    test_file_add("parent.flecs",
        "include child.flecs\n"
        "parent_e { Bar }\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar", .parent = foo });

    ecs_log_set_level(-4);
    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);

    ecs_entity_t child_e = ecs_lookup(world, "child_e");
    test_assert(child_e != 0);
    test_assert(ecs_has_id(world, child_e, bar));

    ecs_entity_t parent_e = ecs_lookup(world, "parent_e");
    test_assert(parent_e == 0 || !ecs_has_id(world, parent_e, bar));

    ecs_fini(world);
}

void Include_include_managed_keeps_implicit_meta_in_parent(void) {
    test_files_install();
    test_file_add("child.flecs", "Child{}\n");
    test_file_add("parent.flecs",
        "include child.flecs\n"
        "struct Position(x: f32)\n"
        "e { Position: {10} }\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, pos));

    ecs_fini(world);
}

void Include_include_keeps_implicit_meta_in_parent(void) {
    test_files_install();
    test_file_add("child.flecs", "Child{}\n");
    test_file_add("parent.flecs",
        "include child.flecs\n"
        "struct Position(x: f32)\n"
        "e { Position: {10} }\n");

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, pos));

    ecs_fini(world);
}

void Include_include_forward_ref_to_later_include_is_retried(void) {
    test_files_install();
    test_file_add("user.flecs",
        "panel { Widget w() }\n"
        "sibling { Ui: {7} }\n");
    test_file_add("lib.flecs",
        "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n");
    test_file_add("parent.flecs",
        "include user.flecs\n"
        "include lib.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "Widget") != 0);
    test_assert(ecs_lookup(world, "panel.w") != 0);
    test_assert(ecs_lookup(world, "sibling") != 0);

    ecs_fini(world);
}

void Include_const_does_not_cross_include(void) {
    test_files_install();
    test_file_add("palette.flecs", "const Amber: Ui = {240}\n");
    test_file_add("parent.flecs",
        "include palette.flecs\n"
        "a { Ui: Amber }\n"
        "b { Ui: {1} }\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") == 0);

    ecs_fini(world);
}

void Include_export_const_in_scope_crosses_include(void) {
    test_files_install();
    test_file_add("cfg.flecs",
        "cfg {\n"
        "  export const maxBays: i32 = 4\n"
        "  export const rate: f32 = 1.5\n"
        "}\n");
    test_file_add("use.flecs",
        "d { Text: {text: \"bays {cfg.maxBays} rate {cfg.rate:.1}\"} }\n");
    test_file_add("parent.flecs",
        "include cfg.flecs\n"
        "include use.flecs\n");

    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"text", ecs_id(ecs_string_t)}
        }
    });

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);

    ecs_entity_t d = ecs_lookup(world, "d");
    test_assert(d != 0);
    const void *t = ecs_get_id(world, d, text);
    test_assert(t != NULL);
    test_str(*(char**)t, "bays 4 rate 1.5");

    ecs_fini(world);
}

void Include_include_forward_ref_to_later_include_inline(void) {
    test_files_install();
    test_file_add("user.flecs",
        "panel { Widget w() }\n"
        "sibling { Ui: {7} }\n");
    test_file_add("lib.flecs",
        "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n");
    test_file_add("parent.flecs",
        "include user.flecs\n"
        "include lib.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_script_run_file(world, "parent.flecs") == 0);

    test_assert(ecs_lookup(world, "Widget") != 0);

    ecs_entity_t w = ecs_lookup(world, "panel.w");
    test_assert(w != 0);

    ecs_entity_t sibling = ecs_lookup(world, "sibling");
    test_assert(sibling != 0);
    const void *ptr = ecs_get_id(world, sibling, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 7);

    ecs_fini(world);
}

void Include_include_forward_ref_to_nested_later_include(void) {
    test_files_install();
    test_file_add("user.flecs",
        "panel { Widget w(n: 5) }\n");
    test_file_add("lib.flecs",
        "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n");
    test_file_add("mid.flecs",
        "include lib.flecs\n");
    test_file_add("parent.flecs",
        "include user.flecs\n"
        "include mid.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    test_assert(ecs_lookup(world, "Widget") != 0);

    ecs_entity_t w = ecs_lookup(world, "panel.w");
    test_assert(w != 0);
    const void *ptr = ecs_get_id(world, w, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 5);

    ecs_fini(world);
}

void Include_include_forward_ref_from_nested_include(void) {
    test_files_install();
    test_file_add("user.flecs",
        "panel { Widget w(n: 5) }\n");
    test_file_add("mid.flecs",
        "include user.flecs\n");
    test_file_add("lib.flecs",
        "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n");
    test_file_add("parent.flecs",
        "include mid.flecs\n"
        "include lib.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    test_assert(ecs_lookup(world, "Widget") != 0);

    ecs_entity_t w = ecs_lookup(world, "panel.w");
    test_assert(w != 0);
    const void *ptr = ecs_get_id(world, w, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 5);

    ecs_fini(world);
}

void Include_include_forward_ref_chain_requires_multiple_passes(void) {
    test_files_install();
    test_file_add("a.flecs",
        "pa { B_t x() }\n");
    test_file_add("b.flecs",
        "template B_t {\n"
        "  Ui: {1}\n"
        "}\n"
        "pb { C_t y() }\n");
    test_file_add("c.flecs",
        "template C_t {\n"
        "  Ui: {2}\n"
        "}\n");
    test_file_add("parent.flecs",
        "include a.flecs\n"
        "include b.flecs\n"
        "include c.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t x = ecs_lookup(world, "pa.x");
    test_assert(x != 0);
    const void *ptr = ecs_get_id(world, x, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 1);

    ecs_entity_t y = ecs_lookup(world, "pb.y");
    test_assert(y != 0);
    ptr = ecs_get_id(world, y, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 2);

    ecs_fini(world);
}

void Include_include_forward_ref_never_resolved_reports_error(void) {
    test_files_install();
    test_file_add("user.flecs",
        "panel { Widget w() }\n");
    test_file_add("other.flecs",
        "other_e {}\n");
    test_file_add("parent.flecs",
        "include user.flecs\n"
        "include other.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);

    ecs_entity_t user_script = ecs_lookup_path_w_sep(
        world, 0, "user.flecs", "/", NULL, false);
    test_assert(user_script != 0);

    const EcsScript *us = ecs_get(world, user_script, EcsScript);
    test_assert(us != NULL);
    test_assert(us->error != NULL);

    test_assert(ecs_lookup(world, "other_e") != 0);
    test_assert(ecs_lookup(world, "Widget") == 0);

    ecs_fini(world);
}

void Include_include_forward_ref_never_resolved_inline_reports_error(void) {
    test_files_install();
    test_file_add("user.flecs",
        "panel { Widget w() }\n");
    test_file_add("other.flecs",
        "other_e {}\n");
    test_file_add("parent.flecs",
        "include user.flecs\n"
        "include other.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);

    test_assert(ecs_lookup(world, "other_e") != 0);
    test_assert(ecs_lookup(world, "Widget") == 0);

    ecs_fini(world);
}

void Include_include_diamond_evaluates_shared_once(void) {
    test_files_install();
    test_file_add("shared.flecs",
        "_ { Ui: {3} }\n");
    test_file_add("a.flecs",
        "include shared.flecs\n");
    test_file_add("b.flecs",
        "include shared.flecs\n");
    test_file_add("parent.flecs",
        "include a.flecs\n"
        "include b.flecs\n");

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    test_int(ecs_count_id(world, ui), 1);

    ecs_fini(world);
}

void Include_include_diamond_forward_ref_is_retried(void) {
    test_files_install();
    test_file_add("shared.flecs",
        "_ { Widget w(n: 4) }\n");
    test_file_add("a.flecs",
        "include shared.flecs\n");
    test_file_add("b.flecs",
        "include shared.flecs\n");
    test_file_add("lib.flecs",
        "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n");
    test_file_add("parent.flecs",
        "include a.flecs\n"
        "include b.flecs\n"
        "include lib.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    test_assert(ecs_lookup(world, "Widget") != 0);
    test_int(ecs_count_id(world, ui), 1);

    ecs_fini(world);
}

void Include_include_forward_ref_to_struct_in_later_include(void) {
    test_files_install();
    test_file_add("use.flecs",
        "e { Position: {x: 10, y: 20} }\n");
    test_file_add("types.flecs",
        "struct Position(x: f32, y: f32)\n");
    test_file_add("parent.flecs",
        "include use.flecs\n"
        "include types.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const float *ptr = ecs_get_id(world, e, pos);
    test_assert(ptr != NULL);
    test_int(ptr[0], 10);
    test_int(ptr[1], 20);

    ecs_fini(world);
}

void Include_include_forward_ref_to_const_in_later_include(void) {
    test_files_install();
    test_file_add("use.flecs",
        "d { Text: {text: \"bays {cfg.maxBays}\"} }\n");
    test_file_add("cfg.flecs",
        "cfg {\n"
        "  export const maxBays: i32 = 4\n"
        "}\n");
    test_file_add("parent.flecs",
        "include use.flecs\n"
        "include cfg.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"text", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t d = ecs_lookup(world, "d");
    test_assert(d != 0);

    const void *t = ecs_get_id(world, d, text);
    test_assert(t != NULL);
    test_str(*(char**)t, "bays 4");

    ecs_fini(world);
}

void Include_include_forward_ref_to_entity_in_later_include(void) {
    test_files_install();
    test_file_add("use.flecs",
        "e { (Likes, Bob) }\n");
    test_file_add("defs.flecs",
        "Likes {}\n"
        "Bob {}\n");
    test_file_add("parent.flecs",
        "include use.flecs\n"
        "include defs.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t likes = ecs_lookup(world, "Likes");
    test_assert(likes != 0);

    ecs_entity_t bob = ecs_lookup(world, "Bob");
    test_assert(bob != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_pair(world, e, likes, bob));

    ecs_fini(world);
}

void Include_include_retried_script_keeps_scope_and_components(void) {
    test_files_install();
    test_file_add("user.flecs",
        "parent_e {\n"
        "  Tag\n"
        "  child_e { Widget w(n: 5) }\n"
        "}\n"
        "sibling_e { Ui: {7} }\n");
    test_file_add("lib.flecs",
        "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n");
    test_file_add("parent.flecs",
        "include user.flecs\n"
        "include lib.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t parent_e = ecs_lookup(world, "parent_e");
    test_assert(parent_e != 0);
    test_assert(ecs_has_id(world, parent_e, tag));

    ecs_entity_t child_e = ecs_lookup(world, "parent_e.child_e");
    test_assert(child_e != 0);
    test_assert(ecs_has_pair(world, child_e, EcsChildOf, parent_e));

    ecs_entity_t w = ecs_lookup(world, "parent_e.child_e.w");
    test_assert(w != 0);
    test_assert(ecs_has_pair(world, w, EcsChildOf, child_e));

    const void *ptr = ecs_get_id(world, w, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 5);

    ecs_entity_t sibling_e = ecs_lookup(world, "sibling_e");
    test_assert(sibling_e != 0);
    ptr = ecs_get_id(world, sibling_e, ui);
    test_assert(ptr != NULL);
    test_int(*(int32_t*)ptr, 7);

    ecs_fini(world);
}

void Include_include_error_reports_failing_file(void) {
    test_files_install();
    test_file_add("good.flecs", "GoodEntity {}\n");
    test_file_add("bad.flecs", "BadEntity {\n  ~~~\n}\n");
    test_file_add("parent.flecs",
        "include good.flecs\n"
        "include bad.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "GoodEntity") != 0);
    test_assert(ecs_lookup(world, "BadEntity") == 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);
    test_assert(strstr(s->error, "bad.flecs") != NULL);
    test_assert(strstr(s->error, "2:") != NULL);

    ecs_fini(world);
}

void Include_include_parse_error_sets_error_on_parent(void) {
    test_files_install();
    test_file_add("bad.flecs", "BadEntity {\n  ~~~\n}\n");
    test_file_add("parent.flecs", "include bad.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });

    if (script) {
        test_assert(ecs_has_id(world, script, EcsScriptError));

        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "bad.flecs") != NULL);
    }

    test_assert(ecs_lookup(world, "BadEntity") == 0);

    ecs_fini(world);
}

void Include_include_parse_error_logs_error(void) {
    test_files_install_ex(false, include_log_error_callback);
    test_file_add("bad.flecs", "BadEntity {\n  ~~~\n}\n");
    test_file_add("parent.flecs", "include bad.flecs\n");

    ecs_log_set_level(-2);

    include_log_error_count = 0;
    include_log_error_level = 0;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_script(world, {
        .filename = "parent.flecs"
    });

    test_assert(include_log_error_count > 0);
    test_int(include_log_error_level, -3);

    ecs_fini(world);
}

void Include_include_eval_error_sets_error_on_parent(void) {
    test_files_install();
    test_file_add("child.flecs", "e {\n Foo: {}\n}\n");
    test_file_add("parent.flecs", "include child.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });

    if (script) {
        test_assert(ecs_has_id(world, script, EcsScriptError));

        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "child.flecs") != NULL);
    }

    ecs_fini(world);
}

void Include_include_nested_parse_error_sets_error_on_parent(void) {
    test_files_install();
    test_file_add("c.flecs", "BadEntity {\n  ~~~\n}\n");
    test_file_add("b.flecs", "include c.flecs\n");
    test_file_add("a.flecs", "include b.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "a.flecs"
    });

    if (script) {
        test_assert(ecs_has_id(world, script, EcsScriptError));

        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "c.flecs") != NULL);
    }

    test_assert(ecs_lookup(world, "BadEntity") == 0);

    ecs_fini(world);
}

void Include_include_parse_error_run_file_fails(void) {
    test_files_install();
    test_file_add("bad.flecs", "BadEntity {\n  ~~~\n}\n");
    test_file_add("parent.flecs", "include bad.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file(world, "parent.flecs") != 0);
    test_assert(ecs_lookup(world, "BadEntity") == 0);

    ecs_fini(world);
}

void Include_include_nested_index_parse_error_sets_error_on_parent(void) {
    test_files_install();
    test_file_add("good1.flecs", "GoodOne {}\n");
    test_file_add("bad.flecs", "BadEntity {\n  ~~~\n}\n");
    test_file_add("good2.flecs", "GoodTwo {}\n");
    test_file_add("index.flecs",
        "include good1.flecs\n"
        "include bad.flecs\n"
        "include good2.flecs\n");
    test_file_add("parent.flecs", "include index.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });

    if (script) {
        test_assert(ecs_has_id(world, script, EcsScriptError));

        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "bad.flecs") != NULL);
    }

    ecs_fini(world);
}

void Include_include_nested_error_reports_position(void) {
    test_files_install();
    test_file_add("bad.flecs",
        "GoodOne {}\n"
        "GoodTwo {}\n"
        "BadEntity {\n"
        "  ~~~\n"
        "}\n");
    test_file_add("other.flecs", "OtherEntity {}\n");
    test_file_add("index.flecs",
        "include other.flecs\n"
        "include bad.flecs\n");
    test_file_add("parent.flecs", "include index.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);
    test_assert(strstr(s->error, "bad.flecs") != NULL);
    test_assert(strstr(s->error, "4:") != NULL);

    ecs_fini(world);
}

static char* include_make_large_script(int32_t lines, const char *last) {
    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int32_t i;
    for (i = 0; i < lines; i ++) {
        ecs_strbuf_appendlit(&buf, "// filler line to grow the script file\n");
    }
    ecs_strbuf_appendstr(&buf, last);
    return ecs_strbuf_get(&buf);
}

void Include_include_error_in_large_file_reports_position(void) {
    test_files_install();

    char *bad = include_make_large_script(2000, "BadEntity {\n  ~~~\n}\n");
    test_file_add("bad.flecs", bad);
    test_file_add("parent.flecs", "include bad.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error != NULL);
    test_assert(strstr(s->error, "bad.flecs") != NULL);
    test_assert(strstr(s->error, "2002:") != NULL);
    test_assert(strstr(s->error, "~~~") != NULL);

    ecs_fini(world);

    ecs_os_free(bad);
}

void Include_include_missing_file_managed_sets_error_on_parent(void) {
    test_files_install();
    test_file_add("parent.flecs", "include does_not_exist.flecs\n");

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, {
        .filename = "parent.flecs"
    });

    if (script) {
        test_assert(ecs_has_id(world, script, EcsScriptError));

        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "does_not_exist.flecs") != NULL);
    }

    ecs_fini(world);
}
