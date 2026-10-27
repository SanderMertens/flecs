#include <async_await_c.h>
#include <stdio.h>

/* This example shows how to call an async function from C, without using a
 * script. Calling an async function returns a future, which is completed at a
 * later point in time. The application can check the status of the future to
 * see whether it was resolved with a value, rejected with an error, or
 * cancelled.
 *
 * See the async_await example for how to await async functions from a script.
 */

// Error id used when a level doesn't exist
ECS_TAG_DECLARE(LevelNotFound);

// Future of the level that the loader hasn't finished loading yet
ecs_script_future_t *pending_load = NULL;
int32_t pending_level = 0;

void load_level(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future)
{
    (void)ctx;
    (void)argc;

    pending_level = *(int32_t*)argv[0].ptr;
    printf("load_level(%d): started\n", pending_level);

    // Don't complete the future yet, but store it so the loader can complete
    // it later.
    pending_load = future;
}

// Invoked when a pending future is cancelled. This can be used to abort work
// that is still in progress.
void load_level_cancel(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future)
{
    (void)ctx;

    printf("load_level(%d): cancelled\n", pending_level);
    ecs_script_future_release(future);
    pending_load = NULL;
}

// System that completes the pending load
void LoaderProgress(
    ecs_iter_t *it)
{
    (void)it;

    if (!pending_load) {
        return;
    }

    if (pending_level <= 3) {
        int32_t entity_count = pending_level * 100;
        ecs_value_t value = { .type = ecs_id(ecs_i32_t), .ptr = &entity_count };
        ecs_script_future_resolve(pending_load, &value);
    } else {
        // Reject with an error id, so callers can tell what went wrong
        ecs_script_future_reject_id(
            pending_load, LevelNotFound, "level does not exist");
    }

    // Release the reference that the load_level callback received
    ecs_script_future_release(pending_load);
    pending_load = NULL;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_TAG_DEFINE(world, LevelNotFound);

    ECS_SYSTEM(world, LoaderProgress, EcsOnUpdate, 0);

    ecs_entity_t load_level_fn = ecs_async_function(world, {
        .name = "load_level",
        .return_type = ecs_id(ecs_i32_t),
        .params = {
            { .name = "level", .type = ecs_id(ecs_i32_t) }
        },
        .callback = load_level,
        .cancel = load_level_cancel
    });

    int32_t level = 1;
    ecs_value_t arg = { .type = ecs_id(ecs_i32_t), .ptr = &level };

    // Load level 1. The returned future is owned by the application, and must
    // be released when it's no longer needed.
    ecs_script_future_t *future = ecs_async_function_call(
        world, load_level_fn, 1, &arg);

    while (ecs_script_future_status(future) == EcsScriptFuturePending) {
        ecs_progress(world, 0);
    }

    const ecs_value_t *value = ecs_script_future_get_value(future);
    printf("level 1: loaded %d entities\n", *(int32_t*)value->ptr);
    ecs_script_future_release(future);

    // Load level 42, which doesn't exist
    level = 42;
    future = ecs_async_function_call(world, load_level_fn, 1, &arg);

    while (ecs_script_future_status(future) == EcsScriptFuturePending) {
        ecs_progress(world, 0);
    }

    char *error_id = ecs_get_path(world,
        ecs_script_future_get_error_id(future));
    printf("level 42: failed with %s (%s)\n", error_id,
        ecs_script_future_get_error(future));
    ecs_os_free(error_id);
    ecs_script_future_release(future);

    // Load level 2, but cancel it before it has finished loading
    level = 2;
    future = ecs_async_function_call(world, load_level_fn, 1, &arg);
    ecs_script_future_cancel(future);

    if (ecs_script_future_status(future) == EcsScriptFutureCancelled) {
        printf("level 2: cancelled\n");
    }

    ecs_script_future_release(future);

    return ecs_fini(world);

    // Output
    //  load_level(1): started
    //  level 1: loaded 100 entities
    //  load_level(42): started
    //  level 42: failed with LevelNotFound (level does not exist)
    //  load_level(2): started
    //  load_level(2): cancelled
    //  level 2: cancelled
}
