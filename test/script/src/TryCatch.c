#include <script.h>
#include "../../../src/addons/script/script.h"

typedef struct TCDrone {
    ecs_entity_t home;
} TCDrone;

static ecs_script_future_t *tc_futures[64];
static int32_t tc_future_count;
static int32_t tc_cancel_count;

static void TryCatch_store_callback(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;
    (void)argv;
    tc_futures[tc_future_count ++] = future;
}

static void TryCatch_cancel_callback(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)future;
    tc_cancel_count ++;
}

static void TryCatch_reset(void)
{
    ecs_os_zeromem(tc_futures);
    tc_future_count = 0;
    tc_cancel_count = 0;
}

/* Register a "fail" async function and return the world */
static void TryCatch_register_fail(
    ecs_world_t *world)
{
    ecs_async_function(world, {
        .name = "fail",
        .return_type = ecs_id(ecs_i32_t),
        .callback = TryCatch_store_callback,
        .cancel = TryCatch_cancel_callback
    });
}

void TryCatch_parse_try_catch_all(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(strstr(str, "try") != NULL);
    test_assert(strstr(str, "catch") != NULL);
    test_assert(strstr(str, "Handled") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_parse_try_catch_typed(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(strstr(str, "try") != NULL);
    test_assert(strstr(str, "catch(DestinationGone)") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_parse_try_multiple_catch(void) {
    ecs_world_t *world = ecs_init();

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  A {}\n"
        "} catch(OutOfFuel) {\n"
        "  B {}\n"
        "} catch {\n"
        "  C {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    char *str = ecs_script_ast_to_str(script, false);
    test_assert(strstr(str, "catch(DestinationGone)") != NULL);
    test_assert(strstr(str, "catch(OutOfFuel)") != NULL);
    ecs_os_free(str);

    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_parse_try_missing_catch(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "}\n", NULL, NULL);
    test_assert(script == NULL);

    ecs_fini(world);
}

void TryCatch_parse_try_missing_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try await fail()\n", NULL, NULL);
    test_assert(script == NULL);

    ecs_fini(world);
}

void TryCatch_parse_catch_missing_scope(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch\n", NULL, NULL);
    test_assert(script == NULL);

    ecs_fini(world);
}

void TryCatch_parse_catch_unclosed_paren(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone {\n"
        "}\n", NULL, NULL);
    test_assert(script == NULL);

    ecs_fini(world);
}

void TryCatch_parse_catch_var_error(void) {
    ecs_world_t *world = ecs_init();

    ecs_log_set_level(-4);
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch($err) {\n"
        "}\n", NULL, NULL);
    test_assert(script == NULL);

    ecs_fini(world);
}

void TryCatch_try_in_plain_eval(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  Foo {}\n"
        "} catch {\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_log_set_level(-4);
    test_assert(ecs_script_eval(script, NULL, NULL) != 0);

    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_catch_all_reject(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "  NotReached {}\n"
        "} catch {\n"
        "  Handled {}\n"
        "}\n"
        "After {}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 1);

    test_int(ecs_script_future_reject(tc_futures[0], "it failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskDone);
    test_assert(result.error == NULL);

    test_assert(ecs_lookup(world, "NotReached") == 0);
    test_assert(ecs_lookup(world, "Handled") != 0);
    test_assert(ecs_lookup(world, "After") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_catch_typed_match(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t err = ecs_entity(world, { .name = "DestinationGone" });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  Typed {}\n"
        "} catch {\n"
        "  All {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(tc_futures[0], err, "gone"), 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskDone);
    test_assert(result.error == NULL);

    test_assert(ecs_lookup(world, "Typed") != 0);
    test_assert(ecs_lookup(world, "All") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_catch_typed_no_match_propagates(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t unused = ecs_entity(world, { .name = "DestinationGone" });
    (void)unused;
    ecs_entity_t other = ecs_entity(world, { .name = "OutOfFuel" });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  Typed {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(
        tc_futures[0], other, "out of fuel"), 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "out of fuel") != NULL);
    test_assert(strstr(result.error, "OutOfFuel") != NULL);
    ecs_os_free(result.error);

    test_assert(ecs_lookup(world, "Typed") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_catch_typed_falls_through_to_catch_all(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t unused = ecs_entity(world, { .name = "DestinationGone" });
    (void)unused;
    ecs_entity_t other = ecs_entity(world, { .name = "OutOfFuel" });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  Typed {}\n"
        "} catch {\n"
        "  All {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(
        tc_futures[0], other, "out of fuel"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    test_assert(ecs_lookup(world, "Typed") == 0);
    test_assert(ecs_lookup(world, "All") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_catch_first_match_wins(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t err = ecs_entity(world, { .name = "DestinationGone" });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  All {}\n"
        "} catch(DestinationGone) {\n"
        "  Typed {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(tc_futures[0], err, "gone"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    /* catch-all listed first wins, even for typed errors */
    test_assert(ecs_lookup(world, "All") != 0);
    test_assert(ecs_lookup(world, "Typed") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_plain_reject_not_caught_by_typed(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t unused = ecs_entity(world, { .name = "DestinationGone" });
    (void)unused;

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  Typed {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "plain error"), 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "plain error") != NULL);
    ecs_os_free(result.error);

    test_assert(ecs_lookup(world, "Typed") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_runtime_error_not_caught(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  const x = $does_not_exist\n"
        "  await fail()\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    ecs_os_free(result.error);

    /* runtime errors must not trigger catch handlers */
    test_assert(ecs_lookup(world, "Handled") == 0);
    test_int(tc_future_count, 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_runtime_error_is_terminal(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  const x = $does_not_exist\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    ecs_os_free(result.error);

    /* errored tasks stay errored and can be freed safely */
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskError);
    test_bool(ecs_script_task_is_ready(task), true);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_await_in_catch(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  await fail()\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 1);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    /* catch handler starts and suspends on its own await */
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 2);
    test_assert(ecs_lookup(world, "Handled") == 0);

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(tc_futures[1], &value), 0);
    ecs_script_future_release(tc_futures[1]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_assert(ecs_lookup(world, "Handled") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_reject_in_catch_propagates(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  await fail()\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "first"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[1], "second"), 0);
    ecs_script_future_release(tc_futures[1]);

    /* error thrown from catch handler is not caught by the same try */
    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "second") != NULL);
    ecs_os_free(result.error);

    test_assert(ecs_lookup(world, "Handled") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_nested_try_inner_catches(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  try {\n"
        "    await fail()\n"
        "  } catch {\n"
        "    Inner {}\n"
        "  }\n"
        "} catch {\n"
        "  Outer {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    test_assert(ecs_lookup(world, "Inner") != 0);
    test_assert(ecs_lookup(world, "Outer") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_nested_try_outer_catches(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t unused = ecs_entity(world, { .name = "DestinationGone" });
    (void)unused;
    ecs_entity_t other = ecs_entity(world, { .name = "OutOfFuel" });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  try {\n"
        "    await fail()\n"
        "  } catch(DestinationGone) {\n"
        "    Inner {}\n"
        "  }\n"
        "} catch {\n"
        "  Outer {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(
        tc_futures[0], other, "out of fuel"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    test_assert(ecs_lookup(world, "Inner") == 0);
    test_assert(ecs_lookup(world, "Outer") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_try_in_entity_scope(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "Parent {\n"
        "  try {\n"
        "    Child {\n"
        "      await fail()\n"
        "      NotReached {}\n"
        "    }\n"
        "  } catch {\n"
        "    Handled {}\n"
        "  }\n"
        "  After {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    /* catch/after statements run in the parent scope, not the scope that
     * was active when the error was thrown */
    test_assert(ecs_lookup(world, "Parent.Handled") != 0);
    test_assert(ecs_lookup(world, "Parent.After") != 0);
    test_assert(ecs_lookup(world, "Parent.Child.NotReached") == 0);
    test_assert(ecs_lookup(world, "Handled") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_error_in_with_scope_unwinds(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    (void)tag;

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  with Tag {\n"
        "    await fail()\n"
        "  }\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    /* entity created in catch must not get the with tag */
    ecs_entity_t handled = ecs_lookup(world, "Handled");
    test_assert(handled != 0);
    test_bool(ecs_has_id(world, handled,
        ecs_lookup(world, "Tag")), false);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_error_in_for_unwinds(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  for i in 0..3 {\n"
        "    await fail()\n"
        "  }\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(tc_futures[0], &value), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 2);

    /* fail on second loop iteration */
    test_int(ecs_script_future_reject(tc_futures[1], "failed"), 0);
    ecs_script_future_release(tc_futures[1]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_int(tc_future_count, 2);
    test_assert(ecs_lookup(world, "Handled") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_loop_forever_catch_resets(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script,
        &(ecs_script_task_desc_t){
            .loop = EcsScriptTaskLoopForever
        });

    for (int32_t i = 0; i < 3; i ++) {
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
        test_int(tc_future_count, i + 1);

        test_int(ecs_script_future_reject(tc_futures[i], "failed"), 0);
        ecs_script_future_release(tc_futures[i]);

        /* iteration completes via catch, task restarts */
        test_int(ecs_script_task_resume(task, NULL),
            EcsScriptTaskPending);
    }

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_loop_count_catch(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script,
        &(ecs_script_task_desc_t){
            .loop = EcsScriptTaskLoopCount,
            .iterations = 2
        });

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    /* first iteration completes via catch */
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 1);

    /* second iteration starts */
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 2);

    test_int(ecs_script_future_reject(tc_futures[1], "failed"), 0);
    ecs_script_future_release(tc_futures[1]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_loop_uncaught_error_is_terminal(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t unused = ecs_entity(world, { .name = "DestinationGone" });
    (void)unused;

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script,
        &(ecs_script_task_desc_t){
            .loop = EcsScriptTaskLoopForever
        });

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    ecs_os_free(result.error);

    /* uncaught errors terminate the task, also in loop mode */
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskError);
    test_int(tc_future_count, 1);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_cancel_not_caught(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_script_task_cancel(task);
    test_int(tc_cancel_count, 1);
    test_bool(ecs_script_future_is_cancelled(tc_futures[0]), true);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskCancelled);
    test_assert(ecs_lookup(world, "Handled") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_resolve_skips_catch(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "  Ok {}\n"
        "} catch {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    test_int(ecs_script_future_resolve(tc_futures[0], &value), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    test_assert(ecs_lookup(world, "Ok") != 0);
    test_assert(ecs_lookup(world, "Handled") == 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_catch_error_from_using_scope(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t errors = ecs_entity(world, { .name = "errors" });
    ecs_entity_t err = ecs_entity(world, {
        .name = "DestinationGone", .parent = errors });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "using errors\n"
        "try {\n"
        "  await fail()\n"
        "} catch(DestinationGone) {\n"
        "  Handled {}\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(tc_futures[0], err, "gone"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);
    test_assert(ecs_lookup(world, "Handled") != 0);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_reject_id_twice_fails(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t err = ecs_entity(world, { .name = "DestinationGone" });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fail()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(tc_futures[0], err, "gone"), 0);
    test_assert(ecs_script_future_reject_id(
        tc_futures[0], err, "gone") != 0);
    test_assert(ecs_script_future_reject(tc_futures[0], "gone") != 0);

    ecs_value_t value = ecs_value(ecs_i32_t, {10});
    test_assert(ecs_script_future_resolve(tc_futures[0], &value) != 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    ecs_os_free(result.error);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_reject_id_null_msg(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t err = ecs_entity(world, { .name = "DestinationGone" });

    ecs_script_t *script = ecs_script_parse(
        world, NULL, "await fail()", NULL, NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject_id(tc_futures[0], err, NULL), 0);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_eval_result_t result = {0};
    test_int(ecs_script_task_resume(task, &result),
        EcsScriptTaskError);
    test_assert(result.error != NULL);
    test_assert(strstr(result.error, "DestinationGone") != NULL);
    ecs_os_free(result.error);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_vars_preserved_across_catch(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t ecs_id(Position) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "Position" }),
        .members = {
            {"x", ecs_id(ecs_f32_t)},
            {"y", ecs_id(ecs_f32_t)}
        }
    });

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "const a = 5\n"
        "try {\n"
        "  const b = 10\n"
        "  await fail()\n"
        "} catch {\n"
        "  Handled { Position: {a, 1} }\n"
        "}\n"
        "After { Position: {a, 2} }", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_entity_t handled = ecs_lookup(world, "Handled");
    test_assert(handled != 0);
    const Position *p = ecs_get(world, handled, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 1);

    ecs_entity_t after = ecs_lookup(world, "After");
    test_assert(after != 0);
    p = ecs_get(world, after, Position);
    test_assert(p != NULL);
    test_int(p->x, 5);
    test_int(p->y, 2);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_free_while_pending_in_try(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t tag = ecs_entity(world, { .name = "Tag" });
    (void)tag;

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "Parent {\n"
        "  try {\n"
        "    with Tag {\n"
        "      await fail()\n"
        "    }\n"
        "  } catch {\n"
        "  }\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    /* free while suspended inside try: frames (including the try frame)
     * must unwind cleanly */
    ecs_script_task_free(task);
    test_int(tc_cancel_count, 1);
    ecs_script_future_release(tc_futures[0]);

    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_free_while_pending_in_catch(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  await fail()\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script, NULL);
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 2);

    ecs_script_task_free(task);
    test_int(tc_cancel_count, 1);
    ecs_script_future_release(tc_futures[1]);

    ecs_script_free(script);
    ecs_fini(world);
}

void TryCatch_this_var_in_catch(void) {
    ecs_world_t *world = ecs_init();

    TryCatch_reset();
    TryCatch_register_fail(world);

    ecs_entity_t ecs_id(TCDrone) = ecs_struct(world, {
        .entity = ecs_entity(world, { .name = "TCDrone" }),
        .members = {{"home", ecs_id(ecs_entity_t)}}
    });

    ecs_entity_t home = ecs_entity(world, { .name = "Home" });
    ecs_entity_t drone = ecs_entity(world, { .name = "Drone1" });
    ecs_set(world, drone, TCDrone, {home});

    ecs_async_function(world, {
        .name = "moveTo",
        .return_type = ecs_id(ecs_i32_t),
        .params = {{"dst", ecs_id(ecs_entity_t)}},
        .callback = TryCatch_store_callback
    });

    /* the drone example: if the destination is gone, go back home */
    ecs_script_t *script = ecs_script_parse(world, NULL,
        "try {\n"
        "  await fail()\n"
        "} catch {\n"
        "  await moveTo(this[TCDrone].home)\n"
        "}", NULL, NULL);
    test_assert(script != NULL);

    ecs_script_task_t *task = ecs_script_task_new(script,
        &(ecs_script_task_desc_t){ .entity = drone });
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);

    test_int(ecs_script_future_reject(tc_futures[0], "failed"), 0);
    ecs_script_future_release(tc_futures[0]);

    /* catch handler can use the task's this variable */
    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskPending);
    test_int(tc_future_count, 2);

    ecs_value_t value = ecs_value(ecs_i32_t, {0});
    test_int(ecs_script_future_resolve(tc_futures[1], &value), 0);
    ecs_script_future_release(tc_futures[1]);

    test_int(ecs_script_task_resume(task, NULL),
        EcsScriptTaskDone);

    ecs_script_task_free(task);
    ecs_script_free(script);
    ecs_fini(world);
}
