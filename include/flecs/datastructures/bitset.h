/**
 * @file bitset.h
 * @brief Bitset data structure.
 */

#ifndef FLECS_BITSET_H
#define FLECS_BITSET_H

#include "../private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** A bitset data structure for compact boolean storage. */
typedef struct ecs_bitset_t {
    uint64_t *data;           /**< Array of 64-bit words storing the bits. */
    int32_t count;            /**< Number of bits in the bitset. */
    ecs_size_t size;          /**< Allocated capacity in bits (rounded to 64). */
} ecs_bitset_t;

/** Initialize a bitset.
 *
 * @param bs The bitset to initialize.
 */
FLECS_DBG_API
void flecs_bitset_init(
    ecs_bitset_t *bs);

/** Deinitialize a bitset.
 *
 * @param bs The bitset to deinitialize.
 */
FLECS_DBG_API
void flecs_bitset_fini(
    ecs_bitset_t *bs);

/** Add n elements to a bitset.
 *
 * @param bs The bitset to add to.
 * @param count Number of bits to add.
 */
FLECS_DBG_API
void flecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count);

/** Ensure an element exists.
 *
 * @param bs The bitset to ensure capacity for.
 * @param count Minimum number of bits the bitset must hold.
 */
FLECS_DBG_API
void flecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count);

/** Set an element.
 *
 * @param bs The bitset to modify.
 * @param elem Index of the bit to set.
 * @param value The boolean value to set.
 */
FLECS_DBG_API
void flecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value);

/** Get an element.
 *
 * @param bs The bitset to read from.
 * @param elem Index of the bit to get.
 * @return The boolean value of the bit.
 */
FLECS_DBG_API
bool flecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem);

/** Return the number of elements.
 *
 * @param bs The bitset.
 * @return The number of bits in the bitset.
 */
FLECS_DBG_API
int32_t flecs_bitset_count(
    const ecs_bitset_t *bs);

/** Remove from a bitset.
 *
 * @param bs The bitset to remove from.
 * @param elem Index of the bit to remove.
 */
FLECS_DBG_API
void flecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem);

/** Swap values in a bitset.
 *
 * @param bs The bitset.
 * @param elem_a Index of the first bit to swap.
 * @param elem_b Index of the second bit to swap.
 */
FLECS_DBG_API
void flecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b);

#ifdef __cplusplus
}
#endif

#endif
