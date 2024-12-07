/**
 * @file datastructures/bitset.c
 * @brief Bitset data structure.
 * 
 * Simple bitset implementation. The bitset allows for storage of arbitrary
 * numbers of bits.
 */

#include "../private_api.h"

static inline
void ensure(
    ecs_bitset_t *bs,
    ecs_size_t size)
{
    if (!bs->size) {
        const int32_t new_size = ((size - 1) / 64 + 1) * ECS_SIZEOF(uint64_t);
        bs->size = ((size - 1) / 64 + 1) * 64;
        bs->data = ecs_os_calloc(new_size);
    } else if (size > bs->size) {
        const int32_t prev_size = ((bs->size - 1) / 64 + 1) * ECS_SIZEOF(uint64_t);
        bs->size = ((size - 1) / 64 + 1) * 64;
        const int32_t new_size = ((size - 1) / 64 + 1) * ECS_SIZEOF(uint64_t);
        bs->data = ecs_os_realloc(bs->data, new_size);
        ecs_os_memset(ECS_OFFSET(bs->data, prev_size), 0, new_size - prev_size);
    }
}

static inline void set_bit(uint64_t *word, uint32_t lo, bool value) {
    *word = (*word & ~((uint64_t)1 << lo)) | ((uint64_t)value << lo);
}

static inline bool get_bit(uint64_t word, uint32_t lo) {
    return !!(word & ((uint64_t)1 << lo));
}

void flecs_bitset_init(
    ecs_bitset_t* bs)
{
    bs->size = 0;
    bs->count = 0;
    bs->data = NULL;
}

void flecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count)
{
    if (count > bs->count) {
        bs->count = count;
        ensure(bs, count);
    }
}

void flecs_bitset_fini(
    ecs_bitset_t *bs)
{
    ecs_os_free(bs->data);
    bs->data = NULL;
    bs->count = 0;
}

void flecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count)
{
    const int32_t elem = bs->count += count;
    ensure(bs, elem);
}

void flecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value)
{
    ecs_check(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    const uint32_t hi = ((uint32_t)elem) >> 6;
    const uint32_t lo = ((uint32_t)elem) & 0x3F;
    set_bit(&bs->data[hi], lo, value);
    error:
        return;
}

bool flecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_check(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    const uint32_t hi = elem >> 6;
    const uint32_t lo = elem & 0x3F;
    return get_bit(bs->data[hi], lo);
error:
    return false;
}

int32_t flecs_bitset_count(
    const ecs_bitset_t *bs)
{
    return bs->count;
}

void flecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_check(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    const int32_t last = bs->count - 1;
    if (elem != last) {
        bool last_value = flecs_bitset_get(bs, last);
        flecs_bitset_set(bs, elem, last_value);
    }
    flecs_bitset_set(bs, last, 0);
    bs->count --;
    error:
        return;
}

void flecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b)
{
    ecs_check(elem_a < bs->count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem_b < bs->count, ECS_INVALID_PARAMETER, NULL);

    bool a = flecs_bitset_get(bs, elem_a);
    bool b = flecs_bitset_get(bs, elem_b);
    flecs_bitset_set(bs, elem_a, b);
    flecs_bitset_set(bs, elem_b, a);
error:
    return;
}

void flecs_bitset_set_range(
    ecs_bitset_t* bs,
    int32_t start,
    int32_t stop,
    bool value)
{
    ecs_check(start < bs->count && stop < bs->count, ECS_INVALID_PARAMETER, NULL);

    for (int32_t elem = start; elem < stop; elem++) {
        const uint32_t hi = ((uint32_t)elem) >> 6;
        const uint32_t lo = ((uint32_t)elem) & 0x3F;
        set_bit(&bs->data[hi], lo, value);
    }
    error:
        return;
}
