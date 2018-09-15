
#include <string.h>
#include "include/private/types.h"

struct EcsArray {
    uint16_t count;
    uint16_t size;
};

#define ARRAY_BUFFER(array) ECS_OFFSET(array, sizeof(EcsArray))

static
EcsArray* resize(
    EcsArray *array,
    uint16_t size)
{
    return realloc(array, sizeof(EcsArray) + size);
}

EcsArray* ecs_array_new(
    uint32_t size,
    const EcsArrayParams *params)
{
    EcsArray *result = malloc(sizeof(EcsArray) + size * params->element_size);
    result->count = 0;
    result->size = size;
    return result;
}

EcsArray* ecs_array_new_from_buffer(
    uint32_t size,
    const EcsArrayParams *params,
    void *buffer)
{
    EcsArray *result = ecs_array_new(size, params);
    void *my_buffer = ARRAY_BUFFER(result);
    memcpy(my_buffer, buffer, size * params->element_size);
    result->count = size;
    return result;
}

void ecs_array_free(
    EcsArray *array)
{
    free(array);
}

EcsArray* ecs_array_add(
    EcsArray *array,
    const EcsArrayParams *params,
    void *elem_out)
{
    uint16_t size = array->size;
    uint16_t count = array->count;
    uint32_t element_size = params->element_size;
    EcsArray *result = array;

    if (count == size) {
        if (!size) {
            size = 1;
        } else {
            size *= 2;
        }
        result = resize(array, size * element_size);
        result->size = size;
    }

    result->count = count + 1;
    *(void**)elem_out = ECS_OFFSET(ARRAY_BUFFER(result), element_size * count);

    return result;
}

EcsArray* ecs_array_remove(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index)
{
    uint16_t size = array->size;
    uint16_t count = array->count;
    uint32_t element_size = params->element_size;
    EcsArray *result = array;

    if (index >= count) {
        return NULL;
    }

    void *buffer = ARRAY_BUFFER(array);
    void *elem = ECS_OFFSET(buffer, index * element_size);

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, element_size * (count - 1));
        memcpy(elem, last_elem, element_size);
    }

    count --;

    if (count < size / 2) {
        size /= 2;
        result = resize(array, size * element_size);
        result->size = size;
    }

    result->count = count;

    return result;
}

uint32_t ecs_array_count(
    EcsArray *array)
{
    if (!array) {
        return 0;
    }
    return array->count;
}

uint32_t ecs_array_size(
    EcsArray *array)
{
    if (!array) {
        return 0;
    }
    return array->size;
}

void* ecs_array_buffer(
    EcsArray *array)
{
    return ARRAY_BUFFER(array);
}

void* ecs_array_get(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index)
{
    uint16_t count = array->count;
    uint32_t element_size = params->element_size;

    if (index >= count) {
        return NULL;
    }

    return ECS_OFFSET(ARRAY_BUFFER(array), element_size * index);
}

static
bool ecs_array_hasnext(
    EcsIter *iter)
{
    EcsArray *array = iter->ctx;
    EcsArrayIter *iter_data = iter->data;
    if (iter_data->index < array->count) {
        return true;
    } else {
        return false;
    }
}

static
void* ecs_array_next(
    EcsIter *iter)
{
    EcsArray *array = iter->ctx;
    EcsArrayIter *iter_data = iter->data;
    uint32_t index = iter_data->index;
    void *result = ECS_OFFSET(ARRAY_BUFFER(array),
        index * iter_data->params->element_size);
    iter_data->index = index + 1;
    return result;
}

EcsIter _ecs_array_iter(
    EcsArray *array,
    const EcsArrayParams *params,
    EcsArrayIter *iter_data)
{
    EcsIter result = {
        .hasnext = ecs_array_hasnext,
        .next = ecs_array_next,
        .ctx = array,
        .data = iter_data /* store index */
    };

    iter_data->params = (void*)params;
    iter_data->index = 0;

    return result;
}

void ecs_array_sort(
    EcsArray *array,
    const EcsArrayParams *params)
{
    uint16_t count = array->count;
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);

    if (count) {
        qsort(buffer, count, element_size, params->compare_action);
    }
}
