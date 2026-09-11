#include <script.h>

static bool ir_enabled = false;
static ecs_script_eval_desc_t ir_desc = {0};

void AsyncBlock_setup(void) {
    const char *ir_param = test_param("ir");
    ir_enabled = ir_param && !strcmp(ir_param, "enabled");
    ir_desc = (ecs_script_eval_desc_t){ .ir = ir_enabled };
}

static ecs_script_future_t *ab_futures[64];
static int32_t ab_future_count;
static int32_t ab_cancel_count;
static int32_t ab_args[64];
static ecs_entity_t ab_entities[64];
static ecs_entity_t ab_entity_args[64];

static void ab_reset(void) {
    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;
}

static void ab_fetch_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    int32_t index = ab_future_count ++;
    ab_entities[index] = ctx->entity;
    if (argc) {
        ab_args[index] = *(int32_t*)argv[0].ptr;
    }
    ab_futures[index] = future;
}

static void ab_fetch_entity_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    int32_t index = ab_future_count ++;
    ab_entities[index] = ctx->entity;
    if (argc) {
        ab_entity_args[index] = *(ecs_entity_t*)argv[0].ptr;
    }
    ab_futures[index] = future;
}

static void ab_cancel_callback(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)future;
    ab_cancel_count ++;
}

static void ab_resolve(int32_t index, int32_t value) {
    ecs_value_t v = ecs_value(ecs_i32_t, {value});
    test_int(ecs_script_future_resolve(ab_futures[index], &v), 0);
    ecs_script_future_release(ab_futures[index]);
}

static void ab_reject(int32_t index) {
    test_int(ecs_script_future_reject(ab_futures[index], "failed"), 0);
    ecs_script_future_release(ab_futures[index]);
}

static ecs_world_t* ab_world(void) {
    ecs_world_t *world = ecs_init();

    ab_reset();

    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_async_function(world, {
        .name = "fetch_entity",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"e", ecs_id(ecs_entity_t)}},
        .callback = ab_fetch_entity_callback,
        .cancel = ab_cancel_callback
    });

    return world;
}

static ecs_entity_t ab_script(ecs_world_t *world, const char *code) {
    ecs_entity_t s = ecs_script(world, {
        .code = code,
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);
    return s;
}

static int32_t ab_mut_i32(
    ecs_world_t *world,
    ecs_entity_t instance,
    const char *template_name,
    const char *member)
{
    ecs_entity_t t = ecs_lookup(world, template_name);
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, instance, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, member), 0);
    return (int32_t)ecs_meta_get_int(&cur);
}

typedef struct { float x, y; } ab_Position;

static const ab_Position* ab_position(
    ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    return ecs_get_id(world, e, pos);
}

void AsyncBlock_parse_async_block(void) {
    ecs_world_t *world = ab_world();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "async {"
        LINE "  const v = await fetch(1)"
        LINE "  await fetch(v)"
        LINE "}", &ir_desc, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "async") != NULL);
    test_assert(strstr(str, "await") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void AsyncBlock_parse_while(void) {
    ecs_world_t *world = ab_world();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  async {"
        LINE "    while m < 3 {"
        LINE "      m = m + 1"
        LINE "    }"
        LINE "  }"
        LINE "}", &ir_desc, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "while") != NULL);
    test_assert(strstr(str, "assign") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void AsyncBlock_script_block_runs_on_progress(void) {
    ecs_world_t *world = ab_world();

    ecs_entity_t s = ab_script(world,
        HEAD "async {"
        LINE "  const v = await fetch(1)"
        LINE "  await fetch(v + 1)"
        LINE "}");

    test_int(ab_future_count, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);
    test_uint(ab_entities[0], s);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 1);

    ab_resolve(0, 10);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 11);

    ab_resolve(1, 20);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_cancel_count, 0);

    ecs_fini(world);
}

void AsyncBlock_script_two_blocks(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}"
        LINE "async {"
        LINE "  await fetch(2)"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    test_int(ab_args[0], 1);
    test_int(ab_args[1], 2);

    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_no_blocks(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "e { Position: {1, 2} }");

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);

    ecs_fini(world);
}

void AsyncBlock_script_block_no_await(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "async {"
        LINE "  const v = 1"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_block_captures_const(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "const x = 5"
        LINE "const y = x * 2"
        LINE "async {"
        LINE "  await fetch(x + y)"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 15);

    ecs_fini(world);
}

void AsyncBlock_script_block_for_loop(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "async {"
        LINE "  for i in 0..3 {"
        LINE "    await fetch(i)"
        LINE "  }"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 0);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 1);
    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 2);
    ab_resolve(2, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_block_if(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "const x = 5"
        LINE "async {"
        LINE "  if x > 3 {"
        LINE "    await fetch(1)"
        LINE "  } else {"
        LINE "    await fetch(2)"
        LINE "  }"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ecs_fini(world);
}

void AsyncBlock_script_block_try_catch(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "async {"
        LINE "  try {"
        LINE "    await fetch(1)"
        LINE "  } catch {"
        LINE "    await fetch(2)"
        LINE "  }"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);
    ab_reject(0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 2);

    ecs_fini(world);
}

void AsyncBlock_script_update_cancels_block(void) {
    ecs_world_t *world = ab_world();

    ecs_entity_t s = ab_script(world,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    test_int(ecs_script_update(world, s, 0,
        HEAD "async {"
        LINE "  await fetch(2)"
        LINE "}"), 0);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 2);

    ecs_fini(world);
}

void AsyncBlock_script_delete_cancels_block(void) {
    ecs_world_t *world = ab_world();

    ecs_entity_t s = ab_script(world,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    ecs_delete(world, s);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_unmanaged_script_eval(void) {
    ecs_world_t *world = ab_world();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}", &ir_desc, NULL);
    test_assert(script != NULL);
    test_int(ecs_script_eval(script, &ir_desc, NULL), 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_uint(ab_entities[0], 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    ecs_script_free(script);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_run_frees_block(void) {
    ecs_world_t *world = ab_world();

    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}", &ir_desc, NULL), 0);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);

    ecs_fini(world);
}

void AsyncBlock_script_progress_from_pipeline(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}");

    test_int(ab_future_count, 0);
    ecs_progress(world, 0);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ecs_progress(world, 0);
    test_int(ab_future_count, 1);

    ab_resolve(0, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);
    test_int(ab_future_count, 1);

    ecs_fini(world);
}

void AsyncBlock_template_block_assign_mut(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  mut m = 0"
        LINE "  child { Position: {m, 0} }"
        LINE "  async {"
        LINE "    const r = await fetch(v)"
        LINE "    m = r"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_int(ab_mut_i32(world, e, "T", "m"), 0);
    test_int(ab_position(world, child)->x, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 5);
    test_uint(ab_entities[0], e);

    ab_resolve(0, 7);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 7);
    test_int(ab_position(world, child)->x, 7);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_this(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch_entity(this)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_uint(ab_entity_args[0], e);
    test_uint(ab_entities[0], e);

    ecs_fini(world);
}

void AsyncBlock_template_block_reads_component_via_this(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(this[T].v * 10)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 50);

    ecs_fini(world);
}

void AsyncBlock_template_block_captures_consts(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "const g = 100"
        LINE "template T {"
        LINE "  prop v = 1"
        LINE "  mut m = 3"
        LINE "  const w = v * 2"
        LINE "  async {"
        LINE "    await fetch(g + w + m)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 113);

    ecs_fini(world);
}

void AsyncBlock_template_two_blocks(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "  async {"
        LINE "    await fetch(v * 2)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    test_int(ab_args[0], 5);
    test_int(ab_args[1], 10);

    ecs_fini(world);
}

void AsyncBlock_template_two_instances(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  mut m = 0"
        LINE "  child { Position: {m, 0} }"
        LINE "  async {"
        LINE "    const r = await fetch(v)"
        LINE "    m = r"
        LINE "  }"
        LINE "}"
        LINE "a { T: {v: 1} }"
        LINE "b { T: {v: 2} }");

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    test_assert(a != 0);
    test_assert(b != 0);

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);

    int32_t ia = ab_entities[0] == a ? 0 : 1;
    int32_t ib = 1 - ia;
    test_uint(ab_entities[ia], a);
    test_uint(ab_entities[ib], b);
    test_int(ab_args[ia], 1);
    test_int(ab_args[ib], 2);

    ab_resolve(ib, 20);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, a, "T", "m"), 0);
    test_int(ab_mut_i32(world, b, "T", "m"), 20);
    test_int(ab_position(world, ecs_lookup(world, "b.child"))->x, 20);

    ab_resolve(ia, 10);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, a, "T", "m"), 10);
    test_int(ab_position(world, ecs_lookup(world, "a.child"))->x, 10);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_no_block(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  child { Position: {v, 0} }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);

    ecs_fini(world);
}

void AsyncBlock_template_instance_delete_cancels(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    ecs_delete(world, e);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_remove_cancels(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(e != 0);
    test_assert(t != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    ecs_remove_id(world, e, t);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_script_update_cancels(void) {
    ecs_world_t *world = ab_world();

    ecs_entity_t s = ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    test_int(ecs_script_update(world, s, 0,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v + 1)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }"), 0);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 6);

    ecs_fini(world);
}

void AsyncBlock_template_update_restarts_block(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(e != 0);
    test_assert(t != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ecs_script_future_release(ab_futures[0]);

    test_int(ab_cancel_count, 0);
    test_int(ecs_script_template_update(world, e, t), 0);
    test_int(ab_cancel_count, 1);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 5);

    ecs_fini(world);
}

void AsyncBlock_template_prop_change_keeps_block(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child { Position: {v, 0} }"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(t != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    int32_t v = 9;
    ecs_set_id(world, e, t, sizeof(int32_t), &v);
    test_int(ab_position(world, child)->x, 9);

    test_int(ab_cancel_count, 0);
    test_int(ab_future_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_while(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  child { Position: {m, 0} }"
        LINE "  async {"
        LINE "    while m < 3 {"
        LINE "      const r = await fetch(m)"
        LINE "      m = r"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 0);

    ab_resolve(0, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 1);
    test_int(ab_position(world, child)->x, 1);

    ab_resolve(1, 2);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 2);
    test_int(ab_mut_i32(world, e, "T", "m"), 2);

    ab_resolve(2, 3);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ab_mut_i32(world, e, "T", "m"), 3);
    test_int(ab_position(world, child)->x, 3);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_while_no_await(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  async {"
        LINE "    while m < 5 {"
        LINE "      m = m + 1"
        LINE "    }"
        LINE "    await fetch(m)"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 5);
    test_int(ab_mut_i32(world, e, "T", "m"), 5);

    ecs_fini(world);
}

void AsyncBlock_template_block_while_continue(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  async {"
        LINE "    while m < 3 {"
        LINE "      m = m + 1"
        LINE "      if m == 2 {"
        LINE "        continue"
        LINE "      }"
        LINE "      await fetch(m)"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 3);

    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_mut_i32(world, e, "T", "m"), 3);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_while_false(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m = 10"
        LINE "  async {"
        LINE "    while m < 3 {"
        LINE "      await fetch(m)"
        LINE "    }"
        LINE "    await fetch(m + 1)"
        LINE "  }"
        LINE "}"
        LINE "T e");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 11);

    ecs_fini(world);
}

void AsyncBlock_template_block_try_catch_assign(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  child { Position: {m, 0} }"
        LINE "  async {"
        LINE "    try {"
        LINE "      const r = await fetch(1)"
        LINE "      m = r"
        LINE "    } catch {"
        LINE "      m = -1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    ab_reject(0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), -1);
    test_int(ab_position(world, child)->x, -1);

    ecs_fini(world);
}

void AsyncBlock_template_block_assign_updates_local(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m = 1"
        LINE "  async {"
        LINE "    m = m + 1"
        LINE "    m = m + 1"
        LINE "    await fetch(m)"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 3);
    test_int(ab_mut_i32(world, e, "T", "m"), 3);

    ecs_fini(world);
}

void AsyncBlock_template_instance_reactive_after_block_done(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  mut m = 0"
        LINE "  child { Position: {v, m} }"
        LINE "  async {"
        LINE "    const r = await fetch(v)"
        LINE "    m = r"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(t != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    ab_resolve(0, 7);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_position(world, child)->x, 5);
    test_int(ab_position(world, child)->y, 7);
    test_int(ecs_script_tasks_progress(world), 0);

    int32_t v = 9;
    ecs_set_id(world, e, t, sizeof(int32_t), &v);
    test_int(ab_position(world, child)->x, 9);
    test_int(ab_position(world, child)->y, 7);
    test_int(ab_future_count, 1);
    test_int(ab_cancel_count, 0);

    ecs_fini(world);
}

void AsyncBlock_template_nested_in_template_instance(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template Inner {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "template Outer {"
        LINE "  prop w = 1"
        LINE "  Inner child(v: w * 2)"
        LINE "}"
        LINE "e { Outer: {w: 5} }");

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 10);
    test_uint(ab_entities[0], child);

    ecs_fini(world);
}

void AsyncBlock_template_block_error_frees_task(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v = 1"
        LINE "  async {"
        LINE "    await fetch(v)"
        LINE "    await fetch(v)"
        LINE "  }"
        LINE "}"
        LINE "e { T: {v: 5} }");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    ecs_log_set_level(-4);
    ab_reject(0);
    test_int(ecs_script_tasks_progress(world), 1);
    ecs_log_set_level(-1);

    test_int(ab_future_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_error_while_outside_async(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  while m < 3 {"
        LINE "    child {}"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_while_in_script_root(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const x = 1"
        LINE "while x < 3 {"
        LINE "  child {}"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_outside_async(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  m = 5"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_to_prop(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  prop v = 0"
        LINE "  async {"
        LINE "    v = 5"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_to_const(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  const c = 0"
        LINE "  async {"
        LINE "    c = 5"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_in_script_block(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const c = 0"
        LINE "async {"
        LINE "  c = 5"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_unknown_var(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  async {"
        LINE "    unknown = 5"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_type_mismatch(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  async {"
        LINE "    m = \"hello\""
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_nested_async(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  async {"
        LINE "    await fetch(1)"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_async_in_entity_scope(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "e {"
        LINE "  async {"
        LINE "    await fetch(1)"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_async_in_if(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const x = 1"
        LINE "if x > 0 {"
        LINE "  async {"
        LINE "    await fetch(1)"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_async_in_template_child(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  child {"
        LINE "    async {"
        LINE "      await fetch(1)"
        LINE "    }"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_entity_in_async(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  e { Position: {1, 2} }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_component_in_async(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  async {"
        LINE "    Position: {1, 2}"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_mut_in_async(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  async {"
        LINE "    mut m = 1"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_parse_async_missing_scope(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_parse(world, NULL,
        "async\n", &ir_desc, NULL) == NULL);

    ecs_fini(world);
}

void AsyncBlock_error_parse_while_missing_scope(void) {
    ecs_world_t *world = ab_world();

    ecs_log_set_level(-4);
    test_assert(ecs_script_parse(world, NULL,
        "async {\n  while x\n}\n", &ir_desc, NULL) == NULL);

    ecs_fini(world);
}
