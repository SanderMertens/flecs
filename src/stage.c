#include "flecs_private.h"

static
void merge_families(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_map_iter_t it = ecs_map_iter(stage->type_index);
    while (ecs_map_hasnext(&it)) {
        uint64_t type_id;
        ecs_vector_t *type = (void*)(uintptr_t)ecs_map_next_w_key(&it, &type_id);

        if (!ecs_map_has(world->main_stage.type_index, type_id, NULL)) {
            ecs_map_set(world->main_stage.type_index, type_id, type);
        } else {
            ecs_vector_free(type);
        }
    }

    ecs_map_clear(stage->type_index);
}

static
void eval_table_columns(
    ecs_world_t *world, 
    uint32_t old_table_count, 
    uint32_t new_table_count) 
{
    ecs_table_t *tables = ecs_vector_first(world->main_stage.tables);

    uint32_t i;
    for (i = old_table_count; i < new_table_count; i ++) {
        ecs_table_eval_columns(world, &tables[i]);
        ecs_notify_systems_of_table(world, &tables[i]);
    }
}

static
void merge_commits(
    ecs_world_t *world,
    ecs_stage_t *stage)
{  
    if (!ecs_map_count(stage->entity_index)) {
        return;
    }

    ecs_map_iter_t it = ecs_map_iter(stage->entity_index);

    while (ecs_map_hasnext(&it)) {
        ecs_entity_t entity;
        uint64_t row64 = ecs_map_next_w_key(&it, &entity);
        ecs_row_t staged_row = ecs_to_row(row64);
        ecs_merge_entity(world, stage, entity, &staged_row);
    }

    it = ecs_map_iter(stage->data_stage);
    while (ecs_map_hasnext(&it)) {
        ecs_vector_t *stage = ecs_map_next_ptr(&it);
        ecs_vector_free(stage);
    }

    ecs_map_clear(stage->entity_index);
    ecs_map_clear(stage->remove_merge);
    ecs_map_clear(stage->data_stage);
}

static
void clean_families(
    ecs_stage_t *stage)
{
    ecs_map_free(stage->type_index); 
}

static
void clean_tables(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_table_t *buffer = ecs_vector_first(stage->tables);
    int32_t i, count = ecs_vector_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = &buffer[i];
        ecs_table_free(world, table);
    }

    ecs_vector_free(stage->tables);
}

/* -- Private functions -- */

void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    bool is_main_stage = stage == &world->main_stage;
    bool is_temp_stage = stage == &world->temp_stage;

    memset(stage, 0, sizeof(ecs_stage_t));

    stage->entity_index = ecs_map_new(0);

    if (is_temp_stage) {
        stage->type_index = world->main_stage.type_index;
    } else {
        stage->type_index = ecs_map_new(0);
    }

    stage->table_index = ecs_map_new(0);
    if (is_main_stage) {
        stage->tables = ecs_vector_new(&table_arr_params, 8);
    } else {
        stage->tables = ecs_vector_new(&table_arr_params, 0);
    }

    if (!is_main_stage) {
        stage->data_stage = ecs_map_new(0);
        stage->remove_merge = ecs_map_new(0);
    }

    stage->commit_count = 0;
    stage->to_type = 0;
    stage->from_type = 0;
    stage->range_check_enabled = true;
}

void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
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
    ecs_stage_t *stage)
{
    assert(stage != &world->main_stage);
    
    /* Keep track of old number of tables so we know how many have been added */
    uint32_t old_table_count = ecs_vector_count(world->main_stage.tables);
    
    bool is_temp_stage = stage == &world->temp_stage;

    /* Merge any new families */
    if (!is_temp_stage) {
        merge_families(world, stage);
    }
    
    /* Merge entities. This can create tables if a new combination of components
     * is found after merging the staged type with the non-staged type. */
    merge_commits(world, stage);

    /* Clear temporary tables used by stage */
    ecs_vector_clear(stage->tables);
    ecs_map_clear(stage->table_index);

    /* Now that all data has been merged, evaluate columns of added tables. This
     * step updates the world for special columns, like prefab components */
    uint32_t new_table_count = ecs_vector_count(world->main_stage.tables);
    if (old_table_count != new_table_count) {
        eval_table_columns(world, old_table_count, new_table_count);
    }
}
