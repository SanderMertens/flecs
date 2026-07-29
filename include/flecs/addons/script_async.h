/**
 * @file addons/script_async.h
 * @brief Async/await support for Flecs script.
 */

#ifdef FLECS_SCRIPT_ASYNC

#ifndef FLECS_SCRIPT
#define FLECS_SCRIPT
#endif

/**
 * @defgroup c_addons_script_async Script Async
 * @ingroup c_addons
 * Async/await support for Flecs script.
 *
 * @{
 */

#ifndef FLECS_SCRIPT_ASYNC_H
#define FLECS_SCRIPT_ASYNC_H

#ifdef __cplusplus
extern "C" {
#endif

FLECS_API
extern ECS_COMPONENT_DECLARE(EcsScriptTask);

/** Future for the result of an async function call.
 * A future is created when a script awaits an async function, and is passed to
 * the async function callback. The callback (or code invoked by it, possibly
 * on another thread) completes the future with ecs_script_future_resolve() or
 * ecs_script_future_reject(), after which the awaiting task can resume.
 */
typedef struct ecs_script_future_t ecs_script_future_t;

/** Resumable script execution.
 * A task runs a script incrementally, suspending when the script awaits an
 * async function and resuming when the awaited future is completed. Tasks are
 * created with ecs_script_task_new() and advanced with
 * ecs_script_task_resume().
 */
typedef struct ecs_script_task_t ecs_script_task_t;

/** Async script function callback.
 * Instead of returning a result directly, an async function receives a future
 * that must be completed with ecs_script_future_resolve() or
 * ecs_script_future_reject(). The future may be completed from inside the
 * callback or at a later point in time, from any thread.
 */
typedef void(*ecs_async_function_callback_t)(
    const ecs_function_ctx_t *ctx,
    int32_t argc,
    const ecs_value_t *argv,
    ecs_script_future_t *future);

/** Async script function cancel callback.
 * Invoked when a pending future is cancelled, which allows an async function
 * to abort in-flight work. After this callback the future can no longer be
 * resolved or rejected.
 */
typedef void(*ecs_async_function_cancel_t)(
    const ecs_function_ctx_t *ctx,
    ecs_script_future_t *future);

/** Task component.
 * This component is added to entities for which tasks are created (see the
 * entity member of ecs_script_task_desc_t) and removed when their last task
 * is freed. The component does not own the tasks; it is kept in sync by
 * ecs_script_task_new() and ecs_script_task_free().
 *
 * When an entity with this component is deleted (or the component is
 * removed), the remaining tasks are cancelled (see ecs_script_task_cancel()).
 * The application remains responsible for freeing them.
 *
 * The component is registered as an opaque type that serializes the current
 * state of each task: the script entity (0 for unmanaged scripts), task
 * status, the line/column of the statement the task is suspended on, the
 * async function being awaited and the number of completed iterations. This
 * makes it possible to
 * inspect where in a script a task is from tools like the explorer.
 */
typedef struct EcsScriptTask {
    ecs_vec_t tasks; /**< vec<ecs_script_task_t*>. Not owned by component. */
} EcsScriptTask;

/** Script task status. */
typedef enum ecs_script_task_status_t {
    EcsScriptTaskPending,    /**< Task is awaiting a future or hasn't run yet. */
    EcsScriptTaskDone,       /**< Task ran to completion. */
    EcsScriptTaskError,      /**< Task encountered an error. */
    EcsScriptTaskCancelled   /**< Task was cancelled. */
} ecs_script_task_status_t;

/** Script task loop mode. */
typedef enum ecs_script_task_loop_t {
    EcsScriptTaskLoopOnce,    /**< Run script once (default). */
    EcsScriptTaskLoopCount,   /**< Run script a fixed number of times. */
    EcsScriptTaskLoopForever  /**< Restart script after each completion. */
} ecs_script_task_loop_t;

/** Used with ecs_script_task_new(). */
typedef struct ecs_script_task_desc_t {
    ecs_entity_t entity;           /**< Entity associated with task. When set,
                                    *   the script gets a "this" variable that
                                    *   is bound to the entity, and the entity
                                    *   gets an EcsScriptTask component that
                                    *   can be used to inspect task state. */
    const ecs_script_vars_t *vars; /**< Variables used by script. */
    void *ctx;                     /**< User context. */
    ecs_ctx_free_t ctx_free;       /**< Callback to free ctx. */
    ecs_script_task_loop_t loop;   /**< Loop mode. */
    int32_t iterations;            /**< Number of iterations. Must be set when
                                    *   loop is EcsScriptTaskLoopCount. */
} ecs_script_task_desc_t;

/** Create task.
 * This operation creates a task that runs the provided script. Unlike
 * ecs_script_eval(), a task evaluates a script incrementally: when the script
 * awaits an async function, evaluation suspends until the awaited future is
 * completed. This makes it possible for scripts to use the await keyword.
 *
 * The returned task does not start running until ecs_script_task_resume() is
 * called. The task must be freed with ecs_script_task_free().
 *
 * @param script The script to run.
 * @param desc Task creation parameters (optional).
 * @return The task, or NULL if failed.
 */
FLECS_API
ecs_script_task_t* ecs_script_task_new(
    const ecs_script_t *script,
    const ecs_script_task_desc_t *desc);

/** Resume task.
 * This operation runs the task until it completes or awaits a future that is
 * still pending. It is safe to call this operation while the task is waiting
 * on a pending future, in which case it returns EcsScriptTaskPending without
 * doing work. To avoid unnecessary calls, an application can first check
 * ecs_script_task_is_ready().
 *
 * If the task loop mode is EcsScriptTaskLoopCount or EcsScriptTaskLoopForever,
 * the task restarts from the beginning of the script after each completed
 * iteration, and EcsScriptTaskDone is not returned until all iterations have
 * completed (never, for EcsScriptTaskLoopForever).
 *
 * When the result parameter is not NULL, the task will capture errors and
 * return them in the output struct. If result.error is set, it must be freed
 * by the application.
 *
 * @param task The task.
 * @param result Output of script evaluation.
 * @return The task status after resuming.
 */
FLECS_API
ecs_script_task_status_t ecs_script_task_resume(
    ecs_script_task_t *task,
    ecs_script_eval_result_t *result);

/** Check if task is ready to resume.
 * This operation returns false if the task is waiting on a future that hasn't
 * been completed yet, in which case calling ecs_script_task_resume() would not
 * make progress. This operation is threadsafe with respect to future
 * completion, which makes it safe to poll tasks that wait on futures that are
 * resolved from other threads.
 *
 * @param task The task.
 * @return True if calling ecs_script_task_resume() would make progress.
 */
FLECS_API
bool ecs_script_task_is_ready(
    const ecs_script_task_t *task);

/** Cancel task.
 * This operation cancels a running task. If the task is waiting on a pending
 * future, the future is cancelled and the async function's cancel callback is
 * invoked (if provided). Cancelling a task that already completed has no
 * effect. A cancelled task can no longer be resumed.
 *
 * @param task The task.
 */
FLECS_API
void ecs_script_task_cancel(
    ecs_script_task_t *task);

/** Free task.
 * This operation frees a task. If the task hasn't finished yet it is
 * cancelled (see ecs_script_task_cancel()).
 *
 * @param task The task.
 */
FLECS_API
void ecs_script_task_free(
    ecs_script_task_t *task);

/** Resolve future.
 * This operation completes a future with a result value. The value must be of
 * the return type of the async function, and is copied into the future. The
 * awaiting task can afterwards be resumed with ecs_script_task_resume().
 *
 * This operation is threadsafe and may be called from a different thread than
 * the one that runs the task.
 *
 * @param future The future.
 * @param value The result value.
 * @return Zero if success, non-zero if the future was already completed or
 *         cancelled.
 */
FLECS_API
int ecs_script_future_resolve(
    ecs_script_future_t *future,
    const ecs_value_t *value);

/** Reject future.
 * This operation completes a future with an error. When the awaiting task is
 * resumed, evaluation of the script fails with the provided error message.
 *
 * This operation is threadsafe and may be called from a different thread than
 * the one that runs the task.
 *
 * @param future The future.
 * @param error The error message.
 * @return Zero if success, non-zero if the future was already completed or
 *         cancelled.
 */
FLECS_API
int ecs_script_future_reject(
    ecs_script_future_t *future,
    const char *error);

/** Reject future with error id.
 * Same as ecs_script_future_reject(), but additionally associates the error
 * with an error identifier. Scripts can catch errors with a specific
 * identifier with a typed catch clause:
 *
 * @code
 * try {
 *   await fetch()
 * } catch(ConnectionLost) {
 *   // runs when the future was rejected with the ConnectionLost entity
 * } catch {
 *   // runs for any other error
 * }
 * @endcode
 *
 * Errors rejected without an error id (or with ecs_script_future_reject())
 * can only be caught by a catch-all clause. When msg is NULL a default
 * message is used.
 *
 * This operation is threadsafe and may be called from a different thread than
 * the one that runs the task.
 *
 * @param future The future.
 * @param error The error identifier. May be 0 if msg is not NULL.
 * @param msg The error message (optional if error is set).
 * @return Zero if success, non-zero if the future was already completed or
 *         cancelled.
 */
FLECS_API
int ecs_script_future_reject_id(
    ecs_script_future_t *future,
    ecs_entity_t error,
    const char *msg);

/** Check if future is cancelled.
 * This operation returns whether a future was cancelled, which happens when
 * the awaiting task is cancelled or freed. Async functions that do work over
 * multiple frames or on other threads can use this to detect that the result
 * is no longer needed.
 *
 * This operation is threadsafe.
 *
 * @param future The future.
 * @return True if the future is cancelled.
 */
FLECS_API
bool ecs_script_future_is_cancelled(
    const ecs_script_future_t *future);

/** Increase future refcount.
 * This operation increases the refcount of a future, which allows an async
 * function to keep the future alive after the callback returns. Each call to
 * this operation must be matched with a call to ecs_script_future_release().
 *
 * A future passed to an async function callback is already retained on behalf
 * of the callback, and must be released after the future is completed.
 *
 * This operation is threadsafe.
 *
 * @param future The future.
 * @return The future.
 */
FLECS_API
ecs_script_future_t* ecs_script_future_retain(
    ecs_script_future_t *future);

/** Decrease future refcount.
 * This operation decreases the refcount of a future, and frees the future when
 * the refcount reaches zero.
 *
 * This operation is threadsafe.
 *
 * @param future The future.
 */
FLECS_API
void ecs_script_future_release(
    ecs_script_future_t *future);

/** Set future context.
 * This operation attaches a user context to a future, which async functions
 * can use to store state associated with the in-flight operation (for example
 * a request handle, so it can be aborted from the cancel callback). The
 * context can only be set once. If a ctx_free callback is provided, it is
 * invoked when the future is freed.
 *
 * @param future The future.
 * @param ctx User context.
 * @param ctx_free Callback to free ctx (optional).
 */
FLECS_API
void ecs_script_future_set_ctx(
    ecs_script_future_t *future,
    void *ctx,
    ecs_ctx_free_t ctx_free);

/** Get future context.
 * Returns the context set with ecs_script_future_set_ctx().
 *
 * @param future The future.
 * @return The user context, or NULL if not set.
 */
FLECS_API
void* ecs_script_future_get_ctx(
    const ecs_script_future_t *future);

/** Used with ecs_async_function_init(). */
typedef struct ecs_async_function_desc_t {
    /** Function name. */
    const char *name;

    /** Parent of function entity. */
    ecs_entity_t parent;

    /** Function parameters. */
    ecs_script_parameter_t params[FLECS_SCRIPT_FUNCTION_ARGS_MAX];

    /** Function return type. */
    ecs_entity_t return_type;

    /** Function implementation. Receives the future to complete instead of a
     * result output parameter. */
    ecs_async_function_callback_t callback;

    /** Invoked when a pending future of this function is cancelled
     * (optional). */
    ecs_async_function_cancel_t cancel;

    /** Context passed to function implementation. */
    void *ctx;
} ecs_async_function_desc_t;

/** Create new async function.
 * This operation creates a new async function that can be called from a
 * script. Async functions do not return their result directly, but instead
 * receive a future that can be completed at a later point in time (see
 * ecs_script_future_resolve()).
 *
 * Async function calls must be prefixed with the await keyword in scripts, and
 * can only be used by scripts that are evaluated as a task (see
 * ecs_script_task_new()).
 *
 * @param world The world.
 * @param desc Async function init parameters.
 * @return The function, or 0 if failed.
*/
FLECS_API
ecs_entity_t ecs_async_function_init(
    ecs_world_t *world,
    const ecs_async_function_desc_t *desc);

#define ecs_async_function(world, ...)\
    ecs_async_function_init(world, &(ecs_async_function_desc_t)__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

/** @} */

#endif
