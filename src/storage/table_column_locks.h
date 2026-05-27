/**
 * @file storage/table_column_locks.h
 * @brief Table column lock declarations for mutable alias detection.
 */

#ifndef FLECS_TABLE_COLUMN_LOCKS_H
#define FLECS_TABLE_COLUMN_LOCKS_H

#ifdef FLECS_MUT_ALIAS_LOCKS

void flecs_table_init_column_locks(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t stage_count);

void flecs_table_resize_column_locks(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t previous_stage_count,
    int32_t new_stage_count);

void flecs_tables_resize_column_locks(
    ecs_world_t *world,
    int32_t previous_stage_count,
    int32_t new_stage_count);

FLECS_ALWAYS_INLINE int32_t flecs_table_column_lock_inc(
    ecs_table_t *table,
    const int16_t column_index);

FLECS_ALWAYS_INLINE int32_t flecs_table_column_lock_inc_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id);

FLECS_ALWAYS_INLINE int32_t flecs_table_column_lock_dec(
    ecs_table_t *table,
    const int16_t column_index);

FLECS_ALWAYS_INLINE int32_t flecs_table_column_lock_dec_multithreaded(
    ecs_table_t *table,
    const int16_t column_index,
    const int32_t stage_id);

#endif

#endif
