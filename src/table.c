#include "flecs_private.h"

static
ecs_data_t* init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = table->type; 
    int32_t i, count = table->column_count;
    
    result->entities = NULL;
    result->record_ptrs = NULL;
    result->marked_dirty = false;

    /* Root tables don't have columns */
    if (!count) {
        result->columns = NULL;
        return result;
    }

    result->columns = ecs_os_calloc(sizeof(ecs_column_t), count);
    
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        const EcsComponent *component = ecs_get_ptr(
                world, entities[i], EcsComponent);

        /* Is the column a component? */
        if (component) {
            /* Is the component associated wit a (non-empty) type? */
            if (component->size) {
                /* This is a regular component column */
                result->columns[i].size = component->size;
                result->columns[i].alignment = component->alignment;
            } else {
                /* This is a tag */
            }
        } else {
            /* This is an entity that was added to the type */
        }
    }

    table->column_count = count;

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
        int32_t c, column_count = table->column_count;
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
    ecs_data_t *stage_data = ecs_vector_first(table->stage_data, ecs_data_t);
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
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        ecs_entities_t components = ecs_type_to_entities(table->type);

        /* Run deinit actions (dtors) for components. Don't run triggers */
        ecs_run_deinit_actions(
            world, table, data, 0, count, components, false);
    }
}

static
int compare_matched_query(
    const void *ptr1,
    const void *ptr2)
{
    const ecs_matched_query_t *m1 = ptr1;
    const ecs_matched_query_t *m2 = ptr2;
    ecs_query_t *q1 = m1->query;
    ecs_query_t *q2 = m2->query;
    ecs_assert(q1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(q2 != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t s1 = q1->system;
    ecs_entity_t s2 = q2->system;
    ecs_assert(s1 != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(s2 != 0, ECS_INTERNAL_ERROR, NULL);

    return s1 - s2;
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
static
void register_monitor(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* First check if system is already registered as monitor. It is possible
     * the query just wants to update the matched_table_index (for example, if
     * query tables got reordered) */
    ecs_vector_each(table->monitors, ecs_matched_query_t, m, {
        if (m->query == query) {
            m->matched_table_index = matched_table_index;
            return;
        }
    });

    /* Monitor hasn't been registered with table, register it now */
    ecs_matched_query_t *m = ecs_vector_add(&table->monitors, ecs_matched_query_t);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    m->query = query;
    m->matched_table_index = matched_table_index;

    /* Sort the vector so we can quickly compare monitors between tables */
    qsort(
        ecs_vector_first(table->monitors, ecs_matched_query_t), 
        ecs_vector_count(table->monitors), 
        sizeof(ecs_matched_query_t), 
        compare_matched_query);

#ifndef NDEBUG
    char *str = ecs_type_str(world, table->type);
    ecs_trace_1("monitor #[green]%s#[reset] registered with table #[red]%s",
        ecs_get_name(world, query->system), str);
    ecs_os_free(str);
#endif
}

static
void register_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    if (table->column_count) {
        if (!table->on_set) {
            table->on_set = ecs_os_calloc(sizeof(ecs_vector_t),
                table->column_count);
        }

        /* Add system to each matched column. This makes it easy to get the list of
        * systems when setting a single component. */
        ecs_vector_each(query->sig.columns, ecs_sig_column_t, column, {
            ecs_sig_oper_kind_t oper_kind = column->oper_kind;

            if (oper_kind == EcsOperAnd || oper_kind == EcsOperOptional) {
                ecs_entity_t comp = column->is.component;
                int32_t index = ecs_type_index_of(table->type, comp);
                if (index == -1) {
                    continue;
                }

                if (index >= table->column_count) {
                    continue;
                }
                
                ecs_vector_t *set_c = table->on_set[index];
                ecs_matched_query_t * m = ecs_vector_add(&set_c, ecs_matched_query_t);
                
                m->query = query;
                m->matched_table_index = matched_table_index;

                table->on_set[index] = set_c;
            }
        });
    }

    /* Add the system to a list that contains all OnSet systems matched with
     * this table. This makes it easy to get the list of systems that need to be
     * executed when all components are set, like when new_w_data is used */
    ecs_matched_query_t * m = ecs_vector_add(&table->on_set_all, 
        ecs_matched_query_t);
    m->query = query;
    m->matched_table_index = matched_table_index;

    qsort(
        ecs_vector_first(table->on_set_all, ecs_matched_query_t), 
        ecs_vector_count(table->on_set_all), 
        sizeof(ecs_matched_query_t), 
        compare_matched_query);
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
        #ifndef NDEBUG
            char *expr = ecs_type_str(world, table->type);
            ecs_trace_1("table #[green][%s]#[reset] %s for single query", expr, 
                activate ? "activated" : "deactivatd");
            ecs_os_free(expr);
        #endif           
    } else {
        ecs_vector_t *queries = table->queries;
        
        if (queries) {
            ecs_query_t **buffer = ecs_vector_first(queries, ecs_query_t*);
            int32_t i, count = ecs_vector_count(queries);
            for (i = 0; i < count; i ++) {
                ecs_query_activate_table(world, buffer[i], table, activate);
            }
        }

        #ifndef NDEBUG
            if (ecs_vector_count(queries)) {
                char *expr = ecs_type_str(world, table->type);
                ecs_trace_1("table #[green][%s]#[reset] %s for %d queries", expr, 
                    activate ? "activated" : "deactivated",
                    ecs_vector_count(queries));
                ecs_os_free(expr);
            }
        #endif         
    }     
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
void ecs_table_register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Register system with the table */
    if (!(query->flags & EcsQueryNoActivation)) {
        ecs_query_t **q = ecs_vector_add(&table->queries, ecs_query_t*);
        if (q) *q = query;

        ecs_data_t *data = ecs_table_get_data(world, table);
        if (data && ecs_vector_count(data->entities)) {
            ecs_table_activate(world, table, query, true);
        }
    }

    /* Register the query as a monitor */
    if (query->flags & EcsQueryMonitor) {
        register_monitor(world, table, query, matched_table_index);
    }

    /* Register the query as an on_set system */
    if (query->flags & EcsQueryOnSet) {
        register_on_set(world, table, query, matched_table_index);
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
     * for tables that don't contain data but are used for graph traversal. */
    if (!stage_data && !create) {
        return NULL;
    }

    ecs_data_t *data_array = ecs_vector_first(stage_data, ecs_data_t);
    int32_t count = ecs_vector_count(stage_data);
    int32_t stage_count = world->stage_count;
    int32_t id = stage->id;

    ecs_assert(id < stage_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!create || id || !world->in_progress, ECS_INTERNAL_ERROR, NULL);

    /* Make sure the array is large enough for the number of active stages. This
     * guarantees that any pointers returned by this function are stable, unless
     * the number of stages changes (can happen when the number of worker 
     * threads changes) */
    if (count != stage_count) {
        if (stage_count > count) {
            /* Grow array, initialize table data to 0 */
            ecs_vector_set_count(&table->stage_data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->stage_data, ecs_data_t);
            memset(&data_array[count], 
                0, sizeof(ecs_data_t) * (stage_count - count));
        } else {
            /* If the number of stages is reduced, deinit redudant stages */
            int i;
            for (i = stage_count; i < count; i ++) {
                deinit_data(table, &data_array[i]);
            }

            ecs_vector_set_count(&table->stage_data, ecs_data_t, stage_count);
            data_array = ecs_vector_first(table->stage_data, ecs_data_t);           
        }

        int i;
        for (i = 0; i < stage_count; i ++) {
            ecs_data_t *data_array = ecs_vector_first(table->stage_data, ecs_data_t);
            ecs_stage_t *stage_array = ecs_vector_first(world->worker_stages, ecs_stage_t);

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
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        run_on_remove_handlers(world, table, data);

        ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
        int32_t i, count = ecs_vector_count(data->entities);
        for(i = 0; i < count; i ++) {
            ecs_eis_delete(&world->stage, entities[i]);
        }
    }

    ecs_table_clear_silent(world, table);
}

/* Free table resources. Do not invoke handlers and do not activate/deactivate
 * table with systems. This function is used when the world is freed. */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_data_t *data = ecs_table_get_data(world, table);
    if (data) {
        run_on_remove_handlers(world, table, data);
    }

    deinit_all_data(table);
    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    ecs_vector_free(table->queries);
    ecs_vector_free((ecs_vector_t*)table->type);
    ecs_os_free(table->dirty_state);
    ecs_vector_free(table->monitors);
}

/* Reset a table to its initial state. This is used to reset the root table of a
 * stage after a merge has occurred, so that it no longer refers to tables that
 * were created in the stage. */
void ecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;

    if (table->lo_edges) {
        memset(table->lo_edges, 0, sizeof(ecs_edge_t) * ECS_HI_COMPONENT_ID);
        ecs_map_clear(table->hi_edges);
    }
}

/* Replace columns. Activate / deactivate table with systems if necessary. */
void ecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = ecs_vector_first(table->stage_data, ecs_data_t);
    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);

    if (table_data) {
        prev_count = ecs_vector_count(table_data->entities);
        run_on_remove_handlers(world, table, table_data);
        deinit_data(table, table_data);
    }

    table_data = ecs_table_get_or_create_data(world, &world->stage, table);
    if (data) {
        *table_data = *data;
    }

    ecs_entities_t components = ecs_type_to_entities(table->type);
    int32_t count = ecs_table_count(table);

    ecs_run_set_systems(world, &world->stage, &components, table, data, 0, 
        count, true);

    if (!prev_count && count) {
        ecs_table_activate(world, table, 0, true);
    } else if (prev_count && !count) {
        ecs_table_activate(world, table, 0, false);
    }
}

static
void mark_dirty(
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index)
{
    data->change_count ++;
    if (table->dirty_state) {
        table->dirty_state[index] ++;
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
    int32_t column_count = table->column_count;

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
            size_t size = columns[i].size;
            size_t alignment = columns[i].alignment;
            if (size) {
                ecs_vector_add_t(&columns[i].data, size, alignment);
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

    /* If the table is monitored indicate that there has been a change */
    mark_dirty(table, data, 0);

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

    uint32_t column_count = table->column_count;
    uint32_t i;

    if (index != count) {   
        /* Move last entity id to index */     
        ecs_entity_t *entities = ecs_vector_first(entity_column, ecs_entity_t);
        ecs_entity_t entity_to_move = entities[count];
        entities[index] = entity_to_move;
        ecs_vector_remove_last(entity_column);

        /* Move last record ptr to index */
        ecs_vector_t *record_column = data->record_ptrs;     
        ecs_record_t **records = ecs_vector_first(record_column, ecs_record_t*);
        ecs_record_t *record_to_move = records[count];

        records[index] = record_to_move;
        ecs_vector_remove_last(record_column);

        /* Move each component value in array to index */
        ecs_column_t *components = data->columns;
        for (i = 0; i < column_count; i ++) {
            ecs_column_t *component_column = &components[i];
            size_t size = component_column->size;
            size_t alignment = component_column->alignment;
            if (size) {
                ecs_vector_remove_index_t(
                    component_column->data, size, alignment, index);
            }
        }

        /* Update record of moved entity in entity index */
        if (!world->in_progress && record_to_move) {
            record_to_move->row = index + 1;
            ecs_assert(record_to_move->table == table, ECS_INTERNAL_ERROR, NULL);
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

    /* If the table is monitored indicate that there has been a change */
    mark_dirty(table, data, 0);    

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

    int32_t column_count = table->column_count;
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
        size_t size = columns[i].size;
        if (!size) {
            continue;
        }
        size_t alignment = columns[i].alignment;

        ecs_vector_addn_t(&columns[i].data, size, alignment, count);
    }

    /* If the table is monitored indicate that there has been a change */
    mark_dirty(table, data, 0);    

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
    int32_t i, column_count = table->column_count;

    if (!columns && column_count) {
        init_data(world, table, data);
        columns = data->columns;
    }

    ecs_vector_set_size(&data->entities, ecs_entity_t, count);
    ecs_vector_set_size(&data->record_ptrs, ecs_record_t*, count);

    for (i = 0; i < column_count; i ++) {
        int32_t size = columns[i].size;
        size_t alignment = columns[i].alignment;
        if (size) {
            ecs_vector_set_size_t(&columns[i].data, size, alignment, count);
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
    int32_t i, column_count = table->column_count;

    if (!columns && column_count) {
        init_data(world, table, data);
        columns = data->columns;
    }

    ecs_vector_set_count(&data->entities, ecs_entity_t, count);
    ecs_vector_set_count(&data->record_ptrs, ecs_record_t*, count);

    for (i = 0; i < column_count; i ++) {
        size_t size = columns[i].size;
        size_t alignment = columns[i].alignment;
        if (size) {
            ecs_vector_set_count_t(&columns[i].data, size, alignment, count);
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
    ecs_data_t *data = ecs_vector_first(table->stage_data, ecs_data_t);
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
    int32_t row_2)
{    
    (void)world;

    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(row_1 >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_2 >= 0, ECS_INTERNAL_ERROR, NULL);
    
    if (row_1 == row_2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_record_t *record_ptr_1 = record_ptrs[row_1];
    ecs_record_t *record_ptr_2 = record_ptrs[row_2];
    
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
    record_ptrs[row_1] = record_ptr_2;
    record_ptrs[row_2] = record_ptr_1;

    /* Swap columns */
    int32_t i, column_count = table->column_count;
    
    for (i = 0; i < column_count; i ++) {
        size_t size = columns[i].size;
        size_t alignment = columns[i].alignment;
        void *data = ecs_vector_first_t(columns[i].data, size, alignment);

        if (size) {
            void *tmp = _ecs_os_alloca(size, 1);

            void *el_1 = ECS_OFFSET(data, size * row_1);
            void *el_2 = ECS_OFFSET(data, size * row_2);

            memcpy(tmp, el_1, size);
            memcpy(el_1, el_2, size);
            memcpy(el_2, tmp, size);
        }
    }

    /* If the table is monitored indicate that there has been a change */
    mark_dirty(table, data, 0);    
}

static
void merge_vector(
    ecs_vector_t **dst_out,
    ecs_vector_t *src,
    size_t size,
    size_t alignment)
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
        ecs_vector_set_count_t(&dst, size, alignment, dst_count + src_count);
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        memcpy(dst_ptr, src_ptr, size * src_count);

        ecs_vector_free(src);
        *dst_out = dst;
    }
}

static
void merge_table_data(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    int32_t old_count,
    int32_t new_count,
    ecs_data_t *new_data,
    ecs_data_t *old_data)
{
    uint16_t i_new, new_component_count = new_table->column_count;
    uint16_t i_old = 0, old_component_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_table->type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_table->type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    if (!new_columns && !new_data->entities) {
        init_data(world, new_table, new_data);
        new_columns = new_data->columns;
    }

    if (!old_count) {
        return;
    }

    for (i_new = 0; i_new < new_component_count; ) {
        if (i_old == old_component_count) {
            break;
        }

        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];
        size_t size = new_columns[i_new].size;
        size_t alignment = new_columns[i_new].alignment;

        if ((new_component & ECS_TYPE_FLAG_MASK) || 
            (old_component & ECS_TYPE_FLAG_MASK)) 
        {
            break;
        }

        if (new_component == old_component) {
            merge_vector(
                &new_columns[i_new].data, old_columns[i_old].data, size, 
                alignment);

            old_columns[i_old].data = NULL;

            /* Mark component column as dirty */
            mark_dirty(new_table, new_data, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            /* New column does not occur in old table, make sure vector is large
             * enough. */
            if (size) {
                ecs_vector_set_count_t(&new_columns[i_new].data, size, alignment,
                    old_count + new_count);
            }
            i_new ++;
        } else if (new_component > old_component) {
            /* Old column does not occur in new table, remove */
            ecs_vector_free(old_columns[i_old].data);
            old_columns[i_old].data = NULL;
            i_old ++;
        }
    }

    /* Initialize remaining columns */
    for (; i_new < new_component_count; i_new ++) {
        size_t size = new_columns[i_new].size;
        size_t alignment = new_columns[i_new].alignment;

        if (size) {
            ecs_vector_set_count_t(&new_columns[i_new].data, size, alignment,
            old_count + new_count);
        }
    }
}

void ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table)
{
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_table != old_table, ECS_INTERNAL_ERROR, NULL);

    /* If there is nothing to merge to, just clear the old table */
    if (!new_table) {
        ecs_table_clear(world, old_table);
        return;
    }

    /* If there is no data to merge, drop out */
    ecs_data_t *old_data = ecs_table_get_data(world, old_table);
    if (!old_data) {
        return;
    }

    ecs_data_t *new_data = ecs_table_get_or_create_data(
        world, &world->stage, new_table);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *old_entities = ecs_vector_first(old_data->entities, ecs_entity_t);

    int32_t old_count = ecs_vector_count(old_data->entities);
    int32_t new_count = ecs_vector_count(new_data->entities);

    ecs_record_t **old_records = ecs_vector_first(old_data->record_ptrs, ecs_record_t*);

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < old_count; i ++) {
        ecs_record_t *record = old_records[i];
        if (!record) {
            record = ecs_eis_get(&world->stage, old_entities[i]);
        }

        bool is_monitored = record->row < 0;
        record->row = ecs_row_to_record(new_count + i, is_monitored);
        record->table = new_table;
    }

    /* Merge table columns */
    merge_table_data(world, new_table, old_table, old_count, new_count, 
        new_data, old_data);

    /* Merge entities */
    merge_vector(&new_data->entities, old_data->entities, sizeof(ecs_entity_t), 
        ECS_ALIGNOF(ecs_entity_t));
    old_data->entities = NULL;

    ecs_assert(ecs_vector_count(new_data->entities) == old_count + new_count, 
        ECS_INTERNAL_ERROR, NULL);

    /* Merge entity index record pointers */
    merge_vector(&new_data->record_ptrs, old_data->record_ptrs, 
        sizeof(ecs_record_t*), ECS_ALIGNOF(ecs_record_t*));
    old_data->record_ptrs = NULL;    

    /* Mark entity column as dirty */
    mark_dirty(new_table, new_data, 0);  
}

void ecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index,
    bool is_copy)
{
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    uint32_t i_new = 0, new_column_count = new_table->column_count;
    uint32_t i_old = 0, old_column_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if (new_component == old_component) {
            ecs_column_t *new_column = &new_columns[i_new];
            ecs_column_t *old_column = &old_columns[i_old];
            size_t size = new_column->size;
            size_t alignment = new_column->alignment;

            if (size) {
                void *dst = ecs_vector_get_t(new_column->data, size, alignment, new_index);
                void *src = ecs_vector_get_t(old_column->data, size, alignment, old_index);
                    
                ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

                if (is_copy) {
                    ecs_c_info_t *cdata = ecs_get_c_info(
                        world, new_component);

                    ecs_copy_t copy = cdata->lifecycle.copy;
                    if (copy) {
                        void *ctx = cdata->lifecycle.ctx;
                        copy(world, new_component, &dst_entity, &src_entity, dst, src, 
                            size, 1, ctx);
                    } else {
                        memcpy(dst, src, size);
                    }
                } else {
                    memcpy(dst, src, size);
                }
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
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

int32_t* ecs_table_get_dirty_state(
    ecs_table_t *table)
{
    if (!table->dirty_state) {
        int32_t column_count = ecs_vector_size(table->type);
        table->dirty_state = ecs_os_calloc(column_count + 1, sizeof(int32_t));
        ecs_assert(table->dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    }
    return table->dirty_state;
}

int32_t* ecs_table_get_monitor(
    ecs_table_t *table)
{
    int32_t *dirty_state = ecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_size(table->type);
    return ecs_os_memdup(dirty_state, (column_count + 1) * sizeof(int32_t));
}
