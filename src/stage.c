
#include "include/private/flecs.h"
#include <string.h>

static
void merge_families(
    ecs_world_t *world,
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->type_index);
    while (ecs_iter_hasnext(&it)) {
        uint64_t type_id;
        EcsArray *type = (void*)(uintptr_t)ecs_map_next(&it, &type_id);

        if (!ecs_map_has(world->main_stage.type_index, type_id, NULL)) {
            ecs_map_set(world->main_stage.type_index, type_id, type);
        }
    }

    ecs_map_clear(stage->type_index);
}

static
void merge_tables(
    ecs_world_t *world,
    EcsStage *stage)
{
    EcsTable *buffer = ecs_array_buffer(stage->tables);
    uint32_t i, count = ecs_array_count(stage->tables);

    EcsStage *main_stage = &world->main_stage;

    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        EcsType type_id = table->type_id;

        /* Ensure table exists in main stage */
        EcsTable *main_table = ecs_world_get_table(
            world, main_stage, type_id);

        ecs_assert(main_table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_table_deinit(world, table);
    }

    ecs_array_clear(stage->tables);
    ecs_map_clear(stage->table_index);
}

static
void merge_commits(
    ecs_world_t *world,
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->entity_index);

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

    ecs_map_clear(stage->entity_index);
    ecs_map_clear(stage->remove_merge);
    ecs_map_clear(stage->data_stage);
}

static
void clean_families(
    EcsStage *stage)
{
    EcsIter it = ecs_map_iter(stage->type_index);
    while (ecs_iter_hasnext(&it)) {
        EcsArray *type = ecs_iter_next(&it);
        ecs_array_free(type);
    }

    ecs_map_free(stage->type_index);
}

static
void clean_tables(
    ecs_world_t *world,
    EcsStage *stage)
{
    EcsTable *buffer = ecs_array_buffer(stage->tables);
    int32_t i, count = ecs_array_count(stage->tables);

    for (i = count - 1; i >= 0; i --) {
        EcsTable *table = &buffer[i];
        ecs_table_deinit(world, table);
    }

    for (i = 0; i < count; i ++) {
        EcsTable *table = &buffer[i];
        ecs_table_free(world, table);
    }

    ecs_array_free(stage->tables);
}

/* -- Private functions -- */

void ecs_stage_init(
    ecs_world_t *world,
    EcsStage *stage)
{
    bool is_main_stage = stage == &world->main_stage;
    bool is_temp_stage = stage == &world->temp_stage;

    memset(stage, 0, sizeof(EcsStage));

    stage->entity_index = ecs_map_new(0);

    if (is_temp_stage) {
        stage->type_index = world->main_stage.type_index;
    } else {
        stage->type_index = ecs_map_new(0);
    }

    stage->table_index = ecs_map_new(0);
    if (is_main_stage) {
        stage->tables = ecs_array_new(&table_arr_params, 8);
    } else {
        stage->tables = ecs_array_new(&table_arr_params, 0);
    }

    if (!is_main_stage) {
        stage->data_stage = ecs_map_new(0);
        stage->remove_merge = ecs_map_new(0);
    }
}

void ecs_stage_deinit(
    ecs_world_t *world,
    EcsStage *stage)
{
    bool is_main_stage = stage == &world->main_stage;
    bool is_temp_stage = stage == &world->temp_stage;

    ecs_map_free(stage->entity_index);

    clean_tables(world, stage);
    ecs_map_free(stage->table_index);

    if (!is_temp_stage) {
        clean_families(stage);
    }

    if (!is_main_stage) {
        ecs_map_free(stage->data_stage);
        ecs_map_free(stage->remove_merge);
    }
}

void ecs_stage_merge(
    ecs_world_t *world,
    EcsStage *stage)
{
    assert(stage != &world->main_stage);
    
    bool is_temp_stage = stage == &world->temp_stage;

    if (!is_temp_stage) {
        merge_families(world, stage);
    }
    
    merge_commits(world, stage);

    merge_tables(world, stage);
}
