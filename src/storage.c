#include "private_api.h"

void ecs_storage_init(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size,
    int32_t elem_count)
{
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);
    s->array = NULL;
    s->count = 0;
    if (elem_count) {
        s->array = flecs_walloc(world, size * elem_count);
    }
    s->size = elem_count;
}

void ecs_storage_fini(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size)
{
    flecs_wfree(world, size * s->size, s->array);
    s->array = NULL;
    s->count = 0;
    s->size = 0;
}

ecs_column_t ecs_storage_copy(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size)
{
    return (ecs_column_t) {
        .count = s->count,
        .size = s->size,
        .array = flecs_wdup(world, s->count * s->size, s->array)
    };
}

void ecs_storage_reclaim(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size)
{
    int32_t count = s->count;
    if (count < s->size) {
        if (count) {
            s->array = flecs_wrealloc(
                world, size * count, size * s->size, s->array);
            s->size = count;
        } else {
            ecs_storage_fini(world, s, size);
        }
    }
}

void ecs_storage_set_size(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size,
    int32_t elem_count)
{
    if (s->size != elem_count) {
        if (elem_count < s->count) {
            elem_count = s->count;
        }

        elem_count = flecs_next_pow_of_2(elem_count);
        if (elem_count < 2) {
            elem_count = 2;
        }
        if (elem_count != s->size) {
            s->array = flecs_wrealloc(
                world, size * elem_count, size * s->size, s->array);
            s->size = elem_count;
        }
    }
}

void ecs_storage_set_count(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size,
    int32_t elem_count)
{
    if (s->count != elem_count) {
        if (s->size < elem_count) {
            ecs_storage_set_size(world, s, size, elem_count);
        }

        s->count = elem_count;
    }
}

void* ecs_storage_grow(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size,
    int32_t elem_count)
{
    ecs_assert(elem_count > 0, ECS_INTERNAL_ERROR, NULL);
    int32_t count = s->count;
    ecs_storage_set_count(world, s, size, count + elem_count);
    return ECS_ELEM(s->array, size, count);
}

void* ecs_storage_append(
    ecs_world_t *world,
    ecs_column_t *s,
    ecs_size_t size)
{
    int32_t count = s->count;
    if (s->size == count) {
        ecs_storage_set_size(world, s, size, count + 1);
    }
    s->count = count + 1;
    return ECS_ELEM(s->array, size, count);
}

void ecs_storage_remove(
    ecs_world_t *world,

    ecs_column_t *s,
    ecs_size_t size,
    int32_t index)
{
    ecs_assert(index < s->count, ECS_OUT_OF_RANGE, NULL);
    if (index == --s->count) {
        return;
    }

    ecs_os_memcpy(
        ECS_ELEM(s->array, size, index),
        ECS_ELEM(s->array, size, s->count),
        size);
}

void ecs_storage_remove_last(
    ecs_world_t *world,
    ecs_column_t *s)
{
    s->count --;
}

int32_t ecs_storage_count(
    ecs_column_t *s)
{
    return s->count;
}

int32_t ecs_storage_size(
    ecs_column_t *s)
{
    return s->size;
}

void* ecs_storage_get(
    ecs_column_t *s,
    ecs_size_t size,
    int32_t index)
{
    ecs_assert(index < s->count, ECS_OUT_OF_RANGE, NULL);
    return ECS_ELEM(s->array, size, index);
}

void* ecs_storage_last(
    ecs_column_t *s,
    ecs_size_t size)
{
    return ECS_ELEM(s->array, size, s->count - 1);
}

void* ecs_storage_first(
    ecs_column_t *s)
{
    return s->array;
}
