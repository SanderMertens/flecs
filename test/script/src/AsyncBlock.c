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

typedef struct { float x, y; } ab_Position;

void AsyncBlock_parse_async_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

void AsyncBlock_parse_async_block_no_space_before_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        HEAD "async{"
        LINE "  Foo {}"
        LINE "}", &ir_desc, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(str != NULL);
    test_assert(strstr(str, "async") != NULL);
    test_assert(strstr(str, "entity: async") == NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void AsyncBlock_parse_while(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  const v = await fetch(1)"
            LINE "  await fetch(v + 1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ab_future_count, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);
    test_uint(ab_entities[0], s);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 11);

    v = ecs_value(ecs_i32_t, {20});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_cancel_count, 0);

    ecs_fini(world);
}

void AsyncBlock_script_two_blocks(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  await fetch(1)"
            LINE "}"
            LINE "async {"
            LINE "  await fetch(2)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    test_int(ab_args[0], 1);
    test_int(ab_args[1], 2);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_no_blocks(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "e { Position: {1, 2} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);

    ecs_fini(world);
}

void AsyncBlock_script_block_no_await(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  const v = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_block_captures_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "const x = 5"
            LINE "const y = x * 2"
            LINE "async {"
            LINE "  await fetch(x + y)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 15);

    ecs_fini(world);
}

void AsyncBlock_script_block_for_loop(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  for i in 0..3 {"
            LINE "    await fetch(i)"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 0);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 1);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 2);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[2], &v), 0);
    ecs_script_future_release(ab_futures[2]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_block_if(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "const x = 5"
            LINE "async {"
            LINE "  if x > 3 {"
            LINE "    await fetch(1)"
            LINE "  } else {"
            LINE "    await fetch(2)"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ecs_fini(world);
}

void AsyncBlock_script_block_try_catch(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  try {"
            LINE "    await fetch(1)"
            LINE "  } catch {"
            LINE "    await fetch(2)"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    test_int(ecs_script_future_reject(ab_futures[0], "failed"), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 2);

    ecs_fini(world);
}

void AsyncBlock_script_update_cancels_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  await fetch(1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  await fetch(1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    test_int(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  await fetch(1)"
        LINE "}", &ir_desc, NULL), 0);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);

    ecs_fini(world);
}

void AsyncBlock_script_progress_from_pipeline(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  await fetch(1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ab_future_count, 0);
    ecs_progress(world, 0);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ecs_progress(world, 0);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    ecs_progress(world, 0);
    ecs_progress(world, 0);
    test_int(ab_future_count, 1);

    ecs_fini(world);
}

void AsyncBlock_template_block_assign_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  mut m = 0"
            LINE "  child { Position: {m, 0} }"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 0);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 5);
    test_uint(ab_entities[0], e);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 7);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->x, 7);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_this(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch_entity(this)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_uint(ab_entity_args[0], e);
    test_uint(ab_entities[0], e);

    ecs_fini(world);
}

void AsyncBlock_template_block_reads_component_via_this(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(this[T].v * 10)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 50);

    ecs_fini(world);
}

void AsyncBlock_template_block_captures_consts(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "const g = 100"
            LINE "template T {"
            LINE "  prop v = 1"
            LINE "  mut m = 3"
            LINE "  const w = v * 2"
            LINE "  async {"
            LINE "    await fetch(g + w + m)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 113);

    ecs_fini(world);
}

void AsyncBlock_template_two_blocks(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "  async {"
            LINE "    await fetch(v * 2)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    test_int(ab_args[0], 5);
    test_int(ab_args[1], 10);

    ecs_fini(world);
}

void AsyncBlock_template_two_instances(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  mut m = 0"
            LINE "  child { Position: {m, 0} }"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "a { T: {v: 1} }"
            LINE "b { T: {v: 2} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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

    ecs_value_t v = ecs_value(ecs_i32_t, {20});
    test_int(ecs_script_future_resolve(ab_futures[ib], &v), 0);
    ecs_script_future_release(ab_futures[ib]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, a, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 0);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, b, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 20);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, ecs_lookup(world, "b.child"), pos);
    test_int(p->x, 20);

    v = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(ab_futures[ia], &v), 0);
    ecs_script_future_release(ab_futures[ia]);

    test_int(ecs_script_tasks_progress(world), 1);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, a, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 10);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, ecs_lookup(world, "a.child"), pos);
    test_int(p->x, 10);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_no_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  child { Position: {v, 0} }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);

    ecs_fini(world);
}

void AsyncBlock_template_instance_delete_cancels(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child { Position: {v, 0} }"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 9);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m = 0"
            LINE "  child { Position: {m, 0} }"
            LINE "  async {"
            LINE "    while m < 3 {"
            LINE "      const r = await fetch(m)"
            LINE "      m = r"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 0);

    ecs_value_t v = ecs_value(ecs_i32_t, {1});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 1);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 1);

    v = ecs_value(ecs_i32_t, {2});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 2);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 2);

    v = ecs_value(ecs_i32_t, {3});
    test_int(ecs_script_future_resolve(ab_futures[2], &v), 0);
    ecs_script_future_release(ab_futures[2]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 3);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 3);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->x, 3);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_while_no_await(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m = 0"
            LINE "  async {"
            LINE "    while m < 5 {"
            LINE "      m = m + 1"
            LINE "    }"
            LINE "    await fetch(m)"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 5);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 5);

    ecs_fini(world);
}

void AsyncBlock_template_block_while_continue(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 3);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 3);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_block_while_false(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m = 10"
            LINE "  async {"
            LINE "    while m < 3 {"
            LINE "      await fetch(m)"
            LINE "    }"
            LINE "    await fetch(m + 1)"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 11);

    ecs_fini(world);
}

void AsyncBlock_template_block_try_catch_assign(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    test_int(ecs_script_future_reject(ab_futures[0], "failed"), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), -1);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, -1);

    ecs_fini(world);
}

void AsyncBlock_template_block_assign_updates_local(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m = 1"
            LINE "  async {"
            LINE "    m = m + 1"
            LINE "    m = m + 1"
            LINE "    await fetch(m)"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 3);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 3);

    ecs_fini(world);
}

void AsyncBlock_template_instance_reactive_after_block_done(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  mut m = 0"
            LINE "  child { Position: {v, m} }"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(e != 0);
    test_assert(t != 0);
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_value_t val = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &val), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 5);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->y, 7);

    test_int(ecs_script_tasks_progress(world), 0);

    int32_t v = 9;
    ecs_set_id(world, e, t, sizeof(int32_t), &v);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->x, 9);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->y, 7);

    test_int(ab_future_count, 1);
    test_int(ab_cancel_count, 0);

    ecs_fini(world);
}

void AsyncBlock_template_nested_in_template_instance(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Inner {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "template Outer {"
            LINE "  prop w = 1"
            LINE "  Inner child(v: w * 2)"
            LINE "}"
            LINE "e { Outer: {w: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 10);
    test_uint(ab_entities[0], child);

    ecs_fini(world);
}

void AsyncBlock_template_block_error_frees_task(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v = 1"
            LINE "  async {"
            LINE "    await fetch(v)"
            LINE "    await fetch(v)"
            LINE "  }"
            LINE "}"
            LINE "e { T: {v: 5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    ecs_log_set_level(-4);

    test_int(ecs_script_future_reject(ab_futures[0], "failed"), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    ecs_log_set_level(-1);

    test_int(ab_future_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_error_while_outside_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const x = 1"
        LINE "while x < 3 {"
        LINE "  child {}"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_outside_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  mut m = 0"
        LINE "  m = 5"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_to_prop(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "const c = 0"
        LINE "async {"
        LINE "  c = 5"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_unknown_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  e { Position: {1, 2} }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_component_in_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "template T {"
        LINE "  async {"
        LINE "    export mut m = 1"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_parse_async_missing_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_parse(world, NULL,
        "async\n", &ir_desc, NULL) == NULL);

    ecs_fini(world);
}

void AsyncBlock_error_parse_while_missing_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_log_set_level(-4);
    test_assert(ecs_script_parse(world, NULL,
        "async {\n  while x\n}\n", &ir_desc, NULL) == NULL);

    ecs_fini(world);
}

void AsyncBlock_template_child_this(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  child {"
            LINE "    async {"
            LINE "      await fetch_entity(this)"
            LINE "      await fetch_entity(this)"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

ecs_entity_t child = ecs_lookup(world, "e.child");
    test_assert(child != 0);
    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[0], child);
    test_uint(ab_entities[0], child);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[1], child);
    test_uint(ab_entities[1], child);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_scopes_and_instances(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  async { await fetch_entity(this) }"
            LINE "  left {"
            LINE "    async { await fetch_entity(this) }"
            LINE "    grandchild { async { await fetch_entity(this) } }"
            LINE "  }"
            LINE "  right { async { await fetch_entity(this) } }"
            LINE "}"
            LINE "a { T: {} }"
            LINE "b { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "b { T: {5} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t b = ecs_lookup(world, "b");
    ecs_entity_t child = ecs_lookup(world, "a.child");
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_args[0], 9);
    test_int(ab_args[1], 15);

    ecs_value_t v = ecs_value(ecs_i32_t, {42});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, a, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "value"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 42);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, b, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "value"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 0);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->y, 42);

    test_uint(ab_entity_args[2], a);
    test_uint(ab_entities[2], child);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[2], &v), 0);
    ecs_script_future_release(ab_futures[2]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[3], child);
    ecs_script_future_release(ab_futures[1]);
    ecs_script_future_release(ab_futures[3]);

    ecs_fini(world);
}

void AsyncBlock_template_child_delete_cancels(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child {"
            LINE "    Position: {v, 0}"
            LINE "    async { await fetch_entity(this) }"
            LINE "    async { await fetch(v) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child {"
            LINE "    Position: {v, 0}"
            LINE "    async { await fetch_entity(this) }"
            LINE "    async { await fetch(v) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child {"
            LINE "    Position: {v, 0}"
            LINE "    async { await fetch_entity(this) }"
            LINE "    async { await fetch(v) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child {"
            LINE "    Position: {v, 0}"
            LINE "    async { await fetch_entity(this) }"
            LINE "    async { await fetch(v) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child {"
            LINE "    Position: {v, 0}"
            LINE "    async { await fetch_entity(this) }"
            LINE "    async { await fetch(v) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

ecs_delete(world, ecs_lookup(world, "e.child"));
    test_int(ecs_script_tasks_progress(world), 0);
    test_int(ab_future_count, 0);
    test_int(ab_cancel_count, 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_prop_change_restarts_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  child {"
            LINE "    Position: {v, 0}"
            LINE "    async { await fetch_entity(this) }"
            LINE "    async { await fetch(v) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 2);
    int32_t v = 9;
    ecs_set_id(world, e, t, sizeof(v), &v);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 9);

    test_int(ab_cancel_count, 1);
    test_int(ecs_script_future_status(ab_futures[0]), EcsScriptFuturePending);
    test_int(ecs_script_future_status(ab_futures[1]), EcsScriptFutureCancelled);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 9);
    ecs_script_future_release(ab_futures[2]);
    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_condition(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop enabled: bool = false"
            LINE "  if enabled {"
            LINE "    child { async { await fetch_entity(this) } }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop count: i32 = 2"
            LINE "  for i in 0..count {"
            LINE "    \"child_{i}\" {"
            LINE "      async { await fetch(i) }"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop count: i32 = 2"
            LINE "  for i in 0..count {"
            LINE "    \"child_{i}\" {"
            LINE "      async { await fetch(i) }"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 2);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 2);
    int32_t count = 3;
    ecs_set_id(world, e, t, sizeof(count), &count);
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ab_future_count, 5);
    test_int(ab_args[2], 0);
    test_int(ab_args[3], 1);
    test_int(ab_args[4], 2);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[2], &v), 0);
    ecs_script_future_release(ab_futures[2]);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[3], &v), 0);
    ecs_script_future_release(ab_futures[3]);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[4], &v), 0);
    ecs_script_future_release(ab_futures[4]);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Inner {"
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
            LINE "e { Outer: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t inner = ecs_lookup(world, "e.inner");
    ecs_entity_t child = ecs_lookup(world, "e.inner.child");
    test_assert(e && inner && child);
    test_int(ecs_script_tasks_progress(world), 2);
    for (int32_t i = 0; i < 2; i ++) {
        test_assert(ab_entity_args[i] == child || ab_entity_args[i] == inner);
        test_uint(ab_entity_args[i], ab_entities[i]);

        ecs_value_t v = ecs_value(ecs_i32_t, {ab_entities[i] == child ? 10 : 20});
        test_int(ecs_script_future_resolve(ab_futures[i], &v), 0);
        ecs_script_future_release(ab_futures[i]);

    }
    test_int(ecs_script_tasks_progress(world), 2);

    ecs_entity_t t = ecs_lookup(world, "Outer");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "v"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 20);

    t = ecs_lookup(world, "Inner");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, inner, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "v"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 10);

    ecs_fini(world);
}

void AsyncBlock_template_child_anonymous(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  for i in 0..2 {"
            LINE "    _ { async { await fetch_entity(this) } }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop enabled: bool = true"
            LINE "  if enabled {"
            LINE "    child { async { await fetch(1) } }"
            LINE "  } else {"
            LINE "    child { async { await fetch(2) } }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_async_function(world, {
        .name = "delete_entity",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"e", ecs_id(ecs_entity_t)}},
        .callback = ab_delete_entity_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  child {"
            LINE "    async {"
            LINE "      await delete_entity(this)"
            LINE "      await fetch(1)"
            LINE "    }"
            LINE "    async { await fetch(2) }"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 1);
    test_assert(!ecs_is_alive(world, child));
    test_int(ab_future_count, 1);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_future_status(ab_futures[0]), EcsScriptFutureCancelled);
    ecs_script_future_release(ab_futures[0]);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_child_script_update_cancels(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    const char *code =
        HEAD "template T {"
        LINE "  child { async { await fetch_entity(this) } }"
        LINE "}"
        LINE "e { T: {} }";

    ecs_entity_t script = ecs_script(world, {
        .code = code,
        .ir = ir_enabled
    });
    test_assert(script != 0);
    const EcsScript *comp = ecs_get(world, script, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop name: string = \"left\""
            LINE "  \"{name}\" { async { await fetch_entity(this) } }"
            LINE "}"
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t child = ecs_lookup(world, "e.child");

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 7);

    test_int(ecs_script_tasks_progress(world), 1);
    test_uint(ab_entity_args[0], child);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 7);
    test_uint(ab_entities[1], child);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_template_child_assign_removes_self(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "e { T: {} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 2);
    test_uint(ab_entity_args[0], child);
    ecs_script_future_release(ab_futures[1]);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_assert(!ecs_is_alive(world, child));
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_template_unrelated_prop_keeps_blocks_running(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  prop other: i32 = 0"
            LINE "  const captured = v * 2"
            LINE "  async { await fetch(captured) }"
            LINE "  child { async { await fetch(42) } }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 2);
    int32_t props[] = {1, 10};
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 0);
    test_int(ecs_script_future_status(ab_futures[0]), EcsScriptFuturePending);
    test_int(ecs_script_future_status(ab_futures[1]), EcsScriptFuturePending);
    test_int(ab_future_count, 2);
    test_uint(ecs_lookup(world, "e.child"), child);
    props[0] = 7;
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_future_status(ab_futures[0]), EcsScriptFutureCancelled);
    test_int(ecs_script_future_status(ab_futures[1]), EcsScriptFuturePending);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 14);
    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }
    ecs_fini(world);
}

void AsyncBlock_template_prop_change_before_progress(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  async { await fetch(v) }"
            LINE "}"
            LINE "T a"
            LINE "T b",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "source { Position: {3, 0} }"
            LINE "template T {"
            LINE "  const captured = source[Position].x * 2"
            LINE "  async { await fetch(captured) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "a { Position: {3, 0} }"
            LINE "b { Position: {5, 0} }"
            LINE "template T {"
            LINE "  prop target: entity = a"
            LINE "  const captured = target[Position].x"
            LINE "  child { async { await fetch(captured) } }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(ecs_script_tasks_progress(world), 2);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_cancel_count, 0);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[2], 7);
    ecs_script_future_release(ab_futures[2]);
    ecs_fini(world);
}

void AsyncBlock_template_mut_sparse_external_update(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m: i32 = 1"
            LINE "  child { Position: {m, 0} }"
            LINE "  async {"
            LINE "    await fetch(0)"
            LINE "    await fetch(m)"
            LINE "    m = m + 1"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *stage_1 = ecs_get_stage(world, 0);
    int32_t value = 9;
    ecs_set_id(stage_1, e, mut, sizeof(value), &value);
    ecs_merge(stage_1);
    test_int(ab_cancel_count, 0);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, ecs_lookup(world, "e.child"), pos);
    test_int(p->x, 9);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 9);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut_1 = ecs_lookup_child(world, t, "mut");
    test_assert(mut_1 != 0);
    const void *val = ecs_get_id(world, e, mut_1);
    test_assert(val != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut_1, ECS_CONST_CAST(void*, val));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 10);

    test_int(ab_cancel_count, 0);
    ecs_fini(world);
}

void AsyncBlock_template_resolved_block_cancelled_before_resume(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  mut m: i32 = 0"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 1);

    ecs_value_t val = ecs_value(ecs_i32_t, {100});
    test_int(ecs_script_future_resolve(ab_futures[0], &val), 0);
    ecs_script_future_release(ab_futures[0]);

    int32_t v = 2;
    ecs_set_id(world, e, t, sizeof(v), &v);
    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 2);

    ecs_entity_t tmpl = ecs_lookup(world, "T");
    test_assert(tmpl != 0);
    ecs_entity_t mut = ecs_lookup_child(world, tmpl, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 0);

    val = ecs_value(ecs_i32_t, {20});
    test_int(ecs_script_future_resolve(ab_futures[1], &val), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);

    tmpl = ecs_lookup(world, "T");
    test_assert(tmpl != 0);
    mut = ecs_lookup_child(world, tmpl, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 20);

    ecs_fini(world);
}

void AsyncBlock_template_outer_capture_restarts_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t source = ecs_entity(world, { .name = "source" });
    ecs_entity_t pos = ecs_lookup(world, "Position");
    ab_Position value = {3, 0};
    ecs_set_id(world, source, pos, sizeof(value), &value);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "const captured = source[Position].x"
            LINE "template T {"
            LINE "  mut m: i32 = 1"
            LINE "  async { await fetch(captured + m) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut text: string = \"old\""
            LINE "  async {"
            LINE "    await fetch(0)"
            LINE "    text = \"{text}!\""
            LINE "    await fetch(text == \"new!\")"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t mut = ecs_lookup(world, "T.mut");
    test_int(ecs_script_tasks_progress(world), 1);
    ecs_string_t value = "new";
    ecs_set_id(world, e, mut, sizeof(value), &value);
    test_int(ab_cancel_count, 0);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_args[1], 1);
    test_str(*(ecs_string_t*)ecs_get_id(world, e, mut), "new!");
    ecs_script_future_release(ab_futures[1]);
    ecs_fini(world);
}

void AsyncBlock_template_sixty_four_muts_manual_restart(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_strbuf_t code = ECS_STRBUF_INIT;
    ecs_strbuf_appendstr(&code, "template T {\n");
    for (int32_t i = 0; i < 64; i ++) {
        ecs_strbuf_append(&code, "mut m%d: i32 = %d\n", i, i);
    }
    ecs_strbuf_appendstr(&code,
        "async { await fetch(m63) }\n}\nT e\n");
    char *expr = ecs_strbuf_get(&code);

    ecs_entity_t s = ecs_script(world, {
        .code = expr,
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

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
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
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
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);
    test_int(ab_args[0], 0);
    test_int(ab_args[1], 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 0);

    int32_t pending[] = {0, 1};
    for (int32_t round = 0; round < 6; round ++) {
        int32_t first_new = ab_future_count;
        if (round % 3 == 0) {
            ecs_value_t v = ecs_value(ecs_i32_t, {0});
            test_int(ecs_script_future_resolve(ab_futures[pending[0]], &v), 0);
            ecs_script_future_release(ab_futures[pending[0]]);

            v = ecs_value(ecs_i32_t, {0});
            test_int(ecs_script_future_resolve(ab_futures[pending[1]], &v), 0);
            ecs_script_future_release(ab_futures[pending[1]]);

            test_int(ecs_script_tasks_progress(world), 2);
        } else {
            int32_t first = round % 3 == 1 ? 1 : 0;

            ecs_value_t v = ecs_value(ecs_i32_t, {0});
            test_int(ecs_script_future_resolve(ab_futures[pending[first]], &v), 0);
            ecs_script_future_release(ab_futures[pending[first]]);

            test_int(ecs_script_tasks_progress(world), 1);

            t = ecs_lookup(world, "T");
            test_assert(t != 0);
            mut = ecs_lookup_child(world, t, "mut");
            test_assert(mut != 0);
            ptr = ecs_get_id(world, e, mut);
            test_assert(ptr != NULL);
            cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
            test_int(ecs_meta_push(&cur), 0);
            test_int(ecs_meta_member(&cur, "m"), 0);
            test_int((int32_t)ecs_meta_get_int(&cur), round * 2 + 1);

            ecs_entity_t pos = ecs_lookup(world, "Position");
            test_assert(pos != 0);
            const ab_Position *p = ecs_get_id(world, child, pos);
            test_int(p->x, round * 2 + 1);

            v = ecs_value(ecs_i32_t, {0});
            test_int(ecs_script_future_resolve(ab_futures[pending[1 - first]], &v), 0);
            ecs_script_future_release(ab_futures[pending[1 - first]]);

            test_int(ecs_script_tasks_progress(world), 1);
        }

        t = ecs_lookup(world, "T");
        test_assert(t != 0);
        mut = ecs_lookup_child(world, t, "mut");
        test_assert(mut != 0);
        ptr = ecs_get_id(world, e, mut);
        test_assert(ptr != NULL);
        cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
        test_int(ecs_meta_push(&cur), 0);
        test_int(ecs_meta_member(&cur, "m"), 0);
        test_int((int32_t)ecs_meta_get_int(&cur), (round + 1) * 2);

        ecs_entity_t pos = ecs_lookup(world, "Position");
        test_assert(pos != 0);
        const ab_Position *p = ecs_get_id(world, child, pos);
        test_int(p->x, (round + 1) * 2);

        test_int(ab_cancel_count, 0);
        test_int(ab_future_count - first_new, round == 5 ? 0 : 2);
        for (int32_t i = first_new; i < ab_future_count; i ++) {
            test_assert(ab_args[i] == 0 || ab_args[i] == 1);
            pending[ab_args[i]] = i;
        }
    }

    test_int(ab_future_count, 12);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 12);

    test_int(ecs_script_tasks_progress(world), 0);
    ecs_fini(world);
}

void AsyncBlock_template_child_mut_after_reset(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template Cell {"
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
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t child = ecs_lookup(world, "e.child");
    test_int(ecs_script_tasks_progress(world), 3);
    test_int(ab_args[0], 0);
    test_int(ab_args[1], 1);
    test_int(ab_args[2], 2);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t t = ecs_lookup(world, "T");
    test_assert(t != 0);
    ecs_entity_t mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    const void *ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    ecs_meta_cursor_t cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 1);

    ecs_entity_t pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    const ab_Position *p = ecs_get_id(world, child, pos);
    test_int(p->x, 1);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[2], &v), 0);
    ecs_script_future_release(ab_futures[2]);

    test_int(ecs_script_tasks_progress(world), 1);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 0);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->x, 0);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[3], &v), 0);
    ecs_script_future_release(ab_futures[3]);

    test_int(ecs_script_tasks_progress(world), 1);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 1);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->x, 1);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    test_int(ecs_script_tasks_progress(world), 1);

    t = ecs_lookup(world, "T");
    test_assert(t != 0);
    mut = ecs_lookup_child(world, t, "mut");
    test_assert(mut != 0);
    ptr = ecs_get_id(world, e, mut);
    test_assert(ptr != NULL);
    cur = ecs_meta_cursor(world, mut, ECS_CONST_CAST(void*, ptr));
    test_int(ecs_meta_push(&cur), 0);
    test_int(ecs_meta_member(&cur, "m"), 0);
    test_int((int32_t)ecs_meta_get_int(&cur), 2);

    pos = ecs_lookup(world, "Position");
    test_assert(pos != 0);
    p = ecs_get_id(world, child, pos);
    test_int(p->x, 2);

    test_int(ab_cancel_count, 0);

    ecs_script_future_release(ab_futures[4]);
    ecs_script_future_release(ab_futures[5]);
    ecs_script_future_release(ab_futures[6]);
    ecs_fini(world);
}

static void ab_fetch_delete_script_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)argc;
    (void)argv;
    ab_futures[ab_future_count ++] = future;
    ecs_delete(ctx->world, ctx->entity);
}

static void ab_fetch_update_script_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)argc;
    (void)argv;
    ab_futures[ab_future_count ++] = future;
    ecs_script_update(ctx->world, ctx->entity, 0, "e {}");
}

void AsyncBlock_delete_script_from_await_callback(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"x", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_delete_script_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  const v = await fetch(1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);
    test_assert(!ecs_is_alive(world, s));

    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_update_script_from_await_callback(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"x", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_update_script_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  const v = await fetch(1)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);
    test_assert(ecs_lookup(world, "e") != 0);

    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 0);

    ecs_fini(world);
}

void AsyncBlock_script_reads_mut_assigned_by_template_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_entity_t position = ecs_struct(world, {
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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m: i32 = 0"
            LINE "  async {"
            LINE "    const r = await fetch(1)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }"
            LINE "const v = e[T.mut].m"
            LINE "out { Position: {v, 0} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t out = ecs_lookup(world, "out");
    test_assert(out != 0);
    const ab_Position *p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);
    ecs_entity_t mut = ecs_lookup(world, "T.mut");
    test_assert(mut != 0);
    const ecs_i32_t *m = ecs_get_id(world, e, mut);
    test_assert(m != NULL);
    test_int(*m, 7);

    test_uint(ecs_lookup(world, "out"), out);
    p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 7);

    ecs_fini(world);
}

void AsyncBlock_assign_export_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_entity_t position = ecs_struct(world, {
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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut m: i32 = 0"
            LINE "async {"
            LINE "  const r = await fetch(1)"
            LINE "  m = r"
            LINE "}"
            LINE "out { Position: {m, 0} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t out = ecs_lookup(world, "out");
    test_assert(out != 0);
    const ab_Position *p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t m = ecs_lookup(world, "m");
    test_assert(m != 0);
    ecs_value_t mv = ecs_mut_var_get(world, m);
    test_assert(mv.ptr != NULL);
    test_uint(mv.type, ecs_id(ecs_i32_t));
    test_int(*(ecs_i32_t*)mv.ptr, 7);

    test_uint(ecs_lookup(world, "out"), out);
    p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 7);

    ecs_fini(world);
}

void AsyncBlock_assign_export_mut_from_template(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_entity_t position = ecs_struct(world, {
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

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut m: i32 = 0"
            LINE "template T {"
            LINE "  async {"
            LINE "    const r = await fetch(1)"
            LINE "    m = r"
            LINE "  }"
            LINE "}"
            LINE "e { T: {} }"
            LINE "out { Position: {m, 0} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t out = ecs_lookup(world, "out");
    test_assert(out != 0);
    const ab_Position *p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_entity_t m = ecs_lookup(world, "m");
    test_assert(m != 0);
    ecs_value_t mv = ecs_mut_var_get(world, m);
    test_assert(mv.ptr != NULL);
    test_int(*(ecs_i32_t*)mv.ptr, 7);

    test_uint(ecs_lookup(world, "out"), out);
    p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 7);

    ecs_fini(world);
}

void AsyncBlock_assign_extern_mut_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ecs_os_zeromem(ab_entities);
    ecs_os_zeromem(ab_entity_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_entity_t position = ecs_struct(world, {
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

    ecs_i32_t value = 0;
    ecs_entity_t m = ecs_mut_var(world, {
        .name = "m",
        .type = ecs_id(ecs_i32_t),
        .value = &value
    });
    test_assert(m != 0);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "async {"
            LINE "  const r = await fetch(1)"
            LINE "  m = r + 1"
            LINE "}"
            LINE "out { Position: {m, 0} }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t out = ecs_lookup(world, "out");
    test_assert(out != 0);
    const ab_Position *p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 0);

    test_int(ecs_script_tasks_progress(world), 1);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {7});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    test_int(ecs_script_tasks_progress(world), 1);

    ecs_value_t mv = ecs_mut_var_get(world, m);
    test_assert(mv.ptr != NULL);
    test_int(*(ecs_i32_t*)mv.ptr, 8);

    test_uint(ecs_lookup(world, "out"), out);
    p = ecs_get_id(world, out, position);
    test_assert(p != NULL);
    test_int(p->x, 8);

    ecs_fini(world);
}

void AsyncBlock_error_assign_to_export_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export const c: i32 = 0"
        LINE "async {"
        LINE "  c = 5"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_compound_assign_export_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut a: i32 = 10"
            LINE "export mut b: i32 = 10"
            LINE "export mut c: i32 = 10"
            LINE "export mut d: i32 = 10"
            LINE "async {"
            LINE "  a += 2 * 3"
            LINE "  b -= 2 + 3"
            LINE "  c *= 2 + 3"
            LINE "  d /= 2"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const char *names[] = {"a", "b", "c", "d"};
    int32_t expect[] = {16, 5, 50, 5};
    for (int32_t i = 0; i < 4; i ++) {
        ecs_entity_t var = ecs_lookup(world, names[i]);
        test_assert(var != 0);
        test_int(*(int32_t*)ecs_mut_var_get(world, var).ptr, expect[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_compound_assign_template_mut_notifies(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut m: i32 = 1"
            LINE "  Position: {m, 0}"
            LINE "  async {"
            LINE "    m += 4"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    test_assert(e != 0);

    const ab_Position *p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_int(p->x, 1);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    p = ecs_get_id(world, e, position);
    test_assert(p != NULL);
    test_int(p->x, 5);

    ecs_fini(world);
}

void AsyncBlock_error_compound_assign_outside_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut a: i32 = 10"
        LINE "a += 1", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_compound_assign_incompatible_type(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "export mut a: string = \"foo\""
        LINE "async {"
        LINE "  a += 1"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_break_for(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "for i in 0..5 {"
        LINE "  if i == 2 {"
        LINE "    break"
        LINE "  }"
        LINE "  \"e_{i}\" {}"
        LINE "}"
        LINE "after {}", &ir_desc, NULL) == 0);

    test_assert(ecs_lookup(world, "e_0") != 0);
    test_assert(ecs_lookup(world, "e_1") != 0);
    test_assert(ecs_lookup(world, "e_2") == 0);
    test_assert(ecs_lookup(world, "e_3") == 0);
    test_assert(ecs_lookup(world, "after") != 0);

    ecs_fini(world);
}

void AsyncBlock_break_while_in_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut m: i32 = 0"
            LINE "export mut after: i32 = 0"
            LINE "async {"
            LINE "  while true {"
            LINE "    m += 1"
            LINE "    if m == 3 {"
            LINE "      break"
            LINE "    }"
            LINE "  }"
            LINE "  after = m * 2"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "m")).ptr, 3);
    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "after")).ptr, 6);

    ecs_fini(world);
}

void AsyncBlock_break_innermost_loop(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut outer: i32 = 0"
            LINE "export mut inner: i32 = 0"
            LINE "async {"
            LINE "  for i in 0..3 {"
            LINE "    outer += 1"
            LINE "    for j in 0..10 {"
            LINE "      if j == 2 {"
            LINE "        break"
            LINE "      }"
            LINE "      inner += 1"
            LINE "    }"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "outer")).ptr, 3);
    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "inner")).ptr, 6);

    ecs_fini(world);
}

void AsyncBlock_error_break_outside_loop(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  break"
        LINE "}", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "break", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_error_break_async_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "for i in 0..3 {"
        LINE "  async {"
        LINE "    break"
        LINE "  }"
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_local_mut(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: f64 = 0"
            LINE "async {"
            LINE "  mut total: f64 = 1"
            LINE "  for i in 0..4 {"
            LINE "    const before = total"
            LINE "    total += i"
            LINE "    total = total + (total - before)"
            LINE "  }"
            LINE "  out = total"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_flt(*(double*)ecs_mut_var_get(
        world, ecs_lookup(world, "out")).ptr, 13);

    ecs_fini(world);
}

void AsyncBlock_local_mut_across_await(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "async {"
            LINE "  mut total: i32 = 100"
            LINE "  mut name: string = \"a\""
            LINE "  const a = await fetch(1)"
            LINE "  total += a"
            LINE "  name = \"b\""
            LINE "  const b = await fetch(2)"
            LINE "  total += b"
            LINE "  if name == \"b\" {"
            LINE "    out = total"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    v = ecs_value(ecs_i32_t, {20});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_future_release(ab_futures[1]);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "out")).ptr, 130);

    ecs_fini(world);
}

void AsyncBlock_local_mut_await_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "async {"
            LINE "  mut value = await fetch(1)"
            LINE "  value += 1"
            LINE "  out = value"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {41});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "out")).ptr, 42);

    ecs_fini(world);
}

void AsyncBlock_local_mut_resets_in_loop(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "async {"
            LINE "  for i in 0..3 {"
            LINE "    mut local: i32 = 1"
            LINE "    local += i"
            LINE "    out += local"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "out")).ptr, 6);

    ecs_fini(world);
}

void AsyncBlock_local_mut_per_task(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop start: i32 = 0"
            LINE "  mut out: i32 = 0"
            LINE "  Position: {out, 0}"
            LINE "  async {"
            LINE "    mut local: i32 = start"
            LINE "    const v = await fetch(0)"
            LINE "    local += v"
            LINE "    out = local"
            LINE "  }"
            LINE "}"
            LINE "T a(start: 10)"
            LINE "T b(start: 20)",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    for (int32_t i = 0; i < 2; i ++) {
        ecs_value_t v = ecs_value(ecs_i32_t, {1});
        test_int(ecs_script_future_resolve(ab_futures[i], &v), 0);
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const ab_Position *a = ecs_get_id(world, ecs_lookup(world, "a"), position);
    const ab_Position *b = ecs_get_id(world, ecs_lookup(world, "b"), position);
    test_assert(a != NULL);
    test_assert(b != NULL);
    test_int(a->x, 11);
    test_int(b->x, 21);

    ecs_fini(world);
}

void AsyncBlock_local_mut_not_reactive(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut count: i32 = 7"
            LINE "  Position: {count, 0}"
            LINE "  async {"
            LINE "    mut count: i32 = 0"
            LINE "    count = 100"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const ab_Position *p = ecs_get_id(world, ecs_lookup(world, "e"), position);
    test_assert(p != NULL);
    test_int(p->x, 7);

    ecs_fini(world);
}

void AsyncBlock_local_mut_array_from_array(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut kind: i32[3] = [1, 2, 3]"
            LINE "export mut out: i32 = 0"
            LINE "proc update() {"
            LINE "  mut k: i32[3] = kind"
            LINE "  k[1] = 10"
            LINE "  out = k[0] + k[1] + k[2]"
            LINE "}"
            LINE "async { update() }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 14);

    int32_t *kind = ecs_mut_var_get(world, ecs_lookup(world, "kind")).ptr;
    test_int(kind[0], 1);
    test_int(kind[1], 2);
    test_int(kind[2], 3);

    ecs_fini(world);
}

void AsyncBlock_error_local_mut_outside_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "mut x = 0", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "e {"
        LINE "  mut x = 0"
        LINE "}", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  mut x: i32"
        LINE "}", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  mut x: i32 = 0"
        LINE "  mut x: i32 = 1"
        LINE "}", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  const x: i32 = 0"
        LINE "  x = 1"
        LINE "}", &ir_desc, NULL) != 0);

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "async {"
        LINE "  mut x: i32 = 0"
        LINE "  x = \"foo\""
        LINE "}", &ir_desc, NULL) != 0);

    ecs_fini(world);
}

void AsyncBlock_restart_only_block_reading_prop(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop a: i32 = 1"
            LINE "  prop b: i32 = 2"
            LINE "  async { await fetch(a) }"
            LINE "  async { await fetch(b) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    test_int(ecs_script_tasks_progress(world), 2);
    test_int(ab_future_count, 2);

    int32_t props[] = {1, 20};
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_future_status(ab_futures[0]), EcsScriptFuturePending);
    test_int(ecs_script_future_status(ab_futures[1]), EcsScriptFutureCancelled);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 20);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_surviving_block_keeps_position(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop a: i32 = 1"
            LINE "  mut steps: i32 = 0"
            LINE "  const label = a * 2"
            LINE "  async {"
            LINE "    await fetch(1)"
            LINE "    await fetch(2)"
            LINE "    await fetch(3)"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 2);

    int32_t prop = 5;
    ecs_set_id(world, e, t, sizeof(prop), &prop);
    test_int(ab_cancel_count, 0);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 3);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_restart_follows_const_chain(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  prop other: i32 = 0"
            LINE "  const first = v * 2"
            LINE "  const second = first + 1"
            LINE "  async { await fetch(second) }"
            LINE "  async { await fetch(42) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    int32_t props[] = {1, 9};
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 0);

    props[0] = 5;
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 1);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 11);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_block_in_removed_if_branch_stops(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop on: bool = true"
            LINE "  if on {"
            LINE "    child { async { await fetch(1) } }"
            LINE "  }"
            LINE "  async { await fetch(2) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    bool on = false;
    ecs_set_id(world, e, t, sizeof(on), &on);
    test_int(ab_cancel_count, 1);
    test_int(ecs_script_future_status(ab_futures[0]), EcsScriptFutureCancelled);
    test_int(ecs_script_future_status(ab_futures[1]), EcsScriptFuturePending);

    on = true;
    ecs_set_id(world, e, t, sizeof(on), &on);
    test_int(ab_cancel_count, 1);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);
    test_int(ab_args[2], 1);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_block_on_deleted_entity_stops(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  child { async { await fetch(1) } }"
            LINE "  async { await fetch(2) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    ecs_delete(world, ecs_lookup(world, "e.child"));
    test_int(ab_cancel_count, 1);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_block_restarts_for_loop_var(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  prop count: i32 = 1"
            LINE "  prop other: i32 = 0"
            LINE "  for i in 0..count {"
            LINE "    \"c_{i}\" {"
            LINE "      async { await fetch(i) }"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);

    int32_t props[] = {1, 5};
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 0);

    props[0] = 2;
    ecs_set_id(world, e, t, sizeof(props), props);
    test_int(ab_cancel_count, 1);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 3);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_export_mut_read_does_not_restart(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut g: i32 = 1"
            LINE "export mut seen: i32 = 0"
            LINE "fn get() -> i32 { g * 10 }"
            LINE "template T {"
            LINE "  async {"
            LINE "    const a = await fetch(g)"
            LINE "    const b = await fetch(get())"
            LINE "    seen = g"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t g = ecs_lookup(world, "g");
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);
    test_int(ab_args[0], 1);

    *(int32_t*)ecs_mut_var_get(world, g).ptr = 2;
    ecs_mut_var_modified(world, g);
    test_int(ab_cancel_count, 0);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 20);

    *(int32_t*)ecs_mut_var_get(world, g).ptr = 3;
    ecs_mut_var_modified(world, g);
    test_int(ab_cancel_count, 0);

    v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(
        world, ecs_lookup(world, "seen")).ptr, 3);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

void AsyncBlock_export_mut_via_template_const_restarts(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ecs_os_zeromem(ab_args);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut g: i32 = 1"
            LINE "template T {"
            LINE "  const captured = g * 2"
            LINE "  async { await fetch(captured) }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t g = ecs_lookup(world, "g");
    ecs_script_tasks_progress(world);
    test_int(ab_args[0], 2);

    *(int32_t*)ecs_mut_var_get(world, g).ptr = 5;
    ecs_mut_var_modified(world, g);
    test_int(ab_cancel_count, 1);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);
    test_int(ab_args[1], 10);

    for (int32_t i = 0; i < ab_future_count; i ++) {
        ecs_script_future_release(ab_futures[i]);
    }

    ecs_fini(world);
}

typedef struct { int32_t a, b; } ab_Pair;
typedef struct { ab_Pair first; ab_Pair second; } ab_Nested;

void AsyncBlock_assign_struct_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Pair(a: i32, b: i32)"
            LINE "export mut st: Pair = {1, 2}"
            LINE "async {"
            LINE "  st.a = 10"
            LINE "  st.b += 5"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ab_Pair *st = ecs_mut_var_get(world, ecs_lookup(world, "st")).ptr;
    test_int(st->a, 10);
    test_int(st->b, 7);

    ecs_fini(world);
}

void AsyncBlock_assign_nested_struct_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Pair(a: i32, b: i32)"
            LINE "struct Nested(first: Pair, second: Pair)"
            LINE "export mut st: Nested = {{1, 2}, {3, 4}}"
            LINE "async {"
            LINE "  st.second.a = 30"
            LINE "  st.first = {10, 20}"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ab_Nested *st = ecs_mut_var_get(world, ecs_lookup(world, "st")).ptr;
    test_int(st->first.a, 10);
    test_int(st->first.b, 20);
    test_int(st->second.a, 30);
    test_int(st->second.b, 4);

    ecs_fini(world);
}

void AsyncBlock_assign_array_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[3] = {}"
            LINE "async {"
            LINE "  v[0] = 5"
            LINE "  for i in 1..3 {"
            LINE "    v[i] = v[i - 1] * 2"
            LINE "  }"
            LINE "  v[2] -= 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const int32_t *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(v[0], 5);
    test_int(v[1], 10);
    test_int(v[2], 19);

    ecs_fini(world);
}

void AsyncBlock_assign_vector_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[] = [1, 2, 3]"
            LINE "async {"
            LINE "  v[1] = 20"
            LINE "  v[2] += 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ecs_vec_t *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(ecs_vec_count(v), 3);
    int32_t *arr = ecs_vec_first(v);
    test_int(arr[0], 1);
    test_int(arr[1], 20);
    test_int(arr[2], 4);

    ecs_fini(world);
}

void AsyncBlock_assign_element_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Pair(a: i32, b: i32)"
            LINE "export mut v: Pair[2] = {}"
            LINE "async {"
            LINE "  v[1].b = 7"
            LINE "  if v[1].b > 0 {"
            LINE "    v[1].b -= 1"
            LINE "  }"
            LINE "  v[0] = {1, 2}"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ab_Pair *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(v[0].a, 1);
    test_int(v[0].b, 2);
    test_int(v[1].a, 0);
    test_int(v[1].b, 6);

    ecs_fini(world);
}

void AsyncBlock_assign_member_element_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t values = ecs_array(world, {
        .type = ecs_id(ecs_i32_t), .count = 2 });
    ecs_entity_t item = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Item" }),
        .members = {{"values", values}}
    });
    ecs_entity_t items = ecs_array(world, { .type = item, .count = 2 });
    ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Group" }),
        .members = {{"items", items}}
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut g: Group = {}"
            LINE "async {"
            LINE "  g.items[1].values[0] = 9"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const int32_t *g = ecs_mut_var_get(world, ecs_lookup(world, "g")).ptr;
    test_int(g[0], 0);
    test_int(g[1], 0);
    test_int(g[2], 9);
    test_int(g[3], 0);

    ecs_fini(world);
}

void AsyncBlock_assign_template_mut_element_notifies(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template T {"
            LINE "  mut v: i32[2] = {}"
            LINE "  Position: {v[0], v[1]}"
            LINE "  async {"
            LINE "    v[1] = 8"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const ab_Position *p = ecs_get_id(world, ecs_lookup(world, "e"), position);
    test_assert(p != NULL);
    test_int(p->x, 0);
    test_int(p->y, 8);

    ecs_fini(world);
}

void AsyncBlock_assign_export_mut_element_notifies(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[2] = {}"
            LINE "template T {"
            LINE "  Position: {v[1], 0}"
            LINE "}"
            LINE "T e"
            LINE "async {"
            LINE "  v[1] = 6"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const ab_Position *p = ecs_get_id(world, ecs_lookup(world, "e"), position);
    test_assert(p != NULL);
    test_int(p->x, 6);

    ecs_fini(world);
}

void AsyncBlock_assign_local_mut_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Pair(a: i32, b: i32)"
            LINE "export mut out: i32 = 0"
            LINE "async {"
            LINE "  mut p: Pair = {1, 2}"
            LINE "  mut v: i32[2] = {}"
            LINE "  p.b = 5"
            LINE "  v[1] = 10"
            LINE "  out = p.a + p.b + v[1]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 16);

    ecs_fini(world);
}

static int32_t ab_call_order[8];
static int32_t ab_call_count = 0;

static void ab_order_func(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    ab_call_order[ab_call_count ++] = *(int32_t*)argv[0].ptr;
    *(int32_t*)result->ptr = *(int32_t*)argv[0].ptr;
}

void AsyncBlock_assign_index_evaluated_before_value(void) {
    ecs_world_t *world = ecs_init();

    ab_call_count = 0;

    ecs_function(world, {
        .name = "order",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_order_func
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[3] = {}"
            LINE "export mut n: i32 = 1"
            LINE "async {"
            LINE "  v[order(n)] = order(2)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(ab_call_count, 2);
    test_int(ab_call_order[0], 1);
    test_int(ab_call_order[1], 2);

    const int32_t *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(v[0], 0);
    test_int(v[1], 2);
    test_int(v[2], 0);

    ecs_fini(world);
}

void AsyncBlock_assign_index_out_of_range(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut a: i32[2] = {}"
            LINE "export mut b: i32[] = [1, 2]"
            LINE "export mut after: i32 = 0"
            LINE "export mut caught: i32 = 0"
            LINE "export mut idx: i32 = 2"
            LINE "export mut neg: i32 = -1"
            LINE "async { a[idx] = 5; after = 1 }"
            LINE "async { a[neg] = 5; after = 2 }"
            LINE "async { b[idx] = 5; after = 3 }"
            LINE "async { b[neg] = 5; after = 4 }"
            LINE "async { a[1] = 7 }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const int32_t *a = ecs_mut_var_get(world, ecs_lookup(world, "a")).ptr;
    test_int(a[0], 0);
    test_int(a[1], 7);
    const ecs_vec_t *b = ecs_mut_var_get(world, ecs_lookup(world, "b")).ptr;
    test_int(ecs_vec_count(b), 2);
    test_int(ecs_vec_first_t(b, int32_t)[0], 1);
    test_int(ecs_vec_first_t(b, int32_t)[1], 2);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void AsyncBlock_assign_string_element(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut names: string[] = [\"a\", \"b\"]"
            LINE "async {"
            LINE "  names[1] = \"long string value\""
            LINE "  names[0] = names[1]"
            LINE "  names[0] = names[0]"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ecs_vec_t *v = ecs_mut_var_get(world, ecs_lookup(world, "names")).ptr;
    test_str(ecs_vec_first_t(v, char*)[0], "long string value");
    test_str(ecs_vec_first_t(v, char*)[1], "long string value");

    ecs_fini(world);
}

void AsyncBlock_declare_array_w_type(void) {
    ecs_world_t *world = ecs_init();

    test_assert(ecs_script_run_w_desc(world, NULL,
        HEAD "struct Pair(a: i32, b: i32)"
        LINE "export mut ships: Pair[10] = {}"
        LINE "export mut vec: Pair[] = []"
        LINE "export const fixed: i32[2] = [1, 2]",
        &ir_desc, NULL) == 0);

    ecs_value_t ships = ecs_mut_var_get(world, ecs_lookup(world, "ships"));
    const EcsArray *array = ecs_get(world, ships.type, EcsArray);
    test_assert(array != NULL);
    test_int(array->count, 10);
    test_uint(array->type, ecs_lookup(world, "Pair"));

    ecs_value_t vec = ecs_mut_var_get(world, ecs_lookup(world, "vec"));
    const EcsVector *vector = ecs_get(world, vec.type, EcsVector);
    test_assert(vector != NULL);
    test_uint(vector->type, ecs_lookup(world, "Pair"));
    test_int(ecs_vec_count((ecs_vec_t*)vec.ptr), 0);

    ecs_value_t fixed = ecs_const_var_get(world, ecs_lookup(world, "fixed"));
    test_int(((int32_t*)fixed.ptr)[1], 2);

    ecs_fini(world);
}

void AsyncBlock_struct_reset_w_empty_initializer(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "struct Pair(a: i32, b: i32)"
            LINE "export mut st: Pair = {1, 2}"
            LINE "export mut v: i32[2] = [3, 4]"
            LINE "async {"
            LINE "  st = {}"
            LINE "  v = {}"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    const ab_Pair *st = ecs_mut_var_get(world, ecs_lookup(world, "st")).ptr;
    test_int(st->a, 0);
    test_int(st->b, 0);
    const int32_t *v = ecs_mut_var_get(world, ecs_lookup(world, "v")).ptr;
    test_int(v[0], 0);
    test_int(v[1], 0);

    ecs_fini(world);
}

void AsyncBlock_error_assign_target(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    const char *scripts[] = {
        HEAD "struct Pair(a: i32, b: i32)\nexport mut x: Pair = {1, 2}\nasync { x.c = 1 }",
        HEAD "export mut x: i32 = 0\nasync { x.c = 1 }",
        HEAD "export mut x: i32 = 0\nasync { x[0] = 1 }",
        HEAD "export mut x: i32[2] = {}\nasync { x[\"foo\"] = 1 }",
        HEAD "export mut x: i32[2] = {}\nasync { x[0].y = 1 }",
        HEAD "export mut x: i32[2] = {}\nasync { x[0] = \"foo\" }",
        HEAD "export mut x: i32[0] = {}",
        HEAD "export mut x: i32[-1] = {}",
        HEAD "export mut x: i32[abc] = {}",
        HEAD "export const x: i32[2] = {}\nasync { x[0] = 1 }",
        HEAD "async { y[0] = 1 }",
        HEAD "y {}\nasync { y.z = 1 }",
        HEAD "struct Pos(x: i32)\ny { Pos: {0} }\nasync { y[Pos].x = 2 }",
        HEAD "export mut x: i32[2] = {}\nasync { x[0][0][0][0][0][0][0][0][0] = 1 }",
    };

    for (int32_t i = 0; i < 14; i ++) {
        test_assert(ecs_script_run_w_desc(
            world, NULL, scripts[i], &ir_desc, NULL) != 0);
    }

    ecs_fini(world);
}

void AsyncBlock_assign_template_and_parent_mut_member(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t position = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "template P {"
            LINE "  mut pos: Position = {1, 2}"
            LINE "  mut own: Position = {3, 4}"
            LINE "  Position: {pos.x + own.x, pos.y + own.y}"
            LINE "  async {"
            LINE "    own.y = 40"
            LINE "  }"
            LINE "}"
            LINE "template Child : parent P {"
            LINE "  async {"
            LINE "    parent.pos.x = 10"
            LINE "    pos.y += 18"
            LINE "  }"
            LINE "}"
            LINE "P p() {"
            LINE "  Child c()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_progress(world, 0);

    const ab_Position *p = ecs_get_id(world, ecs_lookup(world, "p"), position);
    test_assert(p != NULL);
    test_int(p->x, 13);
    test_int(p->y, 60);

    ecs_fini(world);
}

void AsyncBlock_next_frame(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut count: i32 = 0"
            LINE "async {"
            LINE "  while true {"
            LINE "    await next_frame()"
            LINE "    count += 1"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 0);

    ecs_progress(world, 0.1f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 0);

    ecs_progress(world, 0.1f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 1);

    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 1);

    ecs_progress(world, 0.1f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 2);

    ecs_progress(world, 0.1f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 3);

    ecs_fini(world);
}

void AsyncBlock_delay(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "async {"
            LINE "  await delay(1.5)"
            LINE "  done = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);
    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);
    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);
    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 1);

    ecs_fini(world);
}

void AsyncBlock_delay_large_step(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "async {"
            LINE "  await next_frame()"
            LINE "  await delay(1.5)"
            LINE "  done = 1"
            LINE "  await delay(0.25)"
            LINE "  done = 2"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);
    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);

    ecs_progress(world, 100);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 1);

    ecs_progress(world, 100);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_delay_zero_and_negative(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut zero: i32 = 0"
            LINE "export mut neg: i32 = 0"
            LINE "async { while true { await delay(0); zero += 1 } }"
            LINE "async { while true { await delay(-1); neg += 1 } }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "zero")).ptr, 0);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "neg")).ptr, 0);

    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "zero")).ptr, 1);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "neg")).ptr, 1);

    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "zero")).ptr, 2);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "neg")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_delay_time_scale(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "export mut frames: i32 = 0"
            LINE "async {"
            LINE "  await delay(1)"
            LINE "  done = 1"
            LINE "}"
            LINE "async {"
            LINE "  while true {"
            LINE "    await next_frame()"
            LINE "    frames += 1"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "frames")).ptr, 0);

    ecs_set_time_scale(world, 0);
    for (int32_t i = 0; i < 10; i ++) {
        ecs_progress(world, 0.5f);
    }

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "frames")).ptr, 10);

    ecs_set_time_scale(world, 1);
    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);
    ecs_progress(world, 0.5f);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 1);

    ecs_fini(world);
}

void AsyncBlock_delay_time_scale_speedup(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "async {"
            LINE "  await delay(4)"
            LINE "  done = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 1);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);

    ecs_set_time_scale(world, 4);
    ecs_progress(world, 1);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 1);

    ecs_fini(world);
}

void AsyncBlock_delay_cancel_on_restart(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "template T {"
            LINE "  prop v: i32 = 1"
            LINE "  async {"
            LINE "    await delay(v)"
            LINE "    done += v"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_entity_t e = ecs_lookup(world, "e");
    ecs_entity_t t = ecs_lookup(world, "T");

    ecs_progress(world, 0.5f);

    int32_t v = 2;
    ecs_set_id(world, e, t, sizeof(v), &v);

    ecs_progress(world, 1);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);

    ecs_progress(world, 1);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);

    ecs_progress(world, 1);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_delay_cancel_on_delete(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "template T {"
            LINE "  async {"
            LINE "    await next_frame()"
            LINE "    done = 1"
            LINE "  }"
            LINE "  child {"
            LINE "    async {"
            LINE "      await delay(1)"
            LINE "      done = 2"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "T e",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);

    ecs_delete(world, ecs_lookup(world, "e"));

    for (int32_t i = 0; i < 5; i ++) {
        ecs_progress(world, 1);
    }

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "done")).ptr, 0);

    ecs_fini(world);
}

void AsyncBlock_delay_pending_at_fini(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "async {"
            LINE "  await delay(100)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);

    ecs_fini(world);
}

void AsyncBlock_delay_cancel_on_script_delete(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "using flecs.script"
            LINE "export mut done: i32 = 0"
            LINE "async {"
            LINE "  await delay(1)"
            LINE "  done = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_progress(world, 0.5f);

    ecs_delete(world, s);
    ecs_progress(world, 10);
    ecs_progress(world, 10);

    ecs_fini(world);
}

void AsyncBlock_proc_call(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut text: string = \"\""
            LINE "export mut count: i32 = 0"
            LINE "proc alert(message: string, n: i32) {"
            LINE "  text = message"
            LINE "  count += n"
            LINE "}"
            LINE "async {"
            LINE "  alert(\"foo\", 1)"
            LINE "  alert(\"bar\", 2)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_str(*(char**)ecs_mut_var_get(world, ecs_lookup(world, "text")).ptr, "bar");
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 3);

    ecs_fini(world);
}

void AsyncBlock_proc_no_args(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut count: i32 = 0"
            LINE "proc tick() { count += 1 }"
            LINE "async { tick(); tick() }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_proc_control_flow(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "proc sum(limit: i32) {"
            LINE "  const start = 1"
            LINE "  mut total: i32 = start"
            LINE "  for i in 0..10 {"
            LINE "    if i == limit {"
            LINE "      break"
            LINE "    }"
            LINE "    if i == 1 {"
            LINE "      continue"
            LINE "    }"
            LINE "    total += i"
            LINE "  }"
            LINE "  mut n: i32 = 0"
            LINE "  while n < 2 {"
            LINE "    n += 1"
            LINE "    total *= 2"
            LINE "  }"
            LINE "  out = total"
            LINE "}"
            LINE "async { sum(4) }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 24);

    ecs_fini(world);
}

void AsyncBlock_proc_call_proc(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "proc add(v: i32) { out += v }"
            LINE "proc add_twice(v: i32) {"
            LINE "  add(v)"
            LINE "  add(v * 2)"
            LINE "}"
            LINE "async { add_twice(3) }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 9);

    ecs_fini(world);
}

void AsyncBlock_proc_recursion(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "proc count(n: i32) {"
            LINE "  if n > 0 {"
            LINE "    out += 1"
            LINE "    count(n - 1)"
            LINE "  }"
            LINE "}"
            LINE "async { count(10) }",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 10);

    ecs_fini(world);
}

void AsyncBlock_proc_infinite_recursion(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "export mut after: i32 = 0"
            LINE "proc loop() {"
            LINE "  out += 1"
            LINE "  loop()"
            LINE "}"
            LINE "async {"
            LINE "  loop()"
            LINE "  after = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 64);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void AsyncBlock_proc_error_stops_caller(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut v: i32[2] = {}"
            LINE "export mut after: i32 = 0"
            LINE "proc write(i: i32) {"
            LINE "  v[i] = 1"
            LINE "}"
            LINE "async {"
            LINE "  write(5)"
            LINE "  after = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    test_assert(ecs_get(world, s, EcsScript)->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 0);

    ecs_fini(world);
}

void AsyncBlock_proc_from_template_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "export mut out: i32 = 0"
            LINE "proc add(v: i32) { out += v }"
            LINE "template T {"
            LINE "  prop p: i32 = 0"
            LINE "  async {"
            LINE "    add(p)"
            LINE "  }"
            LINE "}"
            LINE "T a(p: 2)"
            LINE "T b(p: 3)",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 5);

    ecs_fini(world);
}

void AsyncBlock_error_proc(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    const char *scripts[] = {
        HEAD "proc p() { await fetch(1) }",
        HEAD "proc p() { const x = await fetch(1) }",
        HEAD "proc p() { try { } catch { } }",
        HEAD "proc p() { }\nexport const x: bool = p()",
        HEAD "export mut m: i32 = 0\nproc p() { }\nasync { m = p() }",
        HEAD "proc p() { }\np()",
        HEAD "proc p() { }\ntemplate T { p() }",
        HEAD "proc p() { }\ntemplate T { const x: bool = p() }",
        HEAD "fn f() -> i32 { 1 }\nasync { f() }",
        HEAD "async { unknown_proc() }",
        HEAD "proc p(a: i32) { }\nasync { p() }",
        HEAD "proc p(a: i32) { }\nasync { p(1, 2) }",
        HEAD "proc p() { }\nasync { p() + 1 }",
        HEAD "proc p() { continue }",
        HEAD "proc p(a: i32) { a = 1 }",
        HEAD "template T { proc p() { } }",
    };

    for (int32_t i = 0; i < 16; i ++) {
        test_assert(ecs_script_run_w_desc(
            world, NULL, scripts[i], &ir_desc, NULL) != 0);
    }

    ecs_fini(world);
}

void AsyncBlock_event_emit_await(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut got: i32 = 0"
            LINE "async {"
            LINE "  const e = await Build"
            LINE "  got = e.slot"
            LINE "}"
            LINE "async {"
            LINE "  emit Build(slot: 7)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "got")).ptr, 7);

    ecs_fini(world);
}

void AsyncBlock_event_listener_after_emitter(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut got: i32 = 0"
            LINE "async {"
            LINE "  emit Build(slot: 7)"
            LINE "}"
            LINE "async {"
            LINE "  const e = await Build"
            LINE "  got = e.slot"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "got")).ptr, 7);

    ecs_fini(world);
}

void AsyncBlock_event_emit_does_not_pause(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut after: i32 = 0"
            LINE "async {"
            LINE "  emit Build(slot: 1)"
            LINE "  after = 1"
            LINE "  emit Build(slot: 2)"
            LINE "  after = 2"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_event_no_listeners(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut after: i32 = 0"
            LINE "async {"
            LINE "  emit Build(slot: 1)"
            LINE "  after = 1"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "after")).ptr, 1);

    ecs_fini(world);
}

void AsyncBlock_event_queued_in_order(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut out: i32 = 0"
            LINE "export mut count: i32 = 0"
            LINE "async {"
            LINE "  while true {"
            LINE "    const e = await Build"
            LINE "    out = out * 10 + e.slot"
            LINE "    count += 1"
            LINE "  }"
            LINE "}"
            LINE "async {"
            LINE "  emit Build(slot: 1)"
            LINE "  emit Build(slot: 2)"
            LINE "  emit Build(slot: 3)"
            LINE "  emit Build(slot: 4)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    for (int32_t i = 0; i < 6; i ++) {
        ecs_script_tasks_progress(world);
    }

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 4);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 1234);

    ecs_fini(world);
}

void AsyncBlock_event_busy_block_receives_queued(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut out: i32 = 0"
            LINE "async {"
            LINE "  const r = await fetch(0)"
            LINE "  while true {"
            LINE "    const e = await Build"
            LINE "    out = out * 10 + e.slot"
            LINE "  }"
            LINE "}"
            LINE "async {"
            LINE "  emit Build(slot: 1)"
            LINE "  emit Build(slot: 2)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    test_assert(ecs_get(world, s, EcsScript)->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 0);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, 12);

    ecs_fini(world);
}

void AsyncBlock_event_to_template_blocks(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut total: i32 = 0"
            LINE "template T {"
            LINE "  prop factor: i32 = 1"
            LINE "  mut last: i32 = 0"
            LINE "  async {"
            LINE "    const e = await Build"
            LINE "    total += e.slot * factor"
            LINE "  }"
            LINE "}"
            LINE "T a(factor: 2)"
            LINE "T b(factor: 3)"
            LINE "async {"
            LINE "  emit Build(slot: 5)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "total")).ptr, 25);

    ecs_fini(world);
}

void AsyncBlock_event_from_template_block(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut total: i32 = 0"
            LINE "template T {"
            LINE "  prop idx: i32 = 1"
            LINE "  async {"
            LINE "    emit Build(slot: idx)"
            LINE "  }"
            LINE "}"
            LINE "T a(idx: 2)"
            LINE "T b(idx: 3)"
            LINE "async {"
            LINE "  while true {"
            LINE "    const e = await Build"
            LINE "    total += e.slot"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    for (int32_t i = 0; i < 3; i ++) {
        ecs_script_tasks_progress(world);
    }

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "total")).ptr, 5);

    ecs_fini(world);
}

void AsyncBlock_event_string_payload(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Msg(text: string, n: i32)"
            LINE "export mut out: string = \"\""
            LINE "async {"
            LINE "  const a = await Msg"
            LINE "  const b = await Msg"
            LINE "  out = b.text"
            LINE "}"
            LINE "async {"
            LINE "  emit Msg(text: \"foo\", n: 1)"
            LINE "  emit Msg(text: \"bar\", n: 2)"
            LINE "  emit Msg(text: \"never received\", n: 3)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    for (int32_t i = 0; i < 4; i ++) {
        ecs_script_tasks_progress(world);
    }

    test_str(*(char**)ecs_mut_var_get(world, ecs_lookup(world, "out")).ptr, "bar");

    ecs_fini(world);
}

void AsyncBlock_event_no_members(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Ping()"
            LINE "export mut count: i32 = 0"
            LINE "async {"
            LINE "  while true {"
            LINE "    await Ping"
            LINE "    count += 1"
            LINE "  }"
            LINE "}"
            LINE "async {"
            LINE "  emit Ping"
            LINE "  emit Ping()"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    for (int32_t i = 0; i < 4; i ++) {
        ecs_script_tasks_progress(world);
    }

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_event_self_emit_does_not_hang(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Tick(n: i32)"
            LINE "export mut count: i32 = 0"
            LINE "async {"
            LINE "  emit Tick(n: 0)"
            LINE "  while true {"
            LINE "    const e = await Tick"
            LINE "    count = e.n + 1"
            LINE "    emit Tick(n: count)"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 0);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 1);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 2);

    ecs_fini(world);
}

void AsyncBlock_event_from_proc(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut got: i32 = 0"
            LINE "proc build(i: i32) {"
            LINE "  emit Build(slot: i * 2)"
            LINE "}"
            LINE "async {"
            LINE "  const e = await Build"
            LINE "  got = e.slot"
            LINE "}"
            LINE "async {"
            LINE "  build(4)"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);

    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "got")).ptr, 8);

    ecs_fini(world);
}

void AsyncBlock_event_queue_limit(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Build(slot: i32)"
            LINE "export mut count: i32 = 0"
            LINE "export mut sent: i32 = 0"
            LINE "async {"
            LINE "  const r = await fetch(0)"
            LINE "  while true {"
            LINE "    const e = await Build"
            LINE "    count += 1"
            LINE "  }"
            LINE "}"
            LINE "async {"
            LINE "  for i in 0..2000 {"
            LINE "    emit Build(slot: i)"
            LINE "    sent += 1"
            LINE "  }"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    test_assert(ecs_get(world, s, EcsScript)->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "sent")).ptr, 2000);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[0], &v), 0);
    ecs_script_future_release(ab_futures[0]);

    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 1024);

    ecs_fini(world);
}

void AsyncBlock_event_cancel_drops_queue(void) {
    ecs_world_t *world = ecs_init();

    ecs_os_zeromem(ab_futures);
    ab_future_count = 0;
    ab_cancel_count = 0;

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"v", ecs_id(ecs_i32_t)}},
        .callback = ab_fetch_callback,
        .cancel = ab_cancel_callback
    });

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Msg(text: string)"
            LINE "export mut count: i32 = 0"
            LINE "template T {"
            LINE "  prop v: i32 = 0"
            LINE "  async {"
            LINE "    const r = await fetch(v)"
            LINE "    while true {"
            LINE "      const e = await Msg"
            LINE "      count += 1"
            LINE "    }"
            LINE "  }"
            LINE "}"
            LINE "T a"
            LINE "async {"
            LINE "  emit Msg(text: \"queued\")"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    test_assert(ecs_get(world, s, EcsScript)->error == NULL);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 1);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t t = ecs_lookup(world, "T");
    int32_t prop = 1;
    ecs_set_id(world, a, t, sizeof(prop), &prop);
    test_int(ab_cancel_count, 1);

    ecs_script_tasks_progress(world);
    test_int(ab_future_count, 2);

    ecs_value_t v = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(ab_futures[1], &v), 0);
    ecs_script_tasks_progress(world);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 0);

    ecs_script_future_release(ab_futures[0]);
    ecs_script_future_release(ab_futures[1]);

    ecs_fini(world);
}

void AsyncBlock_event_cancel_while_waiting(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t s = ecs_script(world, {
        .code = HEAD "event Msg(text: string)"
            LINE "export mut count: i32 = 0"
            LINE "template T {"
            LINE "  prop v: i32 = 0"
            LINE "  async {"
            LINE "    const e = await Msg"
            LINE "    count += v + 1"
            LINE "  }"
            LINE "}"
            LINE "T a"
            LINE "async {"
            LINE "  const r = await Msg"
            LINE "  emit Msg(text: \"second\")"
            LINE "}",
        .ir = ir_enabled
    });
    test_assert(s != 0);
    const EcsScript *comp = ecs_get(world, s, EcsScript);
    test_assert(comp != NULL);
    test_assert(comp->error == NULL);

    ecs_script_tasks_progress(world);

    ecs_entity_t a = ecs_lookup(world, "a");
    ecs_entity_t t = ecs_lookup(world, "T");
    int32_t prop = 1;
    ecs_set_id(world, a, t, sizeof(prop), &prop);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 0);

    ecs_delete(world, a);
    ecs_script_tasks_progress(world);
    test_int(*(int32_t*)ecs_mut_var_get(world, ecs_lookup(world, "count")).ptr, 0);

    ecs_fini(world);
}

void AsyncBlock_error_event(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);

    const char *scripts[] = {
        HEAD "async { emit Unknown(a: 1) }",
        HEAD "struct S(a: i32)\nasync { emit S(a: 1) }",
        HEAD "event E(a: i32)\nasync { emit E(b: 1) }",
        HEAD "event E(a: i32)\nasync { emit E(a: \"foo\") }",
        HEAD "event NoMembers()\nasync { emit NoMembers(1) }",
        HEAD "event E(a: i32)\nemit E(a: 1)",
        HEAD "async { await Unknown }",
        HEAD "struct S(a: i32)\nasync { await S }",
        HEAD "event E(a: i32)\nconst x = E()",
    };

    for (int32_t i = 0; i < 9; i ++) {
        test_assert(ecs_script_run_w_desc(
            world, NULL, scripts[i], &ir_desc, NULL) != 0);
    }

    ecs_fini(world);
}
