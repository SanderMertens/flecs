#include "types.h"

struct ecs_vector_t {
    uint32_t count;
    size_t size;
#ifndef NDEBUG
    size_t elem_size;
#endif
};

#define ARRAY_BUFFER(vector) ECS_OFFSET(vector, sizeof(ecs_vector_t))

/** Resize the vector buffer */
static
ecs_vector_t* resize(
    ecs_vector_t *vector,
    size_t size)
{
    ecs_vector_t *result = ecs_os_realloc(vector, sizeof(ecs_vector_t) + size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, 0);
    return result;
}

/* -- Public functions -- */

ecs_vector_t* _ecs_vector_new(
    size_t elem_size,
    size_t size)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result = ecs_os_malloc(sizeof(ecs_vector_t) + size * elem_size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->count = 0;
    result->size = size;
#ifndef NDEBUG
    result->elem_size = elem_size;
#endif
    return result;
}

void ecs_vector_free(
    ecs_vector_t *vector)
{
    ecs_os_free(vector);
}

void ecs_vector_clear(
    ecs_vector_t *vector)
{
    if (vector) {
        vector->count = 0;
    }
}

void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    size_t elem_size,
    uint32_t n_elems)
{
    ecs_vector_t *vector = *array_inout;
    if (!vector) {
        vector = _ecs_vector_new(elem_size, 1);
        *array_inout = vector;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    size_t size = vector->size;
    uint32_t old_count = vector->count;
    uint32_t count = old_count + n_elems;

    if ((count - 1) >= size) {
        if (!size) {
            size = n_elems;
        } else {
            while (size < count) {
                size *= 2;
            }
        }

        vector = resize(vector, size * elem_size);
        vector->size = size;
        *array_inout = vector;
    }

    vector->count = count;

    return ECS_OFFSET(ARRAY_BUFFER(vector), elem_size * old_count);
}

void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    size_t elem_size)
{
    ecs_vector_t *vector = *array_inout;

    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

        uint32_t count = vector->count;
        if (count < vector->size) {
            vector->count ++;
            return ECS_OFFSET(ARRAY_BUFFER(vector), elem_size * count);
        }
    }

    return _ecs_vector_addn(array_inout, elem_size, 1);
}

uint32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    size_t elem_size,
    uint32_t index)
{
    ecs_assert((*dst)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    void *dst_elem = _ecs_vector_add(dst, elem_size);
    void *src_elem = _ecs_vector_get(src, elem_size, index);

    memcpy(dst_elem, src_elem, elem_size);
    return _ecs_vector_remove_index(src, elem_size, index);
}

uint32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    size_t elem_size,
    void *elem)
{
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    uint32_t count = vector->count;
    void *buffer = ARRAY_BUFFER(vector);
    uint32_t index = ((char*)elem - (char*)buffer) / elem_size;

    if (index >= count) {
        return count;
    }

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, elem_size * (count - 1));
        memcpy(elem, last_elem, elem_size);
    }

    count --;
    vector->count = count;

    return count;
}

void ecs_vector_remove_last(
    ecs_vector_t *vector)
{
    if (vector->count) vector->count --;
}

bool _ecs_vector_pop(
    ecs_vector_t *vector,
    size_t elem_size,
    void *value)
{
    if (!vector) {
        return false;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    uint32_t count = vector->count;
    if (!count) {
        return false;
    }

    void *buffer = ARRAY_BUFFER(vector);
    void *elem = ECS_OFFSET(buffer, (count - 1) * elem_size);

    if (value) {
        memcpy(value, elem, elem_size);
    }

    ecs_vector_remove_last(vector);

    return true;
}

uint32_t _ecs_vector_remove_index(
    ecs_vector_t *vector,
    size_t elem_size,
    uint32_t index)
{
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    uint32_t count = vector->count;
    void *buffer = ARRAY_BUFFER(vector);
    void *elem = ECS_OFFSET(buffer, index * elem_size);

    ecs_assert(index < count, ECS_INVALID_PARAMETER, NULL);

    if (index != (count - 1)) {
        void *last_elem = ECS_OFFSET(buffer, elem_size * (count - 1));
        memcpy(elem, last_elem, elem_size);
    }

    count --;
    vector->count = count;

    return count;
}

void _ecs_vector_reclaim(
    ecs_vector_t **array_inout,
    size_t elem_size)
{
    ecs_vector_t *vector = *array_inout;

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    size_t size = vector->size;
    uint32_t count = vector->count;

    if (count < size) {
        size = count;
        vector = resize(vector, size * elem_size);
        vector->size = size;
        *array_inout = vector;
    }
}

uint32_t ecs_vector_count(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->count;
}

uint32_t ecs_vector_size(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->size;
}

uint32_t _ecs_vector_set_size(
    ecs_vector_t **array_inout,
    size_t elem_size,
    size_t size)
{
    ecs_vector_t *vector = *array_inout;

    if (!vector) {
        *array_inout = _ecs_vector_new(elem_size, size);
        return size;
    } else {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

        uint32_t result = vector->size;

        if (size < vector->count) {
            size = vector->count;
        }

        if (result < size) {
            vector = resize(vector, size * elem_size);
            vector->size = size;
            *array_inout = vector;
            result = size;
        }

        return result;
    }
}

uint32_t _ecs_vector_grow(
    ecs_vector_t **array_inout,
    size_t elem_size,
    size_t size)
{
    uint32_t current = ecs_vector_count(*array_inout);
    return _ecs_vector_set_size(array_inout, elem_size, current + size);
}

uint32_t _ecs_vector_set_count(
    ecs_vector_t **array_inout,
    size_t elem_size,
    uint32_t count)
{
    if (!*array_inout) {
        *array_inout = _ecs_vector_new(elem_size, count);
    }

    ecs_assert((*array_inout)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    (*array_inout)->count = count;
    size_t size = _ecs_vector_set_size(array_inout, elem_size, count);
    return size;
}

uint32_t _ecs_vector_set_min_size(
    ecs_vector_t **vector_inout,
    size_t elem_size,
    uint32_t min_size)
{
    if (!*vector_inout || (*vector_inout)->size < min_size) {
        return ecs_vector_set_size(vector_inout, elem_size, min_size);
    } else {
        return (*vector_inout)->size;
    }
}

void* ecs_vector_first(
    const ecs_vector_t *vector)
{
    if (vector && vector->size) {
        return ARRAY_BUFFER(vector);
    } else {
        return NULL;
    }
}

void* _ecs_vector_get(
    const ecs_vector_t *vector,
    size_t elem_size,
    int32_t index)
{
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    uint32_t count = vector->count;

    if (index >= count) {
        return NULL;
    }

    return ECS_OFFSET(ARRAY_BUFFER(vector), elem_size * index);
}

void* _ecs_vector_last(
    const ecs_vector_t *vector,
    size_t elem_size)
{
    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
        uint32_t count = vector->count;
        return ECS_OFFSET(ARRAY_BUFFER(vector), elem_size * (count - 1));
    } else {
        return NULL;
    }
}

void _ecs_vector_sort(
    ecs_vector_t *vector,
    size_t elem_size,
    ecs_comparator_t compare_action)
{
    if (!vector) {
        return;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    

    uint32_t count = vector->count;
    void *buffer = ARRAY_BUFFER(vector);

    if (count) {
        qsort(buffer, count, elem_size, compare_action);
    }
}

void _ecs_vector_memory(
    const ecs_vector_t *vector,
    size_t elem_size,
    uint32_t *allocd,
    uint32_t *used)
{
    if (!vector) {
        return;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    if (allocd) {
        *allocd += vector->size * elem_size + sizeof(ecs_vector_t);
    }
    if (used) {
        *used += vector->count * elem_size;
    }
}

ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    size_t elem_size)
{
    if (!src) {
        return NULL;
    }

    ecs_vector_t *dst = _ecs_vector_new(elem_size, src->size);
    memcpy(dst, src, sizeof(ecs_vector_t) + elem_size * src->count);
    return dst;
}