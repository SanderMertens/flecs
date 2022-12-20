/**
 * @file datastructures/block_allocator.c
 * @brief Block allocator.
 * 
 * A block allocator is an allocator for a fixed size that allocates blocks of
 * memory with N elements of the requested size.
 */

#include "../private_api.h"

// #ifdef FLECS_SANITIZE
// #define FLECS_USE_OS_ALLOC
// #define FLECS_MEMSET_UNINITIALIZED
// #endif

int64_t ecs_block_allocator_alloc_count = 0;
int64_t ecs_block_allocator_free_count = 0;

static
ecs_block_allocator_chunk_header_t* flecs_balloc_block(
    ecs_block_allocator_t *allocator)
{
    if (!allocator->chunk_size) {
        return NULL;
    }

    ecs_block_allocator_block_t *block = 
        ecs_os_malloc(ECS_SIZEOF(ecs_block_allocator_block_t) +
            allocator->block_size);
    ecs_block_allocator_chunk_header_t *first_chunk = ECS_OFFSET(block, 
        ECS_SIZEOF(ecs_block_allocator_block_t));

    block->memory = first_chunk;
    if (!allocator->block_tail) {
        ecs_assert(!allocator->block_head, ECS_INTERNAL_ERROR, 0);
        block->next = NULL;
        allocator->block_head = block;
        allocator->block_tail = block;
    } else {
        block->next = NULL;
        allocator->block_tail->next = block;
        allocator->block_tail = block;
    }

    ecs_block_allocator_chunk_header_t *chunk = first_chunk;
    int32_t i, end;
    for (i = 0, end = allocator->chunks_per_block - 1; i < end; ++i) {
        chunk->next = ECS_OFFSET(chunk, allocator->chunk_size);
        chunk = chunk->next;
    }

    ecs_os_linc(&ecs_block_allocator_alloc_count);

    chunk->next = NULL;
    return first_chunk;
}

void flecs_ballocator_init(
    ecs_block_allocator_t *ba,
    ecs_size_t size)
{
    ecs_assert(ba != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
    ba->data_size = size;
#ifdef FLECS_SANITIZE
    size += ECS_SIZEOF(int64_t);
#endif
    ba->chunk_size = ECS_ALIGN(size, 16);
    ba->chunks_per_block = ECS_MAX(4096 / ba->chunk_size, 1);
    ba->block_size = ba->chunks_per_block * ba->chunk_size;
    ba->head = NULL;
    ba->block_head = NULL;
    ba->block_tail = NULL;
}

ecs_block_allocator_t* flecs_ballocator_new(
    ecs_size_t size)
{
    ecs_block_allocator_t *result = ecs_os_calloc_t(ecs_block_allocator_t);
    flecs_ballocator_init(result, size);
    return result;
}

void flecs_ballocator_fini(
    ecs_block_allocator_t *ba)
{
    ecs_assert(ba != NULL, ECS_INTERNAL_ERROR, NULL);

#ifdef FLECS_SANITIZE
    ecs_assert(ba->alloc_count == 0, ECS_LEAK_DETECTED, NULL);
#endif

    ecs_block_allocator_block_t *block;
    for (block = ba->block_head; block;) {
        ecs_block_allocator_block_t *next = block->next;
        ecs_os_free(block);
        ecs_os_linc(&ecs_block_allocator_free_count);
        block = next;
    }
    ba->block_head = NULL;
}

void flecs_ballocator_free(
    ecs_block_allocator_t *ba)
{
    flecs_ballocator_fini(ba);
    ecs_os_free(ba);
}

void* flecs_balloc(
    ecs_block_allocator_t *ba) 
{
    void *result;
#ifdef FLECS_USE_OS_ALLOC
    result = ecs_os_malloc(ba->data_size);
#else

    if (!ba) return NULL;

    if (!ba->head) {
        ba->head = flecs_balloc_block(ba);
    }

    result = ba->head;
    ba->head = ba->head->next;

#ifdef FLECS_SANITIZE
    ecs_assert(ba->alloc_count >= 0, ECS_INTERNAL_ERROR, "corrupted allocator");
    ba->alloc_count ++;
    *(int64_t*)result = ba->chunk_size;
    result = ECS_OFFSET(result, ECS_SIZEOF(int64_t));
#endif
#endif

#ifdef FLECS_MEMSET_UNINITIALIZED
    ecs_os_memset(result, 0xAA, ba->data_size);
#endif

    return result;
}

void* flecs_bcalloc(
    ecs_block_allocator_t *ba) 
{
#ifdef FLECS_USE_OS_ALLOC
    return ecs_os_calloc(ba->data_size);
#endif

    if (!ba) return NULL;
    void *result = flecs_balloc(ba);
    ecs_os_memset(result, 0, ba->data_size);
    return result;
}

void flecs_bfree(
    ecs_block_allocator_t *ba, 
    void *memory) 
{
#ifdef FLECS_USE_OS_ALLOC
    ecs_os_free(memory);
    return;
#endif

    if (!ba) {
        ecs_assert(memory == NULL, ECS_INTERNAL_ERROR, NULL);
        return;
    }
    if (memory == NULL) {
        return;
    }

#ifdef FLECS_SANITIZE
    memory = ECS_OFFSET(memory, -ECS_SIZEOF(int64_t));
    if (*(int64_t*)memory != ba->chunk_size) {
        ecs_err("chunk %p returned to wrong allocator "
            "(chunk = %ub, allocator = %ub)",
                memory, *(int64_t*)memory, ba->chunk_size);
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }

    ba->alloc_count --;
#endif

    ecs_block_allocator_chunk_header_t *chunk = memory;
    chunk->next = ba->head;
    ba->head = chunk;
    ecs_assert(ba->alloc_count >= 0, ECS_INTERNAL_ERROR, "corrupted allocator");
}

void* flecs_brealloc(
    ecs_block_allocator_t *dst, 
    ecs_block_allocator_t *src, 
    void *memory)
{
    void *result;
#ifdef FLECS_USE_OS_ALLOC
    result = ecs_os_realloc(memory, dst->data_size);
#else
    if (dst == src) {
        return memory;
    }

    result = flecs_balloc(dst);
    if (result && src) {
        ecs_size_t size = src->data_size;
        if (dst->data_size < size) {
            size = dst->data_size;
        }
        ecs_os_memcpy(result, memory, size);
    }
    flecs_bfree(src, memory);
#endif
#ifdef FLECS_MEMSET_UNINITIALIZED
    if (dst && src && (dst->data_size > src->data_size)) {
        ecs_os_memset(ECS_OFFSET(result, src->data_size), 0xAA, 
            dst->data_size - src->data_size);
    } else if (dst && !src) {
        ecs_os_memset(result, 0xAA, dst->data_size);
    }
#endif

    return result;
}

void* flecs_bdup(
    ecs_block_allocator_t *ba, 
    void *memory)
{
#ifdef FLECS_USE_OS_ALLOC
    if (memory && ba->chunk_size) {
        return ecs_os_memdup(memory, ba->data_size);
    } else {
        return NULL;
    }
#endif

    void *result = flecs_balloc(ba);
    if (result) {
        ecs_os_memcpy(result, memory, ba->data_size);
    }
    return result;
}
