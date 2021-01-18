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

#include "api_defines.h"

typedef struct ecs_switch_header_t {
    int32_t element;        /* First element for value */
    int32_t count;          /* Number of elements for value */
} ecs_switch_header_t;

typedef struct ecs_switch_node_t {
    int32_t next;           /* Next node in list */
    int32_t prev;           /* Prev node in list */
} ecs_switch_node_t;

typedef struct ecs_switch_t {
    uint64_t min;           /* Minimum value the switch can store */
    uint64_t max;           /* Maximum value the switch can store */
    ecs_switch_header_t *headers;   /* Array with headers, indexed by value */
    ecs_vector_t *nodes;    /* Vector with nodes, of type ecs_switch_node_t */
    ecs_vector_t *values;   /* Vector with values, of type uint64_t */
} ecs_switch_t;

/** Create new switch. */
ecs_switch_t* ecs_switch_new(
    uint64_t min, 
    uint64_t max,
    int32_t elements);

/** Free switch. */
void ecs_switch_free(
    ecs_switch_t *sw);

/** Add element to switch, initialize value to 0 */
void ecs_switch_add(
    ecs_switch_t *sw);

/** Set number of elements in switch list */
void ecs_switch_set_count(
    ecs_switch_t *sw,
    int32_t count);

/** Ensure that element exists. */
void ecs_switch_ensure(
    ecs_switch_t *sw,
    int32_t count);

/** Add n elements. */
void ecs_switch_addn(
    ecs_switch_t *sw,
    int32_t count);    

/** Set value of element. */
void ecs_switch_set(
    ecs_switch_t *sw,
    int32_t element,
    uint64_t value);

/** Remove element. */
void ecs_switch_remove(
    ecs_switch_t *sw,
    int32_t element);

/** Get value for element. */
uint64_t ecs_switch_get(
    const ecs_switch_t *sw,
    int32_t element);

/** Swap element. */
void ecs_switch_swap(
    ecs_switch_t *sw,
    int32_t elem_1,
    int32_t elem_2);

/** Get vector with all values. Use together with count(). */
ecs_vector_t* ecs_switch_values(
    const ecs_switch_t *sw);    

/** Return number of different values. */
int32_t ecs_switch_case_count(
    const ecs_switch_t *sw,
    uint64_t value);

/** Return first element for value. */
int32_t ecs_switch_first(
    const ecs_switch_t *sw,
    uint64_t value);

/** Return next element for value. Use with first(). */
int32_t ecs_switch_next(
    const ecs_switch_t *sw,
    int32_t elem);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
