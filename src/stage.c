#include "flecs_private.h"

static
void merge_families(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    bool is_temp_stage = stage == &world->temp_stage;
    if (is_temp_stage) {
        if (stage->last_link) {
            world->stage.last_link = stage->last_link;
        }

        stage->last_link = NULL;
    }
}

static
void notify_new_tables(
    ecs_world_t *world, 
    int32_t old_table_count, 
    int32_t new_table_count) 
{
    ecs_sparse_t *tables = world->stage.tables;
    int32_t i;

    for (i = old_table_count; i < new_table_count; i ++) {
        ecs_table_t *t = ecs_sparse_get(tables, ecs_table_t, i);
        ecs_notify_queries_of_table(world, t);
    }
}

static
void clean_data_stage(
    ecs_stage_t *stage)
{
    ecs_eis_clear(stage);
}

static
void merge_commits(
    ecs_world_t *world,
    ecs_stage_t *stage)
{  
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);

    /* Merge commits */
    ecs_ei_iter_t iter = ecs_ei_iter(&stage->entity_index);
    ecs_record_t *record;
    ecs_entity_t entity;

    while ((record = ecs_ei_next(&iter, &entity))) {
        ecs_merge_entity(world, stage, entity, *record);
    }
}

static
void clean_types(
    ecs_stage_t *stage)
{
    (void)stage;
    
    /* TODO: clean types */
}

static
void clean_tables(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *t = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_table_free(world, t);
    }
}

/* -- Private functions -- */

void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    bool is_main_stage = stage == &world->stage;
    bool is_temp_stage = stage == &world->temp_stage;

    memset(stage, 0, sizeof(ecs_stage_t));

    /* Initialize entity index */
    ecs_eis_new(stage);

    if (is_main_stage) {
        stage->last_link = &world->stage.type_root.link;
        stage->id = 0;
    } else if (is_temp_stage) {
        stage->type_root = world->stage.type_root;
        stage->last_link = NULL;
        stage->id = 1;
    } else {
    }
    
    stage->table_index = ecs_map_new(ecs_table_t*, 0);
    if (is_main_stage) {
        stage->tables = ecs_sparse_new(ecs_table_t, 64);
    } else {
        stage->tables = ecs_sparse_new(ecs_table_t, 8);
    }

    if (!is_main_stage) {
        stage->remove_merge = ecs_map_new(ecs_type_t, 0);
        stage->entity_index.keep_deletes = true;
    }

    stage->range_check_enabled = true;
}

void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    bool is_main_stage = stage == &world->stage;
    bool is_temp_stage = stage == &world->temp_stage;

    if (!is_temp_stage) {
        clean_types(stage);
    }

    if (!is_main_stage) {
        clean_data_stage(stage);
        ecs_map_free(stage->remove_merge);
    }

    clean_tables(world, stage);
    ecs_sparse_free(stage->tables);
    ecs_map_free(stage->table_index);
    ecs_eis_free(stage);
}

void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    assert(stage != &world->stage);
    
    /* Keep track of old number of tables so we know how many have been added */
    int32_t old_table_count = ecs_sparse_count(world->stage.tables);
    
    /* Merge any new types */
    merge_families(world, stage);
    
    /* Merge entities. This can create tables if a new combination of components
     * is found after merging the staged type with the non-staged type. */
    merge_commits(world, stage);

    /* Clear temporary tables used by stage */
    clean_tables(world, stage);
    ecs_sparse_clear(stage->tables);
    ecs_map_clear(stage->table_index);
    ecs_eis_clear(stage);

    /* Now that all data has been merged, evaluate columns of added tables. This
     * step updates the world for special columns, like prefab components */
    int32_t new_table_count = ecs_sparse_count(world->stage.tables);
    if (old_table_count != new_table_count) {
        notify_new_tables(world, old_table_count, new_table_count);
    }
}
