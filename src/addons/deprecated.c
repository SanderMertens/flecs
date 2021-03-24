#include "flecs.h"

#ifdef FLECS_DEPRECATED

#include "../private_api.h"

int32_t ecs_count_w_filter(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    return ecs_count_filter(world, filter);
}

int32_t ecs_count_entity(
    const ecs_world_t *world,
    ecs_id_t entity)
{
    return ecs_count_id(world, entity);
}

void ecs_set_component_actions_w_entity(
    ecs_world_t *world,
    ecs_id_t id,
    EcsComponentLifecycle *actions)
{
    ecs_set_component_actions_w_id(world, id, actions);
}

ecs_entity_t ecs_new_w_entity(
    ecs_world_t *world,
    ecs_id_t id)
{
    return ecs_new_w_id(world, id);
}

const ecs_entity_t* ecs_bulk_new_w_entity(
    ecs_world_t *world,
    ecs_id_t id,
    int32_t count)
{
    return ecs_bulk_new_w_id(world, id, count);
}

void ecs_enable_component_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    ecs_enable_component_w_id(world, entity, id, enable);
}

bool ecs_is_component_enabled_w_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_is_component_enabled_w_id(world, entity, id);
}

const void* ecs_get_w_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_get_w_id(world, entity, id);
}

const void* ecs_get_ref_w_entity(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_get_ref_w_id(world, ref, entity, id);
}

void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_added)
{
    return ecs_get_mut_w_id(world, entity, id, is_added);
}

void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_modified_w_id(world, entity, id);
}

ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr)
{
    return ecs_set_ptr_w_id(world, entity, id, size, ptr);
}

bool ecs_has_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_has_id(world, entity, id);
}

size_t ecs_entity_str(
    const ecs_world_t *world,
    ecs_id_t entity,
    char *buffer,
    size_t buffer_len)
{
    return ecs_id_str(world, entity, buffer, buffer_len);
}

ecs_entity_t ecs_get_parent_w_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_get_object_w_id(world, entity, EcsChildOf, id);
}

int32_t ecs_get_thread_index(
    const ecs_world_t *world)
{
    return ecs_get_stage_id(world);
}

void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_add_id(world, entity, id);
}

void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_remove_id(world, entity, id);
}

void ecs_dim_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t entity_count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    if (type) {
        ecs_table_t *table = ecs_table_from_type(world, type);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = ecs_table_get_or_create_data(table);
        ecs_table_set_size(world, table, data, entity_count);
    }
}

ecs_type_t ecs_type_from_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    return ecs_type_from_id(world, entity);
}

ecs_entity_t ecs_type_to_entity(
    const ecs_world_t *world,
    ecs_type_t type)
{
    return ecs_type_to_id(world, type);
}

bool ecs_type_has_entity(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    return ecs_type_has_id(world, type, entity);
}

bool ecs_type_owns_entity(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    return ecs_type_owns_id(world, type, entity, owned);
}

#endif
