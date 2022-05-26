#include "private_api.h"

void ecs_storage_init(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count)
{
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);
    storage->array = NULL;
    storage->count = 0;
    if (elem_count) {
        storage->array = ecs_os_malloc(size * elem_count);
    }
    storage->size = elem_count;
}

void ecs_storage_fini(
    ecs_column_t *storage)
{
    ecs_os_free(storage->array);
    storage->array = NULL;
    storage->count = 0;
    storage->size = 0;
}

int32_t ecs_storage_count(
    ecs_column_t *storage)
{
    return storage->count;
}

int32_t ecs_storage_size(
    ecs_column_t *storage)
{
    return storage->size;
}

void* ecs_storage_get(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t index)
{
    ecs_assert(index < storage->count, ECS_OUT_OF_RANGE, NULL);
    return ECS_ELEM(storage->array, size, index);
}

void* ecs_storage_last(
    ecs_column_t *storage,
    ecs_size_t size)
{
    return ECS_ELEM(storage->array, size, storage->count - 1);
}

void* ecs_storage_first(
    ecs_column_t *storage)
{
    return storage->array;
}

void* ecs_storage_append(
    ecs_column_t *storage,
    ecs_size_t size)
{
    int32_t count = storage->count;
    if (storage->size == count) {
        ecs_storage_set_size(storage, size, count + 1);
    }
    storage->count = count + 1;
    return ECS_ELEM(storage->array, size, count);
}

void ecs_storage_remove(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t index)
{
    ecs_assert(index < storage->count, ECS_OUT_OF_RANGE, NULL);
    if (index == --storage->count) {
        return;
    }

    ecs_os_memcpy(
        ECS_ELEM(storage->array, size, index),
        ECS_ELEM(storage->array, size, storage->count),
        size);
}

void ecs_storage_remove_last(
    ecs_column_t *storage)
{
    storage->count --;
}

ecs_column_t ecs_storage_copy(
    ecs_column_t *storage,
    ecs_size_t size)
{
    return (ecs_column_t) {
        .count = storage->count,
        .size = storage->size,
        .array = ecs_os_memdup(storage->array, storage->size * size)
    };
}

void ecs_storage_reclaim(
    ecs_column_t *storage,
    ecs_size_t size)
{
    int32_t count = storage->count;
    if (count < storage->size) {
        if (count) {
            storage->array = ecs_os_realloc(storage->array, size * count);
            storage->size = count;
        } else {
            ecs_storage_fini(storage);
        }
    }
}

void ecs_storage_set_size(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count)
{
    if (storage->size != elem_count) {
        if (elem_count < storage->count) {
            elem_count = storage->count;
        }

        elem_count = flecs_next_pow_of_2(elem_count);
        if (elem_count < 2) {
            elem_count = 2;
        }
        if (elem_count != storage->size) {
            storage->array = ecs_os_realloc(storage->array, size * elem_count);
            storage->size = elem_count;
        }
    }
}

void ecs_storage_set_count(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count)
{
    if (storage->count != elem_count) {
        if (storage->size < elem_count) {
            ecs_storage_set_size(storage, size, elem_count);
        }

        storage->count = elem_count;
    }
}

void* ecs_storage_grow(
    ecs_column_t *storage,
    ecs_size_t size,
    int32_t elem_count)
{
    ecs_assert(elem_count > 0, ECS_INTERNAL_ERROR, NULL);
    int32_t count = storage->count;
    ecs_storage_set_count(storage, size, count + elem_count);
    return ECS_ELEM(storage->array, size, count);
}
