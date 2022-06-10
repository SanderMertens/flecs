#include "flecs.h"

#ifdef FLECS_SNAPSHOT

#include "../private_api.h"

/* World snapshot */
struct ecs_snapshot_t {
    ecs_world_t *world;
    ecs_sparse_t *entity_index;
    ecs_vector_t *tables;
    ecs_entity_t last_id;
    ecs_filter_t filter;
};

/** Small footprint data structure for storing data associated with a table. */
typedef struct ecs_table_leaf_t {
    ecs_table_t *table;
    ecs_type_t type;
    ecs_data_t *data;
} ecs_table_leaf_t;

static
ecs_data_t* duplicate_data(
    ecs_table_t *table,
    ecs_data_t *main_data)
{
    if (!ecs_table_count(table)) {
        return NULL;
    }

    ecs_data_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_data_t));
    int32_t i, column_count = table->storage_count;
    result->columns = ecs_os_memdup_n(
        main_data->columns, ecs_column_t, column_count);

    /* Copy entities and records */
    result->entities = ecs_storage_copy_t(&main_data->entities, ecs_entity_t);
    result->records = ecs_storage_copy_t(&main_data->records, ecs_record_t*);

    /* Copy each column */
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &result->columns[i];
        ecs_type_info_t *ti = table->type_info[i];
        int32_t size = ti->size;
        ecs_copy_t copy = ti->hooks.copy;
        if (copy) {
            ecs_column_t dst = ecs_storage_copy(column, size);
            int32_t count = ecs_storage_count(column);
            void *dst_ptr = ecs_storage_first(&dst);
            void *src_ptr = ecs_storage_first(column);

            ecs_xtor_t ctor = ti->hooks.ctor;
            if (ctor) {
                ctor(dst_ptr, count, ti);
            }

            copy(dst_ptr, src_ptr, count, ti);
            *column = dst;
        } else {
            *column = ecs_storage_copy(column, size);
        }
    }

    return result;
}

static
void snapshot_table(
    ecs_snapshot_t *snapshot,
    ecs_table_t *table)
{
    if (table->flags & EcsTableHasBuiltins) {
        return;
    }
    
    ecs_table_leaf_t *l = ecs_vector_get(
        snapshot->tables, ecs_table_leaf_t, (int32_t)table->id);
    ecs_assert(l != NULL, ECS_INTERNAL_ERROR, NULL);
    
    l->table = table;
    l->type = flecs_type_copy(&table->type);
    l->data = duplicate_data(table, &table->data);
}

static
ecs_snapshot_t* snapshot_create(
    const ecs_world_t *world,
    const ecs_sparse_t *entity_index,
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_snapshot_t *result = ecs_os_calloc_t(ecs_snapshot_t);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    ecs_run_aperiodic((ecs_world_t*)world, 0);

    result->world = (ecs_world_t*)world;

    /* If no iterator is provided, the snapshot will be taken of the entire
     * world, and we can simply copy the entity index as it will be restored
     * entirely upon snapshote restore. */
    if (!iter && entity_index) {
        result->entity_index = flecs_sparse_copy(entity_index);
    }

    /* Create vector with as many elements as tables, so we can store the
     * snapshot tables at their element ids. When restoring a snapshot, the code
     * will run a diff between the tables in the world and the snapshot, to see
     * which of the world tables still exist, no longer exist, or need to be
     * deleted. */
    uint64_t t, table_count = flecs_sparse_last_id(&world->store.tables) + 1;
    result->tables = ecs_vector_new(ecs_table_leaf_t, (int32_t)table_count);
    ecs_vector_set_count(&result->tables, ecs_table_leaf_t, (int32_t)table_count);
    ecs_table_leaf_t *arr = ecs_vector_first(result->tables, ecs_table_leaf_t);

    /* Array may have holes, so initialize with 0 */
    ecs_os_memset_n(arr, 0, ecs_table_leaf_t, table_count);

    /* Iterate tables in iterator */
    if (iter) {
        while (next(iter)) {
            ecs_table_t *table = iter->table;
            snapshot_table(result, table);
        }
    } else {
        for (t = 0; t < table_count; t ++) {
            ecs_table_t *table = flecs_sparse_get(
                &world->store.tables, ecs_table_t, t);
            snapshot_table(result, table);
        }
    }

    return result;
}

/** Create a snapshot */
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *stage)
{
    const ecs_world_t *world = ecs_get_world(stage);

    ecs_snapshot_t *result = snapshot_create(
        world, ecs_eis(world), NULL, NULL);

    result->last_id = world->info.last_id;

    return result;
}

/** Create a filtered snapshot */
ecs_snapshot_t* ecs_snapshot_take_w_iter(
    ecs_iter_t *iter)
{
    ecs_world_t *world = iter->world;
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_snapshot_t *result = snapshot_create(
        world, ecs_eis(world), iter, iter ? iter->next : NULL);

    result->last_id = world->info.last_id;

    return result;
}

/* Restoring an unfiltered snapshot restores the world to the exact state it was
 * when the snapshot was taken. */
static
void restore_unfiltered(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    flecs_sparse_restore(ecs_eis(world), snapshot->entity_index);
    flecs_sparse_free(snapshot->entity_index);
    
    world->info.last_id = snapshot->last_id;

    ecs_table_leaf_t *leafs = ecs_vector_first(
        snapshot->tables, ecs_table_leaf_t);
    int32_t i, count = (int32_t)flecs_sparse_last_id(&world->store.tables);
    int32_t snapshot_count = ecs_vector_count(snapshot->tables);

    for (i = 0; i <= count; i ++) {
        ecs_table_t *world_table = flecs_sparse_get(
            &world->store.tables, ecs_table_t, (uint32_t)i);

        if (world_table && (world_table->flags & EcsTableHasBuiltins)) {
            continue;
        }

        ecs_table_leaf_t *snapshot_table = NULL;
        if (i < snapshot_count) {
            snapshot_table = &leafs[i];
            if (!snapshot_table->table) {
                snapshot_table = NULL;
            }
        }

        /* If the world table no longer exists but the snapshot table does,
         * reinsert it */
        if (!world_table && snapshot_table) {
            ecs_table_t *table = flecs_table_find_or_create(world, 
                &snapshot_table->type);
            ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

            if (snapshot_table->data) {
                flecs_table_replace_data(world, table, snapshot_table->data);
            }
        
        /* If the world table still exists, replace its data */
        } else if (world_table && snapshot_table) {
            ecs_assert(snapshot_table->table == world_table, 
                ECS_INTERNAL_ERROR, NULL);

            if (snapshot_table->data) {
                flecs_table_replace_data(
                    world, world_table, snapshot_table->data);
            } else {
                flecs_table_clear_data(
                    world, world_table, &world_table->data);
                flecs_table_init_data(world_table);
            }
        
        /* If the snapshot table doesn't exist, this table was created after the
         * snapshot was taken and needs to be deleted */
        } else if (world_table && !snapshot_table) {
            /* Deleting a table invokes OnRemove triggers & updates the entity
             * index. That is not what we want, since entities may no longer be
             * valid (if they don't exist in the snapshot) or may have been
             * restored in a different table. Therefore first clear the data
             * from the table (which doesn't invoke triggers), and then delete
             * the table. */
            flecs_table_clear_data(world, world_table, &world_table->data);
            flecs_delete_table(world, world_table);
        
        /* If there is no world & snapshot table, nothing needs to be done */
        } else { }

        if (snapshot_table) {
            ecs_os_free(snapshot_table->data);
            flecs_type_free(&snapshot_table->type);
        }
    }

    /* Now that all tables have been restored and world is in a consistent
     * state, run OnSet systems */
    int32_t world_count = flecs_sparse_count(&world->store.tables);
    for (i = 0; i < world_count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(
            &world->store.tables, ecs_table_t, i);
        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        int32_t tcount = ecs_table_count(table);
        if (tcount) {
            flecs_notify_on_set(world, table, 0, tcount, NULL, true);
        }
    }
}

/* Restoring a filtered snapshots only restores the entities in the snapshot
 * to their previous state. */
static
void restore_filtered(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    ecs_table_leaf_t *leafs = ecs_vector_first(
        snapshot->tables, ecs_table_leaf_t);
    int32_t l = 0, snapshot_count = ecs_vector_count(snapshot->tables);

    for (l = 0; l < snapshot_count; l ++) {
        ecs_table_leaf_t *snapshot_table = &leafs[l];
        ecs_table_t *table = snapshot_table->table;

        if (!table) {
            continue;
        }

        ecs_data_t *data = snapshot_table->data;
        if (!data) {
            flecs_type_free(&snapshot_table->type);
            continue;
        }

        /* Delete entity from storage first, so that when we restore it to the
         * current table we can be sure that there won't be any duplicates */
        int32_t i, entity_count = ecs_storage_count(&data->entities);
        ecs_entity_t *entities = ecs_storage_first(
            &snapshot_table->data->entities);
        for (i = 0; i < entity_count; i ++) {
            ecs_entity_t e = entities[i];
            ecs_record_t *r = flecs_entities_get(world, e);
            if (r && r->table) {
                flecs_table_delete(world, r->table, 
                    ECS_RECORD_TO_ROW(r->row), true);
            } else {
                /* Make sure that the entity has the same generation count */
                flecs_entities_set_generation(world, e);
            }
        }

        /* Merge data from snapshot table with world table */
        int32_t old_count = ecs_table_count(snapshot_table->table);
        int32_t new_count = flecs_table_data_count(snapshot_table->data);

        flecs_table_merge(world, table, table, &table->data, snapshot_table->data);

        /* Run OnSet systems for merged entities */
        if (new_count) {
            flecs_notify_on_set(
                world, table, old_count, new_count, NULL, true);
        }

        ecs_os_free(snapshot_table->data->columns);
        ecs_os_free(snapshot_table->data);
        flecs_type_free(&snapshot_table->type);
    }
}

/** Restore a snapshot */
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    ecs_run_aperiodic(world, 0);
    
    if (snapshot->entity_index) {
        /* Unfiltered snapshots have a copy of the entity index which is
         * copied back entirely when the snapshot is restored */
        restore_unfiltered(world, snapshot);
    } else {
        restore_filtered(world, snapshot);
    }

    ecs_vector_free(snapshot->tables);   

    ecs_os_free(snapshot);
}

ecs_iter_t ecs_snapshot_iter(
    ecs_snapshot_t *snapshot)
{
    ecs_snapshot_iter_t iter = {
        .tables = snapshot->tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = snapshot->world,
        .table_count = ecs_vector_count(snapshot->tables),
        .priv.iter.snapshot = iter,
        .next = ecs_snapshot_next
    };
}

bool ecs_snapshot_next(
    ecs_iter_t *it)
{
    ecs_snapshot_iter_t *iter = &it->priv.iter.snapshot;
    ecs_table_leaf_t *tables = ecs_vector_first(iter->tables, ecs_table_leaf_t);
    int32_t count = ecs_vector_count(iter->tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = tables[i].table;
        if (!table) {
            continue;
        }

        ecs_data_t *data = tables[i].data;

        it->table = table;
        it->count = ecs_table_count(table);
        if (data) {
            it->entities = ecs_storage_first(&data->entities);
        } else {
            it->entities = NULL;
        }

        ECS_BIT_SET(it->flags, EcsIterIsValid);
        iter->index = i + 1;
        
        goto yield;
    }

    ECS_BIT_CLEAR(it->flags, EcsIterIsValid);
    return false;

yield:
    ECS_BIT_CLEAR(it->flags, EcsIterIsValid);
    return true;    
}

/** Cleanup snapshot */
void ecs_snapshot_free(
    ecs_snapshot_t *snapshot)
{
    flecs_sparse_free(snapshot->entity_index);

    ecs_table_leaf_t *tables = ecs_vector_first(snapshot->tables, ecs_table_leaf_t);
    int32_t i, count = ecs_vector_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_leaf_t *snapshot_table = &tables[i];
        ecs_table_t *table = snapshot_table->table;
        if (table) {
            ecs_data_t *data = snapshot_table->data;
            if (data) {
                flecs_table_clear_data(snapshot->world, table, data);
                ecs_os_free(data);
            }
            flecs_type_free(&snapshot_table->type);
        }
    }    

    ecs_vector_free(snapshot->tables);
    ecs_os_free(snapshot);
}

#endif
