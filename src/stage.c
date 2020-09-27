#include "private_api.h"

static
ecs_op_t* new_defer_op(ecs_stage_t *stage) {
    ecs_op_t *result = ecs_vector_add(&stage->defer_queue, ecs_op_t);
    ecs_os_memset(result, 0, ECS_SIZEOF(ecs_op_t));
    return result;
}

static 
void new_defer_component_ids(
    ecs_op_t *op, 
    ecs_entities_t *components)
{
    int32_t components_count = components->count;
    if (components_count == 1) {
        ecs_entity_t component = components->array[0];
        op->component = component;
        op->components = (ecs_entities_t) {
            .array = NULL,
            .count = 1
        };
    } else if (components_count) {
        ecs_size_t array_size = components_count * ECS_SIZEOF(ecs_entity_t);
        op->components.array = ecs_os_malloc(array_size);
        ecs_os_memcpy(op->components.array, components->array, array_size);
        op->components.count = components_count;
    } else {
        op->component = 0;
        op->components = (ecs_entities_t){ 0 };
    }
}

static
bool defer_add_remove(
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entities_t *components)
{
    if (stage->defer) {
        ecs_entity_t scope = stage->scope;
        if (components) {
            if (!components->count && !scope) {
                return true;
            }
        }

        ecs_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->entity = entity;
        op->scope = scope;

        new_defer_component_ids(op, components);

        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

bool ecs_defer_none(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    stage->defer ++;
    return false;
}

bool ecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpModified;
        op->entity = entity;
        op->component = component;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

bool ecs_defer_clone(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value)
{   
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClone;
        op->entity = entity;
        op->component = src;
        op->clone_value = clone_value;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;   
}

bool ecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpDelete;
        op->entity = entity;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool ecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClear;
        op->entity = entity;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool ecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    ecs_entities_t *components_ids,
    void **component_data,
    const ecs_entity_t **ids_out)
{
    if (stage->defer) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));
        void **defer_data = NULL;

        /* Use ecs_new_id as this is thread safe */
        int i;
        for (i = 0; i < count; i ++) {
            ids[i] = ecs_new_id(world);
        }

        /* Create private copy for component data */
        if (component_data) {
            int c, c_count = components_ids->count;
            ecs_entity_t *components = components_ids->array;
            defer_data = ecs_os_malloc(ECS_SIZEOF(void*) * c_count);
            for (c = 0; c < c_count; c ++) {
                ecs_entity_t component = components[c];
                const EcsComponent *cptr = ecs_get(world, component, EcsComponent);
                ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

                ecs_size_t size = cptr->size;
                void *data = ecs_os_malloc(size * count);
                defer_data[c] = data;

                ecs_c_info_t *cinfo = NULL;
                ecs_entity_t real_id = ecs_component_id_from_id(world, component);
                if (real_id) {
                    cinfo = ecs_get_c_info(world, real_id);
                }
                ecs_xtor_t ctor;
                if (cinfo && (ctor = cinfo->lifecycle.ctor)) {
                    void *ctx = cinfo->lifecycle.ctx;
                    ctor(world, component, ids, data, ecs_to_size_t(size), count, ctx);
                    ecs_move_t move;
                    if ((move = cinfo->lifecycle.move)) {
                        move(world, component, ids, ids, data, component_data[c], 
                            ecs_to_size_t(size), count, ctx);
                    } else {
                        ecs_os_memcpy(data, component_data[c], size * count);
                    }
                } else {
                    ecs_os_memcpy(data, component_data[c], size * count);
                }
            }
        }

        /* Store data in op */
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpBulkNew;
        op->entities = ids;
        op->bulk_data = defer_data;
        op->count = count;
        new_defer_component_ids(op, components_ids);
        *ids_out = ids;

        return true;
    } else {
        stage->defer ++;
    }

    return false;
}

bool ecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entities_t *components)
{   
    (void)world;
    return defer_add_remove(stage, EcsOpNew, entity, components);
}

bool ecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entities_t *components)
{   
    (void)world;
    return defer_add_remove(stage, EcsOpAdd, entity, components);
}

bool ecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entities_t *components)
{
    (void)world;
    return defer_add_remove(stage, EcsOpRemove, entity, components);
}

bool ecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_size_t size,
    const void *value,
    void **value_out,
    bool *is_added)
{
    if (stage->defer) {
        if (!size) {
            const EcsComponent *cptr = ecs_get(world, component, EcsComponent);
            ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);
            size = cptr->size;
        }

        ecs_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->entity = entity;
        op->component = component;
        op->size = size;
        op->value = ecs_os_malloc(size);

        if (!value) {
            value = ecs_get_w_entity(world, entity, component);
            if (is_added) {
                *is_added = value == NULL;
            }
        }

        ecs_c_info_t *c_info = NULL;
        ecs_entity_t real_id = ecs_component_id_from_id(world, component);
        if (real_id) {
            c_info = ecs_get_c_info(world, real_id);
        }
        ecs_xtor_t ctor;
        if (c_info && (ctor = c_info->lifecycle.ctor)) {
            ctor(world, component, &entity, op->value, ecs_to_size_t(size), 1, 
                c_info->lifecycle.ctx);

            ecs_copy_t copy;
            if (value) {
                if ((copy = c_info->lifecycle.copy)) {
                    copy(world, component, &entity, &entity, op->value, value, 
                        ecs_to_size_t(size), 1, c_info->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(op->value, value, size);
                }
            }
        } else if (value) {
            ecs_os_memcpy(op->value, value, size);
        }
        
        if (value_out) {
            *value_out = op->value;
        }

        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(stage->defer == 0, ECS_INVALID_PARAMETER, NULL);
    if (ecs_vector_count(stage->defer_merge_queue)) {
        stage->defer ++;
        stage->defer_queue = stage->defer_merge_queue;
        ecs_defer_flush(world, stage);
        ecs_vector_clear(stage->defer_merge_queue);
        ecs_assert(stage->defer_queue == NULL, ECS_INVALID_PARAMETER, NULL);
    }    
}

void ecs_stage_defer_begin(
    ecs_world_t *world,
    ecs_stage_t *stage)
{   
    (void)world; 
    ecs_defer_none(world, stage);
    if (stage->defer == 1) {
        stage->defer_queue = stage->defer_merge_queue;
    }
}

void ecs_stage_defer_end(
    ecs_world_t *world,
    ecs_stage_t *stage)
{ 
    (void)world;
    stage->defer --;
    if (!stage->defer) {
        stage->defer_merge_queue = stage->defer_queue;
        stage->defer_queue = NULL;
    }
}

void ecs_stage_merge_post_frame(
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

void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    memset(stage, 0, sizeof(ecs_stage_t));
    if (stage == &world->stage) {
        stage->id = 0;
    } else if (stage == &world->temp_stage) {
        stage->id = 1;
    }
}

void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    ecs_vector_free(stage->defer_queue);
    ecs_vector_free(stage->defer_merge_queue);
}

