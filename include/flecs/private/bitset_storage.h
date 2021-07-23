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

void ecs_bitset_storage_init(
    ecs_world_t *world,
    ecs_storage_t *storage);

#ifdef __cplusplus
}
#endif

#endif
