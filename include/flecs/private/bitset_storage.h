/**
 * @file bitset.h
 * @brief Bitset storage.
 *
 * Storage implementation for bitset storage.
 */

#ifndef FLECS_BITSET_STORAGE_H
#define FLECS_BITSET_STORAGE_H

#include "flecs/private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

ecs_storage_t* ecs_bitset_storage_init(
    ecs_world_t *world,
    ecs_size_t size,
    ecs_size_t alignment);

void ecs_bitset_storage_fini(
    ecs_storage_t *storage);

#ifdef __cplusplus
}
#endif

#endif
