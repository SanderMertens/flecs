/**
 * @file switch_list.h
 * @brief Interleaved linked list for storing mutually exclusive values.
 *
 * Datastructure that stores N interleaved linked lists in an array. 
 * This allows for efficient storage of elements with mutually exclusive values.
 * Each linked list has a header element which points to the index in the array
 * that stores the first node of the list. Each list node points to the next
 * array element.
 *
 * The datastructure needs to be created with min and max values, so that it can
 * allocate an array of headers that can be directly indexed by the value. The
 * values are stored in a contiguous array, which allows for the values to be
 * iterated without having to follow the linked list nodes.
 *
 * The datastructure allows for efficient storage and retrieval for values with
 * mutually exclusive values, such as enumeration values. The linked list allows
 * an application to obtain all elements for a given (enumeration) value without
 * having to search.
 *
 * While the list accepts 64 bit values, it only uses the lower 32bits of the
 * value for selecting the correct linked list.
 */

#ifndef FLECS_SWITCH_LIST_H
#define FLECS_SWITCH_LIST_H

#include "flecs/private/api_defines.h"

typedef struct flecs_switch_header_t {
    int32_t element;        /* First element for value */
    int32_t count;          /* Number of elements for value */
} flecs_switch_header_t;

typedef struct flecs_switch_node_t {
    int32_t next;           /* Next node in list */
    int32_t prev;           /* Prev node in list */
} flecs_switch_node_t;

struct ecs_switch_t {
    uint64_t min;           /* Minimum value the switch can store */
    uint64_t max;           /* Maximum value the switch can store */
    flecs_switch_header_t *headers;   /* Array with headers, indexed by value */
    ecs_vector_t *nodes;    /* Vector with nodes, of type flecs_switch_node_t */
    ecs_vector_t *values;   /* Vector with values, of type uint64_t */
};

/** Create new switch. */
FLECS_DBG_API
ecs_switch_t* flecs_switch_new(
    uint64_t min, 
    uint64_t max,
    int32_t elements);

/** Free switch. */
FLECS_DBG_API
void flecs_switch_free(
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
ecs_vector_t* flecs_switch_values(
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
