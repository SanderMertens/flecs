#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void Include_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

typedef struct test_file_t {
    const char *name;
    const char *content;
    size_t pos;
} test_file_t;

#define TEST_FILE_MAX (64)

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

void Include_include_cycle(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "a.flecs", "include b.flecs\nFoo{}\n", 0 };
    test_files[1] = (test_file_t){ "b.flecs", "include a.flecs\nBar{}\n", 0 };

    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_file_w_desc(world, "a.flecs", &ir_desc) != 0);
    ecs_log_set_level(-1);

    ecs_fini(world);
}

void Include_include_simple(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "Foo{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);

    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_fini(world);
}

void Include_include_subdir(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "sub/child.flecs", "Bar{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include sub/child.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "Bar") != 0);

    ecs_fini(world);
}

void Include_include_missing_file(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "parent.flecs", "include does_not_exist.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_parent_dir_not_allowed(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "parent.flecs", "include ../other.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_relative_to_current_script(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "sub/b.flecs", "B_entity{}\n", 0 };
    test_files[1] = (test_file_t){ "sub/a.flecs", "include b.flecs\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include sub/a.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "B_entity") != 0);

    ecs_fini(world);
}

void Include_include_nested(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "c.flecs", "C_entity{}\n", 0 };
    test_files[1] = (test_file_t){ "b.flecs", "include c.flecs\nB_entity{}\n", 0 };
    test_files[2] = (test_file_t){ "a.flecs", "include b.flecs\nA_entity{}\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "a.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "A_entity") != 0);
    test_assert(ecs_lookup(world, "B_entity") != 0);
    test_assert(ecs_lookup(world, "C_entity") != 0);

    ecs_fini(world);
}

void Include_include_managed_creates_script_entity(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "ChildEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "ChildEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "b.flecs", "BEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "a.flecs", "include b.flecs\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "ChildInline{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);

    test_assert(ecs_lookup(world, "ChildInline") != 0);

    ecs_entity_t child_script = ecs_lookup_path_w_sep(
        world, 0, "child.flecs", "/", NULL, false);
    if (child_script) {
        test_assert(!ecs_has(world, child_script, EcsScript));
    }

    ecs_fini(world);
}

void Include_include_rejects_absolute_unix_path(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "parent.flecs", "include /etc/passwd\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_rejects_windows_drive_letter(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "parent.flecs", "include C:/foo.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_with_line_comment(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "LineCommentEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs // load the child\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "LineCommentEntity") != 0);

    ecs_fini(world);
}

void Include_include_with_block_comment(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "BlockCommentEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs /* load child */\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "BlockCommentEntity") != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_template(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "child.flecs", "TemplateChild{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "template MyTemplate {\n"
        "  include child.flecs\n"
        "}\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_entity_scope(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "child.flecs", "Child{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "Parent {\n"
        "  include child.flecs\n"
        "}\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_with_scope(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "child.flecs", "Child{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "Tag {}\n"
        "with Tag {\n"
        "  include child.flecs\n"
        "}\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_if_scope(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "child.flecs", "Child{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "if true {\n"
        "  include child.flecs\n"
        "}\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_not_allowed_in_for_scope(void) {
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
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);

    ecs_log_set_level(-5);

    test_files[0] = (test_file_t){ "child.flecs", "Child{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "for i in 0 .. 1 {\n"
        "  include child.flecs\n"
        "}\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_fini(world);
}

void Include_include_auto_appends_extension(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "AutoExtEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "AutoExtEntity") != 0);

    ecs_fini(world);
}

void Include_include_auto_appends_extension_subdir(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "sub/child.flecs", "SubAutoExt{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include sub/child\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "SubAutoExt") != 0);

    ecs_fini(world);
}

void Include_include_keeps_explicit_extension(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "ExplicitExtEntity{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);
    test_assert(ecs_lookup(world, "ExplicitExtEntity") != 0);

    ecs_fini(world);
}

void Include_include_auto_appends_extension_managed(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "ManagedAutoExt{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "actual.flecs", "RemappedEntity{}\n", 0 };
    test_fopen_remap_from = "requested.flecs";
    test_fopen_remap_to = "actual.flecs";

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "requested.flecs", &ir_desc) == 0);
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
    os_api.log_ = include_log_error_callback;
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "e {\n Foo: {}\n}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_log_set_level(-2);

    include_log_error_count = 0;
    include_log_error_level = 0;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(include_log_error_count > 0);
    test_int(include_log_error_level, -3);

    ecs_fini(world);
}

void Include_include_managed_eval_error_set_on_script(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "e {\n Foo: {}\n}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "using Foo\n"
        "child_e { Bar }\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n"
        "parent_e { Bar }\n", 0 };

    ecs_world_t *world = ecs_init();

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar", .parent = foo });

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    ecs_entity_t child_e = ecs_lookup(world, "child_e");
    test_assert(child_e != 0);
    test_assert(ecs_has_id(world, child_e, bar));

    ecs_entity_t parent_e = ecs_lookup(world, "parent_e");
    test_assert(parent_e == 0 || !ecs_has_id(world, parent_e, bar));

    ecs_fini(world);
}

void Include_include_managed_using_not_visible_in_parent(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "using Foo\n"
        "child_e { Bar }\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n"
        "parent_e { Bar }\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t foo = ecs_entity(world, { .name = "Foo" });
    ecs_entity_t bar = ecs_entity(world, { .name = "Bar", .parent = foo });

    ecs_log_set_level(-4);
    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "Child{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n"
        "struct Position(x: f32)\n"
        "e { Position: {10} }\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "Child{}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n"
        "struct Position(x: f32)\n"
        "e { Position: {10} }\n", 0 };

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    test_assert(ecs_has_id(world, e, pos));

    ecs_fini(world);
}

void Include_include_forward_ref_to_later_include_is_retried(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "panel { Widget w() }\n"
        "sibling { Ui: {7} }\n", 0 };
    test_files[1] = (test_file_t){ "lib.flecs", "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include user.flecs\n"
        "include lib.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    test_assert(ecs_lookup(world, "Widget") != 0);
    test_assert(ecs_lookup(world, "panel.w") != 0);
    test_assert(ecs_lookup(world, "sibling") != 0);

    ecs_fini(world);
}

void Include_const_does_not_cross_include(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "palette.flecs", "const Amber: Ui = {240}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include palette.flecs\n"
        "a { Ui: Amber }\n"
        "b { Ui: {1} }\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    test_assert(ecs_lookup(world, "a") == 0);
    test_assert(ecs_lookup(world, "b") == 0);

    ecs_fini(world);
}

void Include_export_const_in_scope_crosses_include(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "cfg.flecs", "cfg {\n"
        "  export const maxBays: i32 = 4\n"
        "  export const rate: f32 = 1.5\n"
        "}\n", 0 };
    test_files[1] = (test_file_t){ "use.flecs", "d { Text: {text: \"bays {cfg.maxBays} rate {cfg.rate:.1}\"} }\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include cfg.flecs\n"
        "include use.flecs\n", 0 };

    ecs_world_t *world = ecs_init();

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"text", ecs_id(ecs_string_t)}
        }
    });

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);

    ecs_entity_t d = ecs_lookup(world, "d");
    test_assert(d != 0);
    const void *t = ecs_get_id(world, d, text);
    test_assert(t != NULL);
    test_str(*(char**)t, "bays 4 rate 1.5");

    ecs_fini(world);
}

void Include_include_forward_ref_to_later_include_inline(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "panel { Widget w() }\n"
        "sibling { Ui: {7} }\n", 0 };
    test_files[1] = (test_file_t){ "lib.flecs", "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include user.flecs\n"
        "include lib.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) == 0);

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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "panel { Widget w(n: 5) }\n", 0 };
    test_files[1] = (test_file_t){ "lib.flecs", "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n", 0 };
    test_files[2] = (test_file_t){ "mid.flecs", "include lib.flecs\n", 0 };
    test_files[3] = (test_file_t){ "parent.flecs", "include user.flecs\n"
        "include mid.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "panel { Widget w(n: 5) }\n", 0 };
    test_files[1] = (test_file_t){ "mid.flecs", "include user.flecs\n", 0 };
    test_files[2] = (test_file_t){ "lib.flecs", "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n", 0 };
    test_files[3] = (test_file_t){ "parent.flecs", "include mid.flecs\n"
        "include lib.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "a.flecs", "pa { B_t x() }\n", 0 };
    test_files[1] = (test_file_t){ "b.flecs", "template B_t {\n"
        "  Ui: {1}\n"
        "}\n"
        "pb { C_t y() }\n", 0 };
    test_files[2] = (test_file_t){ "c.flecs", "template C_t {\n"
        "  Ui: {2}\n"
        "}\n", 0 };
    test_files[3] = (test_file_t){ "parent.flecs", "include a.flecs\n"
        "include b.flecs\n"
        "include c.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "panel { Widget w() }\n", 0 };
    test_files[1] = (test_file_t){ "other.flecs", "other_e {}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include user.flecs\n"
        "include other.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "panel { Widget w() }\n", 0 };
    test_files[1] = (test_file_t){ "other.flecs", "other_e {}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include user.flecs\n"
        "include other.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);

    test_assert(ecs_lookup(world, "other_e") != 0);
    test_assert(ecs_lookup(world, "Widget") == 0);

    ecs_fini(world);
}

void Include_include_diamond_evaluates_shared_once(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "shared.flecs", "_ { Ui: {3} }\n", 0 };
    test_files[1] = (test_file_t){ "a.flecs", "include shared.flecs\n", 0 };
    test_files[2] = (test_file_t){ "b.flecs", "include shared.flecs\n", 0 };
    test_files[3] = (test_file_t){ "parent.flecs", "include a.flecs\n"
        "include b.flecs\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "shared.flecs", "_ { Widget w(n: 4) }\n", 0 };
    test_files[1] = (test_file_t){ "a.flecs", "include shared.flecs\n", 0 };
    test_files[2] = (test_file_t){ "b.flecs", "include shared.flecs\n", 0 };
    test_files[3] = (test_file_t){ "lib.flecs", "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n", 0 };
    test_files[4] = (test_file_t){ "parent.flecs", "include a.flecs\n"
        "include b.flecs\n"
        "include lib.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t ui = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Ui" }),
        .members = {
            {"i", ecs_id(ecs_i32_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "use.flecs", "e { Position: {x: 10, y: 20} }\n", 0 };
    test_files[1] = (test_file_t){ "types.flecs", "struct Position(x: f32, y: f32)\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include use.flecs\n"
        "include types.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "use.flecs", "d { Text: {text: \"bays {cfg.maxBays}\"} }\n", 0 };
    test_files[1] = (test_file_t){ "cfg.flecs", "cfg {\n"
        "  export const maxBays: i32 = 4\n"
        "}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include use.flecs\n"
        "include cfg.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t text = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Text" }),
        .members = {
            {"text", ecs_id(ecs_string_t)}
        }
    });

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "use.flecs", "e { (Likes, Bob) }\n", 0 };
    test_files[1] = (test_file_t){ "defs.flecs", "Likes {}\n"
        "Bob {}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include use.flecs\n"
        "include defs.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "user.flecs", "parent_e {\n"
        "  Tag\n"
        "  child_e { Widget w(n: 5) }\n"
        "}\n"
        "sibling_e { Ui: {7} }\n", 0 };
    test_files[1] = (test_file_t){ "lib.flecs", "template Widget {\n"
        "  prop n: i32 = 0\n"
        "  Ui: {n}\n"
        "}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include user.flecs\n"
        "include lib.flecs\n", 0 };

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

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "good.flecs", "GoodEntity {}\n", 0 };
    test_files[1] = (test_file_t){ "bad.flecs", "BadEntity {\n  ~~~\n}\n", 0 };
    test_files[2] = (test_file_t){ "parent.flecs", "include good.flecs\n"
        "include bad.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "bad.flecs", "BadEntity {\n  ~~~\n}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include bad.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });

    if (script) {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "bad.flecs") != NULL);
    }

    test_assert(ecs_lookup(world, "BadEntity") == 0);

    ecs_fini(world);
}

void Include_include_parse_error_logs_error(void) {
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
    os_api.log_ = include_log_error_callback;
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "bad.flecs", "BadEntity {\n  ~~~\n}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include bad.flecs\n", 0 };

    ecs_log_set_level(-2);

    include_log_error_count = 0;
    include_log_error_level = 0;

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });

    test_assert(include_log_error_count > 0);
    test_int(include_log_error_level, -3);

    ecs_fini(world);
}

void Include_include_eval_error_sets_error_on_parent(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "child.flecs", "e {\n Foo: {}\n}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include child.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });

    if (script) {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "child.flecs") != NULL);
    }

    ecs_fini(world);
}

void Include_include_nested_parse_error_sets_error_on_parent(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "c.flecs", "BadEntity {\n  ~~~\n}\n", 0 };
    test_files[1] = (test_file_t){ "b.flecs", "include c.flecs\n", 0 };
    test_files[2] = (test_file_t){ "a.flecs", "include b.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "a.flecs"
    });

    if (script) {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "c.flecs") != NULL);
    }

    test_assert(ecs_lookup(world, "BadEntity") == 0);

    ecs_fini(world);
}

void Include_include_parse_error_run_file_fails(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "bad.flecs", "BadEntity {\n  ~~~\n}\n", 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include bad.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_file_w_desc(world, "parent.flecs", &ir_desc) != 0);
    test_assert(ecs_lookup(world, "BadEntity") == 0);

    ecs_fini(world);
}

void Include_include_nested_index_parse_error_sets_error_on_parent(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "good1.flecs", "GoodOne {}\n", 0 };
    test_files[1] = (test_file_t){ "bad.flecs", "BadEntity {\n  ~~~\n}\n", 0 };
    test_files[2] = (test_file_t){ "good2.flecs", "GoodTwo {}\n", 0 };
    test_files[3] = (test_file_t){ "index.flecs", "include good1.flecs\n"
        "include bad.flecs\n"
        "include good2.flecs\n", 0 };
    test_files[4] = (test_file_t){ "parent.flecs", "include index.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });

    if (script) {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "bad.flecs") != NULL);
    }

    ecs_fini(world);
}

void Include_include_nested_error_reports_position(void) {
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "bad.flecs", "GoodOne {}\n"
        "GoodTwo {}\n"
        "BadEntity {\n"
        "  ~~~\n"
        "}\n", 0 };
    test_files[1] = (test_file_t){ "other.flecs", "OtherEntity {}\n", 0 };
    test_files[2] = (test_file_t){ "index.flecs", "include other.flecs\n"
        "include bad.flecs\n", 0 };
    test_files[3] = (test_file_t){ "parent.flecs", "include index.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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

void Include_include_error_in_large_file_reports_position(void) {
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
    ecs_os_set_api(&os_api);

    ecs_strbuf_t buf = ECS_STRBUF_INIT;
    int32_t i;
    for (i = 0; i < 2000; i ++) {
        ecs_strbuf_appendlit(&buf, "// filler line to grow the script file\n");
    }
    ecs_strbuf_appendstr(&buf, "BadEntity {\n  ~~~\n}\n");
    char *bad = ecs_strbuf_get(&buf);

    test_files[0] = (test_file_t){ "bad.flecs", bad, 0 };
    test_files[1] = (test_file_t){ "parent.flecs", "include bad.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
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
    ecs_os_set_api(&os_api);

    test_files[0] = (test_file_t){ "parent.flecs", "include does_not_exist.flecs\n", 0 };

    ecs_log_set_level(-4);

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });

    if (script) {
        const EcsScript *s = ecs_get(world, script, EcsScript);
        test_assert(s != NULL);
        test_assert(s->error != NULL);
        test_assert(strstr(s->error, "does_not_exist.flecs") != NULL);
    }

    ecs_fini(world);
}

void Include_include_nested_manifest_w_cross_file_refs(void) {
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
    ecs_os_set_api(&os_api);

    char *names[6];
    char *contents[6];
    ecs_strbuf_t manifest = ECS_STRBUF_INIT;
    int32_t i;

    ecs_strbuf_appendlit(&manifest, "include cfg.flecs\n");

    for (i = 0; i < 6; i ++) {
        char buf[64];
        ecs_os_snprintf(buf, 64, "city/f%d.flecs", i);
        names[i] = ecs_os_strdup(buf);

        ecs_strbuf_t file = ECS_STRBUF_INIT;
        ecs_strbuf_append(&file,
            "grp%d {\n"
            "  export const v%d: f32 = cfg.pitch * %d\n"
            "}\n",
            i, i, i);

        if (i + 1 < 6) {
            ecs_strbuf_append(&file,
                "const local%d: f32 = grp%d.v%d + cfg.half\n",
                i, i + 1, i + 1);
        } else {
            ecs_strbuf_append(&file,
                "const local%d: f32 = cfg.half\n", i);
        }

        ecs_strbuf_append(&file,
            "template T%d {\n"
            "  prop x: f32 = 0\n"
            "  Position: {x: $x + local%d, y: cfg.half + grp%d.v%d}\n"
            "}\n",
            i, i, i, i);

        contents[i] = ecs_strbuf_get(&file);
        test_files[i] = (test_file_t){ names[i], contents[i], 0 };

        ecs_strbuf_append(&manifest, "include f%d.flecs\n", i);
    }

    ecs_strbuf_appendlit(&manifest, "include agg.flecs\n");

    ecs_strbuf_t agg = ECS_STRBUF_INIT;
    ecs_strbuf_appendlit(&agg,
        "template Agg {\n"
        "  prop x: f32 = 0\n");
    for (i = 0; i < 4; i ++) {
        ecs_strbuf_append(&agg, "  child%d { T%d: {x: $x} }\n", i, i);
    }
    ecs_strbuf_appendlit(&agg, "}\n");
    char *agg_str = ecs_strbuf_get(&agg);
    char *manifest_str = ecs_strbuf_get(&manifest);

    test_files[6] = (test_file_t){ "city/cfg.flecs", "using flecs.meta\n"
        "struct Position(x: f32, y: f32)\n"
        "cfg {\n"
        "  export const half: f32 = 60\n"
        "  export const pitch: f32 = 120\n"
        "}\n", 0 };
    test_files[7] = (test_file_t){ "city/agg.flecs", agg_str, 0 };
    test_files[8] = (test_file_t){ "city/city.flecs", manifest_str, 0 };
    test_files[9] = (test_file_t){ "parent.flecs", "include city/city.flecs\n"
        "e { Agg: {x: 1} }\n", 0 };

    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScript);

    ecs_entity_t script = ecs_script(world, { .ir = ir_enabled,
        .filename = "parent.flecs"
    });
    test_assert(script != 0);

    const EcsScript *s = ecs_get(world, script, EcsScript);
    test_assert(s != NULL);
    test_assert(s->error == NULL);

    ecs_entity_t templates[6];
    for (i = 0; i < 6; i ++) {
        char buf[64];
        ecs_os_snprintf(buf, 64, "T%d", i);
        templates[i] = ecs_lookup(world, buf);
        test_assert(templates[i] != 0);
        test_assert(ecs_is_alive(world, templates[i]));
        test_assert(ecs_has(world, templates[i], EcsScript));
    }

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    for (i = 0; i < 4; i ++) {
        char buf[64];
        ecs_os_snprintf(buf, 64, "e.child%d", i);
        ecs_entity_t child = ecs_lookup(world, buf);
        test_assert(child != 0);
        test_assert(ecs_has_id(world, child, templates[i]));
    }

    ecs_fini(world);

    for (i = 0; i < 6; i ++) {
        ecs_os_free(names[i]);
        ecs_os_free(contents[i]);
    }
    ecs_os_free(agg_str);
    ecs_os_free(manifest_str);
}
