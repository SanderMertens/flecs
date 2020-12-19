#include "private_api.h"

ecs_data_t* ecs_init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = table->type; 
    int32_t i, 
    count = table->column_count, 
    sw_count = table->sw_column_count,
    bs_count = table->bs_column_count;

    /* Root tables don't have columns */
    if (!count && !sw_count) {
        result->columns = NULL;
        return result;
    }

    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    if (count && !sw_count) {
        result->columns = ecs_os_calloc(ECS_SIZEOF(ecs_column_t) * count);    
    } else if (count || sw_count) {
        /* If a table has switch columns, store vector with the case values
            * as a regular column, so it's easier to access for systems. To
            * enable this, we need to allocate more space. */
        int32_t type_count = ecs_vector_count(type);
        result->columns = ecs_os_calloc(ECS_SIZEOF(ecs_column_t) * type_count);
    }

    if (count) {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = entities[i];

            /* Is the column a component? */
            const EcsComponent *component = ecs_component_from_id(world, e);
            if (component) {
                /* Is the component associated wit a (non-empty) type? */
                if (component->size) {
                    /* This is a regular component column */
                    result->columns[i].size = ecs_to_i16(component->size);
                    result->columns[i].alignment = ecs_to_i16(component->alignment);
                } else {
                    /* This is a tag */
                }
            } else {
                /* This is an entity that was added to the type */
            }
        }
    }

    if (sw_count) {
        int32_t sw_offset = table->sw_column_offset;
        result->sw_columns = ecs_os_calloc(ECS_SIZEOF(ecs_sw_column_t) * sw_count);

        for (i = 0; i < sw_count; i ++) {
            ecs_entity_t e = entities[i + sw_offset];
            ecs_assert(ECS_HAS_ROLE(e, SWITCH), ECS_INTERNAL_ERROR, NULL);
            e = e & ECS_COMPONENT_MASK;
            const EcsType *type_ptr = ecs_get(world, e, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_type_t sw_type = type_ptr->normalized;

            ecs_entity_t *sw_array = ecs_vector_first(sw_type, ecs_entity_t);
            int32_t sw_array_count = ecs_vector_count(sw_type);

            ecs_switch_t *sw = ecs_switch_new(
                sw_array[0], 
                sw_array[sw_array_count - 1], 
                0);
            result->sw_columns[i].data = sw;
            result->sw_columns[i].type = sw_type;

            int32_t column_id = i + table->sw_column_offset;
            result->columns[column_id].data = ecs_switch_values(sw);
            result->columns[column_id].size = sizeof(ecs_entity_t);
            result->columns[column_id].alignment = ECS_ALIGNOF(ecs_entity_t);
        }
    }

    if (bs_count) {
        result->bs_columns = ecs_os_calloc(ECS_SIZEOF(ecs_bs_column_t) * bs_count);
        for (i = 0; i < bs_count; i ++) {
            ecs_bitset_init(&result->bs_columns[i].data);
        }
    }

    return result;
}

static
ecs_flags32_t get_component_action_flags(
    ecs_c_info_t *c_info) 
{
    ecs_flags32_t flags = 0;

    if (c_info->lifecycle.ctor) {
        flags |= EcsTableHasCtors;
    }
    if (c_info->lifecycle.dtor) {
        flags |= EcsTableHasDtors;
    }
    if (c_info->lifecycle.copy) {
        flags |= EcsTableHasCopy;
    }
    if (c_info->lifecycle.move) {
        flags |= EcsTableHasMove;
    }
    if (c_info->on_add) {
        flags |= EcsTableHasOnAdd;
    }
    if (c_info->on_remove) {
        flags |= EcsTableHasOnRemove;
    }    

    return flags;  
}

/* Check if table has instance of component, including traits */
static
bool has_component(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component)
{
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        if (component == ecs_get_typeid(world, entities[i])) {
            return true;
        }
    }
    
    return false;
}

static
void notify_component_info(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_type_t table_type = table->type;
    if (!component || has_component(world, table_type, component)){
        int32_t column_count = ecs_vector_count(table_type);
        ecs_assert(!component || column_count != 0, ECS_INTERNAL_ERROR, NULL);

        if (!column_count) {
            return;
        }
        
        if (!table->c_info) {
            table->c_info = ecs_os_calloc(
                ECS_SIZEOF(ecs_c_info_t*) * column_count);
        }

        /* Reset lifecycle flags before recomputing */
        table->flags &= ~EcsTableHasLifecycle;

        /* Recompute lifecycle flags */
        ecs_entity_t *array = ecs_vector_first(table_type, ecs_entity_t);
        int32_t i;
        for (i = 0; i < column_count; i ++) {
            ecs_entity_t c = ecs_get_typeid(world, array[i]);
            if (!c) {
                continue;
            }
            
            ecs_c_info_t *c_info = ecs_get_c_info(world, c);
            if (c_info) {
                ecs_flags32_t flags = get_component_action_flags(c_info);
                table->flags |= flags;
            }

            /* Store pointer to c_info for fast access */
            table->c_info[i] = c_info;
        }        
    }
}

static
void run_un_set_handlers(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        ecs_run_monitors(world, table, table->un_set_all, 0, count, NULL);
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

    return (s1 > s2) - (s1 < s2);
}

static
void add_monitor(
    ecs_vector_t **array,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Add the system to a list that contains all OnSet systems matched with
     * this table. This makes it easy to get the list of systems that need to be
     * executed when all components are set, like when new_w_data is used */
    ecs_matched_query_t *m = ecs_vector_add(array, ecs_matched_query_t);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    m->query = query;
    m->matched_table_index = matched_table_index;

    /* Sort the system list so that it is easy to get the difference OnSet
     * OnSet systems between two tables. */
    qsort(
        ecs_vector_first(*array, ecs_matched_query_t), 
        ecs_to_size_t(ecs_vector_count(*array)),
        ECS_SIZEOF(ecs_matched_query_t), 
        compare_matched_query);
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

    add_monitor(&table->monitors, query, matched_table_index);

#ifndef NDEBUG
    char *str = ecs_type_str(world, table->type);
    ecs_trace_2("monitor #[green]%s#[reset] registered with table #[red]%s",
        ecs_get_name(world, query->system), str);
    ecs_os_free(str);
#endif
}

static
bool is_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t comp)
{
    if (!(table->flags & EcsTableHasBase)) {
        return false;
    }

    ecs_type_t type = table->type;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = entities[i];
        if (ECS_HAS_ROLE(e, INSTANCEOF)) {
            if (ecs_has_entity(world, e & ECS_COMPONENT_MASK, comp)) {
                return true;
            }
        } else {
            /* ECS_INSTANCEOF will always appear at the end of a type */
            return false;
        }
    }

    return false;
}

static
void register_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;

    if (table->column_count) {
        if (!table->on_set) {
            table->on_set = 
                ecs_os_calloc(ECS_SIZEOF(ecs_vector_t) * table->column_count);
        }

        /* Get the matched table which holds the list of actual components */
        ecs_matched_table_t *matched_table = ecs_vector_get(
            query->tables, ecs_matched_table_t, matched_table_index);

        /* Keep track of whether query matches overrides. When a component is
         * removed, diffing these arrays between the source and detination
         * tables gives the list of OnSet systems to run, after exposing the
         * component that was overridden. */
        bool match_override = false;

        /* Add system to each matched column. This makes it easy to get the list 
         * of systems when setting a single component. */
        ecs_sig_column_t *columns = ecs_vector_first(query->sig.columns,
            ecs_sig_column_t);
        int32_t i, count = ecs_vector_count(query->sig.columns);
        for (i = 0; i < count; i ++) {
            ecs_sig_column_t *column = &columns[i];
            ecs_sig_oper_kind_t oper_kind = column->oper_kind;
            ecs_sig_from_kind_t from_kind = column->from_kind;

            if ((from_kind != EcsFromAny && from_kind != EcsFromOwned) ||
                (oper_kind != EcsOperAnd && oper_kind != EcsOperOptional)) 
            {
                continue;
            }

            ecs_entity_t comp = matched_table->data.components[i];
            int32_t index = ecs_type_index_of(table->type, comp);
            if (index == -1) {
                continue;
            }

            if (index >= table->column_count) {
                continue;
            }
            
            ecs_vector_t *set_c = table->on_set[index];
            ecs_matched_query_t *m = ecs_vector_add(&set_c, ecs_matched_query_t);
            m->query = query;
            m->matched_table_index = matched_table_index;
            table->on_set[index] = set_c;
            
            match_override |= is_override(world, table, comp);
        } 

        if (match_override) {
            add_monitor(&table->on_set_override, query, matched_table_index);
        }
    }

    add_monitor(&table->on_set_all, query, matched_table_index);   
}

static
void register_un_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;
    add_monitor(&table->un_set_all, query, matched_table_index);
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
        ecs_query_notify(world, query, &(ecs_query_event_t) {
            .kind = activate ? EcsQueryTableNonEmpty : EcsQueryTableEmpty,
            .table = table
        });
    } else {
        ecs_vector_t *queries = table->queries;
        ecs_query_t **buffer = ecs_vector_first(queries, ecs_query_t*);
        int32_t i, count = ecs_vector_count(queries);

        for (i = 0; i < count; i ++) {
            ecs_query_notify(world, buffer[i], &(ecs_query_event_t) {
                .kind = activate ? EcsQueryTableNonEmpty : EcsQueryTableEmpty,
                .table = table
            });                
        }
    }     
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
static
void register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Register system with the table */
    if (!(query->flags & EcsQueryNoActivation)) {
#ifndef NDEBUG
        /* Sanity check if query has already been added */
        int32_t i, count = ecs_vector_count(table->queries);
        for (i = 0; i < count; i ++) {
            ecs_query_t **q = ecs_vector_get(table->queries, ecs_query_t*, i);
            ecs_assert(*q != query, ECS_INTERNAL_ERROR, NULL);
        }
#endif

        ecs_query_t **q = ecs_vector_add(&table->queries, ecs_query_t*);
        if (q) *q = query;

        ecs_data_t *data = ecs_table_get_data(table);
        if (data && ecs_vector_count(data->entities)) {
            ecs_table_activate(world, table, query, true);
        }
    }

    /* Register the query as a monitor */
    if (query->flags & EcsQueryMonitor) {
        table->flags |= EcsTableHasMonitors;
        register_monitor(world, table, query, matched_table_index);
    }

    /* Register the query as an on_set system */
    if (query->flags & EcsQueryOnSet) {
        register_on_set(world, table, query, matched_table_index);
    }

    /* Register the query as an un_set system */
    if (query->flags & EcsQueryUnSet) {
        register_un_set(world, table, query, matched_table_index);
    }
}

/* This function is called when a query is unmatched with a table. This can
 * happen for queries that have shared components expressions in their signature
 * and those shared components changed (for example, a base removed a comp). */
static
void unregister_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query)
{
    (void)world;

    if (!(query->flags & EcsQueryNoActivation)) {
        int32_t i, count = ecs_vector_count(table->queries);
        for (i = 0; i < count; i ++) {
            ecs_query_t **q = ecs_vector_get(table->queries, ecs_query_t*, i);
            if (*q == query) {
                break;
            }
        }

        /* Query must have been registered with table */
        ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

        /* Remove query */
        ecs_vector_remove_index(table->queries, ecs_query_t*, i);        
    }
}

static
ecs_data_t* get_data_intern(
    ecs_table_t *table,
    bool create)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = table->data;
    if (data) {
        return data;
    }

    if (!data && !create) {
        return NULL;
    }

    return table->data = ecs_os_calloc(ECS_SIZEOF(ecs_data_t));
}

ecs_data_t* ecs_table_get_data(
    ecs_table_t *table)
{
    return get_data_intern(table, false);
}

ecs_data_t* ecs_table_get_or_create_data(
    ecs_table_t *table)
{
    return get_data_intern(table, true);
}

static
void ctor_component(
    ecs_world_t * world,
    ecs_c_info_t * cdata,
    ecs_column_t * column,
    ecs_entity_t * entities,
    int32_t row,
    int32_t count)
{
    /* A new component is constructed */
    ecs_xtor_t ctor;
    if (cdata && (ctor = cdata->lifecycle.ctor)) {
        void *ctx = cdata->lifecycle.ctx;
        int16_t size = column->size;
        int16_t alignment = column->alignment;

        void *ptr = ecs_vector_get_t(column->data, size, alignment, row);

        ctor(world, cdata->component, entities, ptr, 
            ecs_to_size_t(size), count, ctx);
    }
}

static
void dtor_component(
    ecs_world_t * world,
    ecs_c_info_t * cdata,
    ecs_column_t * column,
    ecs_entity_t * entities,
    int32_t row,
    int32_t count)
{
    /* An old component is destructed */
    ecs_xtor_t dtor;
    if (cdata && (dtor = cdata->lifecycle.dtor)) {
        void *ctx = cdata->lifecycle.ctx;
        int16_t size = column->size;
        int16_t alignment = column->alignment;    

        void *ptr = ecs_vector_get_t(column->data, size, alignment, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        dtor(world, cdata->component, entities, ptr,
            ecs_to_size_t(size), count, ctx);
    }
}

static
void dtor_all_components(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count)
{
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t column_count = table->column_count;
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &data->columns[i];
        dtor_component(
            world, table->c_info[i], column, entities, row, 
            count);
    }
}

static
void run_remove_actions(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    bool dtor_only)
{
    if (count) {
        if (!dtor_only) {
            ecs_run_monitors(world, table, NULL, row, count, table->un_set_all);
        }

        dtor_all_components(world, table, data, row, count);
    }
}

void ecs_table_clear_data(
    ecs_table_t * table,
    ecs_data_t * data)
{
    if (!data) {
        return;
    }
    
    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = table->column_count;
        for (c = 0; c < column_count; c ++) {
            ecs_vector_free(columns[c].data);
        }
        ecs_os_free(columns);
        data->columns = NULL;
    }

    ecs_sw_column_t *sw_columns = data->sw_columns;
    if (sw_columns) {
        int32_t c, column_count = table->sw_column_count;
        for (c = 0; c < column_count; c ++) {
            ecs_switch_free(sw_columns[c].data);
        }
        ecs_os_free(sw_columns);
        data->sw_columns = NULL;
    }

    ecs_bs_column_t *bs_columns = data->bs_columns;
    if (bs_columns) {
        int32_t c, column_count = table->bs_column_count;
        for (c = 0; c < column_count; c ++) {
            ecs_bitset_deinit(&bs_columns[c].data);
        }
        ecs_os_free(bs_columns);
        data->bs_columns = NULL;
    }    

    ecs_vector_free(data->entities);
    ecs_vector_free(data->record_ptrs);

    data->entities = NULL;
    data->record_ptrs = NULL;
}

/* Clear columns. Deactivate table in systems if necessary, but do not invoke
 * OnRemove handlers. This is typically used when restoring a table to a
 * previous state. */
void ecs_table_clear_silent(
    ecs_world_t * world,
    ecs_table_t * table)
{
    ecs_data_t *data = ecs_table_get_data(table);
    if (!data) {
        return;
    }

    int32_t count = ecs_vector_count(data->entities);
    
    ecs_table_clear_data(table, table->data);

    if (count) {
        ecs_table_activate(world, table, 0, false);
    }
}

/* Delete all entities in table, invoke OnRemove handlers. This function is used
 * when an application invokes delete_w_filter. Use ecs_table_clear_silent, as 
 * the table may have to be deactivated with systems. */
void ecs_table_clear(
    ecs_world_t * world,
    ecs_table_t * table)
{
    ecs_data_t *data = ecs_table_get_data(table);
    if (data) {
        run_remove_actions(
            world, table, data, 0, ecs_table_data_count(data), false);

        ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
        int32_t i, count = ecs_vector_count(data->entities);
        for(i = 0; i < count; i ++) {
            ecs_eis_delete(world, entities[i]);
        }
    }

    ecs_table_clear_silent(world, table);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all UnSet handlers, if any */
void ecs_table_unset(
    ecs_world_t * world,
    ecs_table_t * table)
{
    (void)world;
    ecs_data_t *data = ecs_table_get_data(table);
    if (data) {
        run_un_set_handlers(world, table, data);
    }   
}

/* Free table resources. Do not invoke handlers and do not activate/deactivate
 * table with systems. This function is used when the world is freed. */
void ecs_table_free(
    ecs_world_t * world,
    ecs_table_t * table)
{
    (void)world;
    ecs_data_t *data = ecs_table_get_data(table);
    if (data) {
        run_remove_actions(
            world, table, data, 0, ecs_table_data_count(data), false);
    }

    ecs_table_clear_data(table, table->data);
    ecs_table_clear_edges(table);
    
    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    ecs_vector_free(table->queries);
    ecs_vector_free((ecs_vector_t*)table->type);
    ecs_os_free(table->dirty_state);
    ecs_vector_free(table->monitors);
    ecs_vector_free(table->on_set_all);
    ecs_vector_free(table->on_set_override);
    ecs_vector_free(table->un_set_all);

    if (table->c_info) {
        ecs_os_free(table->c_info);
    }
    
    if (table->on_set) {
        int32_t i;
        for (i = 0; i < table->column_count; i ++) {
            ecs_vector_free(table->on_set[i]);
        }
        ecs_os_free(table->on_set);
    }

    table->id = 0;

    ecs_os_free(table->data);
}

/* Reset a table to its initial state. */
void ecs_table_reset(
    ecs_world_t * world,
    ecs_table_t * table)
{
    (void)world;

    if (table->lo_edges) {
        memset(table->lo_edges, 0, ECS_SIZEOF(ecs_edge_t) * ECS_HI_COMPONENT_ID);
        ecs_map_clear(table->hi_edges);
    }
}

static
void mark_table_dirty(
    ecs_table_t *table,
    int32_t index)
{
    if (table->dirty_state) {
        table->dirty_state[index] ++;
    }
}

void ecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (table->dirty_state) {
        int32_t index = ecs_type_index_of(table->type, component);
        ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
        table->dirty_state[index] ++;
    }
}

static
void move_switch_columns(
    ecs_table_t * new_table, 
    ecs_data_t * new_data, 
    int32_t new_index,
    ecs_table_t * old_table, 
    ecs_data_t * old_data, 
    int32_t old_index,
    int32_t count)
{
    int32_t i_old = 0, old_column_count = old_table->sw_column_count;
    int32_t i_new = 0, new_column_count = new_table->sw_column_count;

    if (!old_column_count || !new_column_count) {
        return;
    }

    ecs_sw_column_t *old_columns = old_data->sw_columns;
    ecs_sw_column_t *new_columns = new_data->sw_columns;

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t offset_new = new_table->sw_column_offset;
    int32_t offset_old = old_table->sw_column_offset;

    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new + offset_new];
        ecs_entity_t old_component = old_components[i_old + offset_old];

        if (new_component == old_component) {
            ecs_switch_t *old_switch = old_columns[i_old].data;
            ecs_switch_t *new_switch = new_columns[i_new].data;

            ecs_switch_ensure(new_switch, new_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = ecs_switch_get(old_switch, old_index + i);
                ecs_switch_set(new_switch, new_index + i, value);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

static
void move_bitset_columns(
    ecs_table_t * new_table, 
    ecs_data_t * new_data, 
    int32_t new_index,
    ecs_table_t * old_table, 
    ecs_data_t * old_data, 
    int32_t old_index,
    int32_t count)
{
    int32_t i_old = 0, old_column_count = old_table->bs_column_count;
    int32_t i_new = 0, new_column_count = new_table->bs_column_count;

    if (!old_column_count || !new_column_count) {
        return;
    }

    ecs_bs_column_t *old_columns = old_data->bs_columns;
    ecs_bs_column_t *new_columns = new_data->bs_columns;

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t offset_new = new_table->bs_column_offset;
    int32_t offset_old = old_table->bs_column_offset;

    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new + offset_new];
        ecs_entity_t old_component = old_components[i_old + offset_old];

        if (new_component == old_component) {
            ecs_bitset_t *old_bs = &old_columns[i_old].data;
            ecs_bitset_t *new_bs = &new_columns[i_new].data;

            ecs_bitset_ensure(new_bs, new_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = ecs_bitset_get(old_bs, old_index + i);
                ecs_bitset_set(new_bs, new_index + i, value);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

static
void ensure_data(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t * column_count_out,
    int32_t * sw_column_count_out,
    int32_t * bs_column_count_out,
    ecs_column_t ** columns_out,
    ecs_sw_column_t ** sw_columns_out,
    ecs_bs_column_t ** bs_columns_out)
{
    int32_t column_count = table->column_count;
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = NULL;
    ecs_sw_column_t *sw_columns = NULL;
    ecs_bs_column_t *bs_columns = NULL;

    /* It is possible that the table data was created without content. 
     * Now that data is going to be written to the table, initialize */ 
    if (column_count | sw_column_count | bs_column_count) {
        columns = data->columns;
        sw_columns = data->sw_columns;
        bs_columns = data->bs_columns;

        if (!columns && !sw_columns) {
            ecs_init_data(world, table, data);
            columns = data->columns;
            sw_columns = data->sw_columns;
            bs_columns = data->bs_columns;
        }

        *column_count_out = column_count;
        *sw_column_count_out = sw_column_count;
        *bs_column_count_out = bs_column_count;
        *columns_out = columns;
        *sw_columns_out = sw_columns;
        *bs_columns_out = bs_columns;
    }
}

static
void grow_column(
    ecs_world_t * world,
    ecs_entity_t * entities,
    ecs_column_t * column,
    ecs_c_info_t * c_info,
    int32_t to_add,
    int32_t new_size,
    bool construct)
{
    ecs_vector_t *vec = column->data;
    int16_t alignment = column->alignment;

    int32_t size = column->size;
    int32_t count = ecs_vector_count(vec);
    int32_t old_size = ecs_vector_size(vec);
    int32_t new_count = count + to_add;
    bool can_realloc = new_size != old_size;

    ecs_assert(new_size >= new_count, ECS_INTERNAL_ERROR, NULL);

    /* If the array could possibly realloc and the component has a move action 
     * defined, move old elements manually */
    ecs_move_t move;
    if (c_info && count && can_realloc && (move = c_info->lifecycle.move)) {
        ecs_xtor_t ctor = c_info->lifecycle.ctor;
        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create new vector */
        ecs_vector_t *new_vec = ecs_vector_new_t(size, alignment, new_size);
        ecs_vector_set_count_t(&new_vec, size, alignment, new_count);

        void *old_buffer = ecs_vector_first_t(
            vec, size, alignment);

        void *new_buffer = ecs_vector_first_t(
            new_vec, size, alignment);

        /* First construct elements (old and new) in new buffer */
        ctor(world, c_info->component, entities, new_buffer, 
            ecs_to_size_t(size), construct ? new_count : count, 
            c_info->lifecycle.ctx);
        
        /* Move old elements */
        move(world, c_info->component, entities, entities, 
            new_buffer, old_buffer, ecs_to_size_t(size), count, 
            c_info->lifecycle.ctx);

        /* Free old vector */
        ecs_vector_free(vec);
        column->data = new_vec;
    } else {
        /* If array won't realloc or has no move, simply add new elements */
        if (can_realloc) {
            ecs_vector_set_size_t(&vec, size, alignment, new_size);
        }

        void *elem = ecs_vector_addn_t(&vec, size, alignment, to_add);

        ecs_xtor_t ctor;
        if (construct && c_info && (ctor = c_info->lifecycle.ctor)) {
            /* If new elements need to be constructed and component has a
             * constructor, construct */
            ctor(world, c_info->component, &entities[count], elem, 
                ecs_to_size_t(size), to_add, c_info->lifecycle.ctx);
        }

        column->data = vec;
    }

    ecs_assert(ecs_vector_size(column->data) == new_size, 
        ECS_INTERNAL_ERROR, NULL);
}

static
int32_t grow_data(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t to_add,
    int32_t size,
    const ecs_entity_t *ids)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur_count = ecs_table_data_count(data);
    int32_t column_count = table->column_count;
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = NULL;
    ecs_sw_column_t *sw_columns = NULL;
    ecs_bs_column_t *bs_columns = NULL;
    ensure_data(world, table, data, &column_count, &sw_column_count, 
        &bs_column_count, &columns, &sw_columns, &bs_columns);    

    /* Add record to record ptr array */
    ecs_vector_set_size(&data->record_ptrs, ecs_record_t*, size);
    ecs_record_t **r = ecs_vector_addn(&data->record_ptrs, ecs_record_t*, to_add);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    if (ecs_vector_size(data->record_ptrs) > size) {
        size = ecs_vector_size(data->record_ptrs);
    }

    /* Add entity to column with entity ids */
    ecs_vector_set_size(&data->entities, ecs_entity_t, size);
    ecs_entity_t *e = ecs_vector_addn(&data->entities, ecs_entity_t, to_add);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_size(data->entities) == size, ECS_INTERNAL_ERROR, NULL);

    /* Initialize entity ids and record ptrs */
    int32_t i;
    if (ids) {
        for (i = 0; i < to_add; i ++) {
            e[i] = ids[i];
        }
    } else {
        ecs_os_memset(e, 0, ECS_SIZEOF(ecs_entity_t) * to_add);
    }
    ecs_os_memset(r, 0, ECS_SIZEOF(ecs_record_t*) * to_add);

    /* Add elements to each column array */
    ecs_c_info_t **c_info_array = table->c_info;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        if (!column->size) {
            continue;
        }

        ecs_c_info_t *c_info = NULL;
        if (c_info_array) {
            c_info = c_info_array[i];
        }

        grow_column(world, entities, column, c_info, to_add, size, true);
        ecs_assert(ecs_vector_size(columns[i].data) == size, 
            ECS_INTERNAL_ERROR, NULL);
    }

    /* Add elements to each switch column */
    for (i = 0; i < sw_column_count; i ++) {
        ecs_switch_t *sw = sw_columns[i].data;
        ecs_switch_addn(sw, to_add);
    }

    /* Add elements to each bitset column */
    for (i = 0; i < bs_column_count; i ++) {
        ecs_bitset_t *bs = &bs_columns[i].data;
        ecs_bitset_addn(bs, to_add);
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);

    if (!world->in_progress && !cur_count) {
        ecs_table_activate(world, table, 0, true);
    }

    table->alloc_count ++;

    /* Return index of first added entity */
    return cur_count;
}

static
void fast_append(
    ecs_column_t *columns,
    int32_t column_count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        int16_t size = column->size;
        if (size) {
            int16_t alignment = column->alignment;
            ecs_vector_add_t(&column->data, size, alignment);
        }
    }
}

int32_t ecs_table_append(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    ecs_entity_t entity,
    ecs_record_t * record,
    bool construct)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    int32_t count = ecs_vector_count(data->entities);
    int32_t size = ecs_vector_size(data->entities);

    int32_t column_count = table->column_count;
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = NULL;
    ecs_sw_column_t *sw_columns = NULL;
    ecs_bs_column_t *bs_columns = NULL;
    ensure_data(world, table, data, &column_count, &sw_column_count,
        &bs_column_count, &columns, &sw_columns, &bs_columns);

    /* Grow buffer with entity ids, set new element to new entity */
    ecs_entity_t *e = ecs_vector_add(&data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;    

    /* Keep track of alloc count. This allows references to check if cached
     * pointers need to be updated. */  
    table->alloc_count += (count == size);

    /* Add record ptr to array with record ptrs */
    ecs_record_t **r = ecs_vector_add(&data->record_ptrs, ecs_record_t*);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    *r = record;
 
    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);

    /* If this is the first entity in this table, signal queries so that the
     * table moves from an inactive table to an active table. */
    if (!world->in_progress && !count) {
        ecs_table_activate(world, table, 0, true);
    }

    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(table->flags & EcsTableIsComplex)) {
        fast_append(columns, column_count);
        return count;
    }

    ecs_c_info_t **c_info_array = table->c_info;
    ecs_entity_t *entities = ecs_vector_first(
        data->entities, ecs_entity_t);

    /* Reobtain size to ensure that the columns have the same size as the 
     * entities and record vectors. This keeps reasoning about when allocations
     * occur easier. */
    size = ecs_vector_size(data->entities);

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        if (!column->size) {
            continue;
        }

        ecs_c_info_t *c_info = NULL;
        if (c_info_array) {
            c_info = c_info_array[i];
        }

        grow_column(world, entities, column, c_info, 1, size, construct);
        
        ecs_assert(
            ecs_vector_size(columns[i].data) == ecs_vector_size(data->entities), 
            ECS_INTERNAL_ERROR, NULL); 
            
        ecs_assert(
            ecs_vector_count(columns[i].data) == ecs_vector_count(data->entities), 
            ECS_INTERNAL_ERROR, NULL);                        
    }

    /* Add element to each switch column */
    for (i = 0; i < sw_column_count; i ++) {
        ecs_switch_t *sw = sw_columns[i].data;
        ecs_switch_add(sw);
        columns[i + table->sw_column_offset].data = ecs_switch_values(sw);
    }

    /* Add element to each bitset column */
    for (i = 0; i < bs_column_count; i ++) {
        ecs_bitset_t *bs = &bs_columns[i].data;
        ecs_bitset_addn(bs, 1);
    }    

    return count;
}

static
void fast_delete_last(
    ecs_column_t *columns,
    int32_t column_count) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_vector_remove_last(column->data);
    }
}

static
void fast_delete(
    ecs_column_t *columns,
    int32_t column_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        int16_t size = column->size;
        if (size) {
            int16_t alignment = column->alignment;
            ecs_vector_remove_index_t(column->data, size, alignment, index);
        } 
    }
}

void ecs_table_delete(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t index,
    bool destruct)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *entity_column = data->entities;
    int32_t count = ecs_vector_count(entity_column);

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    ecs_c_info_t **c_info_array = table->c_info;
    int32_t column_count = table->column_count;
    int32_t i;

    ecs_entity_t *entities = ecs_vector_first(entity_column, ecs_entity_t);
    ecs_entity_t entity_to_move = entities[count];

    /* Move last entity id to index */
    entities[index] = entity_to_move;
    ecs_vector_remove_last(entity_column);

    /* Move last record ptr to index */
    ecs_vector_t *record_column = data->record_ptrs;     
    ecs_record_t **records = ecs_vector_first(record_column, ecs_record_t*);

    ecs_assert(count < ecs_vector_count(record_column), ECS_INTERNAL_ERROR, NULL);
    ecs_record_t *record_to_move = records[count];

    records[index] = record_to_move;
    ecs_vector_remove_last(record_column);    

    /* Update record of moved entity in entity index */
    if (index != count) {
        if (record_to_move) {
            if (record_to_move->row >= 0) {
                record_to_move->row = index + 1;
            } else {
                record_to_move->row = -(index + 1);
            }
            ecs_assert(record_to_move->table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(record_to_move->table == table, ECS_INTERNAL_ERROR, NULL);
        }
    } 

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    

    if (!count) {
        ecs_table_activate(world, table, NULL, false);
    }

    /* Move each component value in array to index */
    ecs_column_t *columns = data->columns;

    if (!(table->flags & EcsTableIsComplex)) {
        if (index == count) {
            fast_delete_last(columns, column_count);
        } else {
            fast_delete(columns, column_count, index);
        }
        return;
    }

    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        int16_t size = column->size;
        int16_t alignment = column->alignment;
        if (size) {
            ecs_c_info_t *c_info = c_info_array ? c_info_array[i] : NULL;
            ecs_xtor_t dtor;

            void *dst = ecs_vector_get_t(column->data, size, alignment, index);

            ecs_move_t move;
            if (c_info && (count != index) && (move = c_info->lifecycle.move)) {
                void *ctx = c_info->lifecycle.ctx;
                void *src = ecs_vector_get_t(column->data, size, alignment, count);
                ecs_entity_t component = c_info->component;

                /* If the delete is not destructing the component, the component
                * was already deleted, most likely by a move. In that case we
                * still need to move, but we need to make sure we're moving
                * into an element that is initialized with valid memory, so
                * call the constructor. */
                if (!destruct) {
                    ecs_xtor_t ctor = c_info->lifecycle.ctor;
                    ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);
                    ctor(world, c_info->component, &entity_to_move, dst,
                        ecs_to_size_t(size), 1, c_info->lifecycle.ctx);   
                }

                /* Move last element into deleted element */
                move(world, component, &entity_to_move, &entity_to_move, dst, src,
                    ecs_to_size_t(size), 1, ctx);

                /* Memory has been copied, we can now simply remove last */
                ecs_vector_remove_last(column->data);                              
            } else {
                if (destruct && c_info && (dtor = c_info->lifecycle.dtor)) {
                    dtor(world, c_info->component, &entities[index], dst, 
                        ecs_to_size_t(size), 1, c_info->lifecycle.ctx);
                }

                ecs_vector_remove_index_t(column->data, size, alignment, index);
            }
        }
    }

    /* Remove elements from switch columns */
    ecs_sw_column_t *sw_columns = data->sw_columns;
    int32_t sw_column_count = table->sw_column_count;
    for (i = 0; i < sw_column_count; i ++) {
        ecs_switch_remove(sw_columns[i].data, index);
    }

    /* Remove elements from bitset columns */
    ecs_bs_column_t *bs_columns = data->bs_columns;
    int32_t bs_column_count = table->bs_column_count;
    for (i = 0; i < bs_column_count; i ++) {
        ecs_bitset_remove(&bs_columns[i].data, index);
    }    
}

static
void fast_move(
    ecs_table_t * new_table,
    ecs_data_t * new_data,
    int32_t new_index,
    ecs_table_t * old_table,
    ecs_data_t * old_data,
    int32_t old_index)
{
    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t i_new = 0, new_column_count = new_table->column_count;
    int32_t i_old = 0, old_column_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if (new_component == old_component) {
            ecs_column_t *new_column = &new_columns[i_new];
            ecs_column_t *old_column = &old_columns[i_old];
            int16_t size = new_column->size;

            if (size) {
                int16_t alignment = new_column->alignment;
                void *dst = ecs_vector_get_t(new_column->data, size, alignment, new_index);
                void *src = ecs_vector_get_t(old_column->data, size, alignment, old_index);

                ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_os_memcpy(dst, src, size); 
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

void ecs_table_move(
    ecs_world_t * world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index)
{
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!((new_table->flags | old_table->flags) & EcsTableIsComplex)) {
        fast_move(new_table, new_data, new_index, old_table, old_data, old_index);
        return;
    }

    move_switch_columns(
        new_table, new_data, new_index, old_table, old_data, old_index, 1);

    move_bitset_columns(
        new_table, new_data, new_index, old_table, old_data, old_index, 1);

    bool same_entity = dst_entity == src_entity;

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t i_new = 0, new_column_count = new_table->column_count;
    int32_t i_old = 0, old_column_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if (new_component == old_component) {
            ecs_column_t *new_column = &new_columns[i_new];
            ecs_column_t *old_column = &old_columns[i_old];
            int16_t size = new_column->size;
            int16_t alignment = new_column->alignment;

            if (size) {
                void *dst = ecs_vector_get_t(new_column->data, size, alignment, new_index);
                void *src = ecs_vector_get_t(old_column->data, size, alignment, old_index);

                ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

                ecs_c_info_t *cdata = new_table->c_info[i_new];
                if (same_entity) {
                    ecs_move_t move;
                    if (cdata && (move = cdata->lifecycle.move)) {
                        void *ctx = cdata->lifecycle.ctx;
                        ecs_xtor_t ctor = cdata->lifecycle.ctor;

                        /* Ctor should always be set if copy is set */
                        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);

                        /* Construct a new value, move the value to it */
                        ctor(world, new_component, &dst_entity, dst, 
                                ecs_to_size_t(size), 1, ctx);

                        move(world, new_component, &dst_entity, &src_entity, 
                            dst, src, ecs_to_size_t(size), 1, ctx);
                    } else {
                        ecs_os_memcpy(dst, src, size);
                    }
                } else {
                    ecs_copy_t copy;
                    if (cdata && (copy = cdata->lifecycle.copy)) {
                        void *ctx = cdata->lifecycle.ctx;
                        ecs_xtor_t ctor = cdata->lifecycle.ctor;

                        /* Ctor should always be set if copy is set */
                        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);
                        ctor(world, new_component, &dst_entity, dst, 
                            ecs_to_size_t(size), 1, ctx);
                        copy(world, new_component, &dst_entity, &src_entity, 
                            dst, src, ecs_to_size_t(size), 1, ctx);
                    } else {
                        ecs_os_memcpy(dst, src, size);
                    }
                }
            }
        } else {
            if (new_component < old_component) {
                ctor_component(world, new_table->c_info[i_new],
                    &new_columns[i_new], &dst_entity, new_index, 1);
            } else {
                dtor_component(world, old_table->c_info[i_old],
                    &old_columns[i_old], &src_entity, old_index, 1);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }

    for (; (i_new < new_column_count); i_new ++) {
        ctor_component(world, new_table->c_info[i_new],
            &new_columns[i_new], &dst_entity, new_index, 1);
    }

    for (; (i_old < old_column_count); i_old ++) {
        dtor_component(world, old_table->c_info[i_old],
            &old_columns[i_old], &src_entity, old_index, 1);
    }
}

int32_t ecs_table_appendn(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t to_add,
    const ecs_entity_t *ids)
{
    int32_t cur_count = ecs_table_data_count(data);
    return grow_data(world, table, data, to_add, cur_count + to_add, ids);
}

void ecs_table_set_size(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t size)
{
    int32_t cur_count = ecs_table_data_count(data);

    if (cur_count < size) {
        grow_data(world, table, data, 0, size, NULL);
    } else if (!size) {
        /* Initialize columns if 0 is passed. This is a shortcut to initialize
         * columns when, for example, an API call is inserting bulk data. */
        int32_t column_count = table->column_count;
        int32_t sw_column_count = table->sw_column_count;
        int32_t bs_column_count = table->bs_column_count;
        ecs_column_t *columns;
        ecs_sw_column_t *sw_columns;
        ecs_bs_column_t *bs_columns;
        ensure_data(world, table, data, &column_count, &sw_column_count,
            &bs_column_count, &columns, &sw_columns, &bs_columns);
    }
}

int32_t ecs_table_data_count(
    ecs_data_t *data)
{
    return data ? ecs_vector_count(data->entities) : 0;
}

void ecs_table_swap(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
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

    ecs_assert(record_ptr_1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record_ptr_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Swap entities */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = row_2;
    record_ptr_2->row = row_1;
    record_ptrs[row_1] = record_ptr_2;
    record_ptrs[row_2] = record_ptr_1;

    if (row_2 < 0) {
        record_ptr_1->row --;
    } else {
        record_ptr_1->row ++;
    }
    if (row_1 < 0) {
        record_ptr_2->row --;
    } else {
        record_ptr_2->row ++;
    }    

    /* Swap columns */
    int32_t i, column_count = table->column_count;
    
    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        int16_t alignment = columns[i].alignment;
        void *ptr = ecs_vector_first_t(columns[i].data, size, alignment);

        if (size) {
            void *tmp = ecs_os_alloca(size);

            void *el_1 = ECS_OFFSET(ptr, size * row_1);
            void *el_2 = ECS_OFFSET(ptr, size * row_2);

            ecs_os_memcpy(tmp, el_1, size);
            ecs_os_memcpy(el_1, el_2, size);
            ecs_os_memcpy(el_2, tmp, size);
        }
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    
}

static
void merge_vector(
    ecs_vector_t ** dst_out,
    ecs_vector_t * src,
    int16_t size,
    int16_t alignment)
{
    ecs_vector_t *dst = *dst_out;
    int32_t dst_count = ecs_vector_count(dst);

    if (!dst_count) {
        if (dst) {
            ecs_vector_free(dst);
        }

        *dst_out = src;
    
    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = ecs_vector_count(src);
        ecs_vector_set_count_t(&dst, size, alignment, dst_count + src_count);
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        
        ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);

        ecs_vector_free(src);
        *dst_out = dst;
    }
}

static
void merge_table_data(
    ecs_world_t * world,
    ecs_table_t * new_table,
    ecs_table_t * old_table,
    int32_t old_count,
    int32_t new_count,
    ecs_data_t * old_data,
    ecs_data_t * new_data)
{
    int32_t i_new, new_component_count = new_table->column_count;
    int32_t i_old = 0, old_component_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_table->type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_table->type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    if (!new_columns && !new_data->entities) {
        ecs_init_data(world, new_table, new_data);
        new_columns = new_data->columns;
    }

    if (!old_count) {
        return;
    }

    /* Merge entities */
    merge_vector(&new_data->entities, old_data->entities, ECS_SIZEOF(ecs_entity_t), 
        ECS_ALIGNOF(ecs_entity_t));
    old_data->entities = NULL;
    ecs_entity_t *entities = ecs_vector_first(new_data->entities, ecs_entity_t);

    ecs_assert(ecs_vector_count(new_data->entities) == old_count + new_count, 
        ECS_INTERNAL_ERROR, NULL);

    /* Merge entity index record pointers */
    merge_vector(&new_data->record_ptrs, old_data->record_ptrs, 
        ECS_SIZEOF(ecs_record_t*), ECS_ALIGNOF(ecs_record_t*));
    old_data->record_ptrs = NULL;        

    for (i_new = 0; (i_new < new_component_count) && (i_old < old_component_count); ) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];
        int16_t size = new_columns[i_new].size;
        int16_t alignment = new_columns[i_new].alignment;

        if ((new_component & ECS_ROLE_MASK) || 
            (old_component & ECS_ROLE_MASK)) 
        {
            break;
        }

        if (new_component == old_component) {
            merge_vector(
                &new_columns[i_new].data, old_columns[i_old].data, size, 
                alignment);

            old_columns[i_old].data = NULL;

            /* Mark component column as dirty */
            mark_table_dirty(new_table, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            /* New column does not occur in old table, make sure vector is large
             * enough. */
            if (size) {
                ecs_column_t *column = &new_columns[i_new];
                ecs_vector_set_count_t(&column->data, size, alignment,
                    old_count + new_count);

                /* Construct new values */
                ecs_c_info_t *c_info;
                ecs_xtor_t ctor;
                if ((c_info = new_table->c_info[i_new]) && 
                    (ctor = c_info->lifecycle.ctor)) 
                {
                    ctor_component(world, c_info, column, 
                        entities, 0, old_count + new_count);
                }
            }
            
            i_new ++;
        } else if (new_component > old_component) {
            if (size) {
                ecs_column_t *column = &old_columns[i_old];
                
                /* Destruct old values */
                ecs_c_info_t *c_info;
                ecs_xtor_t dtor;
                if ((c_info = old_table->c_info[i_old]) && 
                    (dtor = c_info->lifecycle.dtor)) 
                {
                    dtor_component(world, c_info, column, 
                        entities, 0, old_count);
                }

                /* Old column does not occur in new table, remove */
                ecs_vector_free(column->data);
                column->data = NULL;

                i_old ++;
            }
        }
    }

    move_switch_columns(
        new_table, new_data, new_count, old_table, old_data, 0, old_count);

    /* Initialize remaining columns */
    for (; i_new < new_component_count; i_new ++) {
        ecs_column_t *column = &new_columns[i_new];
        int16_t size = column->size;
        int16_t alignment = column->alignment;

        if (size) {
            ecs_vector_set_count_t(&column->data, size, alignment,
                old_count + new_count);

            /* Construct new values */
            ecs_c_info_t *c_info;
            ecs_xtor_t ctor;
            if ((c_info = new_table->c_info[i_new]) && 
                (ctor = c_info->lifecycle.ctor)) 
            {
                ctor_component(world, c_info, column, 
                    entities, 0, old_count + new_count);
            }
        }
    }

    /* Destroy remaining columns */
    for (; i_old < old_component_count; i_old ++) {
        ecs_column_t *column = &old_columns[i_old];
                
        /* Destruct old values */
        ecs_c_info_t *c_info;
        ecs_xtor_t dtor;
        if ((c_info = old_table->c_info[i_old]) && 
            (dtor = c_info->lifecycle.dtor)) 
        {
            dtor_component(world, c_info, column, entities, 
                0, old_count);
        }

        /* Old column does not occur in new table, remove */
        ecs_vector_free(column->data);
        column->data = NULL;
    }    

    /* Mark entity column as dirty */
    mark_table_dirty(new_table, 0); 
}

int32_t ecs_table_count(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *data = table->data;
    if (!data) {
        return 0;
    }

    return ecs_table_data_count(data);
}

ecs_data_t* ecs_table_merge(
    ecs_world_t * world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    ecs_data_t *new_data,
    ecs_data_t *old_data)
{
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    bool move_data = false;
    
    /* If there is nothing to merge to, just clear the old table */
    if (!new_table) {
        ecs_table_clear_data(old_table, old_data);
        return NULL;
    }

    /* If there is no data to merge, drop out */
    if (!old_data) {
        return NULL;
    }

    if (!new_data) {
        new_data = ecs_table_get_or_create_data(new_table);
        if (new_table == old_table) {
            move_data = true;
        }
    }

    ecs_entity_t *old_entities = ecs_vector_first(old_data->entities, ecs_entity_t);

    int32_t old_count = ecs_vector_count(old_data->entities);
    int32_t new_count = ecs_vector_count(new_data->entities);

    ecs_record_t **old_records = ecs_vector_first(
        old_data->record_ptrs, ecs_record_t*);

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < old_count; i ++) {
        ecs_record_t *record;
        if (new_table != old_table) {
            record = old_records[i];
            ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        } else {
            record = ecs_eis_get_or_create(world, old_entities[i]);
        }

        bool is_monitored = record->row < 0;
        record->row = ecs_row_to_record(new_count + i, is_monitored);
        record->table = new_table;
    }

    /* Merge table columns */
    if (move_data) {
        *new_data = *old_data;
    } else {
        merge_table_data(world, new_table, old_table, old_count, new_count, 
            old_data, new_data);
    }

    new_table->alloc_count ++;

    if (!new_count && old_count) {
        ecs_table_activate(world, new_table, NULL, true);
    }

    return new_data;
}

void ecs_table_replace_data(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = table->data;
    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);

    if (table_data) {
        prev_count = ecs_vector_count(table_data->entities);
        run_remove_actions(
            world, table, table_data, 0, ecs_table_data_count(table_data), false);
        ecs_table_clear_data(table, table_data);
    }

    if (data) {
        table_data = ecs_table_get_or_create_data(table);
        *table_data = *data;
    } else {
        return;
    }

    int32_t count = ecs_table_count(table);

    if (!prev_count && count) {
        ecs_table_activate(world, table, 0, true);
    } else if (prev_count && !count) {
        ecs_table_activate(world, table, 0, false);
    }
}

bool ecs_table_match_filter(
    ecs_world_t * world,
    ecs_table_t * table,
    const ecs_filter_t * filter)
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
        table->dirty_state = ecs_os_calloc(ECS_SIZEOF(int32_t) * (table->column_count + 1));
        ecs_assert(table->dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    }
    return table->dirty_state;
}

int32_t* ecs_table_get_monitor(
    ecs_table_t *table)
{
    int32_t *dirty_state = ecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = table->column_count;
    return ecs_os_memdup(dirty_state, (column_count + 1) * ECS_SIZEOF(int32_t));
}

void ecs_table_notify(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_table_event_t * event)
{
    switch(event->kind) {
    case EcsTableQueryMatch:
        register_query(
            world, table, event->query, event->matched_table_index);
        break;
    case EcsTableQueryUnmatch:
        unregister_query(
            world, table, event->query);
        break;
    case EcsTableComponentInfo:
        notify_component_info(world, table, event->component);
        break;
    }
}
