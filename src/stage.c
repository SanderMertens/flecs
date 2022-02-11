#include "private_api.h"

static
ecs_defer_op_t* new_defer_op(ecs_stage_t *stage) {
    ecs_defer_op_t *result = ecs_vector_add(&stage->defer_queue, ecs_defer_op_t);
    ecs_os_memset(result, 0, ECS_SIZEOF(ecs_defer_op_t));
    return result;
}

static
bool defer_add_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_defer_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_id_t id)
{
    if (stage->defer) {
        if (!id) {
            return true;
        }

        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->id = id;
        op->is._1.entity = entity;

        if (op_kind == EcsOpNew) {
            world->new_count ++;
        } else if (op_kind == EcsOpAdd) {
            world->add_count ++;
        } else if (op_kind == EcsOpRemove) {
            world->remove_count ++;
        }

        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

static
void merge_stages(
    ecs_world_t *world,
    bool force_merge)
{
    bool is_stage = ecs_poly_is(world, ecs_stage_t);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    bool measure_frame_time = world->measure_frame_time;

    ecs_time_t t_start;
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    if (is_stage) {
        /* Check for consistency if force_merge is enabled. In practice this
         * function will never get called with force_merge disabled for just
         * a single stage. */
        if (force_merge || stage->auto_merge) {
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
                ecs_defer_end((ecs_world_t*)s);
            }
        }
    }

    flecs_eval_component_monitors(world);

    if (measure_frame_time) {
        world->stats.merge_time_total += (float)ecs_time_measure(&t_start);
    }

    world->stats.merge_count_total ++; 

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

bool flecs_defer_none(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    return (++ stage->defer) == 1;
}

bool flecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    (void)world;
    if (stage->defer) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpModified;
        op->id = id;
        op->is._1.entity = entity;
        return true;
    } else {
        stage->defer ++;
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
    (void)world;
    if (stage->defer) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClone;
        op->id = src;
        op->is._1.entity = entity;
        op->is._1.clone_value = clone_value;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;   
}

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    (void)world;
    if (stage->defer) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpDelete;
        op->is._1.entity = entity;
        world->delete_count ++;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    (void)world;
    if (stage->defer) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClear;
        op->is._1.entity = entity;
        world->clear_count ++;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool flecs_defer_on_delete_action(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_id_t id,
    ecs_entity_t action)
{
    (void)world;
    if (stage->defer) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = EcsOpOnDeleteAction;
        op->id = id;
        op->is._1.entity = action;
        world->clear_count ++;
        return true;
    } else {
        stage->defer ++;
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
    (void)world;
    if (stage->defer) {
        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = enable ? EcsOpEnable : EcsOpDisable;
        op->is._1.entity = entity;
        op->id = id;
        return true;
    } else {
        stage->defer ++;
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
    if (stage->defer) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));
        world->bulk_new_count ++;

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
    } else {
        stage->defer ++;
    }

    return false;
}

bool flecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{   
    return defer_add_remove(world, stage, EcsOpNew, entity, id);
}

bool flecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{   
    return defer_add_remove(world, stage, EcsOpAdd, entity, id);
}

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return defer_add_remove(world, stage, EcsOpRemove, entity, id);
}

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_defer_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_id_t id,
    ecs_size_t size,
    const void *value,
    void **value_out,
    bool *is_added)
{
    if (stage->defer) {
        world->set_count ++;
        if (!size) {
            const EcsComponent *cptr = flecs_component_from_id(world, id);
            ecs_check(cptr != NULL, ECS_INVALID_PARAMETER, NULL);
            size = cptr->size;
        }

        ecs_defer_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->id = id;
        op->is._1.entity = entity;
        op->is._1.size = size;
        op->is._1.value = ecs_os_malloc(size);

        if (!value) {
            value = ecs_get_id(world, entity, id);
            if (is_added) {
                *is_added = value == NULL;
            }
        }

        const ecs_type_info_t *c_info = NULL;
        ecs_entity_t real_id = ecs_get_typeid(world, id);
        if (real_id) {
            c_info = flecs_get_c_info(world, real_id);
        }

        if (value) {
            ecs_copy_ctor_t copy;
            if (c_info && (copy = c_info->lifecycle.copy_ctor)) {
                copy(world, id, &c_info->lifecycle, &entity, &entity, 
                    op->is._1.value, value, flecs_itosize(size), 1, 
                        c_info->lifecycle.ctx);
            } else {
                ecs_os_memcpy(op->is._1.value, value, size);
            }
        } else {
            ecs_xtor_t ctor;
            if (c_info && (ctor = c_info->lifecycle.ctor)) {
                ctor(world, id, &entity, op->is._1.value, 
                    flecs_itosize(size), 1, c_info->lifecycle.ctx);
            }
        }

        if (value_out) {
            *value_out = op->is._1.value;
        }

        return true;
    } else {
        stage->defer ++;
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
}

void ecs_set_stages(
    ecs_world_t *world,
    int32_t stage_count)
{
    ecs_poly_assert(world, ecs_world_t);

    ecs_stage_t *stages;
    int32_t i, count = ecs_vector_count(world->worker_stages);

    if (count && count != stage_count) {
        stages = ecs_vector_first(world->worker_stages, ecs_stage_t);

        for (i = 0; i < count; i ++) {
            /* If stage contains a thread handle, ecs_set_threads was used to
             * create the stages. ecs_set_threads and ecs_set_stages should not
             * be mixed. */
            ecs_poly_assert(&stages[i], ecs_stage_t);
            ecs_check(stages[i].thread == 0, ECS_INVALID_OPERATION, NULL);
            flecs_stage_deinit(world, &stages[i]);
        }

        ecs_vector_free(world->worker_stages);
    }
    
    if (stage_count) {
        world->worker_stages = ecs_vector_new(ecs_stage_t, stage_count);

        for (i = 0; i < stage_count; i ++) {
            ecs_stage_t *stage = ecs_vector_add(
                &world->worker_stages, ecs_stage_t);
            flecs_stage_init(world, stage);
            stage->id = 1 + i; /* 0 is reserved for main/temp stage */

            /* Set thread_ctx to stage, as this stage might be used in a
             * multithreaded context */
            stage->thread_ctx = (ecs_world_t*)stage;
        }
    } else {
        /* Set to NULL to prevent double frees */
        world->worker_stages = NULL;
    }

    /* Regardless of whether the stage was just initialized or not, when the
     * ecs_set_stages function is called, all stages inherit the auto_merge
     * property from the world */
    for (i = 0; i < stage_count; i ++) {
        ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
        stage->auto_merge = world->stage.auto_merge;
    }
error:
    return;
}

int32_t ecs_get_stage_count(
    const ecs_world_t *world)
{
    world = ecs_get_world(world);
    return ecs_vector_count(world->worker_stages);
}

int32_t ecs_get_stage_id(
    const ecs_world_t *world)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (ecs_poly_is(world, ecs_stage_t)) {
        ecs_stage_t *stage = (ecs_stage_t*)world;

        /* Index 0 is reserved for main stage */
        return stage->id - 1;
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
    ecs_check(ecs_vector_count(world->worker_stages) > stage_id, 
        ECS_INVALID_PARAMETER, NULL);

    return (ecs_world_t*)ecs_vector_get(
        world->worker_stages, ecs_stage_t, stage_id);
error:
    return NULL;
}

bool ecs_staging_begin(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);

    flecs_process_pending_tables(world);

    int32_t i, count = ecs_get_stage_count(world);
    for (i = 0; i < count; i ++) {
        ecs_world_t *stage = ecs_get_stage(world, i);
        ((ecs_stage_t*)stage)->lookup_path = world->stage.lookup_path;
        ecs_defer_begin(stage);
    }

    bool is_readonly = world->is_readonly;

    /* From this point on, the world is "locked" for mutations, and it is only 
     * allowed to enqueue commands from stages */
    world->is_readonly = true;

    ecs_dbg_3("staging: begin");

    return is_readonly;
}

void ecs_staging_end(
    ecs_world_t *world)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_check(world->is_readonly == true, ECS_INVALID_OPERATION, NULL);

    /* After this it is safe again to mutate the world directly */
    world->is_readonly = false;

    ecs_dbg_3("staging: end");

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
        world->stage.auto_merge = auto_merge;

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

    if (world->is_readonly) {
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
