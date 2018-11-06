
#include "include/private/reflecs.h"

static
void process_families(
    EcsWorld *world,
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->family_stage);
    while (ecs_iter_hasnext(&it)) {
        uint64_t family_id;
        EcsArray *family = (void*)(uintptr_t)ecs_map_next(&it, &family_id);

        if (!ecs_map_has(world->family_index, family_id, NULL)) {
            ecs_map_set(world->family_index, family_id, family);
        }
    }

    ecs_map_clear(stage->family_stage);
}

static
void process_to_delete(
    EcsWorld *world,
    EcsStage *stage)
{
    EcsHandle *buffer = ecs_array_buffer(stage->delete_stage);
    uint32_t i, count = ecs_array_count(stage->delete_stage);
    for (i = 0; i < count; i ++) {
        ecs_delete(world, buffer[i]);
    }
    ecs_array_clear(stage->delete_stage);
}

static
void process_to_commit(
    EcsWorld *world,
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->entity_stage);
    while (ecs_iter_hasnext(&it)) {
        EcsHandle entity;
        uint64_t row64 = ecs_map_next(&it, &entity);
        EcsRow staged_row = ecs_to_row(row64);
        ecs_merge_entity(world, stage, entity, &staged_row);
    }

    it = ecs_map_iter(stage->data_stage);
    while (ecs_iter_hasnext(&it)) {
        EcsArray *stage = ecs_iter_next(&it);
        ecs_array_free(stage);
    }

    ecs_map_clear(stage->entity_stage);
    ecs_map_clear(stage->add_stage);
    ecs_map_clear(stage->remove_stage);
    ecs_map_clear(stage->remove_merge);
    ecs_map_clear(stage->data_stage);
}

/** Stage components for adding or removing from an entity */
static
EcsResult stage_components(
    EcsWorld *world,
    EcsStage *stage,
    EcsHandle entity,
    EcsHandle component,
    EcsMap *stage_index)
{
    EcsFamily family_id;

    family_id = ecs_map_get64(stage_index, entity);
    EcsFamily resolved_family = ecs_family_from_handle(world, component);
    assert(resolved_family != 0);

    EcsFamily new_family_id;
    if (family_id) {
        new_family_id = ecs_family_merge(
            world, stage, family_id, resolved_family, 0);
    } else {
        new_family_id = resolved_family;
    }

    assert(new_family_id != 0);

    if (family_id != new_family_id) {
        ecs_map_set64(stage_index, entity, new_family_id);
    }

    return EcsOk;
}

/* -- Private functions -- */

void ecs_stage_init(
    EcsStage *stage)
{
    stage->add_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->remove_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->remove_merge = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->entity_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->delete_stage = ecs_array_new(&handle_arr_params, 0);
    stage->data_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->family_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
}

void ecs_stage_deinit(
    EcsStage *stage)
{
    ecs_map_free(stage->add_stage);
    ecs_map_free(stage->remove_stage);
    ecs_map_free(stage->remove_merge);
    ecs_map_free(stage->entity_stage);
    ecs_array_free(stage->delete_stage);
    ecs_map_free(stage->data_stage);
    ecs_map_free(stage->family_stage);
}

void ecs_stage_merge(
    EcsWorld *world,
    EcsStage *stage)
{
    process_families(world, stage);
    process_to_delete(world, stage);
    process_to_commit(world, stage);
}

/* -- Public API -- */

EcsResult ecs_add(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    EcsStage *stage = ecs_get_stage(&world);
    return stage_components(world, stage, entity, component, stage->add_stage);
}

EcsResult ecs_remove(
    EcsWorld *world,
    EcsHandle entity,
    EcsHandle component)
{
    EcsStage *stage = ecs_get_stage(&world);
    return stage_components(
        world, stage, entity, component, stage->remove_stage);
}
