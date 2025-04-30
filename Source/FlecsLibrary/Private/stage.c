/**
 * @file stage.c
 * @brief Staging implementation.
 * 
 * A stage is an object that can be used to temporarily store mutations to a
 * world while a world is in readonly mode. ECS operations that are invoked on
 * a stage are stored in a command buffer, which is flushed during sync points,
 * or manually by the user.
 * 
 * Stages contain additional state to enable other API functionality without
 * having to mutate the world, such as setting the current scope, and allocators
 * that are local to a stage.
 * 
 * In a multi threaded application, each thread has its own stage which allows
 * threads to insert mutations without having to lock administration.
 */

#include "private_api.h"

static
void flecs_stage_merge(
    ecs_world_t *world)
{
    bool is_stage = flecs_poly_is(world, ecs_stage_t);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    bool measure_frame_time = ECS_BIT_IS_SET(ecs_world_get_flags(world), 
        EcsWorldMeasureFrameTime);

    ecs_time_t t_start = {0};
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    ecs_dbg_3("#[magenta]merge");
    ecs_log_push_3();

    if (is_stage) {
        /* Check for consistency if force_merge is enabled. In practice this
         * function will never get called with force_merge disabled for just
         * a single stage. */
        ecs_assert(stage->defer == 1, ECS_INVALID_OPERATION, 
            "mismatching defer_begin/defer_end detected");
        flecs_defer_end(world, stage);
    } else {
        /* Merge stages. Only merge if the stage has auto_merging turned on, or 
         * if this is a forced merge (like when ecs_merge is called) */
        int32_t i, count = ecs_get_stage_count(world);
        for (i = 0; i < count; i ++) {
            ecs_stage_t *s = (ecs_stage_t*)ecs_get_stage(world, i);
            flecs_poly_assert(s, ecs_stage_t);
            flecs_defer_end(world, s);
        }
    }

    flecs_eval_component_monitors(world);

    if (measure_frame_time) {
        world->info.merge_time_total += (ecs_ftime_t)ecs_time_measure(&t_start);
    }

    world->info.merge_count_total ++; 

    /* If stage is unmanaged, deferring is always enabled */
    if (stage->id == -1) {
        flecs_defer_begin(world, stage);
    }
    
    ecs_log_pop_3();
}

void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Execute post frame actions */
    int32_t i, count = ecs_vec_count(&stage->post_frame_actions);
    ecs_action_elem_t *elems = ecs_vec_first(&stage->post_frame_actions);
    for (i = 0; i < count; i ++) {
        elems[i].action(world, elems[i].ctx);
    }

    ecs_vec_clear(&stage->post_frame_actions);
}

ecs_entity_t flecs_stage_set_system(
    ecs_stage_t *stage,
    ecs_entity_t system)
{
    ecs_entity_t old = stage->system;
    stage->system = system;
    return old;
}

static
ecs_stage_t* flecs_stage_new(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_stage_t *stage = ecs_os_calloc(sizeof(ecs_stage_t));
    flecs_poly_init(stage, ecs_stage_t);

    stage->world = world;
    stage->thread_ctx = world;

    flecs_stack_init(&stage->allocators.iter_stack);
    flecs_stack_init(&stage->allocators.deser_stack);
    flecs_allocator_init(&stage->allocator);
    flecs_ballocator_init_n(&stage->allocators.cmd_entry_chunk, ecs_cmd_entry_t,
        FLECS_SPARSE_PAGE_SIZE);
    flecs_ballocator_init_t(&stage->allocators.query_impl, ecs_query_impl_t);
    flecs_ballocator_init_t(&stage->allocators.query_cache, ecs_query_cache_t);

    ecs_allocator_t *a = &stage->allocator;
    ecs_vec_init_t(a, &stage->post_frame_actions, ecs_action_elem_t, 0);

    int32_t i;
    for (i = 0; i < 2; i ++) {
        flecs_commands_init(stage, &stage->cmd_stack[i]);
    }

    stage->cmd = &stage->cmd_stack[0];
    return stage;
}

static
void flecs_stage_free(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);

    flecs_poly_fini(stage, ecs_stage_t);

    ecs_allocator_t *a = &stage->allocator;
    
    ecs_vec_fini_t(a, &stage->post_frame_actions, ecs_action_elem_t);
    ecs_vec_fini(NULL, &stage->variables, 0);
    ecs_vec_fini(NULL, &stage->operations, 0);

    int32_t i;
    for (i = 0; i < 2; i ++) {
        flecs_commands_fini(stage, &stage->cmd_stack[i]);
    }

#ifdef FLECS_SCRIPT
    if (stage->runtime) {
        ecs_script_runtime_free(stage->runtime);
    }
#endif

    flecs_stack_fini(&stage->allocators.iter_stack);
    flecs_stack_fini(&stage->allocators.deser_stack);
    flecs_ballocator_fini(&stage->allocators.cmd_entry_chunk);
    flecs_ballocator_fini(&stage->allocators.query_impl);
    flecs_ballocator_fini(&stage->allocators.query_cache);
    flecs_allocator_fini(&stage->allocator);

    ecs_os_free(stage);
}

ecs_allocator_t* flecs_stage_get_allocator(
    ecs_world_t *world)
{
    ecs_stage_t *stage = flecs_stage_from_world(
        ECS_CONST_CAST(ecs_world_t**, &world));
    return &stage->allocator;
}

ecs_stack_t* flecs_stage_get_stack_allocator(
    ecs_world_t *world)
{
    ecs_stage_t *stage = flecs_stage_from_world(
        ECS_CONST_CAST(ecs_world_t**, &world));
    return &stage->allocators.iter_stack;
}

ecs_world_t* ecs_stage_new(
    ecs_world_t *world)
{
    ecs_stage_t *stage = flecs_stage_new(world);
    stage->id = -1;

    flecs_defer_begin(world, stage);

    return (ecs_world_t*)stage;
}

void ecs_stage_free(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_stage_t);
    ecs_stage_t *stage = (ecs_stage_t*)world;
    ecs_check(stage->id == -1, ECS_INVALID_PARAMETER, 
        "cannot free stage that's owned by world");
    flecs_stage_free(stage->world, stage);
error:
    return;
}

void ecs_set_stage_count(
    ecs_world_t *world,
    int32_t stage_count)
{
    flecs_poly_assert(world, ecs_world_t);

    /* World must have at least one default stage */
    ecs_assert(stage_count >= 1 || (world->flags & EcsWorldFini), 
        ECS_INTERNAL_ERROR, NULL);

    const ecs_entity_t *lookup_path = NULL;
    if (world->stage_count >= 1) {
        lookup_path = world->stages[0]->lookup_path;
    }

    int32_t i, count = world->stage_count;
    if (stage_count < count) {
        for (i = stage_count; i < count; i ++) {
            /* If stage contains a thread handle, ecs_set_threads was used to
             * create the stages. ecs_set_threads and ecs_set_stage_count should 
             * not be mixed. */
            ecs_stage_t *stage = world->stages[i];
            flecs_poly_assert(stage, ecs_stage_t);
            ecs_check(stage->thread == 0, ECS_INVALID_OPERATION, 
                "cannot mix using set_stage_count and set_threads");
            flecs_stage_free(world, stage);
        }
    }

    if (stage_count) {
        world->stages = ecs_os_realloc_n(
            world->stages, ecs_stage_t*, stage_count);

        for (i = count; i < stage_count; i ++) {
            ecs_stage_t *stage = world->stages[i] = flecs_stage_new(world);
            stage->id = i;

            /* Set thread_ctx to stage, as this stage might be used in a
             * multithreaded context */
            stage->thread_ctx = (ecs_world_t*)stage;
            stage->thread = 0;
        }
    } else {
        /* Set to NULL to prevent double frees */
        ecs_os_free(world->stages);
        world->stages = NULL;
    }

    /* Regardless of whether the stage was just initialized or not, when the
     * ecs_set_stage_count function is called, all stages inherit the auto_merge
     * property from the world */
    for (i = 0; i < stage_count; i ++) {
        world->stages[i]->lookup_path = lookup_path;
    }

    world->stage_count = stage_count;
error:
    return;
}

int32_t ecs_get_stage_count(
    const ecs_world_t *world)
{
    world = ecs_get_world(world);
    return world->stage_count;
}

int32_t ecs_stage_get_id(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (flecs_poly_is(world, ecs_stage_t)) {
        ecs_stage_t *stage = ECS_CONST_CAST(ecs_stage_t*, world);

        /* Index 0 is reserved for main stage */
        return stage->id;
    } else if (flecs_poly_is(world, ecs_world_t)) {
        return 0;
    } else {
        ecs_throw(ECS_INTERNAL_ERROR, NULL);
    }
error:
    return 0;
}

ecs_world_t* ecs_get_stage(
    const ecs_world_t *world,
    int32_t stage_id)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(world->stage_count > stage_id, ECS_INVALID_PARAMETER, NULL);
    return (ecs_world_t*)world->stages[stage_id];
error:
    return NULL;
}

bool ecs_readonly_begin(
    ecs_world_t *world,
    bool multi_threaded)
{
    flecs_poly_assert(world, ecs_world_t);

    ecs_dbg_3("#[bold]readonly");
    ecs_log_push_3();

    int32_t i, count = ecs_get_stage_count(world);
    for (i = 0; i < count; i ++) {
        ecs_stage_t *stage = world->stages[i];
        stage->lookup_path = world->stages[0]->lookup_path;
        ecs_assert(stage->defer == 0, ECS_INVALID_OPERATION, 
            "deferred mode cannot be enabled when entering readonly mode");
        flecs_defer_begin(world, stage);
    }

    bool is_readonly = ECS_BIT_IS_SET(world->flags, EcsWorldReadonly);

    /* From this point on, the world is "locked" for mutations, and it is only 
     * allowed to enqueue commands from stages */
    ECS_BIT_SET(world->flags, EcsWorldReadonly);
    ECS_BIT_COND(world->flags, EcsWorldMultiThreaded, multi_threaded);

    return is_readonly;
}

void ecs_readonly_end(
    ecs_world_t *world)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(world->flags & EcsWorldReadonly, ECS_INVALID_OPERATION,
        "world is not in readonly mode");

    /* After this it is safe again to mutate the world directly */
    ECS_BIT_CLEAR(world->flags, EcsWorldReadonly);
    ECS_BIT_CLEAR(world->flags, EcsWorldMultiThreaded);

    ecs_log_pop_3();

    flecs_stage_merge(world);
error:
    return;
}

ecs_world_t* flecs_suspend_readonly(
    const ecs_world_t *stage_world,
    ecs_suspend_readonly_state_t *state)
{
    ecs_assert(stage_world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(state != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_world_t *world =
        ECS_CONST_CAST(ecs_world_t*, ecs_get_world(stage_world));
    flecs_poly_assert(world, ecs_world_t);

    bool is_readonly = ECS_BIT_IS_SET(world->flags, EcsWorldReadonly);
    ecs_world_t *temp_world = world;
    ecs_stage_t *stage = flecs_stage_from_world(&temp_world);

    if (!is_readonly && !stage->defer) {
        state->is_readonly = false;
        state->is_deferred = false;
        return world;
    }

    ecs_dbg_3("suspending readonly mode");

    /* Cannot suspend when running with multiple threads */
    ecs_assert(!(world->flags & EcsWorldReadonly) ||
        !(world->flags & EcsWorldMultiThreaded), 
            ECS_INVALID_WHILE_READONLY, NULL);

    state->is_readonly = is_readonly;
    state->is_deferred = stage->defer != 0;
    state->cmd_flushing = stage->cmd_flushing;

    /* Silence readonly checks */
    world->flags &= ~EcsWorldReadonly;
    stage->cmd_flushing = false;

    /* Hack around safety checks (this ought to look ugly) */
    state->defer_count = stage->defer;
    state->cmd_stack[0] = stage->cmd_stack[0];
    state->cmd_stack[1] = stage->cmd_stack[1];
    state->cmd = stage->cmd;

    flecs_commands_init(stage, &stage->cmd_stack[0]);
    flecs_commands_init(stage, &stage->cmd_stack[1]);
    stage->cmd = &stage->cmd_stack[0];

    state->scope = stage->scope;
    state->with = stage->with;
    stage->defer = 0;

    return world;
}

void flecs_resume_readonly(
    ecs_world_t *world,
    ecs_suspend_readonly_state_t *state)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(state != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_world_t *temp_world = world;
    ecs_stage_t *stage = flecs_stage_from_world(&temp_world);

    if (state->is_readonly || state->is_deferred) {
        ecs_dbg_3("resuming readonly mode");

        ecs_run_aperiodic(world, 0);

        /* Restore readonly state / defer count */
        ECS_BIT_COND(world->flags, EcsWorldReadonly, state->is_readonly);
        stage->defer = state->defer_count;
        stage->cmd_flushing = state->cmd_flushing;
        flecs_commands_fini(stage, &stage->cmd_stack[0]);
        flecs_commands_fini(stage, &stage->cmd_stack[1]);
        stage->cmd_stack[0] = state->cmd_stack[0];
        stage->cmd_stack[1] = state->cmd_stack[1];
        stage->cmd = state->cmd;
        
        stage->scope = state->scope;
        stage->with = state->with;
    }
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(flecs_poly_is(world, ecs_world_t) || 
               flecs_poly_is(world, ecs_stage_t), ECS_INVALID_PARAMETER, NULL);
    flecs_stage_merge(world);
error:
    return;
}

bool ecs_stage_is_readonly(
    const ecs_world_t *stage)
{
    const ecs_world_t *world = ecs_get_world(stage);

    if (flecs_poly_is(stage, ecs_stage_t)) {
        if (((const ecs_stage_t*)stage)->id == -1) {
            /* Stage is not owned by world, so never readonly */
            return false;
        }
    }

    if (world->flags & EcsWorldReadonly) {
        if (flecs_poly_is(stage, ecs_world_t)) {
            return true;
        }
    } else {
        if (flecs_poly_is(stage, ecs_stage_t)) {
            return true;
        }
    }

    return false;
}

void ecs_stage_shrink(
    ecs_stage_t *stage)
{
    flecs_sparse_shrink(&stage->cmd_stack[0].entries);
    flecs_sparse_shrink(&stage->cmd_stack[1].entries);
    ecs_vec_reclaim_t(&stage->allocator, &stage->cmd_stack[0].queue, ecs_cmd_t);
    ecs_vec_reclaim_t(&stage->allocator, &stage->cmd_stack[1].queue, ecs_cmd_t);
}

bool ecs_is_deferred(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->defer > 0;
error:
    return false;
}
