#include "private_api.h"

/* Table sanity check to detect storage issues. Only enabled in SANITIZE mode as
 * this can severly slow down many ECS operations. */
#ifdef FLECS_SANITIZE
static
void check_table_sanity(ecs_table_t *table) {
    int32_t size = ecs_vector_size(table->data.entities);
    int32_t count = ecs_vector_count(table->data.entities);
    
    ecs_assert(size == ecs_vector_size(table->data.record_ptrs), 
        ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count == ecs_vector_count(table->data.record_ptrs), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    int32_t sw_offset = table->sw_column_offset;
    int32_t sw_count = table->sw_column_count;
    int32_t bs_offset = table->bs_column_offset;
    int32_t bs_count = table->bs_column_count;
    int32_t type_count = table->type.count;
    ecs_id_t *ids = table->type.array;

    ecs_assert((sw_count + sw_offset) <= type_count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert((bs_count + bs_offset) <= type_count, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *storage_table = table->storage_table;
    if (storage_table) {
        ecs_assert(table->storage_count == storage_table->type.count,
            ECS_INTERNAL_ERROR, NULL);
        ecs_assert(table->storage_ids == storage_table->type.array, 
            ECS_INTERNAL_ERROR, NULL);

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

        ecs_assert(table->data.columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(table->type_info != NULL, ECS_INTERNAL_ERROR, NULL);

        for (i = 0; i < storage_count; i ++) {
            ecs_type_info_t *ti = table->type_info[i];
            ecs_column_t *column = &table->data.columns[i];

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
        ecs_assert(table->data.sw_columns != NULL, 
            ECS_INTERNAL_ERROR, NULL);
        for (i = 0; i < sw_count; i ++) {
            ecs_sw_column_t *sw = &table->data.sw_columns[i];
            ecs_assert(ecs_vector_count(sw->data.values) == count, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ECS_PAIR_FIRST(ids[i + sw_offset]) == EcsUnion,
                ECS_INTERNAL_ERROR, NULL);
        }
    }

    if (bs_count) {
        ecs_assert(table->data.bs_columns != NULL, 
            ECS_INTERNAL_ERROR, NULL);
        for (i = 0; i < bs_count; i ++) {
            ecs_bs_column_t *bs = &table->data.bs_columns[i];
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
void flecs_table_init_storage_map(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!table->storage_table) {
        return;
    }

    ecs_type_t type = table->type;
    ecs_id_t *ids = type.array;
    int32_t t, ids_count = type.count;
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
ecs_flags32_t flecs_type_info_flags(
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
void flecs_table_init_type_info(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->storage_table == table, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type_info == NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_record_t *records = table->records;
    int32_t i, count = table->type.count;
    table->type_info = ecs_os_calloc_n(ecs_type_info_t*, count);

    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        
        /* All ids in the storage table must be components with type info */
        const ecs_type_info_t *ti = idr->type_info;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        table->flags |= flecs_type_info_flags(ti);
        table->type_info[i] = (ecs_type_info_t*)ti;
    }
}

static
void flecs_table_init_storage_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (table->storage_table) {
        return;
    }

    ecs_type_t type = table->type;
    int32_t i, count = type.count;
    ecs_id_t *ids = type.array;
    ecs_table_record_t *records = table->records;

    ecs_id_t array[ECS_ID_CACHE_SIZE];
    ecs_type_t storage_ids = { .array = array };
    if (count > ECS_ID_CACHE_SIZE) {
        storage_ids.array = ecs_os_malloc_n(ecs_id_t, count);
    }

    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        ecs_id_t id = ids[i];

        if (idr->type_info != NULL) {
            storage_ids.array[storage_ids.count ++] = id;
        }
    }

    if (storage_ids.count && storage_ids.count != count) {
        ecs_table_t *storage_table = flecs_table_find_or_create(world, 
            &storage_ids);
        table->storage_table = storage_table;
        table->storage_count = flecs_ito(uint16_t, storage_ids.count);
        table->storage_ids = storage_table->type.array;
        table->type_info = storage_table->type_info;
        table->flags |= storage_table->flags;
        storage_table->refcount ++;
    } else if (storage_ids.count) {
        table->storage_table = table;
        table->storage_count = flecs_ito(uint16_t, count);
        table->storage_ids = type.array;
        flecs_table_init_type_info(table);
    }

    if (storage_ids.array != array) {
        ecs_os_free(storage_ids.array);
    }

    if (!table->storage_map) {
        flecs_table_init_storage_map(table);
    }
}

void flecs_table_init_data(
    ecs_table_t *table)
{
    int32_t sw_count = table->sw_column_count;
    int32_t bs_count = table->bs_column_count;

    ecs_data_t *storage = &table->data;
    int32_t i, count = table->storage_count;

    /* Root tables don't have columns */
    if (!count && !sw_count && !bs_count) {
        storage->columns = NULL;
    }

    if (count) {
        storage->columns = ecs_os_calloc_n(ecs_column_t, count);
    }

    if (sw_count) {
        storage->sw_columns = ecs_os_calloc_n(ecs_sw_column_t, sw_count);
        for (i = 0; i < sw_count; i ++) {
            flecs_switch_init( &storage->sw_columns[i].data, 0);
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
void flecs_table_init_flags(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_id_t *ids = table->type.array;
    int32_t count = table->type.count;

    /* Iterate components to initialize table flags */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        /* As we're iterating over the table components, also set the table
         * flags. These allow us to quickly determine if the table contains
         * data that needs to be handled in a special way. */

        if (id <= EcsLastInternalComponentId) {
            table->flags |= EcsTableHasBuiltins;
        }

        if (id == EcsModule) {
            table->flags |= EcsTableHasBuiltins;
            table->flags |= EcsTableHasModule;
        } else if (id == EcsPrefab) {
            table->flags |= EcsTableIsPrefab;
        } else if (id == EcsDisabled) {
            table->flags |= EcsTableIsDisabled;
        } else {
            ecs_entity_t role = id & ECS_ROLE_MASK;
            if (role == ECS_PAIR) {
                ecs_entity_t r = ECS_PAIR_FIRST(id);

                table->flags |= EcsTableHasPairs;

                if (r == EcsIsA) {
                    table->flags |= EcsTableHasIsA;
                } else if (r == EcsChildOf) {
                    table->flags |= EcsTableHasChildOf;
                    ecs_entity_t obj = ecs_pair_second(world, id);
                    ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

                    if (obj == EcsFlecs || obj == EcsFlecsCore || 
                        ecs_has_id(world, obj, EcsModule)) 
                    {
                        /* If table contains entities that are inside one of the 
                         * builtin modules, it contains builtin entities */
                        table->flags |= EcsTableHasBuiltins;
                        table->flags |= EcsTableHasModule;
                    }
                } else if (r == EcsUnion) {
                    table->flags |= EcsTableHasUnion;

                    if (!table->sw_column_count) {
                        table->sw_column_offset = flecs_ito(int16_t, i);
                    }
                    table->sw_column_count ++;
                }
            } else if (role == ECS_DISABLED) {
                table->flags |= EcsTableHasDisabled;

                if (!table->bs_column_count) {
                    table->bs_column_offset = flecs_ito(int16_t, i);
                }
                table->bs_column_count ++;
            } else if (role == ECS_OVERRIDE) {
                table->flags |= EcsTableHasOverrides;
            }
        } 
    }
}

void flecs_table_init(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *from)
{
    flecs_table_init_flags(world, table);

    int32_t dst_i = 0, dst_count = table->type.count;
    int32_t src_i = 0, src_count = 0;
    ecs_id_t *dst_ids = table->type.array;
    ecs_id_t *src_ids = NULL;
    ecs_table_record_t *tr = NULL, *src_tr = NULL;
    if (from) {
        src_count = from->type.count;
        src_ids = from->type.array;
        src_tr = from->records;
    }

    /* We don't know in advance how large the records array will be, so use
     * cached vector. This eliminates unnecessary allocations, and/or expensive
     * iterations to determine how many records we need. */
    ecs_vector_t *records = world->store.records;
    ecs_vector_clear(records);
    ecs_id_record_t *idr;

    int32_t last_id = -1; /* Track last regular (non-pair) id */
    int32_t first_pair = -1; /* Track the first pair in the table */
    int32_t first_role = -1; /* Track first id with role */

    /* Scan to find boundaries of regular ids, pairs and roles */
    for (dst_i = 0; dst_i < dst_count; dst_i ++) {
        ecs_id_t dst_id = dst_ids[dst_i];
        ecs_entity_t role = dst_id & ECS_ROLE_MASK;
        if (first_pair == -1 && ECS_HAS_ROLE(dst_id, PAIR)) {
            first_pair = dst_i;
        }
        if ((dst_id & ECS_COMPONENT_MASK) == dst_id) {
            last_id = dst_i;
        } else if (first_role == -1 && role != ECS_PAIR) {
            first_role = dst_i;
        }
    }

    /* The easy part: initialize a record for every id in the type */
    for (dst_i = 0; (dst_i < dst_count) && (src_i < src_count); ) {
        ecs_id_t dst_id = dst_ids[dst_i];
        ecs_id_t src_id = src_ids[src_i];

        idr = NULL;

        if (dst_id == src_id) {
            idr = (ecs_id_record_t*)src_tr[src_i].hdr.cache;
        } else if (dst_id < src_id) {
            idr = flecs_ensure_id_record(world, dst_id);
        }
        if (idr) {
            tr = ecs_vector_add(&records, ecs_table_record_t);
            tr->hdr.cache = (ecs_table_cache_t*)idr;
            tr->column = dst_i;
            tr->count = 1;
        }

        dst_i += dst_id <= src_id;
        src_i += dst_id >= src_id;
    }

    /* Add remaining ids that the "from" table didn't have */
    for (; (dst_i < dst_count); dst_i ++) {
        ecs_id_t dst_id = dst_ids[dst_i];
        tr = ecs_vector_add(&records, ecs_table_record_t);
        idr = flecs_ensure_id_record(world, dst_id);
        tr->hdr.cache = (ecs_table_cache_t*)idr;
        ecs_assert(tr->hdr.cache != NULL, ECS_INTERNAL_ERROR, NULL);
        tr->column = dst_i;
        tr->count = 1;
    }

    /* Add records for ids with roles (used by cleanup logic) */
    if (first_role != -1) {
        for (dst_i = first_role; dst_i < dst_count; dst_i ++) {
            ecs_id_t id = dst_ids[dst_i];
            ecs_entity_t role = id & ECS_ROLE_MASK;
            if (role != ECS_PAIR) {
                id &= ECS_COMPONENT_MASK;
                id = ecs_pair(id, EcsWildcard);
                tr = ecs_vector_add(&records, ecs_table_record_t);
                tr->hdr.cache = (ecs_table_cache_t*)flecs_ensure_id_record(
                    world, id);
                tr->column = dst_i;
                tr->count = 1;
            }
        }
    }

    int32_t last_pair = -1;
    int32_t first_tgt_wc = -1;
    int32_t tgt_wc_count = 0;
    bool has_childof = table->flags & EcsTableHasChildOf;
    if (first_pair != -1) {
        /* Add a (Relation, *) record for each relationship. */
        ecs_entity_t r = 0;
        for (dst_i = first_pair; dst_i < dst_count; dst_i ++) {
            ecs_id_t dst_id = dst_ids[dst_i];
            if (!ECS_HAS_ROLE(dst_id, PAIR)) {
                break; /* no more pairs */
            }
            if (r != ECS_PAIR_FIRST(dst_id)) { /* New relation, new record */
                tr = ecs_vector_get(records, ecs_table_record_t, dst_i);
                idr = ((ecs_id_record_t*)tr->hdr.cache)->parent; /* (R, *) */
                ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

                tr = ecs_vector_add(&records, ecs_table_record_t);
                tr->hdr.cache = (ecs_table_cache_t*)idr;
                tr->column = dst_i;
                tr->count = 0;
                r = ECS_PAIR_FIRST(dst_id);
            }

            ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
            tr->count ++;
        }

        last_pair = dst_i;

        /* Add a (*, Target) record for each relationship target. Type
         * ids are sorted relation-first, so we can't simply do a single linear 
         * scan to find all occurrences for a target. */

        /* We're going to insert records from the vector into the index that
         * will get patched up later. To ensure the record pointers don't get
         * invalidated we need to grow the vector so that it won't realloc as
         * we're adding the next set of records */

        int wildcard_count = 3; /* for *, _ and (*, *) */
        wildcard_count += dst_count && !has_childof; /* for (ChildOf, 0) */
        
        ecs_vector_set_min_size(&records, ecs_table_record_t,   
            ecs_vector_count(records) + wildcard_count +
                (last_pair - first_pair));

        for (dst_i = first_pair; dst_i < last_pair; dst_i ++) {
            ecs_id_t dst_id = dst_ids[dst_i];
            ecs_id_t tgt_id = ecs_pair(EcsWildcard, ECS_PAIR_SECOND(dst_id));

            /* To avoid a quadratic search, use the O(1) lookup that the index
             * already provides. */
            idr = flecs_ensure_id_record(world, tgt_id);
            tr = (ecs_table_record_t*)flecs_id_record_table(idr, table);
            if (!tr) {
                tr = ecs_vector_add(&records, ecs_table_record_t);
                tr->column = dst_i;
                tr->count = 1;

                ecs_table_cache_insert(&idr->cache, table, &tr->hdr);

                if (first_tgt_wc == -1) {
                    first_tgt_wc = ecs_vector_count(records) - 1;
                }

                tgt_wc_count ++;
            } else {
                tr->count ++;
            }

            ecs_assert(tr->hdr.cache != NULL, ECS_INTERNAL_ERROR, NULL);
        }
    }

    /* Lastly, add records for all-wildcard ids */
    if (last_id >= 0) {
        tr = ecs_vector_add(&records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_wildcard;
        tr->column = 0;
        tr->count = last_id + 1;
    }
    if (last_pair - first_pair) {
        tr = ecs_vector_add(&records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_wildcard_wildcard;
        tr->column = first_pair;
        tr->count = last_pair - first_pair;
    }
    if (dst_count) {
        tr = ecs_vector_add(&records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_any;
        tr->column = 0;
        tr->count = 1;
    }
    if (dst_count && !has_childof) {
        tr = ecs_vector_add(&records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_childof_0;
        tr->column = 0;
        tr->count = 1;
    }

    /* Now that all records have been added, copy them to array */
    int32_t i, dst_record_count = ecs_vector_count(records);
    ecs_table_record_t *dst_tr =  ecs_os_memdup_n( ecs_vector_first(records, 
        ecs_table_record_t), ecs_table_record_t, dst_record_count);
    table->record_count = flecs_ito(uint16_t, dst_record_count);
    table->records = dst_tr;

    /* Register & patch up records */
    for (i = 0; i < dst_record_count; i ++) {
        tr = &dst_tr[i];
        idr = (ecs_id_record_t*)dst_tr[i].hdr.cache;
        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (i >= first_tgt_wc && i < (first_tgt_wc + tgt_wc_count)) {
            /* If this is a target wildcard record it has already been 
             * registered, but the record is now at a different location in
             * memory. Patch up the linked list with the new address */
            ecs_table_cache_replace(&idr->cache, table, &tr->hdr);
        } else {
            /* Other records are not registered yet */
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_table_cache_insert(&idr->cache, table, &tr->hdr);
        }

        /* Initialize event flags */
        table->flags |= idr->flags & EcsIdEventMask;
    }

    world->store.records = records;

    flecs_table_init_storage_table(world, table);
    flecs_table_init_data(table); 
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
        ecs_table_diff_t diff = {
            .removed = table->type,
            .un_set = table->type
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
            ecs_type_info_t *ti = table->type_info[c];
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
                dtor_component(table->type_info[c], column, i, 1);
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
            flecs_switch_fini(&sw_columns[c].data);
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
    fini_data(world, table, &table->data, false, true, false, true);
}

/* Cleanup, run OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, &table->data, true, true, false, true);
}

/* Cleanup, run OnRemove, delete from entity index, deactivate table */
void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, &table->data, true, true, true, true);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all UnSet handlers, if any */
void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    run_on_remove(world, table, &table->data);
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
        char *expr = ecs_type_str(world, &table->type);
        ecs_dbg_2(
            "#[green]table#[normal] [%s] #[red]deleted#[normal] with id %d", 
            expr, table->id);
        ecs_os_free(expr);
        ecs_log_push_2();
    }

    world->info.empty_table_count -= (ecs_table_count(table) == 0);

    /* Cleanup data, no OnRemove, delete from entity index, don't deactivate */
    fini_data(world, table, &table->data, false, true, true, false);

    flecs_table_clear_edges(world, table);

    if (!is_root) {
        ecs_type_t ids = {
            .array = table->type.array,
            .count = table->type.count
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

    ecs_log_pop_2();
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
    ecs_os_free(table->type.array);
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
    int32_t dst_index,
    ecs_table_t *src_table, 
    int32_t src_index,
    int32_t count,
    bool clear)
{
    int32_t i_old = 0, src_column_count = src_table->sw_column_count;
    int32_t i_new = 0, dst_column_count = dst_table->sw_column_count;

    if (!src_column_count && !dst_column_count) {
        return;
    }

    ecs_sw_column_t *src_columns = src_table->data.sw_columns;
    ecs_sw_column_t *dst_columns = dst_table->data.sw_columns;

    ecs_type_t dst_type = dst_table->type;
    ecs_type_t src_type = src_table->type;

    int32_t offset_new = dst_table->sw_column_offset;
    int32_t offset_old = src_table->sw_column_offset;

    ecs_id_t *dst_ids = dst_type.array;
    ecs_id_t *src_ids = src_type.array;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_entity_t dst_id = dst_ids[i_new + offset_new];
        ecs_entity_t src_id = src_ids[i_old + offset_old];

        if (dst_id == src_id) {
            ecs_switch_t *src_switch = &src_columns[i_old].data;
            ecs_switch_t *dst_switch = &dst_columns[i_new].data;

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
            ecs_switch_t *src_switch = &src_columns[i_old].data;
            ecs_assert(count == flecs_switch_count(src_switch), 
                ECS_INTERNAL_ERROR, NULL);
            flecs_switch_clear(src_switch);
        }
    }
}

static
void move_bitset_columns(
    ecs_table_t *dst_table, 
    int32_t dst_index,
    ecs_table_t *src_table, 
    int32_t src_index,
    int32_t count,
    bool clear)
{
    int32_t i_old = 0, src_column_count = src_table->bs_column_count;
    int32_t i_new = 0, dst_column_count = dst_table->bs_column_count;

    if (!src_column_count && !dst_column_count) {
        return;
    }

    ecs_bs_column_t *src_columns = src_table->data.bs_columns;
    ecs_bs_column_t *dst_columns = dst_table->data.bs_columns;

    ecs_type_t dst_type = dst_table->type;
    ecs_type_t src_type = src_table->type;

    int32_t offset_new = dst_table->bs_column_offset;
    int32_t offset_old = src_table->bs_column_offset;

    ecs_id_t *dst_components = dst_type.array;
    ecs_id_t *src_components = src_type.array;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_id_t dst_component = dst_components[i_new + offset_new];
        ecs_id_t src_component = src_components[i_old + offset_old];

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
    ecs_assert(column != NULL, ECS_INTERNAL_ERROR, NULL);
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
    ecs_type_info_t **type_info = table->type_info;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_type_info_t *ti = type_info[i];
        grow_column(column, ti, to_add, size, true);
        ecs_assert(ecs_vector_size(columns[i].data) == size, 
            ECS_INTERNAL_ERROR, NULL);
    }

    /* Add elements to each switch column */
    for (i = 0; i < sw_column_count; i ++) {
        ecs_switch_t *sw = &sw_columns[i].data;
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

    /* Return index of first added entity */
    return cur_count;
}

static
void fast_append(
    ecs_type_info_t **type_info,
    ecs_column_t *columns,
    int32_t count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_type_info_t *ti = type_info[i];
        ecs_column_t *column = &columns[i];
        ecs_vector_add_t(&column->data, ti->size, ti->alignment);
    }
}

int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_record_t *record,
    bool construct)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(table);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    ecs_data_t *data = &table->data;
    int32_t count = ecs_vector_count(data->entities);
    int32_t column_count = table->storage_count;
    ecs_column_t *columns = table->data.columns;
    
    /* Grow buffer with entity ids, set new element to new entity */
    ecs_entity_t *e = ecs_vector_add(&data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;

    /* Add record ptr to array with record ptrs */
    ecs_record_t **r = ecs_vector_add(&data->record_ptrs, ecs_record_t*);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    *r = record;
 
    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(world, table, 0);
    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_type_info_t **type_info = table->type_info;

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
    int32_t size = ecs_vector_size(data->entities);

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_type_info_t *ti = type_info[i];        
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
        ecs_switch_t *sw = &sw_columns[i].data;
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
    ecs_type_info_t **type_info,
    ecs_column_t *columns,
    int32_t column_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_type_info_t *ti = type_info[i];
        ecs_column_t *column = &columns[i];
        ecs_vector_remove_t(column->data, ti->size, ti->alignment, index);
    }
}

void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool destruct)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(table);

    ecs_data_t *data = &table->data;
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
    ecs_type_info_t **type_info = table->type_info;
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
                remove_component(world, table, type_info[i], &columns[i], 
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
                ecs_type_info_t *ti = type_info[i];
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
        flecs_switch_remove(&sw_columns[i].data, index);
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
    int32_t dst_index,
    ecs_table_t *src_table,
    int32_t src_index)
{
    int32_t i_new = 0, dst_column_count = dst_table->storage_count;
    int32_t i_old = 0, src_column_count = src_table->storage_count;
    ecs_id_t *dst_ids = dst_table->storage_ids;
    ecs_id_t *src_ids = src_table->storage_ids;

    ecs_column_t *src_columns = src_table->data.columns;
    ecs_column_t *dst_columns = dst_table->data.columns;

    ecs_type_info_t **dst_type_info = dst_table->type_info;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_id_t dst_id = dst_ids[i_new];
        ecs_id_t src_id = src_ids[i_old];

        if (dst_id == src_id) {
            ecs_column_t *dst_column = &dst_columns[i_new];
            ecs_column_t *src_column = &src_columns[i_old];
            ecs_type_info_t *ti = dst_type_info[i_new];
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
    int32_t dst_index,
    ecs_table_t *src_table,
    int32_t src_index,
    bool construct)
{
    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!dst_table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(!src_table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_assert(src_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dst_index >= 0, ECS_INTERNAL_ERROR, NULL);

    check_table_sanity(dst_table);
    check_table_sanity(src_table);

    if (!((dst_table->flags | src_table->flags) & EcsTableIsComplex)) {
        fast_move(dst_table, dst_index, src_table, src_index);
        check_table_sanity(dst_table);
        check_table_sanity(src_table);
        return;
    }

    move_switch_columns(dst_table, dst_index, src_table, src_index, 1, false);
    move_bitset_columns(dst_table, dst_index, src_table, src_index, 1, false);

    bool same_entity = dst_entity == src_entity;

    ecs_type_info_t **dst_type_info = dst_table->type_info;
    ecs_type_info_t **src_type_info = src_table->type_info;

    int32_t i_new = 0, dst_column_count = dst_table->storage_count;
    int32_t i_old = 0, src_column_count = src_table->storage_count;
    ecs_id_t *dst_ids = dst_table->storage_ids;
    ecs_id_t *src_ids = src_table->storage_ids;

    ecs_column_t *src_columns = src_table->data.columns;
    ecs_column_t *dst_columns = dst_table->data.columns;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_id_t dst_id = dst_ids[i_new];
        ecs_id_t src_id = src_ids[i_old];

        if (dst_id == src_id) {
            ecs_column_t *dst_column = &dst_columns[i_new];
            ecs_column_t *src_column = &src_columns[i_old];
            ecs_type_info_t *ti = dst_type_info[i_new];
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
                    add_component(world, dst_table, dst_type_info[i_new],
                        &dst_columns[i_new], &dst_entity, dst_id, 
                            dst_index, 1);
                }
            } else {
                remove_component(world, src_table, src_type_info[i_old],
                    &src_columns[i_old], &src_entity, src_id, 
                        src_index, 1);
            }
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }

    if (construct) {
        for (; (i_new < dst_column_count); i_new ++) {
            add_component(world, dst_table, dst_type_info[i_new],
                &dst_columns[i_new], &dst_entity, dst_ids[i_new], dst_index, 1);
        }
    }

    for (; (i_old < src_column_count); i_old ++) {
        remove_component(world, src_table, src_type_info[i_old],
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
    ecs_assert(table != NULL, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(table);

    int32_t cur_count = flecs_table_data_count(data);

    if (cur_count < size) {
        grow_data(world, table, data, 0, size, NULL);
        check_table_sanity(table);
    }
}

bool flecs_table_shrink(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    (void)world;


    check_table_sanity(table);

    ecs_data_t *data = &table->data;
    bool has_payload = data->entities != NULL;
    ecs_vector_reclaim(&data->entities, ecs_entity_t);
    ecs_vector_reclaim(&data->record_ptrs, ecs_record_t*);

    int32_t i, count = table->storage_count;
    ecs_type_info_t **type_info = table->type_info;
    for (i = 0; i < count; i ++) {
        ecs_column_t *column = &data->columns[i];
        ecs_type_info_t *ti = type_info[i];
        ecs_vector_reclaim_t(&column->data, ti->size, ti->alignment);
    }

    return has_payload;
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
        ecs_switch_t *sw = &columns[i].data;
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

    ecs_type_info_t **type_info = table->type_info;

    /* Swap columns */
    int32_t i, column_count = table->storage_count;
    for (i = 0; i < column_count; i ++) {
        ecs_type_info_t *ti = type_info[i];
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

    ecs_type_info_t **dst_type_info = dst_table->type_info;
    ecs_type_info_t **src_type_info = src_table->type_info;

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
        ecs_type_info_t *dst_ti = dst_type_info[i_new];
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
            dtor_component(src_type_info[i_old], column, 0, src_count);

            /* Old column does not occur in new table, remove */
            ecs_vector_free(column->data);
            column->data = NULL;

            i_old ++;
        }
    }

    move_switch_columns(dst_table, dst_count, src_table, 0, src_count, true);
    move_bitset_columns(dst_table, dst_count, src_table, 0, src_count, true);

    /* Initialize remaining columns */
    for (; i_new < dst_column_count; i_new ++) {
        ecs_column_t *column = &dst_columns[i_new];
        ecs_type_info_t *ti = dst_type_info[i_new];
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
        dtor_component(src_type_info[i_old], column, 0, src_count);

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
    return flecs_table_data_count(&table->data);
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
        dst_data = &dst_table->data;
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
    ecs_data_t *table_data = &table->data;
    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    check_table_sanity(table);

    prev_count = ecs_vector_count(table_data->entities);
    run_on_remove(world, table, table_data);
    flecs_table_clear_data(world, table, table_data);

    if (data) {
        table->data = *data;
    } else {
        flecs_table_init_data(table);
    }

    int32_t count = ecs_table_count(table);

    if (!prev_count && count) {
        flecs_table_set_empty(world, table);
    } else if (prev_count && !count) {
        flecs_table_set_empty(world, table);
    }

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
        return &table->data.columns[tr->column];
    }

    return NULL;
}

const ecs_type_t* ecs_table_get_type(
    const ecs_table_t *table)
{
    if (table) {
        return &table->type;
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
    ecs_check(index < table->type.count, ECS_INVALID_PARAMETER, NULL);
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
    int32_t offset = table->type.count;
    return table->storage_map[offset + index];
error:
    return -1;
}

int32_t flecs_table_column_to_union_index(
    const ecs_table_t *table,
    int32_t column)
{
    int32_t sw_count = table->sw_column_count;
    if (sw_count) {
        int32_t sw_offset = table->sw_column_offset;
        if (column >= sw_offset && column < (sw_offset + sw_count)){
            return column - sw_offset;
        }
    }
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
    ecs_type_info_t *ti = table->type_info[column];
    ecs_column_t *c = &table->data.columns[column];
    ecs_assert(c != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_check(!flecs_utosize(c_size) || flecs_utosize(c_size) == ti->size, 
        ECS_INVALID_PARAMETER, NULL);

    return ecs_vector_get_t(c->data, ti->size, ti->alignment, 
        ECS_RECORD_TO_ROW(r->row));
error:
    return NULL;
}
