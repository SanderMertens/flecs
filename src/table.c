#include "private_api.h"

static
ecs_type_t get_storage_type(
    ecs_world_t *world,
    ecs_type_t type)
{
    int32_t i, count = ecs_vector_count(type);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    ecs_ids_t storage_ids = {
        .array = ecs_os_alloca_n(ecs_id_t, count)
    };

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        if ((id == ecs_id(EcsComponent)) || (id == ecs_id(EcsName))) {
            storage_ids.array[storage_ids.count ++] = id;
            continue;
        }

        if (ECS_HAS_ROLE(id, SWITCH)) {
            storage_ids.array[storage_ids.count ++] = id;
            continue;
        }

        const EcsComponent *comp = ecs_component_from_id(world, id);
        if (!comp || !comp->size) {
            continue;
        }

        storage_ids.array[storage_ids.count ++] = id;
    }

    ecs_table_t *storage_table = ecs_table_find_or_create(world, &storage_ids);
    ecs_assert(storage_table != NULL, ECS_INTERNAL_ERROR, NULL);

    return storage_table->type;
}

void ecs_table_storage_init(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = result->type = get_storage_type(world, table->type);
    if (!type) {
        return;
    }

    int32_t i, count = ecs_vector_count(type);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    result->storages = ecs_os_calloc_n(ecs_storage_t, count);

    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        ecs_storage_t *storage = &result->storages[i];

        const EcsComponent *component = ecs_component_from_id(world, id);
        if (component) {
            storage->size = component->size;
            storage->alignment = component->alignment;
        }

        if (ECS_HAS_ROLE(id, DISABLED)) {
            ecs_bitset_storage_init(world, storage);
        } else if (ECS_HAS_ROLE(id, SWITCH)) {
            /* TODO */
        }
    }
}

static
ecs_flags32_t get_lifecycle_flags(
    const ecs_type_info_t *c_info) 
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

    return flags;  
}

/* Check if table has instance of component, including pairs */
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
    ecs_data_t *s = &table->storage;
    ecs_type_t type = s->type;

    if (!component || has_component(world, type, component)){
        int32_t i, count = ecs_vector_count(type);
        ecs_assert(!component || count != 0, ECS_INTERNAL_ERROR, NULL);

        if (!count) {
            return;
        }

        /* Reset lifecycle flags before recomputing */
        table->flags &= ~EcsTableHasLifecycle;

        /* Recompute lifecycle flags */
        ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
        for (i = 0; i < count; i ++) {
            ecs_entity_t c = ecs_get_typeid(world, ids[i]);
            if (!c) {
                continue;
            }
            
            const ecs_type_info_t *info = ecs_get_type_info(world, c);
            if (info) {
                ecs_flags32_t flags = get_lifecycle_flags(info);
                if (!flags) {
                    continue;
                }

                table->flags |= flags;
            }

            s->storages[i].info = info;
        }
    }
}

static
void notify_trigger(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_entity_t event) 
{
    (void)world;

    if (!(table->flags & EcsTableIsDisabled)) {
        if (event == EcsOnAdd) {
            table->flags |= EcsTableHasOnAdd;
        } else if (event == EcsOnRemove) {
            table->flags |= EcsTableHasOnRemove;
        } else if (event == EcsOnSet) {
            table->flags |= EcsTableHasOnSet;
        } else if (event == EcsUnSet) {
            table->flags |= EcsTableHasUnSet;
        }
    }
}

static
void run_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        ecs_run_monitors(world, table, table->un_set_all, 0, count, NULL);

        int32_t i, type_count = ecs_vector_count(table->type);
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
        for (i = 0; i < type_count; i ++) {
            ecs_ids_t removed = {
                .array = &ids[i],
                .count = 1
            };

            ecs_run_remove_actions(world, table, data, 0, count, &removed);
        }
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
 * a list of queries that match so that they can be notified when the table
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
        if (ECS_HAS_RELATION(e, EcsIsA)) {
            if (ecs_has_id(world, ECS_PAIR_OBJECT(e), comp)) {
                return true;
            }
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

    ecs_type_t type = table->storage.type;

    if (type) {
        int32_t storage_count = ecs_vector_count(type);
        if (!table->on_set) {
            table->on_set = ecs_os_calloc_n(ecs_vector_t*, storage_count);
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
        ecs_term_t *terms = query->filter.terms;
        int32_t i, count = query->filter.term_count;
        
        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            ecs_term_id_t *subj = &term->args[0];
            ecs_oper_kind_t oper = term->oper;

            if (!(subj->set.mask & EcsSelf) || !subj->entity ||
                subj->entity != EcsThis)
            {
                continue;
            }

            if (oper != EcsAnd && oper != EcsOptional) {
                continue;
            }

            ecs_entity_t comp = matched_table->iter_data.components[i];
            int32_t index = ecs_type_index_of(type, comp);
            if (index == -1) {
                continue;
            }

            if (index >= storage_count) {
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
    table->flags |= EcsTableHasUnSet;
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

        ecs_data_t *data = &table->storage;
        if (ecs_vector_count(data->entities)) {
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
        ecs_vector_remove(table->queries, ecs_query_t*, i);        
    }
}

const ecs_data_t* ecs_table_storage_get(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return &table->storage;
}

ecs_data_t* ecs_table_storage_get_mut(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_INVALID_OPERATION, NULL);
    return &table->storage;
}

static
void dtor_all_components(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    bool update_entity_index,
    bool is_delete)
{
    /* Can't delete and not update the entity index */
    ecs_assert(!is_delete || update_entity_index, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t **records = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t i, c, end = row + count;
    int32_t storage_count = ecs_vector_count(data->type);
    ecs_storage_t *storages = data->storages;

    (void)records;

    /* If table has components with destructors, iterate component columns */
    if (table->flags & EcsTableHasDtors) {
        /* Prevent the storage from getting modified while deleting */
        ecs_defer_begin(world);

        /* Throw up a lock just to be sure */
        table->lock = true;

        /* Iterate entities first, then components. This ensures that only one
         * entity is invalidated at a time, which ensures that destructors can
         * safely access other entities. */
        for (i = row; i < end; i ++) {
            for (c = 0; c < storage_count; c++) {
                ecs_storage_dtor(world, &storages[c], i);
            }

            /* Update entity index after invoking destructors so that entity can
             * be safely used in destructor callbacks. */
            if (update_entity_index) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i] == ecs_eis_get(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i]->table == table, 
                    ECS_INTERNAL_ERROR, NULL);

                if (is_delete) {
                    ecs_eis_delete(world, e);
                    ecs_assert(ecs_is_valid(world, e) == false, 
                        ECS_INTERNAL_ERROR, NULL);
                } else {
                    // If this is not a delete, clear the entity index record
                    ecs_record_t r = {NULL, 0};
                    ecs_eis_set(world, e, &r);                
                }
            } else {
                /* This should only happen in rare cases, such as when the data
                 * cleaned up is not part of the world (like with snapshots) */
            }
        }

        table->lock = false;
    
        ecs_defer_end(world);

    /* If table does not have destructors, just update entity index */
    } else if (update_entity_index) {
        if (is_delete) {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i] == ecs_eis_get(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i]->table == table, 
                    ECS_INTERNAL_ERROR, NULL);

                ecs_eis_delete(world, e);
                ecs_assert(!ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
            } 
        } else {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i] == ecs_eis_get(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i]->table == table, 
                    ECS_INTERNAL_ERROR, NULL);                
                ecs_record_t r = {NULL, 0};
                ecs_eis_set(world, e, &r);
            }
        }      
    }
}

static
void fini_storage(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    bool do_on_remove,
    bool update_entity_index,
    bool is_delete,
    bool deactivate)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (do_on_remove) {
        run_on_remove(world, table, data);        
    }

    int32_t count = ecs_table_storage_count(data);
    if (count) {
        dtor_all_components(world, table, data, 0, count, 
            update_entity_index, is_delete);
    }

    /* Sanity check */
    ecs_assert(ecs_vector_count(data->record_ptrs) == 
        ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);

    int32_t c, storage_count = ecs_vector_count(data->type);
    ecs_storage_t *storages = data->storages;
    if (storages) {
        for (c = 0; c < storage_count; c ++) {
            /* Sanity check */
            ecs_assert(ecs_storage_count(&storages[c]) == count, 
                ECS_INTERNAL_ERROR, NULL);

            ecs_storage_fini(&storages[c]);
        }
        ecs_os_free(storages);
        data->storages = NULL;
    }

    ecs_vector_free(data->entities);
    ecs_vector_free(data->record_ptrs);

    data->entities = NULL;
    data->record_ptrs = NULL;

    if (deactivate && count) {
        ecs_table_activate(world, table, 0, false);
    }
}

/* Cleanup, no OnRemove, don't update entity index, don't deactivate table */
void ecs_table_clear_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    fini_storage(world, table, data, false, false, false, false);
}

/* Cleanup, no OnRemove, clear entity index, deactivate table */
void ecs_table_clear_entities_silent(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_storage(world, table, &table->storage, false, true, false, true);
}

/* Cleanup, run OnRemove, clear entity index, deactivate table */
void ecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_storage(world, table, &table->storage, true, true, false, true);
}

/* Cleanup, run OnRemove, delete from entity index, deactivate table */
void ecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_storage(world, table, &table->storage, true, true, true, true);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all UnSet handlers, if any */
void ecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    run_on_remove(world, table, &table->storage);
}

/* Free table resources. */
void ecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    (void)world;

    /* Cleanup data, no OnRemove, delete from entity index, don't deactivate */
    ecs_data_t *data = &table->storage;
    fini_storage(world, table, data, false, true, true, false);

    ecs_table_clear_edges(world, table);

    ecs_unregister_table(world, table);

    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    ecs_vector_free(table->queries);
    ecs_os_free(table->dirty_state);
    ecs_vector_free(table->monitors);
    ecs_vector_free(table->on_set_all);
    ecs_vector_free(table->on_set_override);
    ecs_vector_free(table->un_set_all);
    
    if (table->on_set) {
        int32_t i, count = ecs_vector_count(data->type);
        for (i = 0; i < count; i ++) {
            ecs_vector_free(table->on_set[i]);
        }
        ecs_os_free(table->on_set);
    }

    table->id = 0;
}

/* Free table type. Do this separately from freeing the table as types can be
 * in use by application destructors. */
void ecs_table_free_type(
    ecs_table_t *table)
{
    ecs_vector_free((ecs_vector_t*)table->type);
}

/* Reset a table to its initial state. */
void ecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    
    (void)world;
    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    table->lo_edges = NULL;
    table->hi_edges = NULL;
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
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->dirty_state) {
        int32_t index = ecs_type_index_of(table->type, component);
        ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
        table->dirty_state[index] ++;
    }
}

static
int32_t grow_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t to_add,
    int32_t size,
    const ecs_entity_t *ids)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur_count = ecs_table_storage_count(data);
    int32_t storage_count = ecs_vector_count(data->type);
    ecs_storage_t *storages = data->storages;

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

    /* Initialize entity ids */
    int32_t i;
    if (ids) {
        for (i = 0; i < to_add; i ++) {
            e[i] = ids[i];
        }
    } else {
        ecs_os_memset_n(e, 0, ecs_entity_t, to_add);
    }
    ecs_os_memset_n(r, 0, ecs_record_t*, to_add);

    /* Add elements to each column array */
    for (i = 0; i < storage_count; i ++) {
        ecs_storage_grow(world, &storages[i], to_add);
    }

    /* If the table is monitored indicate that there has been a change */
    if (&table->storage == data) {
        mark_table_dirty(table, 0);
        if (!world->is_readonly && !cur_count) {
            ecs_table_activate(world, table, 0, true);
        }        
    }

    table->alloc_count ++;

    /* Return index of first added entity */
    return cur_count;
}

static
void fast_append(
    ecs_storage_t *storages,
    int32_t storage_count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < storage_count; i ++) {
        ecs_storage_t *storage = &storages[i];
        ecs_vector_add_t((ecs_vector_t**)&storage->impl, 
            storage->size, storage->alignment);
    }
}

int32_t ecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    ecs_entity_t entity,
    ecs_record_t * record,
    bool construct)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    int32_t count = ecs_vector_count(data->entities);
    int32_t size = ecs_vector_size(data->entities);

    int32_t storage_count = ecs_vector_count(data->type);
    ecs_storage_t *storages = NULL;

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
    if (!world->is_readonly && !count) {
        ecs_table_activate(world, table, 0, true);
    } 

    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(table->flags & EcsTableIsComplex)) {
        fast_append(storages, storage_count);
        return count;
    }

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < storage_count; i ++) {
        ecs_assert(storages != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_storage_push(world, &storages[i], entity);
    }

    return count;
}

static
void fast_delete_last(
    ecs_storage_t *storages,
    int32_t storage_count) 
{
    int i;
    for (i = 0; i < storage_count; i ++) {
        ecs_vector_remove_last(storages[i].impl);
    }
}

static
void fast_delete(
    ecs_storage_t *storages,
    int32_t storage_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < storage_count; i ++) {
        ecs_storage_t *storage = &storages[i];
        ecs_vector_remove_t(storage->impl, 
            storage->size, storage->alignment, index);
    }
}

void ecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t index,
    bool destruct)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_vector_t *v_entities = data->entities;
    int32_t count = ecs_vector_count(v_entities);

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    /* Move last entity id to index */
    ecs_entity_t *entities = ecs_vector_first(v_entities, ecs_entity_t);
    ecs_entity_t entity_to_move = entities[count];
    ecs_entity_t entity_to_delete = entities[index];
    entities[index] = entity_to_move;
    ecs_vector_remove_last(v_entities);

    /* Move last record ptr to index */
    ecs_vector_t *v_records = data->record_ptrs;     
    ecs_assert(count < ecs_vector_count(v_records), ECS_INTERNAL_ERROR, NULL);

    ecs_record_t **records = ecs_vector_first(v_records, ecs_record_t*);
    ecs_record_t *record_to_move = records[count];
    records[index] = record_to_move;
    ecs_vector_remove_last(v_records); 

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

    /* If table is empty, deactivate it */
    if (!count) {
        ecs_table_activate(world, table, NULL, false);
    }

    /* Destruct component data */
    ecs_storage_t *storages = data->storages;
    int32_t storage_count = ecs_vector_count(data->type);
    int32_t i;

    /* If this is a table without lifecycle callbacks or special columns, take
     * fast path that just remove an element from the array(s) */
    if (!(table->flags & EcsTableIsComplex)) {
        if (index == count) {
            fast_delete_last(storages, storage_count);
        } else {
            fast_delete(storages, storage_count, index);
        }

        return;
    }

    /* Last element, destruct & remove */
    if (index == count) {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & EcsTableHasDtors)) {            
            for (i = 0; i < storage_count; i ++) {
                ecs_storage_dtor(world, &storages[i], entity_to_delete);
            }
        }

        fast_delete_last(storages, storage_count);

    /* Not last element, move last element to deleted element & destruct */
    } else {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & (EcsTableHasDtors | EcsTableHasMove))) {
            for (i = 0; i < storage_count; i ++) {
                ecs_storage_erase(world, &storages[i], index, entity_to_delete);
            }

        } else {
            fast_delete(storages, storage_count, index);
        }
    }
}

static
void fast_move(
    ecs_data_t * new_data,
    int32_t new_index,
    ecs_data_t * old_data,
    int32_t old_index)
{
    ecs_type_t new_type = new_data->type;
    ecs_type_t old_type = old_data->type;

    int32_t i_new = 0, new_storage_count = ecs_vector_count(new_type);
    int32_t i_old = 0, old_storage_count = ecs_vector_count(old_type);
    ecs_id_t *new_ids = ecs_vector_first(new_type, ecs_id_t);
    ecs_id_t *old_ids = ecs_vector_first(old_type, ecs_id_t);

    ecs_storage_t *old_storages = old_data->storages;
    ecs_storage_t *new_storages = new_data->storages;

    for (; (i_new < new_storage_count) && (i_old < old_storage_count);) {
        ecs_id_t new_id = new_ids[i_new];
        ecs_id_t old_id = old_ids[i_old];

        if (new_id == old_id) {
            ecs_storage_t *new_storage = &new_storages[i_new];
            ecs_storage_t *old_storage = &old_storages[i_old];
            ecs_size_t size = new_storage->size;
            ecs_size_t alignment = new_storage->alignment;

            void *dst = ecs_vector_get_t(
                new_storage->impl, size, alignment, new_index);
            void *src = ecs_vector_get_t(
                old_storage->impl, size, alignment, old_index);

            ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_os_memcpy(dst, src, size); 
        }

        i_new += new_id <= old_id;
        i_old += new_id >= old_id;
    }
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
    bool construct)
{
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!new_table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(!old_table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!((new_table->flags | old_table->flags) & EcsTableIsComplex)) {
        fast_move(new_data, new_index, old_data, old_index);
        return;
    }

    bool same_entity = dst_entity == src_entity;

    ecs_type_t new_type = new_data->type;
    ecs_type_t old_type = old_data->type;

    int32_t i_new = 0, new_storage_count = ecs_vector_count(new_type);
    int32_t i_old = 0, old_storage_count = ecs_vector_count(old_type);
    ecs_id_t *new_ids = ecs_vector_first(new_type, ecs_id_t);
    ecs_id_t *old_ids = ecs_vector_first(old_type, ecs_id_t);

    ecs_storage_t *old_storages = old_data->storages;
    ecs_storage_t *new_storages = new_data->storages;

    for (; (i_new < new_storage_count) && (i_old < old_storage_count);) {
        ecs_id_t new_id = new_ids[i_new];
        ecs_id_t old_id = old_ids[i_old];

        if (new_id == old_id) {
            ecs_storage_t *new_storage = &new_storages[i_new];
            ecs_storage_t *old_storage = &old_storages[i_old];
            void *ptr = ecs_storage_get(old_storage, old_index, src_entity);

            if (same_entity) {
                ecs_storage_ctor_move_dtor(world, new_storage, new_index, ptr);
            } else {
                ecs_storage_ctor_copy(world, new_storage, new_index, 
                    dst_entity, ptr);
            }
        } else {
            if (construct && new_id < old_id) {
                ecs_storage_ctor(
                    world, &new_storages[i_new], new_index, dst_entity);
            } else {
                ecs_storage_dtor(world, &old_storages[i_old], src_entity);
            }
        }

        i_new += new_id <= old_id;
        i_old += new_id >= old_id;
    }

    if (construct) {
        for (; (i_new < new_storage_count); i_new ++) {
            ecs_storage_ctor(
                world, &new_storages[i_new], new_index, dst_entity);
        }
    }

    for (; (i_old < old_storage_count); i_old ++) {
        ecs_storage_dtor(world, &old_storages[i_old], src_entity);
    }
}

int32_t ecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *dst_data,
    int32_t count,
    const ecs_entity_t *ids,
    ecs_data_t *src_data,
    int32_t src_row)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    int32_t cur_count = ecs_table_count(table);
    int32_t dst_row = grow_data(world, table, dst_data, count, 
        cur_count + count, ids);

    if (src_data) {
        ecs_type_t dst_type = dst_data->type;
        ecs_type_t src_type = src_data->type;
        int32_t i_dst = 0, dst_storage_count = ecs_vector_count(dst_type);
        int32_t i_src = 0, src_storage_count = ecs_vector_count(src_type);
        ecs_id_t *dst_ids = ecs_vector_first(dst_type, ecs_id_t);
        ecs_id_t *src_ids = ecs_vector_first(src_type, ecs_id_t);

        ecs_storage_t *src_storages = src_data->storages;
        ecs_storage_t *dst_storages = dst_data->storages;

        for (; (i_dst < dst_storage_count) && (i_src < src_storage_count); ) {
            ecs_id_t dst_id = dst_ids[i_dst];
            ecs_id_t src_id = src_ids[i_src];

            if (dst_id == src_id) {
                ecs_storage_push_n(world, &dst_storages[i_dst], count, ids,
                    &src_storages[i_src], src_row);

                if (&table->storage == dst_data) {
                    mark_table_dirty(table, i_dst + 1);
                }
                
                i_dst ++;
                i_src ++;
            }
        }
    }

    return dst_row;
}

void ecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t size)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    int32_t cur_count = ecs_table_storage_count(data);

    if (cur_count < size) {
        grow_data(world, table, data, 0, size, NULL);
    }
}

int32_t ecs_table_storage_count(
    const ecs_data_t *data)
{
    return data ? ecs_vector_count(data->entities) : 0;
}

int32_t ecs_table_count(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_table_storage_count(&table->storage);
}

void ecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2)
{    
    (void)world;

    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_1 >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_2 >= 0, ECS_INTERNAL_ERROR, NULL);
    
    if (row_1 == row_2) {
        return;
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_record_t *record_ptr_1 = record_ptrs[row_1];
    ecs_record_t *record_ptr_2 = record_ptrs[row_2];

    ecs_assert(record_ptr_1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record_ptr_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Keep track of whether entity is watched */
    bool watched_1 = record_ptr_1->row < 0;
    bool watched_2 = record_ptr_2->row < 0;

    /* Swap entities & records */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = ecs_row_to_record(row_2, watched_1);
    record_ptr_2->row = ecs_row_to_record(row_1, watched_2);
    record_ptrs[row_1] = record_ptr_2;
    record_ptrs[row_2] = record_ptr_1;

    int32_t i = 0, storage_count = ecs_vector_count(data->type);
    ecs_storage_t *storages = data->storages;

    for (i = 0; i < storage_count; i ++) {
        ecs_storage_swap(world, &storages[i], row_1, row_2);
    } 
}

static
void table_storage_merge(
    ecs_world_t *world,
    ecs_table_t * new_table,
    int32_t old_count,
    int32_t new_count,
    ecs_data_t * old_data,
    ecs_data_t * new_data)
{
    ecs_type_t new_type = new_data->type;
    ecs_type_t old_type = old_data->type;
    int32_t i_new = 0, new_storage_count = ecs_vector_count(new_type);
    int32_t i_old = 0, old_storage_count = ecs_vector_count(old_type);
    ecs_id_t *new_ids = ecs_vector_first(new_type, ecs_id_t);
    ecs_id_t *old_ids = ecs_vector_first(old_type, ecs_id_t);

    ecs_storage_t *old_storages = old_data->storages;
    ecs_storage_t *new_storages = new_data->storages;

    ecs_assert(new_storages != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!new_storage_count || new_storages, ECS_INTERNAL_ERROR, NULL);

    if (!old_count) {
        return;
    }

    /* Merge entities */
    ecs_vector_merge_t(&new_data->entities, old_data->entities, 
        ECS_SIZEOF(ecs_entity_t), ECS_ALIGNOF(ecs_entity_t));
    old_data->entities = NULL;
    ecs_entity_t *entities = ecs_vector_first(new_data->entities, ecs_entity_t);

    ecs_assert(ecs_vector_count(new_data->entities) == old_count + new_count, 
        ECS_INTERNAL_ERROR, NULL);

    /* Merge entity index record pointers */
    ecs_vector_merge_t(&new_data->record_ptrs, old_data->record_ptrs, 
        ECS_SIZEOF(ecs_record_t*), ECS_ALIGNOF(ecs_record_t*));
    old_data->record_ptrs = NULL;        

    for (; (i_new < new_storage_count) && (i_old < old_storage_count); ) {
        ecs_id_t new_id = new_ids[i_new];
        ecs_id_t old_id = old_ids[i_old];

        if (new_id == old_id) {
            ecs_storage_merge(world, &new_storages[i_new], &old_storages[i_old]);
            mark_table_dirty(new_table, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (new_id < old_id) {
            ecs_storage_push_n(
                world, &new_storages[i_new], old_count, entities, NULL, 0);

            i_new ++;
        } else if (new_id > old_id) {
            ecs_storage_destruct(world, &old_storages[i_old]);

            i_old ++;
        }
    }

    /* Initialize remaining storages */
    for (; i_new < new_storage_count; i_new ++) {
        ecs_storage_push_n(
            world, &new_storages[i_new], old_count, entities, NULL, 0);
    }

    /* Destroy remaining storages */
    for (; i_old < old_storage_count; i_old ++) {
        ecs_storage_destruct(world, &old_storages[i_old]);
    }    

    /* Mark entity column as dirty */
    mark_table_dirty(new_table, 0); 
}

ecs_data_t* ecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    ecs_data_t *new_data,
    ecs_data_t *old_data)
{
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!old_table->lock, ECS_LOCKED_STORAGE, NULL);
    
    bool move_data = false;
    
    /* If there is nothing to merge to, just clear the old table */
    if (!new_table) {
        ecs_table_clear_data(world, old_table, old_data);
        return NULL;
    } else {
        ecs_assert(!new_table->lock, ECS_LOCKED_STORAGE, NULL);
    }

    /* If there is no data to merge, drop out */
    if (!old_data) {
        return NULL;
    }

    if (!new_data) {
        new_data = &new_table->storage;
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
            record = ecs_eis_ensure(world, old_entities[i]);
        }

        bool is_monitored = record->row < 0;
        record->row = ecs_row_to_record(new_count + i, is_monitored);
        record->table = new_table;
    }

    /* Merge table columns */
    if (move_data) {
        *new_data = *old_data;
    } else {
        table_storage_merge(world, new_table, old_count, new_count, 
            old_data, new_data);
    }

    new_table->alloc_count ++;

    if (!new_count && old_count) {
        ecs_table_activate(world, new_table, NULL, true);
    }

    return new_data;
}

void ecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = &table->storage;
    ecs_assert(data != table_data, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    if (table_data) {
        prev_count = ecs_vector_count(table_data->entities);
        run_on_remove(world, table, table_data);
        ecs_table_clear_data(world, table, table_data);
    }

    if (data) {
        table_data = &table->storage;
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
    const ecs_world_t *world,
    const ecs_table_t *table,
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
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    
    if (!table->dirty_state) {
        table->dirty_state = ecs_os_calloc_n(int32_t, 
            ecs_vector_count(table->storage.type));
        ecs_assert(table->dirty_state != NULL, ECS_OUT_OF_MEMORY, NULL);
    }
    return table->dirty_state;
}

int32_t* ecs_table_get_monitor(
    ecs_table_t *table)
{
    int32_t *dirty_state = ecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t storage_count = ecs_vector_count(table->storage.type);
    return ecs_os_memdup_n(dirty_state, int32_t, storage_count + 1);
}

void ecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t * event)
{
    if (world->is_fini) {
        return;
    }

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
    case EcsTableTriggerMatch:
        notify_trigger(world, table, event->event);
        break;
    }
}

void ecs_table_lock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (world->magic == ECS_WORLD_MAGIC && !world->is_readonly) {
        table->lock ++;
    }
}

void ecs_table_unlock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (world->magic == ECS_WORLD_MAGIC && !world->is_readonly) {
        table->lock --;
        ecs_assert(table->lock >= 0, ECS_INVALID_OPERATION, NULL);
    }
}

bool ecs_table_has_module(
    ecs_table_t *table)
{
    return table->flags & EcsTableHasModule;
}

ecs_type_t ecs_table_get_type(
    const ecs_table_t *table)
{
    return table->type;
}

ecs_type_t ecs_table_get_storage_type(
    const ecs_table_t *table)
{
    return table->storage.type;
}

ecs_record_t* ecs_record_find(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_record_t *r = ecs_eis_get(world, entity);
    if (r) {
        return r;
    } else {
        return NULL;
    }
}

void* ecs_record_get_component(
    ecs_record_t *r,
    int32_t index,
    size_t size)
{
    (void)size;
    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    ecs_storage_t *s = &table->storage.storages[index];
    ecs_assert(s->size == ecs_from_size_t(size), ECS_INTERNAL_ERROR, NULL);
    bool is_watched;
    return ecs_storage_get(s, ecs_record_to_row(r->row, &is_watched), 0);
}
