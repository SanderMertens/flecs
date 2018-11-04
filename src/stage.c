
#include "include/private/reflecs.h"

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
        uint64_t key;
        uint64_t row64 = ecs_map_next(&it, &key);
        ecs_commit(world, key);

        if (row64) {
            EcsRow staged_row = ecs_to_row(row64);
            EcsRow row = ecs_to_row(ecs_map_get64(world->entity_index, key));
            EcsTable *table = ecs_world_get_table(world, row.family_id);
            EcsTable *staged_table = ecs_world_get_table(
                world, staged_row.family_id);
            EcsArray *staged_rows = ecs_map_get(
                stage->data_stage, staged_row.family_id);

            void *row_ptr = ecs_table_get(table, row.index);
            void *staged_row_ptr = ecs_array_get(
                staged_rows, &staged_table->row_params, staged_row.index);

            ecs_copy_row(
                world,
                table->family,
                table->columns,
                row_ptr,
                staged_table->family,
                staged_table->columns,
                staged_row_ptr);
        }
    }

    ecs_map_clear(stage->entity_stage);
}

static
void clean_data_stage(
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->data_stage);
    while (ecs_iter_hasnext(&it)) {
        EcsArray *family = ecs_iter_next(&it);
        ecs_array_free(family);
    }

    ecs_map_free(stage->data_stage);
}

/* -- Private functions -- */

void ecs_stage_init(
    EcsStage *stage)
{
    stage->add_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->remove_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->entity_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->data_stage = ecs_map_new(ECS_WORLD_INITIAL_STAGING_COUNT);
    stage->delete_stage = ecs_array_new(&handle_arr_params, 0);
}

void ecs_stage_deinit(
    EcsStage *stage)
{
    ecs_map_free(stage->add_stage);
    ecs_map_free(stage->remove_stage);
    ecs_map_free(stage->entity_stage);
    ecs_array_free(stage->delete_stage);
    clean_data_stage(stage);
}

void ecs_stage_merge(
    EcsWorld *world,
    EcsStage *stage)
{
    uint32_t to_delete_count = ecs_array_count(stage->delete_stage);
    if (to_delete_count) {
        process_to_delete(world, stage);
    }

    uint32_t to_commit_count = ecs_map_count(stage->entity_stage);
    if (to_commit_count) {
        process_to_commit(world, stage);
    }
}
