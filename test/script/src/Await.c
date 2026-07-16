#include <script.h>
#include "../../../src/addons/script/script.h"

static
void Await_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    (void)future;
}

static ecs_script_future_t *await_futures[64];
static int32_t await_future_count;
static int32_t await_cancel_count;
static int32_t await_args[64];
static char *await_strings[64];
static ecs_entity_t await_functions[64];
static ecs_entity_t await_entities[64];
static ecs_entity_t await_entity_args[64][4];
static int32_t await_i32_args[64][4];

typedef struct AwaitJob {
    ecs_entity_t resource;
    int32_t amount;
    ecs_entity_t src;
    ecs_entity_t dst;
} AwaitJob;

typedef struct AwaitDrone {
    ecs_entity_t home;
} AwaitDrone;

typedef struct AwaitTracked {
    int32_t value;
} AwaitTracked;

static int32_t await_tracked_ctor_count;
static int32_t await_tracked_dtor_count;
static int32_t await_future_ctx_free_count;
static int32_t await_future_ctx;
static int64_t await_method_instance;

ECS_CTOR(AwaitTracked, ptr, {
    ptr->value = 0;
    await_tracked_ctor_count ++;
})

ECS_DTOR(AwaitTracked, ptr, {
    (void)ptr;
    await_tracked_dtor_count ++;
})

static
void Await_store_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;

    if (argc) {
        await_args[await_future_count] = *(int32_t*)argv[0].ptr;
    }

    await_futures[await_future_count ++] = future;
}

static
void Await_store_string_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;

    test_int(argc, 1);
    await_strings[await_future_count] = ecs_os_strdup(
        *(ecs_string_t*)argv[0].ptr);
    await_futures[await_future_count ++] = future;
}

static
void Await_record_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    int32_t index = await_future_count;

    await_functions[index] = ctx->function;
    await_entities[index] = ctx->entity;

    for (int32_t i = 0; i < argc; i ++) {
        if (argv[i].type == ecs_id(ecs_entity_t)) {
            await_entity_args[index][i] = *(ecs_entity_t*)argv[i].ptr;
        } else if (argv[i].type == ecs_id(ecs_i32_t)) {
            await_i32_args[index][i] = *(int32_t*)argv[i].ptr;
        }
    }

    await_futures[await_future_count ++] = future;
}

static
void Await_immediate_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    (void)argv;

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    ecs_script_future_resolve(future, &value);
    ecs_script_future_release(future);
    await_future_count ++;
}

static
void Await_method_placeholder(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_value_t *result)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    (void)result;
}

static
void Await_method_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;

    test_int(argc, 1);
    await_method_instance = *(int64_t*)argv[0].ptr;
    await_futures[await_future_count ++] = future;
}

static
void Await_cancel_callback(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)future;
    await_cancel_count ++;
}

static
void Await_future_ctx_free(
    void *ctx)
{
    test_assert(ctx == &await_future_ctx);
    await_future_ctx_free_count ++;
}

static
void Await_store_with_ctx_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    Await_store_callback(ctx, argc, argv, future);
    ecs_script_future_set_ctx(
        future, &await_future_ctx, Await_future_ctx_free);
}

static
void Await_reset(void)
{
    for (int32_t i = 0; i < 64; i ++) {
        ecs_os_free(await_strings[i]);
    }

    ecs_os_zeromem(await_futures);
    await_future_count = 0;
    await_cancel_count = 0;
    await_tracked_ctor_count = 0;
    await_tracked_dtor_count = 0;
    await_method_instance = 0;
    ecs_os_zeromem(await_args);
    ecs_os_zeromem(await_strings);
    ecs_os_zeromem(await_functions);
    ecs_os_zeromem(await_entities);
    ecs_os_zeromem(await_entity_args);
    ecs_os_zeromem(await_i32_args);
}

void Await_parse_await_const(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "const value = await fetch()", NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(strstr(str, "const") != NULL);
    test_assert(strstr(str, "value") != NULL);
    test_assert(strstr(str, "await") != NULL);
    test_assert(strstr(str, "fetch") != NULL);

    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void Await_parse_await_stmt(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fetch()", NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(strstr(str, "await") != NULL);
    test_assert(strstr(str, "fetch") != NULL);

    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void Await_register_async(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t function = ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_callback
    });

    test_assert(function != 0);

    const EcsScriptFunction *f = ecs_get(
        world, function, EcsScriptFunction);
    test_assert(f != NULL);
    test_assert(f->callback == NULL);
    test_assert(f->async_callback == Await_callback);

    ecs_fini(world);
}

void Await_await_const_suspend_resume(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "const value: await fetch()\n"
        "Foo = Position: {$value, 20}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);
    test_assert(task != NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);
    test_assert(ecs_lookup(world, "Foo") == 0);

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(await_futures[0], &value), 0);
    ecs_script_future_release(await_futures[0]);
    test_bool(ecs_script_task_is_ready(task), true);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 1);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_stmt_sequence(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "await step()\n"
        "await step()\n"
        "await step()", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);

    for (int32_t i = 0; i < 3; i ++) {
        ecs_value_t value = ecs_value(ecs_i32_t, {i});
        test_int(ecs_script_future_resolve(await_futures[i], &value), 0);
        ecs_script_future_release(await_futures[i]);

        ecs_script_task_status_t status =
            ecs_script_task_resume(task, NULL);
        test_int(status, i == 2
            ? EcsScriptTaskDone
            : EcsScriptTaskPending);
        test_int(await_future_count, i == 2 ? 3 : i + 2);
    }

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_reject(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fetch()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(await_futures[0], "failed"), 0);
    ecs_script_future_release(await_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "failed") != NULL);
    ecs_os_free(result.error);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_cancel(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback,
        .cancel = Await_cancel_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fetch()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_script_task_free(task);
    test_int(await_cancel_count, 1);
    test_bool(ecs_script_future_is_cancelled(await_futures[0]), true);
    ecs_script_future_release(await_futures[0]);

    ecs_script_free(script);
    ecs_fini(world);
}

void Await_preserve_const_and_using(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t ns = ecs_entity(world, { .name = "ns" });
    ecs_entity_t tag = ecs_entity(world, {
        .name = "Tag",
        .parent = ns
    });

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_async_function(world, {
        .name = "use",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"value", ecs_id(ecs_i32_t)}},
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "using ns\n"
        "const value: 10\n"
        "await step()\n"
        "Foo { Tag }\n"
        "await use($value)", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(await_futures[0], &value), 0);
    ecs_script_future_release(await_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 2);
    test_int(await_args[1], 10);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has_id(world, foo, tag));

    test_int(ecs_script_future_resolve(await_futures[1], &value), 0);
    ecs_script_future_release(await_futures[1]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_in_if(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "if true {\n"
        "  await step()\n"
        "  Foo {}\n"
        "}", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_assert(ecs_lookup(world, "Foo") == 0);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_in_for(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "use",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"value", ecs_id(ecs_i32_t)}},
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "for i in 0..3 { await use($i) }", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    for (int32_t i = 0; i < 3; i ++) {
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_int(await_args[i], i);
        ecs_value_t value = ecs_value(ecs_i32_t, {i});
        ecs_script_future_resolve(await_futures[i], &value);
        ecs_script_future_release(await_futures[i]);
    }

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 3);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_in_with(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ECS_TAG(world, Tag);

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "with Tag {\n"
        "  await step()\n"
        "  Foo {}\n"
        "}", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    test_assert(foo != 0);
    test_assert(ecs_has(world, foo, Tag));

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_in_entity(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "Parent {\n"
        "  await step()\n"
        "  Child {}\n"
        "}", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_entity_t parent = ecs_lookup(world, "Parent");
    test_assert(parent != 0);
    test_assert(ecs_lookup_child(world, parent, "Child") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_in_pair_scope(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ECS_TAG(world, Rel);

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "(Rel, Tgt) {\n"
        "  await step()\n"
        "  Foo {}\n"
        "}", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    ecs_entity_t tgt = ecs_lookup(world, "Tgt");
    test_assert(foo != 0);
    test_assert(tgt != 0);
    test_assert(ecs_has_pair(world, foo, Rel, tgt));

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_preserve_default_component(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "with Position {\n"
        "  await step()\n"
        "  if true {\n"
        "    Foo = 10, 20\n"
        "  }\n"
        "}", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_entity_t foo = ecs_lookup(world, "Foo");
    const Position *p = ecs_get(world, foo, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_preserve_string_const(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_async_function(world, {
        .name = "use",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"value", ecs_id(ecs_string_t)}},
        .callback = Await_store_string_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "const value: \"hello\"\n"
        "await step()\n"
        "await use($value)", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_str(await_strings[1], "hello");

    ecs_script_future_resolve(await_futures[1], &value);
    ecs_script_future_release(await_futures[1]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);

    Await_reset();

    ecs_fini(world);
}

void Await_owner_context(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t ecs_id(AwaitDrone) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "AwaitDrone" }),
        .members = {{"home", ecs_id(ecs_entity_t)}}
    });

    ecs_entity_t home = ecs_entity(world, { .name = "Home" });
    ecs_entity_t drone = ecs_entity(world, { .name = "Drone1" });
    ecs_set(world, drone, AwaitDrone, {home});

    ecs_entity_t move = ecs_async_function(world, {
        .name = "moveTo",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"dst", ecs_id(ecs_entity_t)}},
        .callback = Await_record_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "await moveTo(this[AwaitDrone].home)", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .entity = drone });
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_uint(await_functions[0], move);
    test_uint(await_entities[0], drone);
    test_uint(await_entity_args[0][0], home);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_qualified_call(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t logistics = ecs_entity(world, { .name = "logistics" });
    ecs_entity_t accept = ecs_async_function(world, {
        .name = "acceptJob",
        .parent = logistics,
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_record_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "const job = await logistics.acceptJob()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_uint(await_functions[0], accept);

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    ecs_script_future_resolve(await_futures[0], &value);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

typedef struct AwaitHaulFixture {
    ecs_world_t *world;
    ecs_entity_t job_type;
    ecs_entity_t drone_type;
    ecs_entity_t functions[6];
} AwaitHaulFixture;

static const char *Await_haul_code =
    "const job = await logistics.acceptJob()\n"
    "await takeOff()\n"
    "await moveTo(job.src)\n"
    "await land()\n"
    "await pickUp(job.resource, job.amount, job.src, this)\n"
    "await takeOff()\n"
    "await moveTo(job.dst)\n"
    "await land()\n"
    "await dropOff(job.resource, job.amount, this, job.dst)\n"
    "await takeOff()\n"
    "await moveTo(this[AwaitDrone].home)\n"
    "await land()";

static
AwaitHaulFixture Await_haul_fixture(
    ecs_world_t *world)
{
    AwaitHaulFixture result = { .world = world };
    result.job_type = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "AwaitJob" }),
        .members = {
            {"resource", ecs_id(ecs_entity_t)},
            {"amount", ecs_id(ecs_i32_t)},
            {"src", ecs_id(ecs_entity_t)},
            {"dst", ecs_id(ecs_entity_t)}
        }
    });

    result.drone_type = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "AwaitDrone" }),
        .members = {{"home", ecs_id(ecs_entity_t)}}
    });

    ecs_entity_t logistics = ecs_entity(world, { .name = "logistics" });
    result.functions[0] = ecs_async_function(world, {
        .name = "acceptJob",
        .parent = logistics,
        .return_type = result.job_type,
        .callback = Await_record_callback
    });

    result.functions[1] = ecs_async_function(world, {
        .name = "takeOff",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_record_callback
    });

    result.functions[2] = ecs_async_function(world, {
        .name = "moveTo",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"dst", ecs_id(ecs_entity_t)}},
        .callback = Await_record_callback
    });

    result.functions[3] = ecs_async_function(world, {
        .name = "land",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_record_callback
    });

    result.functions[4] = ecs_async_function(world, {
        .name = "pickUp",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            {"resource", ecs_id(ecs_entity_t)},
            {"amount", ecs_id(ecs_i32_t)},
            {"src", ecs_id(ecs_entity_t)},
            {"drone", ecs_id(ecs_entity_t)}
        },
        .callback = Await_record_callback
    });

    result.functions[5] = ecs_async_function(world, {
        .name = "dropOff",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            {"resource", ecs_id(ecs_entity_t)},
            {"amount", ecs_id(ecs_i32_t)},
            {"drone", ecs_id(ecs_entity_t)},
            {"dst", ecs_id(ecs_entity_t)}
        },
        .callback = Await_record_callback
    });

    return result;
}

static
ecs_entity_t Await_haul_owner(
    const AwaitHaulFixture *fixture,
    const char *name,
    ecs_entity_t home)
{
    ecs_entity_t result = ecs_entity(fixture->world, { .name = name });
    AwaitDrone value = {home};

    ecs_set_id(fixture->world, result, fixture->drone_type,
        ECS_SIZEOF(AwaitDrone), &value);

    return result;
}

static
ecs_entity_t Await_haul_function(
    const AwaitHaulFixture *fixture,
    int32_t step)
{
    int32_t indices[12] = {0, 1, 2, 3, 4, 1, 2, 3, 5, 1, 2, 3};
    return fixture->functions[indices[step % 12]];
}

static
void Await_haul_resolve(
    const AwaitHaulFixture *fixture,
    int32_t future,
    int32_t step,
    AwaitJob *job)
{
    if (!(step % 12)) {
        ecs_value_t value = {fixture->job_type, job};
        test_int(ecs_script_future_resolve(
            await_futures[future], &value), 0);
    } else {
        ecs_value_t value = ecs_value(ecs_i32_t, {0});
        test_int(ecs_script_future_resolve(
            await_futures[future], &value), 0);
    }

    ecs_script_future_release(await_futures[future]);
}

static
void Await_haul_validate(
    int32_t base,
    ecs_entity_t owner,
    ecs_entity_t home,
    const AwaitJob *job)
{
    test_uint(await_entity_args[base + 2][0], job->src);
    test_uint(await_entity_args[base + 4][0], job->resource);
    test_int(await_i32_args[base + 4][1], job->amount);
    test_uint(await_entity_args[base + 4][2], job->src);
    test_uint(await_entity_args[base + 4][3], owner);
    test_uint(await_entity_args[base + 6][0], job->dst);
    test_uint(await_entity_args[base + 8][0], job->resource);
    test_int(await_i32_args[base + 8][1], job->amount);
    test_uint(await_entity_args[base + 8][2], owner);
    test_uint(await_entity_args[base + 8][3], job->dst);
    test_uint(await_entity_args[base + 10][0], home);
}

void Await_haul_single(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    AwaitHaulFixture fixture = Await_haul_fixture(world);

    ecs_entity_t resource = ecs_entity(world, { .name = "Iron" });
    ecs_entity_t src = ecs_entity(world, { .name = "Mine" });
    ecs_entity_t dst = ecs_entity(world, { .name = "Factory" });
    ecs_entity_t home = ecs_entity(world, { .name = "Home" });
    ecs_entity_t owner = Await_haul_owner(&fixture, "Drone1", home);
    AwaitJob job = {resource, 20, src, dst};

    ecs_script_t *script = ecs_script_parse(
        world, NULL, Await_haul_code, NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .entity = owner });

    for (int32_t i = 0; i < 12; i ++) {
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_uint(await_functions[i], Await_haul_function(&fixture, i));
        test_uint(await_entities[i], owner);
        Await_haul_resolve(&fixture, i, i, &job);
    }

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 12);
    Await_haul_validate(0, owner, home, &job);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_haul_loop_3(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    AwaitHaulFixture fixture = Await_haul_fixture(world);

    ecs_entity_t home = ecs_entity(world, { .name = "Home" });
    ecs_entity_t owner = Await_haul_owner(&fixture, "Drone1", home);
    AwaitJob jobs[3] = {
        {
            ecs_entity(world, { .name = "Iron" }), 20,
            ecs_entity(world, { .name = "Mine" }),
            ecs_entity(world, { .name = "Factory" })
        }, {
            ecs_entity(world, { .name = "Copper" }), 30,
            ecs_entity(world, { .name = "Quarry" }),
            ecs_entity(world, { .name = "Foundry" })
        }, {
            ecs_entity(world, { .name = "Water" }), 40,
            ecs_entity(world, { .name = "Well" }),
            ecs_entity(world, { .name = "Reservoir" })
        }
    };

    const char *code =
        "for i in 0..3 {\n"
        "  const job = await logistics.acceptJob()\n"
        "  await takeOff()\n"
        "  await moveTo(job.src)\n"
        "  await land()\n"
        "  await pickUp(job.resource, job.amount, job.src, this)\n"
        "  await takeOff()\n"
        "  await moveTo(job.dst)\n"
        "  await land()\n"
        "  await dropOff(job.resource, job.amount, this, job.dst)\n"
        "  await takeOff()\n"
        "  await moveTo(this[AwaitDrone].home)\n"
        "  await land()\n"
        "}";

    ecs_script_t *script = ecs_script_parse(world, NULL, code, NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .entity = owner });

    for (int32_t i = 0; i < 36; i ++) {
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_uint(await_functions[i], Await_haul_function(&fixture, i));
        test_uint(await_entities[i], owner);
        Await_haul_resolve(&fixture, i, i, &jobs[i / 12]);
    }

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 36);
    Await_haul_validate(0, owner, home, &jobs[0]);
    Await_haul_validate(12, owner, home, &jobs[1]);
    Await_haul_validate(24, owner, home, &jobs[2]);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_haul_loop_count(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    AwaitHaulFixture fixture = Await_haul_fixture(world);

    ecs_entity_t home = ecs_entity(world, { .name = "Home" });
    ecs_entity_t owner = Await_haul_owner(&fixture, "Drone1", home);
    AwaitJob jobs[3] = {
        {
            ecs_entity(world, { .name = "Iron" }), 20,
            ecs_entity(world, { .name = "Mine" }),
            ecs_entity(world, { .name = "Factory" })
        }, {
            ecs_entity(world, { .name = "Copper" }), 30,
            ecs_entity(world, { .name = "Quarry" }),
            ecs_entity(world, { .name = "Foundry" })
        }, {
            ecs_entity(world, { .name = "Water" }), 40,
            ecs_entity(world, { .name = "Well" }),
            ecs_entity(world, { .name = "Reservoir" })
        }
    };

    ecs_script_t *script = ecs_script_parse(
        world, NULL, Await_haul_code, NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){
            .entity = owner,
            .loop = EcsScriptTaskLoopCount,
            .iterations = 3
        });

    for (int32_t iteration = 0; iteration < 3; iteration ++) {
        for (int32_t step = 0; step < 12; step ++) {
            int32_t i = iteration * 12 + step;
            test_int(ecs_script_task_resume(task, NULL),
                EcsScriptTaskPending);
            test_bool(ecs_script_task_is_ready(task), false);
            test_uint(await_functions[i], Await_haul_function(&fixture, step));
            test_uint(await_entities[i], owner);
            Await_haul_resolve(&fixture, i, step, &jobs[iteration]);
        }

        test_int(ecs_script_task_resume(task, NULL), iteration == 2
            ? EcsScriptTaskDone
            : EcsScriptTaskPending);
        test_bool(ecs_script_task_is_ready(task), true);
    }

    test_int(await_future_count, 36);
    Await_haul_validate(0, owner, home, &jobs[0]);
    Await_haul_validate(12, owner, home, &jobs[1]);
    Await_haul_validate(24, owner, home, &jobs[2]);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_haul_loop_forever(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    AwaitHaulFixture fixture = Await_haul_fixture(world);

    ecs_entity_t home = ecs_entity(world, { .name = "Home" });
    ecs_entity_t owner = Await_haul_owner(&fixture, "Drone1", home);
    AwaitJob jobs[3] = {
        {
            ecs_entity(world, { .name = "Iron" }), 20,
            ecs_entity(world, { .name = "Mine" }),
            ecs_entity(world, { .name = "Factory" })
        }, {
            ecs_entity(world, { .name = "Copper" }), 30,
            ecs_entity(world, { .name = "Quarry" }),
            ecs_entity(world, { .name = "Foundry" })
        }, {
            ecs_entity(world, { .name = "Water" }), 40,
            ecs_entity(world, { .name = "Well" }),
            ecs_entity(world, { .name = "Reservoir" })
        }
    };

    ecs_script_t *script = ecs_script_parse(
        world, NULL, Await_haul_code, NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){
            .entity = owner,
            .loop = EcsScriptTaskLoopForever
        });

    for (int32_t iteration = 0; iteration < 3; iteration ++) {
        for (int32_t step = 0; step < 12; step ++) {
            int32_t i = iteration * 12 + step;
            test_int(ecs_script_task_resume(task, NULL),
                EcsScriptTaskPending);
            test_bool(ecs_script_task_is_ready(task), false);
            test_uint(await_functions[i], Await_haul_function(&fixture, step));
            test_uint(await_entities[i], owner);
            Await_haul_resolve(&fixture, i, step, &jobs[iteration]);
        }

        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_bool(ecs_script_task_is_ready(task), true);
    }

    test_int(await_future_count, 36);
    Await_haul_validate(0, owner, home, &jobs[0]);
    Await_haul_validate(12, owner, home, &jobs[1]);
    Await_haul_validate(24, owner, home, &jobs[2]);

    ecs_script_task_cancel(task);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskCancelled);
    test_int(await_future_count, 36);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_haul_multiple_owners(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    AwaitHaulFixture fixture = Await_haul_fixture(world);

    ecs_entity_t homes[2] = {
        ecs_entity(world, { .name = "HomeA" }),
        ecs_entity(world, { .name = "HomeB" })
    };
    ecs_entity_t owners[2] = {
        Await_haul_owner(&fixture, "DroneA", homes[0]),
        Await_haul_owner(&fixture, "DroneB", homes[1])
    };
    AwaitJob jobs[2] = {
        {
            ecs_entity(world, { .name = "Iron" }), 20,
            ecs_entity(world, { .name = "Mine" }),
            ecs_entity(world, { .name = "Factory" })
        }, {
            ecs_entity(world, { .name = "Copper" }), 30,
            ecs_entity(world, { .name = "Quarry" }),
            ecs_entity(world, { .name = "Foundry" })
        }
    };

    ecs_script_t *script = ecs_script_parse(
        world, NULL, Await_haul_code, NULL, NULL);

    ecs_script_task_t *tasks[2] = {
        ecs_script_task_new(script, &(
            ecs_script_task_desc_t){ .entity = owners[0] }),
        ecs_script_task_new(script, &(
            ecs_script_task_desc_t){ .entity = owners[1] })
    };

    for (int32_t step = 0; step < 12; step ++) {
        for (int32_t owner = 0; owner < 2; owner ++) {
            int32_t index = step * 2 + owner;
            test_int(ecs_script_task_resume(tasks[owner], NULL),
                EcsScriptTaskPending);
            test_uint(await_functions[index],
                Await_haul_function(&fixture, step));
            test_uint(await_entities[index], owners[owner]);
        }
        for (int32_t owner = 0; owner < 2; owner ++) {
            int32_t index = step * 2 + owner;
            Await_haul_resolve(&fixture, index, step, &jobs[owner]);
        }
    }

    for (int32_t owner = 0; owner < 2; owner ++) {
        test_int(ecs_script_task_resume(tasks[owner], NULL),
            EcsScriptTaskDone);
    }

    test_int(await_future_count, 24);

    for (int32_t owner = 0; owner < 2; owner ++) {
        test_uint(await_entity_args[4 + owner][0], jobs[owner].src);
        test_uint(await_entity_args[8 + owner][0], jobs[owner].resource);
        test_int(await_i32_args[8 + owner][1], jobs[owner].amount);
        test_uint(await_entity_args[8 + owner][2], jobs[owner].src);
        test_uint(await_entity_args[8 + owner][3], owners[owner]);
        test_uint(await_entity_args[12 + owner][0], jobs[owner].dst);
        test_uint(await_entity_args[16 + owner][0], jobs[owner].resource);
        test_int(await_i32_args[16 + owner][1], jobs[owner].amount);
        test_uint(await_entity_args[16 + owner][2], owners[owner]);
        test_uint(await_entity_args[16 + owner][3], jobs[owner].dst);
        test_uint(await_entity_args[20 + owner][0], homes[owner]);
    }

    ecs_script_task_free(tasks[0]);
    ecs_script_task_free(tasks[1]);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_loop_once(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_immediate_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fetch()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .loop = EcsScriptTaskLoopOnce });

    test_bool(ecs_script_task_is_ready(task), true);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 1);
    test_bool(ecs_script_task_is_ready(task), true);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 1);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_loop_count_multiple_owners(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t owners[2] = {
        ecs_entity(world, { .name = "OwnerA" }),
        ecs_entity(world, { .name = "OwnerB" })
    };

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_record_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await step()", NULL, NULL);

    ecs_script_task_t *tasks[2] = {
        ecs_script_task_new(script, &(ecs_script_task_desc_t){
            .entity = owners[0],
            .loop = EcsScriptTaskLoopCount,
            .iterations = 2
        }),
        ecs_script_task_new(script, &(ecs_script_task_desc_t){
            .entity = owners[1],
            .loop = EcsScriptTaskLoopCount,
            .iterations = 3
        })
    };

    int32_t future = 0;
    for (int32_t owner = 0; owner < 2; owner ++) {
        int32_t iterations = owner + 2;
        for (int32_t iteration = 0; iteration < iterations; iteration ++) {
            test_int(ecs_script_task_resume(tasks[owner], NULL),
                EcsScriptTaskPending);
            test_uint(await_entities[future], owners[owner]);

            ecs_value_t value = ecs_value(ecs_i32_t, {iteration});
            test_int(ecs_script_future_resolve(
                await_futures[future], &value), 0);
            ecs_script_future_release(await_futures[future]);
            future ++;

            test_int(ecs_script_task_resume(tasks[owner], NULL),
                iteration == iterations - 1
                    ? EcsScriptTaskDone
                    : EcsScriptTaskPending);
            test_bool(ecs_script_task_is_ready(tasks[owner]), true);
        }
    }

    test_int(await_future_count, 5);

    ecs_script_task_free(tasks[0]);
    ecs_script_task_free(tasks[1]);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_loop_count_resets_state(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t ns = ecs_entity(world, { .name = "ns" });
    ecs_entity(world, {
        .name = "Tag",
        .parent = ns
    });

    ecs_async_function(world, {
        .name = "use",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"value", ecs_id(ecs_string_t)}},
        .callback = Await_store_string_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "using ns\n"
        "const value: \"hello\"\n"
        "await use($value)", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){
            .loop = EcsScriptTaskLoopCount,
            .iterations = 2
        });

    for (int32_t i = 0; i < 2; i ++) {
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_str(await_strings[i], "hello");

        ecs_value_t value = ecs_value(ecs_i32_t, {i});
        test_int(ecs_script_future_resolve(
            await_futures[i], &value), 0);
        ecs_script_future_release(await_futures[i]);

        test_int(ecs_script_task_resume(task, NULL), i == 1
            ? EcsScriptTaskDone
            : EcsScriptTaskPending);
    }

    test_int(await_future_count, 2);

    ecs_script_task_free(task);
    ecs_script_free(script);

    Await_reset();

    ecs_fini(world);
}

void Await_loop_count_invalid(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "Foo {}", NULL, NULL);

    test_expect_abort();
    ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .loop = EcsScriptTaskLoopCount });
}

void Await_loop_error_is_terminal(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    await_future_ctx_free_count = 0;

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_with_ctx_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await step()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .loop = EcsScriptTaskLoopForever });

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);

    test_int(ecs_script_future_reject(
        await_futures[0], "failed"), 0);
    ecs_script_future_release(await_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    ecs_os_free(result.error);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskError);
    test_int(await_future_count, 1);
    test_int(await_future_ctx_free_count, 1);

    ecs_script_task_free(task);
    test_int(await_future_ctx_free_count, 1);

    ecs_script_free(script);
    ecs_fini(world);
}

void Await_loop_sync_yields(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_immediate_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fetch()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, &(
        ecs_script_task_desc_t){ .loop = EcsScriptTaskLoopForever });

    for (int32_t i = 0; i < 3; i ++) {
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_int(await_future_count, i + 1);
        test_bool(ecs_script_task_is_ready(task), true);
    }

    ecs_script_task_cancel(task);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskCancelled);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_entity_enter_error_restores_state(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t broken = ecs_entity(world, { .name = "Broken" });
    ecs_set(world, broken, EcsDefaultChildComponent, {0});

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "Broken Entity {}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_impl_t *impl = flecs_script_impl(script);
    ecs_script_node_t **stmts = ecs_vec_first(&impl->root->stmts);
    ecs_script_entity_t *node = (ecs_script_entity_t*)stmts[0];
    ecs_script_runtime_t *runtime = ecs_script_runtime_new();
    ecs_script_eval_desc_t desc = { .runtime = runtime };
    ecs_script_eval_visitor_t visitor;

    flecs_script_eval_visit_init(impl, &visitor, &desc);

    ecs_script_entity_t previous = {0};
    visitor.entity = &previous;

    int32_t log_level = ecs_log_get_level();

    ecs_log_set_level(-4);
    flecs_script_entity_state_t state;
    test_int(flecs_script_eval_entity_enter(&visitor, node, &state), -1);
    ecs_log_set_level(log_level);

    test_assert(visitor.entity == &previous);

    flecs_script_eval_visit_fini(&visitor, &desc);
    ecs_script_runtime_free(runtime);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_with_enter_error_restores_state(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ECS_COMPONENT(world, AwaitTracked);
    ecs_set_hooks(world, AwaitTracked, {
        .ctor = ecs_ctor(AwaitTracked),
        .dtor = ecs_dtor(AwaitTracked)
    });
    ecs_struct(world, {
        .entity = ecs_id(AwaitTracked),
        .members = {{"value", ecs_id(ecs_i32_t)}}
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "with AwaitTracked($missing) { Foo {} }", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);
    ecs_script_eval_result_t result = {0};

    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_int(await_tracked_ctor_count, 1);
    test_int(await_tracked_dtor_count, 1);

    ecs_os_free(result.error);

    ecs_script_task_free(task);
    test_int(await_tracked_ctor_count, 1);
    test_int(await_tracked_dtor_count, 1);
    ecs_script_free(script);
    ecs_fini(world);
}

static
ecs_script_task_t* Await_nested_task(
    ecs_world_t *world,
    ecs_script_t **script_out)
{
    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback,
        .cancel = Await_cancel_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "with Tag {\n"
        "  Parent {\n"
        "    (Rel, Tgt) {\n"
        "      for i in 0..2 {\n"
        "        await step()\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    *script_out = script;

    return ecs_script_task_new(script, NULL);
}

void Await_cancel_nested_frames(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_script_t *script;
    ecs_script_task_t *task = Await_nested_task(
        world, &script);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);

    ecs_script_task_cancel(task);
    test_int(await_cancel_count, 1);
    test_bool(ecs_script_future_is_cancelled(await_futures[0]), true);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskCancelled);
    ecs_script_future_release(await_futures[0]);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_reject_nested_frames(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_script_t *script;
    ecs_script_task_t *task = Await_nested_task(
        world, &script);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);

    test_int(ecs_script_future_reject(
        await_futures[0], "nested failure"), 0);
    ecs_script_future_release(await_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "nested failure") != NULL);
    ecs_os_free(result.error);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_cancel_is_terminal(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback,
        .cancel = Await_cancel_callback
    });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await step()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_script_task_cancel(task);
    test_int(await_cancel_count, 1);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskCancelled);
    test_int(await_future_count, 1);
    test_bool(ecs_script_task_is_ready(task), true);

    ecs_script_future_release(await_futures[0]);
    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_immediate_resolve(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_immediate_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "const value = await fetch()\n"
        "Foo {}", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(
        script, NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(await_future_count, 1);
    test_assert(ecs_lookup(world, "Foo") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_await_export_const(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_async_function(world, {
        .name = "fetch",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "export const value: await fetch()", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_assert(task != NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);

    ecs_value_t result = ecs_value(ecs_i32_t, {42});
    test_int(ecs_script_future_resolve(await_futures[0], &result), 0);
    ecs_script_future_release(await_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_entity_t value = ecs_lookup(world, "value");
    test_assert(value != 0);
    ecs_value_t exported = ecs_const_var_get(world, value);
    test_uint(exported.type, ecs_id(ecs_i32_t));
    test_int(*(int32_t*)exported.ptr, 42);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void Await_interleaved_tasks_dynamic_entity(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ECS_TAG(world, Tag);

    ecs_async_function(world, {
        .name = "step",
        .return_type = ecs_id(ecs_i32_t),
        .callback = Await_store_callback
    });

    ecs_script_vars_t *vars[2] = {
        ecs_script_vars_init(world),
        ecs_script_vars_init(world)
    };
    ecs_script_var_t *names[2] = {
        ecs_script_vars_define(vars[0], "name", ecs_string_t),
        ecs_script_vars_define(vars[1], "name", ecs_string_t)
    };
    *(char**)names[0]->value.ptr = ecs_os_strdup("EntityA");
    *(char**)names[1]->value.ptr = ecs_os_strdup("EntityB");

    ecs_script_eval_desc_t parse_desc = { .vars = vars[0] };
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "\"{$name}\" {\n"
        "  await step()\n"
        "  Tag\n"
        "}", &parse_desc, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *tasks[2] = {
        ecs_script_task_new(script, &(
            ecs_script_task_desc_t){ .vars = vars[0] }),
        ecs_script_task_new(script, &(
            ecs_script_task_desc_t){ .vars = vars[1] })
    };

    test_int(ecs_script_task_resume(tasks[0], NULL),
        EcsScriptTaskPending);
    test_int(ecs_script_task_resume(tasks[1], NULL),
        EcsScriptTaskPending);

    ecs_value_t result = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(await_futures[0], &result), 0);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(tasks[0], NULL),
        EcsScriptTaskDone);

    ecs_entity_t entity_a = ecs_lookup(world, "EntityA");
    ecs_entity_t entity_b = ecs_lookup(world, "EntityB");
    test_assert(entity_a != 0);
    test_assert(entity_b != 0);
    test_assert(ecs_has(world, entity_a, Tag));
    test_assert(!ecs_has(world, entity_b, Tag));

    test_int(ecs_script_future_resolve(await_futures[1], &result), 0);
    ecs_script_future_release(await_futures[1]);
    test_int(ecs_script_task_resume(tasks[1], NULL),
        EcsScriptTaskDone);
    test_assert(ecs_has(world, entity_b, Tag));

    ecs_script_task_free(tasks[0]);
    ecs_script_task_free(tasks[1]);
    ecs_script_free(script);
    ecs_script_vars_fini(vars[0]);
    ecs_script_vars_fini(vars[1]);
    ecs_fini(world);
}

void Await_await_method_receiver(void) {
    ecs_world_t *world = ecs_init();

    Await_reset();

    ecs_entity_t method = ecs_method(world, {
        .name = "addAsync",
        .parent = ecs_id(ecs_i64_t),
        .return_type = ecs_id(ecs_i64_t),
        .params = {{"value", ecs_id(ecs_i64_t)}},
        .callback = Await_method_placeholder
    });
    EcsScriptMethod *method_data = ecs_ensure(
        world, method, EcsScriptMethod);
    method_data->callback = NULL;
    method_data->async_callback = Await_method_callback;
    ecs_modified(world, method, EcsScriptMethod);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "await (10).addAsync(20)", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_assert(task != NULL);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(await_future_count, 1);
    test_int(await_method_instance, 10);

    ecs_value_t result = ecs_value(ecs_i64_t, {30});
    test_int(ecs_script_future_resolve(await_futures[0], &result), 0);
    ecs_script_future_release(await_futures[0]);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}
