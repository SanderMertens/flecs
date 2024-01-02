/**
 * @file datastructures/vec.c
 * @brief Vector with allocator support.
 */

#include "../private_api.h"

ecs_vec_t* ecs_vec_init(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(alignment != 0, ECS_INVALID_PARAMETER, NULL);
    v->array = NULL;
    v->count = 0;
    v->size = elem_count;
    v->mem = NULL;
    if (elem_count) {
        if (allocator) {
            v->mem = flecs_alloc(allocator, size * elem_count + alignment);
        } else {
            v->mem = ecs_os_malloc(size * elem_count + alignment);
        }
        v->array = ECS_ALIGN_PTR(v->mem, alignment);
    }
#ifdef FLECS_SANITIZE
    v->elem_size = size;
    v->elem_alignment = alignment;
#endif
    return v;
}

void ecs_vec_init_if(
    ecs_vec_t *vec,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_san_assert(!vec->elem_size || vec->elem_size == size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(!vec->elem_alignment || vec->elem_alignment == alignment, ECS_INVALID_PARAMETER, NULL);
    (void)vec;
    (void)size;
    (void)alignment;
#ifdef FLECS_SANITIZE
    if (!vec->elem_size) {
        ecs_assert(vec->array == NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(vec->count == 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(vec->size == 0, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(vec->mem == NULL, ECS_INTERNAL_ERROR, NULL);
        vec->elem_size = size;
        vec->elem_alignment = alignment;
    }
#endif
}

void ecs_vec_fini(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment)
{
    if (v->array) {
        ecs_san_assert(!size || size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
        ecs_san_assert(!alignment || alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
        if (allocator) {
            flecs_free(allocator, size * v->size + alignment, v->mem);
        } else {
            ecs_os_free(v->mem);
        }
        v->array = NULL;
        v->count = 0;
        v->size = 0;
        v->mem = NULL;
    }
}

ecs_vec_t* ecs_vec_reset(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment)
{
    if (!v->size) {
        ecs_vec_init(allocator, v, size, alignment, 0);
    } else {
        ecs_san_assert(size == v->elem_size, ECS_INTERNAL_ERROR, NULL);
        ecs_san_assert(alignment == v->elem_alignment, ECS_INTERNAL_ERROR, NULL);
        ecs_vec_clear(v);
    }
    return v;
}

void ecs_vec_clear(
    ecs_vec_t *vec)
{
    vec->count = 0;
}

ecs_vec_t ecs_vec_copy(
    ecs_allocator_t *allocator,
    const ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
    (void)alignment;
    void *mem = NULL;
    void *array = NULL;
    if (v->mem) {
        if (allocator) {
            mem = flecs_alloc(allocator, size * v->size + alignment);
        } else {
            mem = ecs_os_malloc(size * v->size + alignment);
        }
        array = ECS_ALIGN_PTR(mem, alignment);
        
        ecs_os_memcpy(array, v->array, size * v->size);
    }
    return (ecs_vec_t) {
        .array = array,
        .count = v->count,
        .size = v->size,
        .mem = mem
#ifdef FLECS_SANITIZE
        , .elem_size = size
        , .elem_alignment = alignment
#endif
    };
}

void ecs_vec_reclaim(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
    (void)alignment;
    int32_t count = v->count;
    if (count < v->size) {
        if (count) {
            size_t old_offset = (uintptr_t)(v->array) - (uintptr_t)(v->mem);

            if (allocator) {
                v->mem = flecs_realloc(
                    allocator, size * count + alignment, v->mem ? size * v->size + alignment : 0, v->mem);
            } else {
                v->mem = ecs_os_realloc(v->mem, size * count + alignment);
            }
            v->array = ECS_ALIGN_PTR(v->mem, alignment);

            // Shift memory if the alignment of the allocated memory is different
            size_t new_offset = (uintptr_t)(v->array) - (uintptr_t)(v->mem);
            if (old_offset != new_offset) {
                ecs_os_memmove(v->array, ECS_OFFSET(v->mem, old_offset), size * count);
            }

            v->size = count;
        } else {
            ecs_vec_fini(allocator, v, size, alignment);
        }
    }
}

void ecs_vec_set_size(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
    (void)alignment;
    if (v->size != elem_count) {
        if (elem_count < v->count) {
            elem_count = v->count;
        }

        elem_count = flecs_next_pow_of_2(elem_count);
        if (elem_count < 2) {
            elem_count = 2;
        }
        if (elem_count != v->size) {
            size_t old_offset = (uintptr_t)(v->array) - (uintptr_t)(v->mem);

            if (allocator) {
                v->mem = flecs_realloc(
                    allocator, size * elem_count + alignment, v->mem ? size * v->size + alignment : 0, v->mem);
            } else {
                v->mem = ecs_os_realloc(v->mem, size * elem_count + alignment);
            }
            v->array = ECS_ALIGN_PTR(v->mem, alignment);

            // Shift memory if the alignment of the allocated memory is different
            size_t new_offset = (uintptr_t)(v->array) - (uintptr_t)(v->mem);
            if (old_offset != new_offset) {
                ecs_os_memmove(v->array, ECS_OFFSET(v->mem, old_offset), size * elem_count);
            }

            v->size = elem_count;
        }
    }
}

void ecs_vec_set_min_size(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    if (elem_count > vec->size) {
        ecs_vec_set_size(allocator, vec, size, alignment, elem_count);
    }
}

void ecs_vec_set_min_count(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    ecs_vec_set_min_size(allocator, vec, size, alignment, elem_count);
    if (vec->count < elem_count) {
        vec->count = elem_count;
    }
}

void ecs_vec_set_min_count_zeromem(
    struct ecs_allocator_t *allocator,
    ecs_vec_t *vec,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    int32_t count = vec->count;
    if (count < elem_count) {
        ecs_vec_set_min_count(allocator, vec, size, alignment, elem_count);
        ecs_os_memset(ECS_ELEM(vec->array, size, count), 0,
            size * (elem_count - count));
    }
}

void ecs_vec_set_count(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
    if (v->count != elem_count) {
        if (v->size < elem_count) {
            ecs_vec_set_size(allocator, v, size, alignment, elem_count);
        }

        v->count = elem_count;
    }
}

void* ecs_vec_grow(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t elem_count)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem_count > 0, ECS_INTERNAL_ERROR, NULL);
    int32_t count = v->count;
    ecs_vec_set_count(allocator, v, size, alignment, count + elem_count);
    return ECS_ELEM(v->array, size, count);
}

void* ecs_vec_append(
    ecs_allocator_t *allocator,
    ecs_vec_t *v,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_san_assert(alignment == v->elem_alignment, ECS_INVALID_PARAMETER, NULL);
    int32_t count = v->count;
    if (v->size == count) {
        ecs_vec_set_size(allocator, v, size, alignment, count + 1);
    }
    v->count = count + 1;
    return ECS_ELEM(v->array, size, count);
}

void ecs_vec_remove(
    ecs_vec_t *v,
    ecs_size_t size,
    int32_t index)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index < v->count, ECS_OUT_OF_RANGE, NULL);
    if (index == --v->count) {
        return;
    }

    ecs_os_memcpy(
        ECS_ELEM(v->array, size, index),
        ECS_ELEM(v->array, size, v->count),
        size);
}

void ecs_vec_remove_last(
    ecs_vec_t *v)
{
    v->count --;
}

int32_t ecs_vec_count(
    const ecs_vec_t *v)
{
    return v->count;
}

int32_t ecs_vec_size(
    const ecs_vec_t *v)
{
    return v->size;
}

void* ecs_vec_get(
    const ecs_vec_t *v,
    ecs_size_t size,
    int32_t index)
{
    ecs_san_assert(size == v->elem_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index < v->count, ECS_OUT_OF_RANGE, NULL);
    return ECS_ELEM(v->array, size, index);
}

void* ecs_vec_last(
    const ecs_vec_t *v,
    ecs_size_t size)
{
    ecs_san_assert(!v->elem_size || size == v->elem_size,
        ECS_INVALID_PARAMETER, NULL);
    return ECS_ELEM(v->array, size, v->count - 1);
}

void* ecs_vec_first(
    const ecs_vec_t *v)
{
    return v->array;
}
