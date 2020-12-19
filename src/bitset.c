
#include "flecs.h"

static
void ensure(
    ecs_bitset_t *bs,
    ecs_size_t size)
{
    if (size > bs->size) {
        bs->size = ((size - 1) / 64 + 1) * 64;
        bs->data = ecs_os_realloc(bs->data, 
            ((size - 1) / 64 + 1) * sizeof(uint64_t));
    }
}

void ecs_bitset_init(
    ecs_bitset_t* bs)
{
    bs->size = 0;
    bs->count = 0;
    bs->data = NULL;
}

void ecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count)
{
    if (count > bs->count) {
        bs->count = count;
        ensure(bs, count);
    }
}

void ecs_bitset_deinit(
    ecs_bitset_t *bs)
{
    ecs_os_free(bs->data);
}

void ecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count)
{
    int32_t elem = bs->count += count;
    ensure(bs, elem);
}

void ecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value)
{
    ecs_assert(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    int32_t hi = elem >> 6;
    int32_t lo = elem & 0x3F;
    uint64_t v = bs->data[hi];
    bs->data[hi] = (v & ~(1 << lo)) | (value << lo);
}

bool ecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_assert(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    return !!(bs->data[elem >> 6] & (1 << (elem & 0x3F)));
}

void ecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_assert(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    int32_t last = bs->count - 1;
    bool last_value = ecs_bitset_get(bs, last);
    ecs_bitset_set(bs, elem, last_value);
    bs->count --;
}

void ecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b)
{
    ecs_assert(elem_a < bs->count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem_b < bs->count, ECS_INVALID_PARAMETER, NULL);

    bool a = ecs_bitset_get(bs, elem_a);
    bool b = ecs_bitset_get(bs, elem_b);
    ecs_bitset_set(bs, elem_a, b);
    ecs_bitset_set(bs, elem_b, a);
}
