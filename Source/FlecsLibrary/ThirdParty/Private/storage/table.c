/**
 * @file storage/table.c
 * @brief Table storage implementation.
 * 
 * Tables are the data structure that store the component data. Tables have
 * columns for each component in the table, and rows for each entity stored in
 * the table. Once created, the component list for a table doesn't change, but
 * entities can move from one table to another.
 * 
 * Each table has a type, which is a vector with the (component) ids in the 
 * table. The vector is sorted by id, which ensures that there can be only one
 * table for each unique combination of components.
 * 
 * Not all ids in a table have to be components. Tags are ids that have no
 * data type associated with them, and as a result don't need to be explicitly
 * stored beyond an element in the table type. To save space and speed up table
 * creation, each table has a reference to a "storage table", which is a table
 * that only includes component ids (so excluding tags).
 * 
 * Note that the actual data is not stored on the storage table. The storage 
 * table is only used for sharing administration. A column_map member maps
 * between column indices of the table and its storage table. Tables are 
 * refcounted, which ensures that storage tables won't be deleted if other
 * tables have references to it.
 */

#include "../private_api.h"

/* Table sanity check to detect storage issues. Only enabled in SANITIZE mode as
 * this can severely slow down many ECS operations. */
#ifdef FLECS_SANITIZE
static
void flecs_table_check_sanity(ecs_table_t *table) {
    int32_t size = ecs_vec_size(&table->data.entities);
    int32_t count = ecs_vec_count(&table->data.entities);

    int32_t i;
    int32_t bs_offset = table->_ ? table->_->bs_offset : 0;
    int32_t bs_count = table->_ ? table->_->bs_count : 0;
    int32_t type_count = table->type.count;
    ecs_id_t *ids = table->type.array;

    ecs_assert((bs_count + bs_offset) <= type_count, ECS_INTERNAL_ERROR, NULL);

    if (table->column_count) {
        int32_t column_count = table->column_count;
        ecs_assert(type_count >= column_count, ECS_INTERNAL_ERROR, NULL);

        int32_t *column_map = table->column_map;
        ecs_assert(column_map != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_assert(table->data.columns != NULL, ECS_INTERNAL_ERROR, NULL);

        for (i = 0; i < column_count; i ++) {
            ecs_vec_t *column = &table->data.columns[i].data;
            ecs_assert(size == column->size, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(count == column->count, ECS_INTERNAL_ERROR, NULL);
            int32_t column_map_id = column_map[i + type_count];
            ecs_assert(column_map_id >= 0, ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        ecs_assert(table->column_map == NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (bs_count) {
        ecs_assert(table->_->bs_columns != NULL, 
            ECS_INTERNAL_ERROR, NULL);
        for (i = 0; i < bs_count; i ++) {
            ecs_bitset_t *bs = &table->_->bs_columns[i];
            ecs_assert(flecs_bitset_count(bs) == count,
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(ECS_HAS_ID_FLAG(ids[i + bs_offset], TOGGLE),
                ECS_INTERNAL_ERROR, NULL);
        }
    }

    ecs_assert((table->_->traversable_count == 0) || 
        (table->flags & EcsTableHasTraversable), ECS_INTERNAL_ERROR, NULL);
}
#else
#define flecs_table_check_sanity(table)
#endif

/* Set flags for type hooks so table operations can quickly check whether a
 * fast or complex operation that invokes hooks is required. */
static
ecs_flags32_t flecs_type_info_flags(
    const ecs_type_info_t *ti) 
{
    ecs_flags32_t flags = 0;

    if (ti->hooks.ctor) {
        flags |= EcsTableHasCtors;
    }
    if (ti->hooks.on_add) {
        flags |= EcsTableHasCtors;
    }
    if (ti->hooks.dtor) {
        flags |= EcsTableHasDtors;
    }
    if (ti->hooks.on_remove) {
        flags |= EcsTableHasDtors;
    }
    if (ti->hooks.copy) {
        flags |= EcsTableHasCopy;
    }
    if (ti->hooks.move) {
        flags |= EcsTableHasMove;
    }  

    return flags;  
}

static
void flecs_table_init_columns(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_count)
{
    if (!column_count) {
        return;
    }

    int32_t i, cur = 0, ids_count = table->type.count;
    ecs_column_t *columns = flecs_wcalloc_n(world, ecs_column_t, column_count);
    table->data.columns = columns;

    ecs_id_t *ids = table->type.array;
    ecs_table_record_t *records = table->_->records;
    int32_t *t2s = table->column_map;
    int32_t *s2t = &table->column_map[ids_count];

    for (i = 0; i < ids_count; i ++) {
        ecs_table_record_t *tr = &records[i];
        ecs_id_record_t *idr = (ecs_id_record_t*)tr->hdr.cache;
        const ecs_type_info_t *ti = idr->type_info;
        if (!ti || (idr->flags & EcsIdIsSparse)) {
            t2s[i] = -1;
            continue;
        }

        t2s[i] = cur;
        s2t[cur] = i;
        tr->column = flecs_ito(int16_t, cur);

        columns[cur].ti = ECS_CONST_CAST(ecs_type_info_t*, ti);
        columns[cur].id = ids[i];
        columns[cur].size = ti->size;

        if (ECS_IS_PAIR(ids[i])) {
            ecs_table_record_t *wc_tr = flecs_id_record_get_table(
                idr->parent, table);
            if (wc_tr->index == tr->index) {
                wc_tr->column = tr->column;
            }
        }

#ifdef FLECS_DEBUG
        ecs_vec_init(NULL, &columns[cur].data, ti->size, 0);
#endif

        table->flags |= flecs_type_info_flags(ti);
        cur ++;
    }
}

/* Initialize table storage */
void flecs_table_init_data(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *storage = &table->data;
    ecs_vec_init_t(NULL, &storage->entities, ecs_entity_t, 0);

    flecs_table_init_columns(world, table, table->column_count);

    ecs_table__t *meta = table->_;
    int32_t i, bs_count = meta->bs_count;

    if (bs_count) {
        meta->bs_columns = flecs_wcalloc_n(world, ecs_bitset_t, bs_count);
        for (i = 0; i < bs_count; i ++) {
            flecs_bitset_init(&meta->bs_columns[i]);
        }
    }
}

/* Initialize table flags. Table flags are used in lots of scenarios to quickly
 * check the features of a table without having to inspect the table type. Table
 * flags are typically used to early-out of potentially expensive operations. */
static
void flecs_table_init_flags(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_id_t *ids = table->type.array;
    int32_t count = table->type.count;

    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

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
        } else if (id == EcsNotQueryable) {
            table->flags |= EcsTableNotQueryable;
        } else {
            if (ECS_IS_PAIR(id)) {
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
                } else if (id == ecs_pair_t(EcsIdentifier, EcsName)) {
                    table->flags |= EcsTableHasName;
                } else if (r == ecs_id(EcsPoly)) {
                    table->flags |= EcsTableHasBuiltins;
                }
            } else {
                if (ECS_HAS_ID_FLAG(id, TOGGLE)) {
                    ecs_table__t *meta = table->_;
                    table->flags |= EcsTableHasToggle;

                    if (!meta->bs_count) {
                        meta->bs_offset = flecs_ito(int16_t, i);
                    }
                    meta->bs_count ++;
                }
                if (ECS_HAS_ID_FLAG(id, AUTO_OVERRIDE)) {
                    table->flags |= EcsTableHasOverrides;
                }
            }
        } 
    }
}

/* Utility function that appends an element to the table record array */
static
void flecs_table_append_to_records(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_vec_t *records,
    ecs_id_t id,
    int32_t column)
{
    /* To avoid a quadratic search, use the O(1) lookup that the index
     * already provides. */
    ecs_id_record_t *idr = flecs_id_record_ensure(world, id);
    ecs_table_record_t *tr = (ecs_table_record_t*)flecs_id_record_get_table(
            idr, table);
    if (!tr) {
        tr = ecs_vec_append_t(&world->allocator, records, ecs_table_record_t);
        tr->index = flecs_ito(int16_t, column);
        tr->count = 1;

        ecs_table_cache_insert(&idr->cache, table, &tr->hdr);
    } else {
        tr->count ++;
    }

    ecs_assert(tr->hdr.cache != NULL, ECS_INTERNAL_ERROR, NULL);
}

void flecs_table_emit(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t event)
{
    flecs_emit(world, world, 0, &(ecs_event_desc_t) {
        .ids = &table->type,
        .event = event,
        .table = table,
        .flags = EcsEventTableOnly,
        .observable = world
    });
}

/* Main table initialization function */
void flecs_table_init(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_t *from)
{
    /* Make sure table->flags is initialized */
    flecs_table_init_flags(world, table);

    /* The following code walks the table type to discover which id records the
     * table needs to register table records with. 
     *
     * In addition to registering itself with id records for each id in the
     * table type, a table also registers itself with wildcard id records. For
     * example, if a table contains (Eats, Apples), it will register itself with
     * wildcard id records (Eats, *),  (*, Apples) and (*, *). This makes it
     * easier for wildcard queries to find the relevant tables. */

    int32_t dst_i = 0, dst_count = table->type.count;
    int32_t src_i = 0, src_count = 0;
    ecs_id_t *dst_ids = table->type.array;
    ecs_id_t *src_ids = NULL;
    ecs_table_record_t *tr = NULL, *src_tr = NULL;
    if (from) {
        src_count = from->type.count;
        src_ids = from->type.array;
        src_tr = from->_->records;
    }

    /* We don't know in advance how large the records array will be, so use
     * cached vector. This eliminates unnecessary allocations, and/or expensive
     * iterations to determine how many records we need. */
    ecs_allocator_t *a = &world->allocator;
    ecs_vec_t *records = &world->store.records;
    ecs_vec_reset_t(a, records, ecs_table_record_t);
    ecs_id_record_t *idr, *childof_idr = NULL;

    int32_t last_id = -1; /* Track last regular (non-pair) id */
    int32_t first_pair = -1; /* Track the first pair in the table */
    int32_t first_role = -1; /* Track first id with role */

    /* Scan to find boundaries of regular ids, pairs and roles */
    for (dst_i = 0; dst_i < dst_count; dst_i ++) {
        ecs_id_t dst_id = dst_ids[dst_i];
        if (first_pair == -1 && ECS_IS_PAIR(dst_id)) {
            first_pair = dst_i;
        }
        if ((dst_id & ECS_COMPONENT_MASK) == dst_id) {
            last_id = dst_i;
        } else if (first_role == -1 && !ECS_IS_PAIR(dst_id)) {
            first_role = dst_i;
        }
    }

    /* The easy part: initialize a record for every id in the type */
    for (dst_i = 0; (dst_i < dst_count) && (src_i < src_count); ) {
        ecs_id_t dst_id = dst_ids[dst_i];
        ecs_id_t src_id = src_ids[src_i];

        idr = NULL;

        if (dst_id == src_id) {
            ecs_assert(src_tr != NULL, ECS_INTERNAL_ERROR, NULL);
            idr = (ecs_id_record_t*)src_tr[src_i].hdr.cache;
        } else if (dst_id < src_id) {
            idr = flecs_id_record_ensure(world, dst_id);
        }
        if (idr) {
            tr = ecs_vec_append_t(a, records, ecs_table_record_t);
            tr->hdr.cache = (ecs_table_cache_t*)idr;
            tr->index = flecs_ito(int16_t, dst_i);
            tr->count = 1;
        }

        dst_i += dst_id <= src_id;
        src_i += dst_id >= src_id;
    }

    /* Add remaining ids that the "from" table didn't have */
    for (; (dst_i < dst_count); dst_i ++) {
        ecs_id_t dst_id = dst_ids[dst_i];
        tr = ecs_vec_append_t(a, records, ecs_table_record_t);
        idr = flecs_id_record_ensure(world, dst_id);
        tr->hdr.cache = (ecs_table_cache_t*)idr;
        ecs_assert(tr->hdr.cache != NULL, ECS_INTERNAL_ERROR, NULL);
        tr->index = flecs_ito(int16_t, dst_i);
        tr->count = 1;
    }

    /* We're going to insert records from the vector into the index that
     * will get patched up later. To ensure the record pointers don't get
     * invalidated we need to grow the vector so that it won't realloc as
     * we're adding the next set of records */
    if (first_role != -1 || first_pair != -1) {
        int32_t start = first_role;
        if (first_pair != -1 && (start != -1 || first_pair < start)) {
            start = first_pair;
        }

        /* Total number of records can never be higher than
         * - number of regular (non-pair) ids +
         * - three records for pairs: (R,T), (R,*), (*,T)
         * - one wildcard (*), one any (_) and one pair wildcard (*,*) record
         * - one record for (ChildOf, 0)
         */
        int32_t flag_id_count = dst_count - start;
        int32_t record_count = start + 3 * flag_id_count + 3 + 1;
        ecs_vec_set_min_size_t(a, records, ecs_table_record_t, record_count);
    }

    /* Add records for ids with roles (used by cleanup logic) */
    if (first_role != -1) {
        for (dst_i = first_role; dst_i < dst_count; dst_i ++) {
            ecs_id_t id = dst_ids[dst_i];
            if (!ECS_IS_PAIR(id)) {
                ecs_entity_t first = 0;
                ecs_entity_t second = 0;
                if (ECS_HAS_ID_FLAG(id, PAIR)) {
                    first = ECS_PAIR_FIRST(id);
                    second = ECS_PAIR_SECOND(id);
                } else {
                    first = id & ECS_COMPONENT_MASK;
                }
                if (first) {
                    flecs_table_append_to_records(world, table, records, 
                        ecs_pair(EcsFlag, first), dst_i);
                }
                if (second) {
                    flecs_table_append_to_records(world, table, records, 
                        ecs_pair(EcsFlag, second), dst_i);
                }
            }
        }
    }

    int32_t last_pair = -1;
    bool has_childof = table->flags & EcsTableHasChildOf;
    if (first_pair != -1) {
        /* Add a (Relationship, *) record for each relationship. */
        ecs_entity_t r = 0;
        for (dst_i = first_pair; dst_i < dst_count; dst_i ++) {
            ecs_id_t dst_id = dst_ids[dst_i];
            if (!ECS_IS_PAIR(dst_id)) {
                break; /* no more pairs */
            }
            if (r != ECS_PAIR_FIRST(dst_id)) { /* New relationship, new record */
                tr = ecs_vec_get_t(records, ecs_table_record_t, dst_i);

                ecs_id_record_t *p_idr = (ecs_id_record_t*)tr->hdr.cache;
                r = ECS_PAIR_FIRST(dst_id);
                if (r == EcsChildOf) {
                    childof_idr = p_idr;
                }

                idr = p_idr->parent; /* (R, *) */
                ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

                tr = ecs_vec_append_t(a, records, ecs_table_record_t);
                tr->hdr.cache = (ecs_table_cache_t*)idr;
                tr->index = flecs_ito(int16_t, dst_i);
                tr->count = 0;
            }

            ecs_assert(tr != NULL, ECS_INTERNAL_ERROR, NULL);
            tr->count ++;
        }

        last_pair = dst_i;

        /* Add a (*, Target) record for each relationship target. Type
         * ids are sorted relationship-first, so we can't simply do a single 
         * linear scan to find all occurrences for a target. */
        for (dst_i = first_pair; dst_i < last_pair; dst_i ++) {
            ecs_id_t dst_id = dst_ids[dst_i];
            ecs_id_t tgt_id = ecs_pair(EcsWildcard, ECS_PAIR_SECOND(dst_id));

            flecs_table_append_to_records(
                world, table, records, tgt_id, dst_i);
        }
    }

    /* Lastly, add records for all-wildcard ids */
    if (last_id >= 0) {
        tr = ecs_vec_append_t(a, records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_wildcard;
        tr->index = 0;
        tr->count = flecs_ito(int16_t, last_id + 1);
    }
    if (last_pair - first_pair) {
        tr = ecs_vec_append_t(a, records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_wildcard_wildcard;
        tr->index = flecs_ito(int16_t, first_pair);
        tr->count = flecs_ito(int16_t, last_pair - first_pair);
    }
    if (dst_count) {
        tr = ecs_vec_append_t(a, records, ecs_table_record_t);
        tr->hdr.cache = (ecs_table_cache_t*)world->idr_any;
        tr->index = 0;
        tr->count = 1;
    }
    if (dst_count && !has_childof) {
        tr = ecs_vec_append_t(a, records, ecs_table_record_t);
        childof_idr = world->idr_childof_0;
        tr->hdr.cache = (ecs_table_cache_t*)childof_idr;
        tr->index = 0;
        tr->count = 1;
    }

    /* Now that all records have been added, copy them to array */
    int32_t i, dst_record_count = ecs_vec_count(records);
    ecs_table_record_t *dst_tr = flecs_wdup_n(world, ecs_table_record_t, 
        dst_record_count, ecs_vec_first_t(records, ecs_table_record_t));
    table->_->record_count = flecs_ito(int16_t, dst_record_count);
    table->_->records = dst_tr;
    int32_t column_count = 0;

    /* Register & patch up records */
    for (i = 0; i < dst_record_count; i ++) {
        tr = &dst_tr[i];
        idr = (ecs_id_record_t*)dst_tr[i].hdr.cache;
        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);

        if (ecs_table_cache_get(&idr->cache, table)) {
            /* If this is a target wildcard record it has already been 
             * registered, but the record is now at a different location in
             * memory. Patch up the linked list with the new address */
            ecs_table_cache_replace(&idr->cache, table, &tr->hdr);
        } else {
            /* Other records are not registered yet */
            ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_table_cache_insert(&idr->cache, table, &tr->hdr);
        }

        /* Claim id record so it stays alive as long as the table exists */
        flecs_id_record_claim(world, idr);

        /* Initialize event flags */
        table->flags |= idr->flags & EcsIdEventMask;

        /* Initialize column index (will be overwritten by init_columns) */
        tr->column = -1;

        if (ECS_ID_ON_INSTANTIATE(idr->flags) == EcsOverride) {
            table->flags |= EcsTableHasOverrides;
        }

        if ((i < table->type.count) && (idr->type_info != NULL)) {
            if (!(idr->flags & EcsIdIsSparse)) {
                column_count ++;
            }
        }
    }

    if (column_count) {
        table->column_map = flecs_walloc_n(world, int32_t, 
            dst_count + column_count);
    }

    table->column_count = flecs_ito(int16_t, column_count);
    flecs_table_init_data(world, table);

    if (table->flags & EcsTableHasName) {
        ecs_assert(childof_idr != NULL, ECS_INTERNAL_ERROR, NULL);
        table->_->name_index = 
            flecs_id_record_name_index_ensure(world, childof_idr);
        ecs_assert(table->_->name_index != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    if (table->flags & EcsTableHasOnTableCreate) {
        flecs_table_emit(world, table, EcsOnTableCreate);
    }
}

/* Unregister table from id records */
static
void flecs_table_records_unregister(
    ecs_world_t *world,
    ecs_table_t *table)
{
    uint64_t table_id = table->id;
    int32_t i, count = table->_->record_count;
    for (i = 0; i < count; i ++) {
        ecs_table_record_t *tr = &table->_->records[i];
        ecs_table_cache_t *cache = tr->hdr.cache;
        ecs_id_t id = ((ecs_id_record_t*)cache)->id;

        ecs_assert(tr->hdr.cache == cache, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(tr->hdr.table == table, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(flecs_id_record_get(world, id) == (ecs_id_record_t*)cache,
            ECS_INTERNAL_ERROR, NULL);
        (void)id;

        ecs_table_cache_remove(cache, table_id, &tr->hdr);
        flecs_id_record_release(world, (ecs_id_record_t*)cache);
    }

    flecs_wfree_n(world, ecs_table_record_t, count, table->_->records);
}

/* Keep track for what kind of builtin events observers are registered that can
 * potentially match the table. This allows code to early out of calling the
 * emit function that notifies observers. */
static
void flecs_table_add_trigger_flags(
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
    } else if (event == EcsOnTableFill) {
        table->flags |= EcsTableHasOnTableFill;
    } else if (event == EcsOnTableEmpty) {
        table->flags |= EcsTableHasOnTableEmpty;
    } else if (event == EcsWildcard) {
        table->flags |= EcsTableHasOnAdd|EcsTableHasOnRemove|EcsTableHasOnSet|
            EcsTableHasOnTableFill|EcsTableHasOnTableEmpty|
            EcsTableHasOnTableCreate|EcsTableHasOnTableDelete;
    }
}

/* Invoke OnRemove observers for all entities in table. Useful during table 
 * deletion or when clearing entities from a table. */
static
void flecs_table_notify_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = data->entities.count;
    if (count) {
        flecs_notify_on_remove(world, table, NULL, 0, count, &table->type);
    }
}

/* Invoke type hook for entities in table */
static
void flecs_table_invoke_hook(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_iter_action_t callback,
    ecs_entity_t event,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count)
{
    void *ptr = ecs_vec_get(&column->data, column->size, row);
    flecs_invoke_hook(world, table, count, row, entities, ptr, column->id, 
        column->ti, event, callback);
}

/* Construct components */
static
void flecs_table_invoke_ctor(
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_xtor_t ctor = ti->hooks.ctor;
    if (ctor) {
        void *ptr = ecs_vec_get(&column->data, column->size, row);
        ctor(ptr, count, ti);
    }
}

/* Destruct components */
static
void flecs_table_invoke_dtor(
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_xtor_t dtor = ti->hooks.dtor;
    if (dtor) {
        void *ptr = ecs_vec_get(&column->data, column->size, row);
        dtor(ptr, count, ti);
    }
}

/* Run hooks that get invoked when component is added to entity */
static
void flecs_table_invoke_add_hooks(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count,
    bool construct)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    if (construct) {
        flecs_table_invoke_ctor(column, row, count);
    }

    ecs_iter_action_t on_add = ti->hooks.on_add;
    if (on_add) {
        flecs_table_invoke_hook(world, table, on_add, EcsOnAdd, column,
            entities, row, count);
    }
}

/* Run hooks that get invoked when component is removed from entity */
static
void flecs_table_invoke_remove_hooks(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_column_t *column,
    ecs_entity_t *entities,
    int32_t row,
    int32_t count,
    bool dtor)
{
    ecs_type_info_t *ti = column->ti;
    ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_iter_action_t on_remove = ti->hooks.on_remove;
    if (on_remove) {
        flecs_table_invoke_hook(world, table, on_remove, EcsOnRemove, column,
            entities, row, count);
    }
    
    if (dtor) {
        flecs_table_invoke_dtor(column, row, count);
    }
}

/* Destruct all components and/or delete all entities in table in range */
static
void flecs_table_dtor_all(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    bool is_delete)
{
    int32_t ids_count = table->column_count;
    ecs_entity_t *entities = data->entities.array;
    int32_t i, c, end = row + count;

    if (is_delete && table->_->traversable_count) {
        /* If table contains monitored entities with traversable relationships,
         * make sure to invalidate observer cache */
        flecs_emit_propagate_invalidate(world, table, row, count);
    }

    /* If table has components with destructors, iterate component columns */
    if (table->flags & EcsTableHasDtors) {
        /* Throw up a lock just to be sure */
        table->_->lock = true;

        /* Run on_remove callbacks first before destructing components */
        for (c = 0; c < ids_count; c++) {
            ecs_column_t *column = &data->columns[c];
            ecs_iter_action_t on_remove = column->ti->hooks.on_remove;
            if (on_remove) {
                flecs_table_invoke_hook(world, table, on_remove, EcsOnRemove, 
                    column, &entities[row], row, count);
            }
        }

        /* Destruct components */
        for (c = 0; c < ids_count; c++) {
            flecs_table_invoke_dtor(&data->columns[c], row, count);
        }

        /* Iterate entities first, then components. This ensures that only one
         * entity is invalidated at a time, which ensures that destructors can
         * safely access other entities. */
        for (i = row; i < end; i ++) {
            /* Update entity index after invoking destructors so that entity can
             * be safely used in destructor callbacks. */
            ecs_entity_t e = entities[i];
            ecs_assert(!e || ecs_is_valid(world, e), 
                ECS_INTERNAL_ERROR, NULL);

            if (is_delete) {
                flecs_entities_remove(world, e);
                ecs_assert(ecs_is_valid(world, e) == false, 
                    ECS_INTERNAL_ERROR, NULL);
            } else {
                // If this is not a delete, clear the entity index record
                ecs_record_t *record = flecs_entities_get(world, e);
                record->table = NULL;
                record->row = 0;
            }
        }

        table->_->lock = false;

    /* If table does not have destructors, just update entity index */
    } else {
        if (is_delete) {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                flecs_entities_remove(world, e);
                ecs_assert(!ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
            } 
        } else {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                ecs_record_t *record = flecs_entities_get(world, e);
                record->table = NULL;
                record->row = record->row & ECS_ROW_FLAGS_MASK;
                (void)e;
            }
        }      
    }
}

#define FLECS_LOCKED_STORAGE_MSG \
    "to fix, defer operations with defer_begin/defer_end"

/* Cleanup table storage */
static
void flecs_table_fini_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    bool do_on_remove,
    bool is_delete,
    bool deactivate)
{
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    if (!data) {
        return;
    }

    if (do_on_remove) {
        flecs_table_notify_on_remove(world, table, data);        
    }

    int32_t count = flecs_table_data_count(data);
    if (count) {
        flecs_table_dtor_all(world, table, data, 0, count, is_delete);
    }

    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = table->column_count;
        for (c = 0; c < column_count; c ++) {
            /* Sanity check */
            ecs_assert(columns[c].data.count == data->entities.count,
                ECS_INTERNAL_ERROR, NULL);
            ecs_vec_fini(&world->allocator,
                &columns[c].data, columns[c].size);
        }
        flecs_wfree_n(world, ecs_column_t, column_count, columns);
        data->columns = NULL;
    }

    ecs_table__t *meta = table->_;
    ecs_bitset_t *bs_columns = meta->bs_columns;
    if (bs_columns) {
        int32_t c, column_count = meta->bs_count;
        for (c = 0; c < column_count; c ++) {
            flecs_bitset_fini(&bs_columns[c]);
        }
        flecs_wfree_n(world, ecs_bitset_t, column_count, bs_columns);
        meta->bs_columns = NULL;
    }

    ecs_vec_fini_t(&world->allocator, &data->entities, ecs_entity_t);

    if (deactivate && count) {
        flecs_table_set_empty(world, table);
    }

    table->_->traversable_count = 0;
    table->flags &= ~EcsTableHasTraversable;
}

const ecs_vec_t* flecs_table_entities(
    const ecs_table_t *table)
{
    return &table->data.entities;
}

ecs_entity_t* flecs_table_entities_array(
    const ecs_table_t *table)
{
    return ecs_vec_first(flecs_table_entities(table));
}

/* Cleanup, no OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities_silent(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_table_fini_data(world, table, &table->data, false, false, true);
}

/* Cleanup, run OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_table_fini_data(world, table, &table->data, true, false, true);
}

/* Cleanup, run OnRemove, delete from entity index, deactivate table */
void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_table_fini_data(world, table, &table->data, true, true, true);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all OnRemove handlers, if any */
void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    flecs_table_notify_on_remove(world, table, &table->data);
}

/* Free table resources. */
void flecs_table_fini(
    ecs_world_t *world,
    ecs_table_t *table)
{
    bool is_root = table == &world->store.root;
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(is_root || table->id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(is_root || flecs_sparse_is_alive(&world->store.tables, table->id),
        ECS_INTERNAL_ERROR, NULL);
    (void)world;

    if (!is_root && !(world->flags & EcsWorldQuit)) {
        if (table->flags & EcsTableHasOnTableDelete) {
            flecs_table_emit(world, table, EcsOnTableDelete);
        }
    }

    if (ecs_should_log_2()) {
        char *expr = ecs_type_str(world, &table->type);
        ecs_dbg_2(
            "#[green]table#[normal] [%s] #[red]deleted#[reset] with id %d", 
            expr, table->id);
        ecs_os_free(expr);
        ecs_log_push_2();
    }

    world->info.empty_table_count -= (ecs_table_count(table) == 0);

    /* Cleanup data, no OnRemove, delete from entity index, don't deactivate */
    flecs_table_fini_data(world, table, &table->data, false, true, false);
    flecs_table_clear_edges(world, table);

    if (!is_root) {
        ecs_type_t ids = {
            .array = table->type.array,
            .count = table->type.count
        };

        flecs_hashmap_remove_w_hash(
            &world->store.table_map, &ids, ecs_table_t*, table->_->hash);
    }

    flecs_wfree_n(world, int32_t, table->column_count + 1, table->dirty_state);
    flecs_wfree_n(world, int32_t, table->column_count + table->type.count, 
        table->column_map);
    flecs_table_records_unregister(world, table);

    /* Update counters */
    world->info.table_count --;
    world->info.table_delete_total ++;

    flecs_free_t(&world->allocator, ecs_table__t, table->_);

    if (!(world->flags & EcsWorldFini)) {
        ecs_assert(!is_root, ECS_INTERNAL_ERROR, NULL);
        flecs_table_free_type(world, table);
        flecs_sparse_remove_t(&world->store.tables, ecs_table_t, table->id);
    }

    ecs_log_pop_2();
}

/* Free table type. Do this separately from freeing the table as types can be
 * in use by application destructors. */
void flecs_table_free_type(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_wfree_n(world, ecs_id_t, table->type.count, table->type.array);
}

/* Reset a table to its initial state. */
void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    flecs_table_clear_edges(world, table);
}

/* Keep track of number of traversable entities in table. A traversable entity
 * is an entity used as target in a pair with a traversable relationship. The
 * traversable count and flag are used by code to early out of mechanisms like
 * event propagation and recursive cleanup. */
void flecs_table_traversable_add(
    ecs_table_t *table,
    int32_t value)
{
    int32_t result = table->_->traversable_count += value;
    ecs_assert(result >= 0, ECS_INTERNAL_ERROR, NULL);
    if (result == 0) {
        table->flags &= ~EcsTableHasTraversable;
    } else if (result == value) {
        table->flags |= EcsTableHasTraversable;
    }
}

/* Mark table column dirty. This usually happens as the result of a set 
 * operation, or iteration of a query with [out] fields. */
static
void flecs_table_mark_table_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index)
{
    (void)world;
    if (table->dirty_state) {
        table->dirty_state[index] ++;
    }
}

/* Mark table component dirty */
void flecs_table_mark_dirty(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->dirty_state) {
        ecs_id_record_t *idr = flecs_id_record_get(world, component);
        if (!idr) {
            return;
        }

        const ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
        if (!tr || tr->column == -1) {
            return;
        }

        table->dirty_state[tr->column + 1] ++;
    }
}

/* Get (or create) dirty state of table. Used by queries for change tracking */
int32_t* flecs_table_get_dirty_state(
    ecs_world_t *world,
    ecs_table_t *table)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!table->dirty_state) {
        int32_t column_count = table->column_count;
        table->dirty_state = flecs_alloc_n(&world->allocator,
             int32_t, column_count + 1);
        ecs_assert(table->dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
        for (int i = 0; i < column_count + 1; i ++) {
            table->dirty_state[i] = 1;
        }
    }
    return table->dirty_state;
}

/* Table move logic for bitset (toggle component) column */
static
void flecs_table_move_bitset_columns(
    ecs_table_t *dst_table, 
    int32_t dst_index,
    ecs_table_t *src_table, 
    int32_t src_index,
    int32_t count,
    bool clear)
{
    ecs_table__t *dst_meta = dst_table->_;
    ecs_table__t *src_meta = src_table->_;
    if (!dst_meta && !src_meta) {
        return;
    }

    int32_t i_old = 0, src_column_count = src_meta ? src_meta->bs_count : 0;
    int32_t i_new = 0, dst_column_count = dst_meta ? dst_meta->bs_count : 0;

    if (!src_column_count && !dst_column_count) {
        return;
    }

    ecs_bitset_t *src_columns = src_meta ? src_meta->bs_columns : NULL;
    ecs_bitset_t *dst_columns = dst_meta ? dst_meta->bs_columns : NULL;

    ecs_type_t dst_type = dst_table->type;
    ecs_type_t src_type = src_table->type;

    int32_t offset_new = dst_meta ? dst_meta->bs_offset : 0;
    int32_t offset_old = src_meta ? src_meta->bs_offset : 0;

    ecs_id_t *dst_ids = dst_type.array;
    ecs_id_t *src_ids = src_type.array;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_id_t dst_id = dst_ids[i_new + offset_new];
        ecs_id_t src_id = src_ids[i_old + offset_old];

        if (dst_id == src_id) {
            ecs_bitset_t *src_bs = &src_columns[i_old];
            ecs_bitset_t *dst_bs = &dst_columns[i_new];

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
        } else if (dst_id > src_id) {
            ecs_bitset_t *src_bs = &src_columns[i_old];
            flecs_bitset_fini(src_bs);
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }

    /* Clear remaining columns */
    if (clear) {
        for (; (i_old < src_column_count); i_old ++) {
            ecs_bitset_t *src_bs = &src_columns[i_old];
            ecs_assert(count == flecs_bitset_count(src_bs), 
                ECS_INTERNAL_ERROR, NULL);
            flecs_bitset_fini(src_bs);
        }
    }
}

/* Grow table column. When a column needs to be reallocated this function takes
 * care of correctly invoking ctor/move/dtor hooks. */
static
void flecs_table_grow_column(
    ecs_world_t *world,
    ecs_column_t *column,
    int32_t to_add,
    int32_t dst_size,
    bool construct)
{
    ecs_assert(column != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_info_t *ti = column->ti;
    int32_t size = column->size;
    int32_t count = column->data.count;
    int32_t src_size = column->data.size;
    int32_t dst_count = count + to_add;
    bool can_realloc = dst_size != src_size;
    void *result = NULL;

    ecs_assert(dst_size >= dst_count, ECS_INTERNAL_ERROR, NULL);

    /* If the array could possibly realloc and the component has a move action 
     * defined, move old elements manually */
    ecs_move_t move_ctor;
    if (count && can_realloc && (move_ctor = ti->hooks.ctor_move_dtor)) {
        ecs_xtor_t ctor = ti->hooks.ctor;
        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(move_ctor != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create  vector */
        ecs_vec_t dst;
        ecs_vec_init(&world->allocator, &dst, size, dst_size);
        dst.count = dst_count;

        void *src_buffer = column->data.array;
        void *dst_buffer = dst.array;

        /* Move (and construct) existing elements to new vector */
        move_ctor(dst_buffer, src_buffer, count, ti);

        if (construct) {
            /* Construct new element(s) */
            result = ECS_ELEM(dst_buffer, size, count);
            ctor(result, to_add, ti);
        }

        /* Free old vector */
        ecs_vec_fini(&world->allocator, &column->data, size);

        column->data = dst;
    } else {
        /* If array won't realloc or has no move, simply add new elements */
        if (can_realloc) {
            ecs_vec_set_size(&world->allocator, &column->data, size, dst_size);
        }

        result = ecs_vec_grow(&world->allocator, &column->data, size, to_add);

        ecs_xtor_t ctor;
        if (construct && (ctor = ti->hooks.ctor)) {
            /* If new elements need to be constructed and component has a
             * constructor, construct */
            ctor(result, to_add, ti);
        }
    }

    ecs_assert(column->data.size == dst_size, ECS_INTERNAL_ERROR, NULL);
}

/* Grow all data structures in a table */
static
int32_t flecs_table_grow_data(
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
    int32_t column_count = table->column_count;

    /* Add entity to column with entity ids */
    ecs_vec_set_size_t(&world->allocator, &data->entities, ecs_entity_t, size);
    ecs_entity_t *e = ecs_vec_last_t(&data->entities, ecs_entity_t) + 1;
    data->entities.count += to_add;
    if (data->entities.size > size) {
        size = data->entities.size;
    }

    /* Initialize entity ids and record ptrs */
    int32_t i;
    if (ids) {
        ecs_os_memcpy_n(e, ids, ecs_entity_t, to_add);
    } else {
        ecs_os_memset(e, 0, ECS_SIZEOF(ecs_entity_t) * to_add);
    }

    /* Add elements to each column array */
    ecs_column_t *columns = data->columns;
    for (i = 0; i < column_count; i ++) {
        flecs_table_grow_column(world, &columns[i], to_add, size, true);
        ecs_assert(columns[i].data.size == size, ECS_INTERNAL_ERROR, NULL);
        flecs_table_invoke_add_hooks(world, table, &columns[i], e, 
            cur_count, to_add, false);
    }

    ecs_table__t *meta = table->_;
    int32_t bs_count = meta->bs_count;
    ecs_bitset_t *bs_columns = meta->bs_columns; 

    /* Add elements to each bitset column */
    for (i = 0; i < bs_count; i ++) {
        ecs_bitset_t *bs = &bs_columns[i];
        flecs_bitset_addn(bs, to_add);
    }

    /* If the table is monitored indicate that there has been a change */
    flecs_table_mark_table_dirty(world, table, 0);

    if (!(world->flags & EcsWorldReadonly) && !cur_count) {
        flecs_table_set_empty(world, table);
    }

    /* Return index of first added entity */
    return cur_count;
}

/* Append operation for tables that don't have any complex logic */
static
void flecs_table_fast_append(
    ecs_world_t *world,
    ecs_column_t *columns,
    int32_t count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_vec_append(&world->allocator, &column->data, column->size);
    }
}

/* Append entity to table */
int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    bool construct,
    bool on_add)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    flecs_table_check_sanity(table);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    ecs_data_t *data = &table->data;
    int32_t count = data->entities.count;
    int32_t column_count = table->column_count;
    ecs_column_t *columns = table->data.columns;

    /* Grow buffer with entity ids, set new element to new entity */
    ecs_entity_t *e = ecs_vec_append_t(&world->allocator, 
        &data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;
 
    /* If the table is monitored indicate that there has been a change */
    flecs_table_mark_table_dirty(world, table, 0);
    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(table->flags & EcsTableIsComplex)) {
        flecs_table_fast_append(world, columns, column_count);
        if (!count) {
            flecs_table_set_empty(world, table); /* See below */
        }
        return count;
    }

    ecs_entity_t *entities = data->entities.array;

    /* Reobtain size to ensure that the columns have the same size as the 
     * entities and record vectors. This keeps reasoning about when allocations
     * occur easier. */
    int32_t size = data->entities.size;

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        flecs_table_grow_column(world, column, 1, size, construct);

        ecs_iter_action_t on_add_hook;
        if (on_add && (on_add_hook = column->ti->hooks.on_add)) {
            flecs_table_invoke_hook(world, table, on_add_hook, EcsOnAdd, column,
                &entities[count], count, 1);
        }

        ecs_assert(columns[i].data.size == 
            data->entities.size, ECS_INTERNAL_ERROR, NULL); 
        ecs_assert(columns[i].data.count == 
            data->entities.count, ECS_INTERNAL_ERROR, NULL);
    }

    ecs_table__t *meta = table->_;
    int32_t bs_count = meta->bs_count;
    ecs_bitset_t *bs_columns = meta->bs_columns;

    /* Add element to each bitset column */
    for (i = 0; i < bs_count; i ++) {
        ecs_assert(bs_columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_bitset_t *bs = &bs_columns[i];
        flecs_bitset_addn(bs, 1);
    }

    /* If this is the first entity in this table, signal queries so that the
     * table moves from an inactive table to an active table. */
    if (!count) {
        flecs_table_set_empty(world, table);
    }

    flecs_table_check_sanity(table);

    return count;
}

/* Delete last operation for tables that don't have any complex logic */
static
void flecs_table_fast_delete_last(
    ecs_column_t *columns,
    int32_t column_count) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_vec_remove_last(&columns[i].data);
    }
}

/* Delete operation for tables that don't have any complex logic */
static
void flecs_table_fast_delete(
    ecs_column_t *columns,
    int32_t column_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_vec_remove(&column->data, column->size, index);
    }
}

/* Delete entity from table */
void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t index,
    bool destruct)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    flecs_table_check_sanity(table);

    ecs_data_t *data = &table->data;
    int32_t count = data->entities.count;

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    /* Move last entity id to index */
    ecs_entity_t *entities = data->entities.array;
    ecs_entity_t entity_to_move = entities[count];
    ecs_entity_t entity_to_delete = entities[index];
    entities[index] = entity_to_move;
    ecs_vec_remove_last(&data->entities);

    /* Update record of moved entity in entity index */
    if (index != count) {
        ecs_record_t *record_to_move = flecs_entities_get(world, entity_to_move);
        if (record_to_move) {
            uint32_t row_flags = record_to_move->row & ECS_ROW_FLAGS_MASK;
            record_to_move->row = ECS_ROW_TO_RECORD(index, row_flags);
            ecs_assert(record_to_move->table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(record_to_move->table == table, ECS_INTERNAL_ERROR, NULL);
        }
    }     

    /* If the table is monitored indicate that there has been a change */
    flecs_table_mark_table_dirty(world, table, 0);    

    /* If table is empty, deactivate it */
    if (!count) {
        flecs_table_set_empty(world, table);
    }

    /* Destruct component data */
    ecs_column_t *columns = data->columns;
    int32_t column_count = table->column_count;
    int32_t i;

    /* If this is a table without lifecycle callbacks or special columns, take
     * fast path that just remove an element from the array(s) */
    if (!(table->flags & EcsTableIsComplex)) {
        if (index == count) {
            flecs_table_fast_delete_last(columns, column_count);
        } else {
            flecs_table_fast_delete(columns, column_count, index);
        }

        flecs_table_check_sanity(table);
        return;
    }

    /* Last element, destruct & remove */
    if (index == count) {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & EcsTableHasDtors)) {            
            for (i = 0; i < column_count; i ++) {
                flecs_table_invoke_remove_hooks(world, table, &columns[i], 
                    &entity_to_delete, index, 1, true);
            }
        }

        flecs_table_fast_delete_last(columns, column_count);

    /* Not last element, move last element to deleted element & destruct */
    } else {
        /* If table has component destructors, invoke */
        if ((table->flags & (EcsTableHasDtors | EcsTableHasMove))) {
            for (i = 0; i < column_count; i ++) {
                ecs_column_t *column = &columns[i];
                ecs_type_info_t *ti = column->ti;
                ecs_size_t size = column->size;
                void *dst = ecs_vec_get(&column->data, size, index);
                void *src = ecs_vec_last(&column->data, size);

                ecs_iter_action_t on_remove = ti->hooks.on_remove;
                if (destruct && on_remove) {
                    flecs_table_invoke_hook(world, table, on_remove, EcsOnRemove,
                        column, &entity_to_delete, index, 1);
                }

                ecs_move_t move_dtor = ti->hooks.move_dtor;
                
                // If neither move nor move_ctor are set, this indicates that non-destructive move 
                // semantics are not supported for this type. In such cases, we set the move_dtor
                // as ctor_move_dtor, which indicates a destructive move operation. 
                // This adjustment ensures compatibility with different language bindings.
                if (!ti->hooks.move_ctor && ti->hooks.ctor_move_dtor) {
                  move_dtor = ti->hooks.ctor_move_dtor;
                }

                if (move_dtor) {
                    move_dtor(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }

                ecs_vec_remove_last(&column->data);
            }
        } else {
            flecs_table_fast_delete(columns, column_count, index);
        }
    }

    /* Remove elements from bitset columns */
    ecs_table__t *meta = table->_;
    ecs_bitset_t *bs_columns = meta->bs_columns;
    int32_t bs_count = meta->bs_count;
    for (i = 0; i < bs_count; i ++) {
        flecs_bitset_remove(&bs_columns[i], index);
    }

    flecs_table_check_sanity(table);
}

/* Move operation for tables that don't have any complex logic */
static
void flecs_table_fast_move(
    ecs_table_t *dst_table,
    int32_t dst_index,
    ecs_table_t *src_table,
    int32_t src_index)
{
    int32_t i_new = 0, dst_column_count = dst_table->column_count;
    int32_t i_old = 0, src_column_count = src_table->column_count;

    ecs_column_t *src_columns = src_table->data.columns;
    ecs_column_t *dst_columns = dst_table->data.columns;

    for (; (i_new < dst_column_count) && (i_old < src_column_count);) {
        ecs_column_t *dst_column = &dst_columns[i_new];
        ecs_column_t *src_column = &src_columns[i_old];
        ecs_id_t dst_id = dst_column->id;
        ecs_id_t src_id = src_column->id;

        if (dst_id == src_id) {
            int32_t size = dst_column->size;
            void *dst = ecs_vec_get(&dst_column->data, size, dst_index);
            void *src = ecs_vec_get(&src_column->data, size, src_index);
            ecs_os_memcpy(dst, src, size);
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }
}

/* Move entity from src to dst table */
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
    ecs_assert(!dst_table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(!src_table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    ecs_assert(src_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dst_index >= 0, ECS_INTERNAL_ERROR, NULL);

    flecs_table_check_sanity(dst_table);
    flecs_table_check_sanity(src_table);

    if (!((dst_table->flags | src_table->flags) & EcsTableIsComplex)) {
        flecs_table_fast_move(dst_table, dst_index, src_table, src_index);
        flecs_table_check_sanity(dst_table);
        flecs_table_check_sanity(src_table);
        return;
    }

    flecs_table_move_bitset_columns(
        dst_table, dst_index, src_table, src_index, 1, false);

    /* If the source and destination entities are the same, move component 
     * between tables. If the entities are not the same (like when cloning) use
     * a copy. */
    bool same_entity = dst_entity == src_entity;

    /* Call move_dtor for moved away from storage only if the entity is at the
     * last index in the source table. If it isn't the last entity, the last 
     * entity in the table will be moved to the src storage, which will take
     * care of cleaning up resources. */
    bool use_move_dtor = ecs_table_count(src_table) == (src_index + 1);

    int32_t i_new = 0, dst_column_count = dst_table->column_count;
    int32_t i_old = 0, src_column_count = src_table->column_count;

    ecs_column_t *src_columns = src_table->data.columns;
    ecs_column_t *dst_columns = dst_table->data.columns;

    for (; (i_new < dst_column_count) && (i_old < src_column_count); ) {
        ecs_column_t *dst_column = &dst_columns[i_new];
        ecs_column_t *src_column = &src_columns[i_old];
        ecs_id_t dst_id = dst_column->id;
        ecs_id_t src_id = src_column->id;

        if (dst_id == src_id) {
            int32_t size = dst_column->size;

            ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
            void *dst = ecs_vec_get(&dst_column->data, size, dst_index);
            void *src = ecs_vec_get(&src_column->data, size, src_index);
            ecs_type_info_t *ti = dst_column->ti;

            if (same_entity) {
                ecs_move_t move = ti->hooks.move_ctor;
                if (use_move_dtor || !move) {
                    /* Also use move_dtor if component doesn't have a move_ctor
                     * registered, to ensure that the dtor gets called to 
                     * cleanup resources. */
                    move = ti->hooks.ctor_move_dtor;
                }

                if (move) {
                    move(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            } else {
                ecs_copy_t copy = ti->hooks.copy_ctor;
                if (copy) {
                    copy(dst, src, 1, ti);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }
            }
        } else {
            if (dst_id < src_id) {
                flecs_table_invoke_add_hooks(world, dst_table,
                    dst_column, &dst_entity, dst_index, 1, construct);
            } else {
                flecs_table_invoke_remove_hooks(world, src_table,
                    src_column, &src_entity, src_index, 1, use_move_dtor);
            }
        }

        i_new += dst_id <= src_id;
        i_old += dst_id >= src_id;
    }

    for (; (i_new < dst_column_count); i_new ++) {
        flecs_table_invoke_add_hooks(world, dst_table, &dst_columns[i_new], 
            &dst_entity, dst_index, 1, construct);
    }

    for (; (i_old < src_column_count); i_old ++) {
        flecs_table_invoke_remove_hooks(world, src_table, &src_columns[i_old], 
            &src_entity, src_index, 1, use_move_dtor);
    }

    flecs_table_check_sanity(dst_table);
    flecs_table_check_sanity(src_table);
}

/* Append n entities to table */
int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t to_add,
    const ecs_entity_t *ids)
{
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    flecs_table_check_sanity(table);
    int32_t cur_count = flecs_table_data_count(data);
    int32_t result = flecs_table_grow_data(
        world, table, data, to_add, cur_count + to_add, ids);
    flecs_table_check_sanity(table);

    return result;
}

/* Set allocated table size */
void flecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t size)
{
    ecs_assert(table != NULL, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    flecs_table_check_sanity(table);

    int32_t cur_count = flecs_table_data_count(data);

    if (cur_count < size) {
        flecs_table_grow_data(world, table, data, 0, size, NULL);
        flecs_table_check_sanity(table);
    }
}

/* Shrink table storage to fit number of entities */
bool flecs_table_shrink(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    (void)world;

    flecs_table_check_sanity(table);

    ecs_data_t *data = &table->data;
    bool has_payload = data->entities.array != NULL;
    ecs_vec_reclaim_t(&world->allocator, &data->entities, ecs_entity_t);

    int32_t i, count = table->column_count;
    for (i = 0; i < count; i ++) {
        ecs_column_t *column = &data->columns[i];
        ecs_vec_reclaim(&world->allocator, &column->data, column->size);
    }

    return has_payload;
}

/* Return number of entities in table */
int32_t flecs_table_data_count(
    const ecs_data_t *data)
{
    return data ? data->entities.count : 0;
}

/* Swap operation for bitset (toggle component) columns */
static
void flecs_table_swap_bitset_columns(
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2)
{
    int32_t i = 0, column_count = table->_->bs_count;
    if (!column_count) {
        return;
    }

    ecs_bitset_t *columns = table->_->bs_columns;

    for (i = 0; i < column_count; i ++) {
        ecs_bitset_t *bs = &columns[i];
        flecs_bitset_swap(bs, row_1, row_2);
    }
}

/* Swap two rows in a table. Used for table sorting. */
void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t row_1,
    int32_t row_2)
{    
    (void)world;

    ecs_assert(!table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(row_1 >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_2 >= 0, ECS_INTERNAL_ERROR, NULL);

    flecs_table_check_sanity(table);
    
    if (row_1 == row_2) {
        return;
    }

    /* If the table is monitored indicate that there has been a change */
    flecs_table_mark_table_dirty(world, table, 0);    

    ecs_entity_t *entities = table->data.entities.array;
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t *record_ptr_1 = flecs_entities_get(world, e1);
    ecs_record_t *record_ptr_2 = flecs_entities_get(world, e2);

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

    flecs_table_swap_bitset_columns(table, row_1, row_2);

    ecs_column_t *columns = table->data.columns;
    if (!columns) {
        flecs_table_check_sanity(table);
        return;
    }

    /* Find the maximum size of column elements
     * and allocate a temporary buffer for swapping */
    int32_t i, temp_buffer_size = ECS_SIZEOF(uint64_t), column_count = table->column_count;
    for (i = 0; i < column_count; i++) {
        temp_buffer_size = ECS_MAX(temp_buffer_size, columns[i].size);
    }

    void* tmp = ecs_os_alloca(temp_buffer_size);

    /* Swap columns */
    for (i = 0; i < column_count; i ++) {
        int32_t size = columns[i].size;
        ecs_column_t *column = &columns[i];
        void *ptr = column->data.array;
        const ecs_type_info_t *ti = column->ti;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);

        void *el_1 = ECS_ELEM(ptr, size, row_1);
        void *el_2 = ECS_ELEM(ptr, size, row_2);

        ecs_move_t move = ti->hooks.move;
        if (!move) {
            ecs_os_memcpy(tmp, el_1, size);
            ecs_os_memcpy(el_1, el_2, size);
            ecs_os_memcpy(el_2, tmp, size);
        } else {
            ecs_move_t move_ctor = ti->hooks.move_ctor;
            ecs_move_t move_dtor = ti->hooks.move_dtor;
            ecs_assert(move_ctor != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(move_dtor != NULL, ECS_INTERNAL_ERROR, NULL);
            move_ctor(tmp, el_1, 1, ti);
            move(el_1, el_2, 1, ti);
            move_dtor(el_2, tmp, 1, ti);
        }
    }

    flecs_table_check_sanity(table);
}

static
void flecs_table_merge_vec(
    ecs_world_t *world,
    ecs_vec_t *dst,
    ecs_vec_t *src,
    int32_t size,
    int32_t elem_size)
{
    int32_t dst_count = dst->count;

    if (!dst_count) {
        ecs_vec_fini(&world->allocator, dst, size);
        *dst = *src;
        src->array = NULL;
        src->count = 0;
        src->size = 0;
    } else {
        int32_t src_count = src->count;

        if (elem_size) {
            ecs_vec_set_size(&world->allocator, 
                dst, size, elem_size);
        }
        ecs_vec_set_count(&world->allocator, 
            dst, size, dst_count + src_count);

        void *dst_ptr = ECS_ELEM(dst->array, size, dst_count);
        void *src_ptr = src->array;
        ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);

        ecs_vec_fini(&world->allocator, src, size);
    }
}

/* Merge data from one table column into other table column */
static
void flecs_table_merge_column(
    ecs_world_t *world,
    ecs_column_t *dst,
    ecs_column_t *src,
    int32_t column_size)
{
    ecs_size_t size = dst->size;
    int32_t dst_count = dst->data.count;

    if (!dst_count) {
        ecs_vec_fini(&world->allocator, &dst->data, size);
        *dst = *src;
        src->data.array = NULL;
        src->data.count = 0;
        src->data.size = 0;

    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = src->data.count;

        flecs_table_grow_column(world, dst, src_count, column_size, false);
        void *dst_ptr = ECS_ELEM(dst->data.array, size, dst_count);
        void *src_ptr = src->data.array;

        /* Move values into column */
        ecs_type_info_t *ti = dst->ti;
        ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_move_t move = ti->hooks.ctor_move_dtor;
        if (move) {
            move(dst_ptr, src_ptr, src_count, ti);
        } else {
            ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);
        }

        ecs_vec_fini(&world->allocator, &src->data, size);
    }
}

/* Merge storage of two tables. */
static
void flecs_table_merge_data(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table,
    int32_t src_count,
    int32_t dst_count,
    ecs_data_t *src_data,
    ecs_data_t *dst_data)
{
    int32_t i_new = 0, dst_column_count = dst_table->column_count;
    int32_t i_old = 0, src_column_count = src_table->column_count;
    ecs_column_t *src_columns = src_data->columns;
    ecs_column_t *dst_columns = dst_data->columns;

    ecs_assert(!dst_column_count || dst_columns, ECS_INTERNAL_ERROR, NULL);

    if (!src_count) {
        return;
    }

    /* Merge entities */
    flecs_table_merge_vec(world, &dst_data->entities, &src_data->entities, 
        ECS_SIZEOF(ecs_entity_t), 0);
    ecs_assert(dst_data->entities.count == src_count + dst_count, 
        ECS_INTERNAL_ERROR, NULL);
    int32_t column_size = dst_data->entities.size;
    ecs_allocator_t *a = &world->allocator;

    for (; (i_new < dst_column_count) && (i_old < src_column_count); ) {
        ecs_column_t *dst_column = &dst_columns[i_new];
        ecs_column_t *src_column = &src_columns[i_old];
        ecs_id_t dst_id = dst_column->id;
        ecs_id_t src_id = src_column->id;
    
        if (dst_id == src_id) {
            flecs_table_merge_column(world, dst_column, src_column, column_size);
            flecs_table_mark_table_dirty(world, dst_table, i_new + 1);
            i_new ++;
            i_old ++;
        } else if (dst_id < src_id) {
            /* New column, make sure vector is large enough. */
            ecs_size_t size = dst_column->size;
            ecs_vec_set_size(a, &dst_column->data, size, column_size);
            ecs_vec_set_count(a, &dst_column->data, size, src_count + dst_count);
            flecs_table_invoke_ctor(dst_column, dst_count, src_count);
            i_new ++;
        } else if (dst_id > src_id) {
            /* Old column does not occur in new table, destruct */
            flecs_table_invoke_dtor(src_column, 0, src_count);
            ecs_vec_fini(a, &src_column->data, src_column->size);
            i_old ++;
        }
    }

    flecs_table_move_bitset_columns(dst_table, dst_count, src_table, 0, src_count, true);

    /* Initialize remaining columns */
    for (; i_new < dst_column_count; i_new ++) {
        ecs_column_t *column = &dst_columns[i_new];
        int32_t size = column->size;
        ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_set_size(a, &column->data, size, column_size);
        ecs_vec_set_count(a, &column->data, size, src_count + dst_count);
        flecs_table_invoke_ctor(column, dst_count, src_count);
    }

    /* Destruct remaining columns */
    for (; i_old < src_column_count; i_old ++) {
        ecs_column_t *column = &src_columns[i_old];
        flecs_table_invoke_dtor(column, 0, src_count);
        ecs_vec_fini(a, &column->data, column->size);
    }    

    /* Mark entity column as dirty */
    flecs_table_mark_table_dirty(world, dst_table, 0); 
}

/* Merge source table into destination table. This typically happens as result
 * of a bulk operation, like when a component is removed from all entities in 
 * the source table (like for the Remove OnDelete policy). */
void flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table)
{
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!src_table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);
    ecs_assert(!dst_table->_->lock, ECS_LOCKED_STORAGE, FLECS_LOCKED_STORAGE_MSG);

    flecs_table_check_sanity(src_table);
    flecs_table_check_sanity(dst_table);

    ecs_data_t *dst_data = &dst_table->data;
    ecs_data_t *src_data = &src_table->data;

    ecs_entity_t *src_entities = src_data->entities.array;
    int32_t src_count = src_data->entities.count;
    int32_t dst_count = dst_data->entities.count;

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < src_count; i ++) {
        ecs_record_t *record = flecs_entities_ensure(world, src_entities[i]);
        uint32_t flags = ECS_RECORD_TO_ROW_FLAGS(record->row);
        record->row = ECS_ROW_TO_RECORD(dst_count + i, flags);
        record->table = dst_table;
    }

    /* Merge table columns */
    flecs_table_merge_data(world, dst_table, src_table, src_count, dst_count, 
        src_data, dst_data);

    if (src_count) {
        if (!dst_count) {
            flecs_table_set_empty(world, dst_table);
        }
        flecs_table_set_empty(world, src_table);

        flecs_table_traversable_add(dst_table, src_table->_->traversable_count);
        flecs_table_traversable_add(src_table, -src_table->_->traversable_count);
        ecs_assert(src_table->_->traversable_count == 0, ECS_INTERNAL_ERROR, NULL);
    }

    flecs_table_check_sanity(src_table);
    flecs_table_check_sanity(dst_table);
}

/* Internal mechanism for propagating information to tables */
void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t *event)
{
    if (world->flags & EcsWorldFini) {
        return;
    }

    switch(event->kind) {
    case EcsTableTriggersForId:
        flecs_table_add_trigger_flags(world, table, event->event);
        break;
    case EcsTableNoTriggersForId:
        break;
    }
}

int32_t flecs_table_get_toggle_column(
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_id_t *ids = table->type.array;
    int32_t i = table->_->bs_offset, end = i + table->_->bs_count;

    for (; i < end; i ++) {
        if (ids[i] == (ECS_TOGGLE | id)) {
            return i;
        }
    }

    return -1;
}

ecs_bitset_t* flecs_table_get_toggle(
    ecs_table_t *table,
    ecs_id_t id)
{
    int32_t toggle_column = flecs_table_get_toggle_column(table, id);
    if (toggle_column == -1) {
        return NULL;
    }

    toggle_column -= table->_->bs_offset;
    ecs_assert(toggle_column >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(toggle_column < table->_->bs_count, ECS_INTERNAL_ERROR, NULL);
    return &table->_->bs_columns[toggle_column];
}


/* -- Public API -- */

void ecs_table_lock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (table) {
        if (flecs_poly_is(world, ecs_world_t) && !(world->flags & EcsWorldReadonly)) {
            table->_->lock ++;
        }
    }
}

void ecs_table_unlock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (table) {
        if (flecs_poly_is(world, ecs_world_t) && !(world->flags & EcsWorldReadonly)) {
            table->_->lock --;
            ecs_assert(table->_->lock >= 0, ECS_INVALID_OPERATION, 
                "table_unlock called more often than table_lock");
        }
    }
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

int32_t ecs_table_get_type_index(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return -1;
    }

    ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr) {
        return -1;
    }

    return tr->index;
error:
    return -1;
}

int32_t ecs_table_get_column_index(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_id_record_t *idr = flecs_id_record_get(world, id);
    if (!idr) {
        return -1;
    }

    ecs_table_record_t *tr = flecs_id_record_get_table(idr, table);
    if (!tr) {
        return -1;
    }

    return tr->column;
error:
    return -1;
}

int32_t ecs_table_column_count(
    const ecs_table_t *table)
{
    return table->column_count;
}

int32_t ecs_table_type_to_column_index(
    const ecs_table_t *table,
    int32_t index)
{
    ecs_assert(index >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index < table->type.count, ECS_INVALID_PARAMETER, NULL);
    int32_t *column_map = table->column_map;
    if (column_map) {
        return column_map[index];
    }
error:
    return -1;
}

int32_t ecs_table_column_to_type_index(
    const ecs_table_t *table,
    int32_t index)
{
    ecs_check(index < table->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(table->column_map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t offset = table->type.count;
    return table->column_map[offset + index];
error:
    return -1;
}

void* ecs_table_get_column(
    const ecs_table_t *table,
    int32_t index,
    int32_t offset)
{
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(index < table->column_count, ECS_INVALID_PARAMETER, NULL);

    ecs_column_t *column = &table->data.columns[index];
    void *result = column->data.array;
    if (offset) {
        result = ECS_ELEM(result, column->size, offset);
    }

    return result;
error:
    return NULL;
}

void* ecs_table_get_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    int32_t offset)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    int32_t index = ecs_table_get_column_index(world, table, id);
    if (index == -1) {
        return NULL;
    }

    return ecs_table_get_column(table, index, offset);
error:
    return NULL;
}

size_t ecs_table_get_column_size(
    const ecs_table_t *table,
    int32_t column)
{
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(column < table->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(table->column_map != NULL, ECS_INVALID_PARAMETER, NULL);

    return flecs_ito(size_t, table->data.columns[column].size);
error:
    return 0;
}

int32_t ecs_table_count(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    return flecs_table_data_count(&table->data);
}

bool ecs_table_has_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    return ecs_table_get_type_index(world, table, id) != -1;
}

int32_t ecs_table_get_depth(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_entity_t rel)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_id_is_valid(world, rel), ECS_INVALID_PARAMETER, NULL);
    ecs_check(ecs_has_id(world, rel, EcsAcyclic), ECS_INVALID_PARAMETER,
        "cannot safely determine depth for relationship that is not acyclic "
            "(add Acyclic property to relationship)");

    world = ecs_get_world(world);

    return flecs_relation_depth(world, rel, table);
error:
    return -1;
}

bool ecs_table_has_flags(
    ecs_table_t *table,
    ecs_flags32_t flags)
{
    return (table->flags & flags) == flags;
}

void ecs_table_swap_rows(
    ecs_world_t* world,
    ecs_table_t* table,
    int32_t row_1,
    int32_t row_2)
{
    flecs_table_swap(world, table, row_1, row_2);
}

int32_t flecs_table_observed_count(
    const ecs_table_t *table)
{
    return table->_->traversable_count;
}

void* ecs_record_get_by_column(
    const ecs_record_t *r,
    int32_t index,
    size_t c_size)
{
    (void)c_size;
    ecs_table_t *table = r->table;

    ecs_check(index < table->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_column_t *column = &table->data.columns[index];
    ecs_size_t size = column->size;

    ecs_check(!flecs_utosize(c_size) || flecs_utosize(c_size) == size, 
        ECS_INVALID_PARAMETER, NULL);

    return ecs_vec_get(&column->data, size, ECS_RECORD_TO_ROW(r->row));
error:
    return NULL;
}

ecs_record_t* ecs_record_find(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_check(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_check(entity != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = flecs_entities_get(world, entity);
    if (r) {
        return r;
    }
error:
    return NULL;
}
