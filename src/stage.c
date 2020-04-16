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
    /* Loop the tables for which the stage has modified data */
    uint32_t i, count = ecs_vector_count(stage->dirty_tables);
    ecs_table_t **tables = ecs_vector_first(stage->dirty_tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = tables[i];
        ecs_data_t *main_data = ecs_table_get_data(world, table);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        uint32_t i, count = ecs_table_data_count(data);
        ecs_entity_t *entities = ecs_vector_first(data->entities);
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs);
        uint32_t c, column_count = ecs_vector_count(table->type);

        ecs_assert(main_data != data, ECS_INTERNAL_ERROR, NULL);

        /* Ensure destination row array is large enough */
        ecs_vector_set_min_size(&table->dst_rows, int32_t, count);
        uint32_t *dst_rows = ecs_vector_first(table->dst_rows);

        /* First make sure that all entities have entries in the main stage */
        for (i = 0; i < count; i ++) {
            ecs_record_t *record = record_ptrs[i];
            ecs_entity_t entity = entities[i];

            /* If the entity did not yet exist in the main stage, register it */
            if (!record) {
                record = ecs_eis_get_or_create(&world->stage, entity);
                record->table = NULL;
            }

            /* Check if entity was already stored in this main stage table. If
             * not, add a new row */
            ecs_table_t *src_table = record->table;
            if (src_table != table) {
                record->table = table;
                dst_rows[i] = record->row = ecs_table_append(
                    world, table, main_data, entity, record);
            } else {
                dst_rows[i] = record->row;
            }
        }

        /* Now copy data column by column from the stage to the main stage */
        for (c = 0; c < column_count; c ++) {
            ecs_column_t *main_column = &main_data->columns[c];
            void *main_data = ecs_vector_first(main_column->data);

            ecs_column_t *column = &data->columns[c];
            void *data = ecs_vector_first(column->data);
            uint16_t size = column->size;
            void *src = data;

            for (i = 0; i < count; i ++) {
                void *dst = ECS_OFFSET(main_data, size * dst_rows[i]);
                memcpy(dst, src, size);
                void *src = ECS_OFFSET(src, size);
            }

            ecs_vector_clear(column->data);
        }

        ecs_vector_clear(data->entities);
        ecs_vector_clear(data->record_ptrs);
    }

    ecs_vector_clear(stage->dirty_tables);
    ecs_eis_clear(stage);
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

        stage->tables = world->stage.tables;
        stage->root = world->stage.root;
    } else {
    }
    
    stage->table_index = ecs_map_new(ecs_table_t*, 0);
    
    if (is_main_stage) {
        stage->tables = ecs_sparse_new(ecs_table_t, 64);
    } else {
        stage->tables = ecs_sparse_new(ecs_table_t, 8);
    }
    
    if (!is_main_stage) {
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
