
#include "include/private/reflecs.h"

static
void process_families(
    EcsWorld *world,
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->family_stage);
    while (ecs_iter_hasnext(&it)) {
        uint64_t type_id;
        EcsArray *family = (void*)(uintptr_t)ecs_map_next(&it, &type_id);

        if (!ecs_map_has(world->family_index, type_id, NULL)) {
            ecs_map_set(world->family_index, type_id, family);
        }
    }

    ecs_map_clear(stage->family_stage);
}

static
void process_tables(
    EcsWorld *world,
    EcsStage *stage)
{
    EcsTable *buffer = ecs_array_buffer(stage->table_db_stage);
    uint32_t i, count = ecs_array_count(stage->table_db_stage);
    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        EcsType type_id = table->type_id;
        if (!ecs_map_has(world->table_index, type_id, NULL)) {
            EcsTable *dst = ecs_array_add(&world->table_db, &table_arr_params);

            *dst = *table;

            uint32_t index = ecs_array_count(world->table_db) - 1;
            ecs_map_set(world->table_index, type_id, index + 1);

            /* Table might still refer to family in stage */
            table = ecs_array_get(world->table_db, &table_arr_params, index);
            table->family = ecs_type_get(world, NULL, type_id);
        } else {
            ecs_table_deinit(world, table);
        }
    }

    ecs_array_clear(stage->table_db_stage);
    ecs_map_clear(stage->table_stage);
}

static
void process_to_delete(
    EcsWorld *world,
    EcsStage *stage)
{
    EcsEntity *buffer = ecs_array_buffer(stage->delete_stage);
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
        EcsEntity entity;
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
    EcsEntity entity,
    EcsEntity component,
    EcsMap *stage_index)
{
    EcsType type_id;

    type_id = ecs_map_get64(stage_index, entity);
    EcsType resolved_family = ecs_type_from_handle(
        world, stage, component, NULL);
    
    ecs_assert(resolved_family != 0, ECS_NOT_A_COMPONENT, NULL);

    EcsType new_type_id;
    if (type_id) {
        new_type_id = ecs_type_merge(
            world, stage, type_id, resolved_family, 0);
    } else {
        new_type_id = resolved_family;
    }

    assert(new_type_id != 0);

    if (type_id != new_type_id) {
        ecs_map_set64(stage_index, entity, new_type_id);
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
    stage->table_db_stage = ecs_array_new(&table_arr_params, 0);
    stage->table_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
}

    EcsMap *add_stage;            /* Entities with components to add */
    EcsMap *remove_stage;         /* Entities with components to remove */
    EcsMap *remove_merge;         /* All removed components before merge */
    EcsArray *delete_stage;       /* Deleted entities while in progress */
    EcsMap *entity_stage;         /* Entities committed while in progress */
    EcsMap *data_stage;           /* Arrays with staged component values */
    EcsMap *family_stage;         /* Families created while >1 threads running*/
    EcsArray *table_db_stage;     /* Tables created while >1 threads running */
    EcsMap *table_stage;          /* Index for table stage */

void ecs_stage_deinit(
    EcsStage *stage)
{
    ecs_map_free(stage->add_stage);
    ecs_map_free(stage->remove_stage);
    ecs_map_free(stage->remove_merge);
    ecs_array_free(stage->delete_stage);
    ecs_map_free(stage->entity_stage);
    ecs_map_free(stage->data_stage);
    ecs_map_free(stage->family_stage);
    ecs_array_free(stage->table_db_stage);
    ecs_map_free(stage->table_stage);
}

void ecs_stage_merge(
    EcsWorld *world,
    EcsStage *stage)
{
    process_families(world, stage);
    process_tables(world, stage);
    process_to_delete(world, stage);
    process_to_commit(world, stage);
}

/* -- Public API -- */

EcsResult ecs_stage_add(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity component)
{
    EcsStage *stage = ecs_get_stage(&world);
    return stage_components(world, stage, entity, component, stage->add_stage);
}

EcsResult ecs_stage_remove(
    EcsWorld *world,
    EcsEntity entity,
    EcsEntity component)
{
    EcsStage *stage = ecs_get_stage(&world);
    return stage_components(
        world, stage, entity, component, stage->remove_stage);
}

