/**
 * @file storage.h
 * @brief Storage API
 */

#ifndef FLECS_STORAGE_H
#define FLECS_STORAGE_H

void ecs_storage_init(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_storage_init_t(storage, T, elem_count) \
    ecs_storage_init(storage, ECS_SIZEOF(T), elem_count)

void ecs_storage_fini(
    ecs_column_t *storage);

int32_t ecs_storage_count(
    ecs_column_t *storage);

int32_t ecs_storage_size(
    ecs_column_t *storage);

void* ecs_storage_get(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t index);

#define ecs_storage_get_t(storage, T, index) \
    ECS_CAST(T*, ecs_storage_get(storage, ECS_SIZEOF(T), index))

void* ecs_storage_first(
    ecs_column_t *storage);

#define ecs_storage_first_t(storage, T) \
    ECS_CAST(T*, ecs_storage_first(storage))

void* ecs_storage_last(
    ecs_column_t *storage,
    ecs_size_t size);

#define ecs_storage_last_t(storage, T) \
    ECS_CAST(T*, ecs_storage_last(storage, ECS_SIZEOF(T)))

void* ecs_storage_append(
    ecs_column_t *storage,
    ecs_size_t size);

#define ecs_storage_append_t(storage, T) \
    ECS_CAST(T*, ecs_storage_append(storage, ECS_SIZEOF(T)))

void ecs_storage_remove(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem);

#define ecs_storage_remove_t(storage, T) \
    ECS_CAST(T*, ecs_storage_remove(storage, ECS_SIZEOF(T)))

void ecs_storage_remove_last(
    ecs_column_t *storage);

ecs_column_t ecs_storage_copy(
    ecs_column_t *storage,
    ecs_size_t size);

#define ecs_storage_copy_t(storage, T) \
    ecs_storage_copy(storage, ECS_SIZEOF(T))

void ecs_storage_reclaim(
    ecs_column_t *storage,
    ecs_size_t size);

#define ecs_storage_reclaim_t(storage, T) \
    ecs_storage_reclaim(storage, ECS_SIZEOF(T))

void ecs_storage_set_size(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_storage_set_size_t(storage, T, elem_count) \
    ecs_storage_set_size(storage, ECS_SIZEOF(T), elem_count)

void ecs_storage_set_count(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_storage_set_count_t(storage, T, elem_count) \
    ecs_storage_set_count(storage, ECS_SIZEOF(T), elem_count)

void* ecs_storage_grow(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count);

#define ecs_storage_grow_t(storage, T, elem_count) \
    ecs_storage_grow(storage, ECS_SIZEOF(T), elem_count)

#endif 
