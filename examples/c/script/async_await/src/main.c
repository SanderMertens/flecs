#include <async_await.h>
#include <stdio.h>

/* This example shows how to register an async function in C, and how to await
 * it from a script. Async functions don't return a result right away. Instead
 * they receive a future, which can be completed at a later point in time. A
 * script that awaits a future suspends until the future is completed.
 *
 * Scripts that use await must run as a task. A task is resumed by the
 * application until the script has completed.
 */

const char *code =
    "const score = await fetch_score(\"Alice\")\n"
    "\n"
    "Alice {\n"
    "  Score: {score}\n"
    "}\n"
    ;

typedef struct {
    int32_t value;
} Score;

// Future of the request that the server hasn't responded to yet
ecs_script_future_t *pending_request = NULL;

void fetch_score(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;

    printf("fetch_score(\"%s\"): request sent\n", *(char**)argv[0].ptr);

    // Don't complete the future yet, but store it so the server can complete
    // it later.
    pending_request = future;
}

// System that responds to the pending request
void ServerProgress(
    ecs_iter_t *it)
{
    (void)it;

    if (!pending_request) {
        return;
    }

    // Resolve the future with the result, after which the script can resume.
    // The callback received a reference to the future, which must be released
    // after the future is completed.
    printf("server: responding with 120\n");
    int32_t score = 120;
    ecs_value_t value = { .type = ecs_id(ecs_i32_t), .ptr = &score };
    ecs_script_future_resolve(pending_request, &value);
    ecs_script_future_release(pending_request);
    pending_request = NULL;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Score);

    ecs_struct(world, {
        .entity = ecs_id(Score),
        .members = {
            { .name = "value", .type = ecs_id(ecs_i32_t) }
        }
    });

    ECS_SYSTEM(world, ServerProgress, EcsOnUpdate, 0);

    // Register the async function. Unlike regular functions, the callback
    // receives a future instead of a result.
    ecs_async_function(world, {
        .name = "fetch_score",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { .name = "name", .type = ecs_id(ecs_string_t) }
        },
        .callback = fetch_score
    });

    ecs_script_t *script = ecs_script_parse(world, "scores", code, NULL, NULL);
    if (!script) {
        printf("script failed to parse\n");
        return -1;
    }

    // Create a task for the script. A task runs a script until it awaits a
    // future that hasn't been completed yet.
    ecs_script_task_t *task = ecs_script_task_new(script, NULL);

    // Resume the task each frame until the script is done
    ecs_script_task_status_t status;
    while ((status = ecs_script_task_resume(task, NULL)) ==
        EcsScriptTaskPending)
    {
        printf("script is waiting\n");
        ecs_progress(world, 0);
    }

    if (status != EcsScriptTaskDone) {
        printf("script failed to run\n");
        return -1;
    }

    ecs_script_task_free(task);
    ecs_script_free(script);

    const Score *s = ecs_get(world, ecs_lookup(world, "Alice"), Score);
    printf("Alice: %d\n", s->value);

    return ecs_fini(world);

    // Output
    //  fetch_score("Alice"): request sent
    //  script is waiting
    //  server: responding with 120
    //  Alice: 120
}
