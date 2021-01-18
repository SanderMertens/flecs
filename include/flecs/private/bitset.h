/**
 * @file bitset.h
 * @brief Bitset datastructure.
 *
 * Simple bitset implementation. The bitset allows for storage of arbitrary
 * numbers of bits.
 */

#ifndef FLECS_BITSET_H
#define FLECS_BITSET_H

#include "api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_bitset_t {
    uint64_t *data;
    int32_t count;
    ecs_size_t size;
} ecs_bitset_t;

/** Initialize bitset. */
void ecs_bitset_init(
    ecs_bitset_t *bs);

/** Deinialize bitset. */
void ecs_bitset_deinit(
    ecs_bitset_t *bs);

/** Add n elements to bitset. */
void ecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count);

/** Ensure element exists. */
void ecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count);

/** Set element. */
void ecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value);

/** Get element. */
bool ecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem);

/** Return number of elements. */
int32_t ecs_bitset_count(
    const ecs_bitset_t *bs);

/** Remove from bitset. */
void ecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem);

/** Swap values in bitset. */
void ecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b);

#ifdef __cplusplus
}
#endif

#endif
