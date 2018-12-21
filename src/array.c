#include <string.h>
#include "include/private/types.h"

struct EcsArray {
    uint32_t count;
    uint32_t size;
};

#define ARRAY_BUFFER(array) ECS_OFFSET(array, sizeof(EcsArray))

/** Resize the array buffer */
static
EcsArray* resize(
    EcsArray *array,
    uint32_t size)
{
    return realloc(array, sizeof(EcsArray) + size);
}

/** Iterator hasnext callback */
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

/** Iterator next callback */
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

/* -- Public functions -- */

EcsArray* ecs_array_new(
    const EcsArrayParams *params,
    uint32_t size)
{
    EcsArray *result = malloc(sizeof(EcsArray) + size * params->element_size);
    result->count = 0;
    result->size = size;
    return result;
}

EcsArray* ecs_array_new_from_buffer(
    const EcsArrayParams *params,
    uint32_t size,
    void *buffer)
{
    EcsArray *result = ecs_array_new(params, size);
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

void ecs_array_clear(
    EcsArray *array)
{
    array->count = 0;
}

void* ecs_array_addn(
    EcsArray **array_inout,
    const EcsArrayParams *params,
    uint32_t n_elems)
{
    EcsArray *array = *array_inout;
    if (!array) {
        array = ecs_array_new(params, 1);
        *array_inout = array;
    }

    uint32_t size = array->size;
    uint32_t old_count = array->count;
    uint32_t count = old_count + n_elems;
    uint32_t element_size = params->element_size;

    if ((count - 1) >= size) {
        if (!size) {
            size = n_elems;
        } else {
            while (size < count) {
                size *= 2;
            }
        }

        array = resize(array, size * element_size);
        array->size = size;
        *array_inout = array;
    }

    array->count = count;

    return ECS_OFFSET(ARRAY_BUFFER(array), element_size * old_count);
}

void* ecs_array_add(
    EcsArray **array_inout,
    const EcsArrayParams *params)
{
    return ecs_array_addn(array_inout, params, 1);
}

uint32_t ecs_array_move_index(
    EcsArray **dst_array,
    EcsArray *src_array,
    const EcsArrayParams *params,
    uint32_t index)
{
    void *dst_elem = ecs_array_add(dst_array, params);
    void *src_elem = ecs_array_get(src_array, params, index);
    memcpy(dst_elem, src_elem, params->element_size);
    return ecs_array_remove_index(src_array, params, index);
}

uint32_t ecs_array_remove(
    EcsArray *array,
    const EcsArrayParams *params,
    void *elem)
{
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);
    uint32_t index = ((char*)elem - (char*)buffer) / element_size;

    if (index >= count) {
        return count;
    }

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, element_size * (count - 1));
        memcpy(elem, last_elem, element_size);
        if (params->move_action) {
            params->move_action(
                array, params, elem, last_elem, params->move_ctx);
        }
    }

    count --;
    array->count = count;

    return count;
}

uint32_t ecs_array_remove_index(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t index)
{
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);
    void *elem = ECS_OFFSET(buffer, index * element_size);

    if (index >= count) {
        return count;
    }

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, element_size * (count - 1));
        memcpy(elem, last_elem, element_size);
        if (params->move_action) {
            params->move_action(
                array, params, elem, last_elem, params->move_ctx);
        }
    }

    count --;
    array->count = count;

    return count;
}

void ecs_array_reclaim(
    EcsArray **array_inout,
    const EcsArrayParams *params)
{
    EcsArray *array = *array_inout;
    uint32_t size = array->size;
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;

    if (count < size) {
        size = count;
        array = resize(array, size * element_size);
        array->size = size;
        *array_inout = array;
    }
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

uint32_t ecs_array_set_size(
    EcsArray **array_inout,
    const EcsArrayParams *params,
    uint32_t size)
{
    EcsArray *array = *array_inout;
    if (!array) {
        *array_inout = ecs_array_new(params, size);
        return size;
    } else {
        uint32_t result = array->size;

        if (size < array->count) {
            size = array->count;
        }

        if (result < size) {
            array = resize(array, size * params->element_size);
            array->size = size;
            *array_inout = array;
            result = size;
        }

        return result;
    }
}

uint32_t ecs_array_set_count(
    EcsArray **array_inout,
    const EcsArrayParams *params,
    uint32_t count)
{
    (*array_inout)->count = count;
    uint32_t size = ecs_array_set_size(array_inout, params, count);
    return size;
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
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;

    if (index >= count) {
        return NULL;
    }

    return ECS_OFFSET(ARRAY_BUFFER(array), element_size * index);
}

uint32_t ecs_array_get_index(
    EcsArray *array,
    const EcsArrayParams *params,
    void *elem)
{
    if (!elem) {
        return 0;
    }
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);
    return ((char*)elem - (char*)buffer) / element_size;
}

void* ecs_array_last(
    EcsArray *array,
    const EcsArrayParams *params)
{
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;
    return ECS_OFFSET(ARRAY_BUFFER(array), element_size * (count - 1));
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
    const EcsArrayParams *params,
    EcsComparator compare_action)
{
    if (!array)
        return;
        
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);

    if (count) {
        qsort(buffer, count, element_size, compare_action);
    }
}

void ecs_array_memory(
    EcsArray *array,
    const EcsArrayParams *params,
    uint32_t *allocd,
    uint32_t *used)
{
    if (!array) return;
    if (allocd) {
        *allocd += array->size * params->element_size + sizeof(EcsArray);
    }
    if (used) {
        *used += array->count * params->element_size;
    }
}
