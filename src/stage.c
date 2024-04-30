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
ecs_cmd_t* flecs_cmd_new(
    ecs_stage_t *stage)
{
    ecs_cmd_t *cmd = ecs_vec_append_t(&stage->allocator, &stage->cmd->queue, 
        ecs_cmd_t);
    cmd->is._1.value = NULL;
    cmd->next_for_entity = 0;
    cmd->entry = NULL;
    cmd->system = stage->system;
    return cmd;
}

static
ecs_cmd_t* flecs_cmd_new_batched(
    ecs_stage_t *stage, 
    ecs_entity_t e)
{
    ecs_vec_t *cmds = &stage->cmd->queue;
    ecs_cmd_entry_t *entry = flecs_sparse_get_any_t(
        &stage->cmd->entries, ecs_cmd_entry_t, e);

    int32_t cur = ecs_vec_count(cmds);
    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    if (entry) {
        if (entry->first == -1) {
            /* Existing but invalidated entry */
            entry->first = cur;
            cmd->entry = entry;
        } else {
            int32_t last = entry->last;
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
    } else {
        cmd->entry = entry = flecs_sparse_ensure_fast_t(
            &stage->cmd->entries, ecs_cmd_entry_t, e);
        entry->first = cur;
    }

    entry->last = cur;

    return cmd;
}

static
void flecs_stage_merge(
    ecs_world_t *world)
{
    bool is_stage = flecs_poly_is(world, ecs_stage_t);
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

bool flecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    flecs_poly_assert(world, ecs_world_t);
    flecs_poly_assert(stage, ecs_stage_t);
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
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
        if (cmd) {
            cmd->kind = EcsCmdModified;
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
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdClone;
        cmd->id = src;
        cmd->entity = entity;
        cmd->is._1.clone_value = clone_value;
        return true;
    }
    return false;   
}

bool flecs_defer_path(
    ecs_stage_t *stage,
    ecs_entity_t parent,
    ecs_entity_t entity,
    const char *name)
{
    if (stage->defer > 0) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdPath;
        cmd->entity = entity;
        cmd->id = parent;
        cmd->is._1.value = ecs_os_strdup(name);
        return true;
    }
    return false;
}

bool flecs_defer_delete(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdDelete;
        cmd->entity = entity;
        return true;
    }
    return false;
}

bool flecs_defer_clear(
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_cmd(stage)) {
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
        cmd->kind = EcsCmdClear;
        cmd->entity = entity;
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
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdOnDeleteAction;
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
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = enable ? EcsCmdEnable : EcsCmdDisable;
        cmd->entity = entity;
        cmd->id = id;
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
            ids[i] = ecs_new(world);
        }

        *ids_out = ids;

        /* Store data in op */
        ecs_cmd_t *cmd = flecs_cmd_new(stage);
        cmd->kind = EcsCmdBulkNew;
        cmd->id = id;
        cmd->is._n.entities = ids;
        cmd->is._n.count = count;
        cmd->entity = 0;
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
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);
        cmd->kind = EcsCmdAdd;
        cmd->id = id;
        cmd->entity = entity;
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
        ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity); 
        cmd->kind = EcsCmdRemove;
        cmd->id = id;
        cmd->entity = entity;
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
    bool *is_new)
{
    ecs_cmd_t *cmd = flecs_cmd_new_batched(stage, entity);

    /* Find type info for id */
    const ecs_type_info_t *ti = NULL;
    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        /* If idr doesn't exist yet, create it but only if the 
         * application is not multithreaded. */
        if (world->flags & EcsWorldMultiThreaded) {
            ti = ecs_get_type_info(world, id);
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
    ecs_check(ti != NULL, ECS_INVALID_PARAMETER, 
        "provided component is not a type");

    /* Make sure the size of the value equals the type size */
    ecs_assert(!size || size == ti->size, ECS_INVALID_PARAMETER,
        "mismatching size specified for component in ensure/emplace/set");
    size = ti->size;

    /* Find existing component. Make sure it's owned, so that we won't use the
     * component of a prefab. */
    void *existing = NULL;
    ecs_table_t *table = NULL;
    if (idr) {
        /* Entity can only have existing component if id record exists */
        ecs_record_t *r = flecs_entities_get(world, entity);
        table = r->table;
        if (r && table) {
            const ecs_table_record_t *tr = flecs_id_record_get_table(
                idr, table);
            if (tr) {
                ecs_assert(tr->column != -1, ECS_NOT_A_COMPONENT, NULL);
                /* Entity has the component */
                ecs_vec_t *column = &table->data.columns[tr->column].data;
                existing = ecs_vec_get(column, size, ECS_RECORD_TO_ROW(r->row));
            }
        }
    }

    /* Get existing value from storage */
    void *cmd_value = existing;
    bool emplace = cmd_kind == EcsCmdEmplace;

    /* If the component does not yet exist, create a temporary value. This is
     * necessary so we can store a component value in the deferred command,
     * without adding the component to the entity which is not allowed in 
     * deferred mode. */
    if (!existing) {
        ecs_stack_t *stack = &stage->cmd->stack;
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
        cmd = flecs_cmd_new(stage);
    }

    if (!existing) {
        /* If component didn't exist yet, insert command that will create it */
        cmd->kind = cmd_kind;
        cmd->id = id;
        cmd->idr = idr;
        cmd->entity = entity;
        cmd->is._1.size = size;
        cmd->is._1.value = cmd_value;

        if (is_new) {
            *is_new = true;
        }
    } else {
        /* If component already exists, still insert an Add command to ensure
         * that any preceding remove commands won't remove the component. If the
         * operation is a set, also insert a Modified command. */
        if (cmd_kind == EcsCmdSet) {
            cmd->kind = EcsCmdAddModified; 
        } else {
            cmd->kind = EcsCmdAdd;
        }
        cmd->id = id;
        cmd->entity = entity;

        if (is_new) {
            *is_new = false;
        }
    }

    return cmd_value;
error:
    return NULL;
}

void flecs_enqueue(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_event_desc_t *desc)
{
    ecs_cmd_t *cmd = flecs_cmd_new(stage);
    cmd->kind = EcsCmdEvent;
    cmd->entity = desc->entity;

    ecs_stack_t *stack = &stage->cmd->stack;
    ecs_event_desc_t *desc_cmd = flecs_stack_alloc_t(stack, ecs_event_desc_t);
    ecs_os_memcpy_t(desc_cmd, desc, ecs_event_desc_t);

    if (desc->ids && desc->ids->count != 0) {
        ecs_type_t *type_cmd = flecs_stack_alloc_t(stack, ecs_type_t);
        int32_t id_count = desc->ids->count;
        type_cmd->count = id_count;
        type_cmd->array = flecs_stack_alloc_n(stack, ecs_id_t, id_count);
        ecs_os_memcpy_n(type_cmd->array, desc->ids->array, ecs_id_t, id_count);
        desc_cmd->ids = type_cmd;
    } else {
        desc_cmd->ids = NULL;
    }

    cmd->is._1.value = desc_cmd;
    cmd->is._1.size = ECS_SIZEOF(ecs_event_desc_t);

    if (desc->param || desc->const_param) {
        ecs_assert(!(desc->const_param && desc->param), ECS_INVALID_PARAMETER, 
            "cannot set param and const_param at the same time");

        const ecs_type_info_t *ti = ecs_get_type_info(world, desc->event);
        ecs_assert(ti != NULL, ECS_INVALID_PARAMETER, 
            "can only enqueue events with data for events that are components");

        void *param_cmd = flecs_stack_alloc(stack, ti->size, ti->alignment);
        ecs_assert(param_cmd != NULL, ECS_INTERNAL_ERROR, NULL);
        if (desc->param) {
            if (ti->hooks.move_ctor) {
                ti->hooks.move_ctor(param_cmd, desc->param, 1, ti);
            } else {
                ecs_os_memcpy(param_cmd, desc->param, ti->size);
            }
        } else {
            if (ti->hooks.copy_ctor) {
                ti->hooks.copy_ctor(param_cmd, desc->const_param, 1, ti);
            } else {
                ecs_os_memcpy(param_cmd, desc->const_param, ti->size);
            }
        }

        desc_cmd->param = param_cmd;
        desc_cmd->const_param = NULL;
    }
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

static
void flecs_commands_init(
    ecs_stage_t *stage,
    ecs_commands_t *cmd)
{
    flecs_stack_init(&cmd->stack);
    ecs_vec_init_t(&stage->allocator, &cmd->queue, ecs_cmd_t, 0);
    flecs_sparse_init_t(&cmd->entries, &stage->allocator,
        &stage->allocators.cmd_entry_chunk, ecs_cmd_entry_t);
}

static
void flecs_commands_fini(
    ecs_stage_t *stage,
    ecs_commands_t *cmd)
{
    /* Make sure stage has no unmerged data */
    ecs_assert(ecs_vec_count(&stage->cmd->queue) == 0, ECS_INTERNAL_ERROR, NULL);

    flecs_stack_fini(&cmd->stack);
    ecs_vec_fini_t(&stage->allocator, &cmd->queue, ecs_cmd_t);
    flecs_sparse_fini(&cmd->entries);
}

void flecs_commands_push(
    ecs_stage_t *stage)
{
    int32_t sp = ++ stage->cmd_sp;
    ecs_assert(sp < ECS_MAX_DEFER_STACK, ECS_INTERNAL_ERROR, NULL);
    stage->cmd = &stage->cmd_stack[sp];
}

void flecs_commands_pop(
    ecs_stage_t *stage)
{
    int32_t sp = -- stage->cmd_sp;
    ecs_assert(sp >= 0, ECS_INTERNAL_ERROR, NULL);
    stage->cmd = &stage->cmd_stack[sp];
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
    for (i = 0; i < ECS_MAX_DEFER_STACK; i ++) {
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
    for (i = 0; i < ECS_MAX_DEFER_STACK; i ++) {
        flecs_commands_fini(stage, &stage->cmd_stack[i]);
    }

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

    flecs_process_pending_tables(world);

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

bool ecs_is_deferred(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->defer > 0;
error:
    return false;
}
