#include <async_block.h>
#include <stdio.h>

/* This example shows how to await an async function from an async block. Code
 * in an async block runs as a task, which is resumed each frame by ecs_progress
 * until the block has completed. This means that unlike the async_await
 * example, the application doesn't have to create or resume tasks.
 *
 * An async block can't create entities, but it can assign mut variables. When a
 * mut variable is assigned, the parts of the script that depend on it are
 * updated automatically.
 */

const char *code =
    "export mut score: i32 = 0\n"
    "\n"
    "Alice {\n"
    "  Score: {score}\n"
    "}\n"
    "\n"
    "async {\n"
    "  const result = await fetch_score(\"Alice\")\n"
    "  score = result\n"
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

    // Resolve the future with the result, after which the async block can
    // resume. The callback received a reference to the future, which must be
    // released after the future is completed.
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

    // Create a managed script. Managed scripts are reactive, which means that
    // Alice's Score is updated when the async block assigns the score variable.
    ecs_entity_t script = ecs_script(world, {
        .code = code
    });

    if (!script) {
        printf("script failed to run\n");
        return -1;
    }

    ecs_entity_t alice = ecs_lookup(world, "Alice");

    // Each frame resumes the async block, and runs the ServerProgress system
    for (int i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        printf("frame %d: Alice: %d\n", i, ecs_get(world, alice, Score)->value);
    }

    return ecs_fini(world);

    // Output
    //  fetch_score("Alice"): request sent
    //  server: responding with 120
    //  frame 0: Alice: 0
    //  frame 1: Alice: 120
    //  frame 2: Alice: 120
}
