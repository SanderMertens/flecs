#include "../private_api.h"

struct ecs_vector_t {
    int32_t count;
    int32_t size;
    
#ifndef FLECS_NDEBUG
    int64_t elem_size; /* Used in debug mode to validate size */
#endif
};

/** Resize the vector buffer */
static
ecs_vector_t* resize(
    ecs_vector_t *vector,
    int16_t offset,
    int32_t size)
{
    ecs_vector_t *result = ecs_os_realloc(vector, offset + size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, 0);
    return result;
}

/* -- Public functions -- */

ecs_vector_t* _ecs_vector_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result =
        ecs_os_malloc(offset + elem_size * elem_count);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->count = 0;
    result->size = elem_count;
#ifndef FLECS_NDEBUG
    result->elem_size = elem_size;
#endif
    return result;
}

ecs_vector_t* _ecs_vector_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result =
        ecs_os_malloc(offset + elem_size * elem_count);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    ecs_os_memcpy(ECS_OFFSET(result, offset), array, elem_size * elem_count);

    result->count = elem_count;
    result->size = elem_count;
#ifndef FLECS_NDEBUG
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

void _ecs_vector_zero(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    void *array = ECS_OFFSET(vector, offset);
    ecs_os_memset(array, 0, elem_size * vector->count);
}

void ecs_vector_assert_size(
    ecs_vector_t *vector,
    ecs_size_t elem_size)
{
    (void)elem_size;
    
    if (vector) {
        ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    }
}

void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_assert(array_inout != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (elem_count == 1) {
        return _ecs_vector_add(array_inout, elem_size, offset);
    }
    
    ecs_vector_t *vector = *array_inout;
    if (!vector) {
        vector = _ecs_vector_new(elem_size, offset, 1);
        *array_inout = vector;
    }

    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    int32_t max_count = vector->size;
    int32_t old_count = vector->count;
    int32_t new_count = old_count + elem_count;

    if ((new_count - 1) >= max_count) {
        if (!max_count) {
            max_count = elem_count;
        } else {
            while (max_count < new_count) {
                max_count *= 2;
            }
        }

        max_count = flecs_next_pow_of_2(max_count);
        vector = resize(vector, offset, max_count * elem_size);
        vector->size = max_count;
        *array_inout = vector;
    }

    vector->count = new_count;

    return ECS_OFFSET(vector, offset + elem_size * old_count);
}

void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset)
{
    ecs_assert(array_inout != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_vector_t *vector = *array_inout;
    int32_t count, size;

    if (vector) {
        ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
        count = vector->count;
        size = vector->size;

        if (count >= size) {
            size *= 2;
            if (!size) {
                size = 2;
            }

            size = flecs_next_pow_of_2(size);
            vector = resize(vector, offset, size * elem_size);
            *array_inout = vector;
            vector->size = size;
        }

        vector->count = count + 1;
        return ECS_OFFSET(vector, offset + elem_size * count);
    }

    vector = _ecs_vector_new(elem_size, offset, 2);
    *array_inout = vector;
    vector->count = 1;
    vector->size = 2;
    return ECS_OFFSET(vector, offset);
}

void* _ecs_vector_insert_at(
    ecs_vector_t **vec,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_assert(vec != NULL, ECS_INTERNAL_ERROR, NULL);
    int32_t count = vec[0]->count;
    if (index == count) {
        return _ecs_vector_add(vec, elem_size, offset);
    }
    ecs_assert(index < count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    _ecs_vector_add(vec, elem_size, offset);
    void *start = _ecs_vector_get(*vec, elem_size, offset, index);
    if (index < count) {
        ecs_os_memmove(ECS_OFFSET(start, elem_size), start, 
            (count - index) * elem_size);
    }

    return start;
}

int32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    if (dst && *dst) {
        ecs_dbg_assert((*dst)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    }
    ecs_dbg_assert(src->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    void *dst_elem = _ecs_vector_add(dst, elem_size, offset);
    void *src_elem = _ecs_vector_get(src, elem_size, offset, index);

    ecs_os_memcpy(dst_elem, src_elem, elem_size);
    return _ecs_vector_remove(src, elem_size, offset, index);
}

void ecs_vector_remove_last(
    ecs_vector_t *vector)
{
    if (vector && vector->count) vector->count --;
}

bool _ecs_vector_pop(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    void *value)
{
    if (!vector) {
        return false;
    }

    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    int32_t count = vector->count;
    if (!count) {
        return false;
    }

    void *elem = ECS_OFFSET(vector, offset + (count - 1) * elem_size);

    if (value) {
        ecs_os_memcpy(value, elem, elem_size);
    }

    ecs_vector_remove_last(vector);

    return true;
}

int32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);
    void *elem = ECS_OFFSET(buffer, index * elem_size);

    ecs_assert(index < count, ECS_INVALID_PARAMETER, NULL);

    count --;
    if (index != count) {
        void *last_elem = ECS_OFFSET(buffer, elem_size * count);
        ecs_os_memcpy(elem, last_elem, elem_size);
    }

    vector->count = count;

    return count;
}

void _ecs_vector_reclaim(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset)
{
    ecs_vector_t *vector = *array_inout;
    if (!vector) {
        return;
    }

    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    int32_t size = vector->size;
    int32_t count = vector->count;

    if (count < size) {
        if (count) {
            size = count;
            vector = resize(vector, offset, size * elem_size);
            vector->size = size;
            *array_inout = vector;
        } else {
            ecs_vector_free(vector);
            *array_inout = NULL;
        }
    }
}

int32_t ecs_vector_count(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->count;
}

int32_t ecs_vector_size(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->size;
}

int32_t _ecs_vector_set_size(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_vector_t *vector = *array_inout;

    if (!vector) {
        *array_inout = _ecs_vector_new(elem_size, offset, elem_count);
        return elem_count;
    } else {
        ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

        int32_t result = vector->size;

        if (elem_count < vector->count) {
            elem_count = vector->count;
        }

        if (result < elem_count) {
            elem_count = flecs_next_pow_of_2(elem_count);
            vector = resize(vector, offset, elem_count * elem_size);
            vector->size = elem_count;
            *array_inout = vector;
            result = elem_count;
        }

        return result;
    }
}

int32_t _ecs_vector_grow(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    int32_t current = ecs_vector_count(*array_inout);
    return _ecs_vector_set_size(array_inout, elem_size, offset, current + elem_count);
}

int32_t _ecs_vector_set_count(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    if (!*array_inout) {
        *array_inout = _ecs_vector_new(elem_size, offset, elem_count);
    }

    ecs_dbg_assert((*array_inout)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    (*array_inout)->count = elem_count;
    ecs_size_t size = _ecs_vector_set_size(array_inout, elem_size, offset, elem_count);
    return size;
}

void* _ecs_vector_first(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    (void)elem_size;

    ecs_dbg_assert(!vector || vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    if (vector && vector->size) {
        return ECS_OFFSET(vector, offset);
    } else {
        return NULL;
    }
}

void* _ecs_vector_get(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_assert(vector != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(index < vector->count, ECS_INTERNAL_ERROR, NULL);

    return ECS_OFFSET(vector, offset + elem_size * index);
}

void* _ecs_vector_last(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    if (vector) {
        ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
        int32_t count = vector->count;
        if (!count) {
            return NULL;
        } else {
            return ECS_OFFSET(vector, offset + elem_size * (count - 1));
        }
    } else {
        return NULL;
    }
}

int32_t _ecs_vector_set_min_size(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    if (!*vector_inout || (*vector_inout)->size < elem_count) {
        return _ecs_vector_set_size(vector_inout, elem_size, offset, elem_count);
    } else {
        return (*vector_inout)->size;
    }
}

int32_t _ecs_vector_set_min_count(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    _ecs_vector_set_min_size(vector_inout, elem_size, offset, elem_count);

    ecs_vector_t *v = *vector_inout;
    if (v && v->count < elem_count) {
        v->count = elem_count;
    }

    return v->count;
}

void _ecs_vector_sort(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    ecs_comparator_t compare_action)
{
    if (!vector) {
        return;
    }

    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    

    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);

    if (count > 1) {
        qsort(buffer, (size_t)count, (size_t)elem_size, compare_action);
    }
}

void _ecs_vector_memory(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t *allocd,
    int32_t *used)
{
    if (!vector) {
        return;
    }

    ecs_dbg_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    if (allocd) {
        *allocd += vector->size * elem_size + offset;
    }
    if (used) {
        *used += vector->count * elem_size;
    }
}

ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset)
{
    if (!src) {
        return NULL;
    }

    ecs_vector_t *dst = _ecs_vector_new(elem_size, offset, src->size);
    ecs_os_memcpy(dst, src, offset + elem_size * src->count);
    return dst;
}
