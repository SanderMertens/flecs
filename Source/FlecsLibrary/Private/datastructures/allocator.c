/**
 * @file datastructures/allocator.c
 * @brief Allocator for any size.
 * 
 * Allocators create a block allocator for each requested size.
 */

#include "../private_api.h"

#ifndef FLECS_USE_OS_ALLOC
static
ecs_size_t flecs_allocator_size(
    ecs_size_t size)
{
    return ECS_ALIGN(size, 16);
}

static
ecs_size_t flecs_allocator_size_hash(
    ecs_size_t size)
{
    return size >> 4;
}
#endif

void flecs_allocator_init(
    ecs_allocator_t *a)
{
    (void)a;
#ifndef FLECS_USE_OS_ALLOC
    flecs_ballocator_init_n(&a->chunks, ecs_block_allocator_t,
        FLECS_SPARSE_PAGE_SIZE);
    flecs_sparse_init_t(&a->sizes, NULL, &a->chunks, ecs_block_allocator_t);
#endif
}

void flecs_allocator_fini(
    ecs_allocator_t *a)
{
    (void)a;
#ifndef FLECS_USE_OS_ALLOC
    ecs_assert(a != NULL, ECS_INVALID_PARAMETER, NULL);

    int32_t i = 0, count = flecs_sparse_count(&a->sizes);
    for (i = 0; i < count; i ++) {
        ecs_block_allocator_t *ba = flecs_sparse_get_dense_t(
            &a->sizes, ecs_block_allocator_t, i);
        flecs_ballocator_fini(ba);
    }
    flecs_sparse_fini(&a->sizes);

    flecs_ballocator_fini(&a->chunks);
#endif
}

ecs_block_allocator_t* flecs_allocator_get(
    ecs_allocator_t *a, 
    ecs_size_t size)
{
#ifndef FLECS_USE_OS_ALLOC
    ecs_assert(size >= 0, ECS_INTERNAL_ERROR, NULL);
    if (!size) {
        return NULL;
    }

    ecs_assert(a != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size <= flecs_allocator_size(size), ECS_INTERNAL_ERROR, NULL);
    size = flecs_allocator_size(size);
    ecs_size_t hash = flecs_allocator_size_hash(size);
    ecs_block_allocator_t *result = flecs_sparse_get_t(&a->sizes, 
        ecs_block_allocator_t, (uint32_t)hash);

    if (!result) {
        result = flecs_sparse_ensure_fast_t(&a->sizes, 
            ecs_block_allocator_t, (uint32_t)hash);
        flecs_ballocator_init(result, size);
    }

    ecs_assert(result->data_size == size, ECS_INTERNAL_ERROR, NULL);

    return result;
#else
    (void)a;
    (void)size;
    ecs_err("invalid call to flecs_allocator_get in FLECS_USE_OS_ALLOC build");
    return NULL;
#endif
}

char* flecs_strdup(
    ecs_allocator_t *a, 
    const char* str)
{
#ifndef FLECS_USE_OS_ALLOC
    ecs_size_t len = ecs_os_strlen(str);
    char *result = flecs_alloc_n(a, char, len + 1);
    ecs_os_memcpy(result, str, len + 1);
    return result;
#else
    (void)a;
    return ecs_os_strdup(str);
#endif
}

void flecs_strfree(
    ecs_allocator_t *a, 
    char* str)
{
#ifndef FLECS_USE_OS_ALLOC
    ecs_size_t len = ecs_os_strlen(str);
    flecs_free_n(a, char, len + 1, str);
#else
    (void)a;
    ecs_os_free(str);
#endif
}

void* flecs_dup(
    ecs_allocator_t *a,
    ecs_size_t size,
    const void *src)
{
    if (!size) {
        return NULL;
    }
#ifndef FLECS_USE_OS_ALLOC
    ecs_block_allocator_t *ba = flecs_allocator_get(a, size);
    void *dst = flecs_balloc(ba);
    ecs_os_memcpy(dst, src, size);
    return dst;
#else
    (void)a;
    return ecs_os_memdup(src, size);
#endif
}

#ifdef FLECS_USE_OS_ALLOC

void* flecs_alloc(
    ecs_allocator_t *a, 
    ecs_size_t size) 
{
    (void)a;
    return ecs_os_malloc(size);
}

void* flecs_calloc(
    ecs_allocator_t *a, 
    ecs_size_t size) 
{
    (void)a;
    return ecs_os_calloc(size);
}

void* flecs_realloc(
    ecs_allocator_t *a, 
    ecs_size_t dst_size, 
    ecs_size_t src_size, 
    void *ptr) 
{
    (void)a;
    (void)src_size;
    return ecs_os_realloc(ptr, dst_size);
}

void flecs_free(
    ecs_allocator_t *a, 
    ecs_size_t size,
    void *ptr) 
{
    (void)a;
    (void)size;
    ecs_os_free(ptr);
}

#endif
