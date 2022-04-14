/**
 * @file iter.h
 * @brief Iterator utilities.
 */

#ifndef FLECS_ITER_H
#define FLECS_ITER_H

void flecs_iter_init(
    ecs_iter_t *it,
    ecs_flags8_t fields);

void flecs_iter_validate(
    ecs_iter_t *it);

void flecs_iter_populate_data(
    ecs_world_t *world,
    ecs_iter_t *it,
    ecs_table_t *table,
    int32_t offset,
    int32_t count,
    void **ptrs,
    ecs_size_t *sizes);

bool flecs_iter_next_row(
    ecs_iter_t *it);

bool flecs_iter_next_instanced(
    ecs_iter_t *it,
    bool result);

#endif
