#include "flecs.h"

#ifdef FLECS_SNAPSHOT

#include "../private_api.h"

/** Small footprint data structure for storing data associated with a table. */
typedef struct ecs_snapshot_table_t {
    ecs_table_t *table;
    ecs_type_t type;
    ecs_data_t storage;
} ecs_snapshot_table_t;

struct ecs_snapshot_t {
    ecs_world_t *world;
    ecs_sparse_t *entity_index;
    ecs_vector_t *tables;
    ecs_entity_t last_id;
    ecs_filter_t filter;
};

static
void copy_storage(
    ecs_world_t *world,
    ecs_snapshot_table_t *snapshot_table,
    ecs_data_t *src_data)
{
    ecs_table_t *table = snapshot_table->table;
    ecs_data_t *dst_data = &snapshot_table->storage;
    ecs_table_storage_init(world, table, dst_data);

    int32_t count = ecs_table_storage_count(src_data);
    ecs_entity_t *ids = ecs_vector_first(src_data->entities, ecs_entity_t);

    ecs_table_appendn(world, table, dst_data, count, ids, src_data, 0);

    ecs_vector_t *dst_r_vec = dst_data->record_ptrs;
    ecs_vector_t *src_r_vec = src_data->record_ptrs;
    ecs_assert(ecs_vector_count(dst_r_vec) == ecs_vector_count(src_r_vec), 
        ECS_INTERNAL_ERROR, NULL);

    /* Copy record ptrs */
    ecs_record_t **dst_r = ecs_vector_first(dst_r_vec, ecs_record_t*);
    ecs_record_t **src_r = ecs_vector_first(src_r_vec, ecs_record_t*);

    ecs_os_memcpy_n(dst_r, src_r, ecs_record_t*, count);
}

static
ecs_snapshot_t* snapshot_create(
    ecs_world_t *world,
    const ecs_sparse_t *entity_index,
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_snapshot_t *result = ecs_os_calloc_t(ecs_snapshot_t);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->world = world;

    /* If no iterator is provided, the snapshot will be taken of the entire
     * world, and we can simply copy the entity index as it will be restored
     * entirely upon snapshote restore. */
    if (!iter && entity_index) {
        result->entity_index = ecs_sparse_copy(entity_index);
        result->tables = ecs_vector_new(ecs_snapshot_table_t, 0);
    }

    ecs_iter_t iter_stack;
    if (!iter) {
        iter_stack = ecs_filter_iter(world, NULL);
        iter = &iter_stack;
        next = ecs_filter_next;
    }

    /* If an iterator is provided, this is a filterred snapshot. In this case we
     * have to patch the entity index one by one upon restore, as we don't want
     * to affect entities that were not part of the snapshot. */
    else {
        result->entity_index = NULL;
    }

    /* Iterate tables in iterator */
    while (next(iter)) {
        ecs_table_t *table = iter->table->table;

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!ecs_table_count(table)) {
            continue;
        }

        ecs_snapshot_table_t *snapshot_table = ecs_vector_add(
            &result->tables, ecs_snapshot_table_t);

        snapshot_table->table = table;
        snapshot_table->type = table->type;
        copy_storage(world, snapshot_table, &table->storage);
    }

    return result;
}

/** Create a snapshot */
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world)
{
    ecs_snapshot_t *result = snapshot_create(
        world,
        world->store.entity_index,
        NULL,
        NULL);

    result->last_id = world->stats.last_id;

    return result;
}

/** Create a filtered snapshot */
ecs_snapshot_t* ecs_snapshot_take_w_iter(
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_world_t *world = iter->world;
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_snapshot_t *result = snapshot_create(
        world,
        world->store.entity_index,
        iter,
        next);

    result->last_id = world->stats.last_id;

    return result;
}

/** Restore a snapshot */
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    bool is_filtered = true;

    if (snapshot->entity_index) {
        ecs_sparse_restore(world->store.entity_index, snapshot->entity_index);
        ecs_sparse_free(snapshot->entity_index);
        is_filtered = false;
    }

    if (!is_filtered) {
        world->stats.last_id = snapshot->last_id;
    }

    ecs_snapshot_table_t *snapshot_tables = ecs_vector_first(
        snapshot->tables, ecs_snapshot_table_t);
    int32_t l = 0, count = ecs_vector_count(snapshot->tables);
    int32_t t, table_count = ecs_sparse_count(world->store.tables);

    for (t = 0; t < table_count; t ++) {
        ecs_table_t *table = ecs_sparse_get(
            world->store.tables, ecs_table_t, t);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_snapshot_table_t *snapshot_table = NULL;
        if (l < count) {
            snapshot_table = &snapshot_tables[l];
        }

        if (snapshot_table && snapshot_table->table == table) {
            ecs_data_t *src_data = &snapshot_table->storage;

            /* If the snapshot is filtered, update the entity index for the
             * entities in the snapshot. If the snapshot was not filtered
             * the entity index would have been replaced entirely, and this
             * is not necessary. */
            if (is_filtered) {
                /* Always delete entity, so that even if the entity is
                 * in the current table, there won't be duplicates */
                ecs_vector_each(src_data->entities, ecs_entity_t, e_ptr, {
                    ecs_record_t *r = ecs_eis_get(world, *e_ptr);
                    ecs_table_t *r_table;
                    if (r && (r_table = r->table)) {
                        ecs_data_t *r_data = &r_table->storage;
                        bool dummy;
                        int32_t r_row = ecs_record_to_row(r->row, &dummy);
                        ecs_table_delete(world, r_table, r_data, r_row, true);
                    } else {
                        ecs_eis_set_generation(world, *e_ptr);
                    }
                });

                ecs_data_t *dst_data = &table->storage;
                int32_t old_count = ecs_table_storage_count(dst_data);
                int32_t new_count = ecs_table_storage_count(src_data);

                ecs_table_merge(world, table, table, dst_data, src_data);

                /* Run OnSet systems for merged entities */
                ecs_ids_t components = ecs_type_to_entities(table->type);
                ecs_run_set_systems(world, &components, table, dst_data,
                    old_count, new_count, true);
            } else {
                ecs_table_replace_data(world, table, src_data);
            }
            
            l ++;
        } else {
            /* If the snapshot is not filtered, the snapshot should restore the
             * world to the exact state it was in. When a snapshot is filtered,
             * it should only update the entities that were in the snapshot.
             * If a table is found that was not in the snapshot, and the
             * snapshot was not filtered, clear the table. */
            if (!is_filtered) {
                /* Clear data of old table. */
                ecs_table_clear_data(world, table, &table->storage);
            }
        }

        table->alloc_count ++;
    }

    /* If snapshot was not filtered, run OnSet systems now. This cannot be done
     * while restoring the snapshot, because the world is in an inconsistent
     * state while restoring. When a snapshot is filtered, the world is not left
     * in an inconsistent state, which makes running OnSet systems while
     * restoring safe */
    if (!is_filtered) {
        for (t = 0; t < table_count; t ++) {
            ecs_table_t *table = ecs_sparse_get(
                world->store.tables, ecs_table_t, t);

            if (table->flags & EcsTableHasBuiltins) {
                continue;
            }

            ecs_ids_t components = ecs_type_to_entities(table->type);

            ecs_run_set_systems(world, &components, table, &table->storage, 0, 
                ecs_table_count(table), true);            
        }
    }

    ecs_vector_free(snapshot->tables);   

    ecs_os_free(snapshot);
}

ecs_iter_t ecs_snapshot_iter(
    ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter)
{
    ecs_snapshot_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = snapshot->tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = snapshot->world,
        .table_count = ecs_vector_count(snapshot->tables),
        .iter.snapshot = iter
    };
}

bool ecs_snapshot_next(
    ecs_iter_t *it)
{
    ecs_snapshot_iter_t *iter = &it->iter.snapshot;
    ecs_snapshot_table_t *tables = ecs_vector_first(iter->tables, ecs_snapshot_table_t);
    int32_t count = ecs_vector_count(iter->tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_snapshot_table_t *snapshot_table = &tables[i];
        ecs_table_t *table = snapshot_table->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_data_t *data = &snapshot_table->storage;
        if (!ecs_table_match_filter(it->world, table, &iter->filter)) {
            continue;
        }

        iter->table.table = table;
        it->table = &iter->table;
        // it->table_columns = data->columns; // TODO
        it->count = ecs_table_storage_count(data);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        iter->index = i + 1;

        return true;
    }

    return false;    
}

/** Cleanup snapshot */
void ecs_snapshot_free(
    ecs_snapshot_t *snapshot)
{
    ecs_sparse_free(snapshot->entity_index);

    ecs_snapshot_table_t *tables = ecs_vector_first(snapshot->tables, ecs_snapshot_table_t);
    int32_t i, count = ecs_vector_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_snapshot_table_t *snapshot_table = &tables[i];
        ecs_table_clear_data(snapshot->world, 
            snapshot_table->table, &snapshot_table->storage);
    }    

    ecs_vector_free(snapshot->tables);
    ecs_os_free(snapshot);
}

#endif
