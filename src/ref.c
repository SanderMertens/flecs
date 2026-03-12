/**
 * @file ref.c
 * @brief Ref API: cached component pointers for faster repeated access.
 */

#include "private_api.h"

/* Create a ref that caches an entity's component pointer and table version. */
ecs_ref_t ecs_ref_init_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_check(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    
    world = ecs_get_world(world);

    flecs_check_exclusive_world_access_read(world);

    ecs_record_t *record = flecs_entities_get(world, entity);
    ecs_check(record != NULL, ECS_INVALID_PARAMETER,
        "cannot create ref for empty entity");

    ecs_ref_t result = {
        .entity = entity,
        .id = id,
        .record = record
    };

    ecs_table_t *table = record->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    result.table_id = table->id;
    result.table_version_fast = flecs_get_table_version_fast(world, result.table_id);
    result.table_version = table->version;
    result.ptr = flecs_get_component(
        world, table, ECS_RECORD_TO_ROW(record->row), 
        flecs_components_get(world, id));

    return result;
error:
    return (ecs_ref_t){0};
}

/* Refresh the cached pointer if the entity's table has changed. Uses a
 * two-level version check: fast version (global) then per-table version. */
void ecs_ref_update(
    const ecs_world_t *world,
    ecs_ref_t *ref)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->id != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->record != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->record == flecs_entities_get_any(world, ref->entity), 
        ECS_INVALID_OPERATION, "a corrupt ref was passed to ecs_ref_update");

    flecs_check_exclusive_world_access_read(world);

    if (ref->table_version_fast == flecs_get_table_version_fast(
        world, ref->table_id)) 
    {
        return;
    }

    ecs_record_t *r = ref->record;
    ecs_table_t *table = r->table;
    if (!table) { /* Entity was deleted or has no components */
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
        flecs_components_get(world, ref->id));

error:
    return;
}

/* Get the cached component pointer, refreshing if stale. */
void* ecs_ref_get_id(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_id_t id)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ref->entity != 0, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(ref->id != 0, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(ref->record != NULL, ECS_INVALID_PARAMETER, "ref not initialized");
    ecs_check(id == ref->id, ECS_INVALID_PARAMETER, "id does not match ref");

    (void)id;

    ecs_ref_update(world, ref);

    return ref->ptr;
error:
    return NULL;
}
