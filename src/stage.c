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
ecs_cmd_t* flecs_cmd_alloc(
    ecs_stage_t *stage)
{
    ecs_cmd_t *cmd = ecs_vec_append_t(&stage->allocator, &stage->commands, 
        ecs_cmd_t);
    ecs_os_zeromem(cmd);
    return cmd;
}

static
ecs_cmd_t* flecs_cmd_new(
    ecs_stage_t *stage, 
    ecs_entity_t e, 
    bool is_delete,
    bool can_batch) 
{
    if (e) {
        ecs_vec_t *cmds = &stage->commands;
        ecs_cmd_entry_t *entry = flecs_sparse_try_t(
            &stage->cmd_entries, ecs_cmd_entry_t, e);
        int32_t cur = ecs_vec_count(cmds);
        if (entry) {
            int32_t last = entry->last;
            if (entry->last == -1) {
                /* Entity was deleted, don't insert command */
                return NULL;
            }

            if (can_batch) {
                ecs_cmd_t *arr = ecs_vec_first_t(cmds, ecs_cmd_t);
                ecs_assert(arr[last].entity == e, ECS_INTERNAL_ERROR, NULL);
                ecs_cmd_t *last_op = &arr[last];
                last_op->next_for_entity = cur;
                if (last == entry->first) {
                    /* Flip sign bit so flush logic can tell which command
                     * is the first for an entity */
                    last_op->next_for_entity *= -1;
                }
            }
        } else if (can_batch || is_delete) {
            entry = flecs_sparse_ensure_t(&stage->cmd_entries, 
                ecs_cmd_entry_t, e);
            entry->first = cur;
        }
        if (can_batch) {
            entry->last = cur;
        }
        if (is_delete) {
            /* Prevent insertion of more commands for entity */
            entry->last = -1;
        }
    }

    return flecs_cmd_alloc(stage);
}

static
void flecs_stages_merge(
    ecs_world_t *world,
    bool force_merge)
{
    bool is_stage = ecs_poly_is(world, ecs_stage_t);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    bool measure_frame_time = ECS_BIT_IS_SET(world->flags, 
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
        if (force_merge || stage->auto_merge) {
            ecs_assert(stage->defer == 1, ECS_INVALID_OPERATION, 
                "mismatching defer_begin/defer_end detected");
            flecs_defer_end(world, stage);
        }
    } else {
        /* Merge stages. Only merge if the stage has auto_merging turned on, or 
         * if this is a forced merge (like when ecs_merge is called) */
        int32_t i, count = ecs_get_stage_count(world);
        for (i = 0; i < count; i ++) {
            ecs_stage_t *s = (ecs_stage_t*)ecs_get_stage(world, i);
            ecs_poly_assert(s, ecs_stage_t);
            if (force_merge || s->auto_merge) {
                flecs_defer_end(world, s);
            }
        }
    }

    flecs_eval_component_monitors(world);

    if (measure_frame_time) {
        world->info.merge_time_total += (ecs_ftime_t)ecs_time_measure(&t_start);
    }

    world->info.merge_count_total ++; 

    /* If stage is asynchronous, deferring is always enabled */
    if (stage->async) {
        flecs_defer_begin(world, stage);
    }
    
    ecs_log_pop_3();
}

static
void flecs_stage_auto_merge(
    ecs_world_t *world)
{
    flecs_stages_merge(world, false);
}

static
void flecs_stage_manual_merge(
    ecs_world_t *world)
{
    flecs_stages_merge(world, true);
}

bool flecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_assert(stage, ecs_stage_t);
    (void)world;
    if (stage->defer < 0) return false;
    return (++ stage->defer) == 1;
}

bool flecs_defer_cmd(
    ecs_stage_t *stage)
{
    if (stage->defer) {
        return (stage->defer > 0);
    }

    stage->defer ++;
    return false;
}

bool flecs_defer_modified(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, false);
        if (cmd) {
            cmd->kind = EcsOpModified;
            cmd->id = id;
            cmd->entity = entity;
        }
        return true;
    }
    return false;
}

bool flecs_defer_clone(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value)
{   
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, false);
        if (cmd) {
            cmd->kind = EcsOpClone;
            cmd->id = src;
            cmd->entity = entity;
            cmd->is._1.clone_value = clone_value;
        }
        return true;
    }
    return false;   
}

bool flecs_defer_delete(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, true, false);
        if (cmd) {
            cmd->kind = EcsOpDelete;
            cmd->entity = entity;
        }
        return true;
    }
    return false;
}

bool flecs_defer_clear(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, true);
        if (cmd) {
            cmd->kind = EcsOpClear;
            cmd->entity = entity;
        }
        return true;
    }
    return false;
}

bool flecs_defer_on_delete_action(
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_alloc(stage);
        cmd->kind = EcsOpOnDeleteAction;
        cmd->id = id;
        cmd->entity = action;
        return true;
    }
    return false;
}

bool flecs_defer_enable(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, false);
        if (cmd) {
            cmd->kind = enable ? EcsOpEnable : EcsOpDisable;
            cmd->entity = entity;
            cmd->id = id;
        }
        return true;
    }
    return false;
}

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    ecs_id_t id,
    const ecs_entity_t **ids_out)
{
    if (flecs_defer_cmd(stage)) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));

        /* Use ecs_new_id as this is thread safe */
        int i;
        for (i = 0; i < count; i ++) {
            ids[i] = ecs_new_id(world);
        }

        *ids_out = ids;

        /* Store data in op */
        ecs_cmd_t *cmd = flecs_cmd_alloc(stage);
        if (cmd) {
            cmd->kind = EcsOpBulkNew;
            cmd->id = id;
            cmd->is._n.entities = ids;
            cmd->is._n.count = count;
        }

        return true;
    }
    return false;
}

bool flecs_defer_add(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage)) {
        ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, true);
        if (cmd) {
            cmd->kind = EcsOpAdd;
            cmd->id = id;
            cmd->entity = entity;
        }
        return true;
    }
    return false;
}

bool flecs_defer_remove(
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_cmd(stage)) {
        ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, true);
        if (cmd) {
            cmd->kind = EcsOpRemove;
            cmd->id = id;
            cmd->entity = entity;
        }
        return true;
    }
    return false;
}

void* flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_cmd_kind_t cmd_kind,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    void *value,
    bool need_value)
{
    ecs_cmd_t *cmd = flecs_cmd_new(stage, entity, false, true);
    if (!cmd) {
        if (need_value) {
            /* Entity is deleted by a previous command, but we still need to 
             * return a temporary storage to the application. */
            cmd_kind = EcsOpSkip;
        } else {
            /* No value needs to be returned, we can drop the command */
            return NULL;
        }
    }

    /* Find type info for id */
    const ecs_type_info_t *ti = NULL;
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        /* If idr doesn't exist yet, create it but only if the 
         * application is not multithreaded. */
        if (stage->async || (world->flags & EcsWorldMultiThreaded)) {
            ti = ecs_get_type_info(world, id);
            ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, NULL);
        } else {
            /* When not in multi threaded mode, it's safe to find or 
             * create the id record. */
            idr = flecs_id_record_ensure(world, id);
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            
            /* Get type_info from id record. We could have called 
             * ecs_get_type_info directly, but since this function can be
             * expensive for pairs, creating the id record ensures we can
             * find the type_info quickly for subsequent operations. */
            ti = idr->type_info;
        }
    } else {
        ti = idr->type_info;
    }

    /* If the id isn't associated with a type, we can't set anything */
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Make sure the size of the value equals the type size */
    ecs_assert(!size || size == ti->size, ECS_INVALID_PARAMETER, NULL);
    size = ti->size;

    /* Find existing component. Make sure it's owned, so that we won't use the
     * component of a prefab. */
    void *existing = NULL;
    ecs_table_t *table = NULL, *storage_table;
    if (idr) {
        /* Entity can only have existing component if id record exists */
        ecs_record_t *r = flecs_entities_get(world, entity);
        table = r->table;
        if (r && table && (storage_table = table->storage_table)) {
            const ecs_table_record_t *tr = flecs_id_record_get_table(
                idr, storage_table);
            if (tr) {
                /* Entity has the component */
                ecs_vec_t *column = &table->data.columns[tr->column];
                existing = ecs_vec_get(column, size, ECS_RECORD_TO_ROW(r->row));
            }
        }
    }

    /* Get existing value from storage */
    void *cmd_value = existing;
    bool emplace = cmd_kind == EcsOpEmplace;

    /* If the component does not yet exist, create a temporary value. This is
     * necessary so we can store a component value in the deferred command,
     * without adding the component to the entity which is not allowed in 
     * deferred mode. */
    if (!existing) {
        ecs_stack_t *stack = &stage->defer_stack;
        cmd_value = flecs_stack_alloc(stack, size, ti->alignment);

        /* If the component doesn't yet exist, construct it and move the 
         * provided value into the component, if provided. Don't construct if
         * this is an emplace operation, in which case the application is
         * responsible for constructing. */
        if (value) {
            if (emplace) {
                ecs_move_t move = ti->hooks.move_ctor;
                if (move) {
                    move(cmd_value, value, 1, ti);
                } else {
                    ecs_os_memcpy(cmd_value, value, size);
                }
            } else {
                ecs_copy_t copy = ti->hooks.copy_ctor;
                if (copy) {
                    copy(cmd_value, value, 1, ti);
                } else {
                    ecs_os_memcpy(cmd_value, value, size);
                }
            }
        } else if (!emplace) {
            /* If the command is not an emplace, construct the temp storage */

            /* Check if entity inherits component */
            void *base = NULL;
            if (table && (table->flags & EcsTableHasIsA)) {
                base = flecs_get_base_component(world, table, id, idr, 0);
            }

            if (!base) {
                /* Normal ctor */
                ecs_xtor_t ctor = ti->hooks.ctor;
                if (ctor) {
                    ctor(cmd_value, 1, ti);
                }
            } else {
                /* Override */
                ecs_copy_t copy = ti->hooks.copy_ctor;
                if (copy) {
                    copy(cmd_value, base, 1, ti);
                } else {
                    ecs_os_memcpy(cmd_value, base, size);
                }
            }
        }
    } else if (value) {
        /* If component exists and value is provided, copy */
        ecs_copy_t copy = ti->hooks.copy;
        if (copy) {
            copy(existing, value, 1, ti);
        } else {
            ecs_os_memcpy(existing, value, size);
        }
    }

    if (!cmd) {
        /* If cmd is NULL, entity was already deleted. Check if we need to
         * insert a command into the queue. */
        if (!ti->hooks.dtor) {
            /* If temporary memory does not need to be destructed, it'll get 
             * freed when the stack allocator is reset. This prevents us
             * from having to insert a command when the entity was 
             * already deleted. */
            return cmd_value;
        }
        cmd = flecs_cmd_alloc(stage);
    }

    if (!existing) {
        /* If component didn't exist yet, insert command that will create it */
        cmd->kind = cmd_kind;
        cmd->id = id;
        cmd->idr = idr;
        cmd->entity = entity;
        cmd->is._1.size = size;
        cmd->is._1.value = cmd_value;
    } else {
        /* If component already exists, still insert an Add command to ensure
         * that any preceding remove commands won't remove the component. If the
         * operation is a set, also insert a Modified command. */
        if (cmd_kind == EcsOpSet) {
            cmd->kind = EcsOpAddModified; 
        } else {
            cmd->kind = EcsOpAdd;
        }
        cmd->id = id;
        cmd->entity = entity;
    }

    return cmd_value;
error:
    return NULL;
}

void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Execute post frame actions */
    ecs_vector_each(stage->post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });

    ecs_vector_free(stage->post_frame_actions);
    stage->post_frame_actions = NULL;
}

void flecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_init(stage, ecs_stage_t);

    stage->world = world;
    stage->thread_ctx = world;
    stage->auto_merge = true;
    stage->async = false;

    flecs_stack_init(&stage->defer_stack);
    flecs_stack_init(&stage->allocators.iter_stack);
    flecs_stack_init(&stage->allocators.deser_stack);
    flecs_allocator_init(&stage->allocator);
    flecs_ballocator_init_n(&stage->allocators.cmd_entry_chunk, ecs_cmd_entry_t,
        FLECS_SPARSE_CHUNK_SIZE);

    ecs_vec_init_t(&stage->allocator, &stage->commands, ecs_cmd_t, 0);
    flecs_sparse_init_t(&stage->cmd_entries, &stage->allocator,
        &stage->allocators.cmd_entry_chunk, ecs_cmd_entry_t);
}

void flecs_stage_fini(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_assert(stage, ecs_stage_t);

    /* Make sure stage has no unmerged data */
    ecs_assert(ecs_vec_count(&stage->commands) == 0, ECS_INTERNAL_ERROR, NULL);

    ecs_poly_fini(stage, ecs_stage_t);

    flecs_sparse_fini(&stage->cmd_entries);

    ecs_vec_fini_t(&stage->allocator, &stage->commands, ecs_cmd_t);
    flecs_stack_fini(&stage->defer_stack);
    flecs_stack_fini(&stage->allocators.iter_stack);
    flecs_stack_fini(&stage->allocators.deser_stack);
    flecs_ballocator_fini(&stage->allocators.cmd_entry_chunk);
    flecs_allocator_fini(&stage->allocator);
}

void ecs_set_stage_count(
    ecs_world_t *world,
    int32_t stage_count)
{
    ecs_poly_assert(world, ecs_world_t);

    /* World must have at least one default stage */
    ecs_assert(stage_count >= 1 || (world->flags & EcsWorldFini), 
        ECS_INTERNAL_ERROR, NULL);

    bool auto_merge = true;
    ecs_entity_t *lookup_path = NULL;
    ecs_entity_t scope = 0;
    ecs_entity_t with = 0;
    if (world->stage_count >= 1) {
        auto_merge = world->stages[0].auto_merge;
        lookup_path = world->stages[0].lookup_path;
        scope = world->stages[0].scope;
        with = world->stages[0].with;
    }

    int32_t i, count = world->stage_count;
    if (count && count != stage_count) {
        ecs_stage_t *stages = world->stages;

        for (i = 0; i < count; i ++) {
            /* If stage contains a thread handle, ecs_set_threads was used to
             * create the stages. ecs_set_threads and ecs_set_stage_count should not
             * be mixed. */
            ecs_poly_assert(&stages[i], ecs_stage_t);
            ecs_check(stages[i].thread == 0, ECS_INVALID_OPERATION, NULL);
            flecs_stage_fini(world, &stages[i]);
        }

        ecs_os_free(world->stages);
    }

    if (stage_count) {
        world->stages = ecs_os_malloc_n(ecs_stage_t, stage_count);

        for (i = 0; i < stage_count; i ++) {
            ecs_stage_t *stage = &world->stages[i];
            flecs_stage_init(world, stage);
            stage->id = i;

            /* Set thread_ctx to stage, as this stage might be used in a
             * multithreaded context */
            stage->thread_ctx = (ecs_world_t*)stage;
        }
    } else {
        /* Set to NULL to prevent double frees */
        world->stages = NULL;
    }

    /* Regardless of whether the stage was just initialized or not, when the
     * ecs_set_stage_count function is called, all stages inherit the auto_merge
     * property from the world */
    for (i = 0; i < stage_count; i ++) {
        world->stages[i].auto_merge = auto_merge;
        world->stages[i].lookup_path = lookup_path;
        world->stages[0].scope = scope;
        world->stages[0].with = with;
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

int32_t ecs_get_stage_id(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ecs_poly_is(world, ecs_stage_t)) {
        ecs_stage_t *stage = (ecs_stage_t*)world;

        /* Index 0 is reserved for main stage */
        return stage->id;
    } else if (ecs_poly_is(world, ecs_world_t)) {
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
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(world->stage_count > stage_id, ECS_INVALID_PARAMETER, NULL);
    return (ecs_world_t*)&world->stages[stage_id];
error:
    return NULL;
}

bool ecs_readonly_begin(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);

    flecs_process_pending_tables(world);

    ecs_dbg_3("#[bold]readonly");
    ecs_log_push_3();

    int32_t i, count = ecs_get_stage_count(world);
    for (i = 0; i < count; i ++) {
        ecs_stage_t *stage = &world->stages[i];
        stage->lookup_path = world->stages[0].lookup_path;
        ecs_assert(stage->defer == 0, ECS_INVALID_OPERATION, 
            "deferred mode cannot be enabled when entering readonly mode");
        flecs_defer_begin(world, stage);
    }

    bool is_readonly = ECS_BIT_IS_SET(world->flags, EcsWorldReadonly);

    /* From this point on, the world is "locked" for mutations, and it is only 
     * allowed to enqueue commands from stages */
    ECS_BIT_SET(world->flags, EcsWorldReadonly);

    /* If world has more than one stage, signal we might be running on multiple
     * threads. This is a stricter version of readonly mode: while some 
     * mutations like implicit component registration are still allowed in plain
     * readonly mode, no mutations are allowed when multithreaded. */
    if (world->worker_cond) {
        ECS_BIT_SET(world->flags, EcsWorldMultiThreaded);
    }

    return is_readonly;
}

void ecs_readonly_end(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(world->flags & EcsWorldReadonly, ECS_INVALID_OPERATION, NULL);

    /* After this it is safe again to mutate the world directly */
    ECS_BIT_CLEAR(world->flags, EcsWorldReadonly);
    ECS_BIT_CLEAR(world->flags, EcsWorldMultiThreaded);

    ecs_log_pop_3();

    flecs_stage_auto_merge(world);
error:
    return;
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_poly_is(world, ecs_world_t) || 
               ecs_poly_is(world, ecs_stage_t), ECS_INVALID_PARAMETER, NULL);
    flecs_stage_manual_merge(world);
error:
    return;
}

void ecs_set_automerge(
    ecs_world_t *world,
    bool auto_merge)
{
    /* If a world is provided, set auto_merge globally for the world. This
     * doesn't actually do anything (the main stage never merges) but it serves
     * as the default for when stages are created. */
    if (ecs_poly_is(world, ecs_world_t)) {
        world->stages[0].auto_merge = auto_merge;

        /* Propagate change to all stages */
        int i, stage_count = ecs_get_stage_count(world);
        for (i = 0; i < stage_count; i ++) {
            ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
            stage->auto_merge = auto_merge;
        }

    /* If a stage is provided, override the auto_merge value for the individual
     * stage. This allows an application to control per-stage which stage should
     * be automatically merged and which one shouldn't */
    } else {
        ecs_poly_assert(world, ecs_stage_t);
        ecs_stage_t *stage = (ecs_stage_t*)world;
        stage->auto_merge = auto_merge;
    }
}

bool ecs_stage_is_readonly(
    const ecs_world_t *stage)
{
    const ecs_world_t *world = ecs_get_world(stage);

    if (ecs_poly_is(stage, ecs_stage_t)) {
        if (((ecs_stage_t*)stage)->async) {
            return false;
        }
    }

    if (world->flags & EcsWorldReadonly) {
        if (ecs_poly_is(stage, ecs_world_t)) {
            return true;
        }
    } else {
        if (ecs_poly_is(stage, ecs_stage_t)) {
            return true;
        }
    }

    return false;
}

ecs_world_t* ecs_async_stage_new(
    ecs_world_t *world)
{
    ecs_stage_t *stage = ecs_os_calloc(sizeof(ecs_stage_t));
    flecs_stage_init(world, stage);

    stage->id = -1;
    stage->auto_merge = false;
    stage->async = true;

    flecs_defer_begin(world, stage);

    return (ecs_world_t*)stage;
}

void ecs_async_stage_free(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_stage_t);
    ecs_stage_t *stage = (ecs_stage_t*)world;
    ecs_check(stage->async == true, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_fini(stage->world, stage);
    ecs_os_free(stage);
error:
    return;
}

bool ecs_stage_is_async(
    ecs_world_t *stage)
{
    if (!stage) {
        return false;
    }
    
    if (!ecs_poly_is(stage, ecs_stage_t)) {
        return false;
    }

    return ((ecs_stage_t*)stage)->async;
}

bool ecs_is_deferred(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->defer != 0;
error:
    return false;
}
