#include "flecs_private.h"

static
void merge_commits(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Loop the tables for which the stage has modified data */
    uint32_t i, table_count = ecs_vector_count(stage->dirty_tables);
    ecs_table_t **tables = ecs_vector_first(stage->dirty_tables);

    for (i = 0; i < table_count; i ++) {
        ecs_table_t *table = tables[i];
        ecs_data_t *main_data = ecs_table_get_data(world, table);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        uint32_t e, entity_count = ecs_table_data_count(data);
        ecs_entity_t *entities = ecs_vector_first(data->entities);
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs);
        uint32_t c, column_count = ecs_vector_count(table->type);

        ecs_assert(main_data != data, ECS_INTERNAL_ERROR, NULL);

        data->marked_dirty = false;

        /* If the table contains no data, this was a staged table that was
         * merged with the main stage. The main stage table will also have been
         * added to the dirty_table list, so we'll iterate it eventually */
        if (!data->columns) {
            continue;
        }

        /* Ensure that table is large enough to store new entities */
        int32_t main_entity_count = ecs_table_count(table);
        ecs_table_set_count(world, table, main_data, main_entity_count + entity_count);

        /* Copy data column by column from the stage to the main stage */
        for (c = 0; c < column_count; c ++) {
            ecs_column_t *main_column = &main_data->columns[c];
            void *dst = ecs_vector_first(main_column->data);
            ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_column_t *column = &data->columns[c];
            uint16_t size = column->size;

            void *src = ecs_vector_first(column->data);
            ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_assert(size == main_column->size, ECS_INTERNAL_ERROR, NULL);

            memcpy( ECS_OFFSET(dst, main_entity_count * size), 
                src, size * entity_count);

            ecs_vector_clear(column->data);
        } 

        /* Copy entity ids */
        ecs_entity_t *main_entities = ecs_vector_first(main_data->entities);
        memcpy(&main_entities[main_entity_count], entities, entity_count * sizeof(ecs_entity_t));

        /* Copy record ptrs */
        ecs_record_t **main_record_ptrs = ecs_vector_first(main_data->record_ptrs);
        memcpy(&main_record_ptrs[main_entity_count], record_ptrs, entity_count * sizeof(ecs_record_t*));

        /* Update the entity index */
        for (e = 0; e < entity_count; e ++) {
            ecs_record_t *record = record_ptrs[e];
            ecs_entity_t entity = entities[e];

            /* If the entity did not yet exist in the main stage, register it */
            if (!record) {
                record = ecs_eis_get_or_create(&world->stage, entity);
            }

            ecs_table_t *src_table = record->table;
            if (src_table != table) {
                /* Delete entity from old table */
                ecs_data_t *src_data = ecs_table_get_data(world, src_table);
                ecs_table_delete(world, stage, src_table, src_data, record->row - 1);
            }

            record->table = table;
            record->row = main_entity_count + e + 1;
        }

        /* If the table was empty, activate it explicitly since we bypassed the
         * regular append path */
        if (!main_entity_count) {
            ecs_table_activate(world, table, NULL, true);
        }

        ecs_vector_clear(data->entities);
        ecs_vector_clear(data->record_ptrs);
    }

    ecs_vector_clear(stage->dirty_tables);
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
        stage->id = 0;
    } else if (is_temp_stage) {
        stage->id = 1;
    }

    /* Initialize root table */
    stage->tables = ecs_sparse_new(ecs_table_t, 64);

    /* Initialize one root table per stage */
    ecs_init_root_table(world, stage);

    if (!is_main_stage) {
        stage->entity_index.keep_deletes = true;
    }

    stage->range_check_enabled = true;
}

void ecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    clean_tables(world, stage);
    ecs_sparse_free(stage->tables);
    ecs_table_free(world, &stage->root);
    ecs_vector_free(stage->dirty_tables);
    ecs_eis_free(stage);
}

static
void merge_tables(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Merge tables created in stage */
    int32_t i, count = ecs_sparse_count(stage->tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);

        /* If the table does not contain any data and/or hasn't been modified
         * during this stage we can skip it. A stage has to recreate the parts
         * of the table graph up to the tables that contain the staged data, and
         * these tables do not need to be merged explicitly. Either they are
         * already created in the main stage, or they will be created as the
         * tables with actual data are merged. */
        if (ecs_table_data_count(data)) {
            /* Find or create the table in the main stage. Even though the
             * table may not have been created in the main stage when the stage
             * looked for it, other stages could have been merged before this
             * stage that already created the table. 
             *
             * If this is the first time that the table is created in the main
             * stage, this will also trigger table notifications for queries. */

            ecs_table_t *main_table = ecs_table_from_type(
                world, &world->stage, table->type);
            ecs_assert(main_table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(main_table != table, ECS_INTERNAL_ERROR, NULL);

            /* Make sure the main stage table does not yet contain data for this
             * stage. That should never happen, as this stage is the only one
             * that could have populated the stage-specific table data, and we
             * wouldn't be here if the stage had found the table in the main
             * stage. */
            ecs_assert(
                ecs_table_get_staged_data(world, stage, main_table) == NULL, 
                ECS_INTERNAL_ERROR, NULL);

            ecs_data_t *main_staged_data = ecs_table_get_or_create_data(
                world, stage, main_table);
            
            ecs_assert(main_staged_data != NULL, ECS_INTERNAL_ERROR, NULL);
                
            /* Move the staged data from the staged table to the stage-specific
             * location in the main stage. This will ensure that the data will
             * be merged in the next step. Reset the data pointer to NULL in the
             * staged table so it won't be cleaned up */
            *main_staged_data = *data;
            *data = (ecs_data_t){ NULL };

            /* Add main stage table to dirty_tables. This will cause both the
             * staged table as well as the main stage table to be added to
             * the array. This is ok, as the staged table is not empty, so 
             * entities won't be added twice. */
            ecs_table_t **el = ecs_vector_add(&stage->dirty_tables, ecs_table_t*);
            *el = main_table;
        }
    }
}

void ecs_stage_merge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(stage != &world->stage, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage->tables != world->stage.tables, ECS_INTERNAL_ERROR, NULL);

    /* First merge tables created by stage. This only happens if a new table was
     * created while iterating that did not yet exist in the main stage. Tables
     * are not modified in the main stage while iterating as this could cause
     * corruption of the administration while iterating, and in the case of
     * multithreaded applications wouldn't be safe. */
    merge_tables(world, stage);

    /* Merge entities. This can create tables if a new combination of components
     * is found after merging the staged type with the non-staged type. */
    merge_commits(world, stage);

    /* Clear temporary tables used by stage */
    clean_tables(world, stage);
    ecs_sparse_clear(stage->tables);
    ecs_eis_clear(stage);
}
