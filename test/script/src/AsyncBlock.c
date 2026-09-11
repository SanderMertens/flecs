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

void AsyncBlock_template_prop_change_restarts_block(void) {
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

    test_int(ab_cancel_count, 1);
    test_int(ab_future_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 9);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);

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

void AsyncBlock_template_child_this(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  child {"
        LINE "    async {"
        LINE "      await fetch_entity(this)"
        LINE "      await fetch_entity(this)"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[0], child);
    test_uint(ab_entities[0], child);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[1], child);
    test_uint(ab_entities[1], child);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_scopes_and_instances(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  async { await fetch_entity(this) }"
        LINE "  left {"
        LINE "    async { await fetch_entity(this) }"
        LINE "    grandchild { async { await fetch_entity(this) } }"
        LINE "  }"
        LINE "  right { async { await fetch_entity(this) } }"
        LINE "}"
        LINE "a { T: {} }"
        LINE "b { T: {} }");

const char *paths[] = {
        "a", "a.left", "a.left.grandchild", "a.right",
        "b", "b.left", "b.left.grandchild", "b.right"
    };
    test_int(ecs_script_tasks_progress(world), 8);
    test_int(ab_future_count, 8);
    for (int32_t i = 0; i < 8; i ++) {
        ecs_entity_t e = ecs_lookup(world, paths[i]);
        test_assert(e != 0);
        test_uint(ab_entity_args[i], e);
        test_uint(ab_entities[i], e);
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_template_child_assign_mut(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop initial: i32 = 3"
        LINE "  mut value: i32 = 0"
        LINE "  const root = this"
        LINE "  child {"
        LINE "    Position: {initial, value}"
        LINE "    const captured = initial * 2"
        LINE "    async {"
        LINE "      const result = await fetch(this[Position].x + captured)"
        LINE "      value = result"
        LINE "      await fetch_entity(root)"
        LINE "      await fetch_entity(this)"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "a { T: {3} }"
        LINE "b { T: {5} }");

ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t child = ecs_lookup(world, "a.child");
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_args[0], 9);
    test_int(ab_args[1], 15);
    ab_resolve(0, 42);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, a, "T", "value"), 42);
    test_int(ab_mut_i32(world, b, "T", "value"), 0);
    test_int(ab_position(world, child)->y, 42);
    test_uint(ab_entity_args[2], a);
    test_uint(ab_entities[2], child);
    ab_resolve(2, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[3], child);
    ecs_script_future_release(ab_futures[1]);
    ecs_script_future_release(ab_futures[3]);

    ecs_fini(world);
}

void AsyncBlock_template_child_delete_cancels(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child {"
        LINE "    Position: {v, 0}"
        LINE "    async { await fetch_entity(this) }"
        LINE "    async { await fetch(v) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(e && child && t);
    test_int(ecs_script_tasks_progress(world), 2);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    ecs_delete(world, child);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_instance_delete_cancels(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child {"
        LINE "    Position: {v, 0}"
        LINE "    async { await fetch_entity(this) }"
        LINE "    async { await fetch(v) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(e && child && t);
    test_int(ecs_script_tasks_progress(world), 2);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    ecs_delete(world, e);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_remove_cancels(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child {"
        LINE "    Position: {v, 0}"
        LINE "    async { await fetch_entity(this) }"
        LINE "    async { await fetch(v) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(e && child && t);
    test_int(ecs_script_tasks_progress(world), 2);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    ecs_remove_id(world, e, t);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_update_cancels(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child {"
        LINE "    Position: {v, 0}"
        LINE "    async { await fetch_entity(this) }"
        LINE "    async { await fetch(v) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(e && child && t);
    test_int(ecs_script_tasks_progress(world), 2);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    test_int(ecs_script_template_update(world, e, t), 0);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 4);
    test_uint(ab_entity_args[2], ecs_lookup(world, "e.child"));
    ecs_script_future_release(ab_futures[2]);
    ecs_script_future_release(ab_futures[3]);

    ecs_fini(world);
}

void AsyncBlock_template_child_delete_before_progress(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child {"
        LINE "    Position: {v, 0}"
        LINE "    async { await fetch_entity(this) }"
        LINE "    async { await fetch(v) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_delete(world, ecs_lookup(world, "e.child"));
    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);
    test_int(ab_cancel_count, 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_prop_change_restarts_block(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  child {"
        LINE "    Position: {v, 0}"
        LINE "    async { await fetch_entity(this) }"
        LINE "    async { await fetch(v) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 2);
    int32_t v = 9;
    ecs_set_id(world, e, t, sizeof(v), &v);
    test_int(ab_position(world, child)->x, 9);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 4);
    test_uint(ab_entity_args[2], child);
    test_int(ab_args[3], 9);
    ecs_script_future_release(ab_futures[2]);
    ecs_script_future_release(ab_futures[3]);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_condition(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop enabled: bool = false"
        LINE "  if enabled {"
        LINE "    child { async { await fetch_entity(this) } }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 0);
    bool enabled = true;
    ecs_set_id(world, e, t, sizeof(enabled), &enabled);
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[0], child);
    ecs_script_future_release(ab_futures[0]);
    enabled = false;
    ecs_set_id(world, e, t, sizeof(enabled), &enabled);
    test_assert(!ecs_is_alive(world, child));
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);
    enabled = true;
    ecs_set_id(world, e, t, sizeof(enabled), &enabled);
    ecs_entity_t recreated = ecs_lookup(world, "e.child");
    test_assert(recreated && recreated != child);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[1], recreated);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_loop(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop count: i32 = 2"
        LINE "  for i in 0..count {"
        LINE "    \"child_{i}\" {"
        LINE "      async { await fetch(i) }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child_0");
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_args[0], 0);
    test_int(ab_args[1], 1);
    test_uint(ab_entities[0], child);
    ecs_script_future_release(ab_futures[1]);
    int32_t count = 1;
    ecs_set_id(world, e, t, sizeof(count), &count);
    test_uint(ecs_lookup(world, "e.child_0"), child);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[2], 0);
    test_uint(ab_entities[2], child);
    count = 3;
    ecs_set_id(world, e, t, sizeof(count), &count);
    test_int(ab_cancel_count, 3);
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ab_future_count, 6);
    test_int(ab_args[3], 0);
    test_int(ab_args[4], 1);
    test_int(ab_args[5], 2);
    test_uint(ab_entities[3], child);
    test_uint(ab_entities[4], ecs_lookup(world, "e.child_1"));
    test_uint(ab_entities[5], ecs_lookup(world, "e.child_2"));
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[2]);
    ecs_script_future_release(ab_futures[3]);
    ecs_script_future_release(ab_futures[4]);
    ecs_script_future_release(ab_futures[5]);

    ecs_fini(world);
}

void AsyncBlock_template_child_completed_block_restarts(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop count: i32 = 2"
        LINE "  for i in 0..count {"
        LINE "    \"child_{i}\" {"
        LINE "      async { await fetch(i) }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 2);
    ab_resolve(0, 0);
    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 2);
    int32_t count = 3;
    ecs_set_id(world, e, t, sizeof(count), &count);
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ab_future_count, 5);
    test_int(ab_args[2], 0);
    test_int(ab_args[3], 1);
    test_int(ab_args[4], 2);
    ab_resolve(2, 0);
    ab_resolve(3, 0);
    ab_resolve(4, 0);
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ecs_script_template_update(world, e, t), 0);
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ab_future_count, 8);
    for (int32_t i = 5; i < 8; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_template_child_nested_template_owner(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template Inner {"
        LINE "  mut v: i32 = 0"
        LINE "  child {"
        LINE "    async {"
        LINE "      const result = await fetch_entity(this)"
        LINE "      v = result"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "template Outer {"
        LINE "  mut v: i32 = 0"
        LINE "  inner {"
        LINE "    Inner: {}"
        LINE "    async {"
        LINE "      const result = await fetch_entity(this)"
        LINE "      v = result"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "e { Outer: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t inner = ecs_lookup(world, "e.inner");
    ecs_entity_t child = ecs_lookup(world, "e.inner.child");
    test_assert(e && inner && child);
    test_int(ecs_script_tasks_progress(world), 2);
    for (int32_t i = 0; i < 2; i ++) {
        test_assert(ab_entity_args[i] == child || ab_entity_args[i] == inner);
        test_uint(ab_entity_args[i], ab_entities[i]);
        ab_resolve(i, ab_entities[i] == child ? 10 : 20);
    }
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_mut_i32(world, e, "Outer", "v"), 20);
    test_int(ab_mut_i32(world, inner, "Inner", "v"), 10);

    ecs_fini(world);
}

void AsyncBlock_template_child_anonymous(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  for i in 0..2 {"
        LINE "    _ { async { await fetch_entity(this) } }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

ecs_entity_t e = ecs_lookup(world, "e");
    test_int(ecs_script_tasks_progress(world), 2);
    test_assert(ab_entities[0] != ab_entities[1]);
    for (int32_t i = 0; i < 2; i ++) {
        test_assert(ecs_is_alive(world, ab_entities[i]));
        test_uint(ab_entity_args[i], ab_entities[i]);
        test_uint(ecs_get_parent(world, ab_entities[i]), e);
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_template_child_branch_keeps_entity(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop enabled: bool = true"
        LINE "  if enabled {"
        LINE "    child { async { await fetch(1) } }"
        LINE "  } else {"
        LINE "    child { async { await fetch(2) } }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[0], 1);
    ecs_script_future_release(ab_futures[0]);
    bool enabled = false;
    ecs_set_id(world, e, t, sizeof(enabled), &enabled);
    test_uint(ecs_lookup(world, "e.child"), child);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 2);
    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    enabled = true;
    ecs_set_id(world, e, t, sizeof(enabled), &enabled);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[2], 1);
    ecs_script_future_release(ab_futures[2]);

    ecs_fini(world);
}

static void ab_delete_entity_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    ab_fetch_entity_callback(ctx, argc, argv, future);
    ecs_delete(ctx->world, ctx->entity);
}

void AsyncBlock_template_child_delete_from_callback(void) {
    ecs_world_t *world = ab_world();

    ecs_async_function(world, {
        .name = "delete_entity",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"e", ecs_id(ecs_entity_t)}},
        .callback = ab_delete_entity_callback,
        .cancel = ab_cancel_callback
    });

    ab_script(world,
        HEAD "template T {"
        LINE "  child {"
        LINE "    async {"
        LINE "      await delete_entity(this)"
        LINE "      await fetch(1)"
        LINE "    }"
        LINE "    async { await fetch(2) }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 1);
    test_assert(!ecs_is_alive(world, child));
    test_int(ab_future_count, 1);
    test_int(ab_cancel_count, 1);
    test_assert(ecs_script_future_is_cancelled(ab_futures[0]));
    ecs_script_future_release(ab_futures[0]);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_script_update_cancels(void) {
    ecs_world_t *world = ab_world();

    const char *code =
        HEAD "template T {"
        LINE "  child { async { await fetch_entity(this) } }"
        LINE "}"
        LINE "e { T: {} }";
    ecs_entity_t script = ab_script(world, code);
    test_int(ecs_script_tasks_progress(world), 1);
    ecs_script_future_release(ab_futures[0]);
    test_int(ecs_script_update(world, script, 0, code), 0);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[1], ecs_lookup(world, "e.child"));
    ecs_script_future_release(ab_futures[1]);
    test_int(ecs_script_update(world, script, 0, ""), 0);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_computed_name(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop name: string = \"left\""
        LINE "  \"{name}\" { async { await fetch_entity(this) } }"
        LINE "}"
        LINE "e { T: {} }");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t left = ecs_lookup(world, "e.left");
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[0], left);
    ecs_script_future_release(ab_futures[0]);
    char *name = "right";
    ecs_set_id(world, e, t, sizeof(name), &name);
    test_assert(!ecs_is_alive(world, left));
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[1], ecs_lookup(world, "e.right"));
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_this_shadows_local(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  child {"
        LINE "    const this: i32 = 7"
        LINE "    const captured = this"
        LINE "    async {"
        LINE "      await fetch_entity(this)"
        LINE "      await fetch(captured)"
        LINE "    }"
        LINE "    Position: {this, 0}"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ab_position(world, child)->x, 7);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[0], child);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 7);
    test_uint(ab_entities[1], child);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_assign_removes_self(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut enabled: bool = true"
        LINE "  if enabled {"
        LINE "    child {"
        LINE "      async {"
        LINE "        await fetch_entity(this)"
        LINE "        enabled = false"
        LINE "      }"
        LINE "      async { await fetch(1) }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "e { T: {} }");

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 2);
    test_uint(ab_entity_args[0], child);
    ecs_script_future_release(ab_futures[1]);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_assert(!ecs_is_alive(world, child));
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_unrelated_prop_restarts_all_blocks(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  prop other: i32 = 0"
        LINE "  const captured = v * 2"
        LINE "  async { await fetch(captured) }"
        LINE "  child { async { await fetch(42) } }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 2);
    int32_t props[] = {1, 10};
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 2);
    test_assert(ecs_script_future_is_cancelled(ab_futures[0]));
    test_assert(ecs_script_future_is_cancelled(ab_futures[1]));
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_args[2], 2);
    test_int(ab_args[3], 42);
    test_uint(ecs_lookup(world, "e.child"), child);
    props[0] = 7;
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 4);
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_args[4], 14);
    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }
    ecs_fini(world);
}

void AsyncBlock_template_prop_change_before_progress(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  async { await fetch(v) }"
        LINE "}"
        LINE "T a"
        LINE "T b");

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t t = ecs_lookup(world, "T");
    for (int32_t v = 2; v <= 4; v ++) {
        ecs_set_id(world, a, t, sizeof(v), &v);
    }
    test_int(ab_cancel_count, 0);
    test_int(ab_future_count, 0);
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    for (int32_t i = 0; i < 2; i ++) {
        test_assert(ab_entities[i] == a || ab_entities[i] == b);
        test_int(ab_args[i], ab_entities[i] == a ? 4 : 1);
    }
    int32_t v = 4;
    ecs_set_id(world, a, t, sizeof(v), &v);
    test_int(ab_cancel_count, 0);
    test_int(ecs_script_tasks_progress(world), 0);
    for (int32_t i = 0; i < 2; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }
    ecs_fini(world);
}

void AsyncBlock_template_static_ref_restarts_block(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "source { Position: {3, 0} }"
        LINE "template T {"
        LINE "  const captured = source[Position].x * 2"
        LINE "  async { await fetch(captured) }"
        LINE "}"
        LINE "T e");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[0], 6);
    ecs_entity_t source = ecs_lookup(world, "source");
    ecs_entity_t pos = ecs_lookup(world, "Position");
    ab_Position value = {7, 0};
    ecs_set_id(world, source, pos, sizeof(value), &value);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 14);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    ecs_fini(world);
}

void AsyncBlock_template_dynamic_ref_restarts_block(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "a { Position: {3, 0} }"
        LINE "b { Position: {5, 0} }"
        LINE "template T {"
        LINE "  prop target: entity = a"
        LINE "  const captured = target[Position].x"
        LINE "  child { async { await fetch(captured) } }"
        LINE "}"
        LINE "T e");

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[0], 3);
    ab_Position value = {7, 0};
    ecs_set_id(world, a, pos, sizeof(value), &value);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 7);
    ecs_set_id(world, e, t, sizeof(b), &b);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[2], 5);
    value.x = 9;
    ecs_set_id(world, a, pos, sizeof(value), &value);
    test_int(ab_cancel_count, 2);
    test_int(ecs_script_tasks_progress(world), 0);
    ecs_set_id(world, b, pos, sizeof(value), &value);
    test_int(ab_cancel_count, 3);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[3], 9);
    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }
    ecs_fini(world);
}

void AsyncBlock_template_mut_shared_between_blocks(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m: i32 = 1"
        LINE "  async {"
        LINE "    const r = await fetch(0)"
        LINE "    m = r"
        LINE "  }"
        LINE "  child { async {"
        LINE "    await fetch(1)"
        LINE "    await fetch(m)"
        LINE "  } }"
        LINE "}"
        LINE "T e");

    test_int(ecs_script_tasks_progress(world), 2);
    ab_resolve(0, 7);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_cancel_count, 0);
    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[2], 7);
    ecs_script_future_release(ab_futures[2]);
    ecs_fini(world);
}

void AsyncBlock_template_mut_sparse_external_update(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m: i32 = 1"
        LINE "  child { Position: {m, 0} }"
        LINE "  async {"
        LINE "    await fetch(0)"
        LINE "    await fetch(m)"
        LINE "    m = m + 1"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mut = ecs_lookup(world, "T.mut");
    ecs_entity_t tag = ecs_new(world);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ecs_has_id(world, mut, EcsSparse));
    test_int(ecs_script_tasks_progress(world), 1);
    ecs_add_id(world, e, tag);
    for (int32_t i = 0; i < 256; i ++) {
        ecs_add_id(world, ecs_new(world), mut);
    }
    test_assert(ecs_get_id(world, e, mut) == ptr);
    ecs_defer_begin(world);
    int32_t value = 9;
    ecs_set_id(world, e, mut, sizeof(value), &value);
    ecs_defer_end(world);
    test_int(ab_cancel_count, 0);
    test_int(ab_position(world, ecs_lookup(world, "e.child"))->x, 9);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 9);
    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 10);
    test_int(ab_cancel_count, 0);
    ecs_fini(world);
}

void AsyncBlock_template_resolved_block_cancelled_before_resume(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  prop v: i32 = 1"
        LINE "  mut m: i32 = 0"
        LINE "  async {"
        LINE "    const r = await fetch(v)"
        LINE "    m = r"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 1);
    ab_resolve(0, 100);
    int32_t v = 2;
    ecs_set_id(world, e, t, sizeof(v), &v);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 2);
    test_int(ab_mut_i32(world, e, "T", "m"), 0);
    ab_resolve(1, 20);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 20);
    ecs_fini(world);
}

void AsyncBlock_template_outer_capture_restarts_block(void) {
    ecs_world_t *world = ab_world();
    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_entity_t pos = ecs_lookup(world, "Position");
    ab_Position value = {3, 0};
    ecs_set_id(world, source, pos, sizeof(value), &value);

    ab_script(world,
        HEAD "const captured = source[Position].x"
        LINE "template T {"
        LINE "  mut m: i32 = 1"
        LINE "  async { await fetch(captured + m) }"
        LINE "}"
        LINE "T e");

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[0], 4);
    value.x = 7;
    ecs_set_id(world, source, pos, sizeof(value), &value);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 8);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    ecs_fini(world);
}

void AsyncBlock_template_mut_string_external_update(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut text: string = \"old\""
        LINE "  async {"
        LINE "    await fetch(0)"
        LINE "    text = \"{text}!\""
        LINE "    await fetch(text == \"new!\")"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mut = ecs_lookup(world, "T.mut");
    test_int(ecs_script_tasks_progress(world), 1);
    ecs_string_t value = "new";
    ecs_set_id(world, e, mut, sizeof(value), &value);
    test_int(ab_cancel_count, 0);
    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 1);
    test_str(*(ecs_string_t*)ecs_get_id(world, e, mut), "new!");
    ecs_script_future_release(ab_futures[1]);
    ecs_fini(world);
}

void AsyncBlock_template_sixty_four_muts_manual_restart(void) {
    ecs_world_t *world = ab_world();
    ecs_strbuf_t code = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&code, "template T {\n");
    for (int32_t i = 0; i < 64; i ++) {
        ecs_strbuf_append(&code, "mut m%d: i32 = %d\n", i, i);
    }
    ecs_strbuf_appendstr(&code,
        "async { await fetch(m63) }\n}\nT e\n");
    char *expr = ecs_strbuf_get(&code);
    ab_script(world, expr);
    ecs_os_free(expr);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[0], 63);
    test_int(ecs_script_template_update(world, e, t), 0);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 63);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);
    ecs_fini(world);
}

void AsyncBlock_template_mut_incremented_by_two_blocks(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template T {"
        LINE "  mut m: i32 = 0"
        LINE "  child { Position: {m, 0} }"
        LINE "  async {"
        LINE "    for i in 0..6 {"
        LINE "      await fetch(0)"
        LINE "      m = m + 1"
        LINE "    }"
        LINE "  }"
        LINE "  async {"
        LINE "    for i in 0..6 {"
        LINE "      await fetch(1)"
        LINE "      m = m + 1"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    test_int(ab_args[0], 0);
    test_int(ab_args[1], 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 0);

    int32_t pending[] = {0, 1};
    for (int32_t round = 0; round < 6; round ++) {
        int32_t first_new = ab_future_count;
        if (round % 3 == 0) {
            ab_resolve(pending[0], 0);
            ab_resolve(pending[1], 0);
            test_int(ecs_script_tasks_progress(world), 2);
        } else {
            int32_t first = round % 3 == 1 ? 1 : 0;
            ab_resolve(pending[first], 0);
            test_int(ecs_script_tasks_progress(world), 1);
            test_int(ab_mut_i32(world, e, "T", "m"), round * 2 + 1);
            test_int(ab_position(world, child)->x, round * 2 + 1);
            ab_resolve(pending[1 - first], 0);
            test_int(ecs_script_tasks_progress(world), 1);
        }

        test_int(ab_mut_i32(world, e, "T", "m"), (round + 1) * 2);
        test_int(ab_position(world, child)->x, (round + 1) * 2);
        test_int(ab_cancel_count, 0);
        test_int(ab_future_count - first_new, round == 5 ? 0 : 2);
        for (int32_t i = first_new; i < ab_future_count; i ++) {
            test_assert(ab_args[i] == 0 || ab_args[i] == 1);
            pending[ab_args[i]] = i;
        }
    }

    test_int(ab_future_count, 12);
    test_int(ab_mut_i32(world, e, "T", "m"), 12);
    test_int(ecs_script_tasks_progress(world), 0);
    ecs_fini(world);
}

void AsyncBlock_template_child_mut_after_reset(void) {
    ecs_world_t *world = ab_world();

    ab_script(world,
        HEAD "template Cell {"
        LINE "  prop value: i32 = 0"
        LINE "  Position: {value, 0}"
        LINE "}"
        LINE "template T {"
        LINE "  mut m: i32 = 0"
        LINE "  mut board: i32 = 1"
        LINE "  child { Position: {m, 0} }"
        LINE "  for i in 0..2 {"
        LINE "    Cell \"cell_{i}\"(value: board) {"
        LINE "      async {"
        LINE "        while true {"
        LINE "          await fetch(i)"
        LINE "          board = 1 - board"
        LINE "          m = m + 1"
        LINE "        }"
        LINE "      }"
        LINE "    }"
        LINE "  }"
        LINE "  reset {"
        LINE "    async {"
        LINE "      while true {"
        LINE "        await fetch(2)"
        LINE "        board = 1"
        LINE "        m = 0"
        LINE "      }"
        LINE "    }"
        LINE "  }"
        LINE "}"
        LINE "T e");

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ab_args[0], 0);
    test_int(ab_args[1], 1);
    test_int(ab_args[2], 2);

    ab_resolve(0, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 1);
    test_int(ab_position(world, child)->x, 1);

    ab_resolve(2, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 0);
    test_int(ab_position(world, child)->x, 0);

    ab_resolve(3, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 1);
    test_int(ab_position(world, child)->x, 1);

    ab_resolve(1, 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_mut_i32(world, e, "T", "m"), 2);
    test_int(ab_position(world, child)->x, 2);
    test_int(ab_cancel_count, 0);

    ecs_script_future_release(ab_futures[4]);
    ecs_script_future_release(ab_futures[5]);
    ecs_script_future_release(ab_futures[6]);
    ecs_fini(world);
}
