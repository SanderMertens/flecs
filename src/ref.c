/**
 * @file ref.c
 * @brief Ref API.
 *
 * Refs provide faster access to components than get.
 */

#include "private_api.h"

static
ecs_record_t* flecs_ref_init(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_id_t id)
{
    (void)id;
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    flecs_check_exclusive_world_access_read(world);

    ecs_record_t *record = flecs_entities_get(world, entity);
    ecs_check(record != NULL, ECS_INVALID_PARAMETER,
        "cannot create ref for empty entity");

    ecs_table_t *table = record->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    *ref = (ecs_ref_t){
        .entity = entity,
        .table_id = table->id,
        .table_version_fast = flecs_get_table_version_fast(world, table->id),
        .table_version = table->version,
#ifdef FLECS_DEBUG
        .id = id,
#endif
    };

    return record;
error:
    return NULL;
}

ecs_ref_t ecs_ref_init_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    world = ecs_get_world(world);

    ecs_ref_t result;
    ecs_record_t *record = flecs_ref_init(world, &result, entity, id);
    if (!record) {
        return (ecs_ref_t){0};
    }

    result.ptr = flecs_get_component(
        world, record->table, ECS_RECORD_TO_ROW(record->row),
        flecs_components_get(world, id));

    return result;
}

ecs_ref_t ecs_ref_from_ptr(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    void *ptr)
{
    world = ecs_get_world(world);

    ecs_ref_t result;
    ecs_record_t *record = flecs_ref_init(world, &result, entity, id);
    if (!record) {
        return (ecs_ref_t){0};
    }

    ecs_assert(ptr == flecs_get_component(
        world, record->table, ECS_RECORD_TO_ROW(record->row),
        flecs_components_get(world, id)), ECS_INVALID_PARAMETER,
            "pointer does not match component of entity");

    result.ptr = ptr;

    return result;
}

void ecs_ref_update(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(id != 0, ECS_INVALID_PARAMETER, NULL);
#ifdef FLECS_DEBUG
    ecs_check(id == ref->id, ECS_INVALID_PARAMETER, "id does not match ref");
#endif

    flecs_check_exclusive_world_access_read(world);

    if (ref->table_version_fast == flecs_get_table_version_fast(
        world, ref->table_id))
    {
        return;
    }

    ecs_record_t *r = flecs_entities_get_any(world, ref->entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = r->table;
    if (!table) { /* Table can be NULL, entity could have been deleted */
        ref->table_id = 0;
        ref->table_version_fast = 0;
        ref->table_version = 0;
        ref->ptr = NULL;
        return;
    }

    if (!ecs_is_alive(world, ref->entity)) {
        ref->table_id = 0;
        ref->table_version_fast = 0;
        ref->table_version = 0;
        ref->ptr = NULL;
        return;
    }

    if (ref->table_id == table->id && ref->table_version == table->version) {
        ref->table_version_fast = flecs_get_table_version_fast(world, ref->table_id);
        return;
    }

    ref->table_id = table->id;
    ref->table_version_fast = flecs_get_table_version_fast(world, ref->table_id);
    ref->table_version = table->version;
    ref->ptr = flecs_get_component(world, table, ECS_RECORD_TO_ROW(r->row),
        flecs_components_get(world, id));

error:
    return;
}

void* ecs_ref_get_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(id != 0, ECS_INVALID_PARAMETER, "ref not initialized");
#ifdef FLECS_DEBUG
    ecs_check(id == ref->id, ECS_INVALID_PARAMETER, "id does not match ref");
#endif

    ecs_ref_update(world, ref, id);

    return ref->ptr;
error:
    return NULL;
}
