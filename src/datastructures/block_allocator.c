#include "../private_api.h"

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

    chunk->next = NULL;
    return first_chunk;
}

void flecs_ballocator_init(
    ecs_block_allocator_t *ba,
    ecs_size_t size)
{
    ecs_assert(ba != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(size != 0, ECS_INTERNAL_ERROR, NULL);
    uint32_t alignment_mask = flecs_ito(uint32_t, 16) - 1u;
    ba->chunk_size = (int32_t)(((uint32_t)size + 
        alignment_mask) & ~alignment_mask);
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
    ecs_block_allocator_block_t *block;
    for (block = ba->block_head; block;) {
        ecs_block_allocator_block_t *next = block->next;
        ecs_os_free(block);
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
    ecs_block_allocator_t *allocator) 
{
    if (!allocator) return NULL;

    if (!allocator->head) {
        allocator->head = flecs_balloc_block(allocator);
    }

    void *result = allocator->head;
    allocator->head = allocator->head->next;
    return result;
}

void* flecs_bcalloc(
    ecs_block_allocator_t *allocator) 
{
    if (!allocator) return NULL;
    void *result = flecs_balloc(allocator);
    ecs_os_memset(result, 0, allocator->chunk_size);
    return result;
}

void flecs_bfree(
    ecs_block_allocator_t *allocator, 
    void *memory) 
{
    if (!allocator) {
        ecs_assert(memory == NULL, ECS_INTERNAL_ERROR, NULL);
        return;
    }
    if (memory == NULL) {
        return;
    }
    ecs_block_allocator_chunk_header_t *chunk = memory;
    chunk->next = allocator->head;
    allocator->head = chunk;
}

void* flecs_brealloc(
    ecs_block_allocator_t *dst, 
    ecs_block_allocator_t *src, 
    void *memory)
{
    void *result = flecs_balloc(dst);
    if (result && src) {
        ecs_os_memcpy(result, memory, src->chunk_size);
    }
    flecs_bfree(src, memory);
    return result;
}

void* flecs_bdup(
    ecs_block_allocator_t *ba, 
    void *memory)
{
    void *result = flecs_balloc(ba);
    if (result) {
        ecs_os_memcpy(result, memory, ba->chunk_size);
    }
    return result;
}
