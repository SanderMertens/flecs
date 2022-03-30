#include "private_api.h"

/* Table sanity check to detect storage issues. Only enabled in SANITIZE mode as
 * this can severly slow down many ECS operations. */
#ifdef FLECS_SANITIZE
static
void check_table_sanity(ecs_table_t *table) {
    int32_t size = ecs_vector_size(table->storage.entities);
    int32_t count = ecs_vector_count(table->storage.entities);
    
    ecs_assert(size == ecs_vector_size(table->storage.record_ptrs), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count == ecs_vector_count(table->storage.record_ptrs), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    int32_t sw_offset = table->sw_column_offset;
    int32_t sw_count = table->sw_column_count;
    int32_t bs_offset = table->bs_column_offset;
    int32_t bs_count = table->bs_column_count;
    int32_t type_count = ecs_vector_count(table->type);
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);

    ecs_assert((sw_count + sw_offset) <= type_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert((bs_count + bs_offset) <= type_count, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *storage_table = table->storage_table;
    if (storage_table) {
        ecs_assert(table->storage_count == ecs_vector_count(storage_table->type),
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(table->storage_ids == ecs_vector_first(
            storage_table->type, ecs_id_t), ECS_INTERNAL_ERROR, NULL);

        int32_t storage_count = table->storage_count;
        ecs_assert(type_count >= storage_count, ECS_INTERNAL_ERROR, NULL);

        int32_t *storage_map = table->storage_map;
        ecs_assert(storage_map != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_id_t *storage_ids = table->storage_ids;
        for (i = 0; i < type_count; i ++) {
            if (storage_map[i] != -1) {
                ecs_assert(ids[i] == storage_ids[storage_map[i]],
                    ECS_INTERNAL_ERROR, NULL);
            }
        }

        for (i = 0; i < storage_count; i ++) {
            ecs_type_info_t *ti = &table->type_info[i];
            ecs_column_t *column = &table->storage.columns[i];

            ecs_vector_assert_size(column->data, ti->size);
            ecs_assert(size == ecs_vector_size(column->data), 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(count == ecs_vector_count(column->data), 
                ECS_INTERNAL_ERROR, NULL);
            int32_t storage_map_id = storage_map[i + type_count];
            ecs_assert(storage_map_id >= 0, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ids[storage_map_id] == storage_ids[i],
                ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        ecs_assert(table->storage_count == 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(table->storage_ids == NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (sw_count) {
        ecs_assert(table->storage.sw_columns != NULL, 
            ECS_INTERNAL_ERROR, NULL);
        for (i = 0; i < sw_count; i ++) {
            ecs_sw_column_t *sw = &table->storage.sw_columns[i];
            ecs_assert(sw->data != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ecs_vector_count(sw->data->values) == count, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert((ids[i + sw_offset] & ECS_ROLE_MASK) == 
                ECS_SWITCH, ECS_INTERNAL_ERROR, NULL);
        }
    }

    if (bs_count) {
        ecs_assert(table->storage.bs_columns != NULL, 
            ECS_INTERNAL_ERROR, NULL);
        for (i = 0; i < bs_count; i ++) {
            ecs_bs_column_t *bs = &table->storage.bs_columns[i];
            ecs_assert(flecs_bitset_count(&bs->data) == count,
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert((ids[i + bs_offset] & ECS_ROLE_MASK) == 
                    ECS_DISABLED, ECS_INTERNAL_ERROR, NULL);
        }
    }
}
#else
#define check_table_sanity(table)
#endif

static
void init_storage_map(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!table->storage_table) {
        return;
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t t, ids_count = ecs_vector_count(type);
    ecs_id_t *storage_ids = table->storage_ids;
    int32_t s, storage_ids_count = table->storage_count;

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
    if (table->storage_table) {
        return;
    }

    ecs_type_t type = table->type;
    int32_t i, count = ecs_vector_count(type);
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    ecs_table_record_t *records = table->records;

    ecs_id_t array[ECS_ID_CACHE_SIZE];
    ecs_ids_t storage_ids = { .array = array };
    if (count > ECS_ID_CACHE_SIZE) {
        storage_ids.array = ecs_os_malloc_n(ecs_id_t, count);
    }

    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        ecs_assert(idr->flags & ECS_TYPE_INFO_INITIALIZED, 
            ECS_INTERNAL_ERROR, NULL);

        if (idr->type_info == NULL) {
            ecs_assert(ecs_get_typeid(world, ids[i]) == 0, 
                ECS_INTERNAL_ERROR, NULL);
            continue; /* not a component */
        }

        storage_ids.array[storage_ids.count ++] = ids[i];
    }
    
    if (storage_ids.count && storage_ids.count != count) {
        ecs_table_t *storage_table = flecs_table_find_or_create(world, 
            &storage_ids);
        table->storage_table = storage_table;
        table->storage_count = flecs_ito(uint16_t, storage_ids.count);
        table->storage_ids = ecs_vector_first(storage_table->type, ecs_id_t);
        storage_table->refcount ++;
    } else if (storage_ids.count) {
        table->storage_table = table;
        table->storage_count = flecs_ito(uint16_t, count);
        table->storage_ids = ecs_vector_first(type, ecs_id_t);
    }

    if (storage_ids.array != array) {
        ecs_os_free(storage_ids.array);
    }

    if (!table->storage_map) {
        init_storage_map(table);
    }
}

static
ecs_flags32_t type_info_flags(
    const ecs_type_info_t *ti) 
{
    ecs_flags32_t flags = 0;

    if (ti->lifecycle.ctor) {
        flags |= EcsTableHasCtors;
    }
    if (ti->lifecycle.on_add) {
        flags |= EcsTableHasCtors;
    }
    if (ti->lifecycle.dtor) {
        flags |= EcsTableHasDtors;
    }
    if (ti->lifecycle.on_remove) {
        flags |= EcsTableHasDtors;
    }
    if (ti->lifecycle.copy) {
        flags |= EcsTableHasCopy;
    }
    if (ti->lifecycle.move) {
        flags |= EcsTableHasMove;
    }  

    return flags;  
}

static
void init_type_info(
    ecs_table_t *table)
{
    ecs_table_t *storage_table = table->storage_table;
    if (!storage_table) {
        return;
    }

    if (storage_table != table) {
        /* Because the storage table is guaranteed to have the same components
         * (but not tags) as this table, we can share the type info cache */
        table->type_info = storage_table->type_info;
        table->flags |= storage_table->flags;
        return;
    }

    if (table->type_info) {
        return;
    }

    ecs_table_record_t *records = table->records;
    int32_t i, count = ecs_vector_count(table->type);
    table->type_info = ecs_os_calloc_n(ecs_type_info_t, count);

    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        ecs_assert(idr->flags & ECS_TYPE_INFO_INITIALIZED, 
            ECS_INTERNAL_ERROR, NULL);
        
        /* All ids in the storage table must be components with type info */
        const ecs_type_info_t *ti = idr->type_info;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        table->flags |= type_info_flags(ti);
        table->type_info[i] = *ti;
    }
}

void flecs_table_init_data(
    ecs_world_t *world,
    ecs_table_t *table)
{
    init_storage_table(world, table);
    init_type_info(table);

    int32_t sw_count = table->sw_column_count;
    int32_t bs_count = table->bs_column_count;

    ecs_data_t *storage = &table->storage;
    int32_t i, count = table->storage_count;

    /* Root tables don't have columns */
    if (!count && !sw_count && !bs_count) {
        storage->columns = NULL;
    }

    if (count) {
        storage->columns = ecs_os_calloc_n(ecs_column_t, count);
    }

    if (sw_count) {
        ecs_entity_t *ids = ecs_vector_first(table->type, ecs_entity_t);
        int32_t sw_offset = table->sw_column_offset;
        storage->sw_columns = ecs_os_calloc_n(ecs_sw_column_t, sw_count);

        for (i = 0; i < sw_count; i ++) {
            ecs_entity_t e = ids[i + sw_offset];
            ecs_assert(ECS_HAS_ROLE(e, SWITCH), ECS_INTERNAL_ERROR, NULL);
            e = e & ECS_COMPONENT_MASK;
            const EcsType *switch_type = ecs_get(world, e, EcsType);
            ecs_assert(switch_type != NULL, 
                ECS_INVALID_PARAMETER, "not a switch");
            ecs_table_t *sw_table = switch_type->normalized;
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
void on_component_callback(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_iter_action_t callback,
    ecs_entity_t event,
    ecs_column_t *column,
    ecs_entity_t *entities,
    ecs_id_t id,
    int32_t row,
    int32_t count,
    ecs_type_info_t *ti)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_iter_t it = { .term_count = 1 };
    it.entities = entities;

    ecs_size_t size = ti->size;
    void *ptr = ecs_vector_get_t(column->data, size, ti->alignment, row);

    flecs_iter_init(&it, flecs_iter_cache_all);
    it.world = world;
    it.real_world = world;
    it.table = table;
    it.type = table->type;
    it.ptrs[0] = ptr;
    it.sizes[0] = size;
    it.ids[0] = id;
    it.event = event;
    it.event_id = id;
    it.ctx = ti->lifecycle.ctx;
    it.count = count;
    flecs_iter_validate(&it);
    callback(&it);
}

static
void ctor_component(
    ecs_type_info_t *ti,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_xtor_t ctor = ti->lifecycle.ctor;
    if (ctor) {
        void *ptr = ecs_vector_get_t(
            column->data, ti->size, ti->alignment, row);
        ctor(ptr, count, ti);
    }
}

static
void add_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_type_info_t *ti,
    ecs_column_t *column,
    ecs_entity_t *entities,
    ecs_id_t id,
    int32_t row,
    int32_t count)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ctor_component(ti, column, row, count);

    ecs_iter_action_t on_add = ti->lifecycle.on_add;
    if (on_add) {
        on_component_callback(world, table, on_add, EcsOnAdd, column,
            entities, id, row, count, ti);
    }
}

static
void dtor_component(
    ecs_type_info_t *ti,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_xtor_t dtor = ti->lifecycle.dtor;
    if (dtor) {
        void *ptr = ecs_vector_get_t(
            column->data, ti->size, ti->alignment, row);
        dtor(ptr, count, ti);
    }
}

static
void remove_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_type_info_t *ti,
    ecs_column_t *column,
    ecs_entity_t *entities,
    ecs_id_t id,
    int32_t row,
    int32_t count)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_iter_action_t on_remove = ti->lifecycle.on_remove;
    if (on_remove) {
        on_component_callback(world, table, on_remove, EcsOnRemove, column,
            entities, id, row, count, ti);
    }
    
    dtor_component(ti, column, row, count);
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

    ecs_id_t *ids = table->storage_ids;
    int32_t ids_count = table->storage_count;
    ecs_record_t **records = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t i, c, end = row + count;

    (void)records;

    /* If table has components with destructors, iterate component columns */
    if (table->flags & EcsTableHasDtors) {
        /* Prevent the storage from getting modified while deleting */
        ecs_defer_begin(world);

        /* Throw up a lock just to be sure */
        table->lock = true;

        /* Run on_remove callbacks in bulk for improved performance */
        for (c = 0; c < ids_count; c++) {
            ecs_column_t *column = &data->columns[c];
            ecs_type_info_t *ti = &table->type_info[c];
            ecs_iter_action_t on_remove = ti->lifecycle.on_remove;
            if (on_remove) {
                on_component_callback(world, table, on_remove, EcsOnRemove, 
                    column, &entities[row], ids[c], row, count, ti);
            }
        }

        /* Iterate entities first, then components. This ensures that only one
         * entity is invalidated at a time, which ensures that destructors can
         * safely access other entities. */
        for (i = row; i < end; i ++) {
            for (c = 0; c < ids_count; c++) {
                ecs_column_t *column = &data->columns[c];
                dtor_component(&table->type_info[c], column, i, 1);
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
                records[i]->row = records[i]->row & ECS_ROW_FLAGS_MASK;
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
        int32_t c, column_count = table->storage_count;
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
            flecs_bitset_fini(&bs_columns[c].data);
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
    ecs_assert(is_root || flecs_sparse_is_alive(&world->store.tables, table->id),
        ECS_INTERNAL_ERROR, NULL);
    (void)world;

    ecs_assert(table->refcount == 0, ECS_INTERNAL_ERROR, NULL);

    if (!is_root) {
        flecs_notify_queries(
            world, &(ecs_query_event_t){
                .kind = EcsQueryTableUnmatch,
                .table = table
            });
    }

    if (ecs_should_log_2()) {
        char *expr = ecs_type_str(world, table->type);
        ecs_dbg_2(
            "#[green]table#[normal] [%s] #[red]deleted#[normal] with id %d", 
            expr, table->id);
        ecs_os_free(expr);
    }

    world->info.empty_table_count -= (ecs_table_count(table) == 0);

    /* Cleanup data, no OnRemove, delete from entity index, don't deactivate */
    fini_data(world, table, &table->storage, false, true, true, false);

    flecs_table_clear_edges(world, table);

    if (!is_root) {
        ecs_ids_t ids = {
            .array = ecs_vector_first(table->type, ecs_id_t),
            .count = ecs_vector_count(table->type)
        };

        flecs_hashmap_remove(&world->store.table_map, &ids, ecs_table_t*);
    }

    ecs_os_free(table->dirty_state);
    ecs_os_free(table->storage_map);

    flecs_table_records_unregister(world, table);

    ecs_table_t *storage_table = table->storage_table;
    if (storage_table == table) {
        if (table->type_info) {
            ecs_os_free(table->type_info);
        }
    } else if (storage_table) {
        flecs_table_release(world, storage_table);
    }

    /* Update counters */
    world->info.table_count --;
    world->info.table_record_count -= table->record_count;
    world->info.table_storage_count -= table->storage_count;
    world->info.table_delete_total ++;
    
    if (!table->storage_count) {
        world->info.tag_table_count --;
    } else {
        world->info.trivial_table_count -= !(table->flags & EcsTableIsComplex);
    }

    if (!world->is_fini) {
        ecs_assert(!is_root, ECS_INTERNAL_ERROR, NULL);
        flecs_table_free_type(table);
        flecs_sparse_remove(&world->store.tables, table->id);
    }
}

void flecs_table_claim(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->refcount > 0, ECS_INTERNAL_ERROR, NULL);
    table->refcount ++;
    (void)world;
}

bool flecs_table_release(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_poly_assert(world, ecs_world_t);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->refcount > 0, ECS_INTERNAL_ERROR, NULL);

    if (--table->refcount == 0) {
        flecs_table_free(world, table);
        return true;
    }
    
    return false;
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
    ecs_table_t *dst_table, 
    ecs_data_t *dst_data, 
    int32_t dst_index,
    ecs_table_t *src_table, 
    ecs_data_t *src_data, 
    int32_t src_index,
    int32_t count,
    bool clear)
{
    int32_t i_old = 0, src_column_count = src_table->sw_column_count;
    int32_t i_new = 0, dst_column_count = dst_table->sw_column_count;

    if (!src_column_count && !dst_column_count) {
        return;
    }

    ecs_sw_column_t *src_columns = src_data->sw_columns;
    ecs_sw_column_t *dst_columns = dst_data->sw_columns;

    ecs_type_t dst_type = dst_table->type;
    ecs_type_t src_type = src_table->type;

    int32_t offset_new = dst_table->sw_column_offset;
    int32_t offset_old = src_table->sw_column_offset;

    ecs_id_t *dst_ids = ecs_vector_first(dst_type, ecs_id_t);
    ecs_id_t *src_ids = ecs_vector_first(src_type, ecs_id_t);

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_entity_t dst_id = dst_ids[i_new + offset_new];
        ecs_entity_t src_id = src_ids[i_old + offset_old];

        if (dst_id == src_id) {
            ecs_switch_t *src_switch = src_columns[i_old].data;
            ecs_switch_t *dst_switch = dst_columns[i_new].data;

            flecs_switch_ensure(dst_switch, dst_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_switch_get(src_switch, src_index + i);
                flecs_switch_set(dst_switch, dst_index + i, value);
            }

            if (clear) {
                ecs_assert(count == flecs_switch_count(src_switch), 
                    ECS_INTERNAL_ERROR, NULL);
                flecs_switch_clear(src_switch);
            }
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }

    /* Clear remaining columns */
    if (clear) {
        for (; (i_old < src_column_count); i_old ++) {
            ecs_switch_t *src_switch = src_columns[i_old].data;
            ecs_assert(count == flecs_switch_count(src_switch), 
                ECS_INTERNAL_ERROR, NULL);
            flecs_switch_clear(src_switch);
        }
    }
}

static
void move_bitset_columns(
    ecs_table_t *dst_table, 
    ecs_data_t *dst_data, 
    int32_t dst_index,
    ecs_table_t *src_table, 
    ecs_data_t *src_data, 
    int32_t src_index,
    int32_t count,
    bool clear)
{
    int32_t i_old = 0, src_column_count = src_table->bs_column_count;
    int32_t i_new = 0, dst_column_count = dst_table->bs_column_count;

    if (!src_column_count && !dst_column_count) {
        return;
    }

    ecs_bs_column_t *src_columns = src_data->bs_columns;
    ecs_bs_column_t *dst_columns = dst_data->bs_columns;

    ecs_type_t dst_type = dst_table->type;
    ecs_type_t src_type = src_table->type;

    int32_t offset_new = dst_table->bs_column_offset;
    int32_t offset_old = src_table->bs_column_offset;

    ecs_entity_t *dst_components = ecs_vector_first(dst_type, ecs_entity_t);
    ecs_entity_t *src_components = ecs_vector_first(src_type, ecs_entity_t);

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_entity_t dst_component = dst_components[i_new + offset_new];
        ecs_entity_t src_component = src_components[i_old + offset_old];

        if (dst_component == src_component) {
            ecs_bitset_t *src_bs = &src_columns[i_old].data;
            ecs_bitset_t *dst_bs = &dst_columns[i_new].data;

            flecs_bitset_ensure(dst_bs, dst_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_bitset_get(src_bs, src_index + i);
                flecs_bitset_set(dst_bs, dst_index + i, value);
            }

            if (clear) {
                ecs_assert(count == flecs_bitset_count(src_bs), 
                    ECS_INTERNAL_ERROR, NULL);
                flecs_bitset_fini(src_bs);
            }
        }

        i_new += dst_component <= src_component;
        i_old += dst_component >= src_component;
    }

    /* Clear remaining columns */
    if (clear) {
        for (; (i_old < src_column_count); i_old ++) {
            ecs_bitset_t *src_bs = &src_columns[i_old].data;
            ecs_assert(count == flecs_bitset_count(src_bs), 
                ECS_INTERNAL_ERROR, NULL);
            flecs_bitset_fini(src_bs);
        }
    }
}

static
void* grow_column(
    ecs_column_t *column,
    ecs_type_info_t *ti,
    int32_t to_add,
    int32_t dst_size,
    bool construct)
{
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *vec = column->data;
    int32_t size = ti->size;
    int32_t alignment = ti->alignment;
    int32_t count = ecs_vector_count(vec);
    int32_t src_size = ecs_vector_size(vec);
    int32_t dst_count = count + to_add;
    bool can_realloc = dst_size != src_size;
    void *result = NULL;

    ecs_assert(dst_size >= dst_count, ECS_INTERNAL_ERROR, NULL);

    /* If the array could possibly realloc and the component has a move action 
     * defined, move old elements manually */
    ecs_move_t move_ctor;
    if (count && can_realloc && (move_ctor = ti->lifecycle.move_ctor)) 
    {
        ecs_xtor_t ctor = ti->lifecycle.ctor;
        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(move_ctor != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create  vector */
        ecs_vector_t *dst_vec = ecs_vector_new_t(size, alignment, dst_size);
        ecs_vector_set_count_t(&dst_vec, size, alignment, dst_count);

        void *src_buffer = ecs_vector_first_t(vec, size, alignment);
        void *dst_buffer = ecs_vector_first_t(dst_vec, size, alignment);

        /* Move (and construct) existing elements to new vector */
        move_ctor(dst_buffer, src_buffer, count, ti);

        if (construct) {
            /* Construct new element(s) */
            result = ECS_OFFSET(dst_buffer, size * count);
            ctor(result, to_add, ti);
        }

        /* Free old vector */
        ecs_vector_free(vec);

        column->data = dst_vec;
    } else {
        /* If array won't realloc or has no move, simply add new elements */
        if (can_realloc) {
            ecs_vector_set_size_t(&vec, size, alignment, dst_size);
        }

        result = ecs_vector_addn_t(&vec, size, alignment, to_add);

        ecs_xtor_t ctor;
        if (construct && (ctor = ti->lifecycle.ctor)) {
            /* If new elements need to be constructed and component has a
             * constructor, construct */
            ctor(result, to_add, ti);
        }

        column->data = vec;
    }

    ecs_assert(ecs_vector_size(column->data) == dst_size, 
        ECS_INTERNAL_ERROR, NULL);

    return result;
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
    int32_t column_count = table->storage_count;
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
    ecs_type_info_t *type_info = table->type_info;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_type_info_t *ti = &type_info[i];
        grow_column(column, ti, to_add, size, true);
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
    ecs_type_info_t *type_info,
    ecs_column_t *columns,
    int32_t count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_type_info_t *ti = &type_info[i];
        ecs_column_t *column = &columns[i];
        ecs_vector_add_t(&column->data, ti->size, ti->alignment);
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

    check_table_sanity(table);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    int32_t count = ecs_vector_count(data->entities);
    int32_t size = ecs_vector_size(data->entities);
    int32_t column_count = table->storage_count;
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

    ecs_type_info_t *type_info = table->type_info;

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(table->flags & EcsTableIsComplex)) {
        fast_append(type_info, columns, column_count);
        if (!count) {
            flecs_table_set_empty(world, table); /* See below */
        }
        return count;
    }

    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_sw_column_t *sw_columns = data->sw_columns;
    ecs_bs_column_t *bs_columns = data->bs_columns;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    /* Reobtain size to ensure that the columns have the same size as the 
     * entities and record vectors. This keeps reasoning about when allocations
     * occur easier. */
    size = ecs_vector_size(data->entities);

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_type_info_t *ti = &type_info[i];        
        grow_column(column, ti, 1, size, construct);

        ecs_iter_action_t on_add = ti->lifecycle.on_add;
        if (on_add) {
            on_component_callback(world, table, on_add, EcsOnAdd, column,
                &entities[count], table->storage_ids[i], count, 1, ti);
        }

        ecs_assert(ecs_vector_size(columns[i].data) == 
            ecs_vector_size(data->entities), ECS_INTERNAL_ERROR, NULL); 
        ecs_assert( ecs_vector_count(columns[i].data) == 
            ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);                        
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

    check_table_sanity(table);

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
    ecs_type_info_t *type_info,
    ecs_column_t *columns,
    int32_t column_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_type_info_t *ti = &type_info[i];
        ecs_column_t *column = &columns[i];
        ecs_vector_remove_t(column->data, ti->size, ti->alignment, index);
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

    check_table_sanity(table);

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
    ecs_type_info_t *type_info = table->type_info;
    ecs_column_t *columns = data->columns;
    int32_t column_count = table->storage_count;
    int32_t i;

    /* If this is a table without lifecycle callbacks or special columns, take
     * fast path that just remove an element from the array(s) */
    if (!(table->flags & EcsTableIsComplex)) {
        if (index == count) {
            fast_delete_last(columns, column_count);
        } else {
            fast_delete(type_info, columns, column_count, index);
        }

        check_table_sanity(table);
        return;
    }

    ecs_id_t *ids = table->storage_ids;

    /* Last element, destruct & remove */
    if (index == count) {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & EcsTableHasDtors)) {            
            for (i = 0; i < column_count; i ++) {
                remove_component(world, table, &type_info[i], &columns[i], 
                    &entity_to_delete, ids[i], index, 1);
            }
        }

        fast_delete_last(columns, column_count);

    /* Not last element, move last element to deleted element & destruct */
    } else {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & (EcsTableHasDtors | EcsTableHasMove))) {
            for (i = 0; i < column_count; i ++) {
                ecs_column_t *column = &columns[i];
                ecs_type_info_t *ti = &type_info[i];
                ecs_size_t size = ti->size;
                ecs_size_t align = ti->alignment;
                ecs_vector_t *vec = column->data;
                void *dst = ecs_vector_get_t(vec, size, align, index);
                void *src = ecs_vector_last_t(vec, size, align);
                
                ecs_iter_action_t on_remove = ti->lifecycle.on_remove;
                if (on_remove) {
                    on_component_callback(world, table, on_remove, EcsOnRemove,
                        column, &entity_to_delete, ids[i], index, 1, ti);
                }

                ecs_move_t move_dtor = ti->lifecycle.move_dtor;
                if (move_dtor) {
                    move_dtor(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }

                ecs_vector_remove_last(vec);
            }
        } else {
            fast_delete(type_info, columns, column_count, index);
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

    check_table_sanity(table);
}

static
void fast_move(
    ecs_table_t *dst_table,
    ecs_data_t *dst_data,
    int32_t dst_index,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_index)
{
    int32_t i_new = 0, dst_column_count = dst_table->storage_count;
    int32_t i_old = 0, src_column_count = src_table->storage_count;
    ecs_id_t *dst_ids = dst_table->storage_ids;
    ecs_id_t *src_ids = src_table->storage_ids;

    ecs_column_t *src_columns = src_data->columns;
    ecs_column_t *dst_columns = dst_data->columns;

    ecs_type_info_t *dst_type_info = dst_table->type_info;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_id_t dst_id = dst_ids[i_new];
        ecs_id_t src_id = src_ids[i_old];

        if (dst_id == src_id) {
            ecs_column_t *dst_column = &dst_columns[i_new];
            ecs_column_t *src_column = &src_columns[i_old];
            ecs_type_info_t *ti = &dst_type_info[i_new];
            int32_t size = ti->size;
            int32_t alignment = ti->alignment;
            void *dst = ecs_vector_get_t(
                dst_column->data, size, alignment, dst_index);
            void *src = ecs_vector_get_t(
                src_column->data, size, alignment, src_index);

            ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_os_memcpy(dst, src, size);
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }
}

void flecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *dst_table,
    ecs_data_t *dst_data,
    int32_t dst_index,
    ecs_table_t *src_table,
    ecs_data_t *src_data,
    int32_t src_index,
    bool construct)
{
    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!dst_table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(!src_table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_assert(src_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dst_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dst_data != NULL, ECS_INTERNAL_ERROR, NULL);

    check_table_sanity(dst_table);
    check_table_sanity(src_table);

    if (!((dst_table->flags | src_table->flags) & EcsTableIsComplex)) {
        fast_move(dst_table, dst_data, dst_index, src_table, src_data, 
            src_index);
        check_table_sanity(dst_table);
        check_table_sanity(src_table);
        return;
    }

    move_switch_columns(dst_table, dst_data, dst_index, src_table, src_data, 
        src_index, 1, false);
    move_bitset_columns(dst_table, dst_data, dst_index, src_table, src_data, 
        src_index, 1, false);

    bool same_entity = dst_entity == src_entity;

    ecs_type_info_t *dst_type_info = dst_table->type_info;
    ecs_type_info_t *src_type_info = src_table->type_info;

    int32_t i_new = 0, dst_column_count = dst_table->storage_count;
    int32_t i_old = 0, src_column_count = src_table->storage_count;
    ecs_id_t *dst_ids = dst_table->storage_ids;
    ecs_id_t *src_ids = src_table->storage_ids;

    ecs_column_t *src_columns = src_data->columns;
    ecs_column_t *dst_columns = dst_data->columns;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_id_t dst_id = dst_ids[i_new];
        ecs_id_t src_id = src_ids[i_old];

        if (dst_id == src_id) {
            ecs_column_t *dst_column = &dst_columns[i_new];
            ecs_column_t *src_column = &src_columns[i_old];
            ecs_type_info_t *ti = &dst_type_info[i_new];
            int32_t size = ti->size;
            int32_t alignment = ti->alignment;

            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
            void *dst = ecs_vector_get_t(
                dst_column->data, size, alignment, dst_index);
            void *src = ecs_vector_get_t(
                src_column->data, size, alignment, src_index);

            ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

            if (same_entity) {
                ecs_move_t move = ti->lifecycle.ctor_move_dtor;
                if (move) {
                    /* ctor + move + dtor */
                    move(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            } else {
                ecs_copy_t copy = ti->lifecycle.copy_ctor;
                if (copy) {
                    copy(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            }
        } else {
            if (dst_id < src_id) {
                if (construct) {
                    add_component(world, dst_table, &dst_type_info[i_new],
                        &dst_columns[i_new], &dst_entity, dst_id, 
                            dst_index, 1);
                }
            } else {
                remove_component(world, src_table, &src_type_info[i_old],
                    &src_columns[i_old], &src_entity, src_id, 
                        src_index, 1);
            }
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }

    if (construct) {
        for (; (i_new < dst_column_count); i_new ++) {
            add_component(world, dst_table, &dst_type_info[i_new],
                &dst_columns[i_new], &dst_entity, dst_ids[i_new], dst_index, 1);
        }
    }

    for (; (i_old < src_column_count); i_old ++) {
        remove_component(world, src_table, &src_type_info[i_old],
            &src_columns[i_old], &src_entity, src_ids[i_old], 
                src_index, 1);
    }

    check_table_sanity(dst_table);
    check_table_sanity(src_table);
}

int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t to_add,
    const ecs_entity_t *ids)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(table);

    int32_t cur_count = flecs_table_data_count(data);
    int32_t result = grow_data(
        world, table, data, to_add, cur_count + to_add, ids);
    check_table_sanity(table);
    return result;
}

void flecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t size)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(table);

    int32_t cur_count = flecs_table_data_count(data);

    if (cur_count < size) {
        grow_data(world, table, data, 0, size, NULL);
        check_table_sanity(table);
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

    check_table_sanity(table);
    
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
        check_table_sanity(table);
        return;
    }

    ecs_type_info_t *type_info = table->type_info;

    /* Swap columns */
    int32_t i, column_count = table->storage_count;
    for (i = 0; i < column_count; i ++) {
        ecs_type_info_t *ti = &type_info[i];
        int32_t size = ti->size;
        int32_t alignment = ti->alignment;

        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

        void *ptr = ecs_vector_first_t(columns[i].data, size, alignment);
        void *tmp = ecs_os_alloca(size);

        void *el_1 = ECS_OFFSET(ptr, size * row_1);
        void *el_2 = ECS_OFFSET(ptr, size * row_2);

        ecs_os_memcpy(tmp, el_1, size);
        ecs_os_memcpy(el_1, el_2, size);
        ecs_os_memcpy(el_2, tmp, size);
    }

    check_table_sanity(table);
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
    ecs_type_info_t *ti,
    int32_t size,
    int32_t alignment,
    ecs_data_t *data,
    int32_t column_id,
    ecs_vector_t *src)
{
    ecs_column_t *column = &data->columns[column_id];
    ecs_vector_t *dst = column->data;
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
        ctor_component(ti, column, dst_count, src_count);
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        
        /* Move values into column */
        ecs_move_t move = ti->lifecycle.move;
        if (move) {
            move(dst_ptr, src_ptr, src_count, ti);
        } else {
            ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);
        }

        ecs_vector_free(src);
    }
}

static
void merge_table_data(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table,
    int32_t src_count,
    int32_t dst_count,
    ecs_data_t *src_data,
    ecs_data_t *dst_data)
{
    int32_t i_new = 0, dst_column_count = dst_table->storage_count;
    int32_t i_old = 0, src_column_count = src_table->storage_count;
    ecs_id_t *dst_ids = dst_table->storage_ids;
    ecs_id_t *src_ids = src_table->storage_ids;

    ecs_type_info_t *dst_type_info = dst_table->type_info;
    ecs_type_info_t *src_type_info = src_table->type_info;

    ecs_column_t *src_columns = src_data->columns;
    ecs_column_t *dst_columns = dst_data->columns;

    if (!dst_columns && !dst_data->entities) {
        dst_columns = dst_data->columns;
    }
    
    ecs_assert(!dst_column_count || dst_columns, ECS_INTERNAL_ERROR, NULL);

    if (!src_count) {
        return;
    }

    /* Merge entities */
    merge_vector(&dst_data->entities, src_data->entities, ECS_SIZEOF(ecs_entity_t), 
        ECS_ALIGNOF(ecs_entity_t));
    src_data->entities = NULL;
    ecs_assert(ecs_vector_count(dst_data->entities) == src_count + dst_count, 
        ECS_INTERNAL_ERROR, NULL);

    /* Merge entity index record pointers */
    merge_vector(&dst_data->record_ptrs, src_data->record_ptrs, 
        ECS_SIZEOF(ecs_record_t*), ECS_ALIGNOF(ecs_record_t*));
    src_data->record_ptrs = NULL;        

    for (; (i_new < dst_column_count) && (i_old < src_column_count); ) {
        ecs_id_t dst_id = dst_ids[i_new];
        ecs_id_t src_id = src_ids[i_old];
        ecs_type_info_t *dst_ti = &dst_type_info[i_new];
        int32_t size = dst_ti->size;
        int32_t alignment = dst_ti->alignment;
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);

        if (dst_id == src_id) {
            merge_column(dst_ti, size, alignment, dst_data, 
                i_new, src_columns[i_old].data);
            src_columns[i_old].data = NULL;

            /* Mark component column as dirty */
            mark_table_dirty(world, dst_table, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (dst_id < src_id) {
            /* New column does not occur in old table, make sure vector is large
             * enough. */
            ecs_column_t *column = &dst_columns[i_new];
            ecs_vector_set_count_t(&column->data, size, alignment,
                src_count + dst_count);

            /* Construct new values */
            ctor_component(dst_ti, column, 0, src_count + dst_count);
            
            i_new ++;
        } else if (dst_id > src_id) {
            ecs_column_t *column = &src_columns[i_old];
            
            /* Destruct old values */
            dtor_component(&src_type_info[i_old], column, 0, src_count);

            /* Old column does not occur in new table, remove */
            ecs_vector_free(column->data);
            column->data = NULL;

            i_old ++;
        }
    }

    move_switch_columns(dst_table, dst_data, dst_count, src_table, src_data, 0, 
        src_count, true);
    move_bitset_columns(dst_table, dst_data, dst_count, src_table, src_data, 0, 
        src_count, true);

    /* Initialize remaining columns */
    for (; i_new < dst_column_count; i_new ++) {
        ecs_column_t *column = &dst_columns[i_new];
        ecs_type_info_t *ti = &dst_type_info[i_new];
        int32_t size = ti->size;
        int32_t alignment = ti->alignment;
        
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_vector_set_count_t(&column->data, size, alignment,
            src_count + dst_count);

        /* Construct new values */
        ctor_component(ti, column, 0, src_count + dst_count);
    }

    /* Destroy remaining columns */
    for (; i_old < src_column_count; i_old ++) {
        ecs_column_t *column = &src_columns[i_old];

        /* Destruct old values */
        dtor_component(&src_type_info[i_old], column, 0, src_count);

        /* Old column does not occur in new table, remove */
        ecs_vector_free(column->data);
        column->data = NULL;
    }    

    /* Mark entity column as dirty */
    mark_table_dirty(world, dst_table, 0); 
}

int32_t ecs_table_count(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return flecs_table_data_count(&table->storage);
}

void flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table,
    ecs_data_t *dst_data,
    ecs_data_t *src_data)
{
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!src_table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(dst_table);
    check_table_sanity(src_table);
    
    bool move_data = false;
    
    /* If there is nothing to merge to, just clear the old table */
    if (!dst_table) {
        flecs_table_clear_data(world, src_table, src_data);
        check_table_sanity(src_table);
        return;
    } else {
        ecs_assert(!dst_table->lock, ECS_LOCKED_STORAGE, NULL);
    }

    /* If there is no data to merge, drop out */
    if (!src_data) {
        return;
    }

    if (!dst_data) {
        dst_data = &dst_table->storage;
        if (dst_table == src_table) {
            move_data = true;
        }
    }

    ecs_entity_t *src_entities = ecs_vector_first(src_data->entities, ecs_entity_t);
    int32_t src_count = ecs_vector_count(src_data->entities);
    int32_t dst_count = ecs_vector_count(dst_data->entities);

    ecs_record_t **src_records = ecs_vector_first(
        src_data->record_ptrs, ecs_record_t*);

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < src_count; i ++) {
        ecs_record_t *record;
        if (dst_table != src_table) {
            record = src_records[i];
            ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        } else {
            record = ecs_eis_ensure(world, src_entities[i]);
        }

        uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
        record->row = ECS_ROW_TO_RECORD(dst_count + i, flags);
        record->table = dst_table;
    }

    /* Merge table columns */
    if (move_data) {
        *dst_data = *src_data;
    } else {
        merge_table_data(world, dst_table, src_table, src_count, dst_count, 
            src_data, dst_data);
    }

    dst_table->alloc_count ++;

    if (src_count) {
        if (!dst_count) {
            flecs_table_set_empty(world, dst_table);
        }
        flecs_table_set_empty(world, src_table);
    }

    check_table_sanity(src_table);
    check_table_sanity(dst_table);
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

    check_table_sanity(table);

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

    check_table_sanity(table);
}

int32_t* flecs_table_get_dirty_state(
    ecs_table_t *table)
{    
    if (!table->dirty_state) {
        int32_t column_count = table->storage_count;
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

    int32_t column_count = table->storage_count;
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
    ecs_check(index < table->storage_count, ECS_INVALID_PARAMETER, NULL);
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

    ecs_check(column < table->storage_count, ECS_INVALID_PARAMETER, NULL);
    ecs_type_info_t *ti = &table->type_info[column];
    ecs_column_t *c = &table->storage.columns[column];
    ecs_assert(c != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_check(!flecs_utosize(c_size) || flecs_utosize(c_size) == ti->size, 
        ECS_INVALID_PARAMETER, NULL);

    return ecs_vector_get_t(c->data, ti->size, ti->alignment, 
        ECS_RECORD_TO_ROW(r->row));
error:
    return NULL;
}
