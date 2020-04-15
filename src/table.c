#include "flecs_private.h"

/** Notify systems that a table has changed its active state */
static
void activate_table(
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

static
ecs_data_t* init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = table->type; 
    int32_t i, count = ecs_vector_count(type);
    result->columns = ecs_os_calloc(sizeof(ecs_column_t), count);
    
    ecs_entity_t *entities = ecs_vector_first(type);

    for (i = 0; i < count; i ++) {
        EcsComponent *component = ecs_get_ptr_intern(
            world, &world->stage, entities[i], EEcsComponent, false, false);

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
            /* This is an entity that was added to the type, likely a CHILDOF or
             * an INSTANCEOF element */
        }

        /* Add flags that enable quickly checking if this table is special */

        if (table && entities[i] <= EcsLastBuiltin) {
            table->flags |= EcsTableHasBuiltins;
        }

        if (table && entities[i] == EEcsPrefab) {
            table->flags |= EcsTableIsPrefab;
        }
    }
    
    result->entities = NULL;

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
    data->entities = NULL;
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
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        ecs_notify(
            world, &world->stage, world->type_sys_remove_index, 
            table->type, table, data, 0, count);
    }
}

/* -- Private functions -- */

ecs_table_t *ecs_bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_assert(world->t_component != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_stage_t *stage = &world->stage;
    ecs_table_t *result = ecs_sparse_add(stage->tables, ecs_table_t);
    result->type = world->t_component;
    result->queries = NULL;
    result->flags = 0;
    result->flags |= EcsTableHasBuiltins;
    result->stage_data = NULL;

    ecs_data_t *data = ecs_vector_add(&result->stage_data, ecs_data_t);
    ecs_assert(data != NULL, ECS_OUT_OF_MEMORY, NULL);
    data->columns = ecs_os_malloc(sizeof(ecs_column_t) * 2);
    ecs_assert(data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_column_t *columns = data->columns;

    data->entities = ecs_vector_new(ecs_entity_t, 16);
    columns[0].data = ecs_vector_new(EcsComponent, 16);
    columns[0].size = sizeof(EcsComponent);
    columns[1].data = ecs_vector_new(EcsId, 16);
    columns[1].size = sizeof(EcsId);

    return result;    
}

static
ecs_data_t* ecs_table_get_data_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table,
    bool create)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data_array = ecs_vector_first(table->stage_data);
    int32_t count = ecs_vector_count(table->stage_data);
    int32_t stage_count = world->stage_count;
    int32_t id = stage->id;

    ecs_assert(id < stage_count, ECS_INTERNAL_ERROR, NULL);

    if (count != stage_count) {
        if (stage_count > count) {
            ecs_vector_set_count(&table->stage_data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->stage_data);
        }

        int32_t i;
        for (i = count; i < stage_count; i ++) {
            init_data(world, table, &data_array[i]);
        }

        for (i = stage_count; i < count; i ++) {
            deinit_data(table, &data_array[i]);
        }

        if (stage_count < count) {
            ecs_vector_set_count(&table->stage_data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->stage_data);
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
    return ecs_table_get_data_intern(world, stage, table, true);
}

void ecs_table_init(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_table_t *table)
{
    table->queries = NULL;
    table->flags = 0;
    table->stage_data = NULL;
}

/* Clear columns. Deactivate table in systems if necessary, but do not invoke
 * OnRemove handlers. This is typically used when restoring a table to a
 * previous state. */
void ecs_table_clear(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = ecs_table_get_data(world, table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t count = ecs_vector_count(data->entities);
    
    deinit_all_data(table);

    if (count) {
        activate_table(world, table, 0, false);
    }
}

/* Delete all entities in table, invoke OnRemove handlers. This function is used
 * when an application invokes delete_w_filter. Use ecs_table_clear, as the
 * table may have to be deactivated with systems. */
void ecs_table_delete_all(
    ecs_world_t *world,
    ecs_table_t *table)
{
    run_on_remove_handlers(world, table);
    ecs_table_clear(world, table);
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
    ecs_vector_free(table->queries);
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

    table_data = ecs_table_get_data(world, table);
    if (data) {
        *table_data = *data;
    }

    int32_t count = 0;
    if (table_data) {
        count = ecs_vector_count(table_data->entities);
    }

    if (!prev_count && count) {
        activate_table(world, table, 0, true);
    } else if (prev_count && !count) {
        activate_table(world, table, 0, false);
    }
}

void ecs_table_register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query)
{
    ecs_data_t *data = ecs_table_get_data(world, table);

    /* Register system with the table */
    ecs_query_t **q = ecs_vector_add(&table->queries, ecs_query_t*);
    if (q) *q = query;

    if (ecs_vector_count(data->entities)) {
        activate_table(world, table, query, true);
    }
}

int32_t ecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_count(table->type);

    /* Fist add entity to column with entity ids */
    ecs_entity_t *e = ecs_vector_add(&data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);

    *e = entity;

    /* Add elements to each column array */
    int32_t i;
    bool reallocd = false;

    for (i = 0; i < column_count; i ++) {
        int32_t size = columns[i].size;
        if (size) {
            void *old_vector = columns[i].data;

            _ecs_vector_add(&columns[i].data, size);
            
            if (old_vector != columns[i].data) {
                reallocd = true;
            }
        }
    }

    int32_t index = ecs_vector_count(data->entities) - 1;

    if (!world->in_progress && !index) {
        activate_table(world, table, 0, true);
    }

    ecs_data_t *main_data = ecs_table_get_data(world, table);
    if (reallocd && main_data == data) {
        world->should_resolve = true;
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
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *entity_vector = data->entities;
    int32_t count = ecs_vector_count(entity_vector);

    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    count --;
    
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_count(table->type);
    int32_t i;

    if (index != count) {
        /* Move last entity in array to index */
        ecs_entity_t *entities = ecs_vector_first(entity_vector);
        ecs_entity_t to_move = entities[count];
        entities[index] = to_move;

        for (i = 0; i < column_count; i ++) {
            if (columns[i].size) {
                _ecs_vector_remove_index(
                    columns[i].data, columns[i].size, index);
            }
        }

        /* Last entity in table is now moved to index of removed entity */
        ecs_record_t record;
        record.table = table;
        record.row = index + 1;
        ecs_eis_set(stage, to_move, &record);

        /* Decrease size of entity column */
        ecs_vector_remove_last(entity_vector);

    /* This is the last entity in the table, just decrease column counts */
    } else {
        ecs_vector_remove_last(entity_vector);

        for (i = 0; i < column_count; i ++) {
            if (columns[i].size) {
                ecs_vector_remove_last(columns[i].data);
            }
        }
    }
    
    if (!world->in_progress && !count) {
        activate_table(world, table, 0, false);
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
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_count(table->type);

    /* Fist add entity to column with entity ids */
    ecs_entity_t *e = ecs_vector_addn(&data->entities, ecs_entity_t, count);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < count; i ++) {
        e[i] = first_entity + i;
    }

    bool reallocd = false;

    /* Add elements to each column array */
    for (i = 0; i < column_count; i ++) {
        size_t column_size = columns[i].size;
        if (!column_size) {
            continue;
        }

        void *old_vector = columns[i].data;

        _ecs_vector_addn(&columns[i].data, column_size, count);

        if (old_vector != columns[i].data) {
            reallocd = true;
        }
    }

    int32_t row_count = ecs_vector_count(data->entities);
    if (!world->in_progress && row_count == count) {
        activate_table(world, table, 0, true);
    }

    ecs_data_t *main_data = ecs_table_get_data(world, table);
    if (reallocd && main_data == data) {
        world->should_resolve = true;
    }

    /* Return index of first added entity */
    return row_count - count;
}

int16_t ecs_table_set_size(
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_count(table->type);

    int32_t size = ecs_vector_set_size(&data->entities, ecs_entity_t, count);
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    int32_t i;
    for (i = 0; i < column_count; i ++) {
        int32_t column_size = columns[i].size;

        if (column_size) {
            int32_t size = _ecs_vector_set_size(&columns[i].data, column_size, count);
            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
            (void)size;
        } else {
            ecs_assert(columns[i].data == NULL, ECS_INTERNAL_ERROR, NULL);
        }
    }

    return 0;
}

uint64_t ecs_table_count(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *data = ecs_vector_first(table->stage_data);
    if (!data) {
        return 0;
    }

    return ecs_vector_count(data->entities);
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
    ecs_assert(new_table != old_table, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t new_type = new_table ? new_table->type : NULL;
    ecs_type_t old_type = old_table->type;
    ecs_assert(new_type != old_type, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *init_data = new_table ? ecs_table_get_data(world, new_table) : NULL;
    ecs_assert(!init_data || init_data->columns != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *old_data = ecs_table_get_data(world, old_table);
    ecs_assert(!old_data || old_data->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *new_columns = init_data ? init_data->columns : NULL;
    ecs_column_t *old_columns = old_data->columns;
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
        ecs_table_delete_all(world, old_table);
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

    merge_vector(&init_data->entities, old_data->entities, sizeof(ecs_entity_t));
    old_data->entities = NULL;
}
