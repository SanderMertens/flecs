/**
 * @file switch_list.h
 * @brief Interleaved linked list for storing mutually exclusive values.
 */

#ifndef FLECS_SWITCH_LIST_H
#define FLECS_SWITCH_LIST_H

#include "flecs/private/api_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_switch_node_t {
    uint32_t next;      /* Next node in list */
    uint32_t prev;      /* Prev node in list */
} ecs_switch_node_t;

typedef struct ecs_switch_page_t {
    ecs_vec_t nodes;    /* vec<ecs_switch_node_t> */
    ecs_vec_t values;   /* vec<uint64_t> */
} ecs_switch_page_t;

typedef struct ecs_switch_t {
    ecs_map_t hdrs;     /* map<uint64_t, uint32_t> */
    ecs_vec_t pages;    /* vec<ecs_switch_page_t> */
} ecs_switch_t;

/** Init new switch. */
FLECS_DBG_API
void flecs_switch_init(
    ecs_switch_t* sw,
    ecs_allocator_t *allocator);

/** Fini switch. */
FLECS_DBG_API
void flecs_switch_fini(
    ecs_switch_t *sw);

/** Set value of element. */
FLECS_DBG_API
bool flecs_switch_set(
    ecs_switch_t *sw,
    uint32_t element,
    uint64_t value);

/** Reset value of element. */
FLECS_DBG_API
bool flecs_switch_reset(
    ecs_switch_t *sw,
    uint32_t element);

/** Get value for element. */
FLECS_DBG_API
uint64_t flecs_switch_get(
    const ecs_switch_t *sw,
    uint32_t element);

/** Get first element for value. */
FLECS_DBG_API
uint32_t flecs_switch_first(
    const ecs_switch_t *sw,
    uint64_t value);

/** Get next element. */
FLECS_DBG_API
uint32_t flecs_switch_next(
    const ecs_switch_t *sw,
    uint32_t previous);

/** Get target iterator. */
FLECS_DBG_API
ecs_map_iter_t flecs_switch_targets(
    const ecs_switch_t *sw);

#ifdef __cplusplus
}
#endif

#endif
