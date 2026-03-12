/**
 * @file datastructures/bitset.c
 * @brief Bitset data structure for arbitrary-length boolean storage.
 */

#include "../private_api.h"

/* Grow the backing storage to accommodate the requested number of bits. */
static
void flecs_bitset_ensure_size(
    ecs_bitset_t *bs,
    ecs_size_t size)
{
    if (!bs->size) {
        int32_t new_size = ((size - 1) / 64 + 1) * ECS_SIZEOF(uint64_t);
        bs->size = ((size - 1) / 64 + 1) * 64;
        bs->data = ecs_os_calloc(new_size);
    } else if (size > bs->size) {
        int32_t prev_size = ((bs->size - 1) / 64 + 1) * ECS_SIZEOF(uint64_t);
        bs->size = ((size - 1) / 64 + 1) * 64;
        int32_t new_size = ((size - 1) / 64 + 1) * ECS_SIZEOF(uint64_t);
        bs->data = ecs_os_realloc(bs->data, new_size);
        ecs_os_memset(ECS_OFFSET(bs->data, prev_size), 0, new_size - prev_size);
    }
}

/* Initialize a bitset to an empty state. */
void flecs_bitset_init(
    ecs_bitset_t* bs)
{
    bs->size = 0;
    bs->count = 0;
    bs->data = NULL;
}

/* Ensure the bitset can store at least the specified number of bits. */
void flecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count)
{
    if (count > bs->count) {
        bs->count = count;
        flecs_bitset_ensure_size(bs, count);
    }
}

/* Finalize the bitset and free its backing storage. */
void flecs_bitset_fini(
    ecs_bitset_t *bs)
{
    ecs_os_free(bs->data);
    bs->data = NULL;
    bs->count = 0;
}

/* Add the specified number of bits to the bitset. */
void flecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count)
{
    int32_t elem = bs->count += count;
    flecs_bitset_ensure_size(bs, elem);
}

/* Set the bit at the specified index to the given value. */
void flecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value)
{
    ecs_check(elem >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    uint32_t hi = ((uint32_t)elem) >> 6;
    uint32_t lo = ((uint32_t)elem) & 0x3F;
    uint64_t v = bs->data[hi];
    bs->data[hi] = (v & ~((uint64_t)1 << lo)) | ((uint64_t)value << lo);
error:
    return;
}

/* Get the value of the bit at the specified index. */
bool flecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_check(elem >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    return !!(bs->data[elem >> 6] & ((uint64_t)1 << ((uint64_t)elem & 0x3F)));
error:
    return false;
}

/* Return the number of bits in the bitset. */
int32_t flecs_bitset_count(
    const ecs_bitset_t *bs)
{
    return bs->count;
}

/* Remove a bit by swapping with the last element and shrinking. */
void flecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_check(elem >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    int32_t last = bs->count - 1;
    bool last_value = flecs_bitset_get(bs, last);
    flecs_bitset_set(bs, elem, last_value);
    flecs_bitset_set(bs, last, 0);
    bs->count --;
error:
    return;
}

/* Swap the values of two bits in the bitset. */
void flecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b)
{
    ecs_check(elem_a >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem_b >= 0, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem_a < bs->count, ECS_INVALID_PARAMETER, NULL);
    ecs_check(elem_b < bs->count, ECS_INVALID_PARAMETER, NULL);

    bool a = flecs_bitset_get(bs, elem_a);
    bool b = flecs_bitset_get(bs, elem_b);
    flecs_bitset_set(bs, elem_a, b);
    flecs_bitset_set(bs, elem_b, a);
error:
    return;
}
