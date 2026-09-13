#include <script.h>

static ecs_script_eval_desc_t ir_desc;

void TemplateParent_setup(void) {
    const char *param = test_param("ir");
    ir_desc = (ecs_script_eval_desc_t){
        .ir = param && !strcmp(param, "enabled")
    };
}

static int32_t value(ecs_world_t *world, const char *path) {
    ecs_entity_t e = ecs_lookup(world, path);
    test_assert(e != 0);
    const int32_t *ptr = ecs_get(world, e, ecs_i32_t);
    test_assert(ptr != NULL);
    return *ptr;
}

void TemplateParent_props(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  flecs.meta.i32: {height}"
        LINE "}"
        LINE "Building b() { Facade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 10);
    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_set_id(world, b, building, sizeof(int32_t), &(int32_t){20});
    test_int(value(world, "b.f"), 20);
    ecs_fini(world);
}

void TemplateParent_muts(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { mut height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building { flecs.meta.i32: {height} }"
        LINE "Building b() { Facade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 10);
    ecs_entity_t mut = ecs_lookup(world, "Building.mut");
    ecs_set_id(world, ecs_lookup(world, "b"), mut,
        sizeof(int32_t), &(int32_t){20});
    test_int(value(world, "b.f"), 20);
    ecs_fini(world);
}

void TemplateParent_mut_from_prop(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template RadioGroup {"
        LINE "  prop default = \"Oak\""
        LINE "  mut active: string = default"
        LINE "}"
        LINE "template RadioButton : parent RadioGroup {"
        LINE "  prop label = \"\""
        LINE "  flecs.meta.i32: {active == label}"
        LINE "}"
        LINE "RadioGroup options(\"Maple\") {"
        LINE "  RadioButton oak(\"Oak\")"
        LINE "  RadioButton maple(\"Maple\")"
        LINE "}",
        &ir_desc, NULL), 0);

    ecs_entity_t group = ecs_lookup(world, "RadioGroup");
    ecs_entity_t options = ecs_lookup(world, "options");
    test_assert(group != 0);
    test_assert(options != 0);
    const ecs_string_t *default_value = ecs_get_id(world, options, group);
    test_assert(default_value != NULL);
    test_str(*default_value, "Maple");
    test_int(value(world, "options.oak"), 0);
    test_int(value(world, "options.maple"), 1);
    ecs_fini(world);
}

void TemplateParent_masking(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building {"
        LINE "  prop height: i32 = 10"
        LINE "  mut width: i32 = 20"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  prop height: i32 = 1"
        LINE "  const width = 2"
        LINE "  flecs.meta.i32: {height + width + parent.height + parent.width}"
        LINE "}"
        LINE "Building b() { Facade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 33);
    ecs_fini(world);
}

void TemplateParent_inheritance(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade { prop width: i32 = 2"
        LINE "}"
        LINE "template BrickFacade : Facade, parent Building {"
        LINE "  flecs.meta.i32: {height * width}"
        LINE "}"
        LINE "Building b() { BrickFacade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 20);
    ecs_fini(world);
}

void TemplateParent_inherited_constraint(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building { prop width: i32 = 2"
        LINE "}"
        LINE "template BrickFacade : Facade { flecs.meta.i32: {height * width} }"
        LINE "Building b() { BrickFacade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 20);
    ecs_fini(world);
}

void TemplateParent_radio_group(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template RadioGroup { mut active: string = \"a\""
        LINE "}"
        LINE "template RadioButton : parent RadioGroup {"
        LINE "  prop label: string = \"\""
        LINE "  flecs.meta.i32: {active == label}"
        LINE "}"
        LINE "template SelectButton : parent RadioGroup {"
        LINE "  prop label: string = \"\""
        LINE "  async { active = label"
        LINE "}"
        LINE "}"
        LINE "RadioGroup group() {"
        LINE "  RadioButton a(\"a\")"
        LINE "  RadioButton b(\"b\")"
        LINE "  SelectButton select(\"b\")"
        LINE "}",
        &ir_desc, NULL), 0);
    ecs_progress(world, 0);
    test_int(value(world, "group.a"), 0);
    test_int(value(world, "group.b"), 1);
    ecs_fini(world);
}

static void invalid(const char *code) {
    ecs_world_t *world = ecs_init();
    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL, code, &ir_desc, NULL) != 0);
    ecs_fini(world);
}

void TemplateParent_missing_parent(void) {
    invalid(HEAD "template Building {}"
        LINE "template Facade : parent Building {}"
        LINE "Facade f()");
}

void TemplateParent_wrong_parent(void) {
    invalid(HEAD "template Building {}"
        LINE "template Facade : parent Building {}"
        LINE "b { Facade f() }");
}

void TemplateParent_invalid_constraint(void) {
    invalid(HEAD "struct Building(height: i32)"
        LINE "template Facade : parent Building {}");
}

void TemplateParent_duplicate_constraint(void) {
    invalid(HEAD "template Building {}"
        LINE "template Facade : parent Building, parent Building {}");
}

void TemplateParent_empty_base(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {}"
        LINE "template BrickFacade : Facade, parent Building {"
        LINE "  flecs.meta.i32: {height}"
        LINE "}"
        LINE "Building b() { BrickFacade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 10);
    ecs_fini(world);
}

void TemplateParent_parent_before_base(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "struct Facade(width: i32)"
        LINE "template BrickFacade : parent Building, Facade {"
        LINE "  flecs.meta.i32: {height * width}"
        LINE "}"
        LINE "Building b() { BrickFacade f(3) }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 30);
    ecs_fini(world);
}

void TemplateParent_nested_members(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "struct Size(width: i32)"
        LINE "template Building { prop size: Size = {10}"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  prop size: Size = {2}"
        LINE "  flecs.meta.i32: {size.width + parent.size.width}"
        LINE "}"
        LINE "template Roof : parent Building { flecs.meta.i32: {size.width} }"
        LINE "Building b() { Facade f(); Roof r() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 12);
    test_int(value(world, "b.r"), 10);
    ecs_fini(world);
}

void TemplateParent_multiple_instances(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building { flecs.meta.i32: {height} }"
        LINE "Building a(1) { Facade f() }"
        LINE "Building b(2) { Facade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "a.f"), 1);
    test_int(value(world, "b.f"), 2);
    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_set_id(world, ecs_lookup(world, "a"), building,
        sizeof(int32_t), &(int32_t){3});
    test_int(value(world, "a.f"), 3);
    test_int(value(world, "b.f"), 2);
    ecs_fini(world);
}

void TemplateParent_qualified_assignment(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { mut height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  mut height: i32 = 1"
        LINE "  flecs.meta.i32: {height + parent.height}"
        LINE "  async {"
        LINE "    parent.height = 20"
        LINE "    height = 2"
        LINE "  }"
        LINE "}"
        LINE "Building b() { Facade f() }",
        &ir_desc, NULL), 0);
    ecs_script_tasks_progress(world);
    test_int(value(world, "b.f"), 22);
    ecs_fini(world);
}

static ecs_script_future_t *pending;
static int32_t fetch_count;

static void fetch(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    pending = future;
    fetch_count ++;
}

void TemplateParent_async_mut_read(void) {
    ecs_world_t *world = ecs_init();
    pending = NULL;
    fetch_count = 0;
    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = fetch
    });
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { mut height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  flecs.meta.i32: {height}"
        LINE "  async {"
        LINE "    await fetch()"
        LINE "    height = height + 1"
        LINE "  }"
        LINE "}"
        LINE "Building b() { Facade f() }",
        &ir_desc, NULL), 0);
    ecs_script_tasks_progress(world);
    test_int(fetch_count, 1);
    ecs_set_id(world, ecs_lookup(world, "b"), ecs_lookup(world, "Building.mut"),
        sizeof(int32_t), &(int32_t){20});
    ecs_value_t result = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(pending, &result), 0);
    ecs_script_future_release(pending);
    ecs_script_tasks_progress(world);
    test_int(fetch_count, 1);
    test_int(value(world, "b.f"), 21);
    ecs_fini(world);
}

void TemplateParent_assign_prop(void) {
    invalid(HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  async {"
        LINE "    height = 20"
        LINE "  }"
        LINE "}");
}

void TemplateParent_assign_masked_const(void) {
    invalid(HEAD "template Building { mut height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  const height = 1"
        LINE "  async {"
        LINE "    height = 20"
        LINE "  }"
        LINE "}");
}

void TemplateParent_conflicting_constraint(void) {
    invalid(HEAD "template Building {}"
        LINE "template Other {}"
        LINE "template Facade : parent Building {}"
        LINE "template BrickFacade : Facade, parent Other {}");
}

void TemplateParent_unknown_constraint(void) {
    invalid(HEAD "template Facade : parent Missing {}");
}

void TemplateParent_parent_is_immediate(void) {
    invalid(HEAD "template Building {}"
        LINE "template Facade : parent Building {}"
        LINE "Building b() { child { Facade f() } }");
}

void TemplateParent_radio_events(void) {
    ecs_world_t *world = ecs_init();
    ECS_IMPORT(world, FlecsScriptEvent);
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "using flecs.script"
        LINE "template RadioGroup {"
        LINE "  mut active: string = \"a\""
        LINE "}"
        LINE "template RadioButton : parent RadioGroup {"
        LINE "  prop label: string = \"\""
        LINE "  flecs.meta.i32: {active == label}"
        LINE "  async {"
        LINE "    while true {"
        LINE "      await on.click(this)"
        LINE "      active = label"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "RadioGroup group() {"
        LINE "  RadioButton a(\"a\"); RadioButton b(\"b\")"
        LINE "}",
        &ir_desc, NULL), 0);
    ecs_script_tasks_progress(world);
    test_int(value(world, "group.a"), 1);
    test_int(value(world, "group.b"), 0);
    for (int32_t i = 0; i < 4; i ++) {
        bool select_b = !(i % 2);
        ecs_entity_t target = ecs_lookup(world, select_b ? "group.b" : "group.a");
        ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
            .target = target, .buttons = 1
        });
        ecs_script_mouse_event(world, &(EcsScriptMouseEvent){
            .target = target, .buttons = 0
        });
        test_int(value(world, "group.a"), !select_b);
        test_int(value(world, "group.b"), select_b);
    }
    ecs_fini(world);
}

void TemplateParent_anonymous_instances(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template RadioGroup {"
        LINE "  mut active: string = \"a\""
        LINE "}"
        LINE "template RadioButton : parent RadioGroup {"
        LINE "  prop label: string = \"\""
        LINE "  flecs.meta.i32: {active == label}"
        LINE "}"
        LINE "RadioGroup() { RadioButton(\"a\"); RadioButton(\"b\") }",
        &ir_desc, NULL), 0);
    ecs_entity_t button = ecs_lookup(world, "RadioButton");
    ecs_iter_t it = ecs_each_id(world, button);
    int32_t count = 0, selected = 0;
    while (ecs_each_next(&it)) {
        for (int32_t i = 0; i < it.count; i ++) {
            selected += *ecs_get(world, it.entities[i], ecs_i32_t);
            count ++;
        }
    }
    test_int(count, 2);
    test_int(selected, 1);
    ecs_fini(world);
}

void TemplateParent_native_instantiation(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building { flecs.meta.i32: {height} }",
        &ir_desc, NULL), 0);
    ecs_entity_t building = ecs_lookup(world, "Building");
    ecs_entity_t facade = ecs_lookup(world, "Facade");
    ecs_defer_begin(world);
    ecs_entity_t b = ecs_entity(world, { .name = "b" });
    ecs_set_id(world, b, building, sizeof(int32_t), &(int32_t){30});
    ecs_entity_t f = ecs_entity(world, { .name = "f", .parent = b });
    ecs_add_id(world, f, facade);
    ecs_defer_end(world);
    test_int(value(world, "b.f"), 30);
    ecs_fini(world);
}

void TemplateParent_non_fragmenting_parent(void) {
    ecs_world_t *world = ecs_init();
    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "template Building { prop height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building { flecs.meta.i32: {height} }"
        LINE "@tree Parent"
        LINE "Building b() { Facade f() }",
        &ir_desc, NULL), 0);
    test_int(value(world, "b.f"), 10);
    ecs_entity_t f = ecs_lookup(world, "b.f");
    test_assert(ecs_has(world, f, EcsParent));
    ecs_set_id(world, ecs_lookup(world, "b"), ecs_lookup(world, "Building"),
        sizeof(int32_t), &(int32_t){20});
    test_int(value(world, "b.f"), 20);
    ecs_fini(world);
}

void TemplateParent_missing_constraint_name(void) {
    invalid(HEAD "template Facade : parent {}");
}

void TemplateParent_unknown_member(void) {
    invalid(HEAD "template Building {}"
        LINE "template Facade : parent Building { flecs.meta.i32: {parent.missing} }");
}

void TemplateParent_assign_unknown_member(void) {
    invalid(HEAD "template Building { mut height: i32 = 10"
        LINE "}"
        LINE "template Facade : parent Building {"
        LINE "  async {"
        LINE "    parent.height.missing = 20"
        LINE "  }"
        LINE "}");
}
