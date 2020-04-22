#include "flecs_private.h"

static
ecs_data_t* init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = table->type; 
    int32_t i, count = ecs_vector_count(type);
    
    result->entities = NULL;
    result->record_ptrs = NULL;
    result->marked_dirty = false;

    /* Root tables don't have columns */
    if (!count) {
        result->columns = NULL;
        return result;
    }

    result->columns = ecs_os_calloc(sizeof(ecs_column_t), count);
    
    ecs_entity_t *entities = ecs_vector_first(type);

    for (i = 0; i < count; i ++) {
        const EcsComponent *component = ecs_get_ptr(
                world, entities[i], EcsComponent);

        /* Is the column a component? */
        if (component) {
            /* Is the component associated wit a (non-empty) type? */
            if (component->size) {
                /* This is a regular component column */
                result->columns[i].size = component->size;
            } else {
                /* This is a tag */
            }
        } else {
            /* This is an entity that was added to the type */
        }
    }

    return result;
}

/* Delete data for a stage */
static
void deinit_data(
    ecs_table_t *table,
    ecs_data_t *data)
{
    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = ecs_vector_count(table->type);
        for (c = 0; c < column_count; c ++) {
            ecs_vector_free(columns[c].data);
        }
        ecs_os_free(columns);
        data->columns = NULL;
    }

    ecs_vector_free(data->entities);
    ecs_vector_free(data->record_ptrs);

    data->entities = NULL;
    data->record_ptrs = NULL;
}

/* Utility function to free data for all stages */
static
void deinit_all_data(
    ecs_table_t *table)
{
    ecs_data_t *stage_data = ecs_vector_first(table->stage_data);
    int32_t i, count = ecs_vector_count(table->stage_data);
    
    for (i = 0; i < count; i ++) {
        deinit_data(table, &stage_data[i]);
    }

    ecs_vector_free(table->stage_data);

    table->stage_data = NULL; 
}

static
void run_on_remove_handlers(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data) {
        return;
    }

    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        ecs_entities_t components = {
            .array = ecs_vector_first(table->type),
            .count = ecs_vector_count(table->type)
        };

        ecs_run_deinit_actions(
            world, &world->stage, table, data, 0, count, components);
    }
}

/* -- Private functions -- */

/* If table goes from 0 to >0 entities or from >0 entities to 0 entities notify
 * queries. This allows systems associated with queries to move inactive tables
 * out of the main loop. */
void ecs_table_activate(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    bool activate)
{
    if (query) {
        ecs_query_activate_table(world, query, table, activate);
    } else {
        ecs_vector_t *queries = table->queries;
        
        if (queries) {
            ecs_query_t **buffer = ecs_vector_first(queries);
            int32_t i, count = ecs_vector_count(queries);
            for (i = 0; i < count; i ++) {
                ecs_query_activate_table(world, buffer[i], table, activate);
            }
        }
    }
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
void ecs_table_register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query)
{
    /* Register system with the table */
    ecs_query_t **q = ecs_vector_add(&table->queries, ecs_query_t*);
    if (q) *q = query;

    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data && ecs_vector_count(data->entities)) {
        ecs_table_activate(world, table, query, true);
    }
}

static
ecs_data_t* ecs_table_get_data_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    bool create)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *stage_data = table->stage_data;

    /* If the table doesn't contain any staged data and we're not asked to
     * create data, don't allocate the array. This will reduce memory footprint
     * for tables that don't contain data but are used for graph traversal */
    if (!stage_data && !create) {
        return NULL;
    }

    ecs_data_t *data_array = ecs_vector_first(stage_data);
    int32_t count = ecs_vector_count(stage_data);
    int32_t stage_count = world->stage_count;
    int32_t id = stage->id;

    ecs_assert(id < stage_count, ECS_INTERNAL_ERROR, NULL);

    /* Make sure the array is large enough for the number of active stages. This
     * guarantees that any pointers returned by this function are stable, unless
     * the number of stages changes (can happen when the number of worker 
     * threads changes) */
    if (count != stage_count) {
        if (stage_count > count) {
            /* Grow array, initialize table data to 0 */
            ecs_vector_set_count(&table->stage_data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->stage_data);
            memset(&data_array[count], 
                0, sizeof(ecs_data_t) * (stage_count - count));
        } else {
            /* If the number of stages is reduced, deinit redudant stages */
            int i;
            for (i = stage_count; i < count; i ++) {
                deinit_data(table, &data_array[i]);
            }

            ecs_vector_set_count(&table->stage_data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->stage_data);           
        }

        int i;
        for (i = 0; i < stage_count; i ++) {
            ecs_data_t *data_array = ecs_vector_first(table->stage_data);
            ecs_stage_t *stage_array = ecs_vector_first(world->worker_stages);

            data_array[0].stage = &world->stage;
            data_array[1].stage = &world->temp_stage;

            for (i = 2; i < stage_count; i ++) {
                data_array[i].stage = &stage_array[i - 2];
            }
        }
    }
    
    return &data_array[id];
}

ecs_data_t* ecs_table_get_data(
    ecs_world_t *world,
    ecs_table_t *table)
{
    return ecs_table_get_data_intern(world, &world->stage, table, false);
}

ecs_data_t* ecs_table_get_staged_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    return ecs_table_get_data_intern(world, stage, table, false);
}

ecs_data_t* ecs_table_get_or_create_data(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    ecs_data_t *result = ecs_table_get_data_intern(world, stage, table, true);

    /* If write access is requested from table and stage is not the main 
     * stage mark this table as dirty as it will contain staged data. */
    if (world->in_progress && !result->marked_dirty) {
        ecs_table_t** table_ptr = ecs_vector_add(
                &stage->dirty_tables, ecs_table_t*);
        *table_ptr = table;

        /* Don't add table multiple times. Value is reset during merge */
        result->marked_dirty = true;
    }

    return result;   
}

void ecs_table_clear_data(
    ecs_table_t *table,
    ecs_data_t *data)
{
    deinit_data(table, data);
}

/* Clear columns. Deactivate table in systems if necessary, but do not invoke
 * OnRemove handlers. This is typically used when restoring a table to a
 * previous state. */
void ecs_table_clear_silent(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (!data) {
        return;
    }

    int32_t count = ecs_vector_count(data->entities);
    
    deinit_all_data(table);

    if (count) {
        ecs_table_activate(world, table, 0, false);
    }
}

/* Delete all entities in table, invoke OnRemove handlers. This function is used
 * when an application invokes delete_w_filter. Use ecs_table_clear_silent, as the
 * table may have to be deactivated with systems. */
void ecs_table_clear(
    ecs_world_t *world,
    ecs_table_t *table)
{
    run_on_remove_handlers(world, table);

    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        ecs_entity_t *entities = ecs_vector_first(data->entities);
        int32_t i, count = ecs_vector_count(data->entities);
        for(i = 0; i < count; i ++) {
            ecs_eis_delete(&world->stage, entities[i]);
        }
    }

    ecs_table_clear_silent(world, table);
}

/* This operation is called when the world is freed. Because of the sequence in
 * which things are cleaned up, this cannot be combined with ecs_table_free. */
void ecs_table_deinit_components(
    ecs_world_t *world,
    ecs_table_t *table)
{
    run_on_remove_handlers(world, table);
}

/* Free table resources. Do not invoke handlers and do not activate/deactivate
 * table with systems. This function is used when the world is freed. */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    deinit_all_data(table);
    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    ecs_vector_free(table->queries);
    ecs_vector_free((ecs_vector_t*)table->type);
}

/* Replace columns. Activate / deactivate table with systems if necessary. */
void ecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = ecs_vector_first(table->stage_data);

    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);

    if (table_data) {
        prev_count = ecs_vector_count(table_data->entities);
        deinit_data(table, table_data);
    }

    table_data = ecs_table_get_or_create_data(world, &world->stage, table);
    if (data) {
        *table_data = *data;
    }

    int32_t count = 0;
    if (table_data) {
        count = ecs_vector_count(table_data->entities);
    }

    if (!prev_count && count) {
        ecs_table_activate(world, table, 0, true);
    } else if (prev_count && !count) {
        ecs_table_activate(world, table, 0, false);
    }
}

int32_t ecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t column_count = ecs_vector_count(table->type);

    if (column_count) {
        ecs_column_t *columns = data->columns;

        /* It is possible that the table data was created without content. Now that
        * data is going to be written to the table, initialize it */ 
        if (!columns) {
            init_data(world, table, data);
            columns = data->columns;
        }

        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Add elements to each column array */
        int32_t i;
        for (i = 0; i < column_count; i ++) {
            int32_t size = columns[i].size;
            if (size) {
                _ecs_vector_add(&columns[i].data, size);
            }
        }
    }

    /* Fist add entity to array with entity ids */
    ecs_entity_t *e = ecs_vector_add(&data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;

    /* Add record ptr to array with record ptrs */
    ecs_record_t **r = ecs_vector_add(&data->record_ptrs, ecs_record_t*);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    *r = record;

    int32_t index = ecs_vector_count(data->entities) - 1;

    if (!world->in_progress && !index) {
        ecs_table_activate(world, table, 0, true);
    }

    /* Return index of last added entity */
    return index;
}

void ecs_table_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *entity_column = data->entities;
    int32_t count = ecs_vector_count(entity_column);

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    uint32_t column_count = ecs_vector_count(table->type);
    uint32_t i;

    if (index != count) {   
        /* Move last entity id to index */     
        ecs_entity_t *entities = ecs_vector_first(entity_column);
        ecs_entity_t entity_to_move = entities[count];
        entities[index] = entity_to_move;
        ecs_vector_remove_last(entity_column);

        /* Move last record ptr to index */
        ecs_vector_t *record_column = data->record_ptrs;     
        ecs_record_t **records = ecs_vector_first(record_column);
        ecs_record_t *record_to_move = records[count];
        records[index] = record_to_move;
        ecs_vector_remove_last(record_column);

        /* Move each component value in array to index */
        ecs_column_t *components = data->columns;
        for (i = 0; i < column_count; i ++) {
            ecs_column_t *component_column = &components[i];
            uint32_t size = component_column->size;
            if (size) {
                _ecs_vector_remove_index(
                    component_column->data, size, index);
            }
        }

        /* Update record of moved entity in entity index */
        if (!world->in_progress && record_to_move) {
            record_to_move->row = index + 1;
        } else {
            ecs_record_t row;
            row.table = table;
            row.row = index + 1;
            ecs_eis_set(stage, entity_to_move, &row);
        }

    /* If this is the last entity in the table, just decrease column counts */
    } else {
        ecs_vector_remove_last(entity_column);
        ecs_vector_remove_last(data->record_ptrs);

        ecs_column_t *components = data->columns;
        for (i = 0; i < column_count; i ++) {
            ecs_column_t *component_column = &components[i];
            if (component_column->size) {
                ecs_vector_remove_last(component_column->data);
            }
        }
    }

    if (!world->in_progress && !count) {
        ecs_table_activate(world, table, NULL, false);
    }
}

int32_t ecs_table_grow(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count,
    ecs_entity_t first_entity)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_count(table->type);
    ecs_column_t *columns = NULL;

    if (column_count) {
        columns = data->columns;

        /* It is possible that the table data was created without content. Now that
        * data is going to be written to the table, initialize it */ 
        if (!columns) {
            init_data(world, table, data);
            columns = data->columns;
        }

        ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    /* Fist grow record ptr array */
    ecs_record_t **r = ecs_vector_addn(&data->record_ptrs, ecs_record_t*, count);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);    

    /* Fist add entity to column with entity ids */
    ecs_entity_t *e = ecs_vector_addn(&data->entities, ecs_entity_t, count);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < count; i ++) {
        e[i] = first_entity + i;
        r[i] = NULL;
    }

    /* Add elements to each column array */
    for (i = 0; i < column_count; i ++) {
        size_t column_size = columns[i].size;
        if (!column_size) {
            continue;
        }

        _ecs_vector_addn(&columns[i].data, column_size, count);
    }

    int32_t row_count = ecs_vector_count(data->entities);
    if (!world->in_progress && row_count == count) {
        ecs_table_activate(world, table, 0, true);
    }

    /* Return index of first added entity */
    return row_count - count;
}

int16_t ecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;

    if (!columns) {
        init_data(world, table, data);
        columns = data->columns;
    }

    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_set_size(&data->entities, ecs_entity_t, count);
    ecs_vector_set_size(&data->record_ptrs, ecs_record_t*, count);

    int32_t i, column_count = ecs_vector_count(table->type);
    for (i = 0; i < column_count; i ++) {
        int32_t column_size = columns[i].size;
        if (column_size) {
            _ecs_vector_set_size(&columns[i].data, column_size, count);
        }
    }

    return 0;
}

int16_t ecs_table_set_count(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;

    if (!columns) {
        init_data(world, table, data);
        columns = data->columns;
    }

    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_set_count(&data->entities, ecs_entity_t, count);
    ecs_vector_set_count(&data->record_ptrs, ecs_record_t*, count);

    int32_t i, column_count = ecs_vector_count(table->type);
    for (i = 0; i < column_count; i ++) {
        int32_t column_size = columns[i].size;
        if (column_size) {
            _ecs_vector_set_count(&columns[i].data, column_size, count);
        }
    }

    return 0;
}

uint64_t ecs_table_data_count(
    ecs_data_t *data)
{
    return data ? ecs_vector_count(data->entities) : 0;
}

uint64_t ecs_table_count(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *data = ecs_vector_first(table->stage_data);
    if (!data) {
        return 0;
    }

    return ecs_table_data_count(data);
}

void ecs_table_swap(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2,
    ecs_record_t *record_ptr_1,
    ecs_record_t *record_ptr_2)
{    
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(row_1 >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_2 >= 0, ECS_INTERNAL_ERROR, NULL);

    if (row_1 == row_2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities);
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];
    
    /* Get pointers to records in entity index */
    if (!record_ptr_1) {
        record_ptr_1 = ecs_eis_get(stage, e1);
    }

    if (!record_ptr_2) {
        record_ptr_2 = ecs_eis_get(stage, e2);
    }

    /* Swap entities */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = row_2 + 1;
    record_ptr_2->row = row_1 + 1;

    /* Swap columns */
    int32_t i, column_count = ecs_vector_count(table->type);
    
    for (i = 0; i < column_count; i ++) {
        void *data = ecs_vector_first(columns[i].data);
        int32_t size = columns[i].size;

        if (size) {
            void *tmp = _ecs_os_alloca(size, 1);

            void *el_1 = ECS_OFFSET(data, size * row_1);
            void *el_2 = ECS_OFFSET(data, size * row_2);

            memcpy(tmp, el_1, size);
            memcpy(el_1, el_2, size);
            memcpy(el_2, tmp, size);
        }
    }
}

void ecs_table_move_back_and_swap(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *entities = ecs_vector_first(data->entities);
    int32_t i;

    /* First move back and swap entities */
    ecs_entity_t e = entities[row - 1];
    for (i = 0; i < count; i ++) {
        ecs_entity_t cur = entities[row + i];
        entities[row + i - 1] = cur;

        ecs_record_t *record_ptr = ecs_eis_get(stage, cur);
        record_ptr->row = row + i;
    }

    entities[row + count - 1] = e;
    ecs_record_t *record_ptr = ecs_eis_get(stage, e);
    record_ptr->row = row + count;

    /* Move back and swap columns */
    int32_t column_count = ecs_vector_count(table->type);
    
    for (i = 0; i < column_count; i ++) {
        void *data = ecs_vector_first(columns[i].data);
        int32_t size = columns[i].size;

        if (size) {
            /* Backup first element */
            void *tmp = _ecs_os_alloca(size, 1);
            void *el = ECS_OFFSET(data, size * (row - 1));
            memcpy(tmp, el, size);

            /* Move component values */
            int32_t j;
            for (j = 0; j < count; j ++) {
                void *dst = ECS_OFFSET(data, size * (row + j - 1));
                void *src = ECS_OFFSET(data, size * (row + j));
                memcpy(dst, src, size);
            }

            /* Move first element to last element */
            void *dst = ECS_OFFSET(data, size * (row + count - 1));
            memcpy(dst, tmp, size);
        }
    }
}

static
void merge_vector(
    ecs_vector_t **dst_out,
    ecs_vector_t *src,
    uint32_t size)
{
    ecs_vector_t *dst = *dst_out;
    uint32_t dst_count = ecs_vector_count(dst);

    if (!dst_count) {
        if (dst) {
            ecs_vector_free(dst);
        }

        *dst_out = src;
    
    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        uint32_t src_count = ecs_vector_count(src);
        _ecs_vector_set_count(&dst, size, dst_count + src_count);
        
        void *dst_ptr = ecs_vector_first(dst);
        void *src_ptr = ecs_vector_first(src);

        dst_ptr = ECS_OFFSET(dst_ptr, size * src_count);
        memcpy(dst_ptr, src_ptr, size * src_count);

        ecs_vector_free(src);
        *dst_out = dst;
    }    
}

void ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table)
{
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_table != old_table, ECS_INTERNAL_ERROR, NULL);

    /* If there is no data to merge, drop out */
    ecs_data_t *old_data = ecs_table_get_data(world, old_table);
    if (!old_data) {
        return;
    }

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;
    ecs_assert(new_type != old_type, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *new_data = ecs_table_get_or_create_data(
        world, &world->stage, new_table);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;
    if (!new_columns) {
        init_data(world, new_table, new_data);
        new_columns = new_data->columns;
    }

    ecs_assert(new_columns != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t old_count = old_columns->data ? ecs_vector_count(old_columns->data) : 0;
    int32_t new_count = 0;
    if (new_columns) {
        new_count = new_columns->data ? ecs_vector_count(new_columns->data) : 0;
    }

    /* First, update entity index so old entities point to new type */
    ecs_entity_t *old_entities = ecs_vector_first(old_data->entities);
    int32_t i;
    for(i = 0; i < old_count; i ++) {
        ecs_record_t record = {.table = new_table, .row = i + new_count};
        ecs_eis_set(&world->stage, old_entities[i], &record);
    }

    if (!new_table) {
        ecs_table_clear(world, old_table);
        return;
    }

    uint16_t i_new, new_component_count = ecs_vector_count(new_type);
    uint16_t i_old = 0, old_component_count = ecs_vector_count(old_type);
    ecs_entity_t *new_components = ecs_vector_first(new_type);
    ecs_entity_t *old_components = ecs_vector_first(old_type);

    if (!old_count) {
        return;
    }

    for (i_new = 0; i_new < new_component_count; ) {
        if (i_old == old_component_count) {
            break;
        }

        ecs_entity_t new_component = 0;
        ecs_entity_t old_component = 0;
        int32_t size = 0;

        if (i_new) {
            new_component = new_components[i_new - 1];
            old_component = old_components[i_old - 1];
            size = new_columns[i_new].size;
        } else {
            size = sizeof(ecs_entity_t);
        }

        if ((new_component & ECS_ENTITY_FLAGS_MASK) || 
            (old_component & ECS_ENTITY_FLAGS_MASK)) 
        {
            break;
        }

        if (new_component == old_component) {
            merge_vector(
                &new_columns[i_new].data, old_columns[i_old].data, size);
            old_columns[i_old].data = NULL;
            
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            /* This should not happen. A table should never be merged to
             * another table of which the type is not a subset. */
            ecs_abort(ECS_INTERNAL_ERROR, NULL);
        } else if (new_component > old_component) {
            /* Old column does not occur in new table, remove */
            ecs_vector_free(old_columns[i_old].data);
            old_columns[i_old].data = NULL;
            i_old ++;
        }
    }

    merge_vector(&new_data->entities, old_data->entities, sizeof(ecs_entity_t));
    old_data->entities = NULL;
}

static
void copy_column(
    ecs_column_t *new_column,
    int32_t new_index,
    ecs_column_t *old_column,
    int32_t old_index)
{
    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    int32_t size = new_column->size;

    if (size) {
        size_t column_size = new_column->size;
        
        void *dst = _ecs_vector_get(new_column->data, column_size, new_index);
        void *src = _ecs_vector_get(old_column->data, column_size, old_index);
            
        ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

        memcpy(dst, src, column_size);
    }
}

void ecs_table_move(
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index)
{
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    uint16_t i_new, new_column_count = ecs_vector_count(new_type);
    if (!new_column_count) {
        return;
    }

    uint16_t i_old = 0, old_column_count = ecs_vector_count(old_type);
    ecs_entity_t *new_components = ecs_vector_first(new_type);
    ecs_entity_t *old_components = ecs_vector_first(old_type);

    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    ecs_assert(old_columns != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_columns != NULL, ECS_INTERNAL_ERROR, NULL);

    for (i_new = 0; i_new < new_column_count; ) {
        if (i_old == old_column_count) {
            break;
        }

        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if ((new_component & ECS_ENTITY_FLAGS_MASK) || 
            (old_component & ECS_ENTITY_FLAGS_MASK)) 
        {
            break;
        }

        if (new_component == old_component) {
            copy_column(&new_columns[i_new], new_index, &old_columns[i_old], old_index);
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            i_new ++;
        } else if (new_component > old_component) {
            i_old ++;
        }
    }
}

bool ecs_table_match_filter(
    ecs_world_t *world,
    ecs_table_t *table,
    const ecs_filter_t *filter)
{
    if (!filter) {
        return true;
    }

    ecs_type_t type = table->type;
    
    if (filter->include) {
        /* If filter kind is exact, types must be the same */
        if (filter->include_kind == EcsMatchExact) {
            if (type != filter->include) {
                return false;
            }

        /* Default for include_kind is MatchAll */
        } else if (!ecs_type_contains(world, type, filter->include, 
            filter->include_kind != EcsMatchAny, true)) 
        {
            return false;
        }
    
    /* If no include filter is specified, make sure that builtin components
     * aren't matched by default. */
    } else {
        if (table->flags & EcsTableHasBuiltins)
        {
            /* Continue if table contains any of the builtin components */
            return false;
        }
    }

    if (filter->exclude) {
        /* If filter kind is exact, types must be the same */
        if (filter->exclude_kind == EcsMatchExact) {
            if (type == filter->exclude) {
                return false;
            }
        
        /* Default for exclude_kind is MatchAny */                
        } else if (ecs_type_contains(world, type, filter->exclude, 
            filter->exclude_kind == EcsMatchAll, true))
        {
            return false;
        }
    }

    return true;
}