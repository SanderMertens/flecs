/**
 * @file storage/table_column_locks.c
 * @brief Table column lock implementation for mutable alias detection.
 */

#include "../private_api.h"

#ifdef FLECS_MUT_ALIAS_LOCKS

void flecs_table_init_column_locks(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t stage_count)
{
    flecs_poly_assert(world, ecs_world_t);
    ecs_assert(stage_count > 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    if (!table->column_lock) {
        int32_t column_count = table->column_count;

        if (column_count == 0) {
            /* tags don't need column locks nor (new) column-less tables */
            return;
        }

        table->column_lock = flecs_alloc_n(&world->allocator,
              int32_t, column_count * stage_count);
        ecs_assert(
            (column_count != 0 && table->column_lock != NULL) ||
            (column_count == 0 && table->column_lock == NULL)
            , ECS_INTERNAL_ERROR, NULL);
        for (int i = 0; i < column_count * stage_count; i ++) {
            table->column_lock[i] = 0;
        }
    }
}

#ifdef FLECS_DEBUG
static
bool flecs_table_column_locks_are_zero(
    ecs_table_t *table,
    int32_t column_count,
    int32_t stage_count)
{
    for (int i = 0; i < column_count * stage_count; i ++) {
        if (table->column_lock[i] != 0) {
            return false;
        }
    }
    return true;
}
#endif

void flecs_table_resize_column_locks(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t previous_stage_count,
    int32_t new_stage_count)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(previous_stage_count > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_stage_count > 0, ECS_INTERNAL_ERROR, NULL);

    if (table->column_lock) {
        int32_t column_count = table->column_count;
        /* if column_lock is not null, column count should be higher than 0 */
        ecs_assert(column_count > 0, ECS_INTERNAL_ERROR, NULL);

        /* assert no locks are held before resizing */
        ecs_dbg_assert(
            flecs_table_column_locks_are_zero(table, column_count,
                previous_stage_count), ECS_INTERNAL_ERROR,
            "cannot resize column locks while locks are held");

        table->column_lock = flecs_realloc_n(&world->allocator,
            int32_t, column_count * new_stage_count, column_count * previous_stage_count, table->column_lock);
        ecs_assert(
            ((column_count != 0 && new_stage_count != 0) && table->column_lock != NULL) ||
            ((column_count == 0 || new_stage_count == 0) && table->column_lock == NULL)
            , ECS_INTERNAL_ERROR, NULL);
        for (int i = 0; i < column_count * new_stage_count; i ++) {
            table->column_lock[i] = 0;
        }

    } else if (table->column_count > 0 && new_stage_count > 0) {
        flecs_table_init_column_locks(world, table, new_stage_count);
    }
}

/* resize column locks for all tables. */
void flecs_tables_resize_column_locks(
    ecs_world_t *world,
    int32_t previous_stage_count,
    int32_t new_stage_count)
{
    flecs_poly_assert(world, ecs_world_t);
    /* no need to resize when world not initialized yet or same stage count */
    if (world->flags & EcsWorldInit || previous_stage_count == new_stage_count) {
        return;
    }

    if (!(world->flags & EcsWorldFini)) {
        int32_t i, count = flecs_sparse_count(&world->store.tables);
        for (i = 1; i < count; i ++) {
            ecs_table_t *table = flecs_sparse_get_dense_t(&world->store.tables,
                ecs_table_t, i);
            flecs_table_resize_column_locks(world, table, previous_stage_count, new_stage_count);
        }
    /* At world fini, free the table locks instead of resizing. The world
     * returns to single threaded mode and sets the stage — that is where
     * tables get cleaned up. When stage count is set to 0 at the end of
     * fini, cleanup has already happened. */
    } else if (new_stage_count != 0) {
        int32_t i, count = flecs_sparse_count(&world->store.tables);
        for (i = 1; i < count; i ++) {
            ecs_table_t *table = flecs_sparse_get_dense_t(&world->store.tables,
                ecs_table_t, i);

            if (table->column_lock) {
                flecs_wfree_n(world, int32_t, previous_stage_count * table->column_count, table->column_lock);
                table->column_lock = NULL;
            }
        }
    }
}

int32_t flecs_table_column_inc(
    ecs_table_t *table,
    const int16_t column_index)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->column_lock != NULL, ECS_INTERNAL_ERROR, NULL);
    return ++table->column_lock[ column_index ];
}

int32_t flecs_table_column_inc_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id
    )
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->column_lock != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_os_ainc(&table->column_lock[ column_index + (stage_id * table->column_count) ]);
}

int32_t flecs_table_column_dec(
    ecs_table_t *table,
    const int16_t column_index)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->column_lock != NULL, ECS_INTERNAL_ERROR, NULL);
    return --table->column_lock[ column_index ];
}

int32_t flecs_table_column_dec_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id
    )
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->column_lock != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_os_adec(&table->column_lock[ column_index + (stage_id * table->column_count) ]);
}

bool flecs_table_column_read_begin(
    ecs_table_t *table,
    const int16_t column_index
    )
{
    return flecs_table_column_inc(table, column_index) <= 0;
}

bool flecs_table_column_read_begin_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id
    )
{
    return flecs_table_column_inc_multithreaded(table, column_index, stage_id) <= 0;
}

bool flecs_table_column_read_end(
    ecs_table_t *table,
    const int16_t column_index
    )
{
    return flecs_table_column_dec(table, column_index) < 0;
}

bool flecs_table_column_read_end_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id
    )
{
    return flecs_table_column_dec_multithreaded(table, column_index, stage_id) < 0;
}

bool flecs_table_column_write_begin(
    ecs_table_t *table,
    const int16_t column_index
    )
{
    return flecs_table_column_dec(table, column_index) != -1;
}

bool flecs_table_column_write_begin_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id
    )
{
    return flecs_table_column_dec_multithreaded(table, column_index, stage_id) != -1;
}

bool flecs_table_column_write_end(
    ecs_table_t *table,
    const int16_t column_index
    )
{
    return flecs_table_column_inc(table, column_index) != 0;
}

bool flecs_table_column_write_end_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id
    )
{
    return flecs_table_column_inc_multithreaded(table, column_index, stage_id) != 0;
}

#endif
