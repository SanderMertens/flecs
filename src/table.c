#include "private_api.h"

/* Count number of switch columns */
static
int32_t switch_column_count(
    ecs_table_t *table)
{
    int32_t i, sw_count = 0, count = ecs_vector_count(table->type);
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];
        if (ECS_HAS_ROLE(id, SWITCH)) {
            if (!sw_count) {
                table->sw_column_offset = i;
            }
            sw_count ++;
        }
    }

    return sw_count;
}

/* Count number of bitset columns */
static
int32_t bitset_column_count(
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, DISABLED)) {
            if (!count) {
                table->bs_column_offset = c_ptr_i;
            }
            count ++;
        }
    });

    return count;
}

static
void init_storage_map(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!table->storage_table) {
        return;
    }

    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    int32_t t, ids_count = ecs_vector_count(table->type);
    ecs_id_t *storage_ids = ecs_vector_first(table->storage_type, ecs_id_t);
    int32_t s, storage_ids_count = ecs_vector_count(table->storage_type);

    if (!ids_count) {
        table->storage_map = NULL;
        return;
    }

    table->storage_map = ecs_os_malloc_n(
        int32_t, ids_count + storage_ids_count);

    int32_t *t2s = table->storage_map;
    int32_t *s2t = &table->storage_map[ids_count];

    for (s = 0, t = 0; (t < ids_count) && (s < storage_ids_count); ) {
        ecs_id_t id = ids[t];
        ecs_id_t storage_id = storage_ids[s];

        if (id == storage_id) {
            t2s[t] = s;
            s2t[s] = t;
        } else {
            t2s[t] = -1;
        }

        /* Ids can never get ahead of storage id, as ids are a superset of the
         * storage ids */
        ecs_assert(id <= storage_id, ECS_INTERNAL_ERROR, NULL);

        t += (id <= storage_id);
        s += (id == storage_id);
    }

    /* Storage ids is always a subset of ids, so all should be iterated */
    ecs_assert(s == storage_ids_count, ECS_INTERNAL_ERROR, NULL);

    /* Initialize remainder of type -> storage_type map */
    for (; (t < ids_count); t ++) {
        t2s[t] = -1;
    }
}

static
void init_storage_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    int32_t i, count = ecs_vector_count(table->type);
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    ecs_ids_t storage_ids = {
        .array = ecs_os_alloca_n(ecs_id_t, count)
    };

    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        if ((id == ecs_id(EcsComponent)) || 
            (ECS_PAIR_FIRST(id) == ecs_id(EcsIdentifier))) 
        {
            storage_ids.array[storage_ids.count ++] = id;
            continue;
        }

        const EcsComponent *comp = flecs_component_from_id(world, id);
        if (!comp || !comp->size) {
            continue;
        }

        storage_ids.array[storage_ids.count ++] = id;
    }
    
    if (storage_ids.count && storage_ids.count != count) {
        table->storage_table = flecs_table_find_or_create(world, &storage_ids);
        table->storage_type = table->storage_table->type;
        table->storage_table->refcount ++;
        ecs_assert(table->storage_table != NULL, ECS_INTERNAL_ERROR, NULL);
    } else if (storage_ids.count) {
        table->storage_table = table;
        table->storage_type = table->storage_table->type;
        ecs_assert(table->storage_table != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (!table->storage_map) {
        init_storage_map(table);
    }
}

void flecs_table_init_data(
    ecs_world_t *world,
    ecs_table_t *table)
{
    init_storage_table(world, table);

    int32_t sw_count = table->sw_column_count = switch_column_count(table);
    int32_t bs_count = table->bs_column_count = bitset_column_count(table);

    ecs_data_t *storage = &table->storage;
    ecs_type_t type = table->storage_type;

    int32_t i, count = ecs_vector_count(type);

    /* Root tables don't have columns */
    if (!count && !sw_count && !bs_count) {
        storage->columns = NULL;
    }

    if (count) {
        ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);
        storage->columns = ecs_os_calloc_n(ecs_column_t, count);

        for (i = 0; i < count; i ++) {
            ecs_entity_t id = ids[i];

            /* Bootstrap components */
            if (id == ecs_id(EcsComponent)) {
                storage->columns[i].size = ECS_SIZEOF(EcsComponent);
                storage->columns[i].alignment = ECS_ALIGNOF(EcsComponent);
                continue;
            } else if (ECS_PAIR_FIRST(id) == ecs_id(EcsIdentifier)) {
                storage->columns[i].size = ECS_SIZEOF(EcsIdentifier);
                storage->columns[i].alignment = ECS_ALIGNOF(EcsIdentifier);
                continue;
            }

            const EcsComponent *component = flecs_component_from_id(world, id);
            ecs_assert(component != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(component->size != 0, ECS_INTERNAL_ERROR, NULL);

            storage->columns[i].size = flecs_itoi16(component->size);
            storage->columns[i].alignment = flecs_itoi16(component->alignment);
        }
    }

    if (sw_count) {
        ecs_entity_t *ids = ecs_vector_first(table->type, ecs_entity_t);
        int32_t sw_offset = table->sw_column_offset;
        storage->sw_columns = ecs_os_calloc_n(ecs_sw_column_t, sw_count);

        for (i = 0; i < sw_count; i ++) {
            ecs_entity_t e = ids[i + sw_offset];
            ecs_assert(ECS_HAS_ROLE(e, SWITCH), ECS_INTERNAL_ERROR, NULL);
            e = e & ECS_COMPONENT_MASK;
            const EcsType *type_ptr = ecs_get(world, e, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_table_t *sw_table = type_ptr->normalized;
            ecs_type_t sw_type = sw_table->type;

            ecs_entity_t *sw_array = ecs_vector_first(sw_type, ecs_entity_t);
            int32_t sw_array_count = ecs_vector_count(sw_type);

            ecs_switch_t *sw = flecs_switch_new(
                sw_array[0], 
                sw_array[sw_array_count - 1], 
                0);

            storage->sw_columns[i].data = sw;
            storage->sw_columns[i].type = sw_table;
        }
    }

    if (bs_count) {
        storage->bs_columns = ecs_os_calloc_n(ecs_bs_column_t, bs_count);
        for (i = 0; i < bs_count; i ++) {
            flecs_bitset_init(&storage->bs_columns[i].data);
        }
    }
}

static
ecs_flags32_t get_component_action_flags(
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
    ecs_type_t table_type = table->storage_type;
    if (!component || has_component(world, table_type, component)){
        int32_t column_count = ecs_vector_count(table_type);
        ecs_assert(!component || column_count != 0, ECS_INTERNAL_ERROR, NULL);

        if (!column_count) {
            return;
        }
        
        if (!table->c_info) {
            table->c_info = ecs_os_calloc(
                ECS_SIZEOF(ecs_type_info_t*) * column_count);
        }

        /* Reset lifecycle flags before recomputing */
        table->flags &= ~EcsTableHasLifecycle;

        /* Recompute lifecycle flags */
        ecs_entity_t *array = ecs_vector_first(table_type, ecs_entity_t);
        int32_t i;
        for (i = 0; i < column_count; i ++) {
            ecs_id_t id = array[i];
            ecs_entity_t c;

            /* Hardcode components used in bootstrap */
            if (id == ecs_id(EcsComponent)) {
                c = id;
            } else if (ECS_PAIR_FIRST(id) == ecs_id(EcsIdentifier)) {
                c = ecs_id(EcsIdentifier);
            } else {
                c = ecs_get_typeid(world, array[i]);
            }
            ecs_assert(c != 0, ECS_INTERNAL_ERROR, NULL);
            
            const ecs_type_info_t *c_info = flecs_get_c_info(world, c);
            if (c_info) {
                ecs_flags32_t flags = get_component_action_flags(c_info);
                table->flags |= flags;
            }

            /* Store pointer to c_info for fast access */
            table->c_info[i] = (ecs_type_info_t*)c_info;
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

static
void run_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        ecs_ids_t removed = {
            .array = ecs_vector_first(table->type, ecs_id_t),
            .count = ecs_vector_count(table->type)
        };

        ecs_table_diff_t diff = {
            .removed = removed,
            .un_set = removed
        };
        
        flecs_notify_on_remove(world, table, NULL, 0, count, &diff);
    }
}

/* -- Private functions -- */

static
void ctor_component(
    ecs_world_t *world,
    ecs_type_info_t *cdata,
    ecs_column_t *column,
    ecs_entity_t *entities,
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
            flecs_itosize(size), count, ctx);
    }
}

static
void dtor_component(
    ecs_world_t *world,
    ecs_type_info_t *cdata,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count)
{
    if (!count) {
        return;
    }
    
    /* An old component is destructed */
    ecs_xtor_t dtor;
    if (cdata && (dtor = cdata->lifecycle.dtor)) {
        void *ctx = cdata->lifecycle.ctx;
        int16_t size = column->size;
        int16_t alignment = column->alignment;

        ecs_assert(column->data != NULL, ECS_INTERNAL_ERROR, NULL);
        void *ptr = ecs_vector_get_t(column->data, size, alignment, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        dtor(world, cdata->component, &entities[row], ptr,
            flecs_itosize(size), count, ctx);
    }
}

static
void dtor_all_components(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
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
    int32_t column_count = ecs_vector_count(table->storage_type);

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
            for (c = 0; c < column_count; c++) {
                ecs_column_t *column = &data->columns[c];
                dtor_component(world, table->c_info[c], column, entities, i, 1);
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
                    records[i]->table = NULL;
                    records[i]->row = 0;
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
                records[i]->table = NULL;
                records[i]->row = 0;
                (void)e;
            }
        }      
    }
}

static
void fini_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    bool do_on_remove,
    bool update_entity_index,
    bool is_delete,
    bool deactivate)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    if (!data) {
        return;
    }

    ecs_flags32_t flags = table->flags;

    if (do_on_remove && (flags & EcsTableHasOnRemove)) {
        run_on_remove(world, table, data);        
    }

    int32_t count = flecs_table_data_count(data);
    if (count) {
        dtor_all_components(world, table, data, 0, count, 
            update_entity_index, is_delete);
    }

    /* Sanity check */
    ecs_assert(ecs_vector_count(data->record_ptrs) == 
        ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = ecs_vector_count(table->storage_type);
        for (c = 0; c < column_count; c ++) {
            /* Sanity check */
            ecs_assert(!columns[c].data || (ecs_vector_count(columns[c].data) == 
                ecs_vector_count(data->entities)), ECS_INTERNAL_ERROR, NULL);

            ecs_vector_free(columns[c].data);
        }
        ecs_os_free(columns);
        data->columns = NULL;
    }

    ecs_sw_column_t *sw_columns = data->sw_columns;
    if (sw_columns) {
        int32_t c, column_count = table->sw_column_count;
        for (c = 0; c < column_count; c ++) {
            flecs_switch_free(sw_columns[c].data);
        }
        ecs_os_free(sw_columns);
        data->sw_columns = NULL;
    }

    ecs_bs_column_t *bs_columns = data->bs_columns;
    if (bs_columns) {
        int32_t c, column_count = table->bs_column_count;
        for (c = 0; c < column_count; c ++) {
            flecs_bitset_deinit(&bs_columns[c].data);
        }
        ecs_os_free(bs_columns);
        data->bs_columns = NULL;
    }

    ecs_vector_free(data->entities);
    ecs_vector_free(data->record_ptrs);

    data->entities = NULL;
    data->record_ptrs = NULL;

    if (deactivate && count) {
        flecs_table_set_empty(world, table);
    }
}

/* Cleanup, no OnRemove, don't update entity index, don't deactivate table */
void flecs_table_clear_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    fini_data(world, table, data, false, false, false, false);
}

/* Cleanup, no OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities_silent(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, &table->storage, false, true, false, true);
}

/* Cleanup, run OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, &table->storage, true, true, false, true);
}

/* Cleanup, run OnRemove, delete from entity index, deactivate table */
void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, &table->storage, true, true, true, true);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all UnSet handlers, if any */
void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    run_on_remove(world, table, &table->storage);
}

/* Free table resources. */
void flecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    bool is_root = table == &world->store.root;
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(is_root || table->id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(is_root || flecs_sparse_is_alive(world->store.tables, table->id),
        ECS_INTERNAL_ERROR, NULL);
    (void)world;

    if (--table->refcount != 0) {
        ecs_assert(table->refcount >= 0, ECS_INTERNAL_ERROR, NULL);
        return;
    }

    if (!is_root) {
        flecs_notify_queries(
            world, &(ecs_query_event_t){
                .kind = EcsQueryTableUnmatch,
                .table = table
            });
    }

#ifndef NDEBUG
    char *expr = ecs_type_str(world, table->type);
    ecs_dbg_2("#[green]table#[normal] [%s] deleted with id %d", expr, table->id);
    ecs_os_free(expr);
#endif    

    /* Cleanup data, no OnRemove, delete from entity index, don't deactivate */
    fini_data(world, table, &table->storage, false, true, true, false);

    flecs_table_clear_edges(world, table);

    if (!is_root) {
        flecs_unregister_table(world, table);

        ecs_ids_t ids = {
            .array = ecs_vector_first(table->type, ecs_id_t),
            .count = ecs_vector_count(table->type)
        };

        flecs_hashmap_remove(world->store.table_map, &ids, ecs_table_t*);
    }

    ecs_os_free(table->dirty_state);
    ecs_os_free(table->storage_map);

    if (table->c_info) {
        ecs_os_free(table->c_info);
    }

    if (table->storage_table && table->storage_table != table) {
        flecs_table_free(world, table->storage_table);
    }

    if (!world->is_fini) {
        ecs_assert(!is_root, ECS_INTERNAL_ERROR, NULL);
        flecs_table_free_type(table);
        flecs_sparse_remove(world->store.tables, table->id);
    }
}

/* Free table type. Do this separately from freeing the table as types can be
 * in use by application destructors. */
void flecs_table_free_type(
    ecs_table_t *table)
{
    ecs_vector_free((ecs_vector_t*)table->type);
}

/* Reset a table to its initial state. */
void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    flecs_table_clear_edges(world, table);
}

static
void mark_table_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index)
{
    (void)world;
    if (table->dirty_state) {
        table->dirty_state[index] ++;
    }
}

void flecs_table_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->dirty_state) {
        int32_t index = ecs_search(world, table->storage_table, component, 0);
        ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
        table->dirty_state[index + 1] ++;
    }
}

static
void move_switch_columns(
    ecs_table_t *new_table, 
    ecs_data_t *new_data, 
    int32_t new_index,
    ecs_table_t *old_table, 
    ecs_data_t *old_data, 
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

    ecs_id_t *new_ids = ecs_vector_first(new_type, ecs_id_t);
    ecs_id_t *old_ids = ecs_vector_first(old_type, ecs_id_t);

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_id = new_ids[i_new + offset_new];
        ecs_entity_t old_id = old_ids[i_old + offset_old];

        if (new_id == old_id) {
            ecs_switch_t *old_switch = old_columns[i_old].data;
            ecs_switch_t *new_switch = new_columns[i_new].data;

            flecs_switch_ensure(new_switch, new_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_switch_get(old_switch, old_index + i);
                flecs_switch_set(new_switch, new_index + i, value);
            }
        }

        i_new += new_id <= old_id;
        i_old += new_id >= old_id;
    }
}

static
void move_bitset_columns(
    ecs_table_t *new_table, 
    ecs_data_t *new_data, 
    int32_t new_index,
    ecs_table_t *old_table, 
    ecs_data_t *old_data, 
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

            flecs_bitset_ensure(new_bs, new_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_bitset_get(old_bs, old_index + i);
                flecs_bitset_set(new_bs, new_index + i, value);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

static
void grow_column(
    ecs_world_t *world,
    ecs_entity_t *entities,
    ecs_column_t *column,
    ecs_type_info_t *c_info,
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
            flecs_itosize(size), construct ? new_count : count, 
            c_info->lifecycle.ctx);
        
        /* Move old elements */
        move(world, c_info->component, entities, entities, 
            new_buffer, old_buffer, flecs_itosize(size), count, 
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
                flecs_itosize(size), to_add, c_info->lifecycle.ctx);
        }

        column->data = vec;
    }

    ecs_assert(ecs_vector_size(column->data) == new_size, 
        ECS_INTERNAL_ERROR, NULL);
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

    int32_t cur_count = flecs_table_data_count(data);
    int32_t column_count = ecs_vector_count(table->storage_type);
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = data->columns;
    ecs_sw_column_t *sw_columns = data->sw_columns;
    ecs_bs_column_t *bs_columns = data->bs_columns; 

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
    ecs_type_info_t **c_info_array = table->c_info;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_assert(column->size != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_type_info_t *c_info = NULL;
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
        flecs_switch_addn(sw, to_add);
    }

    /* Add elements to each bitset column */
    for (i = 0; i < bs_column_count; i ++) {
        ecs_bitset_t *bs = &bs_columns[i].data;
        flecs_bitset_addn(bs, to_add);
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(world, table, 0);

    if (!world->is_readonly && !cur_count) {
        flecs_table_set_empty(world, table);
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

int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record,
    bool construct)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    int32_t count = ecs_vector_count(data->entities);
    int32_t size = ecs_vector_size(data->entities);
    int32_t column_count = ecs_vector_count(table->storage_type);
    ecs_column_t *columns = table->storage.columns;
    
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
    mark_table_dirty(world, table, 0);
    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(table->flags & EcsTableIsComplex)) {
        fast_append(columns, column_count);
        if (!count) {
            flecs_table_set_empty(world, table); /* See below */
        }
        return count;
    }

    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_sw_column_t *sw_columns = table->storage.sw_columns;
    ecs_bs_column_t *bs_columns = table->storage.bs_columns;

    ecs_type_info_t **c_info_array = table->c_info;
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
        ecs_assert(column->size != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_type_info_t *c_info = NULL;
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
        ecs_assert(sw_columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_switch_t *sw = sw_columns[i].data;
        flecs_switch_add(sw);
    }

    /* Add element to each bitset column */
    for (i = 0; i < bs_column_count; i ++) {
        ecs_assert(bs_columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_bitset_t *bs = &bs_columns[i].data;
        flecs_bitset_addn(bs, 1);
    }    

    /* If this is the first entity in this table, signal queries so that the
     * table moves from an inactive table to an active table. */
    if (!count) {
        flecs_table_set_empty(world, table);
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
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

        int16_t alignment = column->alignment;
        ecs_vector_remove_t(column->data, size, alignment, index);
    }
}

void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
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
            uint32_t row_flags = record_to_move->row & ECS_ROW_FLAGS_MASK;
            record_to_move->row = ECS_ROW_TO_RECORD(index, row_flags);
            ecs_assert(record_to_move->table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(record_to_move->table == table, ECS_INTERNAL_ERROR, NULL);
        }
    }     

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(world, table, 0);    

    /* If table is empty, deactivate it */
    if (!count) {
        flecs_table_set_empty(world, table);
    }

    /* Destruct component data */
    ecs_type_info_t **c_info_array = table->c_info;
    ecs_column_t *columns = data->columns;
    int32_t column_count = ecs_vector_count(table->storage_type);
    int32_t i;

    /* If this is a table without lifecycle callbacks or special columns, take
     * fast path that just remove an element from the array(s) */
    if (!(table->flags & EcsTableIsComplex)) {
        if (index == count) {
            fast_delete_last(columns, column_count);
        } else {
            fast_delete(columns, column_count, index);
        }

        return;
    }

    /* Last element, destruct & remove */
    if (index == count) {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & EcsTableHasDtors)) {
            ecs_assert(c_info_array != NULL, ECS_INTERNAL_ERROR, NULL);
            
            for (i = 0; i < column_count; i ++) {
                ecs_type_info_t *c_info = c_info_array[i];
                ecs_xtor_t dtor;
                if (c_info && (dtor = c_info->lifecycle.dtor)) {
                    ecs_size_t size = c_info->size;
                    ecs_size_t alignment = c_info->alignment;
                    dtor(world, c_info->component, &entity_to_delete,
                        ecs_vector_last_t(columns[i].data, size, alignment),
                        flecs_itosize(size), 1, c_info->lifecycle.ctx);
                }        
            }
        }

        fast_delete_last(columns, column_count);

    /* Not last element, move last element to deleted element & destruct */
    } else {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & (EcsTableHasDtors | EcsTableHasMove))) {
            ecs_assert(c_info_array != NULL, ECS_INTERNAL_ERROR, NULL);

            for (i = 0; i < column_count; i ++) {
                ecs_column_t *column = &columns[i];
                ecs_size_t size = column->size;
                ecs_size_t align = column->alignment;
                ecs_vector_t *vec = column->data;
                void *dst = ecs_vector_get_t(vec, size, align, index);
                void *src = ecs_vector_last_t(vec, size, align);
                
                ecs_type_info_t *c_info = c_info_array[i];
                ecs_move_ctor_t move_dtor;
                if (c_info && (move_dtor = c_info->lifecycle.move_dtor)) {
                    move_dtor(world, c_info->component, &c_info->lifecycle,
                        &entity_to_move, &entity_to_delete, dst, src, 
                        flecs_itosize(size), 1, c_info->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }

                ecs_vector_remove_last(vec);
            }

        } else {
            fast_delete(columns, column_count, index);
        }
    }

    /* Remove elements from switch columns */
    ecs_sw_column_t *sw_columns = data->sw_columns;
    int32_t sw_column_count = table->sw_column_count;
    for (i = 0; i < sw_column_count; i ++) {
        flecs_switch_remove(sw_columns[i].data, index);
    }

    /* Remove elements from bitset columns */
    ecs_bs_column_t *bs_columns = data->bs_columns;
    int32_t bs_column_count = table->bs_column_count;
    for (i = 0; i < bs_column_count; i ++) {
        flecs_bitset_remove(&bs_columns[i].data, index);
    }
}

static
void fast_move(
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index)
{
    ecs_type_t new_type = new_table->storage_type;
    ecs_type_t old_type = old_table->storage_type;

    int32_t i_new = 0, new_column_count = ecs_vector_count(new_table->storage_type);
    int32_t i_old = 0, old_column_count = ecs_vector_count(old_table->storage_type);
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
            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

            int16_t alignment = new_column->alignment;
            void *dst = ecs_vector_get_t(
                new_column->data, size, alignment, new_index);
            void *src = ecs_vector_get_t(
                old_column->data, size, alignment, old_index);

            ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_os_memcpy(dst, src, size);
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

void flecs_table_move(
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
        fast_move(new_table, new_data, new_index, old_table, old_data, old_index);
        return;
    }

    move_switch_columns(
        new_table, new_data, new_index, old_table, old_data, old_index, 1);

    move_bitset_columns(
        new_table, new_data, new_index, old_table, old_data, old_index, 1);

    bool same_entity = dst_entity == src_entity;

    ecs_type_t new_type = new_table->storage_type;
    ecs_type_t old_type = old_table->storage_type;

    int32_t i_new = 0, new_column_count = ecs_vector_count(new_table->storage_type);
    int32_t i_old = 0, old_column_count = ecs_vector_count(old_table->storage_type);
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

            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

            void *dst = ecs_vector_get_t(
                new_column->data, size, alignment, new_index);
            void *src = ecs_vector_get_t(
                old_column->data, size, alignment, old_index);

            ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_type_info_t *cdata = new_table->c_info[i_new];
            if (same_entity) {
                ecs_move_ctor_t callback;
                if (cdata && (callback = cdata->lifecycle.ctor_move_dtor)) {
                    void *ctx = cdata->lifecycle.ctx;
                    /* ctor + move + dtor */
                    callback(world, new_component, &cdata->lifecycle, 
                        &dst_entity, &src_entity, 
                        dst, src, flecs_itosize(size), 1, ctx);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            } else {
                ecs_copy_ctor_t copy;
                if (cdata && (copy = cdata->lifecycle.copy_ctor)) {
                    void *ctx = cdata->lifecycle.ctx;
                    copy(world, new_component, &cdata->lifecycle, 
                        &dst_entity, &src_entity, 
                        dst, src, flecs_itosize(size), 1, ctx);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            }
        } else {
            if (new_component < old_component) {
                if (construct) {
                    ctor_component(world, new_table->c_info[i_new],
                        &new_columns[i_new], &dst_entity, new_index, 1);
                }
            } else {
                dtor_component(world, old_table->c_info[i_old],
                    &old_columns[i_old], &src_entity, old_index, 1);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }

    if (construct) {
        for (; (i_new < new_column_count); i_new ++) {
            ctor_component(world, new_table->c_info[i_new],
                &new_columns[i_new], &dst_entity, new_index, 1);
        }
    }

    for (; (i_old < old_column_count); i_old ++) {
        dtor_component(world, old_table->c_info[i_old],
            &old_columns[i_old], &src_entity, old_index, 1);
    }
}

int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t to_add,
    const ecs_entity_t *ids)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    int32_t cur_count = flecs_table_data_count(data);
    return grow_data(world, table, data, to_add, cur_count + to_add, ids);
}

void flecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t size)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    int32_t cur_count = flecs_table_data_count(data);

    if (cur_count < size) {
        grow_data(world, table, data, 0, size, NULL);
    }
}

int32_t flecs_table_data_count(
    const ecs_data_t *data)
{
    return data ? ecs_vector_count(data->entities) : 0;
}

static
void swap_switch_columns(
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2)
{
    int32_t i = 0, column_count = table->sw_column_count;
    if (!column_count) {
        return;
    }

    ecs_sw_column_t *columns = data->sw_columns;

    for (i = 0; i < column_count; i ++) {
        ecs_switch_t *sw = columns[i].data;
        flecs_switch_swap(sw, row_1, row_2);
    }
}

static
void swap_bitset_columns(
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2)
{
    int32_t i = 0, column_count = table->bs_column_count;
    if (!column_count) {
        return;
    }

    ecs_bs_column_t *columns = data->bs_columns;

    for (i = 0; i < column_count; i ++) {
        ecs_bitset_t *bs = &columns[i].data;
        flecs_bitset_swap(bs, row_1, row_2);
    }
}

void flecs_table_swap(
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
    mark_table_dirty(world, table, 0);    

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_record_t *record_ptr_1 = record_ptrs[row_1];
    ecs_record_t *record_ptr_2 = record_ptrs[row_2];

    ecs_assert(record_ptr_1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record_ptr_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Keep track of whether entity is watched */
    uint32_t flags_1 = ECS_RECORD_TO_ROW_FLAGS(record_ptr_1->row);
    uint32_t flags_2 = ECS_RECORD_TO_ROW_FLAGS(record_ptr_2->row);

    /* Swap entities & records */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = ECS_ROW_TO_RECORD(row_2, flags_1);
    record_ptr_2->row = ECS_ROW_TO_RECORD(row_1, flags_2);
    record_ptrs[row_1] = record_ptr_2;
    record_ptrs[row_2] = record_ptr_1;

    swap_switch_columns(table, data, row_1, row_2);
    swap_bitset_columns(table, data, row_1, row_2);  

    ecs_column_t *columns = data->columns;
    if (!columns) {
        return;
    }

    /* Swap columns */
    int32_t i, column_count = ecs_vector_count(table->storage_type);
    
    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        int16_t alignment = columns[i].alignment;

        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

        void *ptr = ecs_vector_first_t(columns[i].data, size, alignment);
        void *tmp = ecs_os_alloca(size);

        void *el_1 = ECS_OFFSET(ptr, size * row_1);
        void *el_2 = ECS_OFFSET(ptr, size * row_2);

        ecs_os_memcpy(tmp, el_1, size);
        ecs_os_memcpy(el_1, el_2, size);
        ecs_os_memcpy(el_2, tmp, size);
    }  
}

static
void merge_vector(
    ecs_vector_t **dst_out,
    ecs_vector_t *src,
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
void merge_column(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t column_id,
    ecs_vector_t *src)
{
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_type_info_t *c_info = table->c_info[column_id];
    ecs_column_t *column = &data->columns[column_id];
    ecs_vector_t *dst = column->data;
    int16_t size = column->size;
    int16_t alignment = column->alignment;
    int32_t dst_count = ecs_vector_count(dst);

    if (!dst_count) {
        if (dst) {
            ecs_vector_free(dst);
        }

        column->data = src;
    
    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = ecs_vector_count(src);
        ecs_vector_set_count_t(&dst, size, alignment, dst_count + src_count);
        column->data = dst;

        /* Construct new values */
        if (c_info) {
            ctor_component(
                world, c_info, column, entities, dst_count, src_count);
        }
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        
        /* Move values into column */
        ecs_move_t move;
        if (c_info && (move = c_info->lifecycle.move)) {
            move(world, c_info->component, entities, entities, 
                dst_ptr, src_ptr, flecs_itosize(size), src_count, 
                c_info->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);
        }

        ecs_vector_free(src);
    }
}

static
void merge_table_data(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    int32_t old_count,
    int32_t new_count,
    ecs_data_t *old_data,
    ecs_data_t *new_data)
{
    ecs_type_t new_type = new_table->storage_type;
    ecs_type_t old_type = old_table->storage_type;
    int32_t i_new = 0, new_column_count = ecs_vector_count(new_type);
    int32_t i_old = 0, old_column_count = ecs_vector_count(old_type);
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    if (!new_columns && !new_data->entities) {
        new_columns = new_data->columns;
    }
    
    ecs_assert(!new_column_count || new_columns, ECS_INTERNAL_ERROR, NULL);

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

    for (; (i_new < new_column_count) && (i_old < old_column_count); ) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];
        int16_t size = new_columns[i_new].size;
        int16_t alignment = new_columns[i_new].alignment;
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

        if (new_component == old_component) {
            merge_column(world, new_table, new_data, i_new, 
                old_columns[i_old].data);
            old_columns[i_old].data = NULL;

            /* Mark component column as dirty */
            mark_table_dirty(world, new_table, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            /* New column does not occur in old table, make sure vector is large
             * enough. */
            ecs_column_t *column = &new_columns[i_new];
            ecs_vector_set_count_t(&column->data, size, alignment,
                old_count + new_count);

            /* Construct new values */
            ecs_type_info_t *c_info = new_table->c_info[i_new];
            if (c_info) {
                ctor_component(world, c_info, column, 
                    entities, 0, old_count + new_count);
            }
            
            i_new ++;
        } else if (new_component > old_component) {
            ecs_column_t *column = &old_columns[i_old];
            
            /* Destruct old values */
            ecs_type_info_t *c_info = old_table->c_info[i_old];
            if (c_info) {
                dtor_component(world, c_info, column, 
                    entities, 0, old_count);
            }

            /* Old column does not occur in new table, remove */
            ecs_vector_free(column->data);
            column->data = NULL;

            i_old ++;
        }
    }

    move_switch_columns(
        new_table, new_data, new_count, old_table, old_data, 0, old_count);

    /* Initialize remaining columns */
    for (; i_new < new_column_count; i_new ++) {
        ecs_column_t *column = &new_columns[i_new];
        int16_t size = column->size;
        int16_t alignment = column->alignment;
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

        ecs_vector_set_count_t(&column->data, size, alignment,
            old_count + new_count);

        /* Construct new values */
        ecs_type_info_t *c_info = new_table->c_info[i_new];
        if (c_info) {
            ctor_component(world, c_info, column, 
                entities, 0, old_count + new_count);
        }
    }

    /* Destroy remaining columns */
    for (; i_old < old_column_count; i_old ++) {
        ecs_column_t *column = &old_columns[i_old];

        /* Destruct old values */
        ecs_type_info_t *c_info = old_table->c_info[i_old];
        if (c_info) {
            dtor_component(world, c_info, column, entities, 
                0, old_count);
        }

        /* Old column does not occur in new table, remove */
        ecs_vector_free(column->data);
        column->data = NULL;
    }    

    /* Mark entity column as dirty */
    mark_table_dirty(world, new_table, 0); 
}

int32_t ecs_table_count(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return flecs_table_data_count(&table->storage);
}

void flecs_table_merge(
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
        flecs_table_clear_data(world, old_table, old_data);
        return;
    } else {
        ecs_assert(!new_table->lock, ECS_LOCKED_STORAGE, NULL);
    }

    /* If there is no data to merge, drop out */
    if (!old_data) {
        return;
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

        uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
        record->row = ECS_ROW_TO_RECORD(new_count + i, flags);
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
        flecs_table_set_empty(world, new_table);
    }
}

void flecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = &table->storage;
    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    prev_count = ecs_vector_count(table_data->entities);
    run_on_remove(world, table, table_data);
    flecs_table_clear_data(world, table, table_data);

    if (data) {
        table->storage = *data;
    } else {
        flecs_table_init_data(world, table);
    }

    int32_t count = ecs_table_count(table);

    if (!prev_count && count) {
        flecs_table_set_empty(world, table);
    } else if (prev_count && !count) {
        flecs_table_set_empty(world, table);
    }

    table->alloc_count ++;
}

int32_t* flecs_table_get_dirty_state(
    ecs_table_t *table)
{    
    if (!table->dirty_state) {
        int32_t column_count = ecs_vector_count(table->storage_type);
        table->dirty_state = ecs_os_malloc_n( int32_t, column_count + 1);
        ecs_assert(table->dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        
        for (int i = 0; i < column_count + 1; i ++) {
            table->dirty_state[i] = 1;
        }
    }
    return table->dirty_state;
}

int32_t* flecs_table_get_monitor(
    ecs_table_t *table)
{
    int32_t *dirty_state = flecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = ecs_vector_count(table->storage_type);
    return ecs_os_memdup(dirty_state, (column_count + 1) * ECS_SIZEOF(int32_t));
}

void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t *event)
{
    if (world->is_fini) {
        return;
    }

    switch(event->kind) {
    case EcsTableComponentInfo:
        notify_component_info(world, table, event->component);
        break;
    case EcsTableTriggersForId:
        notify_trigger(world, table, event->event);
        break;
    case EcsTableNoTriggersForId:
        break;
    }
}

void ecs_table_lock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (table) {
        if (ecs_poly_is(world, ecs_world_t) && !world->is_readonly) {
            table->lock ++;
        }
    }
}

void ecs_table_unlock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (table) {
        if (ecs_poly_is(world, ecs_world_t) && !world->is_readonly) {
            table->lock --;
            ecs_assert(table->lock >= 0, ECS_INVALID_OPERATION, NULL);
        }
    }
}

bool ecs_table_has_module(
    ecs_table_t *table)
{
    return table->flags & EcsTableHasModule;
}

ecs_column_t* ecs_table_column_for_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    ecs_table_t *storage_table = table->storage_table;
    if (!storage_table) {
        return NULL;
    }

    ecs_table_record_t *tr = flecs_get_table_record(world, storage_table, id);
    if (tr) {
        return &table->storage.columns[tr->column];
    }

    return NULL;
}

ecs_type_t ecs_table_get_type(
    const ecs_table_t *table)
{
    if (table) {
        return table->type;
    } else {
        return NULL;
    }
}

ecs_table_t* ecs_table_get_storage_table(
    const ecs_table_t *table)
{
    return table->storage_table;
}

int32_t ecs_table_storage_count(
    const ecs_table_t *table)
{
    return ecs_vector_count(table->storage_type);
}

int32_t ecs_table_type_to_storage_index(
    const ecs_table_t *table,
    int32_t index)
{
    ecs_assert(index >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);
    int32_t *storage_map = table->storage_map;
    if (storage_map) {
        return storage_map[index];
    }
error:
    return -1;
}

int32_t ecs_table_storage_to_type_index(
    const ecs_table_t *table,
    int32_t index)
{
    ecs_check(index < ecs_vector_count(table->storage_type), 
        ECS_INVALID_PARAMETER, NULL);
    ecs_check(table->storage_map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t offset = ecs_vector_count(table->type);
    return table->storage_map[offset + index];
error:
    return -1;
}

ecs_record_t* ecs_record_find(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = ecs_eis_get(world, entity);
    if (r) {
        return r;
    }
error:
    return NULL;
}

void* ecs_record_get_column(
    ecs_record_t *r,
    int32_t column,
    size_t c_size)
{
    (void)c_size;
    ecs_table_t *table = r->table;

    ecs_check(column < ecs_vector_count(table->storage_type), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_column_t *c = &table->storage.columns[column];
    ecs_assert(c != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_check(!flecs_utosize(c_size) || 
        flecs_utosize(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    return ecs_vector_get_t(c->data, c->size, c->alignment, 
        ECS_RECORD_TO_ROW(r->row));
error:
    return NULL;
}
