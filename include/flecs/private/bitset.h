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

void ecs_bitset_init(
    ecs_bitset_t *bs);

void ecs_bitset_deinit(
    ecs_bitset_t *bs);

void ecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count);

void ecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count);

void ecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value);

bool ecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem);

int32_t ecs_bitset_count(
    const ecs_bitset_t *bs);

void ecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem);

void ecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b);

#ifdef __cplusplus
}
#endif

#endif
