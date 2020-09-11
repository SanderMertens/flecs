#include "private_api.h"

/* If all components were removed, don't store the entity in a table. If the
 * entity was deleted, also remove the entity from the entity index so that its
 * id can be recycled. */
static
void clear_columns(
    ecs_world_t *world,
    ecs_stage_t *stage, 
    ecs_data_t *data)
{
    int32_t i, entity_count = ecs_vector_count(data->entities);
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);

    for (i = 0; i < entity_count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_record_t *record = record_ptrs[i];
        
        if (!record && e > ECS_HI_ENTITY_ID) {
            record = ecs_eis_get(stage, e);
        }

        /* Remove component data from previous main stage table */
        if (record) {
            bool is_watched;
            int32_t row = ecs_record_to_row(record->row, &is_watched);

            ecs_table_t *src_table = record->table;
            if (record && record->table) {
                ecs_data_t *src_data = ecs_table_get_data(world, src_table);

                ecs_table_delete(
                    world, &world->stage, src_table, src_data, row, true);
            }

            if (is_watched) {
                ecs_delete_children(world, e);
            }

            /* If the staged record has the table set to the root, this is an entity
            * without components. If the table is NULL, this is a delete. */
            ecs_record_t *staged_record = ecs_eis_get(stage, e);
            if (staged_record->table) {
                /* Clear the entity record. This will set the table to NULL but
                 * if necessary, retain information about whether the entity is
                 * being watched or not. This is not the same as a delete, where
                 * the is_watched information is lost, and the entity id can be
                 * recycled. */
                ecs_eis_clear_entity(&world->stage, e, is_watched);
            } else {
                ecs_eis_delete(&world->stage, e);
            }
        } else {
            ecs_eis_delete(&world->stage, e);
        }
    }
}

static
void merge_commits(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Loop the tables for which the stage has modified data */
    int32_t i, table_count = ecs_vector_count(stage->dirty_tables);
    ecs_table_t **tables = ecs_vector_first(stage->dirty_tables, ecs_table_t*);

    for (i = 0; i < table_count; i ++) {
        ecs_table_t *table = tables[i];
        ecs_data_t *main_data = ecs_table_get_data(world, table);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        int32_t e, entity_count = ecs_table_data_count(data);
        ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
        ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
        int32_t component_count = ecs_vector_count(table->type);
        
        ecs_assert(main_data != data, ECS_INTERNAL_ERROR, NULL);
        data->marked_dirty = false;

        /* If the table contains no data, this was a staged table that was
         * merged with the main stage. The main stage table will also have been
         * added to the dirty_table list, so we'll iterate it eventually */
        if (!data->entities) {
            continue;
        }

        /* If the table has no columns, this is the root table. Entities that
         * are in a root table need to be either deleted or emptied */
        if (!component_count) {
            clear_columns(world, stage, data);
            ecs_table_clear_data(table, data);
            continue;
        }

        /* If there are no entities to be merged, clear table data. This can
         * happen if a table was populated in a stage causing it to be marked as
         * dirty, and then the data got moved to another staged table. */
        if (!entity_count) {
            ecs_table_clear_data(table, data);
            continue;
        }

        for (e = 0; e < entity_count; e ++) {
            ecs_record_t *record = record_ptrs[e];            
            ecs_entity_t entity = entities[e];

            /* If the entity did not yet exist in the main stage, register it */
            if (!record) {
                record = ecs_eis_get_or_create(&world->stage, entity);
                record_ptrs[e] = record;
            }

            bool is_watched;
            int32_t row = ecs_record_to_row(record->row, &is_watched);            
            ecs_table_t *src_table = record->table;

            /* If one or more of the components in the table have a move action
             * we need to move the entity from source to destination table first
             * in the main stage, so that we can move the values from the stage
             * into the current values. */
            if (src_table != table) {
                /* Insert row for entity into target table */
                int32_t dst_row = ecs_table_append(
                    world, table, main_data, entity, record, true);

                if (src_table) {
                    ecs_data_t *src_data = ecs_table_get_data(world, src_table);

                    /* Move entity from src table to destination table */
                    if (src_table->flags & EcsTableHasMove) {
                        ecs_table_move(world, &world->stage, entity, entity, table, 
                            main_data, dst_row, src_table, src_data, row, true);
                    }

                    /* Delete data from source table */
                    ecs_table_delete(
                        world, &world->stage, src_table, src_data, row, false);
                }

                /* Now move data from stage into the main stage */
                ecs_table_move(world, &world->stage, entity, entity, table, 
                    main_data, dst_row, table, data, e, false);

                record->table = table;
                record->row = ecs_row_to_record(dst_row, is_watched);
            } else {
                /* If entity is already in this table, simply move */
                ecs_table_move(world, &world->stage, entity, entity, table, 
                    main_data, row, table, data, e, false);
            }
        }

        ecs_table_clear_data(table, data);
    }

    /* All dirty tables are processed, clear array for next frame. */
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

    /* Clear the root table */
    if (count) {
        ecs_table_reset(world, &stage->root);
    }

    ecs_sparse_clear(stage->tables);
}

/* If a table was created while staged, it was not yet matched with OnSet
 * systems and monitors. Invoke them during the merge. When this applies to a
 * large amount of entities this can be expensive. To reduce this overhead, an
 * application can define the table beforehand by preallocating memory for it */
static
void merge_on_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data)
{
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_record_t **r_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    int32_t i, count = ecs_table_data_count(data);
    
    world->in_progress = true;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        ecs_record_t *r = r_ptrs[i];
        ecs_table_t *src_table = NULL;
        ecs_vector_t *src_mon = NULL;
        ecs_vector_t *src_set = NULL;

        /* Get record from main table so we can figure out which components were
         * added/set while the entity was staged */
        if (!r) {
            r = ecs_eis_get(&world->stage, e);
            if (r) {
                src_table = r->table;
            }
        } else {
            src_table = r->table;
        }

        /* Update record in stage to the table global table */
        ecs_record_t *staged_r = ecs_eis_get(stage, e);
        ecs_assert(staged_r != NULL, ECS_INTERNAL_ERROR, NULL);
        staged_r->table = table;

        if (src_table) {
            src_set = src_table->on_set_all;
            src_mon = src_table->monitors;

            /* We don't know if components were added or removed, so we have to 
             * run UnSet systems too */
            ecs_run_monitors(world, stage, table, src_table->un_set_all, 
                i, 1, table->un_set_all);  

            /* It is possible that removing a component caused a base component
             * to get exposed, which should trigger an OnSet system. */
            if (table->flags & EcsTableHasBase) {
                ecs_run_monitors(world, stage, table, 
                    src_table->on_set_override, i, 1, table->on_set_override);
            }                
        }

        /* Run OnSet systems */
        if (table->on_set_all) {
            ecs_run_monitors(world, stage, table, table->on_set_all, i, 1, src_set);
        }

        /* Run monitors */
        if (table->monitors) {
            ecs_run_monitors(world, stage, table, table->monitors, i, 1, src_mon);
        }

    }
    
    world->in_progress = false;
}

static
void merge_tables(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t start)
{
    /* Merge tables created in stage */
    int32_t i, count = ecs_sparse_count(stage->tables);

    if (start == count) {
        return;
    }

    for (i = start; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);

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
         * that could have populated the *stage-specific* table data, and we
         * wouldn't be here if the stage had found the table in the main
         * stage. */
#ifndef NDEBUG
        ecs_data_t *staged_data = ecs_table_get_staged_data(
            world, stage, main_table);

        ecs_assert(!staged_data || !staged_data->columns, 
            ECS_INTERNAL_ERROR, NULL);
#endif

        ecs_data_t *main_staged_data = ecs_table_get_or_create_data(
            world, stage, main_table);
        
        ecs_assert(main_staged_data != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Move the staged data from the staged table to the stage-specific
         * location in the main stage. This will ensure that the data will
         * be merged in the next step. Reset the data pointer to NULL in the
         * staged table so it won't be cleaned up */
        if (data) {
            *main_staged_data = *data;
        }

        /* If the main_table has been matched with OnSet systems, these
         * systems have not yet been invoked for the entities in this table
         * as systems aren't matched while in progress. Invoke them now. */
        if (main_table->on_set_all || main_table->un_set_all || 
            main_table->monitors || main_table->on_set_override) 
        {
            /* TODO: if an entity is moved by an OnSet handler to a new
             * table that is also created in the stage, it is possible that
             * an OnSet handler is executed twice. To prevent this from
             * happening, only invoke OnSet handlers on tables that were
             * created while iterating. This introduces an edge case where
             * an OnSet system that is applicable exclusively to the new
             * staged table is not invoked, but to address this case we need
             * to keep a shadow entity index while merging, as the only way
             * to determine the exclusive set of OnSet systems is by taking
             * the previous staged table and the new staged table. */
            if (start == 0) {
                merge_on_set(world, stage, main_table, main_staged_data);
            }
        }

        /* Add main stage table to dirty_tables. This will cause both the
         * staged table as well as the main stage table to be added to
         * the array. This is ok, as the staged table is now empty, so 
         * entities won't be added twice. */
        ecs_table_t **el = ecs_vector_add(&stage->dirty_tables, ecs_table_t*);
        *el = main_table;
    }

    /* It is possible that new tables were introduced to the stage while OnSet
     * handlers were executed. Merge those tables as well */
    merge_tables(world, stage, count);

    /* Reset table data */
    for (i = start; i < count; i ++) {
        ecs_table_t *table = ecs_sparse_get(stage->tables, ecs_table_t, i);
        ecs_data_t *data = ecs_table_get_staged_data(world, stage, table);
        if (data) {
            *data = (ecs_data_t){ 0 };
        }
    }    
}

/* -- Private functions -- */

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
    merge_tables(world, stage, 0);

    /* Merge entities. This can create tables if a new combination of components
     * is found after merging the staged type with the non-staged type. */
    merge_commits(world, stage);

    /* Clear temporary tables used by stage */
    clean_tables(world, stage);
    ecs_eis_clear(stage);
}

void ecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    bool is_main_stage = stage == &world->stage;
    bool is_temp_stage = stage == &world->temp_stage;

    memset(stage, 0, sizeof(ecs_stage_t));

    /* Initialize entity index */
    stage->entity_index = ecs_sparse_new(ecs_record_t);
    ecs_sparse_set_id_source(stage->entity_index, &world->stats.last_id);

    if (is_main_stage) {
        stage->id = 0;
    } else if (is_temp_stage) {
        stage->id = 1;
    }

    /* Initialize root table */
    stage->tables = ecs_sparse_new(ecs_table_t);

    /* Initialize one root table per stage */
    ecs_init_root_table(world, stage);

    stage->scope_table = &world->stage.root;
    stage->scope = 0;
    stage->defer = 0;
    stage->defer_queue = NULL;
    stage->post_frame_actions = NULL;

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

void ecs_stage_delete_table(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    /* Notify queries that table is to be removed */
    ecs_notify_queries(
        world, &(ecs_query_event_t){
            .kind = EcsQueryTableUnmatch,
            .table = table
        });

    /* Free resources associated with table */
    ecs_table_free(world, table);

    /* Remove table from sparse set */
    ecs_sparse_remove(stage->tables, table->id);
}
