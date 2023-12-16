/**
 * @file switch_list.h
 * @brief Interleaved linked list for storing mutually exclusive values.
 */

#ifndef FLECS_SWITCH_LIST_H
#define FLECS_SWITCH_LIST_H

#include "flecs/private/api_defines.h"

typedef struct ecs_switch_header_t {
    int32_t element;    /* First element for value */
    int32_t count;      /* Number of elements for value */
} ecs_switch_header_t;

typedef struct ecs_switch_node_t {
    int32_t next;       /* Next node in list */
    int32_t prev;       /* Prev node in list */
} ecs_switch_node_t;

struct ecs_switch_t {    
    ecs_map_t hdrs;     /* map<uint64_t, ecs_switch_header_t> */
    ecs_vec_t nodes;    /* vec<ecs_switch_node_t> */
    ecs_vec_t values;   /* vec<uint64_t> */
};

/** Init new switch. */
FLECS_DBG_API
void flecs_switch_init(
    ecs_switch_t* sw,
    ecs_allocator_t *allocator,
    int32_t elements);

/** Fini switch. */
FLECS_DBG_API
void flecs_switch_fini(
    ecs_switch_t *sw);

/** Remove all values. */
FLECS_DBG_API
void flecs_switch_clear(
    ecs_switch_t *sw);

/** Add element to switch, initialize value to 0 */
FLECS_DBG_API
void flecs_switch_add(
    ecs_switch_t *sw);

/** Set number of elements in switch list */
FLECS_DBG_API
void flecs_switch_set_count(
    ecs_switch_t *sw,
    int32_t count);

/** Get number of elements */
FLECS_DBG_API
int32_t flecs_switch_count(
    ecs_switch_t *sw);

/** Ensure that element exists. */
FLECS_DBG_API
void flecs_switch_ensure(
    ecs_switch_t *sw,
    int32_t count);

/** Add n elements. */
FLECS_DBG_API
void flecs_switch_addn(
    ecs_switch_t *sw,
    int32_t count);    

/** Set value of element. */
FLECS_DBG_API
void flecs_switch_set(
    ecs_switch_t *sw,
    int32_t element,
    uint64_t value);

/** Remove element. */
FLECS_DBG_API
void flecs_switch_remove(
    ecs_switch_t *sw,
    int32_t element);

/** Get value for element. */
FLECS_DBG_API
uint64_t flecs_switch_get(
    const ecs_switch_t *sw,
    int32_t element);

/** Swap element. */
FLECS_DBG_API
void flecs_switch_swap(
    ecs_switch_t *sw,
    int32_t elem_1,
    int32_t elem_2);

/** Get vector with all values. Use together with count(). */
FLECS_DBG_API
ecs_vec_t* flecs_switch_values(
    const ecs_switch_t *sw);    

/** Return number of different values. */
FLECS_DBG_API
int32_t flecs_switch_case_count(
    const ecs_switch_t *sw,
    uint64_t value);

/** Return first element for value. */
FLECS_DBG_API
int32_t flecs_switch_first(
    const ecs_switch_t *sw,
    uint64_t value);

/** Return next element for value. Use with first(). */
FLECS_DBG_API
int32_t flecs_switch_next(
    const ecs_switch_t *sw,
    int32_t elem);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
