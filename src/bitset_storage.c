#include "private_api.h"

typedef struct ecs_bitset_storage_t {
    ecs_storage_t *storage;
    ecs_bitset_t bitset;
    ecs_storage_t *data;
} ecs_bitset_storage_t;

static
void ecs_bitset_storage_fini(
    ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_fini(&impl->bitset);
    ecs_os_free(impl);
}

static
void* ecs_bitset_storage_push(
    ecs_world_t *world,
    ecs_storage_t *s,
    ecs_entity_t id)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_push_n(&impl->bitset, 1);
    return NULL;
}

static
void ecs_bitset_storage_push_n(
    ecs_world_t *world,
    ecs_storage_t *dst,
    int32_t count,
    const ecs_entity_t *ids,
    const ecs_storage_t *src,
    int32_t src_index)
{
    ecs_bitset_storage_t *dst_impl = dst->impl;
    ecs_bitset_push_n(&dst_impl->bitset, count);

    if (src) {
        ecs_bitset_storage_t *src_impl = src->impl;
        ecs_bitset_t *dst_bs = &dst_impl->bitset;
        ecs_bitset_t *src_bs = &src_impl->bitset;

        int i;
        for (i = 0; i < count; i ++) {
            ecs_bitset_set(dst_bs, i, ecs_bitset_get(src_bs, i + src_index));
        }
    }
}

static
void* ecs_bitset_storage_emplace(
    ecs_world_t *world,
    ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_push_n(&impl->bitset, 1);
    return NULL;
}

static
void ecs_bitset_storage_erase(
    ecs_world_t *world,
    ecs_storage_t *s,
    int32_t index,
    uint64_t id)
{
    (void)id;
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_erase(&impl->bitset, index);
}

static
void ecs_bitset_storage_pop(
    ecs_world_t *world,
    ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_pop(&impl->bitset);
}

static
void ecs_bitset_storage_swap(
    ecs_world_t *world,
    ecs_storage_t *s,
    int32_t index_a,
    int32_t index_b)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_swap(&impl->bitset, index_a, index_b);
}

static
void ecs_bitset_storage_merge(
    ecs_world_t *world,
    ecs_storage_t *dst,
    ecs_storage_t *src)
{
    ecs_bitset_storage_t *dst_impl = dst->impl;
    ecs_bitset_storage_t *src_impl = src->impl;

    ecs_bitset_t *dst_bs = &dst_impl->bitset;
    ecs_bitset_t *src_bs = &src_impl->bitset;

    if (!ecs_bitset_count(dst_bs)) {
        ecs_bitset_fini(dst_bs);
        dst_impl->bitset = *src_bs;
    } else {
        int32_t dst_count = ecs_bitset_count(dst_bs); 
        int32_t i, src_count = ecs_bitset_count(src_bs);
        ecs_bitset_push_n(dst_bs, src_count);

        for (i = 0; i < src_count; i ++) {
            ecs_bitset_set(dst_bs, i + dst_count,
                ecs_bitset_get(src_bs, i));
        }

        ecs_bitset_fini(src_bs);
    }

    *src_bs = (ecs_bitset_t){ 0 };
}

static
void ecs_bitset_storage_clear(
    ecs_world_t *world,
    ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_clear(&impl->bitset);
}

static
void ecs_bitset_storage_destruct(
    ecs_world_t *world,
    ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_fini(&impl->bitset);
}

static
void ecs_bitset_storage_reserve(
    ecs_world_t *world,
    ecs_storage_t *s,
    ecs_size_t size)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_reserve(&impl->bitset, size);
}

static
void ecs_bitset_storage_grow(
    ecs_world_t *world,
    ecs_storage_t *s,
    ecs_size_t count)
{
    ecs_bitset_storage_t *impl = s->impl;
    ecs_bitset_grow(&impl->bitset, count);
}

static
void* ecs_bitset_storage_get(
    const ecs_storage_t *s,
    int32_t index,
    uint64_t id)
{
    ecs_bitset_storage_t *impl = s->impl;

    if (ecs_bitset_get(&impl->bitset, index)) {
        if (impl->data) {
            return ecs_storage_get(impl->data, index, id);
        }
    }

    return NULL;
}

static
bool ecs_bitset_storage_has(
    const ecs_storage_t *s,
    int32_t index,
    uint64_t id)
{
    (void)id;

    ecs_bitset_storage_t *impl = s->impl;
    return ecs_bitset_get(&impl->bitset, index);
}

static
int32_t ecs_bitset_storage_count(
    const ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    return ecs_bitset_count(&impl->bitset);
}

static
void ecs_bitset_storage_ctor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id)
{
    (void)world;
    (void)storage;
    (void)index;
    (void)id;
}

static
void ecs_bitset_storage_dtor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index)
{
    (void)world;
    (void)storage;
    (void)index;
}

static
void ecs_bitset_storage_copy(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    const void *ptr)
{
    (void)world;
    (void)storage;
    (void)index;
    (void)ptr;
}

static
void ecs_bitset_storage_ctor_move_dtor(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    void *ptr)
{
    (void)world;
    (void)storage;
    (void)index;
    (void)ptr;
}

static
void ecs_bitset_storage_ctor_copy(
    ecs_world_t *world,
    ecs_storage_t *storage,
    int32_t index,
    ecs_entity_t id,
    const void *ptr)
{
    (void)world;
    (void)storage;
    (void)index;
    (void)id;
    (void)ptr;
}

static
ecs_storage_iter_t ecs_bitset_storage_iter(
    const ecs_storage_t *s)
{
    ecs_bitset_storage_t *impl = s->impl;
    return ecs_storage_iter(impl->data);
}

#define BS_MAX ((uint64_t)0xFFFFFFFFFFFFFFFF)

static
bool ecs_bitset_storage_next(
    const ecs_storage_t *s,
    ecs_storage_iter_t *iter)
{
    ecs_bitset_storage_t *impl = s->impl;

    /* Precomputed single-bit test */
    static const uint64_t bitmask[64] = {
    (uint64_t)1 << 0, (uint64_t)1 << 1, (uint64_t)1 << 2, (uint64_t)1 << 3,
    (uint64_t)1 << 4, (uint64_t)1 << 5, (uint64_t)1 << 6, (uint64_t)1 << 7,
    (uint64_t)1 << 8, (uint64_t)1 << 9, (uint64_t)1 << 10, (uint64_t)1 << 11,
    (uint64_t)1 << 12, (uint64_t)1 << 13, (uint64_t)1 << 14, (uint64_t)1 << 15,
    (uint64_t)1 << 16, (uint64_t)1 << 17, (uint64_t)1 << 18, (uint64_t)1 << 19,
    (uint64_t)1 << 20, (uint64_t)1 << 21, (uint64_t)1 << 22, (uint64_t)1 << 23,
    (uint64_t)1 << 24, (uint64_t)1 << 25, (uint64_t)1 << 26, (uint64_t)1 << 27,  
    (uint64_t)1 << 28, (uint64_t)1 << 29, (uint64_t)1 << 30, (uint64_t)1 << 31,
    (uint64_t)1 << 32, (uint64_t)1 << 33, (uint64_t)1 << 34, (uint64_t)1 << 35,  
    (uint64_t)1 << 36, (uint64_t)1 << 37, (uint64_t)1 << 38, (uint64_t)1 << 39,
    (uint64_t)1 << 40, (uint64_t)1 << 41, (uint64_t)1 << 42, (uint64_t)1 << 43,
    (uint64_t)1 << 44, (uint64_t)1 << 45, (uint64_t)1 << 46, (uint64_t)1 << 47,  
    (uint64_t)1 << 48, (uint64_t)1 << 49, (uint64_t)1 << 50, (uint64_t)1 << 51,
    (uint64_t)1 << 52, (uint64_t)1 << 53, (uint64_t)1 << 54, (uint64_t)1 << 55,  
    (uint64_t)1 << 56, (uint64_t)1 << 57, (uint64_t)1 << 58, (uint64_t)1 << 59,
    (uint64_t)1 << 60, (uint64_t)1 << 61, (uint64_t)1 << 62, (uint64_t)1 << 63
    };

    /* Precomputed test to verify if remainder of block is set (or not) */
    static const uint64_t bitmask_remain[64] = {
    BS_MAX, BS_MAX - (BS_MAX >> 63), BS_MAX - (BS_MAX >> 62),
    BS_MAX - (BS_MAX >> 61), BS_MAX - (BS_MAX >> 60), BS_MAX - (BS_MAX >> 59),
    BS_MAX - (BS_MAX >> 58), BS_MAX - (BS_MAX >> 57), BS_MAX - (BS_MAX >> 56), 
    BS_MAX - (BS_MAX >> 55), BS_MAX - (BS_MAX >> 54), BS_MAX - (BS_MAX >> 53), 
    BS_MAX - (BS_MAX >> 52), BS_MAX - (BS_MAX >> 51), BS_MAX - (BS_MAX >> 50), 
    BS_MAX - (BS_MAX >> 49), BS_MAX - (BS_MAX >> 48), BS_MAX - (BS_MAX >> 47), 
    BS_MAX - (BS_MAX >> 46), BS_MAX - (BS_MAX >> 45), BS_MAX - (BS_MAX >> 44), 
    BS_MAX - (BS_MAX >> 43), BS_MAX - (BS_MAX >> 42), BS_MAX - (BS_MAX >> 41), 
    BS_MAX - (BS_MAX >> 40), BS_MAX - (BS_MAX >> 39), BS_MAX - (BS_MAX >> 38), 
    BS_MAX - (BS_MAX >> 37), BS_MAX - (BS_MAX >> 36), BS_MAX - (BS_MAX >> 35), 
    BS_MAX - (BS_MAX >> 34), BS_MAX - (BS_MAX >> 33), BS_MAX - (BS_MAX >> 32), 
    BS_MAX - (BS_MAX >> 31), BS_MAX - (BS_MAX >> 30), BS_MAX - (BS_MAX >> 29), 
    BS_MAX - (BS_MAX >> 28), BS_MAX - (BS_MAX >> 27), BS_MAX - (BS_MAX >> 26), 
    BS_MAX - (BS_MAX >> 25), BS_MAX - (BS_MAX >> 24), BS_MAX - (BS_MAX >> 23), 
    BS_MAX - (BS_MAX >> 22), BS_MAX - (BS_MAX >> 21), BS_MAX - (BS_MAX >> 20), 
    BS_MAX - (BS_MAX >> 19), BS_MAX - (BS_MAX >> 18), BS_MAX - (BS_MAX >> 17), 
    BS_MAX - (BS_MAX >> 16), BS_MAX - (BS_MAX >> 15), BS_MAX - (BS_MAX >> 14), 
    BS_MAX - (BS_MAX >> 13), BS_MAX - (BS_MAX >> 12), BS_MAX - (BS_MAX >> 11), 
    BS_MAX - (BS_MAX >> 10), BS_MAX - (BS_MAX >> 9), BS_MAX - (BS_MAX >> 8), 
    BS_MAX - (BS_MAX >> 7), BS_MAX - (BS_MAX >> 6), BS_MAX - (BS_MAX >> 5), 
    BS_MAX - (BS_MAX >> 4), BS_MAX - (BS_MAX >> 3), BS_MAX - (BS_MAX >> 2),
    BS_MAX - (BS_MAX >> 1)
    };

    int32_t first = iter->offset;
    int32_t last = 0;
    ecs_bitset_t *bs = &impl->bitset;

    int32_t bs_count = bs->count;
    int32_t bs_block = first >> 6;
    int32_t bs_block_count = ((bs_count - 1) >> 6) + 1;

    if (bs_block >= bs_block_count) {
        goto done;
    }

    uint64_t *bs_data = bs->data;
    int32_t bs_start = first & 0x3F;

    /* Step 1: find the first non-empty block */
    uint64_t v = bs_data[bs_block];
    uint64_t remain = bitmask_remain[bs_start];
    while (!(v & remain)) {
        /* If no elements are remaining, move to next block */
        if ((++bs_block) >= bs_block_count) {
            /* No non-empty blocks left */
            goto done;
        }

        bs_start = 0;
        remain = BS_MAX; /* Test the full block */
        v = bs_data[bs_block];
    }

    /* Step 2: find the first non-empty element in the block */
    while (!(v & bitmask[bs_start])) {
        bs_start ++;

        /* Block was not empty, so bs_start must be smaller than 64 */
        ecs_assert(bs_start < 64, ECS_INTERNAL_ERROR, NULL);
    }
    
    /* Step 3: Find number of contiguous enabled elements after start */
    int32_t bs_end = bs_start, bs_block_end = bs_block;
    
    remain = bitmask_remain[bs_end];
    while ((v & remain) == remain) {
        bs_end = 0;
        bs_block_end ++;

        if (bs_block_end == bs_block_count) {
            break;
        }

        v = bs_data[bs_block_end];
        remain = BS_MAX; /* Test the full block */
    }

    /* Step 4: find remainder of enabled elements in current block */
    if (bs_block_end != bs_block_count) {
        while ((v & bitmask[bs_end])) {
            bs_end ++;
        }
    }

    /* Block was not 100% occupied, so bs_start must be smaller than 64 */
    ecs_assert(bs_end < 64, ECS_INTERNAL_ERROR, NULL);

    /* Step 5: translate to element start/end and make sure that each column
        * range is a subset of the previous one. */
    first = bs_block * 64 + bs_start;
    int32_t cur_last = bs_block_end * 64 + bs_end;
    
    /* No enabled elements found in table */
    if (first == cur_last) {
        goto done;
    }
    
    last = cur_last;
    int32_t elem_count = last - first;

    /* Make sure last element doesn't exceed total number of elements in 
        * the table */
    if (elem_count > bs_count) {
        elem_count = bs_count;
    }
    
    iter->offset = first;
    iter->count = elem_count;

    return true;
done:
    return false;
}

static const
ecs_storage_plugin_t plugin = {
    .fini = ecs_bitset_storage_fini,
    .push = ecs_bitset_storage_push,
    .push_n = ecs_bitset_storage_push_n,
    .emplace = ecs_bitset_storage_emplace,
    .erase = ecs_bitset_storage_erase,
    .pop = ecs_bitset_storage_pop,
    .swap = ecs_bitset_storage_swap,
    .merge = ecs_bitset_storage_merge,
    .clear = ecs_bitset_storage_clear,
    .destruct = ecs_bitset_storage_destruct,

    .reserve = ecs_bitset_storage_reserve,
    .grow = ecs_bitset_storage_grow,

    .get = ecs_bitset_storage_get,
    .has = ecs_bitset_storage_has,
    .count = ecs_bitset_storage_count,

    .ctor = ecs_bitset_storage_ctor,
    .dtor = ecs_bitset_storage_dtor,
    .copy = ecs_bitset_storage_copy,
    .ctor_move_dtor = ecs_bitset_storage_ctor_move_dtor,
    .ctor_copy = ecs_bitset_storage_ctor_copy,

    .iter = ecs_bitset_storage_iter,
    .next = ecs_bitset_storage_next,

    .is_sparse = true
};


/* Public functions */

void ecs_bitset_storage_init(
    ecs_world_t *world,
    ecs_storage_t *storage)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(storage != NULL, ECS_INTERNAL_ERROR, NULL);
    (void)world;
    
    ecs_bitset_storage_t *result = ecs_os_calloc_t(ecs_bitset_storage_t);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_bitset_init(&result->bitset);

    storage->plugin = &plugin;
    storage->impl = result;
}
