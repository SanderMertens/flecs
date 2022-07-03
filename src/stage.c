#include "private_api.h"

static
ecs_defer_op_t* new_defer_op(ecs_stage_t *stage) {
    ecs_defer_op_t *result = ecs_vector_add(&stage->defer_queue, ecs_defer_op_t);
    ecs_os_memset(result, 0, ECS_SIZEOF(ecs_defer_op_t));
    return result;
}

static
void merge_stages(
    ecs_world_t *world,
    bool force_merge)
{
    bool is_stage = ecs_poly_is(world, ecs_stage_t);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    bool measure_frame_time = ECS_BIT_IS_SET(world->flags, 
        EcsWorldMeasureFrameTime);

    ecs_time_t t_start;
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    ecs_dbg_3("#[magenta]merge");

    if (is_stage) {
        /* Check for consistency if force_merge is enabled. In practice this
         * function will never get called with force_merge disabled for just
         * a single stage. */
        if (force_merge || stage->auto_merge) {
            ecs_assert(stage->defer == 1, ECS_INVALID_OPERATION, 
                "mismatching defer_begin/defer_end detected");
            ecs_defer_end((ecs_world_t*)stage);
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
        world->info.merge_time_total += (float)ecs_time_measure(&t_start);
    }

    world->info.merge_count_total ++; 

    /* If stage is asynchronous, deferring is always enabled */
    if (stage->asynchronous) {
        ecs_defer_begin((ecs_world_t*)stage);
    }
}

static
void do_auto_merge(
    ecs_world_t *world)
{
    merge_stages(world, false);
}

static
void do_manual_merge(
    ecs_world_t *world)
{
    merge_stages(world, true);
}

bool flecs_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    if (stage->defer_suspend) return false;
    return (++ stage->defer) == 1;
}

static
bool flecs_defer_op(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;

    /* If deferring is suspended, do operation as usual */
    if (stage->defer_suspend) return false;

    if (stage->defer) {
        /* If deferring is enabled, defer operation */
        return true;
    }

    /* Deferring is disabled, defer while operation is executed */
    stage->defer ++;
    return false;
}

static
bool flecs_defer_add_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_defer_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_op(world, stage)) {
        if (!id) {
            return true;
        }

        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->id = id;
        op->is._1.entity = entity;

        if (op_kind == EcsOpNew) {
            world->info.new_count ++;
        } else if (op_kind == EcsOpAdd) {
            world->info.add_count ++;
        } else if (op_kind == EcsOpRemove) {
            world->info.remove_count ++;
        }

        return true;
    }
    return false;
}

bool flecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (flecs_defer_op(world, stage)) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpModified;
        op->id = id;
        op->is._1.entity = entity;
        return true;
    }
    return false;
}

bool flecs_defer_clone(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value)
{   
    if (flecs_defer_op(world, stage)) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClone;
        op->id = src;
        op->is._1.entity = entity;
        op->is._1.clone_value = clone_value;
        return true;
    }
    return false;   
}

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_op(world, stage)) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpDelete;
        op->is._1.entity = entity;
        world->info.delete_count ++;
        return true;
    }
    return false;
}

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    if (flecs_defer_op(world, stage)) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClear;
        op->is._1.entity = entity;
        world->info.clear_count ++;
        return true;
    }
    return false;
}

bool flecs_defer_on_delete_action(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action)
{
    if (flecs_defer_op(world, stage)) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpOnDeleteAction;
        op->id = id;
        op->is._1.entity = action;
        world->info.clear_count ++;
        return true;
    }
    return false;
}

bool flecs_defer_enable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    if (flecs_defer_op(world, stage)) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = enable ? EcsOpEnable : EcsOpDisable;
        op->is._1.entity = entity;
        op->id = id;
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
    if (flecs_defer_op(world, stage)) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));
        world->info.bulk_new_count ++;

        /* Use ecs_new_id as this is thread safe */
        int i;
        for (i = 0; i < count; i ++) {
            ids[i] = ecs_new_id(world);
        }

        *ids_out = ids;

        /* Store data in op */
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpBulkNew;
        op->id = id;
        op->is._n.entities = ids;
        op->is._n.count = count;

        return true;
    }
    return false;
}

bool flecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{   
    return flecs_defer_add_remove(world, stage, EcsOpNew, entity, id);
}

bool flecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{   
    return flecs_defer_add_remove(world, stage, EcsOpAdd, entity, id);
}

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return flecs_defer_add_remove(world, stage, EcsOpRemove, entity, id);
}

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_defer_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value,
    void **value_out)
{
    if (flecs_defer_op(world, stage)) {
        world->info.set_count ++;

        ecs_id_record_t *idr = flecs_id_record_ensure(world, id);
        ecs_check(idr != NULL && idr->type_info != NULL, 
            ECS_INVALID_PARAMETER, NULL);
        
        const ecs_type_info_t *ti = idr->type_info;
        ecs_assert(!size || size == ti->size, ECS_INVALID_PARAMETER, NULL);
        size = ti->size;

        ecs_stack_t *stack = &stage->defer_stack;
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->id = id;
        op->is._1.entity = entity;
        op->is._1.size = size;
        op->is._1.value = flecs_stack_alloc(stack, size, ti->alignment);

        if (!value) {
            value = ecs_get_id(world, entity, id);
        }

        if (value) {
            ecs_copy_t copy;
            if ((copy = ti->hooks.copy_ctor)) {
                copy(op->is._1.value, value, 1, ti);
            } else {
                ecs_os_memcpy(op->is._1.value, value, size);
            }
        } else {
            ecs_xtor_t ctor;
            if ((ctor = ti->hooks.ctor)) {
                ctor(op->is._1.value, 1, ti);
            }
        }

        if (value_out) {
            *value_out = op->is._1.value;
        }

        return true;
    }
error:
    return false;
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
    stage->asynchronous = false;
    
    flecs_stack_init(&stage->defer_stack);
}

void flecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    ecs_poly_assert(world, ecs_world_t);
    ecs_poly_assert(stage, ecs_stage_t);

    /* Make sure stage has no unmerged data */
    ecs_assert(ecs_vector_count(stage->defer_queue) == 0, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_poly_fini(stage, ecs_stage_t);

    ecs_vector_free(stage->defer_queue);
    flecs_stack_fini(&stage->defer_stack);
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
            flecs_stage_deinit(world, &stages[i]);
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
        ecs_defer_begin((ecs_world_t*)stage);
    }

    bool is_readonly = ECS_BIT_IS_SET(world->flags, EcsWorldReadonly);

    /* From this point on, the world is "locked" for mutations, and it is only 
     * allowed to enqueue commands from stages */
    ECS_BIT_SET(world->flags, EcsWorldReadonly);

    return is_readonly;
}

void ecs_readonly_end(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(world->flags & EcsWorldReadonly, ECS_INVALID_OPERATION, NULL);

    /* After this it is safe again to mutate the world directly */
    ECS_BIT_CLEAR(world->flags, EcsWorldReadonly);

    ecs_log_pop_3();

    do_auto_merge(world);
error:
    return;
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_poly_is(world, ecs_world_t) || 
               ecs_poly_is(world, ecs_stage_t), ECS_INVALID_PARAMETER, NULL);
    do_manual_merge(world);
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
        if (((ecs_stage_t*)stage)->asynchronous) {
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
    stage->asynchronous = true;

    ecs_defer_begin((ecs_world_t*)stage);

    return (ecs_world_t*)stage;
}

void ecs_async_stage_free(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_stage_t);
    ecs_stage_t *stage = (ecs_stage_t*)world;
    ecs_check(stage->asynchronous == true, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_deinit(stage->world, stage);
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

    return ((ecs_stage_t*)stage)->asynchronous;
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
