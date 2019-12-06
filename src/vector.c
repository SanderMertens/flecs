#include "types.h"

struct ecs_vector_t {
    uint32_t count;
    uint32_t size;
};

#define ARRAY_BUFFER(array) ECS_OFFSET(array, sizeof(ecs_vector_t))

/** Resize the array buffer */
static
ecs_vector_t* resize(
    ecs_vector_t *array,
    uint32_t size)
{
    ecs_vector_t *result = ecs_os_realloc(array, sizeof(ecs_vector_t) + size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, 0);
    return result;
}


/* -- Public functions -- */

ecs_vector_t* ecs_vector_new(
    const ecs_vector_params_t *params,
    uint32_t size)
{
    ecs_assert(params->element_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result = ecs_os_malloc(sizeof(ecs_vector_t) + size * params->element_size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->count = 0;
    result->size = size;
    return result;
}

void ecs_vector_free(
    ecs_vector_t *array)
{
    ecs_os_free(array);
}

void ecs_vector_clear(
    ecs_vector_t *array)
{
    if (array) {
        array->count = 0;
    }
}

void* ecs_vector_addn(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params,
    uint32_t n_elems)
{
    ecs_vector_t *array = *array_inout;
    if (!array) {
        array = ecs_vector_new(params, 1);
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

    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);

    return ECS_OFFSET(ARRAY_BUFFER(array), element_size * old_count);
}

void* ecs_vector_add(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params)
{
    return ecs_vector_addn(array_inout, params, 1);
}

uint32_t ecs_vector_move_index(
    ecs_vector_t **dst_array,
    ecs_vector_t *src_array,
    const ecs_vector_params_t *params,
    uint32_t index)
{
    void *dst_elem = ecs_vector_add(dst_array, params);
    void *src_elem = ecs_vector_get(src_array, params, index);
    memcpy(dst_elem, src_elem, params->element_size);

    ecs_assert((*dst_array)->size >= (*dst_array)->count, ECS_INTERNAL_ERROR, NULL);

    return ecs_vector_remove_index(src_array, params, index);
}

uint32_t ecs_vector_remove(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
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

    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);

    return count;
}

void ecs_vector_remove_last(
    ecs_vector_t *array)
{
    if (array->count) array->count --;
}

bool ecs_vector_pop(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *value)
{
    if (!array) {
        return false;
    }

    uint32_t count = array->count;
    if (!count) {
        return false;
    }

    void *buffer = ARRAY_BUFFER(array);
    uint32_t element_size = params->element_size;
    void *elem = ECS_OFFSET(buffer, (count - 1) * element_size);

    if (value) {
        memcpy(value, elem, element_size);
    }

    ecs_vector_remove_last(array);

    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);

    return true;
}

uint32_t ecs_vector_remove_index(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    uint32_t index)
{
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);
    void *elem = ECS_OFFSET(buffer, index * element_size);

    ecs_assert(index < count, ECS_INVALID_PARAMETER, NULL);

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

    ecs_assert(array->size >= count, ECS_INTERNAL_ERROR, NULL);

    return count;
}

void ecs_vector_reclaim(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params)
{
    ecs_vector_t *array = *array_inout;
    uint32_t size = array->size;
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;

    if (count < size) {
        size = count;
        array = resize(array, size * element_size);
        array->size = size;
        *array_inout = array;
    }

    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);    
}

uint32_t ecs_vector_count(
    const ecs_vector_t *array)
{
    if (!array) {
        return 0;
    }
    return array->count;
}

uint32_t ecs_vector_size(
    const ecs_vector_t *array)
{
    if (!array) {
        return 0;
    }
    return array->size;
}

uint32_t ecs_vector_set_size(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params,
    uint32_t size)
{
    ecs_vector_t *array = *array_inout;
    if (!array) {
        *array_inout = ecs_vector_new(params, size);
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

            ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);  
        }

        return result;
    }
}

uint32_t ecs_vector_set_count(
    ecs_vector_t **array_inout,
    const ecs_vector_params_t *params,
    uint32_t count)
{
    if (!*array_inout) {
        *array_inout = ecs_vector_new(params, count);
    }
    (*array_inout)->count = count;
    uint32_t size = ecs_vector_set_size(array_inout, params, count);

    ecs_assert(size >= count, ECS_INTERNAL_ERROR, NULL);

    return size;
}

void* ecs_vector_first(
    const ecs_vector_t *array)
{
    if (array && array->size) {
        return ARRAY_BUFFER(array);
    } else {
        return NULL;
    }
}

void* ecs_vector_get(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params,
    uint32_t index)
{
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;

    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);

    if (index >= count) {
        return NULL;
    }

    return ECS_OFFSET(ARRAY_BUFFER(array), element_size * index);
}

uint32_t ecs_vector_get_index(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params,
    void *elem)
{
    ecs_assert(elem != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);

    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);
    return ((char*)elem - (char*)buffer) / element_size;
}

void* ecs_vector_last(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params)
{
    if (array) {
        ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);
        uint32_t count = array->count;
        uint32_t element_size = params->element_size;
        return ECS_OFFSET(ARRAY_BUFFER(array), element_size * (count - 1));
    } else {
        return NULL;
    }
}

void ecs_vector_sort(
    ecs_vector_t *array,
    const ecs_vector_params_t *params,
    EcsComparator compare_action)
{
    if (!array)
        return;

    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);
    uint32_t count = array->count;
    uint32_t element_size = params->element_size;
    void *buffer = ARRAY_BUFFER(array);

    if (count) {
        qsort(buffer, count, element_size, compare_action);
    }
}

void ecs_vector_memory(
    const ecs_vector_t *array,
    const ecs_vector_params_t *params,
    uint32_t *allocd,
    uint32_t *used)
{
    if (!array) return;
    
    ecs_assert(array->size >= array->count, ECS_INTERNAL_ERROR, NULL);

    if (allocd) {
        *allocd += array->size * params->element_size + sizeof(ecs_vector_t);
    }
    if (used) {
        *used += array->count * params->element_size;
    }
}

ecs_vector_t* ecs_vector_copy(
    const ecs_vector_t *src,
    const ecs_vector_params_t *params)
{
    if (!src) {
        return NULL;
    }

    ecs_vector_t *dst = ecs_vector_new(params, src->size);
    memcpy(dst, src, sizeof(ecs_vector_t) + params->element_size * src->count);
    return dst;
}
