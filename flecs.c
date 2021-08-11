#ifndef FLECS_IMPL
#include "flecs.h"
#endif
#ifndef FLECS_PRIVATE_H
#define FLECS_PRIVATE_H

#ifndef FLECS_TYPES_PRIVATE_H
#define FLECS_TYPES_PRIVATE_H

#ifndef __MACH__
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#ifdef _MSC_VER
//FIXME
#else
#include <sys/param.h>  /* attempt to define endianness */
#endif
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

/**
 * @file entity_index.h
 * @brief Entity index data structure.
 *
 * The entity index stores the table, row for an entity id. It is implemented as
 * a sparse set. This file contains convenience macro's for working with the
 * entity index.
 */

#ifndef FLECS_ENTITY_INDEX_H
#define FLECS_ENTITY_INDEX_H

#ifdef __cplusplus
extern "C" {
#endif

#define ecs_eis_get(world, entity) flecs_sparse_get((world->store).entity_index, ecs_record_t, entity)
#define ecs_eis_get_any(world, entity) flecs_sparse_get_any((world->store).entity_index, ecs_record_t, entity)
#define ecs_eis_set(world, entity, ...) (flecs_sparse_set((world->store).entity_index, ecs_record_t, entity, (__VA_ARGS__)))
#define ecs_eis_ensure(world, entity) flecs_sparse_ensure((world->store).entity_index, ecs_record_t, entity)
#define ecs_eis_delete(world, entity) flecs_sparse_remove((world->store).entity_index, entity)
#define ecs_eis_set_generation(world, entity) flecs_sparse_set_generation((world->store).entity_index, entity)
#define ecs_eis_is_alive(world, entity) flecs_sparse_is_alive((world->store).entity_index, entity)
#define ecs_eis_get_current(world, entity) flecs_sparse_get_alive((world->store).entity_index, entity)
#define ecs_eis_exists(world, entity) flecs_sparse_exists((world->store).entity_index, entity)
#define ecs_eis_recycle(world) flecs_sparse_new_id((world->store).entity_index)
#define ecs_eis_clear_entity(world, entity, is_watched) ecs_eis_set((world->store).entity_index, entity, &(ecs_record_t){NULL, is_watched})
#define ecs_eis_set_size(world, size) flecs_sparse_set_size((world->store).entity_index, size)
#define ecs_eis_count(world) flecs_sparse_count((world->store).entity_index)
#define ecs_eis_clear(world) flecs_sparse_clear((world->store).entity_index)
#define ecs_eis_copy(world) flecs_sparse_copy((world->store).entity_index)
#define ecs_eis_free(world) flecs_sparse_free((world->store).entity_index)
#define ecs_eis_memory(world, allocd, used) flecs_sparse_memory((world->store).entity_index, allocd, used)

#ifdef __cplusplus
}
#endif

#endif
/**
 * @file bitset.h
 * @brief Bitset datastructure.
 *
 * Simple bitset implementation. The bitset allows for storage of arbitrary
 * numbers of bits.
 */

#ifndef FLECS_BITSET_H
#define FLECS_BITSET_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_bitset_t {
    uint64_t *data;
    int32_t count;
    ecs_size_t size;
} ecs_bitset_t;

/** Initialize bitset. */
FLECS_DBG_API
void flecs_bitset_init(
    ecs_bitset_t *bs);

/** Deinialize bitset. */
FLECS_DBG_API
void flecs_bitset_deinit(
    ecs_bitset_t *bs);

/** Add n elements to bitset. */
FLECS_DBG_API
void flecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count);

/** Ensure element exists. */
FLECS_DBG_API
void flecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count);

/** Set element. */
FLECS_DBG_API
void flecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value);

/** Get element. */
FLECS_DBG_API
bool flecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem);

/** Return number of elements. */
FLECS_DBG_API
int32_t flecs_bitset_count(
    const ecs_bitset_t *bs);

/** Remove from bitset. */
FLECS_DBG_API
void flecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem);

/** Swap values in bitset. */
FLECS_DBG_API
void flecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b);

#ifdef __cplusplus
}
#endif

#endif
/**
 * @file sparse.h
 * @brief Sparse set datastructure.
 *
 * This is an implementation of a paged sparse set that stores the payload in
 * the sparse array.
 *
 * A sparse set has a dense and a sparse array. The sparse array is directly
 * indexed by a 64 bit identifier. The sparse element is linked with a dense
 * element, which allows for liveliness checking. The liveliness check itself
 * can be performed by doing (psuedo code):
 *  dense[sparse[sparse_id].dense] == sparse_id
 *
 * To ensure that the sparse array doesn't have to grow to a large size when
 * using large sparse_id's, the sparse set uses paging. This cuts up the array
 * into several pages of 4096 elements. When an element is set, the sparse set
 * ensures that the corresponding page is created. The page associated with an
 * id is determined by shifting a bit 12 bits to the right.
 *
 * The sparse set keeps track of a generation count per id, which is increased
 * each time an id is deleted. The generation is encoded in the returned id.
 *
 * This sparse set implementation stores payload in the sparse array, which is
 * not typical. The reason for this is to guarantee that (in combination with
 * paging) the returned payload pointers are stable. This allows for various
 * optimizations in the parts of the framework that uses the sparse set.
 *
 * The sparse set has been designed so that new ids can be generated in bulk, in
 * an O(1) operation. The way this works is that once a dense-sparse pair is
 * created, it is never unpaired. Instead it is moved to the end of the dense
 * array, and the sparse set stores an additional count to keep track of the
 * last alive id in the sparse set. To generate new ids in bulk, the sparse set
 * only needs to increase this count by the number of requested ids.
 */

#ifndef FLECS_SPARSE_H
#define FLECS_SPARSE_H


#ifdef __cplusplus
extern "C" {
#endif

/** Create new sparse set */
FLECS_DBG_API
ecs_sparse_t* _flecs_sparse_new(
    ecs_size_t elem_size);

#define flecs_sparse_new(type)\
    _flecs_sparse_new(sizeof(type))

/** Set id source. This allows the sparse set to use an external variable for
 * issuing and increasing new ids. */
FLECS_DBG_API
void flecs_sparse_set_id_source(
    ecs_sparse_t *sparse,
    uint64_t *id_source);

/** Free sparse set */
FLECS_DBG_API
void flecs_sparse_free(
    ecs_sparse_t *sparse);

/** Remove all elements from sparse set */
FLECS_DBG_API
void flecs_sparse_clear(
    ecs_sparse_t *sparse);

/** Add element to sparse set, this generates or recycles an id */
FLECS_DBG_API
void* _flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define flecs_sparse_add(sparse, type)\
    ((type*)_flecs_sparse_add(sparse, sizeof(type)))

/** Get last issued id. */
FLECS_DBG_API
uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse);

/** Generate or recycle a new id. */
FLECS_DBG_API
uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse);

/** Generate or recycle new ids in bulk. The returned pointer points directly to
 * the internal dense array vector with sparse ids. Operations on the sparse set
 * can (and likely will) modify the contents of the buffer. */
FLECS_DBG_API
const uint64_t* flecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t count);

/** Remove an element */
FLECS_DBG_API
void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t id);

/** Remove an element, return pointer to the value in the sparse array */
FLECS_DBG_API
void* _flecs_sparse_remove_get(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);    

#define flecs_sparse_remove_get(sparse, type, index)\
    ((type*)_flecs_sparse_remove_get(sparse, sizeof(type), index))

/** Override the generation count for a specific id */
FLECS_DBG_API
void flecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t id);    

/** Check whether an id has ever been issued. */
FLECS_DBG_API
bool flecs_sparse_exists(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Test if id is alive, which requires the generation count tp match. */
FLECS_DBG_API
bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Return identifier with current generation set. */
FLECS_DBG_API
uint64_t flecs_sparse_get_alive(
    const ecs_sparse_t *sparse,
    uint64_t id);

/** Get value from sparse set by dense id. This function is useful in 
 * combination with flecs_sparse_count for iterating all values in the set. */
FLECS_DBG_API
void* _flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define flecs_sparse_get_dense(sparse, type, index)\
    ((type*)_flecs_sparse_get_dense(sparse, sizeof(type), index))

/** Get the number of alive elements in the sparse set. */
FLECS_DBG_API
int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse);

/** Return total number of allocated elements in the dense array */
FLECS_DBG_API
int32_t flecs_sparse_size(
    const ecs_sparse_t *sparse);

/** Get element by (sparse) id. The returned pointer is stable for the duration
 * of the sparse set, as it is stored in the sparse array. */
FLECS_DBG_API
void* _flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get(sparse, type, index)\
    ((type*)_flecs_sparse_get(sparse, sizeof(type), index))

/** Like get_sparse, but don't care whether element is alive or not. */
FLECS_DBG_API
void* _flecs_sparse_get_any(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_get_any(sparse, type, index)\
    ((type*)_flecs_sparse_get_any(sparse, sizeof(type), index))

/** Get or create element by (sparse) id. */
FLECS_DBG_API
void* _flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define flecs_sparse_ensure(sparse, type, index)\
    ((type*)_flecs_sparse_ensure(sparse, sizeof(type), index))

/** Set value. */
FLECS_DBG_API
void* _flecs_sparse_set(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id,
    void *value);

#define flecs_sparse_set(sparse, type, index, value)\
    ((type*)_flecs_sparse_set(sparse, sizeof(type), index, value))

/** Get pointer to ids (alive and not alive). Use with count() or size(). */
FLECS_DBG_API
const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse);

/** Set size of the dense array. */
FLECS_DBG_API
void flecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count);

/** Copy sparse set into a new sparse set. */
FLECS_DBG_API
ecs_sparse_t* flecs_sparse_copy(
    const ecs_sparse_t *src);    

/** Restore sparse set into destination sparse set. */
FLECS_DBG_API
void flecs_sparse_restore(
    ecs_sparse_t *dst,
    const ecs_sparse_t *src);

/** Get memory usage of sparse set. */
FLECS_DBG_API
void flecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used);


/* Publicly exposed APIs 
 * The flecs_ functions aren't exposed directly as this can cause some
 * optimizers to not consider them for link time optimization. */

FLECS_API
ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size);

#define ecs_sparse_new(type)\
    _ecs_sparse_new(sizeof(type))

FLECS_API
void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size);

#define ecs_sparse_add(sparse, type)\
    ((type*)_ecs_sparse_add(sparse, sizeof(type)))

FLECS_API
uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse);

FLECS_API
int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse);

FLECS_API
void* _ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index);

#define ecs_sparse_get_dense(sparse, type, index)\
    ((type*)_ecs_sparse_get_dense(sparse, sizeof(type), index))

FLECS_API
void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id);

#define ecs_sparse_get(sparse, type, index)\
    ((type*)_ecs_sparse_get(sparse, sizeof(type), index))

#ifdef __cplusplus
}
#endif

#endif
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
/**
 * @file hashmap.h
 * @brief Hashmap datastructure.
 *
 * Datastructure that computes a hash to store & retrieve values. Similar to
 * ecs_map_t, but allows for arbitrary keytypes.
 */

#ifndef FLECS_HASHMAP_H
#define FLECS_HASHMAP_H


#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecs_hash_value_action_t hash;
    ecs_compare_action_t compare;
    ecs_size_t key_size;
    ecs_size_t value_size;
    ecs_map_t *impl;
} ecs_hashmap_t;

typedef struct {
    ecs_map_iter_t it;
    struct ecs_hm_bucket_t *bucket;
    int32_t index;
} flecs_hashmap_iter_t;

typedef struct {
    void *key;
    void *value;
    uint64_t hash;
} flecs_hashmap_result_t;

FLECS_DBG_API
ecs_hashmap_t _flecs_hashmap_new(
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare);

#define flecs_hashmap_new(K, V, compare, hash)\
    _flecs_hashmap_new(ECS_SIZEOF(K), ECS_SIZEOF(V), compare, hash)

FLECS_DBG_API
void flecs_hashmap_free(
    ecs_hashmap_t map);

FLECS_DBG_API
void* _flecs_hashmap_get(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_get(map, key, V)\
    (V*)_flecs_hashmap_get(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
flecs_hashmap_result_t _flecs_hashmap_ensure(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size);

#define flecs_hashmap_ensure(map, key, V)\
    _flecs_hashmap_ensure(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _flecs_hashmap_set(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value);

#define flecs_hashmap_set(map, key, value)\
    _flecs_hashmap_set(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(*value), value)

FLECS_DBG_API
void _flecs_hashmap_remove(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size);

#define flecs_hashmap_remove(map, key, V)\
    _flecs_hashmap_remove(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V))

FLECS_DBG_API
void _flecs_hashmap_remove_w_hash(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash);

#define flecs_hashmap_remove_w_hash(map, key, V, hash)\
    _flecs_hashmap_remove_w_hash(map, ECS_SIZEOF(*key), key, ECS_SIZEOF(V), hash)

FLECS_DBG_API
flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t map);

FLECS_DBG_API
void* _flecs_hashmap_next(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size);

#define flecs_hashmap_next(map, V)\
    (V*)_flecs_hashmap_next(map, 0, NULL, ECS_SIZEOF(V))

#define flecs_hashmap_next_w_key(map, K, key, V)\
    (V*)_flecs_hashmap_next(map, ECS_SIZEOF(K), key, ECS_SIZEOF(V))

#ifdef __cplusplus
}
#endif

#endif

#define ECS_MAX_JOBS_PER_WORKER (16)

/** These values are used to verify validity of the pointers passed into the API
 * and to allow for passing a thread as a world to some API calls (this allows
 * for transparently passing thread context to API functions) */
#define ECS_WORLD_MAGIC (0x65637377)
#define ECS_STAGE_MAGIC (0x65637374)

/* Maximum number of entities that can be added in a single operation. 
 * Increasing this value will increase consumption of stack space. */
#define ECS_MAX_ADD_REMOVE (32)

/** Type used for internal string hashmap */
typedef struct ecs_string_t {
    char *value;
    ecs_size_t length;
    uint64_t hash;
} ecs_string_t;

/** Component-specific data */
typedef struct ecs_type_info_t {
    EcsComponentLifecycle lifecycle; /* Component lifecycle callbacks */
    ecs_entity_t component;
    ecs_size_t size;
    ecs_size_t alignment;
    bool lifecycle_set;
} ecs_type_info_t;

/* Table event type for notifying tables of world events */
typedef enum ecs_table_eventkind_t {
    EcsTableQueryMatch,
    EcsTableQueryUnmatch,
    EcsTableTriggerMatch,
    EcsTableComponentInfo
} ecs_table_eventkind_t;

typedef struct ecs_table_event_t {
    ecs_table_eventkind_t kind;

    /* Query event */
    ecs_query_t *query;
    int32_t matched_table_index;

    /* Component info event */
    ecs_entity_t component;

    /* Trigger match */
    ecs_entity_t event;

    /* If the nubmer of fields gets out of hand, this can be turned into a union
     * but since events are very temporary objects, this works for now and makes
     * initializing an event a bit simpler. */
} ecs_table_event_t;    

/** A component column. */
struct ecs_column_t {
    ecs_vector_t *data;        /**< Column data */
    int16_t size;              /**< Column element size */
    int16_t alignment;         /**< Column element alignment */
};

/** A switch column. */
typedef struct ecs_sw_column_t {
    ecs_switch_t *data;   /**< Column data */
    ecs_type_t type;      /**< Switch type */
} ecs_sw_column_t;

/** A bitset column. */
typedef struct ecs_bs_column_t {
    ecs_bitset_t data;   /**< Column data */
} ecs_bs_column_t;

/** Stage-specific component data */
struct ecs_data_t {
    ecs_vector_t *entities;      /**< Entity identifiers */
    ecs_vector_t *record_ptrs;   /**< Ptrs to records in main entity index */
    ecs_column_t *columns;       /**< Component columns */
    ecs_sw_column_t *sw_columns; /**< Switch columns */
    ecs_bs_column_t *bs_columns; /**< Bitset columns */
};

/** Small footprint data structure for storing data associated with a table. */
typedef struct ecs_table_leaf_t {
    ecs_table_t *table;
    ecs_type_t type;
    ecs_data_t *data;
} ecs_table_leaf_t;

/** Flags for quickly checking for special properties of a table. */
#define EcsTableHasBuiltins         1u    /**< Does table have builtin components */
#define EcsTableIsPrefab            2u    /**< Does the table store prefabs */
#define EcsTableHasIsA             4u    /**< Does the table type has IsA */
#define EcsTableHasModule           8u    /**< Does the table have module data */
#define EcsTableHasXor              32u   /**< Does the table type has XOR */
#define EcsTableIsDisabled          64u   /**< Does the table type has EcsDisabled */
#define EcsTableHasCtors            128u
#define EcsTableHasDtors            256u
#define EcsTableHasCopy             512u
#define EcsTableHasMove             1024u
#define EcsTableHasOnAdd            2048u
#define EcsTableHasOnRemove         4096u
#define EcsTableHasOnSet            8192u
#define EcsTableHasUnSet            16384u
#define EcsTableHasMonitors         32768u
#define EcsTableHasSwitch           65536u
#define EcsTableHasDisabled         131072u

/* Composite constants */
#define EcsTableHasLifecycle        (EcsTableHasCtors | EcsTableHasDtors)
#define EcsTableIsComplex           (EcsTableHasLifecycle | EcsTableHasSwitch | EcsTableHasDisabled)
#define EcsTableHasAddActions       (EcsTableHasIsA | EcsTableHasSwitch | EcsTableHasCtors | EcsTableHasOnAdd | EcsTableHasOnSet | EcsTableHasMonitors)
#define EcsTableHasRemoveActions    (EcsTableHasIsA | EcsTableHasDtors | EcsTableHasOnRemove | EcsTableHasUnSet | EcsTableHasMonitors)

/** Edge used for traversing the table graph. */
typedef struct ecs_edge_t {
    ecs_table_t *add;               /**< Edges traversed when adding */
    ecs_table_t *remove;            /**< Edges traversed when removing */
} ecs_edge_t;

/** Quey matched with table with backref to query table administration.
 * This type is used to store a matched query together with the array index of
 * where the table is stored in the query administration. This type is used when
 * an action that originates on a table needs to invoke a query (system) and a
 * fast lookup is required for the query administration, as is the case with
 * OnSet and Monitor systems. */
typedef struct ecs_matched_query_t {
    ecs_query_t *query;             /**< The query matched with the table */
    int32_t matched_table_index;    /**< Table index in the query type */
} ecs_matched_query_t;

/** A table is the Flecs equivalent of an archetype. Tables store all entities
 * with a specific set of components. Tables are automatically created when an
 * entity has a set of components not previously observed before. When a new
 * table is created, it is automatically matched with existing queries */
struct ecs_table_t {
    uint64_t id;                     /**< Table id in sparse set */
    ecs_type_t type;                 /**< Identifies table type in type_index */
    ecs_flags32_t flags;             /**< Flags for testing table properties */
    int32_t column_count;            /**< Number of data columns in table */

    ecs_data_t *data;                /**< Component storage */
    ecs_type_info_t **c_info;        /**< Cached pointers to component info */

    ecs_edge_t *lo_edges;            /**< Edges to other tables */
    ecs_map_t *hi_edges;

    ecs_vector_t *queries;           /**< Queries matched with table */
    ecs_vector_t *monitors;          /**< Monitor systems matched with table */
    ecs_vector_t **on_set;           /**< OnSet systems, broken up by column */
    ecs_vector_t *on_set_all;        /**< All OnSet systems */
    ecs_vector_t *on_set_override;   /**< All OnSet systems with overrides */
    ecs_vector_t *un_set_all;        /**< All UnSet systems */

    int32_t *dirty_state;            /**< Keep track of changes in columns */
    int32_t alloc_count;             /**< Increases when columns are reallocd */

    int32_t sw_column_count;
    int32_t sw_column_offset;
    int32_t bs_column_count;
    int32_t bs_column_offset;

    int32_t lock;
};

/* Sparse query column */
typedef struct flecs_sparse_column_t {
    ecs_sw_column_t *sw_column;
    ecs_entity_t sw_case; 
    int32_t signature_column_index;
} flecs_sparse_column_t;

/* Bitset query column */
typedef struct flecs_bitset_column_t {
    ecs_bs_column_t *bs_column;
    int32_t column_index;
} flecs_bitset_column_t;

/** Type containing data for a table matched with a query. */
typedef struct ecs_matched_table_t {
    int32_t *columns;         /**< Mapping from query terms to table columns */
    ecs_table_t *table;       /**< The current table. */
    ecs_data_t *data;         /**< Table component data */
    ecs_id_t *ids;            /**< Resolved (component) ids for current table */
    ecs_entity_t *subjects;   /**< Subjects (sources) of ids */
    ecs_type_t *types;        /**< Types for ids for current table */
    ecs_size_t *sizes;        /**< Sizes for ids for current table */
    ecs_ref_t *references;    /**< Cached components for non-this terms */

    ecs_vector_t *sparse_columns;  /**< Column ids of sparse columns */
    ecs_vector_t *bitset_columns;  /**< Column ids with disabled flags */
    int32_t *monitor;              /**< Used to monitor table for changes */
    int32_t rank;                  /**< Rank used to sort tables */
} ecs_matched_table_t;

/** Type used to track location of table in queries' table lists.
 * When a table becomes empty or non-empty a signal is sent to a query, which
 * moves the table to or from an empty list. While this ensures that when 
 * iterating no time is spent on iterating over empty tables, doing a linear
 * search for the table in either list can take a significant amount of time if
 * a query is matched with many tables.
 *
 * To avoid a linear search, the query has a map with table indices that can
 * return the location of the table in either list in constant time.
 *
 * If a table is matched multiple times by a query, such as can happen when a
 * query matches pairs, a table can occupy multiple indices.
 */
typedef struct ecs_table_indices_t {
    int32_t *indices; /* If indices are negative, table is in empty list */
    int32_t count;
} ecs_table_indices_t;

/** Type storing an entity range within a table.
 * This type is used for iterating in orer across archetypes. A sorting function
 * constructs a list of the ranges across archetypes that are in order so that
 * when the query iterates over the archetypes, it only needs to iterate the
 * list of ranges. */
typedef struct ecs_table_slice_t {
    ecs_matched_table_t *table;     /**< Reference to the matched table */
    int32_t start_row;              /**< Start of range  */
    int32_t count;                  /**< Number of entities in range */
} ecs_table_slice_t;

#define EcsQueryNeedsTables (1)      /* Query needs matching with tables */ 
#define EcsQueryMonitor (2)          /* Query needs to be registered as a monitor */
#define EcsQueryOnSet (4)            /* Query needs to be registered as on_set system */
#define EcsQueryUnSet (8)            /* Query needs to be registered as un_set system */
#define EcsQueryMatchDisabled (16)   /* Does query match disabled */
#define EcsQueryMatchPrefab (32)     /* Does query match prefabs */
#define EcsQueryHasRefs (64)         /* Does query have references */
#define EcsQueryHasTraits (128)      /* Does query have pairs */
#define EcsQueryIsSubquery (256)     /* Is query a subquery */
#define EcsQueryIsOrphaned (512)     /* Is subquery orphaned */
#define EcsQueryHasOutColumns (1024) /* Does query have out columns */
#define EcsQueryHasOptional (2048)   /* Does query have optional columns */

#define EcsQueryNoActivation (EcsQueryMonitor | EcsQueryOnSet | EcsQueryUnSet)

/* Query event type for notifying queries of world events */
typedef enum ecs_query_eventkind_t {
    EcsQueryTableMatch,
    EcsQueryTableEmpty,
    EcsQueryTableNonEmpty,
    EcsQueryTableRematch,
    EcsQueryTableUnmatch,
    EcsQueryOrphan
} ecs_query_eventkind_t;

typedef struct ecs_query_event_t {
    ecs_query_eventkind_t kind;
    ecs_table_t *table;
    ecs_query_t *parent_query;
} ecs_query_event_t;

/** Query that is automatically matched against active tables */
struct ecs_query_t {
    /* Signature of query */
    ecs_filter_t filter;

    /* Reference to world */
    ecs_world_t *world;

    /* Tables matched with query */
    ecs_vector_t *tables;
    ecs_vector_t *empty_tables;
    ecs_map_t *table_indices;

    /* Handle to system (optional) */
    ecs_entity_t system;   

    /* Used for sorting */
    ecs_entity_t order_by_component;
    ecs_order_by_action_t order_by;
    ecs_vector_t *table_slices;     

    /* Used for table sorting */
    ecs_entity_t group_by_id;
    ecs_group_by_action_t group_by;
    void *group_by_ctx;
    ecs_ctx_free_t group_by_ctx_free;

    /* Subqueries */
    ecs_query_t *parent;
    ecs_vector_t *subqueries;

    /* The query kind determines how it is registered with tables */
    ecs_flags32_t flags;

    uint64_t id;                /* Id of query in query storage */
    int32_t cascade_by;         /* Identify CASCADE column */
    int32_t match_count;        /* How often have tables been (un)matched */
    int32_t prev_match_count;   /* Used to track if sorting is needed */

    bool needs_reorder;         /* Whether next iteration should reorder */
    bool constraints_satisfied; /* Are all term constraints satisfied */
};

/** Event mask */
#define EcsEventAdd    (1)
#define EcsEventRemove (2)

/** Triggers for a specific id */
typedef struct ecs_id_trigger_t {
    ecs_map_t *on_add_triggers;
    ecs_map_t *on_remove_triggers;
    ecs_map_t *on_set_triggers;
    ecs_map_t *un_set_triggers;
} ecs_id_trigger_t;

/** Keep track of how many [in] columns are active for [out] columns of OnDemand
 * systems. */
typedef struct ecs_on_demand_out_t {
    ecs_entity_t system;    /* Handle to system */
    int32_t count;         /* Total number of times [out] columns are used */
} ecs_on_demand_out_t;

/** Keep track of which OnDemand systems are matched with which [in] columns */
typedef struct ecs_on_demand_in_t {
    int32_t count;         /* Number of active systems with [in] column */
    ecs_vector_t *systems;  /* Systems that have this column as [out] column */
} ecs_on_demand_in_t;

/** Types for deferred operations */
typedef enum ecs_op_kind_t {
    EcsOpNew,
    EcsOpClone,
    EcsOpBulkNew,
    EcsOpAdd,
    EcsOpRemove,   
    EcsOpSet,
    EcsOpMut,
    EcsOpModified,
    EcsOpDelete,
    EcsOpClear,
    EcsOpEnable,
    EcsOpDisable
} ecs_op_kind_t;

typedef struct ecs_op_1_t {
    ecs_entity_t entity;        /* Entity id */
    void *value;                /* Value (used for set / get_mut) */
    ecs_size_t size;            /* Size of value */
    bool clone_value;           /* Clone entity with value (used for clone) */ 
} ecs_op_1_t;

typedef struct ecs_op_n_t {
    ecs_entity_t *entities;  
    void **bulk_data;
    int32_t count;
} ecs_op_n_t;

typedef struct ecs_op_t {
    ecs_op_kind_t kind;         /* Operation kind */    
    ecs_entity_t component;     /* Single component (components.count = 1) */
    ecs_ids_t components;  /* Multiple components */
    union {
        ecs_op_1_t _1;
        ecs_op_n_t _n;
    } is;
} ecs_op_t;

/** A stage is a data structure in which delta's are stored until it is safe to
 * merge those delta's with the main world stage. A stage allows flecs systems
 * to arbitrarily add/remove/set components and create/delete entities while
 * iterating. Additionally, worker threads have their own stage that lets them
 * mutate the state of entities without requiring locks. */
struct ecs_stage_t {
    int32_t magic;              /* Magic number to verify thread pointer */
    int32_t id;                 /* Unique id that identifies the stage */

    /* Are operations deferred? */
    int32_t defer;
    ecs_vector_t *defer_queue;

    ecs_world_t *thread_ctx;    /* Points to stage when a thread stage */
    ecs_world_t *world;         /* Reference to world */
    ecs_os_thread_t thread;     /* Thread handle (0 if no threading is used) */

    /* One-shot actions to be executed after the merge */
    ecs_vector_t *post_frame_actions;

    /* Namespacing */
    ecs_table_t *scope_table;      /* Table for current scope */
    ecs_entity_t scope;            /* Entity of current scope */
    ecs_entity_t with;             /* Id to add by default to new entities */

    /* Properties */
    bool auto_merge;               /* Should this stage automatically merge? */
    bool asynchronous;             /* Is stage asynchronous? (write only) */
};

/* Component monitor */
typedef struct ecs_monitor_t {
    ecs_vector_t *queries;  /* vector<ecs_query_t*> */
    bool is_dirty;          /* Should queries be rematched? */
} ecs_monitor_t;

/* Component monitors */
typedef struct ecs_monitor_set_t {
    ecs_map_t *monitors; /* map<id, ecs_monitor_t> */
    bool is_dirty;       /* Should monitors be evaluated? */
} ecs_monitor_set_t;

/* Relation monitors. TODO: implement generic monitor mechanism */
typedef struct ecs_relation_monitor_t {
    ecs_map_t *monitor_sets; /* map<relation_id, ecs_monitor_set_t> */
    bool is_dirty;          /* Should monitor sets be evaluated? */
} ecs_relation_monitor_t;

/* Payload for table index which returns all tables for a given component, with
 * the column of the component in the table. */
typedef struct ecs_table_record_t {
    ecs_table_t *table;
    int32_t column;
    int32_t count;
} ecs_table_record_t;

/* Payload for id index which contains all datastructures for an id. */
struct ecs_id_record_t {
    /* All tables that contain the id */
    ecs_map_t *table_index;         /* map<table_id, ecs_table_record_t> */

    ecs_entity_t on_delete;         /* Cleanup action for removing id */
    ecs_entity_t on_delete_object;  /* Cleanup action for removing object */
};

typedef struct ecs_store_t {
    /* Entity lookup */
    ecs_sparse_t *entity_index; /* sparse<entity, ecs_record_t> */

    /* Table lookup by id */
    ecs_sparse_t *tables; /* sparse<table_id, ecs_table_t> */

    /* Table lookup by hash */
    ecs_hashmap_t table_map; /* hashmap<ecs_ids_t, ecs_table_t*> */

    /* Root table */
    ecs_table_t root;
} ecs_store_t;

/** Supporting type to store looked up or derived entity data */
typedef struct ecs_entity_info_t {
    ecs_record_t *record;       /* Main stage record in entity index */
    ecs_table_t *table;         /* Table. Not set if entity is empty */
    ecs_data_t *data;           /* Stage-specific table columns */
    int32_t row;                /* Actual row (stripped from is_watched bit) */
    bool is_watched;            /* Is entity being watched */
} ecs_entity_info_t;

/** Supporting type to store looked up component data in specific table */
typedef struct ecs_column_info_t {
    ecs_entity_t id;
    const ecs_type_info_t *ci;
    int32_t column;
} ecs_column_info_t;

/* fini actions */
typedef struct ecs_action_elem_t {
    ecs_fini_action_t action;
    void *ctx;
} ecs_action_elem_t;

/* Alias */
typedef struct ecs_alias_t {
    char *name;
    ecs_entity_t entity;
} ecs_alias_t;

/** The world stores and manages all ECS data. An application can have more than
 * one world, but data is not shared between worlds. */
struct ecs_world_t {
    int32_t magic;               /* Magic number to verify world pointer */

    /* --  Type metadata -- */

    ecs_map_t *id_index;         /* map<id, ecs_id_record_t> */
    ecs_map_t *id_triggers;      /* map<id, ecs_id_trigger_t> */
    ecs_sparse_t *type_info;     /* sparse<type_id, type_info_t> */

    /* Is entity range checking enabled? */
    bool range_check_enabled;


    /* --  Data storage -- */

    ecs_store_t store;


    /* --  Storages for API objects -- */

    ecs_sparse_t *queries; /* sparse<query_id, ecs_query_t> */
    ecs_sparse_t *triggers; /* sparse<query_id, ecs_trigger_t> */
    ecs_sparse_t *observers; /* sparse<query_id, ecs_observer_t> */
    

    /* Keep track of components that were added/removed to/from monitored
     * entities. Monitored entities are entities that a query has matched with
     * specifically, as is the case with PARENT / CASCADE columns, FromEntity
     * columns and columns matched from prefabs. 
     * When these entities change type, queries may have to be rematched.
     * Queries register themselves as component monitors for specific components
     * and when these components change they are rematched. The component 
     * monitors are evaluated during a merge. */
    ecs_relation_monitor_t monitors;

    /* -- Systems -- */
    
    ecs_entity_t pipeline;             /* Current pipeline */
    ecs_map_t *on_activate_components; /* Trigger on activate of [in] column */
    ecs_map_t *on_enable_components;   /* Trigger on enable of [in] column */
    ecs_vector_t *fini_tasks;          /* Tasks to execute on ecs_fini */


    /* -- Lookup Indices -- */

    ecs_map_t *type_handles;          /* Handles to named types */


    /* -- Aliasses -- */

    ecs_hashmap_t aliases;
    ecs_hashmap_t symbols;


    /* -- Staging -- */

    ecs_stage_t stage;               /* Main storage */
    ecs_vector_t *worker_stages;     /* Stages for threads */


    /* -- Hierarchy administration -- */

    const char *name_prefix;        /* Remove prefix from C names in modules */


    /* -- Multithreading -- */

    ecs_os_cond_t worker_cond;       /* Signal that worker threads can start */
    ecs_os_cond_t sync_cond;         /* Signal that worker thread job is done */
    ecs_os_mutex_t sync_mutex;       /* Mutex for job_cond */
    int32_t workers_running;         /* Number of threads running */
    int32_t workers_waiting;         /* Number of workers waiting on sync */


    /* -- Time management -- */

    ecs_time_t world_start_time;  /* Timestamp of simulation start */
    ecs_time_t frame_start_time;  /* Timestamp of frame start */
    FLECS_FLOAT fps_sleep;        /* Sleep time to prevent fps overshoot */


    /* -- Metrics -- */

    ecs_world_info_t stats;


    /* -- Settings from command line arguments -- */

    int arg_fps;
    int arg_threads;


    /* -- World lock -- */

    ecs_os_mutex_t mutex;         /* Locks the world if locking enabled */
    ecs_os_mutex_t thr_sync;      /* Used to signal threads at end of frame */
    ecs_os_cond_t thr_cond;       /* Used to signal threads at end of frame */


    /* -- Defered operation count -- */
    
    int32_t new_count;
    int32_t bulk_new_count;
    int32_t delete_count;
    int32_t clear_count;
    int32_t add_count;
    int32_t remove_count;
    int32_t set_count;
    int32_t discard_count;


    /* -- World state -- */

    bool quit_workers;            /* Signals worker threads to quit */
    bool is_readonly;             /* Is world being progressed */
    bool is_fini;                 /* Is the world being cleaned up? */
    bool measure_frame_time;      /* Time spent on each frame */
    bool measure_system_time;     /* Time spent by each system */
    bool should_quit;             /* Did a system signal that app should quit */
    bool locking_enabled;         /* Lock world when in progress */ 

    void *context;               /* Application context */
    ecs_vector_t *fini_actions;  /* Callbacks to execute when world exits */
};

#endif

////////////////////////////////////////////////////////////////////////////////
//// Core bootstrap functions
////////////////////////////////////////////////////////////////////////////////

#define ECS_TYPE_DECL(component)\
static const ecs_entity_t __##component = ecs_id(component);\
ECS_VECTOR_DECL(FLECS__T##component, ecs_entity_t, 1)

#define ECS_TYPE_IMPL(component)\
ECS_VECTOR_IMPL(FLECS__T##component, ecs_entity_t, &__##component, 1)

/* Bootstrap world */
void flecs_bootstrap(
    ecs_world_t *world);

ecs_type_t flecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity);

#define flecs_bootstrap_component(world, id)\
    ecs_component_init(world, &(ecs_component_desc_t){\
        .entity = {\
            .entity = ecs_id(id),\
            .name = #id,\
            .symbol = #id\
        },\
        .size = sizeof(id),\
        .alignment = ECS_ALIGNOF(id)\
    });

#define flecs_bootstrap_tag(world, name)\
    ecs_set_name(world, name, (char*)&#name[ecs_os_strlen("Ecs")]);\
    ecs_set_symbol(world, name, #name);\
    ecs_add_pair(world, name, EcsChildOf, ecs_get_scope(world))


/* Bootstrap functions for other parts in the code */
void flecs_bootstrap_hierarchy(ecs_world_t *world);

////////////////////////////////////////////////////////////////////////////////
//// Entity API
////////////////////////////////////////////////////////////////////////////////

/* Mark an entity as being watched. This is used to trigger automatic rematching
 * when entities used in system expressions change their components. */
void flecs_set_watch(
    ecs_world_t *world,
    ecs_entity_t entity);

/* Obtain entity info */
bool flecs_get_info(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_info_t *info);

void flecs_run_monitors(
    ecs_world_t *world, 
    ecs_table_t *dst_table,
    ecs_vector_t *v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors);

void flecs_register_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name);

void flecs_unregister_name(
    ecs_world_t *world,
    ecs_entity_t entity);


////////////////////////////////////////////////////////////////////////////////
//// World API
////////////////////////////////////////////////////////////////////////////////

/* Get current stage */
ecs_stage_t* flecs_stage_from_world(
    ecs_world_t **world_ptr);

/* Get current thread-specific stage from readonly world */
const ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world);

/* Get component callbacks */
const ecs_type_info_t *flecs_get_c_info(
    const ecs_world_t *world,
    ecs_entity_t component);

/* Get or create component callbacks */
ecs_type_info_t * flecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component);

void flecs_eval_component_monitors(
    ecs_world_t *world);

void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id);

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id,
    ecs_query_t *query);

void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event);

void flecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event);

void flecs_register_table(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_unregister_table(
    ecs_world_t *world,
    ecs_table_t *table);

ecs_id_record_t* flecs_ensure_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

void flecs_clear_id_record(
    const ecs_world_t *world,
    ecs_id_t id);

void flecs_triggers_notify(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

ecs_map_t* flecs_triggers_get(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event);

void flecs_trigger_fini(
    ecs_world_t *world,
    ecs_trigger_t *trigger);

void flecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer);

void flecs_use_intern(
    ecs_entity_t entity,
    const char *name,
    ecs_vector_t **alias_vector);


////////////////////////////////////////////////////////////////////////////////
//// Stage API
////////////////////////////////////////////////////////////////////////////////

/* Initialize stage data structures */
void flecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Deinitialize stage */
void flecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage);

/* Post-frame merge actions */
void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage);  

/* Delete table from stage */
void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table);


////////////////////////////////////////////////////////////////////////////////
//// Defer API
////////////////////////////////////////////////////////////////////////////////

bool flecs_defer_none(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component);

bool flecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components);

bool flecs_defer_clone(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value);

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    const ecs_ids_t *components,
    void **component_data,
    const ecs_entity_t **ids_out);

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity);

bool flecs_defer_enable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool enable);    

bool flecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components);

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components);

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_size_t size,
    const void *value,
    void **value_out,
    bool *is_added);

bool flecs_defer_flush(
    ecs_world_t *world,
    ecs_stage_t *stage);

bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage);

////////////////////////////////////////////////////////////////////////////////
//// Type API
////////////////////////////////////////////////////////////////////////////////

/* Test if type_id_1 contains type_id_2 */
ecs_entity_t flecs_type_contains(
    const ecs_world_t *world,
    ecs_type_t type_id_1,
    ecs_type_t type_id_2,
    bool match_all,
    bool match_prefab);

void flecs_run_add_actions(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *added,
    bool get_all,
    bool run_on_set);   

void flecs_run_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *removed);

void flecs_run_set_systems(
    ecs_world_t *world,
    ecs_id_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool set_all);


////////////////////////////////////////////////////////////////////////////////
//// Table API
////////////////////////////////////////////////////////////////////////////////

/** Find or create table for a set of components */
ecs_table_t* flecs_table_find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *type);   

/* Get table data */
ecs_data_t *flecs_table_get_data(
    const ecs_table_t *table);

/* Get or create data */
ecs_data_t *flecs_table_get_or_create_data(
    ecs_table_t *table);

/* Initialize columns for data */
ecs_data_t* flecs_init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result); 

/* Clear all entities from a table. */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table);

/* Reset a table to its initial state */
void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear all entities from the table. Do not invoke OnRemove systems */
void flecs_table_clear_entities_silent(
    ecs_world_t *world,
    ecs_table_t *table);

/* Clear table data. Don't call OnRemove handlers. */
void flecs_table_clear_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);    

/* Return number of entities in data */
int32_t flecs_table_data_count(
    const ecs_data_t *data);

/* Add a new entry to the table for the specified entity */
int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t entity,
    ecs_record_t *record,
    bool construct);

/* Delete an entity from the table. */
void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t index,
    bool destruct);

/* Move a row from one table to another */
void flecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index,
    bool construct);

/* Grow table with specified number of records. Populate table with entities,
 * starting from specified entity id. */
int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count,
    const ecs_entity_t *ids);

/* Set table to a fixed size. Useful for preallocating memory in advance. */
void flecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t count);

/* Match table with filter */
bool flecs_table_match_filter(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_filter_t *filter);

bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_type_t type,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_type_t *types,
    ecs_entity_t *subjects,
    ecs_size_t *sizes,
    void **ptrs);

/* Get dirty state for table columns */
int32_t* flecs_table_get_dirty_state(
    ecs_table_t *table);

/* Get monitor for monitoring table changes */
int32_t* flecs_table_get_monitor(
    ecs_table_t *table);

/* Initialize root table */
void flecs_init_root_table(
    ecs_world_t *world);

/* Unset components in table */
void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table);

/* Free table */
void flecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table); 

/* Free table */
void flecs_table_free_type(
    ecs_table_t *table);     
    
/* Replace data */
void flecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data);

/* Merge data of one table into another table */
ecs_data_t* flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    ecs_data_t *new_data,
    ecs_data_t *old_data);

void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2);

ecs_table_t *flecs_table_traverse_add(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_ids_t *to_add,
    ecs_ids_t *added);

ecs_table_t *flecs_table_traverse_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_ids_t *to_remove,
    ecs_ids_t *removed);

void flecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component);

const EcsComponent* flecs_component_from_id(
    const ecs_world_t *world,
    ecs_entity_t e);

int32_t flecs_table_switch_from_case(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_entity_t add);    

void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t *event);

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table);

void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table);

ecs_column_t *ecs_table_column_for_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id);

////////////////////////////////////////////////////////////////////////////////
//// Query API
////////////////////////////////////////////////////////////////////////////////

void flecs_query_set_iter(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count);

void flecs_run_monitor(
    ecs_world_t *world,
    ecs_matched_query_t *monitor,
    ecs_ids_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities);

bool flecs_query_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_query_t *query,
    ecs_match_failure_t *failure_info);

void flecs_query_notify(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event);

void ecs_iter_init(
    ecs_iter_t *it);

void ecs_iter_fini(
    ecs_iter_t *it);

////////////////////////////////////////////////////////////////////////////////
//// Time API
////////////////////////////////////////////////////////////////////////////////

void flecs_os_time_setup(void);

uint64_t flecs_os_time_now(void);

void flecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec);

/* Increase or reset timer resolution (Windows only) */
FLECS_API
void flecs_increase_timer_resolution(
    bool enable);

////////////////////////////////////////////////////////////////////////////////
//// Utilities
////////////////////////////////////////////////////////////////////////////////

uint64_t flecs_hash(
    const void *data,
    ecs_size_t length);

/* Convert 64 bit signed integer to 16 bit */
int8_t flflecs_to_i8(
    int64_t v);

/* Convert 64 bit signed integer to 16 bit */
int16_t flecs_to_i16(
    int64_t v);

/* Convert 64 bit unsigned integer to 32 bit */
uint32_t flecs_to_u32(
    uint64_t v);        

/* Convert signed integer to size_t */
size_t flecs_to_size_t(
    int64_t size);

/* Convert size_t to ecs_size_t */
ecs_size_t flecs_from_size_t(
    size_t size);    

/* Get next power of 2 */
int32_t flecs_next_pow_of_2(
    int32_t n);

/* Convert 64bit value to ecs_record_t type. ecs_record_t is stored as 64bit int in the
 * entity index */
ecs_record_t flecs_to_row(
    uint64_t value);

/* Get 64bit integer from ecs_record_t */
uint64_t flecs_from_row(
    ecs_record_t record);

/* Get actual row from record row */
int32_t flecs_record_to_row(
    int32_t row, 
    bool *is_watched_out);

/* Convert actual row to record row */
int32_t flecs_row_to_record(
    int32_t row, 
    bool is_watched);

/* Convert type to entity array */
ecs_ids_t flecs_type_to_ids(
    ecs_type_t type); 

/* Convert a symbol name to an entity name by removing the prefix */
const char* flecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name);

/* Compare function for entity ids */
int flecs_entity_compare(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2); 

/* Compare function for entity ids which can be used with qsort */
int flecs_entity_compare_qsort(
    const void *e1,
    const void *e2);

uint64_t flecs_string_hash(
    const void *ptr);

ecs_hashmap_t flecs_table_hashmap_new(void);
ecs_hashmap_t flecs_string_hashmap_new(void);

#define assert_func(cond) _assert_func(cond, #cond, __FILE__, __LINE__, __func__)
void _assert_func(
    bool cond,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *func);

#endif

static
char *ecs_vasprintf(
    const char *fmt,
    va_list args)
{
    ecs_size_t size = 0;
    char *result  = NULL;
    va_list tmpa;

    va_copy(tmpa, args);

    size = vsnprintf(result, 0, fmt, tmpa);

    va_end(tmpa);

    if ((int32_t)size < 0) { 
        return NULL; 
    }

    result = (char *) ecs_os_malloc(size + 1);

    if (!result) { 
        return NULL; 
    }

    ecs_os_vsprintf(result, fmt, args);

    return result;
}

static
char* ecs_colorize(
    char *msg)
{
    ecs_strbuf_t buff = ECS_STRBUF_INIT;
    char *ptr, ch, prev = '\0';
    bool isNum = false;
    char isStr = '\0';
    bool isVar = false;
    bool overrideColor = false;
    bool autoColor = true;
    bool dontAppend = false;
    bool use_colors = true;

    for (ptr = msg; (ch = *ptr); ptr++) {
        dontAppend = false;

        if (!overrideColor) {
            if (isNum && !isdigit(ch) && !isalpha(ch) && (ch != '.') && (ch != '%')) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isNum = false;
            }
            if (isStr && (isStr == ch) && prev != '\\') {
                isStr = '\0';
            } else if (((ch == '\'') || (ch == '"')) && !isStr &&
                !isalpha(prev) && (prev != '\\'))
            {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
                isStr = ch;
            }

            if ((isdigit(ch) || (ch == '%' && isdigit(prev)) ||
                (ch == '-' && isdigit(ptr[1]))) && !isNum && !isStr && !isVar &&
                 !isalpha(prev) && !isdigit(prev) && (prev != '_') &&
                 (prev != '.'))
            {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
                isNum = true;
            }

            if (isVar && !isalpha(ch) && !isdigit(ch) && ch != '_') {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                isVar = false;
            }

            if (!isStr && !isVar && ch == '$' && isalpha(ptr[1])) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
                isVar = true;
            }
        }

        if (!isVar && !isStr && !isNum && ch == '#' && ptr[1] == '[') {
            bool isColor = true;
            overrideColor = true;

            /* Custom colors */
            if (!ecs_os_strncmp(&ptr[2], "]", ecs_os_strlen("]"))) {
                autoColor = false;
            } else if (!ecs_os_strncmp(&ptr[2], "green]", ecs_os_strlen("green]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREEN);
            } else if (!ecs_os_strncmp(&ptr[2], "red]", ecs_os_strlen("red]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_RED);
            } else if (!ecs_os_strncmp(&ptr[2], "blue]", ecs_os_strlen("red]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_BLUE);
            } else if (!ecs_os_strncmp(&ptr[2], "magenta]", ecs_os_strlen("magenta]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_MAGENTA);
            } else if (!ecs_os_strncmp(&ptr[2], "cyan]", ecs_os_strlen("cyan]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_CYAN);
            } else if (!ecs_os_strncmp(&ptr[2], "yellow]", ecs_os_strlen("yellow]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_YELLOW);
            } else if (!ecs_os_strncmp(&ptr[2], "grey]", ecs_os_strlen("grey]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_GREY);
            } else if (!ecs_os_strncmp(&ptr[2], "white]", ecs_os_strlen("white]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "bold]", ecs_os_strlen("bold]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_BOLD);
            } else if (!ecs_os_strncmp(&ptr[2], "normal]", ecs_os_strlen("normal]"))) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else if (!ecs_os_strncmp(&ptr[2], "reset]", ecs_os_strlen("reset]"))) {
                overrideColor = false;
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            } else {
                isColor = false;
                overrideColor = false;
            }

            if (isColor) {
                ptr += 2;
                while ((ch = *ptr) != ']') ptr ++;
                dontAppend = true;
            }
            if (!autoColor) {
                overrideColor = true;
            }
        }

        if (ch == '\n') {
            if (isNum || isStr || isVar || overrideColor) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
                overrideColor = false;
                isNum = false;
                isStr = false;
                isVar = false;
            }
        }

        if (!dontAppend) {
            ecs_strbuf_appendstrn(&buff, ptr, 1);
        }

        if (!overrideColor) {
            if (((ch == '\'') || (ch == '"')) && !isStr) {
                if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
            }
        }

        prev = ch;
    }

    if (isNum || isStr || isVar || overrideColor) {
        if (use_colors) ecs_strbuf_appendstr(&buff, ECS_NORMAL);
    }

    return ecs_strbuf_get(&buff);
}

static int trace_indent = 0;
static int trace_level = 0;

static
void ecs_log_print(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    va_list args)
{
    (void)level;
    (void)line;

    if (level > trace_level) {
        return;
    }

    /* Massage filename so it doesn't take up too much space */
    char file_buf[256];
    ecs_os_strcpy(file_buf, file);
    file = file_buf;

    char *file_ptr = strrchr(file, '/');
    if (file_ptr) {
        file = file_ptr + 1;
    } else {
        file = file_buf;
    }

    char indent[32];
    int i;
    for (i = 0; i < trace_indent; i ++) {
        indent[i * 2] = '|';
        indent[i * 2 + 1] = ' ';
    }
    indent[i * 2] = '\0';

    char *msg = ecs_vasprintf(fmt, args);
    char *color_msg = ecs_colorize(msg);

    if (level >= 0) {
        ecs_os_log("%sinfo%s: %s%s%s%s:%s%d%s: %s", ECS_MAGENTA, ECS_NORMAL, 
            ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
            color_msg);
    } else if (level == -2) {
        ecs_os_warn("%swarn%s: %s%s%s%s:%s%d%s: %s", ECS_YELLOW, ECS_NORMAL, 
            ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
            color_msg);
    } else if (level == -3) {
        ecs_os_err("%serr%s:  %s%s%s%s:%s%d%s: %s", ECS_RED, ECS_NORMAL, 
            ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
            color_msg);
    } else if (level == -4) {
        ecs_os_err("%sfatal%s:  %s%s%s%s:%s%d%s: %s", ECS_RED, ECS_NORMAL, 
            ECS_GREY, indent, ECS_NORMAL, file, ECS_GREEN, line, ECS_NORMAL, 
            color_msg);
    }

    ecs_os_free(color_msg);
    ecs_os_free(msg);
}

void _ecs_trace(
    int level,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(level, file, line, fmt, args);
    va_end(args);    
}

void _ecs_warn(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(-2, file, line, fmt, args);
    va_end(args);
}

void _ecs_err(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(-3, file, line, fmt, args);
    va_end(args);
}

void _ecs_fatal(
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    ecs_log_print(-4, file, line, fmt, args);
    va_end(args);
}

void ecs_log_push(void) {
    trace_indent ++;
}

void ecs_log_pop(void) {
    trace_indent --;
}

void ecs_tracing_enable(
    int level)
{
    trace_level = level;
}

void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int64_t column,
    const char *fmt,
    ...)
{
    if (trace_level >= -2) {
        va_list args;
        va_start(args, fmt);
        char *msg = ecs_vasprintf(fmt, args);

        if (column != -1) {
            if (name) {
                ecs_os_err("%s:%d: error: %s", name, column + 1, msg);
            } else {
                ecs_os_err("%d: error: %s", column + 1, msg);
            }
        } else {
            if (name) {
                ecs_os_err("%s: error: %s", name, msg);
            } else {
                ecs_os_err("error: %s", msg);
            }            
        }
        
        ecs_os_err("    %s", expr);

        if (column != -1) {
            ecs_os_err("    %*s^", column, "");
        } else {
            ecs_os_err("");
        }

        ecs_os_free(msg);        
    }

    ecs_os_abort();
}

void _ecs_abort(
    int32_t err,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        char *msg = ecs_vasprintf(fmt, args);
        va_end(args);
        _ecs_fatal(file, line, "%s (%s)", msg, ecs_strerror(err));
        ecs_os_free(msg);
    } else {
        _ecs_fatal(file, line, "%s", ecs_strerror(err));
    }

    ecs_os_abort();
}

void _ecs_assert(
    bool condition,
    int32_t err,
    const char *cond_str,
    const char *file,
    int32_t line,
    const char *fmt,
    ...)
{
    if (!condition) {
        if (fmt) {
            va_list args;
            va_start(args, fmt);
            char *msg = ecs_vasprintf(fmt, args);
            va_end(args);            
            _ecs_fatal(file, line, "assert(%s) %s (%s)", 
                cond_str, msg, ecs_strerror(err));
            ecs_os_free(msg);
        } else {
            _ecs_fatal(file, line, "assert(%s) %s", 
                cond_str, ecs_strerror(err));
        }

        ecs_os_abort();
    }
}

void _ecs_deprecated(
    const char *file,
    int32_t line,
    const char *msg)
{
    _ecs_err(file, line, "%s", msg);
}

#define ECS_ERR_STR(code) case code: return &(#code[4])

const char* ecs_strerror(
    int32_t error_code)
{
    switch (error_code) {
    ECS_ERR_STR(ECS_INVALID_PARAMETER);
    ECS_ERR_STR(ECS_NOT_A_COMPONENT);
    ECS_ERR_STR(ECS_TYPE_NOT_AN_ENTITY);
    ECS_ERR_STR(ECS_INTERNAL_ERROR);
    ECS_ERR_STR(ECS_ALREADY_DEFINED);
    ECS_ERR_STR(ECS_INVALID_COMPONENT_SIZE);
    ECS_ERR_STR(ECS_INVALID_COMPONENT_ALIGNMENT);
    ECS_ERR_STR(ECS_OUT_OF_MEMORY);
    ECS_ERR_STR(ECS_MODULE_UNDEFINED);
    ECS_ERR_STR(ECS_COLUMN_INDEX_OUT_OF_RANGE);
    ECS_ERR_STR(ECS_COLUMN_IS_NOT_SHARED);
    ECS_ERR_STR(ECS_COLUMN_IS_SHARED);
    ECS_ERR_STR(ECS_COLUMN_HAS_NO_DATA);
    ECS_ERR_STR(ECS_COLUMN_TYPE_MISMATCH);
    ECS_ERR_STR(ECS_INVALID_WHILE_ITERATING);
    ECS_ERR_STR(ECS_INVALID_FROM_WORKER);
    ECS_ERR_STR(ECS_OUT_OF_RANGE);
    ECS_ERR_STR(ECS_THREAD_ERROR);
    ECS_ERR_STR(ECS_MISSING_OS_API);
    ECS_ERR_STR(ECS_UNSUPPORTED);
    ECS_ERR_STR(ECS_NO_OUT_COLUMNS);
    ECS_ERR_STR(ECS_COLUMN_ACCESS_VIOLATION);
    ECS_ERR_STR(ECS_DESERIALIZE_FORMAT_ERROR);
    ECS_ERR_STR(ECS_TYPE_CONSTRAINT_VIOLATION);
    ECS_ERR_STR(ECS_COMPONENT_NOT_REGISTERED);
    ECS_ERR_STR(ECS_INCONSISTENT_COMPONENT_ID);
    ECS_ERR_STR(ECS_TYPE_INVALID_CASE);
    ECS_ERR_STR(ECS_INCONSISTENT_NAME);
    ECS_ERR_STR(ECS_INCONSISTENT_COMPONENT_ACTION);
    ECS_ERR_STR(ECS_INVALID_OPERATION);
    ECS_ERR_STR(ECS_INVALID_DELETE);
    ECS_ERR_STR(ECS_CYCLE_DETECTED);
    ECS_ERR_STR(ECS_LOCKED_STORAGE);
    }

    return "unknown error code";
}

ecs_data_t* flecs_init_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *result)
{
    ecs_type_t type = table->type; 
    int32_t i, 
    count = table->column_count, 
    sw_count = table->sw_column_count,
    bs_count = table->bs_column_count;

    /* Root tables don't have columns */
    if (!count && !sw_count && !bs_count) {
        result->columns = NULL;
        return result;
    }

    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    if (count && !sw_count) {
        result->columns = ecs_os_calloc(ECS_SIZEOF(ecs_column_t) * count);    
    } else if (count || sw_count) {
        /* If a table has switch columns, store vector with the case values
            * as a regular column, so it's easier to access for systems. To
            * enable this, we need to allocate more space. */
        int32_t type_count = ecs_vector_count(type);
        result->columns = ecs_os_calloc(ECS_SIZEOF(ecs_column_t) * type_count);
    }

    if (count) {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = entities[i];

            /* Is the column a component? */
            const EcsComponent *component = flecs_component_from_id(world, e);
            if (component) {
                /* Is the component associated wit a (non-empty) type? */
                if (component->size) {
                    /* This is a regular component column */
                    result->columns[i].size = flecs_to_i16(component->size);
                    result->columns[i].alignment = flecs_to_i16(component->alignment);
                } else {
                    /* This is a tag */
                }
            } else {
                /* This is an entity that was added to the type */
            }
        }
    }

    if (sw_count) {
        int32_t sw_offset = table->sw_column_offset;
        result->sw_columns = ecs_os_calloc(ECS_SIZEOF(ecs_sw_column_t) * sw_count);

        for (i = 0; i < sw_count; i ++) {
            ecs_entity_t e = entities[i + sw_offset];
            ecs_assert(ECS_HAS_ROLE(e, SWITCH), ECS_INTERNAL_ERROR, NULL);
            e = e & ECS_COMPONENT_MASK;
            const EcsType *type_ptr = ecs_get(world, e, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_type_t sw_type = type_ptr->normalized;

            ecs_entity_t *sw_array = ecs_vector_first(sw_type, ecs_entity_t);
            int32_t sw_array_count = ecs_vector_count(sw_type);

            ecs_switch_t *sw = flecs_switch_new(
                sw_array[0], 
                sw_array[sw_array_count - 1], 
                0);
            result->sw_columns[i].data = sw;
            result->sw_columns[i].type = sw_type;

            int32_t column_id = i + table->sw_column_offset;
            result->columns[column_id].data = flecs_switch_values(sw);
            result->columns[column_id].size = sizeof(ecs_entity_t);
            result->columns[column_id].alignment = ECS_ALIGNOF(ecs_entity_t);
        }
    }
    
    if (bs_count) {
        result->bs_columns = ecs_os_calloc(ECS_SIZEOF(ecs_bs_column_t) * bs_count);
        for (i = 0; i < bs_count; i ++) {
            flecs_bitset_init(&result->bs_columns[i].data);
        }
    }

    return result;
}

static
ecs_flags32_t get_component_action_flags(
    const ecs_type_info_t *c_info) 
{
    ecs_flags32_t flags = 0;

    if (c_info->lifecycle.ctor) {
        flags |= EcsTableHasCtors;
    }
    if (c_info->lifecycle.dtor) {
        flags |= EcsTableHasDtors;
    }
    if (c_info->lifecycle.copy) {
        flags |= EcsTableHasCopy;
    }
    if (c_info->lifecycle.move) {
        flags |= EcsTableHasMove;
    }  

    return flags;  
}

/* Check if table has instance of component, including pairs */
static
bool has_component(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component)
{
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        if (component == ecs_get_typeid(world, entities[i])) {
            return true;
        }
    }
    
    return false;
}

static
void notify_component_info(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_type_t table_type = table->type;
    if (!component || has_component(world, table_type, component)){
        int32_t column_count = ecs_vector_count(table_type);
        ecs_assert(!component || column_count != 0, ECS_INTERNAL_ERROR, NULL);

        if (!column_count) {
            return;
        }
        
        if (!table->c_info) {
            table->c_info = ecs_os_calloc(
                ECS_SIZEOF(ecs_type_info_t*) * column_count);
        }

        /* Reset lifecycle flags before recomputing */
        table->flags &= ~EcsTableHasLifecycle;

        /* Recompute lifecycle flags */
        ecs_entity_t *array = ecs_vector_first(table_type, ecs_entity_t);
        int32_t i;
        for (i = 0; i < column_count; i ++) {
            ecs_entity_t c = ecs_get_typeid(world, array[i]);
            if (!c) {
                continue;
            }
            
            const ecs_type_info_t *c_info = flecs_get_c_info(world, c);
            if (c_info) {
                ecs_flags32_t flags = get_component_action_flags(c_info);
                table->flags |= flags;
            }

            /* Store pointer to c_info for fast access */
            table->c_info[i] = (ecs_type_info_t*)c_info;
        }        
    }
}

static
void notify_trigger(
    ecs_world_t *world, 
    ecs_table_t *table, 
    ecs_entity_t event) 
{
    (void)world;

    if (!(table->flags & EcsTableIsDisabled)) {
        if (event == EcsOnAdd) {
            table->flags |= EcsTableHasOnAdd;
        } else if (event == EcsOnRemove) {
            table->flags |= EcsTableHasOnRemove;
        } else if (event == EcsOnSet) {
            table->flags |= EcsTableHasOnSet;
        } else if (event == EcsUnSet) {
            table->flags |= EcsTableHasUnSet;
        }
    }
}

static
void run_on_remove(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    int32_t count = ecs_vector_count(data->entities);
    if (count) {
        flecs_run_monitors(world, table, table->un_set_all, 0, count, NULL);

        int32_t i, type_count = ecs_vector_count(table->type);
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
        for (i = 0; i < type_count; i ++) {
            ecs_ids_t removed = {
                .array = &ids[i],
                .count = 1
            };

            flecs_run_remove_actions(world, table, data, 0, count, &removed);
        }
    }
}

static
int compare_matched_query(
    const void *ptr1,
    const void *ptr2)
{
    const ecs_matched_query_t *m1 = ptr1;
    const ecs_matched_query_t *m2 = ptr2;
    ecs_query_t *q1 = m1->query;
    ecs_query_t *q2 = m2->query;
    ecs_assert(q1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(q2 != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t s1 = q1->system;
    ecs_entity_t s2 = q2->system;
    ecs_assert(s1 != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(s2 != 0, ECS_INTERNAL_ERROR, NULL);

    return (s1 > s2) - (s1 < s2);
}

static
void add_monitor(
    ecs_vector_t **array,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Add the system to a list that contains all OnSet systems matched with
     * this table. This makes it easy to get the list of systems that need to be
     * executed when all components are set, like when new_w_data is used */
    ecs_matched_query_t *m = ecs_vector_add(array, ecs_matched_query_t);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);

    m->query = query;
    m->matched_table_index = matched_table_index;

    /* Sort the system list so that it is easy to get the difference OnSet
     * OnSet systems between two tables. */
    qsort(
        ecs_vector_first(*array, ecs_matched_query_t), 
        flecs_to_size_t(ecs_vector_count(*array)),
        ECS_SIZEOF(ecs_matched_query_t), 
        compare_matched_query);
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of queries that match so that they can be notified when the table
 * becomes empty / non-empty. */
static
void register_monitor(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    /* First check if system is already registered as monitor. It is possible
     * the query just wants to update the matched_table_index (for example, if
     * query tables got reordered) */
    ecs_vector_each(table->monitors, ecs_matched_query_t, m, {
        if (m->query == query) {
            m->matched_table_index = matched_table_index;
            return;
        }
    });

    add_monitor(&table->monitors, query, matched_table_index);

#ifndef NDEBUG
    char *str = ecs_type_str(world, table->type);
    ecs_trace_2("monitor #[green]%s#[reset] registered with table #[red]%s",
        ecs_get_name(world, query->system), str);
    ecs_os_free(str);
#endif
}

static
bool is_override(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t comp)
{
    if (!(table->flags & EcsTableHasIsA)) {
        return false;
    }

    ecs_type_t type = table->type;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = entities[i];
        if (ECS_HAS_RELATION(e, EcsIsA)) {
            if (ecs_has_id(world, ECS_PAIR_OBJECT(e), comp)) {
                return true;
            }
        }
    }

    return false;
}

static
void register_on_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;

    if (table->column_count) {
        if (!table->on_set) {
            table->on_set = 
                ecs_os_calloc(ECS_SIZEOF(ecs_vector_t) * table->column_count);
        }

        /* Get the matched table which holds the list of actual components */
        ecs_matched_table_t *matched_table = ecs_vector_get(
            query->tables, ecs_matched_table_t, matched_table_index);

        /* Keep track of whether query matches overrides. When a component is
         * removed, diffing these arrays between the source and detination
         * tables gives the list of OnSet systems to run, after exposing the
         * component that was overridden. */
        bool match_override = false;

        /* Add system to each matched column. This makes it easy to get the list 
         * of systems when setting a single component. */
        ecs_term_t *terms = query->filter.terms;
        int32_t i, count = query->filter.term_count;
        
        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            ecs_term_id_t *subj = &term->args[0];
            ecs_oper_kind_t oper = term->oper;

            if (!(subj->set.mask & EcsSelf) || !subj->entity ||
                subj->entity != EcsThis)
            {
                continue;
            }

            if (oper != EcsAnd && oper != EcsOptional) {
                continue;
            }

            ecs_entity_t comp = matched_table->ids[i];
            int32_t index = ecs_type_index_of(table->type, 0, comp);
            if (index == -1) {
                continue;
            }

            if (index >= table->column_count) {
                continue;
            }
            
            ecs_vector_t *set_c = table->on_set[index];
            ecs_matched_query_t *m = ecs_vector_add(&set_c, ecs_matched_query_t);
            m->query = query;
            m->matched_table_index = matched_table_index;
            table->on_set[index] = set_c;
            
            match_override |= is_override(world, table, comp);
        } 

        if (match_override) {
            add_monitor(&table->on_set_override, query, matched_table_index);
        }
    }

    add_monitor(&table->on_set_all, query, matched_table_index);   
}

static
void register_un_set(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    (void)world;
    table->flags |= EcsTableHasUnSet;
    add_monitor(&table->un_set_all, query, matched_table_index);
}

/* -- Private functions -- */

/* If table goes from 0 to >0 entities or from >0 entities to 0 entities notify
 * queries. This allows systems associated with queries to move inactive tables
 * out of the main loop. */
static
void table_activate(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    bool activate)
{
    if (query) {
        flecs_query_notify(world, query, &(ecs_query_event_t) {
            .kind = activate ? EcsQueryTableNonEmpty : EcsQueryTableEmpty,
            .table = table
        });
    } else {
        ecs_vector_t *queries = table->queries;
        ecs_query_t **buffer = ecs_vector_first(queries, ecs_query_t*);
        int32_t i, count = ecs_vector_count(queries);

        for (i = 0; i < count; i ++) {
            flecs_query_notify(world, buffer[i], &(ecs_query_event_t) {
                .kind = activate ? EcsQueryTableNonEmpty : EcsQueryTableEmpty,
                .table = table
            });                
        }
    }     
}

/* This function is called when a query is matched with a table. A table keeps
 * a list of tables that match so that they can be notified when the table
 * becomes empty / non-empty. */
static
void register_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query,
    int32_t matched_table_index)
{
    /* Register system with the table */
    if (!(query->flags & EcsQueryNoActivation)) {
#ifndef NDEBUG
        /* Sanity check if query has already been added */
        int32_t i, count = ecs_vector_count(table->queries);
        for (i = 0; i < count; i ++) {
            ecs_query_t **q = ecs_vector_get(table->queries, ecs_query_t*, i);
            ecs_assert(*q != query, ECS_INTERNAL_ERROR, NULL);
        }
#endif

        ecs_query_t **q = ecs_vector_add(&table->queries, ecs_query_t*);
        if (q) *q = query;

        ecs_data_t *data = flecs_table_get_data(table);
        if (data && ecs_vector_count(data->entities)) {
            table_activate(world, table, query, true);
        }
    }

    /* Register the query as a monitor */
    if (query->flags & EcsQueryMonitor) {
        table->flags |= EcsTableHasMonitors;
        register_monitor(world, table, query, matched_table_index);
    }

    /* Register the query as an on_set system */
    if (query->flags & EcsQueryOnSet) {
        register_on_set(world, table, query, matched_table_index);
    }

    /* Register the query as an un_set system */
    if (query->flags & EcsQueryUnSet) {
        register_un_set(world, table, query, matched_table_index);
    }
}

/* This function is called when a query is unmatched with a table. This can
 * happen for queries that have shared components expressions in their signature
 * and those shared components changed (for example, a base removed a comp). */
static
void unregister_query(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_query_t *query)
{
    (void)world;

    if (!(query->flags & EcsQueryNoActivation)) {
        int32_t i, count = ecs_vector_count(table->queries);
        for (i = 0; i < count; i ++) {
            ecs_query_t **q = ecs_vector_get(table->queries, ecs_query_t*, i);
            if (*q == query) {
                break;
            }
        }

        /* Query must have been registered with table */
        ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);

        /* Remove query */
        ecs_vector_remove(table->queries, ecs_query_t*, i);        
    }
}

ecs_data_t* flecs_table_get_data(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->data;
}

ecs_data_t* flecs_table_get_or_create_data(
    ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_data_t *data = table->data;
    if (!data) {
        data = table->data = ecs_os_calloc(ECS_SIZEOF(ecs_data_t));
    }
    return data;
}

static
void ctor_component(
    ecs_world_t *world,
    ecs_type_info_t * cdata,
    ecs_column_t * column,
    ecs_entity_t * entities,
    int32_t row,
    int32_t count)
{
    /* A new component is constructed */
    ecs_xtor_t ctor;
    if (cdata && (ctor = cdata->lifecycle.ctor)) {
        void *ctx = cdata->lifecycle.ctx;
        int16_t size = column->size;
        int16_t alignment = column->alignment;

        void *ptr = ecs_vector_get_t(column->data, size, alignment, row);

        ctor(world, cdata->component, entities, ptr, 
            flecs_to_size_t(size), count, ctx);
    }
}

static
void dtor_component(
    ecs_world_t *world,
    ecs_type_info_t * cdata,
    ecs_column_t * column,
    ecs_entity_t * entities,
    int32_t row,
    int32_t count)
{
    if (!count) {
        return;
    }
    
    /* An old component is destructed */
    ecs_xtor_t dtor;
    if (cdata && (dtor = cdata->lifecycle.dtor)) {
        void *ctx = cdata->lifecycle.ctx;
        int16_t size = column->size;
        int16_t alignment = column->alignment;

        ecs_assert(column->data != NULL, ECS_INTERNAL_ERROR, NULL);
        void *ptr = ecs_vector_get_t(column->data, size, alignment, row);
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        dtor(world, cdata->component, &entities[row], ptr,
            flecs_to_size_t(size), count, ctx);
    }
}

static
void dtor_all_components(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    bool update_entity_index,
    bool is_delete)
{
    /* Can't delete and not update the entity index */
    ecs_assert(!is_delete || update_entity_index, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t **records = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t i, c, column_count = table->column_count, end = row + count;

    (void)records;

    /* If table has components with destructors, iterate component columns */
    if (table->flags & EcsTableHasDtors) {
        /* Prevent the storage from getting modified while deleting */
        ecs_defer_begin(world);

        /* Throw up a lock just to be sure */
        table->lock = true;

        /* Iterate entities first, then components. This ensures that only one
         * entity is invalidated at a time, which ensures that destructors can
         * safely access other entities. */
        for (i = row; i < end; i ++) {
            for (c = 0; c < column_count; c++) {
                ecs_column_t *column = &data->columns[c];
                dtor_component(world, table->c_info[c], column, entities, i, 1);
            }

            /* Update entity index after invoking destructors so that entity can
             * be safely used in destructor callbacks. */
            if (update_entity_index) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i] == ecs_eis_get(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i]->table == table, 
                    ECS_INTERNAL_ERROR, NULL);

                if (is_delete) {
                    ecs_eis_delete(world, e);
                    ecs_assert(ecs_is_valid(world, e) == false, 
                        ECS_INTERNAL_ERROR, NULL);
                } else {
                    // If this is not a delete, clear the entity index record
                    ecs_record_t r = {NULL, 0};
                    ecs_eis_set(world, e, &r);                
                }
            } else {
                /* This should only happen in rare cases, such as when the data
                 * cleaned up is not part of the world (like with snapshots) */
            }
        }

        table->lock = false;
    
        ecs_defer_end(world);

    /* If table does not have destructors, just update entity index */
    } else if (update_entity_index) {
        if (is_delete) {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i] == ecs_eis_get(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i]->table == table, 
                    ECS_INTERNAL_ERROR, NULL);

                ecs_eis_delete(world, e);
                ecs_assert(!ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
            } 
        } else {
            for (i = row; i < end; i ++) {
                ecs_entity_t e = entities[i];
                ecs_assert(!e || ecs_is_valid(world, e), ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i] == ecs_eis_get(world, e), 
                    ECS_INTERNAL_ERROR, NULL);
                ecs_assert(!e || records[i]->table == table, 
                    ECS_INTERNAL_ERROR, NULL);                
                ecs_record_t r = {NULL, 0};
                ecs_eis_set(world, e, &r);
            }
        }      
    }
}

static
void fini_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    bool do_on_remove,
    bool update_entity_index,
    bool is_delete,
    bool deactivate)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    if (!data) {
        return;
    }

    if (do_on_remove) {
        run_on_remove(world, table, data);        
    }

    int32_t count = flecs_table_data_count(data);
    if (count) {
        dtor_all_components(world, table, data, 0, count, 
            update_entity_index, is_delete);
    }

    /* Sanity check */
    ecs_assert(ecs_vector_count(data->record_ptrs) == 
        ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    
    ecs_column_t *columns = data->columns;
    if (columns) {
        int32_t c, column_count = table->column_count;
        for (c = 0; c < column_count; c ++) {
            /* Sanity check */
            ecs_assert(!columns[c].data || (ecs_vector_count(columns[c].data) == 
                ecs_vector_count(data->entities)), ECS_INTERNAL_ERROR, NULL);

            ecs_vector_free(columns[c].data);
        }
        ecs_os_free(columns);
        data->columns = NULL;
    }

    ecs_sw_column_t *sw_columns = data->sw_columns;
    if (sw_columns) {
        int32_t c, column_count = table->sw_column_count;
        for (c = 0; c < column_count; c ++) {
            flecs_switch_free(sw_columns[c].data);
        }
        ecs_os_free(sw_columns);
        data->sw_columns = NULL;
    }

    ecs_bs_column_t *bs_columns = data->bs_columns;
    if (bs_columns) {
        int32_t c, column_count = table->bs_column_count;
        for (c = 0; c < column_count; c ++) {
            flecs_bitset_deinit(&bs_columns[c].data);
        }
        ecs_os_free(bs_columns);
        data->bs_columns = NULL;
    }

    ecs_vector_free(data->entities);
    ecs_vector_free(data->record_ptrs);

    data->entities = NULL;
    data->record_ptrs = NULL;

    if (deactivate && count) {
        table_activate(world, table, 0, false);
    }
}

/* Cleanup, no OnRemove, don't update entity index, don't deactivate table */
void flecs_table_clear_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data)
{
    fini_data(world, table, data, false, false, false, false);
}

/* Cleanup, no OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities_silent(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, flecs_table_get_data(table), 
        false, true, false, true);
}

/* Cleanup, run OnRemove, clear entity index, deactivate table */
void flecs_table_clear_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, flecs_table_get_data(table), true, true, false, true);
}

/* Cleanup, run OnRemove, delete from entity index, deactivate table */
void flecs_table_delete_entities(
    ecs_world_t *world,
    ecs_table_t *table)
{
    fini_data(world, table, flecs_table_get_data(table), true, true, true, true);
}

/* Unset all components in table. This function is called before a table is 
 * deleted, and invokes all UnSet handlers, if any */
void flecs_table_remove_actions(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_data_t *data = flecs_table_get_data(table);
    if (data) {
        run_on_remove(world, table, data);
    }   
}

/* Free table resources. */
void flecs_table_free(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    (void)world;

    /* Cleanup data, no OnRemove, delete from entity index, don't deactivate */
    ecs_data_t *data = flecs_table_get_data(table);
    fini_data(world, table, data, false, true, true, false);

    flecs_table_clear_edges(world, table);

    flecs_unregister_table(world, table);

    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    ecs_vector_free(table->queries);
    ecs_os_free(table->dirty_state);
    ecs_vector_free(table->monitors);
    ecs_vector_free(table->on_set_all);
    ecs_vector_free(table->on_set_override);
    ecs_vector_free(table->un_set_all);

    if (table->c_info) {
        ecs_os_free(table->c_info);
    }
    
    if (table->on_set) {
        int32_t i;
        for (i = 0; i < table->column_count; i ++) {
            ecs_vector_free(table->on_set[i]);
        }
        ecs_os_free(table->on_set);
    }

    table->id = 0;

    ecs_os_free(table->data);
}

/* Free table type. Do this separately from freeing the table as types can be
 * in use by application destructors. */
void flecs_table_free_type(
    ecs_table_t *table)
{
    ecs_vector_free((ecs_vector_t*)table->type);
}

/* Reset a table to its initial state. */
void flecs_table_reset(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    
    (void)world;
    ecs_os_free(table->lo_edges);
    ecs_map_free(table->hi_edges);
    table->lo_edges = NULL;
    table->hi_edges = NULL;
}

static
void mark_table_dirty(
    ecs_table_t *table,
    int32_t index)
{
    if (table->dirty_state) {
        table->dirty_state[index] ++;
    }
}

void flecs_table_mark_dirty(
    ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->dirty_state) {
        int32_t index = ecs_type_index_of(table->type, 0, component);
        ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);
        table->dirty_state[index] ++;
    }
}

static
void move_switch_columns(
    ecs_table_t * new_table, 
    ecs_data_t * new_data, 
    int32_t new_index,
    ecs_table_t * old_table, 
    ecs_data_t * old_data, 
    int32_t old_index,
    int32_t count)
{
    int32_t i_old = 0, old_column_count = old_table->sw_column_count;
    int32_t i_new = 0, new_column_count = new_table->sw_column_count;

    if (!old_column_count || !new_column_count) {
        return;
    }

    ecs_sw_column_t *old_columns = old_data->sw_columns;
    ecs_sw_column_t *new_columns = new_data->sw_columns;

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t offset_new = new_table->sw_column_offset;
    int32_t offset_old = old_table->sw_column_offset;

    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new + offset_new];
        ecs_entity_t old_component = old_components[i_old + offset_old];

        if (new_component == old_component) {
            ecs_switch_t *old_switch = old_columns[i_old].data;
            ecs_switch_t *new_switch = new_columns[i_new].data;

            flecs_switch_ensure(new_switch, new_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_switch_get(old_switch, old_index + i);
                flecs_switch_set(new_switch, new_index + i, value);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

static
void move_bitset_columns(
    ecs_table_t * new_table, 
    ecs_data_t * new_data, 
    int32_t new_index,
    ecs_table_t * old_table, 
    ecs_data_t * old_data, 
    int32_t old_index,
    int32_t count)
{
    int32_t i_old = 0, old_column_count = old_table->bs_column_count;
    int32_t i_new = 0, new_column_count = new_table->bs_column_count;

    if (!old_column_count || !new_column_count) {
        return;
    }

    ecs_bs_column_t *old_columns = old_data->bs_columns;
    ecs_bs_column_t *new_columns = new_data->bs_columns;

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t offset_new = new_table->bs_column_offset;
    int32_t offset_old = old_table->bs_column_offset;

    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new + offset_new];
        ecs_entity_t old_component = old_components[i_old + offset_old];

        if (new_component == old_component) {
            ecs_bitset_t *old_bs = &old_columns[i_old].data;
            ecs_bitset_t *new_bs = &new_columns[i_new].data;

            flecs_bitset_ensure(new_bs, new_index + count);

            int i;
            for (i = 0; i < count; i ++) {
                uint64_t value = flecs_bitset_get(old_bs, old_index + i);
                flecs_bitset_set(new_bs, new_index + i, value);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

static
void ensure_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t * column_count_out,
    int32_t * sw_column_count_out,
    int32_t * bs_column_count_out,
    ecs_column_t ** columns_out,
    ecs_sw_column_t ** sw_columns_out,
    ecs_bs_column_t ** bs_columns_out)
{
    int32_t column_count = table->column_count;
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = NULL;
    ecs_sw_column_t *sw_columns = NULL;
    ecs_bs_column_t *bs_columns = NULL;

    /* It is possible that the table data was created without content. 
     * Now that data is going to be written to the table, initialize */ 
    if (column_count | sw_column_count | bs_column_count) {
        columns = data->columns;
        sw_columns = data->sw_columns;
        bs_columns = data->bs_columns;

        if (!columns && !sw_columns && !bs_columns) {
            flecs_init_data(world, table, data);
            columns = data->columns;
            sw_columns = data->sw_columns;
            bs_columns = data->bs_columns;

            ecs_assert(sw_column_count == 0 || sw_columns != NULL, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(bs_column_count == 0 || bs_columns != NULL, 
                ECS_INTERNAL_ERROR, NULL);
        }

        *column_count_out = column_count;
        *sw_column_count_out = sw_column_count;
        *bs_column_count_out = bs_column_count;
        *columns_out = columns;
        *sw_columns_out = sw_columns;
        *bs_columns_out = bs_columns;
    }

    ecs_assert(!column_count || columns, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!sw_column_count || sw_columns, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!bs_column_count || bs_columns, ECS_INTERNAL_ERROR, NULL);
}

static
void grow_column(
    ecs_world_t *world,
    ecs_entity_t * entities,
    ecs_column_t * column,
    ecs_type_info_t * c_info,
    int32_t to_add,
    int32_t new_size,
    bool construct)
{
    ecs_vector_t *vec = column->data;
    int16_t alignment = column->alignment;

    int32_t size = column->size;
    int32_t count = ecs_vector_count(vec);
    int32_t old_size = ecs_vector_size(vec);
    int32_t new_count = count + to_add;
    bool can_realloc = new_size != old_size;

    ecs_assert(new_size >= new_count, ECS_INTERNAL_ERROR, NULL);

    /* If the array could possibly realloc and the component has a move action 
     * defined, move old elements manually */
    ecs_move_t move;
    if (c_info && count && can_realloc && (move = c_info->lifecycle.move)) {
        ecs_xtor_t ctor = c_info->lifecycle.ctor;
        ecs_assert(ctor != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Create new vector */
        ecs_vector_t *new_vec = ecs_vector_new_t(size, alignment, new_size);
        ecs_vector_set_count_t(&new_vec, size, alignment, new_count);

        void *old_buffer = ecs_vector_first_t(
            vec, size, alignment);

        void *new_buffer = ecs_vector_first_t(
            new_vec, size, alignment);

        /* First construct elements (old and new) in new buffer */
        ctor(world, c_info->component, entities, new_buffer, 
            flecs_to_size_t(size), construct ? new_count : count, 
            c_info->lifecycle.ctx);
        
        /* Move old elements */
        move(world, c_info->component, entities, entities, 
            new_buffer, old_buffer, flecs_to_size_t(size), count, 
            c_info->lifecycle.ctx);

        /* Free old vector */
        ecs_vector_free(vec);
        column->data = new_vec;
    } else {
        /* If array won't realloc or has no move, simply add new elements */
        if (can_realloc) {
            ecs_vector_set_size_t(&vec, size, alignment, new_size);
        }

        void *elem = ecs_vector_addn_t(&vec, size, alignment, to_add);

        ecs_xtor_t ctor;
        if (construct && c_info && (ctor = c_info->lifecycle.ctor)) {
            /* If new elements need to be constructed and component has a
             * constructor, construct */
            ctor(world, c_info->component, &entities[count], elem, 
                flecs_to_size_t(size), to_add, c_info->lifecycle.ctx);
        }

        column->data = vec;
    }

    ecs_assert(ecs_vector_size(column->data) == new_size, 
        ECS_INTERNAL_ERROR, NULL);
}

static
int32_t grow_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t to_add,
    int32_t size,
    const ecs_entity_t *ids)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t cur_count = flecs_table_data_count(data);
    int32_t column_count = table->column_count;
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = NULL;
    ecs_sw_column_t *sw_columns = NULL;
    ecs_bs_column_t *bs_columns = NULL;
    ensure_data(world, table, data, &column_count, &sw_column_count, 
        &bs_column_count, &columns, &sw_columns, &bs_columns);    

    /* Add record to record ptr array */
    ecs_vector_set_size(&data->record_ptrs, ecs_record_t*, size);
    ecs_record_t **r = ecs_vector_addn(&data->record_ptrs, ecs_record_t*, to_add);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    if (ecs_vector_size(data->record_ptrs) > size) {
        size = ecs_vector_size(data->record_ptrs);
    }

    /* Add entity to column with entity ids */
    ecs_vector_set_size(&data->entities, ecs_entity_t, size);
    ecs_entity_t *e = ecs_vector_addn(&data->entities, ecs_entity_t, to_add);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_size(data->entities) == size, ECS_INTERNAL_ERROR, NULL);

    /* Initialize entity ids and record ptrs */
    int32_t i;
    if (ids) {
        for (i = 0; i < to_add; i ++) {
            e[i] = ids[i];
        }
    } else {
        ecs_os_memset(e, 0, ECS_SIZEOF(ecs_entity_t) * to_add);
    }
    ecs_os_memset(r, 0, ECS_SIZEOF(ecs_record_t*) * to_add);

    /* Add elements to each column array */
    ecs_type_info_t **c_info_array = table->c_info;
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        if (!column->size) {
            continue;
        }

        ecs_type_info_t *c_info = NULL;
        if (c_info_array) {
            c_info = c_info_array[i];
        }

        grow_column(world, entities, column, c_info, to_add, size, true);
        ecs_assert(ecs_vector_size(columns[i].data) == size, 
            ECS_INTERNAL_ERROR, NULL);
    }

    /* Add elements to each switch column */
    for (i = 0; i < sw_column_count; i ++) {
        ecs_switch_t *sw = sw_columns[i].data;
        flecs_switch_addn(sw, to_add);
    }

    /* Add elements to each bitset column */
    for (i = 0; i < bs_column_count; i ++) {
        ecs_bitset_t *bs = &bs_columns[i].data;
        flecs_bitset_addn(bs, to_add);
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);

    if (!world->is_readonly && !cur_count) {
        table_activate(world, table, 0, true);
    }

    table->alloc_count ++;

    /* Return index of first added entity */
    return cur_count;
}

static
void fast_append(
    ecs_column_t *columns,
    int32_t column_count)
{
    /* Add elements to each column array */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        int16_t size = column->size;
        if (size) {
            int16_t alignment = column->alignment;
            ecs_vector_add_t(&column->data, size, alignment);
        }
    }
}

int32_t flecs_table_append(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    ecs_entity_t entity,
    ecs_record_t * record,
    bool construct)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    /* Get count & size before growing entities array. This tells us whether the
     * arrays will realloc */
    int32_t count = ecs_vector_count(data->entities);
    int32_t size = ecs_vector_size(data->entities);

    int32_t column_count = table->column_count;
    int32_t sw_column_count = table->sw_column_count;
    int32_t bs_column_count = table->bs_column_count;
    ecs_column_t *columns = NULL;
    ecs_sw_column_t *sw_columns = NULL;
    ecs_bs_column_t *bs_columns = NULL;

    ensure_data(world, table, data, &column_count, &sw_column_count,
        &bs_column_count, &columns, &sw_columns, &bs_columns);

    /* Grow buffer with entity ids, set new element to new entity */
    ecs_entity_t *e = ecs_vector_add(&data->entities, ecs_entity_t);
    ecs_assert(e != NULL, ECS_INTERNAL_ERROR, NULL);
    *e = entity;    

    /* Keep track of alloc count. This allows references to check if cached
     * pointers need to be updated. */  
    table->alloc_count += (count == size);

    /* Add record ptr to array with record ptrs */
    ecs_record_t **r = ecs_vector_add(&data->record_ptrs, ecs_record_t*);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    *r = record;
 
    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);

    /* If this is the first entity in this table, signal queries so that the
     * table moves from an inactive table to an active table. */
    if (!world->is_readonly && !count) {
        table_activate(world, table, 0, true);
    } 

    ecs_assert(count >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Fast path: no switch columns, no lifecycle actions */
    if (!(table->flags & EcsTableIsComplex)) {
        fast_append(columns, column_count);
        return count;
    }

    ecs_type_info_t **c_info_array = table->c_info;
    ecs_entity_t *entities = ecs_vector_first(
        data->entities, ecs_entity_t);

    /* Reobtain size to ensure that the columns have the same size as the 
     * entities and record vectors. This keeps reasoning about when allocations
     * occur easier. */
    size = ecs_vector_size(data->entities);

    /* Grow component arrays with 1 element */
    int32_t i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        if (!column->size) {
            continue;
        }

        ecs_type_info_t *c_info = NULL;
        if (c_info_array) {
            c_info = c_info_array[i];
        }

        grow_column(world, entities, column, c_info, 1, size, construct);
        
        ecs_assert(
            ecs_vector_size(columns[i].data) == ecs_vector_size(data->entities), 
            ECS_INTERNAL_ERROR, NULL); 
            
        ecs_assert(
            ecs_vector_count(columns[i].data) == ecs_vector_count(data->entities), 
            ECS_INTERNAL_ERROR, NULL);                        
    }

    /* Add element to each switch column */
    for (i = 0; i < sw_column_count; i ++) {
        ecs_assert(sw_columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_switch_t *sw = sw_columns[i].data;
        flecs_switch_add(sw);
        columns[i + table->sw_column_offset].data = flecs_switch_values(sw);
    }

    /* Add element to each bitset column */
    for (i = 0; i < bs_column_count; i ++) {
        ecs_assert(bs_columns != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_bitset_t *bs = &bs_columns[i].data;
        flecs_bitset_addn(bs, 1);
    }    

    return count;
}

static
void fast_delete_last(
    ecs_column_t *columns,
    int32_t column_count) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        ecs_vector_remove_last(column->data);
    }
}

static
void fast_delete(
    ecs_column_t *columns,
    int32_t column_count,
    int32_t index) 
{
    int i;
    for (i = 0; i < column_count; i ++) {
        ecs_column_t *column = &columns[i];
        int16_t size = column->size;
        if (size) {
            int16_t alignment = column->alignment;
            ecs_vector_remove_t(column->data, size, alignment, index);
        } 
    }
}

void flecs_table_delete(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t index,
    bool destruct)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_vector_t *v_entities = data->entities;
    int32_t count = ecs_vector_count(v_entities);

    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    count --;
    ecs_assert(index <= count, ECS_INTERNAL_ERROR, NULL);

    /* Move last entity id to index */
    ecs_entity_t *entities = ecs_vector_first(v_entities, ecs_entity_t);
    ecs_entity_t entity_to_move = entities[count];
    ecs_entity_t entity_to_delete = entities[index];
    entities[index] = entity_to_move;
    ecs_vector_remove_last(v_entities);

    /* Move last record ptr to index */
    ecs_vector_t *v_records = data->record_ptrs;     
    ecs_assert(count < ecs_vector_count(v_records), ECS_INTERNAL_ERROR, NULL);

    ecs_record_t **records = ecs_vector_first(v_records, ecs_record_t*);
    ecs_record_t *record_to_move = records[count];
    records[index] = record_to_move;
    ecs_vector_remove_last(v_records); 

    /* Update record of moved entity in entity index */
    if (index != count) {
        if (record_to_move) {
            if (record_to_move->row >= 0) {
                record_to_move->row = index + 1;
            } else {
                record_to_move->row = -(index + 1);
            }
            ecs_assert(record_to_move->table != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(record_to_move->table == table, ECS_INTERNAL_ERROR, NULL);
        }
    }     

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    

    /* If table is empty, deactivate it */
    if (!count) {
        table_activate(world, table, NULL, false);
    }

    /* Destruct component data */
    ecs_type_info_t **c_info_array = table->c_info;
    ecs_column_t *columns = data->columns;
    int32_t column_count = table->column_count;
    int32_t i;

    /* If this is a table without lifecycle callbacks or special columns, take
     * fast path that just remove an element from the array(s) */
    if (!(table->flags & EcsTableIsComplex)) {
        if (index == count) {
            fast_delete_last(columns, column_count);
        } else {
            fast_delete(columns, column_count, index);
        }

        return;
    }

    /* Last element, destruct & remove */
    if (index == count) {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & EcsTableHasDtors)) {
            ecs_assert(c_info_array != NULL, ECS_INTERNAL_ERROR, NULL);
            
            for (i = 0; i < column_count; i ++) {
                ecs_type_info_t *c_info = c_info_array[i];
                ecs_xtor_t dtor;
                if (c_info && (dtor = c_info->lifecycle.dtor)) {
                    ecs_size_t size = c_info->size;
                    ecs_size_t alignment = c_info->alignment;
                    dtor(world, c_info->component, &entity_to_delete,
                        ecs_vector_last_t(columns[i].data, size, alignment),
                        flecs_to_size_t(size), 1, c_info->lifecycle.ctx);
                }        
            }
        }

        fast_delete_last(columns, column_count);

    /* Not last element, move last element to deleted element & destruct */
    } else {
        /* If table has component destructors, invoke */
        if (destruct && (table->flags & (EcsTableHasDtors | EcsTableHasMove))) {
            ecs_assert(c_info_array != NULL, ECS_INTERNAL_ERROR, NULL);

            for (i = 0; i < column_count; i ++) {
                ecs_column_t *column = &columns[i];
                ecs_size_t size = column->size;
                ecs_size_t align = column->alignment;
                ecs_vector_t *vec = column->data;
                void *dst = ecs_vector_get_t(vec, size, align, index);
                void *src = ecs_vector_last_t(vec, size, align);
                
                ecs_type_info_t *c_info = c_info_array[i];
                ecs_move_ctor_t move_dtor;
                if (c_info && (move_dtor = c_info->lifecycle.move_dtor)) {
                    move_dtor(world, c_info->component, &c_info->lifecycle,
                        &entity_to_move, &entity_to_delete, dst, src, 
                        flecs_to_size_t(size), 1, c_info->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, src, size);
                }

                ecs_vector_remove_last(vec);
            }

        } else {
            fast_delete(columns, column_count, index);
        }
    }

    /* Remove elements from switch columns */
    ecs_sw_column_t *sw_columns = data->sw_columns;
    int32_t sw_column_count = table->sw_column_count;
    for (i = 0; i < sw_column_count; i ++) {
        flecs_switch_remove(sw_columns[i].data, index);
    }

    /* Remove elements from bitset columns */
    ecs_bs_column_t *bs_columns = data->bs_columns;
    int32_t bs_column_count = table->bs_column_count;
    for (i = 0; i < bs_column_count; i ++) {
        flecs_bitset_remove(&bs_columns[i].data, index);
    }
}

static
void fast_move(
    ecs_table_t * new_table,
    ecs_data_t * new_data,
    int32_t new_index,
    ecs_table_t * old_table,
    ecs_data_t * old_data,
    int32_t old_index)
{
    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t i_new = 0, new_column_count = new_table->column_count;
    int32_t i_old = 0, old_column_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if (new_component == old_component) {
            ecs_column_t *new_column = &new_columns[i_new];
            ecs_column_t *old_column = &old_columns[i_old];
            int16_t size = new_column->size;

            if (size) {
                int16_t alignment = new_column->alignment;
                void *dst = ecs_vector_get_t(
                    new_column->data, size, alignment, new_index);
                void *src = ecs_vector_get_t(
                    old_column->data, size, alignment, old_index);

                ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_os_memcpy(dst, src, size); 
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }
}

void flecs_table_move(
    ecs_world_t *world,
    ecs_entity_t dst_entity,
    ecs_entity_t src_entity,
    ecs_table_t *new_table,
    ecs_data_t *new_data,
    int32_t new_index,
    ecs_table_t *old_table,
    ecs_data_t *old_data,
    int32_t old_index,
    bool construct)
{
    ecs_assert(new_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!new_table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(!old_table->lock, ECS_LOCKED_STORAGE, NULL);

    ecs_assert(old_index >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(old_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(new_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!((new_table->flags | old_table->flags) & EcsTableIsComplex)) {
        fast_move(new_table, new_data, new_index, old_table, old_data, old_index);
        return;
    }

    move_switch_columns(
        new_table, new_data, new_index, old_table, old_data, old_index, 1);

    move_bitset_columns(
        new_table, new_data, new_index, old_table, old_data, old_index, 1);

    bool same_entity = dst_entity == src_entity;

    ecs_type_t new_type = new_table->type;
    ecs_type_t old_type = old_table->type;

    int32_t i_new = 0, new_column_count = new_table->column_count;
    int32_t i_old = 0, old_column_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    for (; (i_new < new_column_count) && (i_old < old_column_count);) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];

        if (new_component == old_component) {
            ecs_column_t *new_column = &new_columns[i_new];
            ecs_column_t *old_column = &old_columns[i_old];
            int16_t size = new_column->size;
            int16_t alignment = new_column->alignment;

            if (size) {
                void *dst = ecs_vector_get_t(
                    new_column->data, size, alignment, new_index);
                void *src = ecs_vector_get_t(
                    old_column->data, size, alignment, old_index);

                ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);
                ecs_assert(src != NULL, ECS_INTERNAL_ERROR, NULL);

                ecs_type_info_t *cdata = new_table->c_info[i_new];
                if (same_entity) {
                    ecs_move_ctor_t callback;
                    if (cdata && (callback = cdata->lifecycle.ctor_move_dtor)) {
                        void *ctx = cdata->lifecycle.ctx;
                        /* ctor + move + dtor */
                        callback(world, new_component, &cdata->lifecycle, 
                            &dst_entity, &src_entity, 
                            dst, src, flecs_to_size_t(size), 1, ctx);
                    } else {
                        ecs_os_memcpy(dst, src, size);
                    }
                } else {
                    ecs_copy_ctor_t copy;
                    if (cdata && (copy = cdata->lifecycle.copy_ctor)) {
                        void *ctx = cdata->lifecycle.ctx;
                        copy(world, new_component, &cdata->lifecycle, 
                            &dst_entity, &src_entity, 
                            dst, src, flecs_to_size_t(size), 1, ctx);
                    } else {
                        ecs_os_memcpy(dst, src, size);
                    }
                }
            }
        } else {
            if (construct && new_component < old_component) {
                ctor_component(world, new_table->c_info[i_new],
                    &new_columns[i_new], &dst_entity, new_index, 1);
            } else {
                dtor_component(world, old_table->c_info[i_old],
                    &old_columns[i_old], &src_entity, old_index, 1);
            }
        }

        i_new += new_component <= old_component;
        i_old += new_component >= old_component;
    }

    if (construct) {
        for (; (i_new < new_column_count); i_new ++) {
            ctor_component(world, new_table->c_info[i_new],
                &new_columns[i_new], &dst_entity, new_index, 1);
        }
    }

    for (; (i_old < old_column_count); i_old ++) {
        dtor_component(world, old_table->c_info[i_old],
            &old_columns[i_old], &src_entity, old_index, 1);
    }
}

int32_t flecs_table_appendn(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t to_add,
    const ecs_entity_t *ids)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    int32_t cur_count = flecs_table_data_count(data);
    return grow_data(world, table, data, to_add, cur_count + to_add, ids);
}

void flecs_table_set_size(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t size)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    int32_t cur_count = flecs_table_data_count(data);

    if (cur_count < size) {
        grow_data(world, table, data, 0, size, NULL);
    } else if (!size) {
        /* Initialize columns if 0 is passed. This is a shortcut to initialize
         * columns when, for example, an API call is inserting bulk data. */
        int32_t column_count = table->column_count;
        int32_t sw_column_count = table->sw_column_count;
        int32_t bs_column_count = table->bs_column_count;
        ecs_column_t *columns;
        ecs_sw_column_t *sw_columns;
        ecs_bs_column_t *bs_columns;
        ensure_data(world, table, data, &column_count, &sw_column_count,
            &bs_column_count, &columns, &sw_columns, &bs_columns);
    }
}

int32_t flecs_table_data_count(
    const ecs_data_t *data)
{
    return data ? ecs_vector_count(data->entities) : 0;
}

static
void swap_switch_columns(
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t row_1,
    int32_t row_2)
{
    int32_t i = 0, column_count = table->sw_column_count;
    if (!column_count) {
        return;
    }

    ecs_sw_column_t *columns = data->sw_columns;

    for (i = 0; i < column_count; i ++) {
        ecs_switch_t *sw = columns[i].data;
        flecs_switch_swap(sw, row_1, row_2);
    }
}

static
void swap_bitset_columns(
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t row_1,
    int32_t row_2)
{
    int32_t i = 0, column_count = table->bs_column_count;
    if (!column_count) {
        return;
    }

    ecs_bs_column_t *columns = data->bs_columns;

    for (i = 0; i < column_count; i ++) {
        ecs_bitset_t *bs = &columns[i].data;
        flecs_bitset_swap(bs, row_1, row_2);
    }
}

void flecs_table_swap(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row_1,
    int32_t row_2)
{    
    (void)world;

    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_1 >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row_2 >= 0, ECS_INTERNAL_ERROR, NULL);
    
    if (row_1 == row_2) {
        return;
    }

    /* If the table is monitored indicate that there has been a change */
    mark_table_dirty(table, 0);    

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_entity_t e1 = entities[row_1];
    ecs_entity_t e2 = entities[row_2];

    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    ecs_record_t *record_ptr_1 = record_ptrs[row_1];
    ecs_record_t *record_ptr_2 = record_ptrs[row_2];

    ecs_assert(record_ptr_1 != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(record_ptr_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Keep track of whether entity is watched */
    bool watched_1 = record_ptr_1->row < 0;
    bool watched_2 = record_ptr_2->row < 0;

    /* Swap entities & records */
    entities[row_1] = e2;
    entities[row_2] = e1;
    record_ptr_1->row = flecs_row_to_record(row_2, watched_1);
    record_ptr_2->row = flecs_row_to_record(row_1, watched_2);
    record_ptrs[row_1] = record_ptr_2;
    record_ptrs[row_2] = record_ptr_1;

    swap_switch_columns(table, data, row_1, row_2);
    swap_bitset_columns(table, data, row_1, row_2);  

    ecs_column_t *columns = data->columns;
    if (!columns) {
        return;
    }

    /* Swap columns */
    int32_t i, column_count = table->column_count;
    
    for (i = 0; i < column_count; i ++) {
        int16_t size = columns[i].size;
        int16_t alignment = columns[i].alignment;
        void *ptr = ecs_vector_first_t(columns[i].data, size, alignment);

        if (size) {
            void *tmp = ecs_os_alloca(size);

            void *el_1 = ECS_OFFSET(ptr, size * row_1);
            void *el_2 = ECS_OFFSET(ptr, size * row_2);

            ecs_os_memcpy(tmp, el_1, size);
            ecs_os_memcpy(el_1, el_2, size);
            ecs_os_memcpy(el_2, tmp, size);
        }
    }  
}

static
void merge_vector(
    ecs_vector_t **dst_out,
    ecs_vector_t *src,
    int16_t size,
    int16_t alignment)
{
    ecs_vector_t *dst = *dst_out;
    int32_t dst_count = ecs_vector_count(dst);

    if (!dst_count) {
        if (dst) {
            ecs_vector_free(dst);
        }

        *dst_out = src;
    
    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = ecs_vector_count(src);
        ecs_vector_set_count_t(&dst, size, alignment, dst_count + src_count);
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        
        ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);

        ecs_vector_free(src);
        *dst_out = dst;
    }
}

static
void merge_column(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t column_id,
    ecs_vector_t *src)
{
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    ecs_type_info_t *c_info = table->c_info[column_id];
    ecs_column_t *column = &data->columns[column_id];
    ecs_vector_t *dst = column->data;
    int16_t size = column->size;
    int16_t alignment = column->alignment;
    int32_t dst_count = ecs_vector_count(dst);

    if (!dst_count) {
        if (dst) {
            ecs_vector_free(dst);
        }

        column->data = src;
    
    /* If the new table is not empty, copy the contents from the
     * src into the dst. */
    } else {
        int32_t src_count = ecs_vector_count(src);
        ecs_vector_set_count_t(&dst, size, alignment, dst_count + src_count);
        column->data = dst;

        /* Construct new values */
        if (c_info) {
            ctor_component(
                world, c_info, column, entities, dst_count, src_count);
        }
        
        void *dst_ptr = ecs_vector_first_t(dst, size, alignment);
        void *src_ptr = ecs_vector_first_t(src, size, alignment);

        dst_ptr = ECS_OFFSET(dst_ptr, size * dst_count);
        
        /* Move values into column */
        ecs_move_t move;
        if (c_info && (move = c_info->lifecycle.move)) {
            move(world, c_info->component, entities, entities, 
                dst_ptr, src_ptr, flecs_to_size_t(size), src_count, 
                c_info->lifecycle.ctx);
        } else {
            ecs_os_memcpy(dst_ptr, src_ptr, size * src_count);
        }

        ecs_vector_free(src);
    }
}

static
void merge_table_data(
    ecs_world_t *world,
    ecs_table_t * new_table,
    ecs_table_t * old_table,
    int32_t old_count,
    int32_t new_count,
    ecs_data_t * old_data,
    ecs_data_t * new_data)
{
    int32_t i_new = 0, new_component_count = new_table->column_count;
    int32_t i_old = 0, old_component_count = old_table->column_count;
    ecs_entity_t *new_components = ecs_vector_first(new_table->type, ecs_entity_t);
    ecs_entity_t *old_components = ecs_vector_first(old_table->type, ecs_entity_t);

    ecs_column_t *old_columns = old_data->columns;
    ecs_column_t *new_columns = new_data->columns;

    if (!new_columns && !new_data->entities) {
        flecs_init_data(world, new_table, new_data);
        new_columns = new_data->columns;
    }
    
    ecs_assert(!new_component_count || new_columns, ECS_INTERNAL_ERROR, NULL);

    if (!old_count) {
        return;
    }

    /* Merge entities */
    merge_vector(&new_data->entities, old_data->entities, ECS_SIZEOF(ecs_entity_t), 
        ECS_ALIGNOF(ecs_entity_t));
    old_data->entities = NULL;
    ecs_entity_t *entities = ecs_vector_first(new_data->entities, ecs_entity_t);

    ecs_assert(ecs_vector_count(new_data->entities) == old_count + new_count, 
        ECS_INTERNAL_ERROR, NULL);

    /* Merge entity index record pointers */
    merge_vector(&new_data->record_ptrs, old_data->record_ptrs, 
        ECS_SIZEOF(ecs_record_t*), ECS_ALIGNOF(ecs_record_t*));
    old_data->record_ptrs = NULL;        

    for (; (i_new < new_component_count) && (i_old < old_component_count); ) {
        ecs_entity_t new_component = new_components[i_new];
        ecs_entity_t old_component = old_components[i_old];
        int16_t size = new_columns[i_new].size;
        int16_t alignment = new_columns[i_new].alignment;

        if (new_component == old_component) {
            merge_column(world, new_table, new_data, i_new, 
                old_columns[i_old].data);
            old_columns[i_old].data = NULL;

            /* Mark component column as dirty */
            mark_table_dirty(new_table, i_new + 1);
            
            i_new ++;
            i_old ++;
        } else if (new_component < old_component) {
            /* New column does not occur in old table, make sure vector is large
             * enough. */
            if (size) {
                ecs_column_t *column = &new_columns[i_new];
                ecs_vector_set_count_t(&column->data, size, alignment,
                    old_count + new_count);

                /* Construct new values */
                ecs_type_info_t *c_info = new_table->c_info[i_new];
                if (c_info) {
                    ctor_component(world, c_info, column, 
                        entities, 0, old_count + new_count);
                }
            }
            
            i_new ++;
        } else if (new_component > old_component) {
            if (size) {
                ecs_column_t *column = &old_columns[i_old];
                
                /* Destruct old values */
                ecs_type_info_t *c_info = old_table->c_info[i_old];
                if (c_info) {
                    dtor_component(world, c_info, column, 
                        entities, 0, old_count);
                }

                /* Old column does not occur in new table, remove */
                ecs_vector_free(column->data);
                column->data = NULL;
            }

            i_old ++;
        }
    }

    move_switch_columns(
        new_table, new_data, new_count, old_table, old_data, 0, old_count);

    /* Initialize remaining columns */
    for (; i_new < new_component_count; i_new ++) {
        ecs_column_t *column = &new_columns[i_new];
        int16_t size = column->size;
        int16_t alignment = column->alignment;

        if (size) {
            ecs_vector_set_count_t(&column->data, size, alignment,
                old_count + new_count);

            /* Construct new values */
            ecs_type_info_t *c_info = new_table->c_info[i_new];
            if (c_info) {
                ctor_component(world, c_info, column, 
                    entities, 0, old_count + new_count);
            }
        }
    }

    /* Destroy remaining columns */
    for (; i_old < old_component_count; i_old ++) {
        ecs_column_t *column = &old_columns[i_old];

        /* Destruct old values */
        ecs_type_info_t *c_info = old_table->c_info[i_old];
        if (c_info) {
            dtor_component(world, c_info, column, entities, 
                0, old_count);
        }

        /* Old column does not occur in new table, remove */
        ecs_vector_free(column->data);
        column->data = NULL;
    }    

    /* Mark entity column as dirty */
    mark_table_dirty(new_table, 0); 
}

int32_t ecs_table_count(
    const ecs_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_data_t *data = table->data;
    if (!data) {
        return 0;
    }

    return flecs_table_data_count(data);
}

ecs_data_t* flecs_table_merge(
    ecs_world_t *world,
    ecs_table_t *new_table,
    ecs_table_t *old_table,
    ecs_data_t *new_data,
    ecs_data_t *old_data)
{
    ecs_assert(old_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!old_table->lock, ECS_LOCKED_STORAGE, NULL);
    
    bool move_data = false;
    
    /* If there is nothing to merge to, just clear the old table */
    if (!new_table) {
        flecs_table_clear_data(world, old_table, old_data);
        return NULL;
    } else {
        ecs_assert(!new_table->lock, ECS_LOCKED_STORAGE, NULL);
    }

    /* If there is no data to merge, drop out */
    if (!old_data) {
        return NULL;
    }

    if (!new_data) {
        new_data = flecs_table_get_or_create_data(new_table);
        if (new_table == old_table) {
            move_data = true;
        }
    }

    ecs_entity_t *old_entities = ecs_vector_first(old_data->entities, ecs_entity_t);
    int32_t old_count = ecs_vector_count(old_data->entities);
    int32_t new_count = ecs_vector_count(new_data->entities);

    ecs_record_t **old_records = ecs_vector_first(
        old_data->record_ptrs, ecs_record_t*);

    /* First, update entity index so old entities point to new type */
    int32_t i;
    for(i = 0; i < old_count; i ++) {
        ecs_record_t *record;
        if (new_table != old_table) {
            record = old_records[i];
            ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);
        } else {
            record = ecs_eis_ensure(world, old_entities[i]);
        }

        bool is_monitored = record->row < 0;
        record->row = flecs_row_to_record(new_count + i, is_monitored);
        record->table = new_table;
    }

    /* Merge table columns */
    if (move_data) {
        *new_data = *old_data;
    } else {
        merge_table_data(world, new_table, old_table, old_count, new_count, 
            old_data, new_data);
    }

    new_table->alloc_count ++;

    if (!new_count && old_count) {
        table_activate(world, new_table, NULL, true);
    }

    return new_data;
}

void flecs_table_replace_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data)
{
    int32_t prev_count = 0;
    ecs_data_t *table_data = table->data;
    ecs_assert(!data || data != table_data, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);

    if (table_data) {
        prev_count = ecs_vector_count(table_data->entities);
        run_on_remove(world, table, table_data);
        flecs_table_clear_data(world, table, table_data);
    }

    if (data) {
        table_data = flecs_table_get_or_create_data(table);
        *table_data = *data;
    } else {
        return;
    }

    int32_t count = ecs_table_count(table);

    if (!prev_count && count) {
        table_activate(world, table, 0, true);
    } else if (prev_count && !count) {
        table_activate(world, table, 0, false);
    }
}

bool flecs_table_match_filter(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_filter_t * filter)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!filter) {
        return true;
    }

    ecs_type_t type = table->type;
    
    if (filter->include) {
        /* If filter kind is exact, types must be the same */
        if (filter->include_kind == EcsMatchExact) {
            if (type != filter->include) {
                return false;
            }

        /* Default for include_kind is MatchAll */
        } else if (!flecs_type_contains(world, type, filter->include, 
            filter->include_kind != EcsMatchAny, true)) 
        {
            return false;
        }
    }

    if (filter->exclude) {
        /* If filter kind is exact, types must be the same */
        if (filter->exclude_kind == EcsMatchExact) {
            if (type == filter->exclude) {
                return false;
            }
        
        /* Default for exclude_kind is MatchAny */                
        } else if (flecs_type_contains(world, type, filter->exclude, 
            filter->exclude_kind == EcsMatchAll, true))
        {
            return false;
        }
    }

    return true;
}

int32_t* flecs_table_get_dirty_state(
    ecs_table_t *table)
{
    ecs_assert(!table->lock, ECS_LOCKED_STORAGE, NULL);
    
    if (!table->dirty_state) {
        table->dirty_state = ecs_os_calloc(ECS_SIZEOF(int32_t) * (table->column_count + 1));
        ecs_assert(table->dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);
    }
    return table->dirty_state;
}

int32_t* flecs_table_get_monitor(
    ecs_table_t *table)
{
    int32_t *dirty_state = flecs_table_get_dirty_state(table);
    ecs_assert(dirty_state != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t column_count = table->column_count;
    return ecs_os_memdup(dirty_state, (column_count + 1) * ECS_SIZEOF(int32_t));
}

void flecs_table_notify(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_table_event_t * event)
{
    if (world->is_fini) {
        return;
    }

    switch(event->kind) {
    case EcsTableQueryMatch:
        register_query(
            world, table, event->query, event->matched_table_index);
        break;
    case EcsTableQueryUnmatch:
        unregister_query(
            world, table, event->query);
        break;
    case EcsTableComponentInfo:
        notify_component_info(world, table, event->component);
        break;
    case EcsTableTriggerMatch:
        notify_trigger(world, table, event->event);
        break;
    }
}

void ecs_table_lock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (world->magic == ECS_WORLD_MAGIC && !world->is_readonly) {
        table->lock ++;
    }
}

void ecs_table_unlock(
    ecs_world_t *world,
    ecs_table_t *table)
{
    if (world->magic == ECS_WORLD_MAGIC && !world->is_readonly) {
        table->lock --;
        ecs_assert(table->lock >= 0, ECS_INVALID_OPERATION, NULL);
    }
}

bool ecs_table_has_module(
    ecs_table_t *table)
{
    return table->flags & EcsTableHasModule;
}

ecs_column_t *ecs_table_column_for_id(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
    if (tr) {
        ecs_data_t *data = table->data;
        if (data) {
            return &data->columns[tr->column];
        }
    }

    return NULL;
}


static
const ecs_entity_t* new_w_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_ids_t *component_ids,
    int32_t count,
    void **c_info,
    int32_t *row_out);

static
void* get_component_w_index(
    ecs_table_t *table,
    int32_t column_index,
    int32_t row)
{
    ecs_assert(column_index < table->column_count, ECS_NOT_A_COMPONENT, NULL);

    ecs_data_t *data = flecs_table_get_data(table);
    ecs_column_t *column = &data->columns[column_index];

    /* If size is 0, component does not have a value. This is likely caused by
    * an application trying to call ecs_get with a tag. */
    int32_t size = column->size;    
    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);

    void *ptr = ecs_vector_first_t(column->data, size, column->alignment); 
    return ECS_OFFSET(ptr, size * row);  
}

static
void* get_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    int32_t row,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    ecs_table_record_t *tr = ecs_map_get(idr->table_index, 
        ecs_table_record_t, table->id);
    if (!tr) {
       return NULL;
    }

    return get_component_w_index(table, tr->column, row);
}

static
void* get_base_component(
    const ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    ecs_map_t *table_index,
    ecs_map_t *table_index_isa,
    int32_t recur_depth)
{
    /* Cycle detected in IsA relation */
    ecs_assert(recur_depth < ECS_MAX_RECURSION, ECS_INVALID_PARAMETER, NULL);

    /* Table (and thus entity) does not have component, look for base */
    if (!(table->flags & EcsTableHasIsA)) {
        return NULL;
    }

    /* Exclude Name */
    if (id == ecs_pair(ecs_id(EcsIdentifier), EcsName)) {
        return NULL;
    }

    /* Should always be an id record for IsA, otherwise a table with a 
     * HasBase flag set should not exist. */
    if (!table_index_isa) {
        ecs_id_record_t *idr = flecs_get_id_record(world, ecs_pair(EcsIsA, EcsWildcard));
        ecs_assert(idr != NULL, ECS_INTERNAL_ERROR, NULL);
        table_index_isa = idr->table_index;
    }

    /* Table should always be in the table index for (IsA, *), otherwise the
     * HasBase flag should not have been set */
    ecs_table_record_t *tr_isa = ecs_map_get(
        table_index_isa, ecs_table_record_t, table->id);
    ecs_assert(tr_isa != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t type = table->type;
    ecs_id_t *ids = ecs_vector_first(type, ecs_id_t);
    int32_t i = tr_isa->column, count = tr_isa->count;
    void *ptr = NULL;

    do {
        ecs_id_t pair = ids[i];

        ecs_entity_t base = ecs_pair_object(world, pair);
        ecs_record_t *r = ecs_eis_get(world, base);
        if (!r) {
            continue;
        }

        table = r->table;
        if (!table) {
            continue;
        }

        ecs_table_record_t *tr = ecs_map_get(table_index, 
            ecs_table_record_t, table->id);
        if (!tr) {
            ptr = get_base_component(world, table, id, table_index, 
                table_index_isa, recur_depth + 1);
        } else {
            bool is_monitored;
            int32_t row = flecs_record_to_row(r->row, &is_monitored);
            ptr = get_component_w_index(table, tr->column, row);
        }
        i ++;
    } while (!ptr && (i < count));

    return ptr;
}

/* Utility to compute actual row from row in record */
static
int32_t set_row_info(
    ecs_entity_info_t *info,
    int32_t row)
{
    return info->row = flecs_record_to_row(row, &info->is_watched);
}

/* Utility to set info from main stage record */
static
void set_info_from_record(
    ecs_entity_info_t * info,
    ecs_record_t * record)
{
    ecs_assert(record != NULL, ECS_INTERNAL_ERROR, NULL);

    info->record = record;

    ecs_table_t *table = record->table;

    set_row_info(info, record->row);

    info->table = table;
    if (!info->table) {
        return;
    }

    ecs_data_t *data = flecs_table_get_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    info->data = data;

    ecs_assert(ecs_vector_count(data->entities) > info->row, 
        ECS_INTERNAL_ERROR, NULL);
}

static
const ecs_type_info_t *get_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_entity_t real_id = ecs_get_typeid(world, component);
    if (real_id) {
        return flecs_get_c_info(world, real_id);
    } else {
        return NULL;
    }
}

static
int get_column_info(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_ids_t * components,
    ecs_column_info_t * cinfo,
    bool get_all)
{
    int32_t column_count = table->column_count;
    ecs_entity_t *type_array = ecs_vector_first(table->type, ecs_entity_t);

    if (get_all) {
        int32_t i, count = ecs_vector_count(table->type);
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = type_array[i];
            cinfo[i].id = id;
            cinfo[i].ci = get_c_info(world, id);
            cinfo[i].column = i;            
        }

        return count;
    } else {
        ecs_entity_t *array = components->array;
        int32_t i, cur, count = components->count;
        for (i = 0; i < count; i ++) {
            ecs_entity_t id = array[i];
            cinfo[i].id = id;
            cinfo[i].ci = get_c_info(world, id);
            cinfo[i].column = -1;

            for (cur = 0; cur < column_count; cur ++) {
                if (type_array[cur] == id) {
                    cinfo[i].column = cur;
                    break;
                }
            }
        }

        return count;
    }
}

#ifdef FLECS_SYSTEM 
static
void run_set_systems_for_entities(
    ecs_world_t * world,
    ecs_ids_t * components,
    ecs_table_t * table,
    int32_t row,
    int32_t count,
    ecs_entity_t * entities,
    bool set_all)
{
    if (set_all) {
        /* Run OnSet systems for all components of the entity. This usually
         * happens when an entity is created directly in its target table. */
        ecs_vector_t *queries = table->on_set_all;
        ecs_vector_each(queries, ecs_matched_query_t, m, {
            flecs_run_monitor(world, m, components, row, count, entities);
        });
    } else {
        /* Run OnSet systems for a specific component. This usually happens when
         * an application calls ecs_set or ecs_modified. The entity's table
         * stores a vector for each component with the OnSet systems for that
         * component. This vector maintains the same order as the table's type,
         * which makes finding the correct set of systems as simple as getting
         * the index of a component id in the table type. 
         *
         * One thing to note is that the system may be invoked for a table that
         * is not the same as the entity for which the system is invoked. This
         * can happen in the case of instancing, where adding an IsA
         * relationship conceptually adds components to an entity, but the 
         * actual components are stored on the base entity. */
        ecs_vector_t **on_set_systems = table->on_set;
        if (on_set_systems) {
            int32_t index = ecs_type_index_of(table->type, 0, components->array[0]);
            
            /* This should never happen, as an OnSet system should only ever be
             * invoked for entities that have the component for which this
             * function was invoked. */
            ecs_assert(index != -1, ECS_INTERNAL_ERROR, NULL);

            ecs_vector_t *queries = on_set_systems[index];
            ecs_vector_each(queries, ecs_matched_query_t, m, {
                flecs_run_monitor(world, m, components, row, count, entities);
            });
        }
    }
}
#endif

static
void notify(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    ecs_entity_t event,
    ecs_ids_t *ids)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_id_t *arr = ids->array;
    int32_t arr_count = ids->count;

    int i;
    for (i = 0; i < arr_count; i ++) {
        flecs_triggers_notify(world, arr[i], event, table, data, row, count);
    }
}

static
void instantiate(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count);

static
void instantiate_children(
    ecs_world_t * world,
    ecs_entity_t base,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    ecs_table_t * child_table)
{
    ecs_type_t type = child_table->type;
    ecs_data_t *child_data = flecs_table_get_data(child_table);
    if (!child_data || !flecs_table_data_count(child_data)) {
        return;
    }

    int32_t column_count = child_table->column_count;
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t type_count = ecs_vector_count(type);   

    /* Instantiate child table for each instance */

    /* Create component array for creating the table */
    ecs_ids_t components = {
        .array = ecs_os_alloca_n(ecs_entity_t, type_count + 1)
    };

    void **c_info = ecs_os_alloca_n(void*, column_count);

    /* Copy in component identifiers. Find the base index in the component
     * array, since we'll need this to replace the base with the instance id */
    int i, base_index = -1, pos = 0;

    for (i = 0; i < type_count; i ++) {
        ecs_entity_t c = type_array[i];

        /* Make sure instances don't have EcsPrefab */
        if (c == EcsPrefab) {
            continue;
        }

        /* Keep track of the element that creates the ChildOf relationship with
         * the prefab parent. We need to replace this element to make sure the
         * created children point to the instance and not the prefab */ 
        if (ECS_HAS_RELATION(c, EcsChildOf) && (ecs_entity_t_lo(c) == base)) {
            base_index = pos;
        }        

        /* Store pointer to component array. We'll use this component array to
        * create our new entities in bulk with new_w_data */
        if (i < column_count) {
            ecs_column_t *column = &child_data->columns[i];
            c_info[pos] = ecs_vector_first_t(
                column->data, column->size, column->alignment);
        } else if (pos < column_count) {
            c_info[pos] = NULL;
        }

        components.array[pos] = c;
        pos ++;
    }

    ecs_assert(base_index != -1, ECS_INTERNAL_ERROR, NULL);

    /* If children are added to a prefab, make sure they are prefabs too */
    if (table->flags & EcsTableIsPrefab) {
        components.array[pos] = EcsPrefab;
        pos ++;
    }

    components.count = pos;

    /* Instantiate the prefab child table for each new instance */
    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
    int32_t child_count = ecs_vector_count(child_data->entities);

    for (i = row; i < count + row; i ++) {
        ecs_entity_t instance = entities[i];

        /* Replace ChildOf element in the component array with instance id */
        components.array[base_index] = ecs_pair(EcsChildOf, instance);

        /* Find or create table */
        ecs_table_t *i_table = flecs_table_find_or_create(world, &components);
        ecs_assert(i_table != NULL, ECS_INTERNAL_ERROR, NULL); 

        /* The instance is trying to instantiate from a base that is also
         * its parent. This would cause the hierarchy to instantiate itself
         * which would cause infinite recursion. */
        int j;
        ecs_entity_t *children = ecs_vector_first(
            child_data->entities, ecs_entity_t);
#ifndef NDEBUG
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];        
            ecs_assert(child != instance, ECS_INVALID_PARAMETER, NULL);
        }
#endif

        /* Create children */
        int32_t child_row; 
        new_w_data(world, i_table, NULL, child_count, c_info, &child_row);       

        /* If prefab child table has children itself, recursively instantiate */
        ecs_data_t *i_data = flecs_table_get_data(i_table);
        for (j = 0; j < child_count; j ++) {
            ecs_entity_t child = children[j];
            instantiate(world, child, i_table, i_data, child_row + j, 1);
        }
    }       
}

static
void instantiate(
    ecs_world_t * world,
    ecs_entity_t base,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count)
{    
    /* If base is a parent, instantiate children of base for instances */
    const ecs_id_record_t *r = flecs_get_id_record(
        world, ecs_pair(EcsChildOf, base));

    if (r && r->table_index) {
        ecs_table_record_t *tr;
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            instantiate_children(
                world, base, table, data, row, count, tr->table);
        }
    }
}

static
bool override_component(
    ecs_world_t *world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count);

static
bool override_from_base(
    ecs_world_t *world,
    ecs_entity_t base,
    ecs_entity_t component,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count)
{
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_info_t base_info;
    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);

    if (!flecs_get_info(world, base, &base_info) || !base_info.table) {
        return false;
    }

    void *base_ptr = get_component(
        world, base_info.table, base_info.row, component);
    if (base_ptr) {
        int16_t data_size = column->size;
        void *data_array = ecs_vector_first_t(
            column->data, column->size, column->alignment);
        void *data_ptr = ECS_OFFSET(data_array, data_size * row);

        component = ecs_get_typeid(world, component);
        const ecs_type_info_t *cdata = flecs_get_c_info(world, component);
        int32_t index;

        ecs_copy_t copy = cdata ? cdata->lifecycle.copy : NULL;
        if (copy) {
            ecs_entity_t *entities = ecs_vector_first(
                data->entities, ecs_entity_t);

            void *ctx = cdata->lifecycle.ctx;
            for (index = 0; index < count; index ++) {
                copy(world, component, &entities[row], &base,
                    data_ptr, base_ptr, flecs_to_size_t(data_size), 1, ctx);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }
        } else {
            for (index = 0; index < count; index ++) {
                ecs_os_memcpy(data_ptr, base_ptr, data_size);
                data_ptr = ECS_OFFSET(data_ptr, data_size);
            }                    
        }

        return true;
    } else {
        /* If component not found on base, check if base itself inherits */
        ecs_type_t base_type = base_info.table->type;
        return override_component(world, component, base_type, data, column, 
            row, count);
    }
}

static
bool override_component(
    ecs_world_t * world,
    ecs_entity_t component,
    ecs_type_t type,
    ecs_data_t * data,
    ecs_column_t * column,
    int32_t row,
    int32_t count)
{
    ecs_entity_t *type_array = ecs_vector_first(type, ecs_entity_t);
    int32_t i, type_count = ecs_vector_count(type);

    /* Walk prefabs */
    i = type_count - 1;
    do {
        ecs_entity_t e = type_array[i];

        if (!(e & ECS_ROLE_MASK)) {
            break;
        }

        if (ECS_HAS_RELATION(e, EcsIsA)) {
            if (override_from_base(world, ecs_pair_object(world, e), component,
                data, column, row, count))
            {
                return true;
            }
        }
    } while (--i >= 0);

    return false;
}

static
void components_override(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    ecs_column_info_t * component_info,
    int32_t component_count,
    bool run_on_set)
{
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(component_info != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table_without_base = table;
    ecs_column_t *columns = data->columns;
    ecs_type_t type = table->type;
    int32_t column_count = table->column_count;

    int i;
    for (i = 0; i < component_count; i ++) {
        ecs_entity_t component = component_info[i].id;

        if (component >= ECS_HI_COMPONENT_ID) {
            if (ECS_HAS_RELATION(component, EcsIsA)) {
                ecs_entity_t base = ECS_PAIR_OBJECT(component);

                /* Illegal to create an instance of 0 */
                ecs_assert(base != 0, ECS_INVALID_PARAMETER, NULL);
                instantiate(world, base, table, data, row, count);

                /* If table has on_set systems, get table without the base
                 * entity that was just added. This is needed to determine the
                 * diff between the on_set systems of the current table and the
                 * table without the base, as these are the systems that need to
                 * be invoked */
                ecs_ids_t to_remove = {
                    .array = &component,
                    .count = 1
                };

                table_without_base = flecs_table_traverse_remove(world, 
                    table_without_base, &to_remove, NULL);
            }
        }

        int32_t column_index = component_info[i].column;
        if (column_index == -1 || column_index >= column_count) {
            continue;
        }

        /* column_index is lower than column count, which means we must have
         * data columns */
        ecs_assert(data->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!data->columns[column_index].size) {
            continue;
        }

        ecs_column_t *column = &columns[column_index];
        if (override_component(world, component, type, data, column, 
            row, count)) 
        {
            ecs_ids_t to_remove = {
                .array = &component,
                .count = 1
            };
            table_without_base = flecs_table_traverse_remove(world, 
                table_without_base, &to_remove, NULL);
        }
    }

    /* Run OnSet actions when a base entity is added to the entity for 
     * components not overridden by the entity. */
    if (run_on_set && table_without_base != table) {
        flecs_run_monitors(world, table, table->on_set_all, row, count, 
            table_without_base->on_set_all);
    }
}

static
void set_switch(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,    
    ecs_ids_t *entities,
    bool reset)
{
    ecs_entity_t *array = entities->array;
    int32_t i, comp_count = entities->count;

    for (i = 0; i < comp_count; i ++) {
        ecs_entity_t e = array[i];

        if (ECS_HAS_ROLE(e, CASE)) {
            e = e & ECS_COMPONENT_MASK;

            ecs_entity_t sw_case = 0;
            if (!reset) {
                sw_case = e;
                ecs_assert(sw_case != 0, ECS_INTERNAL_ERROR, NULL);
            }

            int32_t sw_index = flecs_table_switch_from_case(world, table, e);
            ecs_assert(sw_index != -1, ECS_INTERNAL_ERROR, NULL);
            ecs_switch_t *sw = data->sw_columns[sw_index].data;
            ecs_assert(sw != NULL, ECS_INTERNAL_ERROR, NULL);
            
            int32_t r;
            for (r = 0; r < count; r ++) {
                flecs_switch_set(sw, row + r, sw_case);
            }
        }
    }
}

static
void ecs_components_switch(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count,
    ecs_ids_t *added,
    ecs_ids_t *removed)
{
    if (added) {
        set_switch(world, table, data, row, count, added, false);
    }
    if (removed) {
        set_switch(world, table, data, row, count, removed, true);
    } 
}

static
int32_t new_entity(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_info_t * info,
    ecs_table_t * new_table,
    ecs_ids_t * added,
    bool construct)
{
    ecs_record_t *record = info->record;
    ecs_data_t *new_data = flecs_table_get_or_create_data(new_table);
    int32_t new_row;

    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!record) {
        record = ecs_eis_ensure(world, entity);
    }

    new_row = flecs_table_append(
        world, new_table, new_data, entity, record, construct);

    record->table = new_table;
    record->row = flecs_row_to_record(new_row, info->is_watched);

    ecs_assert(
        ecs_vector_count(new_data[0].entities) > new_row, 
        ECS_INTERNAL_ERROR, NULL);

    if (new_table->flags & EcsTableHasAddActions) {
        flecs_run_add_actions(
            world, new_table, new_data, new_row, 1, added, true, true);

        if (new_table->flags & EcsTableHasMonitors) {
            flecs_run_monitors(
                world, new_table, new_table->monitors, new_row, 1, NULL);              
        }        
    }

    info->data = new_data;
    
    return new_row;
}

static
int32_t move_entity(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_info_t * info,
    ecs_table_t * src_table,
    ecs_data_t * src_data,
    int32_t src_row,
    ecs_table_t * dst_table,
    ecs_ids_t * added,
    ecs_ids_t * removed,
    bool construct)
{    
    ecs_data_t *dst_data = flecs_table_get_or_create_data(dst_table);
    ecs_assert(src_data != dst_data, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_is_alive(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src_table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src_row >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = info->record;
    ecs_assert(!record || record == ecs_eis_get(world, entity), 
        ECS_INTERNAL_ERROR, NULL);

    int32_t dst_row = flecs_table_append(world, dst_table, dst_data, entity, 
        record, false);

    ecs_assert(ecs_vector_count(src_data->entities) > src_row, 
        ECS_INTERNAL_ERROR, NULL);

    /* Copy entity & components from src_table to dst_table */
    if (src_table->type) {
        /* If components were removed, invoke remove actions before deleting */
        if (removed && (src_table->flags & EcsTableHasRemoveActions)) {
            /* If entity was moved, invoke UnSet monitors for each component that
             * the entity no longer has */
            flecs_run_monitors(world, dst_table, src_table->un_set_all, 
                src_row, 1, dst_table->un_set_all);

            flecs_run_remove_actions(
                world, src_table, src_data, src_row, 1, removed);
        }

        flecs_table_move(world, entity, entity, dst_table, dst_data, dst_row, 
            src_table, src_data, src_row, construct);                
    }

    /* Update entity index & delete old data after running remove actions */
    record->table = dst_table;
    record->row = flecs_row_to_record(dst_row, info->is_watched);

    flecs_table_delete(world, src_table, src_data, src_row, false);

    /* If components were added, invoke add actions */
    if (src_table != dst_table || (added && added->count)) {
        if (added && (dst_table->flags & EcsTableHasAddActions)) {
            flecs_run_add_actions(
                world, dst_table, dst_data, dst_row, 1, added, false, true);
        }

        /* Run monitors */
        if (dst_table->flags & EcsTableHasMonitors) {
            flecs_run_monitors(world, dst_table, dst_table->monitors, dst_row, 
                1, src_table->monitors);
        }

        /* If removed components were overrides, run OnSet systems for those, as 
         * the value of those components changed from the removed component to 
         * the value of component on the base entity */
        if (removed && dst_table->flags & EcsTableHasIsA) {
            flecs_run_monitors(world, dst_table, src_table->on_set_override, 
                dst_row, 1, dst_table->on_set_override);          
        }
    }

    info->data = dst_data;

    return dst_row;
}

static
void delete_entity(
    ecs_world_t * world,
    ecs_table_t * src_table,
    ecs_data_t * src_data,
    int32_t src_row,
    ecs_ids_t * removed)
{
    if (removed) {
        flecs_run_monitors(world, src_table, src_table->un_set_all, 
            src_row, 1, NULL);

        /* Invoke remove actions before deleting */
        if (src_table->flags & EcsTableHasRemoveActions) {   
            flecs_run_remove_actions(
                world, src_table, src_data, src_row, 1, removed);
        } 
    }

    flecs_table_delete(world, src_table, src_data, src_row, true);
}

/* Updating component monitors is a relatively expensive operation that only
 * happens for entities that are monitored. The approach balances the amount of
 * processing between the operation on the entity vs the amount of work that
 * needs to be done to rematch queries, as a simple brute force approach does
 * not scale when there are many tables / queries. Therefore we need to do a bit
 * of bookkeeping that is more intelligent than simply flipping a flag */
static
void update_component_monitor_w_array(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t relation,
    ecs_ids_t * entities)
{
    if (!entities) {
        return;
    }

    int i;
    for (i = 0; i < entities->count; i ++) {
        ecs_entity_t id = entities->array[i];
        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_entity_t rel = ECS_PAIR_RELATION(id);
            
            /* If a relationship has changed, check if it could have impacted
             * the shape of the graph for that relationship. If so, mark the
             * relationship as dirty */        
            if (rel != relation && flecs_get_id_record(world, ecs_pair(rel, entity))) {
                update_component_monitor_w_array(world, entity, rel, entities);
            }

        }
        
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            /* If an IsA relationship is added to a monitored entity (can
             * be either a parent or a base) component monitors need to be
             * evaluated for the components of the prefab. */
            ecs_entity_t base = ecs_pair_object(world, id);
            ecs_type_t type = ecs_get_type(world, base);
            ecs_ids_t base_entities = flecs_type_to_ids(type);

            /* This evaluates the component monitor for all components of the
             * base entity. If the base entity contains IsA relationships
             * these will be evaluated recursively as well. */
            update_component_monitor_w_array(
                world, entity, relation, &base_entities);               
        } else {
            flecs_monitor_mark_dirty(world, relation, id);
        }
    }
}

static
void update_component_monitors(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_ids_t * added,
    ecs_ids_t * removed)
{
    update_component_monitor_w_array(world, entity, 0, added);
    update_component_monitor_w_array(world, entity, 0, removed);
}

static
void commit(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_info_t * info,
    ecs_table_t * dst_table,   
    ecs_ids_t * added,
    ecs_ids_t * removed,
    bool construct)
{
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);
    
    ecs_table_t *src_table = info->table;
    if (src_table == dst_table) {
        /* If source and destination table are the same no action is needed *
         * However, if a component was added in the process of traversing a
         * table, this suggests that a case switch could have occured. */
        if (((added && added->count) || (removed && removed->count)) && 
             src_table && src_table->flags & EcsTableHasSwitch) 
        {
            ecs_components_switch(
                world, src_table, info->data, info->row, 1, added, removed);
        }

        return;
    }

    if (src_table) {
        ecs_data_t *src_data = info->data;
        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

        if (dst_table->type) { 
            info->row = move_entity(world, entity, info, src_table, 
                src_data, info->row, dst_table, added, removed, construct);
            info->table = dst_table;
        } else {
            delete_entity(world, src_table, src_data, info->row, removed);

            ecs_eis_set(world, entity, &(ecs_record_t){
                NULL, (info->is_watched == true) * -1
            });
        }      
    } else {        
        if (dst_table->type) {
            info->row = new_entity(
                world, entity, info, dst_table, added, construct);
            info->table = dst_table;
        }        
    }

    /* If the entity is being watched, it is being monitored for changes and
     * requires rematching systems when components are added or removed. This
     * ensures that systems that rely on components from containers or prefabs
     * update the matched tables when the application adds or removes a 
     * component from, for example, a container. */
    if (info->is_watched) {
        update_component_monitors(world, entity, added, removed);
    }

    if ((!src_table || !src_table->type) && world->range_check_enabled) {
        ecs_assert(!world->stats.max_id || entity <= world->stats.max_id, ECS_OUT_OF_RANGE, 0);
        ecs_assert(entity >= world->stats.min_id, ECS_OUT_OF_RANGE, 0);
    } 
}

static
void new(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_ids_t * to_add)
{
    ecs_entity_info_t info = {0};
    ecs_table_t *table = flecs_table_traverse_add(
        world, &world->store.root, to_add, NULL);
    new_entity(world, entity, &info, table, to_add, true);
}

static
const ecs_entity_t* new_w_data(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_ids_t * component_ids,
    int32_t count,
    void ** component_data,
    int32_t * row_out)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);
    
    int32_t sparse_count = ecs_eis_count(world);
    const ecs_entity_t *ids = flecs_sparse_new_ids(world->store.entity_index, count);
    ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_type_t type = table->type;   

    if (!type) {
        return ids;        
    }

    ecs_ids_t component_array = { 0 };
    if (!component_ids) {
        component_ids = &component_array;
        component_array.array = ecs_vector_first(type, ecs_entity_t);
        component_array.count = ecs_vector_count(type);
    }

    ecs_data_t *data = flecs_table_get_or_create_data(table);
    int32_t row = flecs_table_appendn(world, table, data, count, ids);
    ecs_ids_t added = flecs_type_to_ids(type);
    
    /* Update entity index. */
    int i;
    ecs_record_t **record_ptrs = ecs_vector_first(data->record_ptrs, ecs_record_t*);
    for (i = 0; i < count; i ++) { 
        record_ptrs[row + i] = ecs_eis_set(world, ids[i], 
        &(ecs_record_t){
            .table = table,
            .row = row + i + 1
        });
    }

    flecs_defer_none(world, &world->stage);

    flecs_run_add_actions(world, table, data, row, count, &added, 
        true, component_data == NULL);

    if (component_data) {
        /* Set components that we're setting in the component mask so the init
         * actions won't call OnSet triggers for them. This ensures we won't
         * call OnSet triggers multiple times for the same component */
        int32_t c_i;
        for (c_i = 0; c_i < component_ids->count; c_i ++) {
            ecs_entity_t c = component_ids->array[c_i];
            
            /* Bulk copy column data into new table */
            int32_t table_index = ecs_type_index_of(type, 0, c);
            ecs_assert(table_index >= 0, ECS_INTERNAL_ERROR, NULL);
            if (table_index >= table->column_count) {
                continue;
            }

            ecs_column_t *column = &data->columns[table_index];
            int16_t size = column->size;
            if (!size) {
                continue;
            }

            int16_t alignment = column->alignment;
            void *ptr = ecs_vector_first_t(column->data, size, alignment);
            ptr = ECS_OFFSET(ptr, size * row);

            /* Copy component data */
            void *src_ptr = component_data[c_i];
            if (!src_ptr) {
                continue;
            }

            const ecs_type_info_t *cdata = get_c_info(world, c);
            ecs_copy_t copy;
            if (cdata && (copy = cdata->lifecycle.copy)) {
                ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
                copy(world, c, entities, entities, ptr, src_ptr, 
                    flecs_to_size_t(size), count, cdata->lifecycle.ctx);
            } else {
                ecs_os_memcpy(ptr, src_ptr, size * count);
            } 
        };

        flecs_run_set_systems(world, 0, table, data, NULL, row, count, true);            
    }

    flecs_run_monitors(world, table, table->monitors, row, count, NULL);

    flecs_defer_flush(world, &world->stage);

    if (row_out) {
        *row_out = row;
    }

    ids = flecs_sparse_ids(world->store.entity_index);

    return &ids[sparse_count];
}

static
bool has_type(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type,
    bool match_any,
    bool match_prefabs)    
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return false;
    }

    if (!type) {
        return true;
    }

    ecs_type_t entity_type = ecs_get_type(world, entity);

    return flecs_type_contains(
        world, entity_type, type, match_any, match_prefabs) != 0;
}

static
void add_remove(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_ids_t * to_add,
    ecs_ids_t * to_remove)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(to_add->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_entity_t add_buffer[ECS_MAX_ADD_REMOVE];
    ecs_entity_t remove_buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t added = { .array = add_buffer };
    ecs_ids_t removed = { .array = remove_buffer };

    ecs_table_t *src_table = info.table;

    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, to_remove, &removed);

    dst_table = flecs_table_traverse_add(
        world, dst_table, to_add, &added);    

    commit(world, entity, &info, dst_table, &added, &removed, true);
}

static
void add_ids_w_info(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_info_t * info,
    ecs_ids_t * components,
    bool construct)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t added = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, components, &added);

    commit(world, entity, info, dst_table, &added, NULL, construct);
}

static
void remove_ids_w_info(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_info_t * info,
    ecs_ids_t * components)
{
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t removed = { .array = buffer };

    ecs_table_t *src_table = info->table;
    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, components, &removed);

    commit(world, entity, info, dst_table, NULL, &removed, true);
}

static
void add_ids(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_ids_t * components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_add(world, stage, entity, components)) {
        return;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t added = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = flecs_table_traverse_add(
        world, src_table, components, &added);

    commit(world, entity, &info, dst_table, &added, NULL, true);

    flecs_defer_flush(world, stage);
}

static
void remove_ids(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_ids_t * components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(components->count < ECS_MAX_ADD_REMOVE, ECS_INVALID_PARAMETER, NULL);    
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_remove(world, stage, entity, components)) {
        return;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_entity_t buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t removed = { .array = buffer };

    ecs_table_t *src_table = info.table;
    ecs_table_t *dst_table = flecs_table_traverse_remove(
        world, src_table, components, &removed);

    commit(world, entity, &info, dst_table, NULL, &removed, true);

    flecs_defer_flush(world, stage);
}

static
void *get_mutable(
    ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_entity_info_t * info,
    bool * is_added)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((component & ECS_COMPONENT_MASK) == component || 
        ECS_HAS_ROLE(component, PAIR), ECS_INVALID_PARAMETER, NULL);

    void *dst = NULL;
    if (flecs_get_info(world, entity, info) && info->table) {
        dst = get_component(world, info->table, info->row, component);
    }

    if (!dst) {
        ecs_table_t *table = info->table;

        ecs_ids_t to_add = {
            .array = &component,
            .count = 1
        };

        add_ids_w_info(world, entity, info, &to_add, true);

        flecs_get_info(world, entity, info);
        ecs_assert(info->table != NULL, ECS_INTERNAL_ERROR, NULL);

        dst = get_component(world, info->table, info->row, component);

        if (is_added) {
            *is_added = table != info->table;
        }

        return dst;
    } else {
        if (is_added) {
            *is_added = false;
        }

        return dst;
    }
}


/* -- Private functions -- */

void flecs_run_add_actions(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    ecs_ids_t * added,
    bool get_all,
    bool run_on_set)
{
    ecs_assert(added != NULL, ECS_INTERNAL_ERROR, NULL);

    if (table->flags & EcsTableHasIsA) {
        ecs_column_info_t cinfo[ECS_MAX_ADD_REMOVE];

        int added_count = get_column_info(
            world, table, added, cinfo, get_all);

        components_override(
            world, table, data, row, count, cinfo, 
            added_count, run_on_set);
    }

    if (table->flags & EcsTableHasSwitch) {
        ecs_components_switch(world, table, data, row, count, added, NULL);
    }

    if (table->flags & EcsTableHasOnAdd) {
        notify(world, table, data, row, count, EcsOnAdd, added);
    }
}

void flecs_run_remove_actions(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_data_t * data,
    int32_t row,
    int32_t count,
    ecs_ids_t * removed)
{
    ecs_assert(removed != NULL, ECS_INTERNAL_ERROR, NULL);

    if (count) {
        if (table->flags & EcsTableHasUnSet) {
            notify(world, table, data, row, count, EcsUnSet, removed);
        } 
        if (table->flags & EcsTableHasOnRemove) {
            notify(world, table, data, row, count, EcsOnRemove, removed);
        }
    }
}

bool flecs_get_info(
    const ecs_world_t * world,
    ecs_entity_t entity,
    ecs_entity_info_t * info)
{
    info->table = NULL;
    info->record = NULL;
    info->data = NULL;
    info->is_watched = false;

    if (entity & ECS_ROLE) {
        return false;
    }
    
    ecs_record_t *record = ecs_eis_get(world, entity);

    if (!record) {
        return false;
    }

    set_info_from_record(info, record);

    return true;
}

void flecs_run_set_systems(
    ecs_world_t *world,
    ecs_id_t component,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_column_t *column,
    int32_t row,
    int32_t count,
    bool set_all)
{
    ecs_assert(set_all || column != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!column || column->size != 0, ECS_INTERNAL_ERROR, NULL);

    if (!count || !data || (column && !column->size)) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    ecs_ids_t components;

    if (!set_all) {
        const ecs_type_info_t *info = get_c_info(world, component);
        ecs_on_set_t on_set;
        if (info && (on_set = info->lifecycle.on_set)) {
            ecs_size_t size = column->size;
            void *ptr = ecs_vector_get_t(
                column->data, size, column->alignment, row);
            on_set(world, component, entities, ptr, flecs_to_size_t(size), count, 
                info->lifecycle.ctx);
        }

        components = (ecs_ids_t){
            .array = &component,
            .count = component != 0
        };
    } else {
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
        int32_t i, column_count = table->column_count;        

        for (i = 0; i < column_count; i ++) {
            ecs_id_t id = ids[i];
            const ecs_type_info_t *info = get_c_info(world, id);
            ecs_on_set_t on_set;
            if (info && (on_set = info->lifecycle.on_set)) {
                ecs_column_t *c = &data->columns[i];
                ecs_size_t size = c->size;
                if (!size) {
                    continue;
                }

                void *ptr = ecs_vector_get_t(c->data, size, c->alignment, row);
                on_set(world, ids[i], entities, ptr, flecs_to_size_t(size), 
                    count, info->lifecycle.ctx);
            }
        }

        components = (ecs_ids_t){
            .array = ids,
            .count = column_count
        };        
    }

#ifdef FLECS_SYSTEM
    run_set_systems_for_entities(world, &components, table, row, 
        count, entities, set_all);
#endif

    if (table->flags & EcsTableHasOnSet) {
        if (!set_all) {
            notify(world, table, data, row, count, EcsOnSet, &components);
        } else {
            int32_t i, column_count = table->column_count;
            for (i = 0; i < column_count; i ++) {
                ecs_column_t *c = &data->columns[i];
                if (c->size) {
                    notify(world, table, data, row, count, EcsOnSet, 
                        &components);
                }
            }
        }
    }
}

void flecs_run_monitors(
    ecs_world_t * world, 
    ecs_table_t * dst_table,
    ecs_vector_t * v_dst_monitors, 
    int32_t dst_row, 
    int32_t count, 
    ecs_vector_t *v_src_monitors)
{
    (void)world;
    (void)dst_table;
    (void)v_dst_monitors;
    (void)dst_row;
    (void)count;
    (void)v_src_monitors;

#ifdef FLECS_SYSTEM    
    if (v_dst_monitors == v_src_monitors) {
        return;
    }

    if (!v_dst_monitors) {
        return;
    }

    ecs_assert(!(dst_table->flags & EcsTableIsPrefab), ECS_INTERNAL_ERROR, NULL);
    
    if (!v_src_monitors) {
        ecs_vector_each(v_dst_monitors, ecs_matched_query_t, monitor, {
            flecs_run_monitor(world, monitor, NULL, dst_row, count, NULL);
        });
    } else {
        /* If both tables have monitors, run the ones that dst_table has and
         * src_table doesn't have */
        int32_t i, m_count = ecs_vector_count(v_dst_monitors);
        int32_t j = 0, src_count = ecs_vector_count(v_src_monitors);
        ecs_matched_query_t *dst_monitors = ecs_vector_first(v_dst_monitors, ecs_matched_query_t);
        ecs_matched_query_t *src_monitors = ecs_vector_first(v_src_monitors, ecs_matched_query_t);

        for (i = 0; i < m_count; i ++) {
            ecs_matched_query_t *dst = &dst_monitors[i];

            ecs_entity_t system = dst->query->system;
            ecs_assert(system != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_matched_query_t *src = 0;
            while (j < src_count) {
                src = &src_monitors[j];
                if (src->query->system < system) {
                    j ++;
                } else {
                    break;
                }
            }

            if (src && src->query->system == system) {
                continue;
            }

            flecs_run_monitor(world, dst, NULL, dst_row, count, NULL);
        }
    }
#endif
}

int32_t flecs_record_to_row(
    int32_t row, 
    bool *is_watched_out) 
{
    bool is_watched = row < 0;
    row = row * -(is_watched * 2 - 1) - 1 * (row != 0);
    *is_watched_out = is_watched;
    return row;
}

int32_t flecs_row_to_record(
    int32_t row, 
    bool is_watched) 
{
    return (row + 1) * -(is_watched * 2 - 1);
}

ecs_ids_t flecs_type_to_ids(
    ecs_type_t type)
{
    return (ecs_ids_t){
        .array = ecs_vector_first(type, ecs_entity_t),
        .count = ecs_vector_count(type)
    };
}

void flecs_set_watch(
    ecs_world_t *world,
    ecs_entity_t entity)
{    
    (void)world;

    ecs_record_t *record = ecs_eis_get(world, entity);
    if (!record) {
        ecs_record_t new_record = {.row = -1, .table = NULL};
        ecs_eis_set(world, entity, &new_record);
    } else {
        if (record->row > 0) {
            record->row *= -1;

        } else if (record->row == 0) {
            /* If entity is empty, there is no index to change the sign of. In
             * this case, set the index to -1, and assign an empty type. */
            record->row = -1;
            record->table = NULL;
        }
    }
}


/* -- Public functions -- */

bool ecs_commit(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_record_t *record,
    ecs_table_t *table,
    ecs_ids_t *added,
    ecs_ids_t *removed)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *src_table = NULL;
    if (!record) {
        record = ecs_eis_get(world, entity);
        src_table = record->table;
    }

    ecs_entity_info_t info = {0};
    if (record) {
        set_info_from_record(&info, record);
    }

    commit(world, entity, &info, table, added, removed, true);

    return src_table != table;
}

ecs_entity_t ecs_new_id(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);

    /* It is possible that the world passed to this function is a stage, so
     * make sure we have the actual world. Cast away const since this is one of
     * the few functions that may modify the world while it is in readonly mode,
     * since it is thread safe (uses atomic inc when in threading mode) */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);

    ecs_entity_t entity;

    int32_t stage_count = ecs_get_stage_count(unsafe_world);
    if (stage->asynchronous || (ecs_os_has_threading() && stage_count > 1)) {
        /* Can't atomically increase number above max int */
        ecs_assert(
            unsafe_world->stats.last_id < UINT_MAX, ECS_INTERNAL_ERROR, NULL);

        entity = (ecs_entity_t)ecs_os_ainc(
            (int32_t*)&unsafe_world->stats.last_id);
    } else {
        entity = ecs_eis_recycle(unsafe_world);
    }

    ecs_assert(!unsafe_world->stats.max_id || 
        ecs_entity_t_lo(entity) <= unsafe_world->stats.max_id, 
        ECS_OUT_OF_RANGE, NULL);

    return entity;
}

ecs_entity_t ecs_set_with(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_id_t prev = stage->with;
    stage->with = id;
    return prev;
}

ecs_entity_t ecs_get_with(
    const ecs_world_t *world)
{
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->with;
}

ecs_entity_t ecs_new_component_id(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* It is possible that the world passed to this function is a stage, so
     * make sure we have the actual world. Cast away const since this is one of
     * the few functions that may modify the world while it is in readonly mode,
     * but only if single threaded. */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);

    if (unsafe_world->is_readonly) {
        /* Can't issue new comp id while iterating when in multithreaded mode */
        ecs_assert(ecs_get_stage_count(world) <= 1, 
            ECS_INVALID_WHILE_ITERATING, NULL);
    }

    ecs_entity_t id;

    if (unsafe_world->stats.last_component_id < ECS_HI_COMPONENT_ID) {
        do {
            id = unsafe_world->stats.last_component_id ++;
        } while (ecs_exists(unsafe_world, id) && id < ECS_HI_COMPONENT_ID);        
    }
    
    if (unsafe_world->stats.last_component_id >= ECS_HI_COMPONENT_ID) {
        /* If the low component ids are depleted, return a regular entity id */
        id = ecs_new_id(unsafe_world);
    }

    return id;
}

ecs_entity_t ecs_new_w_type(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    if (!type) {
        return ecs_new_w_id(world, 0);
    }

    ecs_stage_t *stage = flecs_stage_from_world(&world);    
    ecs_entity_t entity = ecs_new_id(world);
    ecs_id_t with = stage->with;
    ecs_entity_t scope = stage->scope;

    ecs_ids_t to_add = flecs_type_to_ids(type);
    if (flecs_defer_new(world, stage, entity, &to_add)) {
        if (with) {
            ecs_add_id(world, entity, with);
        }
        if (scope) {
            ecs_add_id(world, entity, ecs_pair(EcsChildOf, scope));
        }
        return entity;
    }

    new(world, entity, &to_add);

    ecs_id_t ids[2];
    to_add = (ecs_ids_t){ .array = ids, .count = 0 };

    if (with) {
        ids[to_add.count ++] = with;
    }

    if (scope) {
        ids[to_add.count ++] = ecs_pair(EcsChildOf, scope);
    }

    if (to_add.count) {
        add_ids(world, entity, &to_add);
    }

    flecs_defer_flush(world, stage);    

    return entity;
}

ecs_entity_t ecs_new_w_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);    
    ecs_entity_t entity = ecs_new_id(world);

    ecs_id_t ids[3];
    ecs_ids_t to_add = { .array = ids, .count = 0 };

    if (id) {
        ids[to_add.count ++] = id;
    }

    ecs_id_t with = stage->with;
    if (with) {
        ids[to_add.count ++] = with;
    }

    ecs_entity_t scope = stage->scope;
    if (scope) {
        if (!id || !ECS_HAS_RELATION(id, EcsChildOf)) {
            ids[to_add.count ++] = ecs_pair(EcsChildOf, scope);
        }
    }

    if (flecs_defer_new(world, stage, entity, &to_add)) {
        return entity;
    } 

    if (to_add.count) {
        new(world, entity, &to_add);
    } else {
        ecs_eis_set(world, entity, &(ecs_record_t){ 0 });
    }

    flecs_defer_flush(world, stage);

    return entity;
}

#ifdef FLECS_PARSER

/* Traverse table graph by either adding or removing identifiers parsed from the
 * passed in expression. */
static
ecs_table_t *traverse_from_expr(
    ecs_world_t *world,
    ecs_table_t *table,
    const char *name,
    const char *expr,
    ecs_ids_t *modified,
    bool is_add,
    bool replace_and)
{
    int32_t size = modified->count;
    if (size < ECS_MAX_ADD_REMOVE) {
        size = ECS_MAX_ADD_REMOVE;
    }

    const char *ptr = expr;
    if (ptr) {
        ecs_term_t term = {0};
        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))){
            if (!ecs_term_is_initialized(&term)) {
                break;
            }

            if (ecs_term_finalize(world, name, expr, &term)) {
                return NULL;
            }

            if (!ecs_term_is_trivial(&term)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid non-trivial term in add expression");
                return NULL;
            }

            if (modified->count == size) {
                size *= 2;
                ecs_id_t *arr = ecs_os_malloc(size * ECS_SIZEOF(ecs_id_t));
                ecs_os_memcpy(arr, modified->array, 
                    modified->count * ECS_SIZEOF(ecs_id_t));

                if (modified->count != ECS_MAX_ADD_REMOVE) {
                    ecs_os_free(modified->array);
                }
                
                modified->array = arr;
            }

            if (term.oper == EcsAnd || !replace_and) {
                /* Regular AND expression */
                ecs_ids_t arr = { .array = &term.id, .count = 1 };
                if (is_add) {
                    table = flecs_table_traverse_add(
                        world, table, &arr, modified);
                } else {
                    table = flecs_table_traverse_remove(
                        world, table, &arr, modified);
                }

                ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
            } else if (term.oper == EcsAndFrom) {
                /* Add all components from the specified type */
                const EcsType *t = ecs_get(world, term.id, EcsType);
                if (!t) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "expected type for AND role");
                    return NULL;
                }
                
                ecs_id_t *ids = ecs_vector_first(t->normalized, ecs_id_t);
                int32_t i, count = ecs_vector_count(t->normalized);
                for (i = 0; i < count; i ++) {
                    ecs_ids_t arr = { .array = &ids[i], .count = 1 };
                    if (is_add) {
                        table = flecs_table_traverse_add(
                            world, table, &arr, modified);
                    } else {
                        table = flecs_table_traverse_remove(
                            world, table, &arr, modified);
                    }
                    
                    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
                }
            }

            ecs_term_fini(&term);
        }
    }

    return table;
}

/* Add/remove components based on the parsed expression. This operation is 
 * slower than traverse_from_expr, but safe to use from a deferred context. */
static
void defer_from_expr(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const char *expr,
    bool is_add,
    bool replace_and)
{
    const char *ptr = expr;
    if (ptr) {
        ecs_term_t term = {0};
        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) {
            if (!ecs_term_is_initialized(&term)) {
                break;
            }

            if (ecs_term_finalize(world, name, expr, &term)) {
                return;
            }

            if (!ecs_term_is_trivial(&term)) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid non-trivial term in add expression");
                return;
            }

            if (term.oper == EcsAnd || !replace_and) {
                /* Regular AND expression */
                if (is_add) {
                    ecs_add_id(world, entity, term.id);
                } else {
                    ecs_remove_id(world, entity, term.id);
                }
            } else if (term.oper == EcsAndFrom) {
                /* Add all components from the specified type */
                const EcsType *t = ecs_get(world, term.id, EcsType);
                if (!t) {
                    ecs_parser_error(name, expr, (ptr - expr), 
                        "expected type for AND role");
                    return;
                }
                
                ecs_id_t *ids = ecs_vector_first(t->normalized, ecs_id_t);
                int32_t i, count = ecs_vector_count(t->normalized);
                for (i = 0; i < count; i ++) {
                    if (is_add) {
                        ecs_add_id(world, entity, ids[i]);
                    } else {
                        ecs_remove_id(world, entity, ids[i]);
                    }
                }
            }

            ecs_term_fini(&term);
        }
    }
}
#endif

/* If operation is not deferred, add/remove components by finding the target
 * table and moving the entity towards it. */
static 
void traverse_add_remove(
    ecs_world_t *world,
    ecs_entity_t result,
    const char *name,
    const ecs_entity_desc_t *desc,
    ecs_entity_t scope,
    ecs_id_t with,
    bool new_entity,
    bool name_assigned)
{
    const char *sep = desc->sep;

    /* Find existing table */
    ecs_entity_info_t info = {0};
    ecs_table_t *src_table = NULL, *table = NULL;
    if (!new_entity) {
        if (flecs_get_info(world, result, &info)) {
            table = info.table;
        }
    }

    ecs_entity_t added_buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t added = { .array = added_buffer };

    ecs_entity_t removed_buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t removed = { .array = removed_buffer };

    /* Find destination table */

    /* If this is a new entity without a name, add the scope. If a name is
     * provided, the scope will be added by the add_path_w_sep function */
    if (new_entity) {
        if (new_entity && scope && !name && !name_assigned) {
            ecs_entity_t id = ecs_pair(EcsChildOf, scope);
            ecs_ids_t arr = { .array = &id, .count = 1 };
            table = flecs_table_traverse_add(world, table, &arr, &added);
            ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
        }

        if (with) {
            ecs_ids_t arr = { .array = &with, .count = 1 };
            table = flecs_table_traverse_add(world, table, &arr, &added);
            ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);            
        }
    }

    /* If a name is provided but not yet assigned, add the Name component */
    if (name && !name_assigned) {
        ecs_entity_t id = ecs_pair(ecs_id(EcsIdentifier), EcsName);
        ecs_ids_t arr = { .array = &id, .count = 1 };
        table = flecs_table_traverse_add(world, table, &arr, &added);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);            
    }

    /* Add components from the 'add' id array */
    int32_t i = 0;
    ecs_id_t id;
    const ecs_id_t *ids = desc->add;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        ecs_ids_t arr = { .array = &id, .count = 1 };
        table = flecs_table_traverse_add(world, table, &arr, &added);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    }

    /* Add components from the 'remove' id array */
    i = 0;
    ids = desc->remove;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        ecs_ids_t arr = { .array = &id, .count = 1 };
        table = flecs_table_traverse_remove(world, table, &arr, &removed);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    }

    /* Add components from the 'add_expr' expression */
    if (desc->add_expr) {
#ifdef FLECS_PARSER
        table = traverse_from_expr(
            world, table, name, desc->add_expr, &added, true, true);
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Remove components from the 'remove_expr' expression */
    if (desc->remove_expr) {
#ifdef FLECS_PARSER
    table = traverse_from_expr(
        world, table, name, desc->remove_expr, &removed, false, true);
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Commit entity to destination table */
    if (src_table != table) {
        commit(world, result, &info, table, &added, &removed, true);
    }

    /* Set name */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, result, scope, name, sep, NULL);
        ecs_assert(ecs_get_name(world, result) != NULL,
            ECS_INTERNAL_ERROR, NULL);
    }

    if (desc->symbol) {
        const char *sym = ecs_get_symbol(world, result);
        if (sym) {
            ecs_assert(!ecs_os_strcmp(desc->symbol, sym),
                ECS_INCONSISTENT_NAME, desc->symbol);
        } else {
            ecs_set_symbol(world, result, desc->symbol);
        }
    }

    if (added.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(added.array);
    }
    if (removed.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(removed.array);
    }
}

/* When in deferred mode, we need to add/remove components one by one using
 * the regular operations. */
static 
void deferred_add_remove(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name,
    const ecs_entity_desc_t *desc,
    ecs_entity_t scope,
    ecs_id_t with,
    bool new_entity,
    bool name_assigned)
{
    const char *sep = desc->sep;

    /* If this is a new entity without a name, add the scope. If a name is
     * provided, the scope will be added by the add_path_w_sep function */
    if (new_entity) {
        if (new_entity && scope && !name && !name_assigned) {
            ecs_add_id(world, entity, ecs_pair(EcsChildOf, scope));
        }

        if (with) {
            ecs_add_id(world, entity, with);
        }
    }

    /* Add components from the 'add' id array */
    int32_t i = 0;
    ecs_id_t id;
    const ecs_id_t *ids = desc->add;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        ecs_add_id(world, entity, id);
    }

    /* Add components from the 'remove' id array */
    i = 0;
    ids = desc->remove;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        ecs_remove_id(world, entity, id);
    }

    /* Add components from the 'add_expr' expression */
    if (desc->add_expr) {
#ifdef FLECS_PARSER
        defer_from_expr(world, entity, name, desc->add_expr, true, true);
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Remove components from the 'remove_expr' expression */
    if (desc->remove_expr) {
#ifdef FLECS_PARSER
        defer_from_expr(world, entity, name, desc->remove_expr, true, false);
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* Set name */
    if (name && !name_assigned) {
        ecs_add_path_w_sep(world, entity, scope, name, sep, NULL);   
    }

    /* Currently it's not supported to set the symbol from a deferred context */
    if (desc->symbol) {
        const char *sym = ecs_get_symbol(world, entity);
        ecs_assert(!ecs_os_strcmp(sym, desc->symbol), ECS_UNSUPPORTED, NULL);
        (void)sym;
    }
}

ecs_entity_t ecs_entity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(desc != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_entity_t scope = ecs_get_scope(world);
    ecs_id_t with = ecs_get_with(world);

    const char *name = desc->name;
    const char *sep = desc->sep;
    if (!sep) {
        sep = ".";
    }

    const char *root_sep = desc->root_sep;

    bool new_entity = false;
    bool name_assigned = false;

    /* Remove optional prefix from name. Entity names can be derived from 
     * language identifiers, such as components (typenames) and systems
     * function names). Because C does not have namespaces, such identifiers
     * often encode the namespace as a prefix.
     * To ensure interoperability between C and C++ (and potentially other 
     * languages with namespacing) the entity must be stored without this prefix
     * and with the proper namespace, which is what the name_prefix is for */
    const char *prefix = world->name_prefix;
    if (name && prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(name, prefix, len) && 
           (isupper(name[len]) || name[len] == '_')) 
        {
            if (name[len] == '_') {
                name = name + len + 1;
            } else {
                name = name + len;
            }
        }
    }

    /* Find or create entity */
    ecs_entity_t result = desc->entity;
    if (!result) {
        if (name) {
            result = ecs_lookup_path_w_sep(
                world, scope, name, sep, root_sep, false);
            if (result) {
                name_assigned = true;
            }
        }

        if (!result) {
            if (desc->use_low_id) {
                result = ecs_new_component_id(world);
            } else {
                result = ecs_new_id(world);
            }
            new_entity = true;
            ecs_assert(ecs_get_type(world, result) == NULL, 
                ECS_INTERNAL_ERROR, NULL);
        }
    } else {
        ecs_assert(ecs_is_valid(world, result), ECS_INVALID_PARAMETER, NULL);

        name_assigned = ecs_has_pair(
            world, result, ecs_id(EcsIdentifier), EcsName);
        if (name && name_assigned) {
            /* If entity has name, verify that name matches */
            char *path = ecs_get_path_w_sep(world, scope, result, sep, NULL);
            if (path) {
                if (ecs_os_strcmp(path, name)) {
                    /* Mismatching name */
                    ecs_os_free(path);
                    return 0;
                }
                ecs_os_free(path);
            }
        }
    }

    ecs_assert(name_assigned == ecs_has_pair(
        world, result, ecs_id(EcsIdentifier), EcsName),
            ECS_INTERNAL_ERROR, NULL);

    if (stage->defer) {
        deferred_add_remove(world, result, name, desc, 
            scope, with, new_entity, name_assigned);
    } else {
        traverse_add_remove(world, result, name, desc,
            scope, with, new_entity, name_assigned);
    }

    return result;
}

ecs_entity_t ecs_component_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = (ecs_world_t*)ecs_get_world(world);

    bool is_readonly = world->is_readonly;
    bool is_deferred = ecs_is_deferred(world);
    int32_t defer_count = 0;
    ecs_vector_t *defer_queue = NULL;
    ecs_stage_t *stage = NULL;

    /* If world is readonly or deferring is enabled, component registration can
     * still happen directly on the main storage, but only if the application
     * is singlethreaded. */
    if (is_readonly || is_deferred) {
        ecs_assert(ecs_get_stage_count(world) <= 1, 
            ECS_INVALID_WHILE_ITERATING, NULL);

        /* Silence readonly warnings */
        world->is_readonly = false;

        /* Hack around safety checks (this ought to look ugly) */
        ecs_world_t *temp_world = world;
        stage = flecs_stage_from_world(&temp_world);
        defer_count = stage->defer;
        defer_queue = stage->defer_queue;
        stage->defer = 0;
        stage->defer_queue = NULL;
    }

    ecs_entity_desc_t entity_desc = desc->entity;
    entity_desc.use_low_id = true;
    if (!entity_desc.symbol) {
        entity_desc.symbol = entity_desc.name;
    }

    ecs_entity_t e = desc->entity.entity;
    ecs_entity_t result = ecs_entity_init(world, &entity_desc);
    if (!result) {
        return 0;
    }

    bool added = false;
    EcsComponent *ptr = ecs_get_mut(world, result, EcsComponent, &added);

    if (added) {
        ptr->size = flecs_from_size_t(desc->size);
        ptr->alignment = flecs_from_size_t(desc->alignment);
    } else {
        if (ptr->size != flecs_from_size_t(desc->size)) {
            ecs_abort(ECS_INVALID_COMPONENT_SIZE, desc->entity.name);
        }
        if (ptr->alignment != flecs_from_size_t(desc->alignment)) {
            ecs_abort(ECS_INVALID_COMPONENT_ALIGNMENT, desc->entity.name);
        }
    }

    ecs_modified(world, result, EcsComponent);

    if (e > world->stats.last_component_id && e < ECS_HI_COMPONENT_ID) {
        world->stats.last_component_id = e + 1;
    }

    /* Ensure components cannot be deleted */
    ecs_add_pair(world, result, EcsOnDelete, EcsThrow);    

    if (is_readonly || is_deferred) {
        /* Restore readonly state / defer count */
        world->is_readonly = is_readonly;
        stage->defer = defer_count;
        stage->defer_queue = defer_queue;
    }

    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(ecs_has(world, result, EcsComponent), ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_entity_t ecs_type_init(
    ecs_world_t *world,
    const ecs_type_desc_t *desc)
{
    ecs_entity_t result = ecs_entity_init(world, &desc->entity);
    if (!result) {
        return 0;
    }

    ecs_table_t *table = NULL, *normalized = NULL;

    ecs_entity_t added_buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t added = { .array = added_buffer };

    /* Find destination table (and type) */

    /* Add components from the 'add' id array */
    int32_t i = 0;
    ecs_id_t id;
    const ecs_id_t *ids = desc->ids;
    while ((i < ECS_MAX_ADD_REMOVE) && (id = ids[i ++])) {
        ecs_ids_t arr = { .array = &id, .count = 1 };
        normalized = flecs_table_traverse_add(world, normalized, &arr, &added);
        table = flecs_table_traverse_add(world, table, &arr, &added);
        ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    }

    /* If expression is set, add it to the table */
    if (desc->ids_expr) {
#ifdef FLECS_PARSER
        normalized = traverse_from_expr(
            world, normalized, desc->entity.name, desc->ids_expr, &added, 
            true, true);

        table = traverse_from_expr(
            world, table, desc->entity.name, desc->ids_expr, &added, 
            true, false);
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    if (added.count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(added.array);
    }

    ecs_type_t type = NULL;
    ecs_type_t normalized_type = NULL;
    
    if (table) {
        type = table->type;
    }
    if (normalized) {
        normalized_type = normalized->type;
    }

    bool add = false;
    EcsType *type_ptr = ecs_get_mut(world, result, EcsType, &add);
    if (add) {
        type_ptr->type = type;
        type_ptr->normalized = normalized_type;

        /* This will allow the type to show up in debug tools */
        if (type) {
            ecs_map_set(world->type_handles, (uintptr_t)type, &result);
        }

        ecs_modified(world, result, EcsType);
    } else {
        if (type_ptr->type != type) {
            ecs_abort(ECS_ALREADY_DEFINED, desc->entity.name);
        }
        if (type_ptr->normalized != normalized_type) {
            ecs_abort(ECS_ALREADY_DEFINED, desc->entity.name);
        }        
    }

    return result;
}

const ecs_entity_t* ecs_bulk_new_w_data(
    ecs_world_t *world,
    int32_t count,
    const ecs_ids_t *components,
    void * data)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    const ecs_entity_t *ids;
    if (flecs_defer_bulk_new(world, stage, count, components, data, &ids)) {
        return ids;
    }

    ecs_table_t *table = flecs_table_find_or_create(world, components);    
    ids = new_w_data(world, table, NULL, count, data, NULL);
    flecs_defer_flush(world, stage);
    return ids;
}

const ecs_entity_t* ecs_bulk_new_w_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    const ecs_entity_t *ids;
    ecs_ids_t components = flecs_type_to_ids(type);
    if (flecs_defer_bulk_new(world, stage, count, &components, NULL, &ids)) {
        return ids;
    }
    ecs_table_t *table = ecs_table_from_type(world, type);
    ids = new_w_data(world, table, NULL, count, NULL, NULL);
    flecs_defer_flush(world, stage);
    return ids;
}

const ecs_entity_t* ecs_bulk_new_w_id(
    ecs_world_t *world,
    ecs_id_t id,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_ids_t components = {
        .array = &id,
        .count = 1
    };
    const ecs_entity_t *ids;
    if (flecs_defer_bulk_new(world, stage, count, &components, NULL, &ids)) {
        return ids;
    }
    ecs_table_t *table = flecs_table_find_or_create(world, &components);
    ids = new_w_data(world, table, NULL, count, NULL, NULL);
    flecs_defer_flush(world, stage);
    return ids;
}

void ecs_clear(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_clear(world, stage, entity)) {
        return;
    }

    ecs_entity_info_t info;
    info.table = NULL;

    flecs_get_info(world, entity, &info);

    ecs_table_t *table = info.table;
    if (table) {
        ecs_type_t type = table->type;

        /* Remove all components */
        ecs_ids_t to_remove = flecs_type_to_ids(type);
        remove_ids_w_info(world, entity, &info, &to_remove);
    }    

    flecs_defer_flush(world, stage);
}

static
void on_delete_action(
    ecs_world_t *world,
    ecs_entity_t entity);

static
void throw_invalid_delete(
    ecs_world_t *world,
    ecs_id_t id)
{
    char buff[256];
    ecs_id_str(world, id, buff, 256);
    ecs_abort(ECS_INVALID_DELETE, buff);    
}

static
void remove_from_table(
    ecs_world_t *world,
    ecs_table_t *src_table,
    ecs_id_t id,
    int32_t column,
    int32_t column_count)
{
    ecs_entity_t removed_buffer[ECS_MAX_ADD_REMOVE];
    ecs_ids_t removed = { .array = removed_buffer };

    if (column_count > ECS_MAX_ADD_REMOVE) {
        removed.array = ecs_os_malloc_n(ecs_id_t, column_count);
    }

    ecs_table_t *dst_table = src_table; 
    ecs_id_t *ids = ecs_vector_first(src_table->type, ecs_id_t);

    /* If id is pair but the column pointed to is not a pair, the record is
     * pointing to an instance of the id that has a (non-PAIR) role. */
    bool is_pair = ECS_HAS_ROLE(id, PAIR);     
    bool is_role = is_pair && !ECS_HAS_ROLE(ids[column], PAIR);
    ecs_assert(!is_role || ((ids[column] & ECS_ROLE_MASK) != 0), 
        ECS_INTERNAL_ERROR, NULL);
    bool is_wildcard = ecs_id_is_wildcard(id);

    int32_t i, count = ecs_vector_count(src_table->type), removed_count = 0;
    ecs_entity_t entity = ECS_PAIR_RELATION(id);

    for (i = column; i < count; i ++) {
        ecs_id_t e = ids[i];

        if (is_role) {
            if ((e & ECS_COMPONENT_MASK) != entity) {
                continue;
            }
        } else if (is_wildcard && !ecs_id_match(e, id)) {
            continue;
        }

        ecs_ids_t to_remove = { .array = &e, .count = 1 };
        dst_table = flecs_table_traverse_remove(
            world, dst_table, &to_remove, &removed);
        
        removed_count ++;
        if (removed_count == column_count) {
            break;
        }
    }

    ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!dst_table->type) {
        /* If this removes all components, clear table */
        flecs_table_clear_entities(world, src_table);
    } else {
        /* Otherwise, merge table into dst_table */
        if (dst_table != src_table) {
            ecs_data_t *src_data = flecs_table_get_data(src_table);
            int32_t src_count = ecs_table_count(src_table);
            if (removed.count && src_data) {
                flecs_run_remove_actions(world, src_table, 
                    src_data, 0, src_count, &removed);
            }

            ecs_data_t *dst_data = flecs_table_get_data(dst_table);
            flecs_table_merge(world, dst_table, src_table, dst_data, src_data);
        }
    }

    if (column_count > ECS_MAX_ADD_REMOVE) {
        ecs_os_free(removed.array);
    }
}

static
void delete_objects(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_data_t *data = flecs_table_get_data(table);
    if (data) {
        ecs_entity_t *entities = ecs_vector_first(
            data->entities, ecs_entity_t);

        int32_t i, count = ecs_vector_count(data->entities);
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = entities[i];
            ecs_record_t *r = flecs_sparse_get(
                world->store.entity_index, ecs_record_t, e);
            
            /* If row is negative, it means the entity is being monitored. Only
             * monitored entities can have delete actions */
            if (r && r->row < 0) {
                /* Make row positive which prevents infinite recursion in case
                 * of cyclic delete actions */
                r->row = (-r->row);

                /* Run delete actions for objects */
                on_delete_action(world, entities[i]);
            }        
        }

        /* Clear components from table (invokes destructors, OnRemove) */
        flecs_table_delete_entities(world, table);            
    } 
}

static
void delete_tables_for_id_record(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_id_record_t *idr)
{
    /* Delete tables in id record. Because deleting the table updates the
     * map, remove the map pointer from the id record. This will prevent the
     * table from removing itself from the map as it is deleted, which
     * allows for iterating the map without changing it. */
    
    if (!world->is_fini) {
        ecs_map_t *table_index = idr->table_index;
        idr->table_index = NULL;
        ecs_map_iter_t it = ecs_map_iter(table_index);
        ecs_table_record_t *tr;
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            flecs_delete_table(world, tr->table);
        }
        ecs_map_free(table_index);

        flecs_clear_id_record(world, id);
    }
}

static
void on_delete_object_action(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (idr) {
        ecs_map_t *table_index = idr->table_index;
        ecs_map_iter_t it = ecs_map_iter(table_index);
        ecs_table_record_t *tr;

        /* Execute the on delete action */
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            ecs_table_t *table = tr->table;

            if (!ecs_table_count(table)) {
                continue;
            }

            ecs_id_t *rel_id = ecs_vector_get(table->type, ecs_id_t, tr->column);
            ecs_assert(rel_id != NULL, ECS_INTERNAL_ERROR, NULL);

            ecs_entity_t rel = ECS_PAIR_RELATION(*rel_id);
            /* delete_object_action should be invoked for relations */
            ecs_assert(rel != 0, ECS_INTERNAL_ERROR,  NULL);

            /* Get the record for the relation, to find the delete action */
            ecs_id_record_t *idrr = flecs_get_id_record(world, rel);
            if (idrr) {
                ecs_entity_t action = idrr->on_delete_object;
                if (!action || action == EcsRemove) {
                    remove_from_table(world, table, id, tr->column, tr->count);
                    it = ecs_map_iter(table_index);
                } else if (action == EcsDelete) {
                    delete_objects(world, table);
                } else if (action == EcsThrow) {
                    throw_invalid_delete(world, id);
                }
            } else {
                /* If no record was found for the relation, assume the default
                 * action which is to remove the relationship */
                remove_from_table(world, table, id, tr->column, tr->count);
                it = ecs_map_iter(table_index);
            }
        }

        delete_tables_for_id_record(world, id, idr);
    }
}

static
void on_delete_relation_action(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_id_record_t *idr = flecs_get_id_record(world, id);

    char buf[255]; ecs_id_str(world, id, buf, 255);

    if (idr) {
        ecs_entity_t on_delete = idr->on_delete;
        if (on_delete == EcsThrow) {
            throw_invalid_delete(world, id);
        }

        ecs_map_t *table_index = idr->table_index;
        ecs_map_iter_t it = ecs_map_iter(table_index);
        ecs_table_record_t *tr;
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            ecs_table_t *table = tr->table;
            ecs_entity_t action = idr->on_delete;

            if (!action || action == EcsRemove) {
                remove_from_table(world, table, id, tr->column, tr->count);
            } else if (action == EcsDelete) {
                delete_objects(world, table);
            }
        }

        delete_tables_for_id_record(world, id, idr);
    }
}

static
void on_delete_action(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    on_delete_relation_action(world, entity);
    on_delete_relation_action(world, ecs_pair(entity, EcsWildcard));
    on_delete_object_action(world, ecs_pair(EcsWildcard, entity));
}

void ecs_delete_children(
    ecs_world_t *world,
    ecs_entity_t parent)
{
    on_delete_action(world, parent);
}

void ecs_delete(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    if (flecs_defer_delete(world, stage, entity)) {
        return;
    }

    ecs_record_t *r = flecs_sparse_get(
        world->store.entity_index, ecs_record_t, entity);
    if (r) {
        ecs_entity_info_t info = {0};
        set_info_from_record(&info, r);

        ecs_table_t *table = info.table;
        uint64_t table_id = 0;
        if (table) {
            table_id = table->id;
        }

        if (info.is_watched) {
            /* Make row positive which prevents infinite recursion in case
             * of cyclic delete actions */
            r->row = (-r->row);

            /* Ensure that the store contains no dangling references to the
             * deleted entity (as a component, or as part of a relation) */
            on_delete_action(world, entity);

            /* Refetch data. In case of circular relations, the entity may have
             * moved to a different table. */
            set_info_from_record(&info, r);
            
            table = info.table;
            if (table) {
                table_id = table->id;
            } else {
                table_id = 0;
            }

            if (r->table) {
                ecs_ids_t to_remove = flecs_type_to_ids(r->table->type);
                update_component_monitors(world, entity, NULL, &to_remove);
            }
        }

        ecs_assert(!table_id || table, ECS_INTERNAL_ERROR, NULL);

        /* If entity has components, remove them. Check if table is still alive,
         * as delete actions could have deleted the table already. */
        if (table_id && flecs_sparse_is_alive(world->store.tables, table_id)) {
            ecs_type_t type = table->type;
            ecs_ids_t to_remove = flecs_type_to_ids(type);
            delete_entity(world, table, info.data, info.row, &to_remove);
            r->table = NULL;
        }

        r->row = 0;

        /* Remove (and invalidate) entity after executing handlers */
        flecs_sparse_remove(world->store.entity_index, entity);
    }

    flecs_defer_flush(world, stage);
}

void ecs_add_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t components = flecs_type_to_ids(type);
    add_ids(world, entity, &components);
}

void ecs_add_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t components = { .array = &id, .count = 1 };
    add_ids(world, entity, &components);
}

void ecs_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t components = flecs_type_to_ids(type);
    remove_ids(world, entity, &components);
}

void ecs_remove_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t components = { .array = &id, .count = 1 };
    remove_ids(world, entity, &components);
}

// DEPRECATED
void ecs_add_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id_add,
    ecs_id_t id_remove)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id_add), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id_remove), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t components_add = { .array = &id_add, .count = 1 };      
    ecs_ids_t components_remove = { .array = &id_remove, .count = 1 };      
    add_remove(world, entity, &components_add, &components_remove);
}

void ecs_add_remove_type(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    ecs_ids_t components_add = flecs_type_to_ids(to_add);
    ecs_ids_t components_remove = flecs_type_to_ids(to_remove);
    add_remove(world, entity, &components_add, &components_remove);
}

ecs_entity_t ecs_clone(
    ecs_world_t *world,
    ecs_entity_t dst,
    ecs_entity_t src,
    bool copy_value)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, src), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    
    if (!dst) {
        dst = ecs_new_id(world);
    }

    if (flecs_defer_clone(world, stage, dst, src, copy_value)) {
        return dst;
    }

    ecs_entity_info_t src_info;
    bool found = flecs_get_info(world, src, &src_info);
    ecs_table_t *src_table = src_info.table;

    if (!found || !src_table) {
        return dst;
    }

    ecs_type_t src_type = src_table->type;
    ecs_ids_t to_add = flecs_type_to_ids(src_type);

    ecs_entity_info_t dst_info = {0};
    dst_info.row = new_entity(world, dst, &dst_info, src_table, &to_add, true);

    if (copy_value) {
        flecs_table_move(world, dst, src, src_table, dst_info.data, 
            dst_info.row, src_table, src_info.data, src_info.row, true);

        flecs_run_set_systems(world, 0,
                src_table, src_info.data, NULL, dst_info.row, 1, true);
    }

    flecs_defer_flush(world, stage);

    return dst;
}

const void* ecs_get_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(flecs_stage_from_readonly_world(world)->asynchronous == false, 
        ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    ecs_record_t *r = ecs_eis_get(world, entity);
    if (!r) {
        return NULL;
    }

    ecs_table_t *table = r->table;
    if (!table) {
        return NULL;
    }

    ecs_id_record_t *idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    ecs_table_record_t *tr = ecs_map_get(idr->table_index, 
        ecs_table_record_t, table->id);
    if (!tr) {
       return get_base_component(world, table, id, idr->table_index, NULL, 0);
    }

    bool is_monitored;
    int32_t row = flecs_record_to_row(r->row, &is_monitored);

    return get_component_w_index(table, tr->column, row);
}

const void* ecs_get_ref_w_id(
    const ecs_world_t * world,
    ecs_ref_t * ref,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ref != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!entity || !ref->entity || entity == ref->entity, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!id || !ref->component || id == ref->component, ECS_INVALID_PARAMETER, NULL);
    ecs_record_t *record = ref->record;

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    entity |= ref->entity;

    if (!record) {
        record = ecs_eis_get(world, entity);
    }

    if (!record || !record->table) {
        return NULL;
    }

    ecs_table_t *table = record->table;

    if (ref->record == record &&
        ref->table == table &&
        ref->row == record->row &&
        ref->alloc_count == table->alloc_count)
    {
        return ref->ptr;
    }

    id |= ref->component;

    int32_t row = record->row;

    ref->entity = entity;
    ref->component = id;
    ref->table = table;
    ref->row = record->row;
    ref->alloc_count = table->alloc_count;

    if (table) {
        bool is_monitored;
        row = flecs_record_to_row(row, &is_monitored);
        ref->ptr = get_component(world, table, row, id);
    }

    ref->record = record;

    return ref->ptr;
}

void* ecs_get_mut_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool * is_added)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    void *result;

    if (flecs_defer_set(
        world, stage, EcsOpMut, entity, id, 0, NULL, &result, is_added))
    {
        return result;
    }

    ecs_entity_info_t info;
    result = get_mutable(world, entity, id, &info, is_added);
    
    /* Store table so we can quickly check if returned pointer is still valid */
    ecs_table_t *table = info.record->table;
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Keep track of alloc count of table, since even if the entity has not
     * moved, other entities could have been added to the table which could
     * reallocate arrays. Also store the row, as the entity could have 
     * reallocated. */
    int32_t alloc_count = table->alloc_count;
    int32_t row = info.record->row;
    
    flecs_defer_flush(world, stage);

    /* Ensure that after flushing, the pointer is still valid. Flushing may
     * trigger callbacks, which could do anything with the entity */
    if (table != info.record->table || 
        alloc_count != info.record->table->alloc_count ||
        row != info.record->row) 
    {
        if (flecs_get_info(world, entity, &info) && info.table) {
            result = get_component(world, info.table, info.row, id);
        } else {
            /* A trigger has removed the component we just added. This is not
             * allowed, an application should always be able to assume that
             * get_mut returns a valid pointer. */
            ecs_assert(false, ECS_INVALID_OPERATION, NULL);
        }
    }

    return result;
}

void* ecs_emplace_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!ecs_has_id(world, entity, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    void *result;

    if (flecs_defer_set(
        world, stage, EcsOpMut, entity, id, 0, NULL, &result, NULL))
    {
        return result;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_ids_t to_add = {
        .array = &id,
        .count = 1
    };

    add_ids_w_info(world, entity, &info, &to_add, 
        false /* Add component without constructing it */ );

    void *ptr = get_component(world, info.table, info.row, id);

    flecs_defer_flush(world, stage);

    return ptr;
}

void ecs_modified_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_modified(world, stage, entity, id)) {
        return;
    }

    /* If the entity does not have the component, calling ecs_modified is 
     * invalid. The assert needs to happen after the defer statement, as the
     * entity may not have the component when this function is called while
     * operations are being deferred. */
    ecs_assert(ecs_has_id(world, entity, id), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_entity_info_t info = {0};
    if (flecs_get_info(world, entity, &info)) {
        ecs_column_t *column = ecs_table_column_for_id(world, info.table, id);
        ecs_assert(column != NULL, ECS_INTERNAL_ERROR, NULL);
        flecs_run_set_systems(world, id, 
            info.table, info.data, column, info.row, 1, false);
    }

    flecs_table_mark_dirty(info.table, id);
    
    flecs_defer_flush(world, stage);
}

static
ecs_entity_t assign_ptr_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    void * ptr,
    bool is_move,
    bool notify)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (!entity) {
        entity = ecs_new_id(world);
        ecs_entity_t scope = stage->scope;
        if (scope) {
            ecs_add_pair(world, entity, EcsChildOf, scope);
        }
    }

    if (flecs_defer_set(world, stage, EcsOpSet, entity, id, 
        flecs_from_size_t(size), ptr, NULL, NULL))
    {
        return entity;
    }

    ecs_entity_info_t info;

    void *dst = get_mutable(world, entity, id, &info, NULL);

    /* This can no longer happen since we defer operations */
    ecs_assert(dst != NULL, ECS_INTERNAL_ERROR, NULL);

    if (ptr) {
        ecs_entity_t real_id = ecs_get_typeid(world, id);
        const ecs_type_info_t *cdata = get_c_info(world, real_id);
        if (cdata) {
            if (is_move) {
                ecs_move_t move = cdata->lifecycle.move;
                if (move) {
                    move(world, real_id, &entity, &entity, dst, ptr, size, 1, 
                        cdata->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, ptr, flecs_from_size_t(size));
                }
            } else {
                ecs_copy_t copy = cdata->lifecycle.copy;
                if (copy) {
                    copy(world, real_id, &entity, &entity, dst, ptr, size, 1, 
                        cdata->lifecycle.ctx);
                } else {
                    ecs_os_memcpy(dst, ptr, flecs_from_size_t(size));
                }
            }
        } else {
            ecs_os_memcpy(dst, ptr, flecs_from_size_t(size));
        }
    } else {
        memset(dst, 0, size);
    }

    flecs_table_mark_dirty(info.table, id);

    if (notify) {
        ecs_column_t *column = ecs_table_column_for_id(world, info.table, id);
        flecs_run_set_systems(world, id, 
            info.table, info.data, column, info.row, 1, false);
    }

    flecs_defer_flush(world, stage);

    return entity;
}

ecs_entity_t ecs_set_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!entity || ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Safe to cast away const: function won't modify if move arg is false */
    return assign_ptr_w_id(
        world, entity, id, size, (void*)ptr, false, true);
}

ecs_entity_t ecs_get_case(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t sw_id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, sw_id), ECS_INVALID_PARAMETER, NULL);

    ecs_entity_info_t info;
    ecs_table_t *table;
    if (!flecs_get_info(world, entity, &info) || !(table = info.table)) {
        return 0;
    }

    sw_id = sw_id | ECS_SWITCH;

    ecs_type_t type = table->type;
    int32_t index = ecs_type_index_of(type, 0, sw_id);
    if (index == -1) {
        return 0;
    }

    index -= table->sw_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_assert(info.data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_switch_t *sw = info.data->sw_columns[index].data;  
    return flecs_switch_get(sw, info.row);  
}

void ecs_enable_component_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);

    if (flecs_defer_enable(
        world, stage, entity, id, enable))
    {
        return;
    } else {
        /* Operations invoked by enable/disable should not be deferred */
        stage->defer --;
    }

    ecs_entity_info_t info;
    flecs_get_info(world, entity, &info);

    ecs_entity_t bs_id = (id & ECS_COMPONENT_MASK) | ECS_DISABLED;
    
    ecs_table_t *table = info.table;
    int32_t index = -1;
    if (table) {
        index = ecs_type_index_of(table->type, 0, bs_id);
    }

    if (index == -1) {
        ecs_add_id(world, entity, bs_id);
        ecs_enable_component_w_id(world, entity, id, enable);
        return;
    }

    index -= table->bs_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_assert(info.data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &info.data->bs_columns[index].data;
    ecs_assert(bs != NULL, ECS_INTERNAL_ERROR, NULL);

    flecs_bitset_set(bs, info.row, enable);
}

bool ecs_is_component_enabled_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_entity_info_t info;
    ecs_table_t *table;
    if (!flecs_get_info(world, entity, &info) || !(table = info.table)) {
        return false;
    }

    ecs_entity_t bs_id = (id & ECS_COMPONENT_MASK) | ECS_DISABLED;

    ecs_type_t type = table->type;
    int32_t index = ecs_type_index_of(type, 0, bs_id);
    if (index == -1) {
        /* If table does not have DISABLED column for component, component is
         * always enabled, if the entity has it */
        return ecs_has_id(world, entity, id);
    }

    index -= table->bs_column_offset;
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    /* Data cannot be NULl, since entity is stored in the table */
    ecs_assert(info.data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_bitset_t *bs = &info.data->bs_columns[index].data;  

    return flecs_bitset_get(bs, info.row);
}

bool ecs_has_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    if (ECS_HAS_ROLE(id, CASE)) {
        ecs_entity_info_t info;
        ecs_table_t *table;
        if (!flecs_get_info(world, entity, &info) || !(table = info.table)) {
            return false;
        }

        int32_t index = flecs_table_switch_from_case(world, table, id);
        ecs_assert(index < table->sw_column_count, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = info.data;
        ecs_switch_t *sw = data->sw_columns[index].data;
        ecs_entity_t value = flecs_switch_get(sw, info.row);

        return value == (id & ECS_COMPONENT_MASK);
    } else {
        ecs_table_t *table = ecs_get_table(world, entity);
        if (!table) {
            return false;
        }

        return ecs_type_match(
            world, table, table->type, 0, id, EcsIsA, 0, 0, NULL) != -1;
    }
}

bool ecs_has_type(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_type_t type)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    return has_type(world, entity, type, true, true);
}

ecs_entity_t ecs_get_object(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t rel,
    int32_t index)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(rel != 0, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return 0;
    }

    ecs_table_t *table = ecs_get_table(world, entity);
    if (!table) {
        return 0;
    }

    ecs_id_t wc = ecs_pair(rel, EcsWildcard);
    ecs_table_record_t *tr = flecs_get_table_record(world, table, wc);
    if (!tr) {
        return 0;
    }

    if (index >= tr->count) {
        return 0;
    }

    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    return ecs_pair_object(world, ids[tr->column + index]);
}

const char* ecs_get_name(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    const EcsIdentifier *ptr = ecs_get_pair(
        world, entity, EcsIdentifier, EcsName);

    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

const char* ecs_get_symbol(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);

    const EcsIdentifier *ptr = ecs_get_pair(
        world, entity, EcsIdentifier, EcsSymbol);

    if (ptr) {
        return ptr->value;
    } else {
        return NULL;
    }
}

ecs_entity_t ecs_set_name(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        entity = ecs_new_id(world);
    }
    
    ecs_set_pair(world, entity, EcsIdentifier, EcsName, {.value = (char*)name});

    return entity;
}

ecs_entity_t ecs_set_symbol(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        entity = ecs_new_id(world);
    }
    
    ecs_set_pair(world, entity, EcsIdentifier, EcsSymbol, {
        .value = (char*)name
    });

    return entity;
}

ecs_type_t ecs_type_from_id(
    ecs_world_t *world,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    if (!id) {
        return NULL;
    }

    if (!(id & ECS_ROLE_MASK)) {
        const EcsType *type = ecs_get(world, id, EcsType);
        if (type) {
            return type->normalized;
        }
    }

    ecs_ids_t ids = {
        .array = &id,
        .count = 1
    };

    ecs_table_t *table = flecs_table_find_or_create(world, &ids);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

ecs_id_t ecs_type_to_id(
    const ecs_world_t *world, 
    ecs_type_t type)
{
    (void)world;

    if (!type) {
        return 0;
    }
    
    /* If array contains n entities, it cannot be reduced to a single entity */
    if (ecs_vector_count(type) != 1) {
        ecs_abort(ECS_TYPE_NOT_AN_ENTITY, NULL);
    }

    return *(ecs_vector_first(type, ecs_id_t));
}

ecs_id_t ecs_make_pair(
    ecs_entity_t relation,
    ecs_entity_t object)
{
    return ecs_pair(relation, object);
}

bool ecs_is_valid(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    /* 0 is not a valid entity id */
    if (!entity) {
        return false;
    }

    /* Entities should not contain data in dead zone bits */
    if (entity & ~0xFF00FFFFFFFFFFFF) {
        return false;
    }
    
    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    /* When checking roles and/or pairs, the generation count may have been
     * stripped away. Just test if the entity is 0 or not. */
    if (ECS_HAS_ROLE(entity, PAIR)) {
        ecs_entity_t lo = ECS_PAIR_OBJECT(entity);
        ecs_entity_t hi = ECS_PAIR_RELATION(entity);
        return lo != 0 && hi != 0;
    } else
    if (entity & ECS_ROLE) {
        return ecs_entity_t_lo(entity) != 0;
    }

    /* An id may not yet exist in the world which does not mean it cannot be
     * used as an entity identifier. An example is when a hard-coded entity id
     * is used. However, if the entity id does exist in the world, it must be
     * alive. */
    return !ecs_exists(world, entity) || ecs_is_alive(world, entity);
}

bool ecs_is_alive(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    return ecs_eis_is_alive(world, entity);
}

ecs_entity_t ecs_get_alive(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    if (ecs_is_alive(world, entity)) {
        return entity;
    }

    /* Make sure id does not have generation. This guards against accidentally
     * "upcasting" a not alive identifier to a alive one. */
    ecs_assert((uint32_t)entity == entity, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_entity_t current = ecs_eis_get_current(world, entity);
    if (!current || !ecs_is_alive(world, current)) {
        return 0;
    }

    return current;
}

void ecs_ensure(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    if (ecs_eis_is_alive(world, entity)) {
        /* Nothing to be done, already alive */
        return;
    }

    /* Ensure id exists. The underlying datastructure will verify that the
     * generation count matches the provided one. */
    ecs_eis_ensure(world, entity);
}

bool ecs_exists(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    return ecs_eis_exists(world, entity);
}

ecs_table_t* ecs_get_table(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, entity), ECS_INVALID_PARAMETER, NULL);
    
    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_record_t *record = ecs_eis_get(world, entity);
    ecs_table_t *table;
    if (record && (table = record->table)) {
        return table;
    }
    
    return NULL;
}

ecs_type_t ecs_get_type(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = ecs_get_table(world, entity);
    if (table) {
        return table->type;
    }

    return NULL;
}

ecs_id_t ecs_get_typeid(
    const ecs_world_t *world,
    ecs_id_t id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_is_valid(world, id), ECS_INVALID_PARAMETER, NULL);

    if (ECS_HAS_ROLE(id, PAIR)) {
        /* Make sure we're not working with a stage */
        world = ecs_get_world(world);

        ecs_entity_t rel = ecs_get_alive(world, ECS_PAIR_RELATION(id));

        /* If relation is marked as a tag, it never has data. Return relation */
        if (ecs_has_id(world, rel, EcsTag)) {
            return 0;
        }

        const EcsComponent *ptr = ecs_get(world, rel, EcsComponent);
        if (ptr && ptr->size != 0) {
            return rel;
        } else {
            ecs_entity_t obj = ecs_get_alive(world, ECS_PAIR_OBJECT(id));
            ptr = ecs_get(world, obj, EcsComponent);
            
            if (ptr && ptr->size != 0) {
                return obj;
            }

            /* Neither relation nor object have data */
            return 0;
        }

    } else if (id & ECS_ROLE_MASK) {
        return 0;
    }

    return id;
}

int32_t ecs_count_type(
    const ecs_world_t *world,
    ecs_type_t type)
{    
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!type) {
        return 0;
    }

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    return ecs_count_filter(world, &(ecs_filter_t){
        .include = type
    });
}

int32_t ecs_count_id(
    const ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!entity) {
        return 0;
    }

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    /* Get temporary type that just contains entity */
    ECS_VECTOR_STACK(type, ecs_entity_t, &entity, 1);

    return ecs_count_filter(world, &(ecs_filter_t){
        .include = type
    });
}

int32_t ecs_count_filter(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Make sure we're not working with a stage */
    world = ecs_get_world(world);

    ecs_sparse_t *tables = world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);
    int32_t result = 0;

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
        if (!filter || flecs_table_match_filter(world, table, filter)) {
            result += ecs_table_count(table);
        }
    }
    
    return result;
}

bool ecs_defer_begin(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_none(world, stage);
}

bool ecs_defer_end(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    return flecs_defer_flush(world, stage);
}

static
size_t append_to_str(
    char **buffer,
    const char *str,
    size_t bytes_left,
    size_t *required)
{
    char *ptr = NULL;
    if (buffer) {
        ptr = *buffer;
    }

    size_t len = strlen(str);
    size_t to_write;
    if (bytes_left < len) {
        to_write = bytes_left;
        bytes_left = 0;
    } else {
        to_write = len;
        bytes_left -= len;
    }
    
    if (to_write && ptr) {
        ecs_os_memcpy(ptr, str, to_write);
    }

    (*required) += len;

    if (buffer) {
        (*buffer) += to_write;
    }

    return bytes_left;
}

const char* ecs_role_str(
    ecs_entity_t entity)
{
    if (ECS_HAS_ROLE(entity, PAIR)) {
        return "PAIR";
    } else
    if (ECS_HAS_ROLE(entity, DISABLED)) {
        return "DISABLED";
    } else    
    if (ECS_HAS_ROLE(entity, XOR)) {
        return "XOR";
    } else
    if (ECS_HAS_ROLE(entity, OR)) {
        return "OR";
    } else
    if (ECS_HAS_ROLE(entity, AND)) {
        return "AND";
    } else
    if (ECS_HAS_ROLE(entity, NOT)) {
        return "NOT";
    } else
    if (ECS_HAS_ROLE(entity, SWITCH)) {
        return "SWITCH";
    } else
    if (ECS_HAS_ROLE(entity, CASE)) {
        return "CASE";
    } else
    if (ECS_HAS_ROLE(entity, OWNED)) {
        return "OWNED";
    } else {
        return "UNKNOWN";
    }
}

size_t ecs_id_str(
    const ecs_world_t *world,
    ecs_id_t id,
    char *buffer,
    size_t buffer_len)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    char *ptr = buffer;
    char **pptr = NULL;
    if (ptr) {
        pptr = &ptr;
    }

    size_t bytes_left = buffer_len - 1, required = 0;
    if (id & ECS_ROLE_MASK && !ECS_HAS_ROLE(id, PAIR)) {
        const char *role = ecs_role_str(id);
        bytes_left = append_to_str(pptr, role, bytes_left, &required);
        bytes_left = append_to_str(pptr, "|", bytes_left, &required);
    }

    ecs_entity_t e = id & ECS_COMPONENT_MASK;

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t lo = ECS_PAIR_OBJECT(id);
        ecs_entity_t hi = ECS_PAIR_RELATION(id);

        if (lo) lo = ecs_get_alive(world, lo);
        if (hi) hi = ecs_get_alive(world, hi);

        if (hi) {
            char *hi_path = ecs_get_fullpath(world, hi);
            bytes_left = append_to_str(pptr, "(", bytes_left, &required);
            bytes_left = append_to_str(pptr, hi_path, bytes_left, &required);
            ecs_os_free(hi_path);
            bytes_left = append_to_str(pptr, ",", bytes_left, &required);
        }

        char *lo_path = ecs_get_fullpath(world, lo);
        bytes_left = append_to_str(pptr, lo_path, bytes_left, &required);
        ecs_os_free(lo_path);

        if (hi) {
            append_to_str(pptr, ")", bytes_left, &required);
        }
    } else {
        char *path = ecs_get_fullpath(world, e);
        append_to_str(pptr, path, bytes_left, &required);
        ecs_os_free(path);
    }

    if (ptr) {
        ptr[0] = '\0';
    }
    
    return required;
}

static
void flush_bulk_new(
    ecs_world_t * world,
    ecs_op_t * op)
{
    ecs_entity_t *ids = op->is._n.entities;
    void **bulk_data = op->is._n.bulk_data;
    if (bulk_data) {
        ecs_entity_t *components = op->components.array;
        int c, c_count = op->components.count;
        for (c = 0; c < c_count; c ++) {
            ecs_entity_t component = components[c];
            const EcsComponent *cptr = flecs_component_from_id(world, component);
            ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);
            size_t size = flecs_to_size_t(cptr->size);
            void *ptr, *data = bulk_data[c];
            int i, count = op->is._n.count;
            for (i = 0, ptr = data; i < count; i ++, ptr = ECS_OFFSET(ptr, size)) {
                assign_ptr_w_id(world, ids[i], component, size, ptr, 
                    true, true);
            }
            ecs_os_free(data);
        }
        ecs_os_free(bulk_data);
    } else {
        int i, count = op->is._n.count;
        for (i = 0; i < count; i ++) {
            add_ids(world, ids[i], &op->components);
        }
    }

    if (op->components.count > 1) {
        ecs_os_free(op->components.array);
    }

    ecs_os_free(ids);
}

static
void free_value(
    ecs_world_t *world,
    ecs_entity_t *entities,
    ecs_id_t id,
    void *value,
    int32_t count)
{
    ecs_entity_t real_id = ecs_get_typeid(world, id);
    const ecs_type_info_t *info = flecs_get_c_info(world, real_id);
    ecs_xtor_t dtor;
    
    if (info && (dtor = info->lifecycle.dtor)) {
        ecs_size_t size = info->size;
        void *ptr;
        int i;
        for (i = 0, ptr = value; i < count; i ++, ptr = ECS_OFFSET(ptr, size)) {
            dtor(world, id, &entities[i], ptr, flecs_to_size_t(size), 1, 
                info->lifecycle.ctx);
        }
    }
}

static
void discard_op(
    ecs_world_t *world,
    ecs_op_t * op)
{
    if (op->kind == EcsOpBulkNew) {
        void **bulk_data = op->is._n.bulk_data;
        if (bulk_data) {
            ecs_entity_t *entities = op->is._n.entities;
            ecs_entity_t *components = op->components.array;
            int c, c_count = op->components.count;
            for (c = 0; c < c_count; c ++) {
                free_value(world, entities, components[c], bulk_data[c], 
                    op->is._n.count);
                ecs_os_free(bulk_data[c]);
            }
        }
    } else {
        void *value = op->is._1.value;
        if (value) {
            free_value(world, &op->is._1.entity, op->component, op->is._1.value, 1);
            ecs_os_free(value);
        }
    }

    ecs_entity_t *components = op->components.array;
    if (components) {
        ecs_os_free(components);
    }    
}

static 
bool is_entity_valid(
    ecs_world_t *world,
    ecs_entity_t e)
{
    if (ecs_exists(world, e) && !ecs_is_alive(world, e)) {
        return false;
    }
    return true;
}

static
bool remove_invalid(
    ecs_world_t * world,
    ecs_ids_t * ids)
{
    ecs_entity_t *array = ids->array;
    int32_t i, offset = 0, count = ids->count;

    for (i = 0; i < count; i ++) {
        ecs_id_t id = array[i];
        bool is_remove = false;

        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_entity_t rel = ecs_pair_relation(world, id);
            if (!rel || !is_entity_valid(world, rel)) {
                /* After relation is deleted we can no longer see what its
                 * delete action was, so pretend this never happened */
                is_remove = true;
            } else {
                ecs_entity_t obj = ecs_pair_object(world, id);
                if (!obj || !is_entity_valid(world, obj)) {
                    /* Check the relation's policy for deleted objects */
                    ecs_id_record_t *idr = flecs_get_id_record(world, rel);
                    if (!idr || (idr->on_delete_object == EcsRemove)) {
                        is_remove = true;
                    } else {
                        if (idr->on_delete_object == EcsDelete) {
                            /* Entity should be deleted, don't bother checking
                             * other ids */
                            return false;
                        } else if (idr->on_delete_object == EcsThrow) {
                            /* If policy is throw this object should not have
                             * been deleted */
                            throw_invalid_delete(world, id);
                        }
                    }
                }
            }

        } else {
            id &= ECS_COMPONENT_MASK;

            if (!is_entity_valid(world, id)) {
                /* After relation is deleted we can no longer see what its
                 * delete action was, so pretend this never happened */
                is_remove = true;
            }
        }

        if (is_remove) {
            offset ++;
            count --;
        }

        ids->array[i] = ids->array[i + offset];
    }

    ids->count = count;

    return true;
}

/* Leave safe section. Run all deferred commands. */
bool flecs_defer_flush(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!--stage->defer) {
        /* Set to NULL. Processing deferred commands can cause additional
         * commands to get enqueued (as result of reactive systems). Make sure
         * that the original array is not reallocated, as this would complicate
         * processing the queue. */
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;

        if (defer_queue) {
            ecs_op_t *ops = ecs_vector_first(defer_queue, ecs_op_t);
            int32_t i, count = ecs_vector_count(defer_queue);
            
            for (i = 0; i < count; i ++) {
                ecs_op_t *op = &ops[i];
                ecs_entity_t e = op->is._1.entity;
                if (op->kind == EcsOpBulkNew) {
                    e = 0;
                }

                /* If entity is no longer alive, this could be because the queue
                 * contained both a delete and a subsequent add/remove/set which
                 * should be ignored. */
                if (e && !ecs_is_alive(world, e) && ecs_eis_exists(world, e)) {
                    ecs_assert(op->kind != EcsOpNew && op->kind != EcsOpClone, 
                        ECS_INTERNAL_ERROR, NULL);
                    world->discard_count ++;
                    discard_op(world, op);
                    continue;
                }

                if (op->components.count == 1) {
                    op->components.array = &op->component;
                }

                switch(op->kind) {
                case EcsOpNew:
                case EcsOpAdd:
                    if (remove_invalid(world, &op->components)) {
                        world->add_count ++;
                        add_ids(world, e, &op->components);
                    } else {
                        ecs_delete(world, e);
                    }
                    break;
                case EcsOpRemove:
                    remove_ids(world, e, &op->components);
                    break;
                case EcsOpClone:
                    ecs_clone(world, e, op->component, op->is._1.clone_value);
                    break;
                case EcsOpSet:
                    assign_ptr_w_id(world, e, 
                        op->component, flecs_to_size_t(op->is._1.size), 
                        op->is._1.value, true, true);
                    break;
                case EcsOpMut:
                    assign_ptr_w_id(world, e, 
                        op->component, flecs_to_size_t(op->is._1.size), 
                        op->is._1.value, true, false);
                    break;
                case EcsOpModified:
                    ecs_modified_id(world, e, op->component);
                    break;
                case EcsOpDelete: {
                    ecs_delete(world, e);
                    break;
                }
                case EcsOpEnable:
                    ecs_enable_component_w_id(
                        world, e, op->component, true);
                    break;
                case EcsOpDisable:
                    ecs_enable_component_w_id(
                        world, e, op->component, false);
                    break;
                case EcsOpClear:
                    ecs_clear(world, e);
                    break;
                case EcsOpBulkNew:
                    flush_bulk_new(world, op);

                    /* Continue since flush_bulk_new is repsonsible for cleaning
                    * up resources. */
                    continue;
                }

                if (op->components.count > 1) {
                    ecs_os_free(op->components.array);
                }

                if (op->is._1.value) {
                    ecs_os_free(op->is._1.value);
                }                  
            }

            if (stage->defer_queue) {
                ecs_vector_free(stage->defer_queue);
            }

            /* Restore defer queue */
            ecs_vector_clear(defer_queue);
            stage->defer_queue = defer_queue;
        }

        return true;
    }

    return false;
}

/* Delete operations from queue without executing them. */
bool flecs_defer_purge(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(stage != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!--stage->defer) {
        ecs_vector_t *defer_queue = stage->defer_queue;
        stage->defer_queue = NULL;

        if (defer_queue) {
            ecs_op_t *ops = ecs_vector_first(defer_queue, ecs_op_t);
            int32_t i, count = ecs_vector_count(defer_queue);
            for (i = 0; i < count; i ++) {
                discard_op(world, &ops[i]);
            }

            if (stage->defer_queue) {
                ecs_vector_free(stage->defer_queue);
            }

            /* Restore defer queue */
            ecs_vector_clear(defer_queue);
            stage->defer_queue = defer_queue;
        }

        return true;
    }

    return false;
}

static
ecs_op_t* new_defer_op(ecs_stage_t *stage) {
    ecs_op_t *result = ecs_vector_add(&stage->defer_queue, ecs_op_t);
    ecs_os_memset(result, 0, ECS_SIZEOF(ecs_op_t));
    return result;
}

static 
void new_defer_component_ids(
    ecs_op_t *op, 
    const ecs_ids_t *components)
{
    ecs_assert(components != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t components_count = components->count;
    if (components_count == 1) {
        ecs_entity_t component = components->array[0];
        op->component = component;
        op->components = (ecs_ids_t) {
            .array = NULL,
            .count = 1
        };
    } else if (components_count) {
        ecs_size_t array_size = components_count * ECS_SIZEOF(ecs_entity_t);
        op->components.array = ecs_os_malloc(array_size);
        ecs_os_memcpy(op->components.array, components->array, array_size);
        op->components.count = components_count;
    } else {
        op->component = 0;
        op->components = (ecs_ids_t){ 0 };
    }
}

static
bool defer_add_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_ids_t *components)
{
    if (stage->defer) {
        if (components) {
            if (!components->count) {
                return true;
            }
        }

        ecs_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->is._1.entity = entity;

        new_defer_component_ids(op, components);

        if (op_kind == EcsOpNew) {
            world->new_count ++;
        } else if (op_kind == EcsOpAdd) {
            world->add_count ++;
        } else if (op_kind == EcsOpRemove) {
            world->remove_count ++;
        }

        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

static
void merge_stages(
    ecs_world_t *world,
    bool force_merge)
{
    bool is_stage = world->magic == ECS_STAGE_MAGIC;
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    bool measure_frame_time = world->measure_frame_time;

    ecs_time_t t_start;
    if (measure_frame_time) {
        ecs_os_get_time(&t_start);
    }

    if (is_stage) {
        /* Check for consistency if force_merge is enabled. In practice this
         * function will never get called with force_merge disabled for just
         * a single stage. */
        if (force_merge || stage->auto_merge) {
            ecs_defer_end((ecs_world_t*)stage);
        }
    } else {
        /* Merge stages. Only merge if the stage has auto_merging turned on, or 
         * if this is a forced merge (like when ecs_merge is called) */
        int32_t i, count = ecs_get_stage_count(world);
        for (i = 0; i < count; i ++) {
            ecs_stage_t *s = (ecs_stage_t*)ecs_get_stage(world, i);
            ecs_assert(s->magic == ECS_STAGE_MAGIC, ECS_INTERNAL_ERROR, NULL);
            if (force_merge || s->auto_merge) {
                ecs_defer_end((ecs_world_t*)s);
            }
        }
    }

    flecs_eval_component_monitors(world);

    if (measure_frame_time) {
        world->stats.merge_time_total += 
            (FLECS_FLOAT)ecs_time_measure(&t_start);
    }

    world->stats.merge_count_total ++; 

    /* If stage is asynchronous, deferring is always enabled */
    if (stage->asynchronous) {
        ecs_defer_begin((ecs_world_t*)stage);
    }
}

static
void do_auto_merge(
    ecs_world_t *world)
{
    merge_stages(world, false);
}

static
void do_manual_merge(
    ecs_world_t *world)
{
    merge_stages(world, true);
}

bool flecs_defer_none(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    return (++ stage->defer) == 1;
}

bool flecs_defer_modified(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpModified;
        op->component = component;
        op->is._1.entity = entity;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;
}

bool flecs_defer_clone(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t src,
    bool clone_value)
{   
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClone;
        op->component = src;
        op->is._1.entity = entity;
        op->is._1.clone_value = clone_value;
        return true;
    } else {
        stage->defer ++;
    }
    
    return false;   
}

bool flecs_defer_delete(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpDelete;
        op->is._1.entity = entity;
        world->delete_count ++;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool flecs_defer_clear(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpClear;
        op->is._1.entity = entity;
        world->clear_count ++;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool flecs_defer_enable(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_entity_t component,
    bool enable)
{
    (void)world;
    if (stage->defer) {
        ecs_op_t *op = new_defer_op(stage);
        op->kind = enable ? EcsOpEnable : EcsOpDisable;
        op->is._1.entity = entity;
        op->component = component;
        return true;
    } else {
        stage->defer ++;
    }
    return false;
}

bool flecs_defer_bulk_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    int32_t count,
    const ecs_ids_t *components_ids,
    void **component_data,
    const ecs_entity_t **ids_out)
{
    if (stage->defer) {
        ecs_entity_t *ids = ecs_os_malloc(count * ECS_SIZEOF(ecs_entity_t));
        void **defer_data = NULL;

        world->bulk_new_count ++;

        /* Use ecs_new_id as this is thread safe */
        int i;
        for (i = 0; i < count; i ++) {
            ids[i] = ecs_new_id(world);
        }

        /* Create private copy for component data */
        if (component_data) {
            int c, c_count = components_ids->count;
            ecs_entity_t *components = components_ids->array;
            defer_data = ecs_os_malloc(ECS_SIZEOF(void*) * c_count);
            for (c = 0; c < c_count; c ++) {
                ecs_entity_t comp = components[c];
                const EcsComponent *cptr = flecs_component_from_id(world, comp);
                ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);

                ecs_size_t size = cptr->size;
                void *data = ecs_os_malloc(size * count);
                defer_data[c] = data;

                const ecs_type_info_t *cinfo = NULL;
                ecs_entity_t real_id = ecs_get_typeid(world, comp);
                if (real_id) {
                    cinfo = flecs_get_c_info(world, real_id);
                }
                ecs_xtor_t ctor;
                if (cinfo && (ctor = cinfo->lifecycle.ctor)) {
                    void *ctx = cinfo->lifecycle.ctx;
                    ctor(world, comp, ids, data, flecs_to_size_t(size), count, ctx);
                    ecs_move_t move;
                    if ((move = cinfo->lifecycle.move)) {
                        move(world, comp, ids, ids, data, component_data[c], 
                            flecs_to_size_t(size), count, ctx);
                    } else {
                        ecs_os_memcpy(data, component_data[c], size * count);
                    }
                } else {
                    ecs_os_memcpy(data, component_data[c], size * count);
                }
            }
        }

        /* Store data in op */
        ecs_op_t *op = new_defer_op(stage);
        op->kind = EcsOpBulkNew;
        op->is._n.entities = ids;
        op->is._n.bulk_data = defer_data;
        op->is._n.count = count;
        new_defer_component_ids(op, components_ids);
        *ids_out = ids;

        return true;
    } else {
        stage->defer ++;
    }

    return false;
}

bool flecs_defer_new(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components)
{   
    return defer_add_remove(world, stage, EcsOpNew, entity, components);
}

bool flecs_defer_add(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components)
{   
    return defer_add_remove(world, stage, EcsOpAdd, entity, components);
}

bool flecs_defer_remove(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t entity,
    ecs_ids_t *components)
{
    return defer_add_remove(world, stage, EcsOpRemove, entity, components);
}

bool flecs_defer_set(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_op_kind_t op_kind,
    ecs_entity_t entity,
    ecs_entity_t component,
    ecs_size_t size,
    const void *value,
    void **value_out,
    bool *is_added)
{
    if (stage->defer) {
        world->set_count ++;
        if (!size) {
            const EcsComponent *cptr = flecs_component_from_id(world, component);
            ecs_assert(cptr != NULL, ECS_INVALID_PARAMETER, NULL);
            size = cptr->size;
        }

        ecs_op_t *op = new_defer_op(stage);
        op->kind = op_kind;
        op->component = component;
        op->is._1.entity = entity;
        op->is._1.size = size;
        op->is._1.value = ecs_os_malloc(size);

        if (!value) {
            value = ecs_get_id(world, entity, component);
            if (is_added) {
                *is_added = value == NULL;
            }
        }

        const ecs_type_info_t *c_info = NULL;
        ecs_entity_t real_id = ecs_get_typeid(world, component);
        if (real_id) {
            c_info = flecs_get_c_info(world, real_id);
        }

        if (value) {
            ecs_copy_ctor_t copy;
            if (c_info && (copy = c_info->lifecycle.copy_ctor)) {
                copy(world, component, &c_info->lifecycle, &entity, &entity, 
                    op->is._1.value, value, flecs_to_size_t(size), 1, 
                        c_info->lifecycle.ctx);
            } else {
                ecs_os_memcpy(op->is._1.value, value, size);
            }
        } else {
            ecs_xtor_t ctor;
            if (c_info && (ctor = c_info->lifecycle.ctor)) {
                ctor(world, component, &entity, op->is._1.value, 
                    flecs_to_size_t(size), 1, c_info->lifecycle.ctx);
            }
        }

        if (value_out) {
            *value_out = op->is._1.value;
        }

        return true;
    } else {
        stage->defer ++;
    }

    return false;
}

void flecs_stage_merge_post_frame(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    /* Execute post frame actions */
    ecs_vector_each(stage->post_frame_actions, ecs_action_elem_t, action, {
        action->action(world, action->ctx);
    });

    ecs_vector_free(stage->post_frame_actions);
    stage->post_frame_actions = NULL;
}

void flecs_stage_init(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    memset(stage, 0, sizeof(ecs_stage_t));

    stage->magic = ECS_STAGE_MAGIC;
    stage->world = world;
    stage->thread_ctx = world;
    stage->auto_merge = true;
    stage->asynchronous = false;
}

void flecs_stage_deinit(
    ecs_world_t *world,
    ecs_stage_t *stage)
{
    (void)world;
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(stage->magic == ECS_STAGE_MAGIC, ECS_INVALID_PARAMETER, NULL);

    /* Make sure stage has no unmerged data */
    ecs_assert(ecs_vector_count(stage->defer_queue) == 0, 
        ECS_INVALID_PARAMETER, NULL);

    /* Set magic to 0 so that accessing the stage after deinitializing it will
     * throw an assert. */
    stage->magic = 0;

    ecs_vector_free(stage->defer_queue);
}

void ecs_set_stages(
    ecs_world_t *world,
    int32_t stage_count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stages;
    int32_t i, count = ecs_vector_count(world->worker_stages);

    if (count && count != stage_count) {
        stages = ecs_vector_first(world->worker_stages, ecs_stage_t);

        for (i = 0; i < count; i ++) {
            /* If stage contains a thread handle, ecs_set_threads was used to
             * create the stages. ecs_set_threads and ecs_set_stages should not
             * be mixed. */
            ecs_assert(stages[i].magic == ECS_STAGE_MAGIC, 
                ECS_INTERNAL_ERROR, NULL);
            ecs_assert(stages[i].thread == 0, ECS_INVALID_OPERATION, NULL);
            flecs_stage_deinit(world, &stages[i]);
        }

        ecs_vector_free(world->worker_stages);
    }
    
    if (stage_count) {
        world->worker_stages = ecs_vector_new(ecs_stage_t, stage_count);

        for (i = 0; i < stage_count; i ++) {
            ecs_stage_t *stage = ecs_vector_add(
                &world->worker_stages, ecs_stage_t);
            flecs_stage_init(world, stage);
            stage->id = 1 + i; /* 0 is reserved for main/temp stage */

            /* Set thread_ctx to stage, as this stage might be used in a
             * multithreaded context */
            stage->thread_ctx = (ecs_world_t*)stage;
        }
    } else {
        /* Set to NULL to prevent double frees */
        world->worker_stages = NULL;
    }

    /* Regardless of whether the stage was just initialized or not, when the
     * ecs_set_stages function is called, all stages inherit the auto_merge
     * property from the world */
    for (i = 0; i < stage_count; i ++) {
        ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
        stage->auto_merge = world->stage.auto_merge;
    }
}

int32_t ecs_get_stage_count(
    const ecs_world_t *world)
{
    world = ecs_get_world(world);
    return ecs_vector_count(world->worker_stages);
}

int32_t ecs_get_stage_id(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (world->magic == ECS_STAGE_MAGIC) {
        ecs_stage_t *stage = (ecs_stage_t*)world;

        /* Index 0 is reserved for main stage */
        return stage->id - 1;
    } else if (world->magic == ECS_WORLD_MAGIC) {
        return 0;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

ecs_world_t* ecs_get_stage(
    const ecs_world_t *world,
    int32_t stage_id)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vector_count(world->worker_stages) > stage_id, 
        ECS_INVALID_PARAMETER, NULL);

    return (ecs_world_t*)ecs_vector_get(
        world->worker_stages, ecs_stage_t, stage_id);
}

bool ecs_staging_begin(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    int32_t i, count = ecs_get_stage_count(world);
    for (i = 0; i < count; i ++) {
        ecs_defer_begin(ecs_get_stage(world, i));
    }

    bool is_readonly = world->is_readonly;

    /* From this point on, the world is "locked" for mutations, and it is only 
     * allowed to enqueue commands from stages */
    world->is_readonly = true;

    return is_readonly;
}

void ecs_staging_end(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->is_readonly == true, ECS_INVALID_OPERATION, NULL);

    /* After this it is safe again to mutate the world directly */
    world->is_readonly = false;

    do_auto_merge(world);
}

void ecs_merge(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC || 
               world->magic == ECS_STAGE_MAGIC, ECS_INVALID_PARAMETER, NULL);
    do_manual_merge(world);
}

void ecs_set_automerge(
    ecs_world_t *world,
    bool auto_merge)
{
    /* If a world is provided, set auto_merge globally for the world. This
     * doesn't actually do anything (the main stage never merges) but it serves
     * as the default for when stages are created. */
    if (world->magic == ECS_WORLD_MAGIC) {
        world->stage.auto_merge = auto_merge;

        /* Propagate change to all stages */
        int i, stage_count = ecs_get_stage_count(world);
        for (i = 0; i < stage_count; i ++) {
            ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
            stage->auto_merge = auto_merge;
        }

    /* If a stage is provided, override the auto_merge value for the individual
     * stage. This allows an application to control per-stage which stage should
     * be automatically merged and which one shouldn't */
    } else {
        /* Magic needs to be either a world or a stage */
        ecs_assert(world->magic == ECS_STAGE_MAGIC, 
            ECS_INVALID_FROM_WORKER, NULL);

        ecs_stage_t *stage = (ecs_stage_t*)world;
        stage->auto_merge = auto_merge;
    }
}

bool ecs_stage_is_readonly(
    const ecs_world_t *stage)
{
    const ecs_world_t *world = ecs_get_world(stage);

    if (stage->magic == ECS_STAGE_MAGIC) {
        if (((ecs_stage_t*)stage)->asynchronous) {
            return false;
        }
    }

    if (world->is_readonly) {
        if (stage->magic == ECS_WORLD_MAGIC) {
            return true;
        }
    } else {
        if (stage->magic == ECS_STAGE_MAGIC) {
            return true;
        }
    }

    return false;
}

ecs_world_t* ecs_async_stage_new(
    ecs_world_t *world)
{
    ecs_stage_t *stage = ecs_os_calloc(sizeof(ecs_stage_t));
    flecs_stage_init(world, stage);

    stage->id = -1;
    stage->auto_merge = false;
    stage->asynchronous = true;

    ecs_defer_begin((ecs_world_t*)stage);

    return (ecs_world_t*)stage;
}

void ecs_async_stage_free(
    ecs_world_t *world)
{
    ecs_assert(world->magic == ECS_STAGE_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = (ecs_stage_t*)world;
    ecs_assert(stage->asynchronous == true, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_deinit(stage->world, stage);
    ecs_os_free(stage);
}

bool ecs_stage_is_async(
    ecs_world_t *stage)
{
    if (!stage) {
        return false;
    }
    
    if (stage->magic != ECS_STAGE_MAGIC) {
        return false;
    }

    return ((ecs_stage_t*)stage)->asynchronous;
}

bool ecs_is_deferred(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->defer != 0;
}

/** Resize the vector buffer */
static
ecs_vector_t* resize(
    ecs_vector_t *vector,
    int16_t offset,
    int32_t size)
{
    ecs_vector_t *result = ecs_os_realloc(vector, offset + size);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, 0);
    return result;
}

/* -- Public functions -- */

ecs_vector_t* _ecs_vector_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result =
        ecs_os_malloc(offset + elem_size * elem_count);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->count = 0;
    result->size = elem_count;
#ifndef NDEBUG
    result->elem_size = elem_size;
#endif
    return result;
}

ecs_vector_t* _ecs_vector_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array)
{
    ecs_assert(elem_size != 0, ECS_INTERNAL_ERROR, NULL);
    
    ecs_vector_t *result =
        ecs_os_malloc(offset + elem_size * elem_count);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    ecs_os_memcpy(ECS_OFFSET(result, offset), array, elem_size * elem_count);

    result->count = elem_count;
    result->size = elem_count;
#ifndef NDEBUG
    result->elem_size = elem_size;
#endif
    return result;   
}

void ecs_vector_free(
    ecs_vector_t *vector)
{
    ecs_os_free(vector);
}

void ecs_vector_clear(
    ecs_vector_t *vector)
{
    if (vector) {
        vector->count = 0;
    }
}

void _ecs_vector_zero(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    void *array = ECS_OFFSET(vector, offset);
    ecs_os_memset(array, 0, elem_size * vector->count);
}

void ecs_vector_assert_size(
    ecs_vector_t *vector,
    ecs_size_t elem_size)
{
    (void)elem_size;
    
    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    }
}

void* _ecs_vector_addn(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_assert(array_inout != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (elem_count == 1) {
        return _ecs_vector_add(array_inout, elem_size, offset);
    }
    
    ecs_vector_t *vector = *array_inout;
    if (!vector) {
        vector = _ecs_vector_new(elem_size, offset, 1);
        *array_inout = vector;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    int32_t max_count = vector->size;
    int32_t old_count = vector->count;
    int32_t new_count = old_count + elem_count;

    if ((new_count - 1) >= max_count) {
        if (!max_count) {
            max_count = elem_count;
        } else {
            while (max_count < new_count) {
                max_count *= 2;
            }
        }

        vector = resize(vector, offset, max_count * elem_size);
        vector->size = max_count;
        *array_inout = vector;
    }

    vector->count = new_count;

    return ECS_OFFSET(vector, offset + elem_size * old_count);
}

void* _ecs_vector_add(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset)
{
    ecs_vector_t *vector = *array_inout;
    int32_t count, size;

    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
        count = vector->count;
        size = vector->size;

        if (count >= size) {
            size *= 2;
            if (!size) {
                size = 2;
            }
            vector = resize(vector, offset, size * elem_size);
            *array_inout = vector;
            vector->size = size;
        }

        vector->count = count + 1;
        return ECS_OFFSET(vector, offset + elem_size * count);
    }

    vector = _ecs_vector_new(elem_size, offset, 2);
    *array_inout = vector;
    vector->count = 1;
    vector->size = 2;
    return ECS_OFFSET(vector, offset);
}

int32_t _ecs_vector_move_index(
    ecs_vector_t **dst,
    ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_assert((*dst)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(src->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    void *dst_elem = _ecs_vector_add(dst, elem_size, offset);
    void *src_elem = _ecs_vector_get(src, elem_size, offset, index);

    ecs_os_memcpy(dst_elem, src_elem, elem_size);
    return _ecs_vector_remove(src, elem_size, offset, index);
}

void ecs_vector_remove_last(
    ecs_vector_t *vector)
{
    if (vector && vector->count) vector->count --;
}

bool _ecs_vector_pop(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    void *value)
{
    if (!vector) {
        return false;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    int32_t count = vector->count;
    if (!count) {
        return false;
    }

    void *elem = ECS_OFFSET(vector, offset + (count - 1) * elem_size);

    if (value) {
        ecs_os_memcpy(value, elem, elem_size);
    }

    ecs_vector_remove_last(vector);

    return true;
}

int32_t _ecs_vector_remove(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);
    void *elem = ECS_OFFSET(buffer, index * elem_size);

    ecs_assert(index < count, ECS_INVALID_PARAMETER, NULL);

    count --;
    if (index != count) {
        void *last_elem = ECS_OFFSET(buffer, elem_size * count);
        ecs_os_memcpy(elem, last_elem, elem_size);
    }

    vector->count = count;

    return count;
}

void _ecs_vector_reclaim(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset)
{
    ecs_vector_t *vector = *array_inout;

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    
    int32_t size = vector->size;
    int32_t count = vector->count;

    if (count < size) {
        size = count;
        vector = resize(vector, offset, size * elem_size);
        vector->size = size;
        *array_inout = vector;
    }
}

int32_t ecs_vector_count(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->count;
}

int32_t ecs_vector_size(
    const ecs_vector_t *vector)
{
    if (!vector) {
        return 0;
    }
    return vector->size;
}

int32_t _ecs_vector_set_size(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_vector_t *vector = *array_inout;

    if (!vector) {
        *array_inout = _ecs_vector_new(elem_size, offset, elem_count);
        return elem_count;
    } else {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

        int32_t result = vector->size;

        if (elem_count < vector->count) {
            elem_count = vector->count;
        }

        if (result < elem_count) {
            elem_count = flecs_next_pow_of_2(elem_count);
            vector = resize(vector, offset, elem_count * elem_size);
            vector->size = elem_count;
            *array_inout = vector;
            result = elem_count;
        }

        return result;
    }
}

int32_t _ecs_vector_grow(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    int32_t current = ecs_vector_count(*array_inout);
    return _ecs_vector_set_size(array_inout, elem_size, offset, current + elem_count);
}

int32_t _ecs_vector_set_count(
    ecs_vector_t **array_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    if (!*array_inout) {
        *array_inout = _ecs_vector_new(elem_size, offset, elem_count);
    }

    ecs_assert((*array_inout)->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    (*array_inout)->count = elem_count;
    ecs_size_t size = _ecs_vector_set_size(array_inout, elem_size, offset, elem_count);
    return size;
}

void* _ecs_vector_first(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    (void)elem_size;

    ecs_assert(!vector || vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
    if (vector && vector->size) {
        return ECS_OFFSET(vector, offset);
    } else {
        return NULL;
    }
}

void* _ecs_vector_get(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    if (!vector) {
        return NULL;
    }
    
    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);

    int32_t count = vector->count;

    if (index >= count) {
        return NULL;
    }

    return ECS_OFFSET(vector, offset + elem_size * index);
}

void* _ecs_vector_last(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset)
{
    if (vector) {
        ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);
        int32_t count = vector->count;
        if (!count) {
            return NULL;
        } else {
            return ECS_OFFSET(vector, offset + elem_size * (count - 1));
        }
    } else {
        return NULL;
    }
}

int32_t _ecs_vector_set_min_size(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    if (!*vector_inout || (*vector_inout)->size < elem_count) {
        return _ecs_vector_set_size(vector_inout, elem_size, offset, elem_count);
    } else {
        return (*vector_inout)->size;
    }
}

int32_t _ecs_vector_set_min_count(
    ecs_vector_t **vector_inout,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    _ecs_vector_set_min_size(vector_inout, elem_size, offset, elem_count);

    ecs_vector_t *v = *vector_inout;
    if (v && v->count < elem_count) {
        v->count = elem_count;
    }

    return v->count;
}

void _ecs_vector_sort(
    ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    ecs_comparator_t compare_action)
{
    if (!vector) {
        return;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);    

    int32_t count = vector->count;
    void *buffer = ECS_OFFSET(vector, offset);

    if (count > 1) {
        qsort(buffer, (size_t)count, (size_t)elem_size, compare_action);
    }
}

void _ecs_vector_memory(
    const ecs_vector_t *vector,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t *allocd,
    int32_t *used)
{
    if (!vector) {
        return;
    }

    ecs_assert(vector->elem_size == elem_size, ECS_INTERNAL_ERROR, NULL);

    if (allocd) {
        *allocd += vector->size * elem_size + offset;
    }
    if (used) {
        *used += vector->count * elem_size;
    }
}

ecs_vector_t* _ecs_vector_copy(
    const ecs_vector_t *src,
    ecs_size_t elem_size,
    int16_t offset)
{
    if (!src) {
        return NULL;
    }

    ecs_vector_t *dst = _ecs_vector_new(elem_size, offset, src->size);
    ecs_os_memcpy(dst, src, offset + elem_size * src->count);
    return dst;
}

/** The number of elements in a single chunk */
#define CHUNK_COUNT (4096)

/** Compute the chunk index from an id by stripping the first 12 bits */
#define CHUNK(index) ((int32_t)((uint32_t)index >> 12))

/** This computes the offset of an index inside a chunk */
#define OFFSET(index) ((int32_t)index & 0xFFF)

/* Utility to get a pointer to the payload */
#define DATA(array, size, offset) (ECS_OFFSET(array, size * offset))

typedef struct chunk_t {
    int32_t *sparse;            /* Sparse array with indices to dense array */
    void *data;                 /* Store data in sparse array to reduce  
                                 * indirection and provide stable pointers. */
} chunk_t;

struct ecs_sparse_t {
    ecs_vector_t *dense;        /* Dense array with indices to sparse array. The
                                 * dense array stores both alive and not alive
                                 * sparse indices. The 'count' member keeps
                                 * track of which indices are alive. */

    ecs_vector_t *chunks;       /* Chunks with sparse arrays & data */
    ecs_size_t size;            /* Element size */
    int32_t count;              /* Number of alive entries */
    uint64_t max_id_local;      /* Local max index (if no global is set) */
    uint64_t *max_id;           /* Maximum issued sparse index */
};

static
chunk_t* chunk_new(
    ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    int32_t count = ecs_vector_count(sparse->chunks);
    chunk_t *chunks;

    if (count <= chunk_index) {
        ecs_vector_set_count(&sparse->chunks, chunk_t, chunk_index + 1);
        chunks = ecs_vector_first(sparse->chunks, chunk_t);
        ecs_os_memset(&chunks[count], 0, (1 + chunk_index - count) * ECS_SIZEOF(chunk_t));
    } else {
        chunks = ecs_vector_first(sparse->chunks, chunk_t);
    }

    ecs_assert(chunks != NULL, ECS_INTERNAL_ERROR, NULL);

    chunk_t *result = &chunks[chunk_index];
    ecs_assert(result->sparse == NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data == NULL, ECS_INTERNAL_ERROR, NULL);

    /* Initialize sparse array with zero's, as zero is used to indicate that the
     * sparse element has not been paired with a dense element. Use zero
     * as this means we can take advantage of calloc having a possibly better 
     * performance than malloc + memset. */
    result->sparse = ecs_os_calloc(ECS_SIZEOF(int32_t) * CHUNK_COUNT);

    /* Initialize the data array with zero's to guarantee that data is 
     * always initialized. When an entry is removed, data is reset back to
     * zero. Initialize now, as this can take advantage of calloc. */
    result->data = ecs_os_calloc(sparse->size * CHUNK_COUNT);

    ecs_assert(result->sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result->data != NULL, ECS_INTERNAL_ERROR, NULL);

    return result;
}

static
void chunk_free(
    chunk_t *chunk)
{
    ecs_os_free(chunk->sparse);
    ecs_os_free(chunk->data);
}

static
chunk_t* get_chunk(
    const ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    /* If chunk_index is below zero, application used an invalid entity id */
    ecs_assert(chunk_index >= 0, ECS_INVALID_PARAMETER, NULL);
    chunk_t *result = ecs_vector_get(sparse->chunks, chunk_t, chunk_index);
    if (result && !result->sparse) {
        return NULL;
    }

    return result;
}

static
chunk_t* get_or_create_chunk(
    ecs_sparse_t *sparse,
    int32_t chunk_index)
{
    chunk_t *chunk = get_chunk(sparse, chunk_index);
    if (chunk) {
        return chunk;
    }

    return chunk_new(sparse, chunk_index);
}

static
void grow_dense(
    ecs_sparse_t *sparse)
{
    ecs_vector_add(&sparse->dense, uint64_t);
}

static
uint64_t strip_generation(
    uint64_t *index_out)
{
    uint64_t index = *index_out;
    uint64_t gen = index & ECS_GENERATION_MASK;
    /* Make sure there's no junk in the id */
    ecs_assert(gen == (index & (0xFFFFFFFFull << 32)),
        ECS_INVALID_PARAMETER, NULL);
    *index_out -= gen;
    return gen;
}

static
void assign_index(
    chunk_t * chunk, 
    uint64_t * dense_array, 
    uint64_t index, 
    int32_t dense)
{
    /* Initialize sparse-dense pair. This assigns the dense index to the sparse
     * array, and the sparse index to the dense array .*/
    chunk->sparse[OFFSET(index)] = dense;
    dense_array[dense] = index;
}

static
uint64_t inc_gen(
    uint64_t index)
{
    /* When an index is deleted, its generation is increased so that we can do
     * liveliness checking while recycling ids */
    return ECS_GENERATION_INC(index);
}

static
uint64_t inc_id(
    ecs_sparse_t *sparse)
{
    /* Generate a new id. The last issued id could be stored in an external
     * variable, such as is the case with the last issued entity id, which is
     * stored on the world. */
    return ++ (sparse->max_id[0]);
}

static
uint64_t get_id(
    const ecs_sparse_t *sparse)
{
    return sparse->max_id[0];
}

static
void set_id(
    ecs_sparse_t *sparse,
    uint64_t value)
{
    /* Sometimes the max id needs to be assigned directly, which typically 
     * happens when the API calls get_or_create for an id that hasn't been 
     * issued before. */
    sparse->max_id[0] = value;
}

/* Pair dense id with new sparse id */
static
uint64_t create_id(
    ecs_sparse_t *sparse,
    int32_t dense)
{
    uint64_t index = inc_id(sparse);
    grow_dense(sparse);

    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    ecs_assert(chunk->sparse[OFFSET(index)] == 0, ECS_INTERNAL_ERROR, NULL);
    
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    assign_index(chunk, dense_array, index, dense);
    
    return index;
}

/* Create new id */
static
uint64_t new_index(
    ecs_sparse_t *sparse)
{
    ecs_vector_t *dense = sparse->dense;
    int32_t dense_count = ecs_vector_count(dense);
    int32_t count = sparse->count ++;

    ecs_assert(count <= dense_count, ECS_INTERNAL_ERROR, NULL);

    if (count < dense_count) {
        /* If there are unused elements in the dense array, return first */
        uint64_t *dense_array = ecs_vector_first(dense, uint64_t);
        return dense_array[count];
    } else {
        return create_id(sparse, count);
    }
}

/* Try obtaining a value from the sparse set, don't care about whether the
 * provided index matches the current generation count.  */
static
void* try_sparse_any(
    const ecs_sparse_t *sparse,
    uint64_t index)
{    
    strip_generation(&index);

    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, sparse->size, offset);
}

/* Try obtaining a value from the sparse set, make sure it's alive. */
static
void* try_sparse(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return NULL;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    bool in_use = dense && (dense < sparse->count);
    if (!in_use) {
        return NULL;
    }

    uint64_t gen = strip_generation(&index);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;

    if (cur_gen != gen) {
        return NULL;
    }

    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, sparse->size, offset);
}

/* Get value from sparse set when it is guaranteed that the value exists. This
 * function is used when values are obtained using a dense index */
static
void* get_sparse(
    const ecs_sparse_t *sparse,
    int32_t dense,
    uint64_t index)
{
    strip_generation(&index);
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    int32_t offset = OFFSET(index);
    
    ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(dense == chunk->sparse[offset], ECS_INTERNAL_ERROR, NULL);
    (void)dense;

    return DATA(chunk->data, sparse->size, offset);
}

/* Swap dense elements. A swap occurs when an element is removed, or when a
 * removed element is recycled. */
static
void swap_dense(
    ecs_sparse_t * sparse,
    chunk_t * chunk_a,
    int32_t a,
    int32_t b)
{
    ecs_assert(a != b, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    uint64_t index_a = dense_array[a];
    uint64_t index_b = dense_array[b];

    chunk_t *chunk_b = get_or_create_chunk(sparse, CHUNK(index_b));
    assign_index(chunk_a, dense_array, index_a, b);
    assign_index(chunk_b, dense_array, index_b, a);
}

ecs_sparse_t* _flecs_sparse_new(
    ecs_size_t size)
{
    ecs_sparse_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_sparse_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
    result->size = size;
    result->max_id_local = UINT64_MAX;
    result->max_id = &result->max_id_local;

    /* Consume first value in dense array as 0 is used in the sparse array to
     * indicate that a sparse element hasn't been paired yet. */
    uint64_t *first = ecs_vector_add(&result->dense, uint64_t);
    *first = 0;

    result->count = 1;

    return result;
}

void flecs_sparse_set_id_source(
    ecs_sparse_t * sparse,
    uint64_t * id_source)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    sparse->max_id = id_source;
}

void flecs_sparse_clear(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_vector_each(sparse->chunks, chunk_t, chunk, {
        chunk_free(chunk);
    });

    ecs_vector_free(sparse->chunks);
    ecs_vector_set_count(&sparse->dense, uint64_t, 1);

    sparse->chunks = NULL;   
    sparse->count = 1;
    sparse->max_id_local = 0;
}

void flecs_sparse_free(
    ecs_sparse_t *sparse)
{
    if (sparse) {
        flecs_sparse_clear(sparse);
        ecs_vector_free(sparse->dense);
        ecs_os_free(sparse);
    }
}

uint64_t flecs_sparse_new_id(
    ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return new_index(sparse);
}

const uint64_t* flecs_sparse_new_ids(
    ecs_sparse_t *sparse,
    int32_t new_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t dense_count = ecs_vector_count(sparse->dense);
    int32_t count = sparse->count;
    int32_t remaining = dense_count - count;
    int32_t i, to_create = new_count - remaining;

    if (to_create > 0) {
        flecs_sparse_set_size(sparse, dense_count + to_create);
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);

        for (i = 0; i < to_create; i ++) {
            uint64_t index = create_id(sparse, count + i);
            dense_array[dense_count + i] = index;
        }
    }

    sparse->count += new_count;

    return ecs_vector_get(sparse->dense, uint64_t, count);
}

void* _flecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t size)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    uint64_t index = new_index(sparse);
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    ecs_assert(chunk != NULL, ECS_INTERNAL_ERROR, NULL);
    return DATA(chunk->data, size, OFFSET(index));
}

uint64_t flecs_sparse_last_id(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INTERNAL_ERROR, NULL);
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    return dense_array[sparse->count - 1];
}

void* _flecs_sparse_ensure(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ecs_vector_count(sparse->dense) > 0, ECS_INTERNAL_ERROR, NULL);
    (void)size;

    uint64_t gen = strip_generation(&index);
    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        /* Check if element is alive. If element is not alive, update indices so
         * that the first unused dense element points to the sparse element. */
        int32_t count = sparse->count;
        if (dense == count) {
            /* If dense is the next unused element in the array, simply increase
             * the count to make it part of the alive set. */
            sparse->count ++;
        } else if (dense > count) {
            /* If dense is not alive, swap it with the first unused element. */
            swap_dense(sparse, chunk, dense, count);

            /* First unused element is now last used element */
            sparse->count ++;
        } else {
            /* Dense is already alive, nothing to be done */
        }

        /* Ensure provided generation matches current. Only allow mismatching
         * generations if the provided generation count is 0. This allows for
         * using the ensure function in combination with ids that have their
         * generation stripped. */
        ecs_vector_t *dense_vector = sparse->dense;
        uint64_t *dense_array = ecs_vector_first(dense_vector, uint64_t);    
        ecs_assert(!gen || dense_array[dense] == (index | gen), ECS_INTERNAL_ERROR, NULL);
        (void)dense_vector;
        (void)dense_array;
    } else {
        /* Element is not paired yet. Must add a new element to dense array */
        grow_dense(sparse);

        ecs_vector_t *dense_vector = sparse->dense;
        uint64_t *dense_array = ecs_vector_first(dense_vector, uint64_t);    
        int32_t dense_count = ecs_vector_count(dense_vector) - 1;
        int32_t count = sparse->count ++;

        /* If index is larger than max id, update max id */
        if (index >= get_id(sparse)) {
            set_id(sparse, index + 1);
        }

        if (count < dense_count) {
            /* If there are unused elements in the list, move the first unused
             * element to the end of the list */
            uint64_t unused = dense_array[count];
            chunk_t *unused_chunk = get_or_create_chunk(sparse, CHUNK(unused));
            assign_index(unused_chunk, dense_array, unused, dense_count);
        }

        assign_index(chunk, dense_array, index, count);
        dense_array[count] |= gen;
    }

    return DATA(chunk->data, sparse->size, offset);
}

void* _flecs_sparse_set(
    ecs_sparse_t * sparse,
    ecs_size_t elem_size,
    uint64_t index,
    void * value)
{
    void *ptr = _flecs_sparse_ensure(sparse, elem_size, index);
    ecs_os_memcpy(ptr, value, elem_size);
    return ptr;
}

void* _flecs_sparse_remove_get(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    uint64_t gen = strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
        uint64_t cur_gen = dense_array[dense] & ECS_GENERATION_MASK;
        if (gen != cur_gen) {
            /* Generation doesn't match which means that the provided entity is
             * already not alive. */
            return NULL;
        }

        /* Increase generation */
        dense_array[dense] = index | inc_gen(cur_gen);
        
        int32_t count = sparse->count;
        if (dense == (count - 1)) {
            /* If dense is the last used element, simply decrease count */
            sparse->count --;
        } else if (dense < count) {
            /* If element is alive, move it to unused elements */
            swap_dense(sparse, chunk, dense, count - 1);
            sparse->count --;
        } else {
            /* Element is not alive, nothing to be done */
            return NULL;
        }

        /* Reset memory to zero on remove */
        return DATA(chunk->data, sparse->size, offset);
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
        return NULL;
    }
}

void flecs_sparse_remove(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    void *ptr = _flecs_sparse_remove_get(sparse, 0, index);
    if (ptr) {
        ecs_os_memset(ptr, 0, sparse->size);
    }
}

void flecs_sparse_set_generation(
    ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = get_or_create_chunk(sparse, CHUNK(index));
    
    uint64_t index_w_gen = index;
    strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    if (dense) {
        /* Increase generation */
        uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
        dense_array[dense] = index_w_gen;
    } else {
        /* Element is not paired and thus not alive, nothing to be done */
    }
}

bool flecs_sparse_exists(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return false;
    }
    
    strip_generation(&index);
    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];

    return dense != 0;
}

void* _flecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    int32_t dense_index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(dense_index < sparse->count, ECS_INVALID_PARAMETER, NULL);
    (void)size;

    dense_index ++;

    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);
    return get_sparse(sparse, dense_index, dense_array[dense_index]);
}

bool flecs_sparse_is_alive(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    return try_sparse(sparse, index) != NULL;
}

uint64_t flecs_sparse_get_alive(
    const ecs_sparse_t *sparse,
    uint64_t index)
{
    chunk_t *chunk = get_chunk(sparse, CHUNK(index));
    if (!chunk) {
        return 0;
    }

    int32_t offset = OFFSET(index);
    int32_t dense = chunk->sparse[offset];
    uint64_t *dense_array = ecs_vector_first(sparse->dense, uint64_t);

    /* If dense is 0 (tombstone) this will return 0 */
    return dense_array[dense];
}

void* _flecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    return try_sparse(sparse, index);
}

void* _flecs_sparse_get_any(
    ecs_sparse_t *sparse,
    ecs_size_t size,
    uint64_t index)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || size == sparse->size, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    return try_sparse_any(sparse, index);
}

int32_t flecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }

    return sparse->count - 1;
}

int32_t flecs_sparse_size(
    const ecs_sparse_t *sparse)
{
    if (!sparse) {
        return 0;
    }
        
    return ecs_vector_count(sparse->dense) - 1;
}

const uint64_t* flecs_sparse_ids(
    const ecs_sparse_t *sparse)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    return &(ecs_vector_first(sparse->dense, uint64_t)[1]);
}

void flecs_sparse_set_size(
    ecs_sparse_t *sparse,
    int32_t elem_count)
{
    ecs_assert(sparse != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_vector_set_size(&sparse->dense, uint64_t, elem_count);
}

static
void sparse_copy(
    ecs_sparse_t * dst,
    const ecs_sparse_t * src)
{
    flecs_sparse_set_size(dst, flecs_sparse_size(src));
    const uint64_t *indices = flecs_sparse_ids(src);
    
    ecs_size_t size = src->size;
    int32_t i, count = src->count;

    for (i = 0; i < count - 1; i ++) {
        uint64_t index = indices[i];
        void *src_ptr = _flecs_sparse_get(src, size, index);
        void *dst_ptr = _flecs_sparse_ensure(dst, size, index);
        flecs_sparse_set_generation(dst, index);
        ecs_os_memcpy(dst_ptr, src_ptr, size);
    }

    set_id(dst, get_id(src));

    ecs_assert(src->count == dst->count, ECS_INTERNAL_ERROR, NULL);
}

ecs_sparse_t* flecs_sparse_copy(
    const ecs_sparse_t *src)
{
    if (!src) {
        return NULL;
    }

    ecs_sparse_t *dst = _flecs_sparse_new(src->size);
    sparse_copy(dst, src);

    return dst;
}

void flecs_sparse_restore(
    ecs_sparse_t * dst,
    const ecs_sparse_t * src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    dst->count = 1;
    if (src) {
        sparse_copy(dst, src);
    }
}

void flecs_sparse_memory(
    ecs_sparse_t *sparse,
    int32_t *allocd,
    int32_t *used)
{
    (void)sparse;
    (void)allocd;
    (void)used;
}

ecs_sparse_t* _ecs_sparse_new(
    ecs_size_t elem_size)
{
    return _flecs_sparse_new(elem_size);
}

void* _ecs_sparse_add(
    ecs_sparse_t *sparse,
    ecs_size_t elem_size)
{
    return _flecs_sparse_add(sparse, elem_size);
}

uint64_t ecs_sparse_last_id(
    const ecs_sparse_t *sparse)
{
    return flecs_sparse_last_id(sparse);
}

int32_t ecs_sparse_count(
    const ecs_sparse_t *sparse)
{
    return flecs_sparse_count(sparse);
}

void* _ecs_sparse_get_dense(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    int32_t index)
{
    return _flecs_sparse_get_dense(sparse, elem_size, index);
}

void* _ecs_sparse_get(
    const ecs_sparse_t *sparse,
    ecs_size_t elem_size,
    uint64_t id)
{
    return _flecs_sparse_get(sparse, elem_size, id);
}

#ifdef FLECS_DEPRECATED


int32_t ecs_count_w_filter(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    return ecs_count_filter(world, filter);
}

int32_t ecs_count_entity(
    const ecs_world_t *world,
    ecs_id_t entity)
{
    return ecs_count_id(world, entity);
}

void ecs_set_component_actions_w_entity(
    ecs_world_t *world,
    ecs_id_t id,
    EcsComponentLifecycle *actions)
{
    ecs_set_component_actions_w_id(world, id, actions);
}

ecs_entity_t ecs_new_w_entity(
    ecs_world_t *world,
    ecs_id_t id)
{
    return ecs_new_w_id(world, id);
}

const ecs_entity_t* ecs_bulk_new_w_entity(
    ecs_world_t *world,
    ecs_id_t id,
    int32_t count)
{
    return ecs_bulk_new_w_id(world, id, count);
}

void ecs_enable_component_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool enable)
{
    ecs_enable_component_w_id(world, entity, id, enable);
}

bool ecs_is_component_enabled_w_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_is_component_enabled_w_id(world, entity, id);
}

const void* ecs_get_w_id(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_get_id(world, entity, id);
}

const void* ecs_get_w_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_get_id(world, entity, id);
}

const void* ecs_get_ref_w_entity(
    const ecs_world_t *world,
    ecs_ref_t *ref,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_get_ref_w_id(world, ref, entity, id);
}

void* ecs_get_mut_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_added)
{
    return ecs_get_mut_id(world, entity, id, is_added);
}

void* ecs_get_mut_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    bool *is_added)
{
    return ecs_get_mut_id(world, entity, id, is_added);
}

void ecs_modified_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_modified_id(world, entity, id);
}

void ecs_modified_w_id(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_modified_id(world, entity, id);
}

ecs_entity_t ecs_set_ptr_w_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id,
    size_t size,
    const void *ptr)
{
    return ecs_set_id(world, entity, id, size, ptr);
}

bool ecs_has_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    return ecs_has_id(world, entity, id);
}

size_t ecs_entity_str(
    const ecs_world_t *world,
    ecs_id_t entity,
    char *buffer,
    size_t buffer_len)
{
    return ecs_id_str(world, entity, buffer, buffer_len);
}

ecs_entity_t ecs_get_parent_w_entity(
    const ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    (void)id;
    return ecs_get_object(world, entity, EcsChildOf, 0);
}

int32_t ecs_get_thread_index(
    const ecs_world_t *world)
{
    return ecs_get_stage_id(world);
}

void ecs_add_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_add_id(world, entity, id);
}

void ecs_remove_entity(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_id_t id)
{
    ecs_remove_id(world, entity, id);
}

void ecs_dim_type(
    ecs_world_t *world,
    ecs_type_t type,
    int32_t entity_count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    if (type) {
        ecs_table_t *table = ecs_table_from_type(world, type);
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
        
        ecs_data_t *data = flecs_table_get_or_create_data(table);
        flecs_table_set_size(world, table, data, entity_count);
    }
}

ecs_type_t ecs_type_from_entity(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    return ecs_type_from_id(world, entity);
}

ecs_entity_t ecs_type_to_entity(
    const ecs_world_t *world,
    ecs_type_t type)
{
    return ecs_type_to_id(world, type);
}

bool ecs_type_has_entity(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity)
{
    return ecs_type_has_id(world, type, entity, false);
}

bool ecs_type_owns_entity(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t entity,
    bool owned)
{
    return ecs_type_has_id(world, type, entity, owned);
}

ecs_type_t ecs_column_type(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->types != NULL, ECS_INTERNAL_ERROR, NULL);
    return it->types[index - 1];
}

int32_t ecs_column_index_from_name(
    const ecs_iter_t *it,
    const char *name)
{
    if (it->query) {
        ecs_term_t *terms = it->query->filter.terms;
        int32_t i, count = it->query->filter.term_count;

        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            if (term->name) {
                if (!strcmp(name, term->name)) {
                    return i + 1;
                }
            }
        }
    }

    return 0;
}

void* ecs_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column)
{
    return ecs_term_w_size(it, size, column);
}

bool ecs_is_owned(
    const ecs_iter_t *it,
    int32_t column)
{
    return ecs_term_is_owned(it, column);
}

bool ecs_is_readonly(
    const ecs_iter_t *it,
    int32_t column)
{
    return ecs_term_is_readonly(it, column);
}

ecs_entity_t ecs_column_source(
    const ecs_iter_t *it,
    int32_t index)
{
    return ecs_term_source(it, index);
}

ecs_id_t ecs_column_entity(
    const ecs_iter_t *it,
    int32_t index)
{
    return ecs_term_id(it, index);
}

size_t ecs_column_size(
    const ecs_iter_t *it,
    int32_t index)
{
    return ecs_term_size(it, index);
}

int32_t ecs_table_component_index(
    const ecs_iter_t *it,
    ecs_entity_t component)
{
    return ecs_iter_find_column(it, component);
}

void* ecs_table_column(
    const ecs_iter_t *it,
    int32_t column_index)
{
    return ecs_iter_column_w_size(it, 0, column_index);
}

size_t ecs_table_column_size(
    const ecs_iter_t *it,
    int32_t column_index)
{
    return ecs_iter_column_size(it, column_index);
}

ecs_query_t* ecs_query_new(
    ecs_world_t *world,
    const char *expr)
{
    return ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = expr
    });
}

void ecs_query_free(
    ecs_query_t *query)
{
    ecs_query_fini(query);
}

ecs_query_t* ecs_subquery_new(
    ecs_world_t *world,
    ecs_query_t *parent,
    const char *expr)
{
    return ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = expr,
        .parent = parent
    });
}

#endif

#ifdef FLECS_PLECS


#define TOK_NEWLINE '\n'

static
ecs_entity_t ensure_entity(
    ecs_world_t *world,
    const char *path)
{
    ecs_entity_t e = ecs_lookup_fullpath(world, path);
    if (!e) {
        e = ecs_new_from_path(world, 0, path);
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);
    }

    return e;
}

static
int create_term(
    ecs_world_t *world, 
    ecs_term_t *term,
    const char *name,
    const char *expr,
    int32_t column)
{
    if (!ecs_term_id_is_set(&term->pred)) {
        ecs_parser_error(name, expr, column, "missing predicate in expression");
        return -1;
    }

    if (!ecs_term_id_is_set(&term->args[0])) {
        ecs_parser_error(name, expr, column, "missing subject in expression");
        return -1;
    }

    ecs_entity_t pred = ensure_entity(world, term->pred.name);
    ecs_entity_t subj = ensure_entity(world, term->args[0].name);
    ecs_entity_t obj = 0;

    if (ecs_term_id_is_set(&term->args[1])) {
        obj = ensure_entity(world, term->args[1].name);
    }

    if (!obj) {
        ecs_add_id(world, subj, pred);
    } else {
        ecs_add_pair(world, subj, pred, obj);
    }

    return 0;
}

int ecs_plecs_from_str(
    ecs_world_t *world,
    const char *name,
    const char *expr) 
{
    const char *ptr = expr;
    ecs_term_t term = {0};

    if (!expr) {
        return 0;
    }

    while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) {
        if (!ecs_term_is_initialized(&term)) {
            break;
        }
        
        if (create_term(world, &term, name, expr, (int32_t)(ptr - expr))) {
            return -1;
        }

        ecs_term_fini(&term);

        if (ptr[0] == TOK_NEWLINE) {
            ptr ++;
            expr = ptr;
        }
    }

    return 0;
}

int ecs_plecs_from_file(
    ecs_world_t *world,
    const char *filename) 
{
    FILE* file;
    char* content = NULL;
    int32_t bytes;
    size_t size;

    /* Open file for reading */
    ecs_os_fopen(&file, filename, "r");
    if (!file) {
        ecs_err("%s (%s)", ecs_os_strerror(errno), filename);
        goto error;
    }

    /* Determine file size */
    fseek(file, 0 , SEEK_END);
    bytes = (int32_t)ftell(file);
    if (bytes == -1) {
        goto error;
    }
    rewind(file);

    /* Load contents in memory */
    content = ecs_os_malloc(bytes + 1);
    size = (size_t)bytes;
    if (!(size = fread(content, 1, size, file)) && bytes) {
        ecs_err("%s: read zero bytes instead of %d", filename, size);
        ecs_os_free(content);
        content = NULL;
        goto error;
    } else {
        content[size] = '\0';
    }

    fclose(file);

    int result = ecs_plecs_from_str(world, filename, content);
    ecs_os_free(content);
    return result;
error:
    ecs_os_free(content);
    return -1;
}

#endif

#ifdef FLECS_MODULE


char* ecs_module_path_from_c(
    const char *c_name)
{
    ecs_strbuf_t str = ECS_STRBUF_INIT;
    const char *ptr;
    char ch;

    for (ptr = c_name; (ch = *ptr); ptr++) {
        if (isupper(ch)) {
            ch = flflecs_to_i8(tolower(ch));
            if (ptr != c_name) {
                ecs_strbuf_appendstrn(&str, ".", 1);
            }
        }

        ecs_strbuf_appendstrn(&str, &ch, 1);
    }

    return ecs_strbuf_get(&str);
}

ecs_entity_t ecs_import(
    ecs_world_t *world,
    ecs_module_action_t init_action,
    const char *module_name,
    void *handles_out,
    size_t handles_size)
{
    ecs_assert(!world->is_readonly, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t old_scope = ecs_set_scope(world, 0);
    const char *old_name_prefix = world->name_prefix;

    char *path = ecs_module_path_from_c(module_name);
    ecs_entity_t e = ecs_lookup_fullpath(world, path);
    ecs_os_free(path);
    
    if (!e) {
        ecs_trace_1("import %s", module_name);
        ecs_log_push();

        /* Load module */
        init_action(world);

        /* Lookup module entity (must be registered by module) */
        e = ecs_lookup_fullpath(world, module_name);
        ecs_assert(e != 0, ECS_MODULE_UNDEFINED, module_name);

        ecs_log_pop();
    }

    /* Copy value of module component in handles_out parameter */
    if (handles_size && handles_out) {
        void *handles_ptr = ecs_get_mut_id(world, e, e, NULL);
        ecs_os_memcpy(handles_out, handles_ptr, flecs_from_size_t(handles_size));   
    }

    /* Restore to previous state */
    ecs_set_scope(world, old_scope);
    world->name_prefix = old_name_prefix;

    return e;
}

ecs_entity_t ecs_import_from_library(
    ecs_world_t *world,
    const char *library_name,
    const char *module_name)
{
    ecs_assert(library_name != NULL, ECS_INVALID_PARAMETER, NULL);

    char *import_func = (char*)module_name; /* safe */
    char *module = (char*)module_name;

    if (!ecs_os_has_modules() || !ecs_os_has_dl()) {
        ecs_os_err(
            "library loading not supported, set module_to_dl, dlopen, dlclose "
            "and dlproc os API callbacks first");
        return 0;
    }

    /* If no module name is specified, try default naming convention for loading
     * the main module from the library */
    if (!import_func) {
        import_func = ecs_os_malloc(ecs_os_strlen(library_name) + ECS_SIZEOF("Import"));
        ecs_assert(import_func != NULL, ECS_OUT_OF_MEMORY, NULL);
        
        const char *ptr;
        char ch, *bptr = import_func;
        bool capitalize = true;
        for (ptr = library_name; (ch = *ptr); ptr ++) {
            if (ch == '.') {
                capitalize = true;
            } else {
                if (capitalize) {
                    *bptr = flflecs_to_i8(toupper(ch));
                    bptr ++;
                    capitalize = false;
                } else {
                    *bptr = flflecs_to_i8(tolower(ch));
                    bptr ++;
                }
            }
        }

        *bptr = '\0';

        module = ecs_os_strdup(import_func);
        ecs_assert(module != NULL, ECS_OUT_OF_MEMORY, NULL);

        ecs_os_strcat(bptr, "Import");
    }

    char *library_filename = ecs_os_module_to_dl(library_name);
    if (!library_filename) {
        ecs_os_err("failed to find library file for '%s'", library_name);
        if (module != module_name) {
            ecs_os_free(module);
        }
        return 0;
    } else {
        ecs_trace_1("found file '%s' for library '%s'", 
            library_filename, library_name);
    }

    ecs_os_dl_t dl = ecs_os_dlopen(library_filename);
    if (!dl) {
        ecs_os_err("failed to load library '%s' ('%s')", 
            library_name, library_filename);
        
        ecs_os_free(library_filename);

        if (module != module_name) {
            ecs_os_free(module);
        }    

        return 0;
    } else {
        ecs_trace_1("library '%s' ('%s') loaded", 
            library_name, library_filename);
    }

    ecs_module_action_t action = (ecs_module_action_t)
        ecs_os_dlproc(dl, import_func);
    if (!action) {
        ecs_os_err("failed to load import function %s from library %s",
            import_func, library_name);
        ecs_os_free(library_filename);
        ecs_os_dlclose(dl);            
        return 0;
    } else {
        ecs_trace_1("found import function '%s' in library '%s' for module '%s'",
            import_func, library_name, module);
    }

    /* Do not free id, as it will be stored as the component identifier */
    ecs_entity_t result = ecs_import(world, action, module, NULL, 0);

    if (import_func != module_name) {
        ecs_os_free(import_func);
    }

    if (module != module_name) {
        ecs_os_free(module);
    }

    ecs_os_free(library_filename);

    return result;
}

void ecs_add_module_tag(
    ecs_world_t *world,
    ecs_entity_t module)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(module != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_entity_t e = module;
    do {
        ecs_add_id(world, e, EcsModule);
        ecs_type_t type = ecs_get_type(world, e);
        int32_t index = ecs_type_index_of(type, 0, 
            ecs_pair(EcsChildOf, EcsWildcard));
        if (index == -1) {
            return;
        }

        ecs_entity_t *pair = ecs_vector_get(type, ecs_id_t, index);
        ecs_assert(pair != NULL, ECS_INTERNAL_ERROR, NULL);
        e = ecs_pair_object(world, *pair);
    } while (true);
}

ecs_entity_t ecs_module_init(
    ecs_world_t *world,
    const ecs_component_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    const char *name = desc->entity.name;

    char *module_path = ecs_module_path_from_c(name);
    ecs_entity_t e = ecs_new_from_fullpath(world, module_path);
    ecs_set_symbol(world, e, module_path);
    ecs_os_free(module_path);

    ecs_component_desc_t private_desc = *desc;
    private_desc.entity.entity = e;
    private_desc.entity.name = NULL;

    ecs_entity_t result = ecs_component_init(world, &private_desc);
    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result == e, ECS_INTERNAL_ERROR, NULL);

    /* Add module tag */
    ecs_add_module_tag(world, result);

    /* Add module to itself. This way we have all the module information stored
     * in a single contained entity that we can use for namespacing */
    ecs_set_id(world, result, result, desc->size, NULL);

    return result;
}

#endif

#ifdef FLECS_QUEUE

struct ecs_queue_t {
    ecs_vector_t *data;
    int32_t index;
};

ecs_queue_t* _ecs_queue_new(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count)
{
    ecs_queue_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_queue_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = _ecs_vector_new(elem_size, offset, elem_count);
    result->index = 0;
    return result;
}

ecs_queue_t* _ecs_queue_from_array(
    ecs_size_t elem_size,
    int16_t offset,
    int32_t elem_count,
    void *array)
{
    ecs_queue_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_queue_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->data = _ecs_vector_from_array(elem_size, offset, elem_count, array);
    result->index = 0;
    return result;    
}

void* _ecs_queue_push(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset)
{
    int32_t size = ecs_vector_size(buffer->data);
    int32_t count = ecs_vector_count(buffer->data);
    void *result;

    if (count == buffer->index) {
        result = _ecs_vector_add(&buffer->data, elem_size, offset);
    } else {
        result = _ecs_vector_get(buffer->data, elem_size, offset, buffer->index);
    }

    buffer->index = (buffer->index + 1) % size;

    return result;
}

void ecs_queue_free(
    ecs_queue_t *buffer)
{
    ecs_vector_free(buffer->data);
    ecs_os_free(buffer);
}

void* _ecs_queue_get(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset,
    int32_t index)
{
    int32_t count = ecs_vector_count(buffer->data);
    int32_t size = ecs_vector_size(buffer->data);
    index = ((buffer->index - count + size) + (int32_t)index) % size;
    return _ecs_vector_get(buffer->data, elem_size, offset, index);
}

void* _ecs_queue_last(
    ecs_queue_t *buffer,
    ecs_size_t elem_size,
    int16_t offset)
{
    int32_t index = buffer->index;
    if (!index) {
        index = ecs_vector_size(buffer->data);
    }

    return _ecs_vector_get(buffer->data, elem_size, offset, index - 1);
}

int32_t ecs_queue_index(
    ecs_queue_t *buffer)
{
    return buffer->index;
}

int32_t ecs_queue_count(
    ecs_queue_t *buffer)
{
    return ecs_vector_count(buffer->data);
}

#endif


#ifdef FLECS_STATS

#ifdef FLECS_SYSTEM
#ifndef FLECS_SYSTEM_PRIVATE_H
#define FLECS_SYSTEM_PRIVATE_H


typedef struct EcsSystem {
    ecs_iter_action_t action;       /* Callback to be invoked for matching it */

    ecs_entity_t entity;            /* Entity id of system, used for ordering */
    ecs_query_t *query;             /* System query */
    ecs_on_demand_out_t *on_demand; /* Keep track of [out] column refs */
    ecs_system_status_action_t status_action; /* Status action */   
    ecs_entity_t tick_source;       /* Tick source associated with system */
    
    int32_t invoke_count;           /* Number of times system is invoked */
    FLECS_FLOAT time_spent;         /* Time spent on running system */
    FLECS_FLOAT time_passed;        /* Time passed since last invocation */

    ecs_entity_t self;              /* Entity associated with system */

    void *ctx;                      /* Userdata for system */
    void *status_ctx;               /* User data for status action */ 
    void *binding_ctx;              /* Optional language binding context */

    ecs_ctx_free_t ctx_free;
    ecs_ctx_free_t status_ctx_free;
    ecs_ctx_free_t binding_ctx_free;      
} EcsSystem;

/* Invoked when system becomes active / inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate,
    const EcsSystem *system_data);

/* Internal function to run a system */
ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    EcsSystem *system_data,
    int32_t stage_current,
    int32_t stage_count,
    FLECS_FLOAT delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param);

#endif
#endif

#ifdef FLECS_PIPELINE
#ifndef FLECS_PIPELINE_PRIVATE_H
#define FLECS_PIPELINE_PRIVATE_H


/** Instruction data for pipeline.
 * This type is the element type in the "ops" vector of a pipeline and contains
 * information about the set of systems that need to be ran before a merge. */
typedef struct ecs_pipeline_op_t {
    int32_t count;              /**< Number of systems to run before merge */
} ecs_pipeline_op_t;

typedef struct EcsPipelineQuery {
    ecs_query_t *query;
    ecs_query_t *build_query;
    int32_t match_count;
    ecs_vector_t *ops;
} EcsPipelineQuery;

////////////////////////////////////////////////////////////////////////////////
//// Pipeline API
////////////////////////////////////////////////////////////////////////////////

/** Update a pipeline (internal function).
 * Before running a pipeline, it must be updated. During this update phase
 * all systems in the pipeline are collected, ordered and sync points are 
 * inserted where necessary. This operation may only be called when staging is
 * disabled.
 *
 * Because multiple threads may run a pipeline, preparing the pipeline must 
 * happen synchronously, which is why this function is separate from 
 * ecs_pipeline_run. Not running the prepare step may cause systems to not get
 * ran, or ran in the wrong order.
 *
 * If 0 is provided for the pipeline id, the default pipeline will be ran (this
 * is either the builtin pipeline or the pipeline set with set_pipeline()).
 * 
 * @param world The world.
 * @param pipeline The pipeline to run.
 * @return The number of elements in the pipeline.
 */
int32_t ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    bool start_of_frame); 

////////////////////////////////////////////////////////////////////////////////
//// Worker API
////////////////////////////////////////////////////////////////////////////////

void ecs_worker_begin(
    ecs_world_t *world);

bool ecs_worker_sync(
    ecs_world_t *world);

void ecs_worker_end(
    ecs_world_t *world);

void ecs_workers_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time);

#endif
#endif

static
int32_t t_next(
    int32_t t)
{
    return (t + 1) % ECS_STAT_WINDOW;
}

static
int32_t t_prev(
    int32_t t)
{
    return (t - 1 + ECS_STAT_WINDOW) % ECS_STAT_WINDOW;
}

static
void _record_gauge(
    ecs_gauge_t *m,
    int32_t t,
    float value)
{
    m->avg[t] = value;
    m->min[t] = value;
    m->max[t] = value;
}

static
float _record_counter(
    ecs_counter_t *m,
    int32_t t,
    float value)
{
    int32_t tp = t_prev(t);
    float prev = m->value[tp];
    m->value[t] = value;
    _record_gauge((ecs_gauge_t*)m, t, value - prev);
    return value - prev;
}

/* Macro's to silence conversion warnings without adding casts everywhere */
#define record_gauge(m, t, value)\
    _record_gauge(m, t, (float)value)

#define record_counter(m, t, value)\
    _record_counter(m, t, (float)value)

static
void print_value(
    const char *name,
    float value)
{
    ecs_size_t len = ecs_os_strlen(name);
    printf("%s: %*s %.2f\n", name, 32 - len, "", (double)value);
}

static
void print_gauge(
    const char *name,
    int32_t t,
    const ecs_gauge_t *m)
{
    print_value(name, m->avg[t]);
}

static
void print_counter(
    const char *name,
    int32_t t,
    const ecs_counter_t *m)
{
    print_value(name, m->rate.avg[t]);
}

void ecs_gauge_reduce(
    ecs_gauge_t *dst,
    int32_t t_dst,
    ecs_gauge_t *src,
    int32_t t_src)
{
    ecs_assert(dst != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(src != NULL, ECS_INVALID_PARAMETER, NULL);

    bool min_set = false;
    dst->min[t_dst] = 0;
    dst->avg[t_dst] = 0;
    dst->max[t_dst] = 0;

    int32_t i;
    for (i = 0; i < ECS_STAT_WINDOW; i ++) {
        int32_t t = (t_src + i) % ECS_STAT_WINDOW;
        dst->avg[t_dst] += src->avg[t] / (float)ECS_STAT_WINDOW;
        if (!min_set || (src->min[t] < dst->min[t_dst])) {
            dst->min[t_dst] = src->min[t];
            min_set = true;
        }
        if ((src->max[t] > dst->max[t_dst])) {
            dst->max[t_dst] = src->max[t];
        }
    }
}

void ecs_get_world_stats(
    const ecs_world_t *world,
    ecs_world_stats_t *s)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(s != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    int32_t t = s->t = t_next(s->t);

    float delta_world_time = record_counter(&s->world_time_total_raw, t, world->stats.world_time_total_raw);
    record_counter(&s->world_time_total, t, world->stats.world_time_total);
    record_counter(&s->frame_time_total, t, world->stats.frame_time_total);
    record_counter(&s->system_time_total, t, world->stats.system_time_total);
    record_counter(&s->merge_time_total, t, world->stats.merge_time_total);

    float delta_frame_count = record_counter(&s->frame_count_total, t, world->stats.frame_count_total);
    record_counter(&s->merge_count_total, t, world->stats.merge_count_total);
    record_counter(&s->pipeline_build_count_total, t, world->stats.pipeline_build_count_total);
    record_counter(&s->systems_ran_frame, t, world->stats.systems_ran_frame);

    if (delta_world_time != 0.0f && delta_frame_count != 0.0f) {
        record_gauge(
            &s->fps, t, 1.0f / (delta_world_time / (float)delta_frame_count));
    } else {
        record_gauge(&s->fps, t, 0);
    }

    record_gauge(&s->entity_count, t, flecs_sparse_count(world->store.entity_index));
    record_gauge(&s->component_count, t, ecs_count_id(world, ecs_id(EcsComponent)));
    record_gauge(&s->query_count, t, flecs_sparse_count(world->queries));
    record_gauge(&s->system_count, t, ecs_count_id(world, ecs_id(EcsSystem)));

    record_counter(&s->new_count, t, world->new_count);
    record_counter(&s->bulk_new_count, t, world->bulk_new_count);
    record_counter(&s->delete_count, t, world->delete_count);
    record_counter(&s->clear_count, t, world->clear_count);
    record_counter(&s->add_count, t, world->add_count);
    record_counter(&s->remove_count, t, world->remove_count);
    record_counter(&s->set_count, t, world->set_count);
    record_counter(&s->discard_count, t, world->discard_count);

    /* Compute table statistics */
    int32_t empty_table_count = 0;
    int32_t singleton_table_count = 0;
    int32_t matched_table_count = 0, matched_entity_count = 0;

    int32_t i, count = flecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);
        int32_t entity_count = ecs_table_count(table);

        if (!entity_count) {
            empty_table_count ++;
        }

        /* Singleton tables are tables that have just one entity that also has
         * itself in the table type. */
        if (entity_count == 1) {
            ecs_data_t *data = flecs_table_get_data(table);
            ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);
            if (ecs_type_has_id(world, table->type, entities[0], false)) {
                singleton_table_count ++;
            }
        }

        /* If this table matches with queries and is not empty, increase the
         * matched table & matched entity count. These statistics can be used to
         * compute actual fragmentation ratio for queries. */
        int32_t queries_matched = ecs_vector_count(table->queries);
        if (queries_matched && entity_count) {
            matched_table_count ++;
            matched_entity_count += entity_count;
        }
    }

    record_gauge(&s->matched_table_count, t, matched_table_count);
    record_gauge(&s->matched_entity_count, t, matched_entity_count);
    
    record_gauge(&s->table_count, t, count);
    record_gauge(&s->empty_table_count, t, empty_table_count);
    record_gauge(&s->singleton_table_count, t, singleton_table_count);
}

void ecs_get_query_stats(
    const ecs_world_t *world,
    const ecs_query_t *query,
    ecs_query_stats_t *s)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(s != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)world;

    int32_t t = s->t = t_next(s->t);

    int32_t i, entity_count = 0, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *matched_tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);
    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *matched = &matched_tables[i];
        if (matched->table) {
            entity_count += ecs_table_count(matched->table);
        }
    }

    record_gauge(&s->matched_table_count, t, count);
    record_gauge(&s->matched_empty_table_count, t, 
        ecs_vector_count(query->empty_tables));
    record_gauge(&s->matched_entity_count, t, entity_count);
}

#ifdef FLECS_SYSTEM
bool ecs_get_system_stats(
    const ecs_world_t *world,
    ecs_entity_t system,
    ecs_system_stats_t *s)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(s != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(system != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    const EcsSystem *ptr = ecs_get(world, system, EcsSystem);
    if (!ptr) {
        return false;
    }

    ecs_get_query_stats(world, ptr->query, &s->query_stats);
    int32_t t = s->query_stats.t;

    record_counter(&s->time_spent, t, ptr->time_spent);
    record_counter(&s->invoke_count, t, ptr->invoke_count);
    record_gauge(&s->active, t, !ecs_has_id(world, system, EcsInactive));
    record_gauge(&s->enabled, t, !ecs_has_id(world, system, EcsDisabled));

    return true;
}
#endif


#ifdef FLECS_PIPELINE

static ecs_system_stats_t* get_system_stats(
    ecs_map_t *systems,
    ecs_entity_t system)
{
    ecs_assert(systems != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(system != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_system_stats_t *s = ecs_map_get(systems, ecs_system_stats_t, system);
    if (!s) {
        ecs_system_stats_t stats;
        memset(&stats, 0, sizeof(ecs_system_stats_t));
        ecs_map_set(systems, system, &stats);
        s = ecs_map_get(systems, ecs_system_stats_t, system);
        ecs_assert(s != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return s;
}

bool ecs_get_pipeline_stats(
    const ecs_world_t *world,
    ecs_entity_t pipeline,
    ecs_pipeline_stats_t *s)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(s != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(pipeline != 0, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);

    const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
    if (!pq) {
        return false;
    }

    /* First find out how many systems are matched by the pipeline */
    ecs_iter_t it = ecs_query_iter(pq->query);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        count += it.count;
    }

    if (!s->system_stats) {
        s->system_stats = ecs_map_new(ecs_system_stats_t, count);
    }    

    /* Also count synchronization points */
    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    count += ecs_vector_count(ops);

    /* Make sure vector is large enough to store all systems & sync points */
    ecs_vector_set_count(&s->systems, ecs_entity_t, count - 1);
    ecs_entity_t *systems = ecs_vector_first(s->systems, ecs_entity_t);

    /* Populate systems vector, keep track of sync points */
    it = ecs_query_iter(pq->query);
    int32_t i_system = 0, ran_since_merge = 0;
    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            systems[i_system ++] = it.entities[i];
            ran_since_merge ++;
            if (op != op_last && ran_since_merge == op->count) {
                ran_since_merge = 0;
                op++;
                systems[i_system ++] = 0; /* 0 indicates a merge point */
            }

            ecs_system_stats_t *sys_stats = get_system_stats(
                s->system_stats, it.entities[i]);
            ecs_get_system_stats(world, it.entities[i], sys_stats);
        }
    }

    ecs_assert(i_system == (count - 1), ECS_INTERNAL_ERROR, NULL);

    return true;
}
#endif

void ecs_dump_world_stats(
    const ecs_world_t *world,
    const ecs_world_stats_t *s)
{
    int32_t t = s->t;

    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(s != NULL, ECS_INVALID_PARAMETER, NULL);

    world = ecs_get_world(world);    
    
    print_counter("Frame", t, &s->frame_count_total);
    printf("-------------------------------------\n");
    print_counter("pipeline rebuilds", t, &s->pipeline_build_count_total);
    print_counter("systems ran last frame", t, &s->systems_ran_frame);
    printf("\n");
    print_value("target FPS", world->stats.target_fps);
    print_value("time scale", world->stats.time_scale);
    printf("\n");
    print_gauge("actual FPS", t, &s->fps);
    print_counter("frame time", t, &s->frame_time_total);
    print_counter("system time", t, &s->system_time_total);
    print_counter("merge time", t, &s->merge_time_total);
    print_counter("simulation time elapsed", t, &s->world_time_total);
    printf("\n");
    print_gauge("entity count", t, &s->entity_count);
    print_gauge("component count", t, &s->component_count);
    print_gauge("query count", t, &s->query_count);
    print_gauge("system count", t, &s->system_count);
    print_gauge("table count", t, &s->table_count);
    print_gauge("singleton table count", t, &s->singleton_table_count);
    print_gauge("empty table count", t, &s->empty_table_count);
    printf("\n");
    print_counter("deferred new operations", t, &s->new_count);
    print_counter("deferred bulk_new operations", t, &s->bulk_new_count);
    print_counter("deferred delete operations", t, &s->delete_count);
    print_counter("deferred clear operations", t, &s->clear_count);
    print_counter("deferred add operations", t, &s->add_count);
    print_counter("deferred remove operations", t, &s->remove_count);
    print_counter("deferred set operations", t, &s->set_count);
    print_counter("discarded operations", t, &s->discard_count);
    printf("\n");
}

#endif

#ifdef FLECS_SNAPSHOT


/* World snapshot */
struct ecs_snapshot_t {
    ecs_world_t *world;
    ecs_sparse_t *entity_index;
    ecs_vector_t *tables;
    ecs_entity_t last_id;
    ecs_filter_t filter;
};

static
ecs_data_t* duplicate_data(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *main_data)
{
    ecs_data_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_data_t));

    int32_t i, column_count = table->column_count;
    ecs_entity_t *components = ecs_vector_first(table->type, ecs_entity_t);

    result->columns = ecs_os_memdup(
        main_data->columns, ECS_SIZEOF(ecs_column_t) * column_count);

    /* Copy entities */
    result->entities = ecs_vector_copy(main_data->entities, ecs_entity_t);
    ecs_entity_t *entities = ecs_vector_first(result->entities, ecs_entity_t);

    /* Copy record ptrs */
    result->record_ptrs = ecs_vector_copy(main_data->record_ptrs, ecs_record_t*);

    /* Copy each column */
    for (i = 0; i < column_count; i ++) {
        ecs_entity_t component = components[i];
        ecs_column_t *column = &result->columns[i];

        component = ecs_get_typeid(world, component);

        const ecs_type_info_t *cdata = flecs_get_c_info(world, component);
        int16_t size = column->size;
        int16_t alignment = column->alignment;
        ecs_copy_t copy;

        if (cdata && (copy = cdata->lifecycle.copy)) {
            int32_t count = ecs_vector_count(column->data);
            ecs_vector_t *dst_vec = ecs_vector_new_t(size, alignment, count);
            ecs_vector_set_count_t(&dst_vec, size, alignment, count);
            void *dst_ptr = ecs_vector_first_t(dst_vec, size, alignment);
            void *ctx = cdata->lifecycle.ctx;
            
            ecs_xtor_t ctor = cdata->lifecycle.ctor;
            if (ctor) {
                ctor(world, component, entities, dst_ptr, flecs_to_size_t(size), 
                    count, ctx);
            }

            void *src_ptr = ecs_vector_first_t(column->data, size, alignment);
            copy(world, component, entities, entities, dst_ptr, src_ptr, 
                flecs_to_size_t(size), count, ctx);

            column->data = dst_vec;
        } else {
            column->data = ecs_vector_copy_t(column->data, size, alignment);
        }
    }

    return result;
}

static
ecs_snapshot_t* snapshot_create(
    ecs_world_t *world,
    const ecs_sparse_t *entity_index,
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_snapshot_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_snapshot_t));
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    result->world = world;

    /* If no iterator is provided, the snapshot will be taken of the entire
     * world, and we can simply copy the entity index as it will be restored
     * entirely upon snapshote restore. */
    if (!iter && entity_index) {
        result->entity_index = flecs_sparse_copy(entity_index);
        result->tables = ecs_vector_new(ecs_table_leaf_t, 0);
    }

    ecs_iter_t iter_stack;
    if (!iter) {
        iter_stack = ecs_filter_iter(world, NULL);
        iter = &iter_stack;
        next = ecs_filter_next;
    }

    /* If an iterator is provided, this is a filterred snapshot. In this case we
     * have to patch the entity index one by one upon restore, as we don't want
     * to affect entities that were not part of the snapshot. */
    else {
        result->entity_index = NULL;
    }

    /* Iterate tables in iterator */
    while (next(iter)) {
        ecs_table_t *t = iter->table;

        if (t->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_data_t *data = flecs_table_get_data(t);
        if (!data || !data->entities || !ecs_vector_count(data->entities)) {
            continue;
        }

        ecs_table_leaf_t *l = ecs_vector_add(&result->tables, ecs_table_leaf_t);
        l->table = t;
        l->type = t->type;
        l->data = duplicate_data(world, t, data);
    }

    return result;
}

/** Create a snapshot */
ecs_snapshot_t* ecs_snapshot_take(
    ecs_world_t *world)
{
    ecs_snapshot_t *result = snapshot_create(
        world,
        world->store.entity_index,
        NULL,
        NULL);

    result->last_id = world->stats.last_id;

    return result;
}

/** Create a filtered snapshot */
ecs_snapshot_t* ecs_snapshot_take_w_iter(
    ecs_iter_t *iter,
    ecs_iter_next_action_t next)
{
    ecs_world_t *world = iter->world;
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_snapshot_t *result = snapshot_create(
        world,
        world->store.entity_index,
        iter,
        next);

    result->last_id = world->stats.last_id;

    return result;
}

/** Restore a snapshot */
void ecs_snapshot_restore(
    ecs_world_t *world,
    ecs_snapshot_t *snapshot)
{
    bool is_filtered = true;

    if (snapshot->entity_index) {
        flecs_sparse_restore(world->store.entity_index, snapshot->entity_index);
        flecs_sparse_free(snapshot->entity_index);
        is_filtered = false;
    }

    if (!is_filtered) {
        world->stats.last_id = snapshot->last_id;
    }

    ecs_table_leaf_t *leafs = ecs_vector_first(snapshot->tables, ecs_table_leaf_t);
    int32_t l = 0, count = ecs_vector_count(snapshot->tables);
    int32_t t, table_count = flecs_sparse_count(world->store.tables);

    for (t = 0; t < table_count; t ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, t);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        ecs_table_leaf_t *leaf = NULL;
        if (l < count) {
            leaf = &leafs[l];
        }

        if (leaf && leaf->table == table) {
            /* If the snapshot is filtered, update the entity index for the
             * entities in the snapshot. If the snapshot was not filtered
             * the entity index would have been replaced entirely, and this
             * is not necessary. */
            if (is_filtered) {
                ecs_vector_each(leaf->data->entities, ecs_entity_t, e_ptr, {
                    ecs_record_t *r = ecs_eis_get(world, *e_ptr);
                    if (r && r->table) {
                        ecs_data_t *data = flecs_table_get_data(r->table);
                        
                        /* Data must be not NULL, otherwise entity index could
                         * not point to it */
                        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

                        bool is_monitored;
                        int32_t row = flecs_record_to_row(r->row, &is_monitored);
                        
                        /* Always delete entity, so that even if the entity is
                        * in the current table, there won't be duplicates */
                        flecs_table_delete(world, r->table, data, row, true);
                    } else {
                        ecs_eis_set_generation(world, *e_ptr);
                    }
                });

                int32_t old_count = ecs_table_count(table);
                int32_t new_count = flecs_table_data_count(leaf->data);

                ecs_data_t *data = flecs_table_get_data(table);
                data = flecs_table_merge(world, table, table, data, leaf->data);

                /* Run OnSet systems for merged entities */
                flecs_run_set_systems(world, 0, table, data, NULL,
                    old_count, new_count, true);

                ecs_os_free(leaf->data->columns);
            } else {
                flecs_table_replace_data(world, table, leaf->data);
            }
            
            ecs_os_free(leaf->data);
            l ++;
        } else {
            /* If the snapshot is not filtered, the snapshot should restore the
             * world to the exact state it was in. When a snapshot is filtered,
             * it should only update the entities that were in the snapshot.
             * If a table is found that was not in the snapshot, and the
             * snapshot was not filtered, clear the table. */
            if (!is_filtered) {
                /* Clear data of old table. */
                flecs_table_clear_data(world, table, flecs_table_get_data(table));
            }
        }

        table->alloc_count ++;
    }

    /* If snapshot was not filtered, run OnSet systems now. This cannot be done
     * while restoring the snapshot, because the world is in an inconsistent
     * state while restoring. When a snapshot is filtered, the world is not left
     * in an inconsistent state, which makes running OnSet systems while
     * restoring safe */
    if (!is_filtered) {
        for (t = 0; t < table_count; t ++) {
            ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, t);
            if (table->flags & EcsTableHasBuiltins) {
                continue;
            }
            
            ecs_data_t *table_data = flecs_table_get_data(table);
            int32_t entity_count = flecs_table_data_count(table_data);

            flecs_run_set_systems(world, 0, table, 
                table_data, NULL, 0, entity_count, true);            
        }
    }

    ecs_vector_free(snapshot->tables);   

    ecs_os_free(snapshot);
}

ecs_iter_t ecs_snapshot_iter(
    ecs_snapshot_t *snapshot,
    const ecs_filter_t *filter)
{
    ecs_snapshot_iter_t iter = {
        .filter = filter ? *filter : (ecs_filter_t){0},
        .tables = snapshot->tables,
        .index = 0
    };

    return (ecs_iter_t){
        .world = snapshot->world,
        .table_count = ecs_vector_count(snapshot->tables),
        .iter.snapshot = iter
    };
}

bool ecs_snapshot_next(
    ecs_iter_t *it)
{
    ecs_snapshot_iter_t *iter = &it->iter.snapshot;
    ecs_table_leaf_t *tables = ecs_vector_first(iter->tables, ecs_table_leaf_t);
    int32_t count = ecs_vector_count(iter->tables);
    int32_t i;

    for (i = iter->index; i < count; i ++) {
        ecs_table_t *table = tables[i].table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        ecs_data_t *data = tables[i].data;

        /* Table must have data or it wouldn't have been added */
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!flecs_table_match_filter(it->world, table, &iter->filter)) {
            continue;
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = flecs_table_data_count(data);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        it->is_valid = true;
        iter->index = i + 1;
        goto yield;
    }

    it->is_valid = false;
    return false;

yield:
    it->is_valid = true;
    return true;    
}

/** Cleanup snapshot */
void ecs_snapshot_free(
    ecs_snapshot_t *snapshot)
{
    flecs_sparse_free(snapshot->entity_index);

    ecs_table_leaf_t *tables = ecs_vector_first(snapshot->tables, ecs_table_leaf_t);
    int32_t i, count = ecs_vector_count(snapshot->tables);
    for (i = 0; i < count; i ++) {
        ecs_table_leaf_t *leaf = &tables[i];
        flecs_table_clear_data(snapshot->world, leaf->table, leaf->data);
        ecs_os_free(leaf->data);
    }    

    ecs_vector_free(snapshot->tables);
    ecs_os_free(snapshot);
}

#endif

#ifdef FLECS_BULK


static
void bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    bool is_delete)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);
    (void)stage;

    int32_t i, count = flecs_sparse_count(world->store.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!flecs_table_match_filter(world, table, filter)) {
            continue;
        }

        /* Remove entities from index */
        ecs_data_t *data = flecs_table_get_data(table);
        if (!data) {
            /* If table has no data, there's nothing to delete */
            continue;
        }

        /* Both filters passed, clear table */
        if (is_delete) {
            flecs_table_delete_entities(world, table);
        } else {
            flecs_table_clear_entities_silent(world, table);
        }
    }
}

static
void merge_table(
    ecs_world_t *world,
    ecs_table_t *dst_table,
    ecs_table_t *src_table,
    ecs_ids_t *to_add,
    ecs_ids_t *to_remove)
{    
    if (!dst_table->type) {
        /* If this removes all components, clear table */
        flecs_table_clear_entities(world, src_table);
    } else {
        /* Merge table into dst_table */
        if (dst_table != src_table) {
            ecs_data_t *src_data = flecs_table_get_data(src_table);
            int32_t dst_count = ecs_table_count(dst_table);
            int32_t src_count = ecs_table_count(src_table);

            if (to_remove && to_remove->count && src_data) {
                flecs_run_remove_actions(world, src_table, 
                    src_data, 0, src_count, to_remove);
            }

            ecs_data_t *dst_data = flecs_table_get_data(dst_table);
            dst_data = flecs_table_merge(
                world, dst_table, src_table, dst_data, src_data);

            if (to_add && to_add->count && dst_data) {
                flecs_run_add_actions(world, dst_table, dst_data, 
                    dst_count, src_count, to_add, false, true);
            }
        }
    }
}

/* -- Public API -- */

void ecs_bulk_delete(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    bulk_delete(world, filter, true);
}

void ecs_bulk_add_remove_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);
    (void)stage;

    ecs_ids_t to_add_array = flecs_type_to_ids(to_add);
    ecs_ids_t to_remove_array = flecs_type_to_ids(to_remove);

    ecs_ids_t added = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_add_array.count),
        .count = 0
    }; 

    ecs_ids_t removed = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_remove_array.count),
        .count = 0
    };

    int32_t i, count = flecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!flecs_table_match_filter(world, table, filter)) {
            continue;
        }

        ecs_table_t *dst_table = flecs_table_traverse_remove(
            world, table, &to_remove_array, &removed);
        
        dst_table = flecs_table_traverse_add(
            world, dst_table, &to_add_array, &added);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);

        if (table == dst_table || (!added.count && !removed.count)) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   

        merge_table(world, dst_table, table, &added, &removed);
        added.count = 0;
        removed.count = 0;
    }    
}

void ecs_bulk_add_type(
    ecs_world_t *world,
    ecs_type_t to_add,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);
    (void)stage;

    ecs_ids_t to_add_array = flecs_type_to_ids(to_add);
    ecs_ids_t added = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_add_array.count),
        .count = 0
    };

    int32_t i, count = flecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!flecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = flecs_table_traverse_add(
            world, table, &to_add_array, &added);
        
        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!added.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table, &added, NULL);
        added.count = 0;
    }    
}

void ecs_bulk_add_entity(
    ecs_world_t *world,
    ecs_entity_t to_add,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_add != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);
    (void)stage;

    ecs_ids_t to_add_array = { .array = &to_add, .count = 1 };

    ecs_entity_t added_entity;
    ecs_ids_t added = {
        .array = &added_entity,
        .count = 0
    };

    int32_t i, count = flecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!flecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = flecs_table_traverse_add(
            world, table, &to_add_array, &added);

        ecs_assert(added.count <= to_add_array.count, ECS_INTERNAL_ERROR, NULL);
        
        if (!added.count) {
            continue;
        }         

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table, &added, NULL);
        added.count = 0;
    }    
}

void ecs_bulk_remove_type(
    ecs_world_t *world,
    ecs_type_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);
    (void)stage;

    ecs_ids_t to_remove_array = flecs_type_to_ids(to_remove);
    ecs_ids_t removed = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * to_remove_array.count),
        .count = 0
    };

    int32_t i, count = flecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!flecs_table_match_filter(world, table, filter)) {
            continue;
        }
        
        ecs_table_t *dst_table = flecs_table_traverse_remove(
            world, table, &to_remove_array, &removed);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!removed.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);
        merge_table(world, dst_table, table, NULL, &removed);
        removed.count = 0;        
    }    
}

void ecs_bulk_remove_entity(
    ecs_world_t *world,
    ecs_entity_t to_remove,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(to_remove != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_stage_t *stage = flecs_stage_from_world(&world);
    ecs_assert(stage == &world->stage, ECS_UNSUPPORTED, NULL);
    (void)stage;

    ecs_ids_t to_remove_array = { .array = &to_remove, .count = 1 };

    ecs_entity_t removed_entity;
    ecs_ids_t removed = {
        .array = &removed_entity,
        .count = 0
    };

    int32_t i, count = flecs_sparse_count(world->store.tables);
    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);

        if (table->flags & EcsTableHasBuiltins) {
            continue;
        }

        if (!flecs_table_match_filter(world, table, filter)) {
            continue;
        }            

        ecs_table_t *dst_table = flecs_table_traverse_remove(
            world, table, &to_remove_array, &removed);

        ecs_assert(removed.count <= to_remove_array.count, ECS_INTERNAL_ERROR, NULL);

        if (!removed.count) {
            continue;
        }

        ecs_assert(dst_table != NULL, ECS_INTERNAL_ERROR, NULL);   
        merge_table(world, dst_table, table, NULL, &removed);
        removed.count = 0;        
    }    
}

#endif

#ifdef FLECS_PARSER


/* TODO: after new query parser is working & code is ported to new types for
 *       storing terms, this code needs a big cleanup */

#define ECS_ANNOTATION_LENGTH_MAX (16)

#define TOK_COLON ':'
#define TOK_AND ','
#define TOK_OR "||"
#define TOK_NOT '!'
#define TOK_OPTIONAL '?'
#define TOK_BITWISE_OR '|'
#define TOK_NAME_SEP '.'
#define TOK_BRACKET_OPEN '['
#define TOK_BRACKET_CLOSE ']'
#define TOK_WILDCARD '*'
#define TOK_SINGLETON '$'
#define TOK_PAREN_OPEN '('
#define TOK_PAREN_CLOSE ')'
#define TOK_AS_ENTITY '\\'

#define TOK_SELF "self"
#define TOK_SUPERSET "superset"
#define TOK_SUBSET "subset"
#define TOK_CASCADE_SET "cascade"
#define TOK_ALL "all"

#define TOK_ANY "ANY"
#define TOK_OWNED "OWNED"
#define TOK_SHARED "SHARED"
#define TOK_SYSTEM "SYSTEM"
#define TOK_PARENT "PARENT"
#define TOK_CASCADE "CASCADE"

#define TOK_ROLE_PAIR "PAIR"
#define TOK_ROLE_AND "AND"
#define TOK_ROLE_OR "OR"
#define TOK_ROLE_XOR "XOR"
#define TOK_ROLE_NOT "NOT"
#define TOK_ROLE_SWITCH "SWITCH"
#define TOK_ROLE_CASE "CASE"
#define TOK_ROLE_DISABLED "DISABLED"

#define TOK_IN "in"
#define TOK_OUT "out"
#define TOK_INOUT "inout"

#define ECS_MAX_TOKEN_SIZE (256)

typedef char ecs_token_t[ECS_MAX_TOKEN_SIZE];

static
const char *skip_newline_and_space(
    const char *ptr)
{
    while (isspace(*ptr)) {
        ptr ++;
    }

    return ptr;    
}

/** Skip spaces when parsing signature */
static
const char *skip_space(
    const char *ptr)
{
    while ((*ptr != '\n') && isspace(*ptr)) {
        ptr ++;
    }

    return ptr;
}

/* -- Private functions -- */

static
bool valid_token_start_char(
    char ch)
{
    if (ch && (isalpha(ch) || (ch == '.') || (ch == '_') || (ch == '*') ||
        (ch == '0') || (ch == TOK_AS_ENTITY) || isdigit(ch))) 
    {
        return true;
    }

    return false;
}

static
bool valid_token_char(
    char ch)
{
    if (ch && (isalpha(ch) || isdigit(ch) || ch == '_' || ch == '.')) {
        return true;
    }

    return false;
}

static
bool valid_operator_char(
    char ch)
{
    if (ch == TOK_OPTIONAL || ch == TOK_NOT) {
        return true;
    }

    return false;
}

static
const char* parse_digit(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token_out)
{
    ptr = skip_space(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!isdigit(ch)) {
        ecs_parser_error(name, sig, column, 
            "invalid start of number '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    for (; (ch = *ptr); ptr ++) {
        if (!isdigit(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    return skip_space(ptr);
}


static
const char* parse_token(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr,
    char *token_out)
{
    ptr = skip_space(ptr);
    char *tptr = token_out, ch = ptr[0];

    if (!valid_token_start_char(ch)) {
        ecs_parser_error(name, sig, column, 
            "invalid start of identifier '%s'", ptr);
        return NULL;
    }

    tptr[0] = ch;
    tptr ++;
    ptr ++;

    int tmpl_nesting = 0;

    for (; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            tmpl_nesting ++;
        } else if (ch == '>') {
            if (!tmpl_nesting) {
                break;
            }
            tmpl_nesting --;
        } else
        if (!valid_token_char(ch)) {
            break;
        }

        tptr[0] = ch;
        tptr ++;
    }

    tptr[0] = '\0';

    if (tmpl_nesting != 0) {
        ecs_parser_error(name, sig, column, 
            "identifier '%s' has mismatching < > pairs", ptr);
        return NULL;
    }

    return skip_space(ptr);
}

static
int parse_identifier(
    const char *token,
    ecs_term_id_t *out)
{
    char ch = token[0];

    const char *tptr = token;
    if (ch == TOK_AS_ENTITY) {
        tptr ++;
    }

    out->name = ecs_os_strdup(tptr);

    if (ch == TOK_AS_ENTITY) {
        out->var = EcsVarIsEntity;
    } else if (ecs_identifier_is_var(tptr)) {
        out->var = EcsVarIsVariable;
    }

    return 0;
}

static
ecs_entity_t parse_role(
    const char *name,
    const char *sig,
    int64_t column,
    const char *token)
{
    if        (!ecs_os_strcmp(token, TOK_ROLE_PAIR)) 
    {
        return ECS_PAIR;            
    } else if (!ecs_os_strcmp(token, TOK_ROLE_AND)) {
        return ECS_AND;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_OR)) {
        return ECS_OR;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_XOR)) {
        return ECS_XOR;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_NOT)) {
        return ECS_NOT;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_SWITCH)) {
        return ECS_SWITCH;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_CASE)) {
        return ECS_CASE;
    } else if (!ecs_os_strcmp(token, TOK_OWNED)) {
        return ECS_OWNED;
    } else if (!ecs_os_strcmp(token, TOK_ROLE_DISABLED)) {
        return ECS_DISABLED;        
    } else {
        ecs_parser_error(name, sig, column, "invalid role '%s'", token);
        return 0;
    }
}

static
ecs_oper_kind_t parse_operator(
    char ch)
{
    if (ch == TOK_OPTIONAL) {
        return EcsOptional;
    } else if (ch == TOK_NOT) {
        return EcsNot;
    } else {
        ecs_abort(ECS_INTERNAL_ERROR, NULL);
    }
}

static
const char* parse_annotation(
    const char *name,
    const char *sig,
    int64_t column,
    const char *ptr, 
    ecs_inout_kind_t *inout_kind_out)
{
    char token[ECS_MAX_TOKEN_SIZE];

    ptr = parse_token(name, sig, column, ptr, token);
    if (!ptr) {
        return NULL;
    }

    if (!strcmp(token, "in")) {
        *inout_kind_out = EcsIn;
    } else
    if (!strcmp(token, "out")) {
        *inout_kind_out = EcsOut;
    } else
    if (!strcmp(token, "inout")) {
        *inout_kind_out = EcsInOut;
    }

    ptr = skip_space(ptr);

    if (ptr[0] != TOK_BRACKET_CLOSE) {
        ecs_parser_error(name, sig, column, "expected ]");
        return NULL;
    }

    return ptr + 1;
}

static
uint8_t parse_set_token(
    const char *token)
{
    if (!ecs_os_strcmp(token, TOK_SELF)) {
        return EcsSelf;
    } else if (!ecs_os_strcmp(token, TOK_SUPERSET)) {
        return EcsSuperSet;
    } else if (!ecs_os_strcmp(token, TOK_SUBSET)) {
        return EcsSubSet;
    } else if (!ecs_os_strcmp(token, TOK_CASCADE_SET)) {
        return EcsCascade;
    } else if (!ecs_os_strcmp(token, TOK_ALL)) {
        return EcsAll;
    } else {
        return 0;
    }
}

static
const char* parse_set_expr(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_id_t *id)
{
    do {
        uint8_t tok = parse_set_token(token);
        if (!tok) {
            ecs_parser_error(name, expr, column, 
                "invalid set token '%s'", token);
            return NULL;
        }

        if (id->set.mask & tok) {
            ecs_parser_error(name, expr, column, 
                "duplicate set token '%s'", token);
            return NULL;            
        }

        if ((tok == EcsSubSet && id->set.mask & EcsSuperSet) ||
            (tok == EcsSuperSet && id->set.mask & EcsSubSet))
        {
            ecs_parser_error(name, expr, column, 
                "cannot mix superset and subset", token);
            return NULL;            
        }    

        id->set.mask |= tok;

        if (ptr[0] == TOK_PAREN_OPEN) {
            ptr ++;

            /* Relationship (overrides IsA default) */
            if (!isdigit(ptr[0]) && valid_token_start_char(ptr[0])) {
                ptr = parse_token(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }         

                ecs_entity_t rel = ecs_lookup_fullpath(world, token);
                if (!rel) {
                    ecs_parser_error(name, expr, column, 
                        "unresolved identifier '%s'", token);
                    return NULL;
                }

                id->set.relation = rel;

                if (ptr[0] == TOK_AND) {
                    ptr = skip_space(ptr + 1);
                } else if (ptr[0] != TOK_PAREN_CLOSE) {
                    ecs_parser_error(name, expr, column, 
                        "expected ',' or ')'");
                    return NULL;
                }
            }

            /* Max depth of search */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }

                id->set.max_depth = atoi(token);
                if (id->set.max_depth < 0) {
                    ecs_parser_error(name, expr, column, 
                        "invalid negative depth");
                    return NULL;  
                }

                if (ptr[0] == ',') {
                    ptr = skip_space(ptr + 1);
                }
            }

            /* If another digit is found, previous depth was min depth */
            if (isdigit(ptr[0])) {
                ptr = parse_digit(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }

                id->set.min_depth = id->set.max_depth;
                id->set.max_depth = atoi(token);
                if (id->set.max_depth < 0) {
                    ecs_parser_error(name, expr, column, 
                        "invalid negative depth");
                    return NULL;  
                }
            }

            if (ptr[0] != TOK_PAREN_CLOSE) {
                ecs_parser_error(name, expr, column, "expected ')'");
                return NULL;                
            } else {
                ptr = skip_space(ptr + 1);
                if (ptr[0] != TOK_PAREN_CLOSE && ptr[0] != TOK_AND) { 
                    ecs_parser_error(name, expr, column, 
                        "expected end of set expr");
                    return NULL;
                }
            }
        }

        /* Next token in set expression */
        if (ptr[0] == TOK_BITWISE_OR) {
            ptr ++;
            if (valid_token_start_char(ptr[0])) {
                ptr = parse_token(name, expr, (ptr - expr), ptr, token);
                if (!ptr) {
                    return NULL;
                }
            }

        /* End of set expression */
        } else if (ptr[0] == TOK_PAREN_CLOSE || ptr[0] == TOK_AND) {
            break;
        }
    } while (true);

    if (id->set.mask & EcsCascade && !(id->set.mask & EcsSuperSet) && 
        !(id->set.mask & EcsSubSet))
    {
        /* If cascade is used without specifying superset or subset, assume
         * superset */
        id->set.mask |= EcsSuperSet;
    }

    if (id->set.mask & EcsSelf && id->set.min_depth != 0) {
        ecs_parser_error(name, expr, column, 
            "min_depth must be zero for set expression with 'self'");
        return NULL;        
    }

    return ptr;
}

static
const char* parse_arguments(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    int64_t column,
    const char *ptr,
    char *token,
    ecs_term_t *term)
{
    (void)column;

    int32_t arg = 0;

    do {
        if (valid_token_start_char(ptr[0])) {
            if (arg == 2) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "too many arguments in term");
                return NULL;
            }

            ptr = parse_token(name, expr, (ptr - expr), ptr, token);
            if (!ptr) {
                return NULL;
            }

            /* If token is a self, superset or subset token, this is a set
             * expression */
            if (!ecs_os_strcmp(token, TOK_ALL) ||
                !ecs_os_strcmp(token, TOK_CASCADE_SET) ||
                !ecs_os_strcmp(token, TOK_SELF) || 
                !ecs_os_strcmp(token, TOK_SUPERSET) || 
                !ecs_os_strcmp(token, TOK_SUBSET))
            {
                ptr = parse_set_expr(world, name, expr, (ptr - expr), ptr, 
                    token, &term->args[arg]);
                if (!ptr) {
                    return NULL;
                }

            /* Regular identifier */
            } else if (parse_identifier(token, &term->args[arg])) {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "invalid identifier '%s'", token);
                return NULL;
            }

            if (ptr[0] == TOK_AND) {
                ptr = skip_space(ptr + 1);

            } else if (ptr[0] == TOK_PAREN_CLOSE) {
                ptr = skip_space(ptr + 1);
                break;

            } else {
                ecs_parser_error(name, expr, (ptr - expr), 
                    "expected ',' or ')'");
                return NULL;
            }

        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected identifier or set expression");
            return NULL;
        }

        arg ++;

    } while (true);

    return ptr;
}

static
const char* parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term_out)
{
    const char *ptr = expr;
    char token[ECS_MAX_TOKEN_SIZE] = {0};
    ecs_term_t term = { .move = true /* parser never owns resources */ };

    ptr = skip_space(ptr);

    /* Inout specifiers always come first */
    if (ptr[0] == TOK_BRACKET_OPEN) {
        ptr = parse_annotation(name, expr, (ptr - expr), ptr + 1, &term.inout);
        if (!ptr) {
            return NULL;
        }
        ptr = skip_space(ptr);
    }

    if (valid_operator_char(ptr[0])) {
        term.oper = parse_operator(ptr[0]);
        ptr = skip_space(ptr + 1);
    }

    /* If next token is the start of an identifier, it could be either a type
     * role, source or component identifier */
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }
        
        /* Is token a source identifier? */
        if (ptr[0] == TOK_COLON) {
            ptr ++;
            goto parse_source;
        }

        /* Is token a type role? */
        if (ptr[0] == TOK_BITWISE_OR && ptr[1] != TOK_BITWISE_OR) {
            ptr ++;
            goto parse_role;
        }

        /* Is token a predicate? */
        if (ptr[0] == TOK_PAREN_OPEN) {
            goto parse_predicate;    
        }

        /* Next token must be a predicate */
        goto parse_predicate;

    /* If next token is the source token, this is an empty source */
    } else if (ptr[0] == TOK_COLON) {
        goto empty_source;

    /* If next token is a singleton, assign identifier to pred and subject */
    } else if (ptr[0] == TOK_SINGLETON) {
        ptr ++;
        if (valid_token_start_char(ptr[0])) {
            ptr = parse_token(name, expr, (ptr - expr), ptr, token);
            if (!ptr) {
                return NULL;
            }

            goto parse_singleton;

        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "expected identifier after singleton operator");
            return NULL;
        }

    /* Pair with implicit subject */
    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;

    /* Nothing else expected here */
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected character '%c'", ptr[0]);
        return NULL;
    }

empty_source:
    term.args[0].set.mask = EcsNothing;
    ptr = skip_space(ptr + 1);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_predicate;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after source operator");
        return NULL;
    }

parse_source:
    if (!ecs_os_strcmp(token, TOK_PARENT)) {
        term.args[0].set.mask = EcsSuperSet;
        term.args[0].set.relation = EcsChildOf;
        term.args[0].set.max_depth = 1;
    } else if (!ecs_os_strcmp(token, TOK_SYSTEM)) {
        term.args[0].name = ecs_os_strdup(name);
    } else if (!ecs_os_strcmp(token, TOK_ANY)) {
        term.args[0].set.mask = EcsSelf | EcsSuperSet;
        term.args[0].set.relation = EcsIsA;
        term.args[0].entity = EcsThis;
    } else if (!ecs_os_strcmp(token, TOK_OWNED)) {
        term.args[0].set.mask = EcsSelf;
        term.args[0].entity = EcsThis;
    } else if (!ecs_os_strcmp(token, TOK_SHARED)) {
        term.args[0].set.mask = EcsSuperSet;
        term.args[0].set.relation = EcsIsA;
        term.args[0].entity = EcsThis;
    } else if (!ecs_os_strcmp(token, TOK_CASCADE)) {
        term.args[0].set.mask = EcsSuperSet | EcsCascade;
        term.args[0].set.relation = EcsChildOf;
        term.args[0].entity = EcsThis;
        term.oper = EcsOptional;
    } else {
         if (parse_identifier(token, &term.args[0])) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid identifier '%s'", token); 
            return NULL;           
        }
    }

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* Is the next token a role? */
        if (ptr[0] == TOK_BITWISE_OR && ptr[1] != TOK_BITWISE_OR) {
            ptr++;
            goto parse_role;
        }      

        /* If not, it's a predicate */
        goto parse_predicate;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after source");
        return NULL;
    }

parse_role:
    term.role = parse_role(name, expr, (ptr - expr), token);
    if (!term.role) {
        return NULL;
    }

    ptr = skip_space(ptr);

    /* If next token is the source token, this is an empty source */
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        /* If not, it's a predicate */
        goto parse_predicate;

    } else if (ptr[0] == TOK_PAREN_OPEN) {
        goto parse_pair;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected identifier after role");
        return NULL;
    }

parse_predicate:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

    ptr = skip_space(ptr);
    
    if (ptr[0] == TOK_PAREN_OPEN) {
        ptr ++;
        if (ptr[0] == TOK_PAREN_CLOSE) {
            term.args[0].set.mask = EcsNothing;
            ptr ++;
            ptr = skip_space(ptr);
        } else {
            ptr = parse_arguments(
                world, name, expr, (ptr - expr), ptr, token, &term);
        }

        goto parse_done;

    } else if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        goto parse_name;
    }

    goto parse_done;

parse_pair:
    ptr = parse_token(name, expr, (ptr - expr), ptr + 1, token);
    if (!ptr) {
        return NULL;
    }

    if (ptr[0] == TOK_AND) {
        ptr ++;
        term.args[0].entity = EcsThis;
        goto parse_pair_predicate;
    } else {
        ecs_parser_error(name, expr, (ptr - expr), 
            "unexpected character '%c'", ptr[0]);
    }

parse_pair_predicate:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;            
    }

    ptr = skip_space(ptr);
    if (valid_token_start_char(ptr[0])) {
        ptr = parse_token(name, expr, (ptr - expr), ptr, token);
        if (!ptr) {
            return NULL;
        }

        if (ptr[0] == TOK_PAREN_CLOSE) {
            ptr ++;
            goto parse_pair_object;
        } else {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected character '%c'", ptr[0]);
            return NULL;
        }
    } else if (ptr[0] == TOK_PAREN_CLOSE) {
        /* No object */
        goto parse_done;
    }

parse_pair_object:
    if (parse_identifier(token, &term.args[1])) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;
    }

    ptr = skip_space(ptr);
    goto parse_done; 

parse_singleton:
    if (parse_identifier(token, &term.pred)) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid identifier '%s'", token); 
        return NULL;        
    }

    parse_identifier(token, &term.args[0]);
    goto parse_done;

parse_name:
    term.name = ecs_os_strdup(token);
    ptr = skip_space(ptr);

parse_done:
    *term_out = term;

    return ptr;
}

char* ecs_parse_term(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    const char *ptr,
    ecs_term_t *term)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_term_id_t *subj = &term->args[0];

    bool prev_or = false;
    if (ptr != expr) {
        /* If this is not the start of the expression, scan back to check if
         * previous token was an OR */
        const char *bptr = ptr - 1;
        do {
            char ch = bptr[0];
            if (isspace(ch)) {
                bptr --;
                continue;
            }

            /* Previous token was not an OR */
            if (ch == TOK_AND) {
                break;
            }

            /* Previous token was an OR */
            if (ch == TOK_OR[0]) {
                prev_or = true;
                break;
            }

            ecs_parser_error(name, expr, (ptr - expr), 
                "invalid preceding token");
            return NULL;
        } while (true);
    }

    ptr = skip_newline_and_space(ptr);
    if (!ptr[0]) {
        return (char*)ptr;
    }

    if (ptr == expr && !strcmp(expr, "0")) {
        return (char*)&ptr[1];
    }

    int32_t prev_set = subj->set.mask;

    /* Parse next element */
    ptr = parse_term(world, name, ptr, term);
    if (!ptr) {
        ecs_term_fini(term);
        return NULL;
    }

    /* Post-parse consistency checks */

    /* If next token is OR, term is part of an OR expression */
    if (!ecs_os_strncmp(ptr, TOK_OR, 2) || prev_or) {
        /* An OR operator must always follow an AND or another OR */
        if (term->oper != EcsAnd) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine || with other operators");
            ecs_term_fini(term);
            return NULL;
        }

        term->oper = EcsOr;
    }

    /* Term must either end in end of expression, AND or OR token */
    if (ptr[0] != TOK_AND && (ptr[0] != TOK_OR[0]) && (ptr[0] != '\n') && ptr[0]) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "expected end of expression or next term");
        ecs_term_fini(term);
        return NULL;
    }

    /* If the term just contained a 0, the expression has nothing. Ensure
     * that after the 0 nothing else follows */
    if (!ecs_os_strcmp(term->pred.name, "0")) {
        if (ptr[0]) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "unexpected term after 0"); 
            ecs_term_fini(term);
            return NULL;
        }

        subj->set.mask = EcsNothing;
    }

    /* Cannot combine EcsNothing with operators other than AND */
    if (term->oper != EcsAnd && subj->set.mask == EcsNothing) {
        ecs_parser_error(name, expr, (ptr - expr), 
            "invalid operator for empty source"); 
        ecs_term_fini(term);    
        return NULL;
    }

    /* Verify consistency of OR expression */
    if (prev_or && term->oper == EcsOr) {
        /* Set expressions must be the same for all OR terms */
        if (subj->set.mask != prev_set) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "cannot combine different sources in OR expression");
            ecs_term_fini(term);
            return NULL;
        }

        term->oper = EcsOr;
    }

    /* Automatically assign This if entity is not assigned and the set is
     * nothing */
    if (subj->set.mask != EcsNothing) {
        if (!subj->name) {
            if (!subj->entity) {
                subj->entity = EcsThis;
            }
        }
    }

    if (subj->name && !ecs_os_strcmp(subj->name, "0")) {
        subj->entity = 0;
        subj->set.mask = EcsNothing;
    }

    /* Process role */
    if (term->role == ECS_AND) {
        term->oper = EcsAndFrom;
        term->role = 0;
    } else if (term->role == ECS_OR) {
        term->oper = EcsOrFrom;
        term->role = 0;
    } else if (term->role == ECS_NOT) {
        term->oper = EcsNotFrom;
        term->role = 0;
    }

    if (ptr[0]) {
        if (ptr[0] == ',') {
            ptr ++;
        } else if (ptr[0] == '|') {
            ptr += 2;
        }
    }

    ptr = skip_space(ptr);

    return (char*)ptr;
}

#endif

#ifdef FLECS_DIRECT_ACCESS


/* Prefix with "da" so that they don't conflict with other get_column's */

static
ecs_column_t *da_get_column(
    const ecs_table_t *table,
    int32_t column)
{
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(column <= table->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_data_t *data = table->data;
    if (data && data->columns) {
        return &table->data->columns[column];    
    } else {
        return NULL;
    }    
}

static
ecs_column_t *da_get_or_create_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column)
{
    ecs_column_t *c = da_get_column(table, column);
    if (!c && (!table->data || !table->data->columns)) {
        ecs_data_t *data = flecs_table_get_or_create_data(table);
        flecs_init_data(world, table, data);
        c = da_get_column(table, column);
    }
    ecs_assert(c != NULL, ECS_INTERNAL_ERROR, NULL);
    return c;
}

static
ecs_entity_t* get_entity_array(
    ecs_table_t *table,
    int32_t row)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->data != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->data->entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_entity_t *array = ecs_vector_first(table->data->entities, ecs_entity_t);
    return &array[row];
}

/* -- Public API -- */

ecs_type_t ecs_table_get_type(
    const ecs_table_t *table)
{
    return table->type;
}

ecs_record_t* ecs_record_find(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_record_t *r = ecs_eis_get(world, entity);
    if (r) {
        return r;
    } else {
        return NULL;
    }
}

ecs_record_t* ecs_record_ensure(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_record_t *r = ecs_eis_ensure(world, entity);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
    return r;
}

ecs_record_t ecs_table_insert(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    ecs_record_t *record)
{
    ecs_data_t *data = flecs_table_get_or_create_data(table);
    int32_t index = flecs_table_append(world, table, data, entity, record, true);
    if (record) {
        record->table = table;
        record->row = index + 1;
    }
    return (ecs_record_t){table, index + 1};
}

int32_t ecs_table_find_column(
    const ecs_table_t *table,
    ecs_entity_t component)
{
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(component != 0, ECS_INVALID_PARAMETER, NULL);
    return ecs_type_index_of(table->type, 0, component);
}

ecs_vector_t* ecs_table_get_column(
    const ecs_table_t *table,
    int32_t column)
{
    ecs_column_t *c = da_get_column(table, column);
    return c ? c->data : NULL;
}

ecs_vector_t* ecs_table_set_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column,
    ecs_vector_t* vector)
{
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    if (vector) {
        ecs_vector_assert_size(vector, c->size);
    } else {
        ecs_vector_t *entities = ecs_table_get_entities(table);
        if (entities) {
            int32_t count = ecs_vector_count(entities);
            vector = ecs_table_get_column(table, column);
            if (!vector) {
                vector = ecs_vector_new_t(c->size, c->alignment, count);
            } else {
                ecs_vector_set_count_t(&vector, c->size, c->alignment, count);
            }
        }
    }
    c->data = vector;
    
    return vector;
}

ecs_vector_t* ecs_table_get_entities(
    const ecs_table_t *table)
{
    ecs_data_t *data = table->data;
    if (!data) {
        return NULL;
    }

    return data->entities;
}

ecs_vector_t* ecs_table_get_records(
    const ecs_table_t *table)
{
    ecs_data_t *data = table->data;
    if (!data) {
        return NULL;
    }

    return data->record_ptrs;
}

void ecs_table_set_entities(
    ecs_table_t *table,
    ecs_vector_t *entities,
    ecs_vector_t *records)
{
    ecs_vector_assert_size(entities, sizeof(ecs_entity_t));
    ecs_vector_assert_size(records, sizeof(ecs_record_t*));
    ecs_assert(ecs_vector_count(entities) == ecs_vector_count(records), 
        ECS_INVALID_PARAMETER, NULL);

    ecs_data_t *data = table->data;
    if (!data) {
        data = flecs_table_get_or_create_data(table);
        ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    data->entities = entities;
    data->record_ptrs = records;
}

void ecs_records_clear(
    ecs_vector_t *records)
{
    int32_t i, count = ecs_vector_count(records);
    ecs_record_t **r = ecs_vector_first(records, ecs_record_t*);

    for (i = 0; i < count; i ++) {
        r[i]->table = NULL;
        if (r[i]->row < 0) {
            r[i]->row = -1;
        } else {
            r[i]->row = 0;
        }
    }
}

void ecs_records_update(
    ecs_world_t *world,
    ecs_vector_t *entities,
    ecs_vector_t *records,
    ecs_table_t *table)
{
    int32_t i, count = ecs_vector_count(records);
    ecs_entity_t *e = ecs_vector_first(entities, ecs_entity_t);
    ecs_record_t **r = ecs_vector_first(records, ecs_record_t*);

    for (i = 0; i < count; i ++) {
        r[i] = ecs_record_ensure(world, e[i]);
        ecs_assert(r[i] != NULL, ECS_INTERNAL_ERROR, NULL);

        r[i]->table = table;
        r[i]->row = i + 1;
    }    
}

void ecs_table_delete_column(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column,
    ecs_vector_t *vector)
{
    if (!vector) {
        vector = ecs_table_get_column(table, column);
        if (!vector) {
            return;
        }

        ecs_assert(table->data != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(table->data->columns != NULL, ECS_INTERNAL_ERROR, NULL);

        table->data->columns[column].data = NULL;
    }

    ecs_column_t *c = da_get_or_create_column(world, table, column);
    ecs_vector_assert_size(vector, c->size);

    ecs_type_info_t *c_info = table->c_info[column];
    ecs_xtor_t dtor;
    if (c_info && (dtor = c_info->lifecycle.dtor)) {
        ecs_entity_t *entities = get_entity_array(table, 0);
        int16_t alignment = c->alignment;
        int32_t count = ecs_vector_count(vector);
        void *ptr = ecs_vector_first_t(vector, c->size, alignment);
        dtor(world, c_info->component, entities, ptr, flecs_to_size_t(c->size), 
            count, c_info->lifecycle.ctx);
    }

    if (c->data == vector) {
        c->data = NULL;
    }

    ecs_vector_free(vector);
}

void* ecs_record_get_column(
    ecs_record_t *r,
    int32_t column,
    size_t c_size)
{
    (void)c_size;
    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_column(table, column);
    if (!c) {
        return NULL;
    }

    int16_t size = c->size;
    ecs_assert(!flecs_from_size_t(c_size) || flecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    void *array = ecs_vector_first_t(c->data, c->size, c->alignment);
    bool is_watched;
    int32_t row = flecs_record_to_row(r->row, &is_watched);
    return ECS_OFFSET(array, size * row);
}

void ecs_record_copy_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t c_size,
    const void *value,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(c_size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    int16_t size = c->size;
    ecs_assert(!flecs_from_size_t(c_size) || flecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    int16_t alignment = c->alignment;
    bool is_monitored;
    int32_t row = flecs_record_to_row(r->row, &is_monitored);
    void *ptr = ecs_vector_get_t(c->data, size, alignment, row);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_info_t *c_info = table->c_info[column];
    ecs_copy_t copy;
    if (c_info && (copy = c_info->lifecycle.copy)) {
        ecs_entity_t *entities = get_entity_array(table, row);
        copy(world, c_info->component, entities, entities, ptr, value, c_size, 
            count, c_info->lifecycle.ctx);
    } else {
        ecs_os_memcpy(ptr, value, size * count);
    }
}

void ecs_record_copy_pod_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t c_size,
    const void *value,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(c_size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);
    (void)c_size;

    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    int16_t size = c->size;
    ecs_assert(!flecs_from_size_t(c_size) || flecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    int16_t alignment = c->alignment;
    bool is_monitored;
    int32_t row = flecs_record_to_row(r->row, &is_monitored);
    void *ptr = ecs_vector_get_t(c->data, size, alignment, row);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_os_memcpy(ptr, value, size * count);
}

void ecs_record_move_to(
    ecs_world_t *world,
    ecs_record_t *r,
    int32_t column,
    size_t c_size,
    void *value,
    int32_t count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(r != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(c_size != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(value != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(count != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_table_t *table = r->table;
    ecs_column_t *c = da_get_or_create_column(world, table, column);
    int16_t size = c->size;
    ecs_assert(!flecs_from_size_t(c_size) || flecs_from_size_t(c_size) == c->size, 
        ECS_INVALID_PARAMETER, NULL);

    int16_t alignment = c->alignment;
    bool is_monitored;
    int32_t row = flecs_record_to_row(r->row, &is_monitored);
    void *ptr = ecs_vector_get_t(c->data, size, alignment, row);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_type_info_t *c_info = table->c_info[column];
    ecs_move_t move;
    if (c_info && (move = c_info->lifecycle.move)) {
        ecs_entity_t *entities = get_entity_array(table, row);
        move(world, c_info->component, entities, entities, ptr, value, c_size, 
            count, c_info->lifecycle.ctx);
    } else {
        ecs_os_memcpy(ptr, value, size * count);
    }
}

#endif

/* Roles */
const ecs_id_t ECS_CASE =  (ECS_ROLE | (0x7Cull << 56));
const ecs_id_t ECS_SWITCH =  (ECS_ROLE | (0x7Bull << 56));
const ecs_id_t ECS_PAIR =  (ECS_ROLE | (0x7Aull << 56));
const ecs_id_t ECS_OWNED =  (ECS_ROLE | (0x75ull << 56));
const ecs_id_t ECS_DISABLED =  (ECS_ROLE | (0x74ull << 56));

/* Core scopes & entities */
const ecs_entity_t EcsWorld =                 ECS_HI_COMPONENT_ID + 0;
const ecs_entity_t EcsFlecs =                 ECS_HI_COMPONENT_ID + 1;
const ecs_entity_t EcsFlecsCore =             ECS_HI_COMPONENT_ID + 2;
const ecs_entity_t EcsModule =                ECS_HI_COMPONENT_ID + 3;
const ecs_entity_t EcsPrefab =                ECS_HI_COMPONENT_ID + 4;
const ecs_entity_t EcsDisabled =              ECS_HI_COMPONENT_ID + 5;
const ecs_entity_t EcsHidden =                ECS_HI_COMPONENT_ID + 6;

/* Relation properties */
const ecs_entity_t EcsWildcard =              ECS_HI_COMPONENT_ID + 10;
const ecs_entity_t EcsThis =                  ECS_HI_COMPONENT_ID + 11;
const ecs_entity_t EcsTransitive =            ECS_HI_COMPONENT_ID + 12;
const ecs_entity_t EcsFinal =                 ECS_HI_COMPONENT_ID + 13;
const ecs_entity_t EcsTag =                   ECS_HI_COMPONENT_ID + 14;

/* Identifier tags */
const ecs_entity_t EcsName =                  ECS_HI_COMPONENT_ID + 15;
const ecs_entity_t EcsSymbol =                ECS_HI_COMPONENT_ID + 16;

/* Relations */
const ecs_entity_t EcsChildOf =               ECS_HI_COMPONENT_ID + 20;
const ecs_entity_t EcsIsA =                   ECS_HI_COMPONENT_ID + 21;

/* Events */
const ecs_entity_t EcsOnAdd =                 ECS_HI_COMPONENT_ID + 30;
const ecs_entity_t EcsOnRemove =              ECS_HI_COMPONENT_ID + 31;
const ecs_entity_t EcsOnSet =                 ECS_HI_COMPONENT_ID + 32;
const ecs_entity_t EcsUnSet =                 ECS_HI_COMPONENT_ID + 33;
const ecs_entity_t EcsOnDelete =              ECS_HI_COMPONENT_ID + 34;
const ecs_entity_t EcsOnCreateTable =         ECS_HI_COMPONENT_ID + 35;
const ecs_entity_t EcsOnDeleteTable =         ECS_HI_COMPONENT_ID + 36;
const ecs_entity_t EcsOnTableEmpty =          ECS_HI_COMPONENT_ID + 37;
const ecs_entity_t EcsOnTableNonEmpty =       ECS_HI_COMPONENT_ID + 38;
const ecs_entity_t EcsOnCreateTrigger =       ECS_HI_COMPONENT_ID + 39;
const ecs_entity_t EcsOnDeleteTrigger =       ECS_HI_COMPONENT_ID + 40;
const ecs_entity_t EcsOnDeleteObservable =    ECS_HI_COMPONENT_ID + 41;
const ecs_entity_t EcsOnComponentLifecycle =  ECS_HI_COMPONENT_ID + 42;
const ecs_entity_t EcsOnDeleteObject =        ECS_HI_COMPONENT_ID + 43;

/* Actions */
const ecs_entity_t EcsRemove =                ECS_HI_COMPONENT_ID + 50;
const ecs_entity_t EcsDelete =                ECS_HI_COMPONENT_ID + 51;
const ecs_entity_t EcsThrow =                 ECS_HI_COMPONENT_ID + 52;

/* Systems */
const ecs_entity_t EcsOnDemand =              ECS_HI_COMPONENT_ID + 60;
const ecs_entity_t EcsMonitor =               ECS_HI_COMPONENT_ID + 61;
const ecs_entity_t EcsDisabledIntern =        ECS_HI_COMPONENT_ID + 62;
const ecs_entity_t EcsInactive =              ECS_HI_COMPONENT_ID + 63;
const ecs_entity_t EcsPipeline =              ECS_HI_COMPONENT_ID + 64;
const ecs_entity_t EcsPreFrame =              ECS_HI_COMPONENT_ID + 65;
const ecs_entity_t EcsOnLoad =                ECS_HI_COMPONENT_ID + 66;
const ecs_entity_t EcsPostLoad =              ECS_HI_COMPONENT_ID + 67;
const ecs_entity_t EcsPreUpdate =             ECS_HI_COMPONENT_ID + 68;
const ecs_entity_t EcsOnUpdate =              ECS_HI_COMPONENT_ID + 69;
const ecs_entity_t EcsOnValidate =            ECS_HI_COMPONENT_ID + 70;
const ecs_entity_t EcsPostUpdate =            ECS_HI_COMPONENT_ID + 71;
const ecs_entity_t EcsPreStore =              ECS_HI_COMPONENT_ID + 72;
const ecs_entity_t EcsOnStore =               ECS_HI_COMPONENT_ID + 73;
const ecs_entity_t EcsPostFrame =             ECS_HI_COMPONENT_ID + 74;


/* -- Private functions -- */

const ecs_world_t* ecs_get_world(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        return world;
    } else {
        return ((ecs_stage_t*)world)->world;
    }
}

const ecs_stage_t* flecs_stage_from_readonly_world(
    const ecs_world_t *world)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_STAGE_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        return &world->stage;

    } else if (world->magic == ECS_STAGE_MAGIC) {
        return (ecs_stage_t*)world;
    }
    
    return NULL;    
}

ecs_stage_t *flecs_stage_from_world(
    ecs_world_t **world_ptr)
{
    ecs_world_t *world = *world_ptr;

    ecs_assert(world->magic == ECS_WORLD_MAGIC ||
               world->magic == ECS_STAGE_MAGIC,
               ECS_INTERNAL_ERROR,
               NULL);

    if (world->magic == ECS_WORLD_MAGIC) {
        ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
        return &world->stage;

    } else if (world->magic == ECS_STAGE_MAGIC) {
        ecs_stage_t *stage = (ecs_stage_t*)world;
        *world_ptr = stage->world;
        return stage;
    }
    
    return NULL;
}

/* Evaluate component monitor. If a monitored entity changed it will have set a
 * flag in one of the world's component monitors. Queries can register 
 * themselves with component monitors to determine whether they need to rematch
 * with tables. */
static
void eval_component_monitor(
    ecs_world_t *world)
{
    ecs_relation_monitor_t *rm = &world->monitors;

    if (!rm->is_dirty) {
        return;
    }

    ecs_map_iter_t it = ecs_map_iter(rm->monitor_sets);
    ecs_monitor_set_t *ms;

    while ((ms = ecs_map_next(&it, ecs_monitor_set_t, NULL))) {
        if (!ms->is_dirty) {
            continue;
        }

        if (ms->monitors) {
            ecs_map_iter_t mit = ecs_map_iter(ms->monitors);
            ecs_monitor_t *m;
            while ((m = ecs_map_next(&mit, ecs_monitor_t, NULL))) {
                if (!m->is_dirty) {
                    continue;
                }

                ecs_vector_each(m->queries, ecs_query_t*, q_ptr, {
                    flecs_query_notify(world, *q_ptr, &(ecs_query_event_t) {
                        .kind = EcsQueryTableRematch
                    });
                });

                m->is_dirty = false;
            }
        }

        ms->is_dirty = false;
    }

    rm->is_dirty = false;
}

void flecs_monitor_mark_dirty(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id)
{
    ecs_assert(world->monitors.monitor_sets != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Only flag if there are actually monitors registered, so that we
     * don't waste cycles evaluating monitors if there's no interest */
    ecs_monitor_set_t *ms = ecs_map_get(world->monitors.monitor_sets, 
        ecs_monitor_set_t, relation);
    if (ms && ms->monitors) {
        ecs_monitor_t *m = ecs_map_get(ms->monitors, 
            ecs_monitor_t, id);
        if (m) {
            m->is_dirty = true;
            ms->is_dirty = true;
            world->monitors.is_dirty = true;
        }
    }
}

void flecs_monitor_register(
    ecs_world_t *world,
    ecs_entity_t relation,
    ecs_entity_t id,
    ecs_query_t *query)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->monitors.monitor_sets != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_monitor_set_t *ms = ecs_map_ensure(
        world->monitors.monitor_sets, ecs_monitor_set_t, relation);
    ecs_assert(ms != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!ms->monitors) {
        ms->monitors = ecs_map_new(ecs_monitor_t, 1);
    }

    ecs_monitor_t *m = ecs_map_ensure(ms->monitors, ecs_monitor_t, id);
    ecs_assert(m != NULL, ECS_INTERNAL_ERROR, NULL);        

    ecs_query_t **q = ecs_vector_add(&m->queries, ecs_query_t*);
    *q = query;
}

static
void monitors_init(
    ecs_relation_monitor_t *rm)
{
    rm->monitor_sets = ecs_map_new(ecs_monitor_t, 0);
    rm->is_dirty = false;
}

static
void monitors_fini(
    ecs_relation_monitor_t *rm)
{
    ecs_map_iter_t it = ecs_map_iter(rm->monitor_sets);
    ecs_monitor_set_t *ms;

    while ((ms = ecs_map_next(&it, ecs_monitor_set_t, NULL))) {
        if (ms->monitors) {
            ecs_map_iter_t mit = ecs_map_iter(ms->monitors);
            ecs_monitor_t *m;
            while ((m = ecs_map_next(&mit, ecs_monitor_t, NULL))) {
                ecs_vector_free(m->queries);
            }

            ecs_map_free(ms->monitors);
        }
    }

    ecs_map_free(rm->monitor_sets);
}

static
void init_store(
    ecs_world_t *world) 
{
    ecs_os_memset(&world->store, 0, ECS_SIZEOF(ecs_store_t));
    
    /* Initialize entity index */
    world->store.entity_index = flecs_sparse_new(ecs_record_t);
    flecs_sparse_set_id_source(world->store.entity_index, &world->stats.last_id);

    /* Initialize root table */
    world->store.tables = flecs_sparse_new(ecs_table_t);

    /* Initialize table map */
    world->store.table_map = flecs_table_hashmap_new();

    /* Initialize one root table per stage */
    flecs_init_root_table(world);
}

static
void clean_tables(
    ecs_world_t *world)
{
    int32_t i, count = flecs_sparse_count(world->store.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *t = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);
        flecs_table_free(world, t);
    }

    /* Free table types separately so that if application destructors rely on
     * a type it's still valid. */
    for (i = 0; i < count; i ++) {
        ecs_table_t *t = flecs_sparse_get_dense(world->store.tables, ecs_table_t, i);
        flecs_table_free_type(t);
    }    

    /* Clear the root table */
    if (count) {
        flecs_table_reset(world, &world->store.root);
    }
}

static
void fini_store(ecs_world_t *world) {
    clean_tables(world);
    flecs_sparse_free(world->store.tables);
    flecs_table_free(world, &world->store.root);
    flecs_sparse_clear(world->store.entity_index);
    flecs_hashmap_free(world->store.table_map);
}

/* -- Public functions -- */

ecs_world_t *ecs_mini(void) {
    ecs_os_init();

    ecs_trace_1("bootstrap");
    ecs_log_push();

    if (!ecs_os_has_heap()) {
        ecs_abort(ECS_MISSING_OS_API, NULL);
    }

    if (!ecs_os_has_threading()) {
        ecs_trace_1("threading not available");
    }

    if (!ecs_os_has_time()) {
        ecs_trace_1("time management not available");
    }

    ecs_world_t *world = ecs_os_calloc(sizeof(ecs_world_t));
    ecs_assert(world != NULL, ECS_OUT_OF_MEMORY, NULL);

    world->magic = ECS_WORLD_MAGIC;
    world->fini_actions = NULL; 

    world->type_info = flecs_sparse_new(ecs_type_info_t);
    world->id_index = ecs_map_new(ecs_id_record_t, 8);
    world->id_triggers = ecs_map_new(ecs_id_trigger_t, 8);

    world->queries = flecs_sparse_new(ecs_query_t);
    world->triggers = flecs_sparse_new(ecs_trigger_t);
    world->observers = flecs_sparse_new(ecs_observer_t);
    world->fini_tasks = ecs_vector_new(ecs_entity_t, 0);
    world->name_prefix = NULL;

    world->aliases = flecs_string_hashmap_new();
    world->symbols = flecs_string_hashmap_new();

    monitors_init(&world->monitors);

    world->type_handles = ecs_map_new(ecs_entity_t, 0);
    world->on_activate_components = ecs_map_new(ecs_on_demand_in_t, 0);
    world->on_enable_components = ecs_map_new(ecs_on_demand_in_t, 0);

    world->worker_stages = NULL;
    world->workers_waiting = 0;
    world->workers_running = 0;
    world->quit_workers = false;
    world->is_readonly = false;
    world->is_fini = false;
    world->measure_frame_time = false;
    world->measure_system_time = false;
    world->should_quit = false;
    world->locking_enabled = false;
    world->pipeline = 0;

    world->frame_start_time = (ecs_time_t){0, 0};
    if (ecs_os_has_time()) {
        ecs_os_get_time(&world->world_start_time);
    }

    world->stats.target_fps = 0;
    world->stats.last_id = 0;

    world->stats.delta_time_raw = 0;
    world->stats.delta_time = 0;
    world->stats.time_scale = 1.0;
    world->stats.frame_time_total = 0;
    world->stats.system_time_total = 0;
    world->stats.merge_time_total = 0;
    world->stats.world_time_total = 0;
    world->stats.frame_count_total = 0;
    world->stats.merge_count_total = 0;
    world->stats.systems_ran_frame = 0;
    world->stats.pipeline_build_count_total = 0;
    
    world->range_check_enabled = false;

    world->fps_sleep = 0;

    world->context = NULL;

    world->arg_fps = 0;
    world->arg_threads = 0;

    flecs_stage_init(world, &world->stage);
    ecs_set_stages(world, 1);

    init_store(world);

    flecs_bootstrap(world);

    ecs_log_pop();

    return world;
}

ecs_world_t *ecs_init(void) {
    ecs_world_t *world = ecs_mini();

#ifdef FLECS_MODULE_H
    ecs_trace_1("import builtin modules");
    ecs_log_push();
#ifdef FLECS_SYSTEM_H
    ECS_IMPORT(world, FlecsSystem);
#endif
#ifdef FLECS_PIPELINE_H
    ECS_IMPORT(world, FlecsPipeline);
#endif
#ifdef FLECS_TIMER_H
    ECS_IMPORT(world, FlecsTimer);
#endif
    ecs_log_pop();
#endif

    return world;
}

#define ARG(short, long, action)\
    if (i < argc) {\
        if (argv[i][0] == '-') {\
            if (argv[i][1] == '-') {\
                if (long && !strcmp(&argv[i][2], long ? long : "")) {\
                    action;\
                    parsed = true;\
                }\
            } else {\
                if (short && argv[i][1] == short) {\
                    action;\
                    parsed = true;\
                }\
            }\
        }\
    }

ecs_world_t* ecs_init_w_args(
    int argc,
    char *argv[])
{
    (void)argc;
    (void)argv;
    return ecs_init();
}

void ecs_quit(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);
    world->should_quit = true;
}

bool ecs_should_quit(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->should_quit;
}

static
void on_demand_in_map_fini(
    ecs_map_t *map)
{
    ecs_map_iter_t it = ecs_map_iter(map);
    ecs_on_demand_in_t *elem;

    while ((elem = ecs_map_next(&it, ecs_on_demand_in_t, NULL))) {
        ecs_vector_free(elem->systems);
    }

    ecs_map_free(map);
}

void flecs_notify_tables(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_table_event_t *event)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    /* If no id is specified, broadcast to all tables */
    if (!id) {
        ecs_sparse_t *tables = world->store.tables;
        int32_t i, count = flecs_sparse_count(tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
            flecs_table_notify(world, table, event);
        }

    /* If id is specified, only broadcast to tables with id */
    } else {
        ecs_id_record_t *r = flecs_get_id_record(world, id);
        if (!r) {
            return;
        }

        ecs_table_record_t *tr;
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            flecs_table_notify(world, tr->table, event);
        }
    }
}

static
void default_ctor(
    ecs_world_t *world, ecs_entity_t component, const ecs_entity_t *entity_ptr,
    void *ptr, size_t size, int32_t count, void *ctx)
{
    (void)world; (void)component; (void)entity_ptr; (void)ctx;
    ecs_os_memset(ptr, 0, flecs_from_size_t(size) * count);
}

static
void default_copy_ctor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, const void *src_ptr,
    size_t size, int32_t count, void *ctx)
{
    callbacks->ctor(world, component, dst_entity, dst_ptr, size, count, ctx);
    callbacks->copy(world, component, dst_entity, src_entity, dst_ptr, src_ptr, 
        size, count, ctx);
}

static
void default_move_ctor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->ctor(world, component, dst_entity, dst_ptr, size, count, ctx);
    callbacks->move(world, component, dst_entity, src_entity, dst_ptr, src_ptr, 
        size, count, ctx);
}

static
void default_ctor_w_move_w_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->ctor(world, component, dst_entity, dst_ptr, size, count, ctx);
    callbacks->move(world, component, dst_entity, src_entity, dst_ptr, src_ptr, 
        size, count, ctx);
    callbacks->dtor(world, component, src_entity, src_ptr, size, count, ctx);
}

static
void default_move_ctor_w_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->move_ctor(world, component, callbacks, dst_entity, src_entity, 
        dst_ptr, src_ptr, size, count, ctx);
    callbacks->dtor(world, component, src_entity, src_ptr, size, count, ctx);
}

static
void default_move(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    callbacks->move(world, component, dst_entity, src_entity, 
        dst_ptr, src_ptr, size, count, ctx);
}

static
void default_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    (void)callbacks;
    (void)src_entity;

    /* When there is no move, destruct the destination component & memcpy the
     * component to dst. The src component does not have to be destructed when
     * a component has a trivial move. */
    callbacks->dtor(world, component, dst_entity, dst_ptr, size, count, ctx);
    ecs_os_memcpy(dst_ptr, src_ptr, flecs_from_size_t(size) * count);
}

static
void default_move_w_dtor(
    ecs_world_t *world, ecs_entity_t component,
    const EcsComponentLifecycle *callbacks, const ecs_entity_t *dst_entity,
    const ecs_entity_t *src_entity, void *dst_ptr, void *src_ptr, size_t size,
    int32_t count, void *ctx)
{
    /* If a component has a move, the move will take care of memcpying the data
     * and destroying any data in dst. Because this is not a trivial move, the
     * src component must also be destructed. */
    callbacks->move(world, component, dst_entity, src_entity, 
        dst_ptr, src_ptr, size, count, ctx);
    callbacks->dtor(world, component, src_entity, src_ptr, size, count, ctx);
}

void ecs_set_component_actions_w_id(
    ecs_world_t *world,
    ecs_entity_t component,
    EcsComponentLifecycle *lifecycle)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    flecs_stage_from_world(&world);

    const EcsComponent *component_ptr = ecs_get(world, component, EcsComponent);

    /* Cannot register lifecycle actions for things that aren't a component */
    ecs_assert(component_ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    /* Cannot register lifecycle actions for components with size 0 */
    ecs_assert(component_ptr->size != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_type_info_t *c_info = flecs_get_or_create_c_info(world, component);
    ecs_assert(c_info != NULL, ECS_INTERNAL_ERROR, NULL);

    if (c_info->lifecycle_set) {
        ecs_assert(c_info->component == component, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(c_info->lifecycle.ctor == lifecycle->ctor, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_assert(c_info->lifecycle.dtor == lifecycle->dtor, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_assert(c_info->lifecycle.copy == lifecycle->copy, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
        ecs_assert(c_info->lifecycle.move == lifecycle->move, 
            ECS_INCONSISTENT_COMPONENT_ACTION, NULL);
    } else {
        c_info->component = component;
        c_info->lifecycle = *lifecycle;
        c_info->lifecycle_set = true;
        c_info->size = component_ptr->size;
        c_info->alignment = component_ptr->alignment;

        /* If no constructor is set, invoking any of the other lifecycle actions 
         * is not safe as they will potentially access uninitialized memory. For 
         * ease of use, if no constructor is specified, set a default one that 
         * initializes the component to 0. */
        if (!lifecycle->ctor && 
            (lifecycle->dtor || lifecycle->copy || lifecycle->move)) 
        {
            c_info->lifecycle.ctor = default_ctor;   
        }

        /* Set default copy ctor, move ctor and merge */
        if (lifecycle->copy && !lifecycle->copy_ctor) {
            c_info->lifecycle.copy_ctor = default_copy_ctor;
        }

        if (lifecycle->move && !lifecycle->move_ctor) {
            c_info->lifecycle.move_ctor = default_move_ctor;
        }

        if (!lifecycle->ctor_move_dtor) {
            if (lifecycle->move) {
                if (lifecycle->dtor) {
                    if (lifecycle->move_ctor) {
                        /* If an explicit move ctor has been set, use callback 
                         * that uses the move ctor vs. using a ctor+move */
                        c_info->lifecycle.ctor_move_dtor = 
                            default_move_ctor_w_dtor;
                    } else {
                        /* If no explicit move_ctor has been set, use
                         * combination of ctor + move + dtor */
                        c_info->lifecycle.ctor_move_dtor = 
                            default_ctor_w_move_w_dtor;
                    }
                } else {
                    /* If no dtor has been set, this is just a move ctor */
                    c_info->lifecycle.ctor_move_dtor = 
                        c_info->lifecycle.move_ctor;
                }            
            }
        }

        if (!lifecycle->move_dtor) {
            if (lifecycle->move) {
                if (lifecycle->dtor) {
                    c_info->lifecycle.move_dtor = default_move_w_dtor;
                } else {
                    c_info->lifecycle.move_dtor = default_move;
                }
            } else {
                if (lifecycle->dtor) {
                    c_info->lifecycle.move_dtor = default_dtor;
                }
            }
        }

        /* Broadcast to all tables since we need to register a ctor for every
         * table that uses the component as itself, as predicate or as object.
         * The latter is what makes selecting the right set of tables complex,
         * as it depends on the predicate of a pair whether the object is used
         * as the component type or not. 
         * A more selective approach requires a more expressive notification
         * framework. */
        flecs_notify_tables(world, 0, &(ecs_table_event_t) {
            .kind = EcsTableComponentInfo,
            .component = component
        });
    }
}

bool ecs_component_has_actions(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);

    const ecs_type_info_t *c_info = flecs_get_c_info(world, component);
    return (c_info != NULL) && c_info->lifecycle_set;
}

void ecs_atfini(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_action_elem_t *elem = ecs_vector_add(&world->fini_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;
}

void ecs_run_post_frame(
    ecs_world_t *world,
    ecs_fini_action_t action,
    void *ctx)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(action != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_action_elem_t *elem = ecs_vector_add(&stage->post_frame_actions, 
        ecs_action_elem_t);
    ecs_assert(elem != NULL, ECS_INTERNAL_ERROR, NULL);

    elem->action = action;
    elem->ctx = ctx;    
}

/* Unset data in tables */
static
void fini_unset_tables(
    ecs_world_t *world)
{
    ecs_sparse_t *tables = world->store.tables;
    int32_t i, count = flecs_sparse_count(tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
        flecs_table_remove_actions(world, table);
    }
}

/* Invoke fini actions */
static
void fini_actions(
    ecs_world_t *world)
{
    ecs_vector_each(world->fini_actions, ecs_action_elem_t, elem, {
        elem->action(world, elem->ctx);
    });

    ecs_vector_free(world->fini_actions);
}

/* Cleanup component lifecycle callbacks & systems */
static
void fini_component_lifecycle(
    ecs_world_t *world)
{
    flecs_sparse_free(world->type_info);
}

/* Cleanup queries */
static
void fini_queries(
    ecs_world_t *world)
{
    int32_t i, count = flecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        ecs_query_t *query = flecs_sparse_get_dense(world->queries, ecs_query_t, 0);
        ecs_query_fini(query);
    }
    flecs_sparse_free(world->queries);
}

static
void fini_observers(
    ecs_world_t *world)
{
    flecs_sparse_free(world->observers);
}

/* Cleanup stages */
static
void fini_stages(
    ecs_world_t *world)
{
    flecs_stage_deinit(world, &world->stage);
    ecs_set_stages(world, 0);
}

/* Cleanup id index */
static
void fini_id_index(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(world->id_index);
    ecs_id_record_t *r;
    while ((r = ecs_map_next(&it, ecs_id_record_t, NULL))) {
        ecs_map_free(r->table_index);
    }

    ecs_map_free(world->id_index);
}

static
void fini_id_triggers(
    ecs_world_t *world)
{
    ecs_map_iter_t it = ecs_map_iter(world->id_triggers);
    ecs_id_trigger_t *t;
    while ((t = ecs_map_next(&it, ecs_id_trigger_t, NULL))) {
        ecs_map_free(t->on_add_triggers);
        ecs_map_free(t->on_remove_triggers);
        ecs_map_free(t->on_set_triggers);
        ecs_map_free(t->un_set_triggers);
    }
    ecs_map_free(world->id_triggers);
    flecs_sparse_free(world->triggers);
}

/* Cleanup aliases & symbols */
static
void fini_aliases(
    ecs_hashmap_t *map)
{
    flecs_hashmap_iter_t it = flecs_hashmap_iter(*map);
    ecs_string_t *key;
    while (flecs_hashmap_next_w_key(&it, ecs_string_t, &key, ecs_entity_t)) {
        ecs_os_free(key->value);
    }
    
    flecs_hashmap_free(*map);
}

/* Cleanup misc structures */
static
void fini_misc(
    ecs_world_t *world)
{
    on_demand_in_map_fini(world->on_activate_components);
    on_demand_in_map_fini(world->on_enable_components);
    ecs_map_free(world->type_handles);
    ecs_vector_free(world->fini_tasks);
    monitors_fini(&world->monitors);
}

/* The destroyer of worlds */
int ecs_fini(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    world->is_fini = true;

    /* Operations invoked during UnSet/OnRemove/destructors are deferred and
     * will be discarded after world cleanup */
    ecs_defer_begin(world);

    /* Run UnSet/OnRemove actions for components while the store is still
     * unmodified by cleanup. */
    fini_unset_tables(world);
    
    /* Run fini actions (simple callbacks ran when world is deleted) before
     * destroying the storage */
    fini_actions(world);

    /* This will destroy all entities and components. After this point no more
     * user code is executed. */
    fini_store(world);

    /* Purge deferred operations from the queue. This discards operations but
     * makes sure that any resources in the queue are freed */
    flecs_defer_purge(world, &world->stage);

    /* Entity index is kept alive until this point so that user code can do
     * validity checks on entity ids, even though after store cleanup the index
     * will be empty, so all entity ids are invalid. */
    flecs_sparse_free(world->store.entity_index);
    
    if (world->locking_enabled) {
        ecs_os_mutex_free(world->mutex);
    }

    fini_stages(world);

    fini_component_lifecycle(world);

    fini_queries(world);

    fini_observers(world);

    fini_id_index(world);

    fini_id_triggers(world);

    fini_aliases(&world->aliases);
    
    fini_aliases(&world->symbols);
    
    fini_misc(world);

    /* In case the application tries to use the memory of the freed world, this
     * will trigger an assert */
    world->magic = 0;

    flecs_increase_timer_resolution(0);

    /* End of the world */
    ecs_os_free(world);

    ecs_os_fini(); 

    return 0;
}

void ecs_dim(
    ecs_world_t *world,
    int32_t entity_count)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_eis_set_size(world, entity_count + ECS_HI_COMPONENT_ID);
}

void flecs_eval_component_monitors(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);    
    eval_component_monitor(world);
}

void ecs_measure_frame_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (world->stats.target_fps == 0.0f || enable) {
        world->measure_frame_time = enable;
    }
}

void ecs_measure_system_time(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);
    world->measure_system_time = enable;
}

/* Increase timer resolution based on target fps */
static 
void set_timer_resolution(
    FLECS_FLOAT fps)
{
    if(fps >= 60.0f) flecs_increase_timer_resolution(1);
    else flecs_increase_timer_resolution(0);
}

void ecs_set_target_fps(
    ecs_world_t *world,
    FLECS_FLOAT fps)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(ecs_os_has_time(), ECS_MISSING_OS_API, NULL);

    if (!world->arg_fps) {
        ecs_measure_frame_time(world, true);
        world->stats.target_fps = fps;
        set_timer_resolution(fps);
    }
}

void* ecs_get_context(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    world = ecs_get_world(world);
    return world->context;
}

void ecs_set_context(
    ecs_world_t *world,
    void *context)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    world->context = context;
}

void ecs_set_entity_range(
    ecs_world_t *world,
    ecs_entity_t id_start,
    ecs_entity_t id_end)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(!id_end || id_end > id_start, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!id_end || id_end > world->stats.last_id, ECS_INVALID_PARAMETER, NULL);

    if (world->stats.last_id < id_start) {
        world->stats.last_id = id_start - 1;
    }

    world->stats.min_id = id_start;
    world->stats.max_id = id_end;
}

bool ecs_enable_range_check(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    bool old_value = world->range_check_enabled;
    world->range_check_enabled = enable;
    return old_value;
}

int32_t ecs_get_threads(
    ecs_world_t *world)
{
    return ecs_vector_count(world->worker_stages);
}

bool ecs_enable_locking(
    ecs_world_t *world,
    bool enable)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);

    if (enable) {
        if (!world->locking_enabled) {
            world->mutex = ecs_os_mutex_new();
            world->thr_sync = ecs_os_mutex_new();
            world->thr_cond = ecs_os_cond_new();
        }
    } else {
        if (world->locking_enabled) {
            ecs_os_mutex_free(world->mutex);
            ecs_os_mutex_free(world->thr_sync);
            ecs_os_cond_free(world->thr_cond);
        }
    }

    bool old = world->locking_enabled;
    world->locking_enabled = enable;
    return old;
}

void ecs_lock(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->mutex);
}

void ecs_unlock(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->mutex);
}

void ecs_begin_wait(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_lock(world->thr_sync);
    ecs_os_cond_wait(world->thr_cond, world->thr_sync);
}

void ecs_end_wait(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);    
    ecs_assert(world->locking_enabled, ECS_INVALID_PARAMETER, NULL);
    ecs_os_mutex_unlock(world->thr_sync);
}

const ecs_type_info_t * flecs_get_c_info(
    const ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);   

    ecs_assert(component != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!(component & ECS_ROLE_MASK), ECS_INTERNAL_ERROR, NULL);

    return flecs_sparse_get(world->type_info, ecs_type_info_t, component);
}

ecs_type_info_t * flecs_get_or_create_c_info(
    ecs_world_t *world,
    ecs_entity_t component)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    const ecs_type_info_t *c_info = flecs_get_c_info(world, component);
    ecs_type_info_t *c_info_mut = NULL;
    if (!c_info) {
        c_info_mut = flecs_sparse_ensure(
            world->type_info, ecs_type_info_t, component);
        ecs_assert(c_info_mut != NULL, ECS_INTERNAL_ERROR, NULL);         
    } else {
        c_info_mut = (ecs_type_info_t*)c_info;
    }

    return c_info_mut;
}

static
FLECS_FLOAT insert_sleep(
    ecs_world_t *world,
    ecs_time_t *stop)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    ecs_time_t start = *stop;
    FLECS_FLOAT delta_time = (FLECS_FLOAT)ecs_time_measure(stop);

    if (world->stats.target_fps == (FLECS_FLOAT)0.0) {
        return delta_time;
    }

    FLECS_FLOAT target_delta_time = 
        ((FLECS_FLOAT)1.0 / (FLECS_FLOAT)world->stats.target_fps);

    /* Calculate the time we need to sleep by taking the measured delta from the
     * previous frame, and subtracting it from target_delta_time. */
    FLECS_FLOAT sleep = target_delta_time - delta_time;

    /* Pick a sleep interval that is 20 times lower than the time one frame
     * should take. This means that this function at most iterates 20 times in
     * a busy loop */
    FLECS_FLOAT sleep_time = sleep / (FLECS_FLOAT)4.0;

    do {
        /* Only call sleep when sleep_time is not 0. On some platforms, even
         * a sleep with a timeout of 0 can cause stutter. */
        if (sleep_time != 0) {
            ecs_sleepf((double)sleep_time);
        }

        ecs_time_t now = start;
        delta_time = (FLECS_FLOAT)ecs_time_measure(&now);
    } while ((target_delta_time - delta_time) > 
        (sleep_time / (FLECS_FLOAT)2.0));

    return delta_time;
}

static
FLECS_FLOAT start_measure_frame(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    FLECS_FLOAT delta_time = 0;

    if (world->measure_frame_time || (user_delta_time == 0)) {
        ecs_time_t t = world->frame_start_time;
        do {
            if (world->frame_start_time.sec) {
                delta_time = insert_sleep(world, &t);

                ecs_time_measure(&t);
            } else {
                ecs_time_measure(&t);
                if (world->stats.target_fps != 0) {
                    delta_time = (FLECS_FLOAT)1.0 / world->stats.target_fps;
                } else {
                    /* Best guess */
                    delta_time = (FLECS_FLOAT)1.0 / (FLECS_FLOAT)60.0; 
                }
            }
        
        /* Keep trying while delta_time is zero */
        } while (delta_time == 0);

        world->frame_start_time = t;  

        /* Keep track of total time passed in world */
        world->stats.world_time_total_raw += (FLECS_FLOAT)delta_time;
    }

    return (FLECS_FLOAT)delta_time;
}

static
void stop_measure_frame(
    ecs_world_t* world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);  

    if (world->measure_frame_time) {
        ecs_time_t t = world->frame_start_time;
        world->stats.frame_time_total += (FLECS_FLOAT)ecs_time_measure(&t);
    }
}

FLECS_FLOAT ecs_frame_begin(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);

    ecs_assert(user_delta_time != 0 || ecs_os_has_time(), ECS_MISSING_OS_API, "get_time");

    if (world->locking_enabled) {
        ecs_lock(world);
    }

    /* Start measuring total frame time */
    FLECS_FLOAT delta_time = start_measure_frame(world, user_delta_time);
    if (user_delta_time == 0) {
        user_delta_time = delta_time;
    }  

    world->stats.delta_time_raw = user_delta_time;
    world->stats.delta_time = user_delta_time * world->stats.time_scale;

    /* Keep track of total scaled time passed in world */
    world->stats.world_time_total += world->stats.delta_time;

    flecs_eval_component_monitors(world);

    return world->stats.delta_time;
}

void ecs_frame_end(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);
    ecs_assert(world->is_readonly == false, ECS_INVALID_OPERATION, NULL);

    world->stats.frame_count_total ++;

    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        flecs_stage_merge_post_frame(world, stage);
    });        

    if (world->locking_enabled) {
        ecs_unlock(world);

        ecs_os_mutex_lock(world->thr_sync);
        ecs_os_cond_broadcast(world->thr_cond);
        ecs_os_mutex_unlock(world->thr_sync);
    }

    stop_measure_frame(world);
}

const ecs_world_info_t* ecs_get_world_info(
    const ecs_world_t *world)
{
    world = ecs_get_world(world);
    return &world->stats;
}

void flecs_notify_queries(
    ecs_world_t *world,
    ecs_query_event_t *event)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL); 

    int32_t i, count = flecs_sparse_count(world->queries);
    for (i = 0; i < count; i ++) {
        flecs_query_notify(world, 
            flecs_sparse_get_dense(world->queries, ecs_query_t, i), event);
    }    
}

void flecs_delete_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL); 

    /* Notify queries that table is to be removed */
    flecs_notify_queries(
        world, &(ecs_query_event_t){
            .kind = EcsQueryTableUnmatch,
            .table = table
        });

    uint64_t id = table->id;

    /* Free resources associated with table */
    flecs_table_free(world, table);
    flecs_table_free_type(table);

    /* Remove table from sparse set */
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);
    flecs_sparse_remove(world->store.tables, id);
}

static
void register_table_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column)
{
    ecs_id_record_t *r = flecs_ensure_id_record(world, id);
    ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!r->table_index) {
        r->table_index = ecs_map_new(ecs_table_record_t, 1);
    }

    ecs_table_record_t *tr = ecs_map_ensure(
        r->table_index, ecs_table_record_t, table->id);

    /* A table can be registered for the same entity multiple times if this is
     * a trait. In that case make sure the column with the first occurrence is
     * registered with the index */
    if (!tr->table || column < tr->column) {
        tr->table = table;
        tr->column = column;
        tr->count = 1;
    } else {
        tr->count ++;
    }

    char buf[255]; ecs_id_str(world, id, buf, 255);

    /* Set flags if triggers are registered for table */
    if (!(table->flags & EcsTableIsDisabled)) {
        if (flecs_triggers_get(world, id, EcsOnAdd)) {
            table->flags |= EcsTableHasOnAdd;
        }
        if (flecs_triggers_get(world, id, EcsOnRemove)) {
            table->flags |= EcsTableHasOnRemove;
        }
        if (flecs_triggers_get(world, id, EcsOnSet)) {
            table->flags |= EcsTableHasOnSet;
        }
        if (flecs_triggers_get(world, id, EcsUnSet)) {
            table->flags |= EcsTableHasUnSet;
        }                
    }
}

static
void unregister_table_for_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_id_record_t *r = flecs_get_id_record(world, id);
    if (!r || !r->table_index) {
        return;
    }

    ecs_map_remove(r->table_index, table->id);
    if (!ecs_map_count(r->table_index)) {
        flecs_clear_id_record(world, id);
    }
}

static
void do_register_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id,
    int32_t column,
    bool unregister)
{
    if (unregister) {
        unregister_table_for_id(world, table, id);
    } else {
        register_table_for_id(world, table, id, column);
    }
}

static
void do_register_each_id(
    ecs_world_t *world,
    ecs_table_t *table,
    bool unregister)
{
    int32_t i, count = ecs_vector_count(table->type);
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    bool has_childof = false;
    
    for (i = 0; i < count; i ++) {
        ecs_entity_t id = ids[i];

        /* This check ensures that legacy CHILDOF works */
        if (ECS_HAS_RELATION(id, EcsChildOf)) {
            has_childof = true;
        } 

        do_register_id(world, table, id, i, unregister);
        do_register_id(world, table, EcsWildcard, i, unregister);

        if (ECS_HAS_ROLE(id, PAIR)) {
            ecs_entity_t pred_w_wildcard = ecs_pair(
                ECS_PAIR_RELATION(id), EcsWildcard);       
            do_register_id(world, table, pred_w_wildcard, i, unregister);

            ecs_entity_t obj_w_wildcard = ecs_pair(
                EcsWildcard, ECS_PAIR_OBJECT(id));
            do_register_id(world, table, obj_w_wildcard, i, unregister);

            ecs_entity_t all_wildcard = ecs_pair(EcsWildcard, EcsWildcard);
            do_register_id(world, table, all_wildcard, i, unregister);

            if (!unregister) {
                flecs_set_watch(world, ecs_pair_relation(world, id));
                flecs_set_watch(world, ecs_pair_object(world, id));
            }
        } else {
            if (id & ECS_ROLE_MASK) {
                id &= ECS_COMPONENT_MASK;
                do_register_id(world, table, ecs_pair(id, EcsWildcard), 
                    i, unregister);
            }

            if (!unregister) {
                flecs_set_watch(world, id);
            }            
        }
    }

    if (!has_childof) {
        do_register_id(world, table, ecs_pair(EcsChildOf, 0), 0, unregister);
    }
}

void flecs_register_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    do_register_each_id(world, table, false);
}

void flecs_unregister_table(
    ecs_world_t *world,
    ecs_table_t *table)
{
    /* Remove table from id indices */
    do_register_each_id(world, table, true);

    /* Remove table from table map */
    ecs_ids_t key = {
        .array = ecs_vector_first(table->type, ecs_id_t),
        .count = ecs_vector_count(table->type)
    };

    flecs_hashmap_remove(world->store.table_map, &key, ecs_table_t*);
}

ecs_id_record_t* flecs_ensure_id_record(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return ecs_map_ensure(world->id_index, ecs_id_record_t, id);
}

ecs_id_record_t* flecs_get_id_record(
    const ecs_world_t *world,
    ecs_id_t id)
{
    return ecs_map_get(world->id_index, ecs_id_record_t, id);
}

ecs_table_record_t* flecs_get_table_record(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id)
{
    ecs_id_record_t* idr = flecs_get_id_record(world, id);
    if (!idr) {
        return NULL;
    }

    return ecs_map_get(idr->table_index, ecs_table_record_t, table->id);
}

void flecs_clear_id_record(
    const ecs_world_t *world,
    ecs_id_t id)    
{
    ecs_id_record_t *r = flecs_get_id_record(world, id);
    if (!r) {
        return;
    }

    ecs_map_free(r->table_index);
    ecs_map_remove(world->id_index, id);
}

#ifdef FLECS_SANITIZE
static 
void verify_nodes(
    flecs_switch_header_t *hdr,
    flecs_switch_node_t *nodes)
{
    if (!hdr) {
        return;
    }

    int32_t prev = -1, elem = hdr->element, count = 0;
    while (elem != -1) {
        ecs_assert(prev == nodes[elem].prev, ECS_INTERNAL_ERROR, NULL);
        prev = elem;
        elem = nodes[elem].next;
        count ++;
    }

    ecs_assert(count == hdr->count, ECS_INTERNAL_ERROR, NULL);
}
#else
#define verify_nodes(hdr, nodes)
#endif

static
flecs_switch_header_t *get_header(
    const ecs_switch_t *sw,
    uint64_t value)
{
    if (value == 0) {
        return NULL;
    }

    value = (uint32_t)value;

    ecs_assert(value >= sw->min, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(value <= sw->max, ECS_INTERNAL_ERROR, NULL);

    uint64_t index = value - sw->min;

    return &sw->headers[index];
}

static
void remove_node(
    flecs_switch_header_t *hdr,
    flecs_switch_node_t *nodes,
    flecs_switch_node_t *node,
    int32_t element)
{
    ecs_assert(&nodes[element] == node, ECS_INTERNAL_ERROR, NULL);

    /* Update previous node/header */
    if (hdr->element == element) {
        ecs_assert(node->prev == -1, ECS_INVALID_PARAMETER, NULL);
        /* If this is the first node, update the header */
        hdr->element = node->next;
    } else {
        /* If this is not the first node, update the previous node to the 
         * removed node's next ptr */
        ecs_assert(node->prev != -1, ECS_INVALID_PARAMETER, NULL);
        flecs_switch_node_t *prev_node = &nodes[node->prev];
        prev_node->next = node->next;
    }

    /* Update next node */
    int32_t next = node->next;
    if (next != -1) {
        ecs_assert(next >= 0, ECS_INVALID_PARAMETER, NULL);
        /* If this is not the last node, update the next node to point to the
         * removed node's prev ptr */
        flecs_switch_node_t *next_node = &nodes[next];
        next_node->prev = node->prev;
    }

    /* Decrease count of current header */
    hdr->count --;
    ecs_assert(hdr->count >= 0, ECS_INTERNAL_ERROR, NULL);
}

ecs_switch_t* flecs_switch_new(
    uint64_t min, 
    uint64_t max,
    int32_t elements)
{
    ecs_assert(min <= max, ECS_INVALID_PARAMETER, NULL);

    /* Min must be larger than 0, as 0 is an invalid entity id, and should
     * therefore never occur as case id */
    ecs_assert(min > 0, ECS_INVALID_PARAMETER, NULL);

    ecs_switch_t *result = ecs_os_malloc(ECS_SIZEOF(ecs_switch_t));
    result->min = (uint32_t)min;
    result->max = (uint32_t)max;

    int32_t count = (int32_t)(max - min) + 1;
    result->headers = ecs_os_calloc(ECS_SIZEOF(flecs_switch_header_t) * count);
    result->nodes = ecs_vector_new(flecs_switch_node_t, elements);
    result->values = ecs_vector_new(uint64_t, elements);

    int64_t i;
    for (i = 0; i < count; i ++) {
        result->headers[i].element = -1;
        result->headers[i].count = 0;
    }

    flecs_switch_node_t *nodes = ecs_vector_first(
        result->nodes, flecs_switch_node_t);
    uint64_t *values = ecs_vector_first(
        result->values, uint64_t);        

    for (i = 0; i < elements; i ++) {
        nodes[i].prev = -1;
        nodes[i].next = -1;
        values[i] = 0;
    }

    return result;
}

void flecs_switch_free(
    ecs_switch_t *sw)
{
    ecs_os_free(sw->headers);
    ecs_vector_free(sw->nodes);
    ecs_vector_free(sw->values);
    ecs_os_free(sw);
}

void flecs_switch_add(
    ecs_switch_t *sw)
{
    flecs_switch_node_t *node = ecs_vector_add(&sw->nodes, flecs_switch_node_t);
    uint64_t *value = ecs_vector_add(&sw->values, uint64_t);
    node->prev = -1;
    node->next = -1;
    *value = 0;
}

void flecs_switch_set_count(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    if (old_count == count) {
        return;
    }

    ecs_vector_set_count(&sw->nodes, flecs_switch_node_t, count);
    ecs_vector_set_count(&sw->values, uint64_t, count);

    flecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, flecs_switch_node_t);
    uint64_t *values = ecs_vector_first(sw->values, uint64_t);

    int32_t i;
    for (i = old_count; i < count; i ++) {
        flecs_switch_node_t *node = &nodes[i];
        node->prev = -1;
        node->next = -1;
        values[i] = 0;
    }
}

void flecs_switch_ensure(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    if (old_count >= count) {
        return;
    }

    flecs_switch_set_count(sw, count);
}

void flecs_switch_addn(
    ecs_switch_t *sw,
    int32_t count)
{
    int32_t old_count = ecs_vector_count(sw->nodes);
    flecs_switch_set_count(sw, old_count + count);
}

void flecs_switch_set(
    ecs_switch_t *sw,
    int32_t element,
    uint64_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->values), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vector_first(sw->values, uint64_t);
    uint64_t cur_value = values[element];

    /* If the node is already assigned to the value, nothing to be done */
    if (cur_value == value) {
        return;
    }

    flecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, flecs_switch_node_t);
    flecs_switch_node_t *node = &nodes[element];

    flecs_switch_header_t *cur_hdr = get_header(sw, cur_value);
    flecs_switch_header_t *dst_hdr = get_header(sw, value);

    verify_nodes(cur_hdr, nodes);
    verify_nodes(dst_hdr, nodes);

    /* If value is not 0, and dst_hdr is NULL, then this is not a valid value
     * for this switch */
    ecs_assert(dst_hdr != NULL || !value, ECS_INVALID_PARAMETER, NULL);

    if (cur_hdr) {
        remove_node(cur_hdr, nodes, node, element);
    }

    /* Now update the node itself by adding it as the first node of dst */
    node->prev = -1;
    values[element] = value;

    if (dst_hdr) {
        node->next = dst_hdr->element;

        /* Also update the dst header */
        int32_t first = dst_hdr->element;
        if (first != -1) {
            ecs_assert(first >= 0, ECS_INTERNAL_ERROR, NULL);
            flecs_switch_node_t *first_node = &nodes[first];
            first_node->prev = element;
        }

        dst_hdr->element = element;
        dst_hdr->count ++;        
    }
}

void flecs_switch_remove(
    ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vector_first(sw->values, uint64_t);
    uint64_t value = values[element];
    flecs_switch_node_t *nodes = ecs_vector_first(sw->nodes, flecs_switch_node_t);
    flecs_switch_node_t *node = &nodes[element];

    /* If node is currently assigned to a case, remove it from the list */
    if (value != 0) {
        flecs_switch_header_t *hdr = get_header(sw, value);
        ecs_assert(hdr != NULL, ECS_INTERNAL_ERROR, NULL);

        verify_nodes(hdr, nodes);
        remove_node(hdr, nodes, node, element);
    }

    int32_t last_elem = ecs_vector_count(sw->nodes) - 1;
    if (last_elem != element) {
        flecs_switch_node_t *last = ecs_vector_last(sw->nodes, flecs_switch_node_t);
        int32_t next = last->next, prev = last->prev;
        if (next != -1) {
            flecs_switch_node_t *n = &nodes[next];
            n->prev = element;
        }

        if (prev != -1) {
            flecs_switch_node_t *n = &nodes[prev];
            n->next = element;
        } else {
            flecs_switch_header_t *hdr = get_header(sw, values[last_elem]);
            if (hdr && hdr->element != -1) {
                ecs_assert(hdr->element == last_elem, 
                    ECS_INTERNAL_ERROR, NULL);
                hdr->element = element;
            }
        }
    }

    /* Remove element from arrays */
    ecs_vector_remove(sw->nodes, flecs_switch_node_t, element);
    ecs_vector_remove(sw->values, uint64_t, element);
}

uint64_t flecs_switch_get(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->values), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    uint64_t *values = ecs_vector_first(sw->values, uint64_t);
    return values[element];
}

ecs_vector_t* flecs_switch_values(
    const ecs_switch_t *sw)
{
    return sw->values;
}

int32_t flecs_switch_case_count(
    const ecs_switch_t *sw,
    uint64_t value)
{
    flecs_switch_header_t *hdr = get_header(sw, value);
    if (!hdr) {
        return 0;
    }

    return hdr->count;
}

void flecs_switch_swap(
    ecs_switch_t *sw,
    int32_t elem_1,
    int32_t elem_2)
{
    uint64_t v1 = flecs_switch_get(sw, elem_1);
    uint64_t v2 = flecs_switch_get(sw, elem_2);

    flecs_switch_set(sw, elem_2, v1);
    flecs_switch_set(sw, elem_1, v2);
}

int32_t flecs_switch_first(
    const ecs_switch_t *sw,
    uint64_t value)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((uint32_t)value <= sw->max, ECS_INVALID_PARAMETER, NULL);
    ecs_assert((uint32_t)value >= sw->min, ECS_INVALID_PARAMETER, NULL);
    
    flecs_switch_header_t *hdr = get_header(sw, value);
    ecs_assert(hdr != NULL, ECS_INVALID_PARAMETER, NULL);

    return hdr->element;
}

int32_t flecs_switch_next(
    const ecs_switch_t *sw,
    int32_t element)
{
    ecs_assert(sw != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element < ecs_vector_count(sw->nodes), ECS_INVALID_PARAMETER, NULL);
    ecs_assert(element >= 0, ECS_INVALID_PARAMETER, NULL);

    flecs_switch_node_t *nodes = ecs_vector_first(
        sw->nodes, flecs_switch_node_t);

    return nodes[element].next;
}

#ifndef _MSC_VER
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

/* See explanation below. The hashing function may read beyond the memory passed
 * into the hashing function, but only at word boundaries. This should be safe,
 * but trips up address sanitizers and valgrind.
 * This ensures clean valgrind logs in debug mode & the best perf in release */
#ifndef NDEBUG
#define VALGRIND
#endif

/*
-------------------------------------------------------------------------------
lookup3.c, by Bob Jenkins, May 2006, Public Domain.
  http://burtleburtle.net/bob/c/lookup3.c
-------------------------------------------------------------------------------
*/

#ifdef _MSC_VER
//FIXME
#else
#include <sys/param.h>  /* attempt to define endianness */
#endif
#ifdef linux
# include <endian.h>    /* attempt to define endianness */
#endif

/*
 * My best guess at if you are big-endian or little-endian.  This may
 * need adjustment.
 */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && \
     __BYTE_ORDER == __LITTLE_ENDIAN) || \
    (defined(i386) || defined(__i386__) || defined(__i486__) || \
     defined(__i586__) || defined(__i686__) || defined(vax) || defined(MIPSEL))
# define HASH_LITTLE_ENDIAN 1
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) || \
      (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
# define HASH_LITTLE_ENDIAN 0
#else
# define HASH_LITTLE_ENDIAN 0
#endif

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

/*
-------------------------------------------------------------------------------
mix -- mix 3 32-bit values reversibly.
This is reversible, so any information in (a,b,c) before mix() is
still in (a,b,c) after mix().
If four pairs of (a,b,c) inputs are run through mix(), or through
mix() in reverse, there are at least 32 bits of the output that
are sometimes the same for one pair and different for another pair.
This was tested for:
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.
Some k values for my "a-=c; a^=rot(c,k); c+=b;" arrangement that
satisfy this are
    4  6  8 16 19  4
    9 15  3 18 27 15
   14  9  3  7 17  3
Well, "9 15 3 18 27 15" didn't quite get 32 bits diffing
for "differ" defined as + with a one-bit base and a two-bit delta.  I
used http://burtleburtle.net/bob/hash/avalanche.html to choose 
the operations, constants, and arrangements of the variables.
This does not achieve avalanche.  There are input bits of (a,b,c)
that fail to affect some output bits of (a,b,c), especially of a.  The
most thoroughly mixed value is c, but it doesn't really even achieve
avalanche in c.
This allows some parallelism.  Read-after-writes are good at doubling
the number of bits affected, so the goal of mixing pulls in the opposite
direction as the goal of parallelism.  I did what I could.  Rotates
seem to cost as much as shifts on every machine I could lay my hands
on, and rotates are much kinder to the top and bottom bits, so I used
rotates.
-------------------------------------------------------------------------------
*/
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);  c += b; \
  b -= a;  b ^= rot(a, 6);  a += c; \
  c -= b;  c ^= rot(b, 8);  b += a; \
  a -= c;  a ^= rot(c,16);  c += b; \
  b -= a;  b ^= rot(a,19);  a += c; \
  c -= b;  c ^= rot(b, 4);  b += a; \
}

/*
-------------------------------------------------------------------------------
final -- final mixing of 3 32-bit values (a,b,c) into c
Pairs of (a,b,c) values differing in only a few bits will usually
produce values of c that look totally different.  This was tested for
* pairs that differed by one bit, by two bits, in any combination
  of top bits of (a,b,c), or in any combination of bottom bits of
  (a,b,c).
* "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  is commonly produced by subtraction) look like a single 1-bit
  difference.
* the base values were pseudorandom, all zero but one bit set, or 
  all zero plus a counter that starts at zero.
These constants passed:
 14 11 25 16 4 14 24
 12 14 25 16 4 14 24
and these came close:
  4  8 15 26 3 22 24
 10  8 15 26 3 22 24
 11  8 15 26 3 22 24
-------------------------------------------------------------------------------
*/
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c,4);  \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}


/*
 * hashlittle2: return 2 32-bit hash values
 *
 * This is identical to hashlittle(), except it returns two 32-bit hash
 * values instead of just one.  This is good enough for hash table
 * lookup with 2^^64 buckets, or if you want a second hash if you're not
 * happy with the first, or if you want a probably-unique 64-bit ID for
 * the key.  *pc is better mixed than *pb, so use *pc first.  If you want
 * a 64-bit value do something like "*pc + (((uint64_t)*pb)<<32)".
 */
static
void hashlittle2( 
  const void *key,       /* the key to hash */
  size_t      length,    /* length of the key */
  uint32_t   *pc,        /* IN: primary initval, OUT: primary hash */
  uint32_t   *pb)        /* IN: secondary initval, OUT: secondary hash */
{
  uint32_t a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;     /* needed for Mac Powerbook G4 */

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((uint32_t)length) + *pc;
  c += *pb;

  u.ptr = key;
  if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0)) {
    const uint32_t *k = (const uint32_t *)key;         /* read 32-bit chunks */
    const uint8_t  *k8;
    (void)k8;

    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
    }

    /*----------------------------- handle the last (probably partial) block */
    /* 
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */
#ifndef VALGRIND

    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
    case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
    case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
    case 6 : b+=k[1]&0xffff; a+=k[0]; break;
    case 5 : b+=k[1]&0xff; a+=k[0]; break;
    case 4 : a+=k[0]; break;
    case 3 : a+=k[0]&0xffffff; break;
    case 2 : a+=k[0]&0xffff; break;
    case 1 : a+=k[0]&0xff; break;
    case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
    }

#else /* make valgrind happy */

    k8 = (const uint8_t *)k;
    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=((uint32_t)k8[10])<<16;  /* fall through */
    case 10: c+=((uint32_t)k8[9])<<8;    /* fall through */
    case 9 : c+=k8[8];                   /* fall through */
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=((uint32_t)k8[6])<<16;   /* fall through */
    case 6 : b+=((uint32_t)k8[5])<<8;    /* fall through */
    case 5 : b+=k8[4];                   /* fall through */
    case 4 : a+=k[0]; break;
    case 3 : a+=((uint32_t)k8[2])<<16;   /* fall through */
    case 2 : a+=((uint32_t)k8[1])<<8;    /* fall through */
    case 1 : a+=k8[0]; break;
    case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
    }

#endif /* !valgrind */

  } else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0)) {
    const uint16_t *k = (const uint16_t *)key;         /* read 16-bit chunks */
    const uint8_t  *k8;

    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
    {
      a += k[0] + (((uint32_t)k[1])<<16);
      b += k[2] + (((uint32_t)k[3])<<16);
      c += k[4] + (((uint32_t)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
    }

    /*----------------------------- handle the last (probably partial) block */
    k8 = (const uint8_t *)k;
    switch(length)
    {
    case 12: c+=k[4]+(((uint32_t)k[5])<<16);
             b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 11: c+=((uint32_t)k8[10])<<16;     /* fall through */
    case 10: c+=k[4];
             b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 9 : c+=k8[8];                      /* fall through */
    case 8 : b+=k[2]+(((uint32_t)k[3])<<16);
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 7 : b+=((uint32_t)k8[6])<<16;      /* fall through */
    case 6 : b+=k[2];
             a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 5 : b+=k8[4];                      /* fall through */
    case 4 : a+=k[0]+(((uint32_t)k[1])<<16);
             break;
    case 3 : a+=((uint32_t)k8[2])<<16;      /* fall through */
    case 2 : a+=k[0];
             break;
    case 1 : a+=k8[0];
             break;
    case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
    }

  } else {                        /* need to read the key one byte at a time */
    const uint8_t *k = (const uint8_t *)key;

    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      a += ((uint32_t)k[1])<<8;
      a += ((uint32_t)k[2])<<16;
      a += ((uint32_t)k[3])<<24;
      b += k[4];
      b += ((uint32_t)k[5])<<8;
      b += ((uint32_t)k[6])<<16;
      b += ((uint32_t)k[7])<<24;
      c += k[8];
      c += ((uint32_t)k[9])<<8;
      c += ((uint32_t)k[10])<<16;
      c += ((uint32_t)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
    }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
    {
    case 12: c+=((uint32_t)k[11])<<24;
    case 11: c+=((uint32_t)k[10])<<16;
    case 10: c+=((uint32_t)k[9])<<8;
    case 9 : c+=k[8];
    case 8 : b+=((uint32_t)k[7])<<24;
    case 7 : b+=((uint32_t)k[6])<<16;
    case 6 : b+=((uint32_t)k[5])<<8;
    case 5 : b+=k[4];
    case 4 : a+=((uint32_t)k[3])<<24;
    case 3 : a+=((uint32_t)k[2])<<16;
    case 2 : a+=((uint32_t)k[1])<<8;
    case 1 : a+=k[0];
             break;
    case 0 : *pc=c; *pb=b; return;  /* zero length strings require no mixing */
    }
  }

  final(a,b,c);
  *pc=c; *pb=b;
}

uint64_t flecs_hash(
    const void *data,
    ecs_size_t length)
{
    uint32_t h_1 = 0;
    uint32_t h_2 = 0;

    hashlittle2(
        data,
        flecs_to_size_t(length),
        &h_1,
        &h_2);

    return h_1 | ((uint64_t)h_2 << 32);
}


static
int resolve_identifier(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_id_t *identifier)
{
    if (!identifier->name) {
        return 0;
    }

    if (identifier->var == EcsVarDefault) {
        if (ecs_identifier_is_var(identifier->name)) {
            identifier->var = EcsVarIsVariable;
        }
    }

    if (identifier->var != EcsVarIsVariable) {
        if (ecs_identifier_is_0(identifier->name)) {
            identifier->entity = 0;
        } else {
            ecs_entity_t e = ecs_lookup_symbol(world, identifier->name, true);
            if (!e) {
                ecs_parser_error(name, expr, 0,
                    "unresolved identifier '%s'", identifier->name);
                return -1;
            }

            /* Use OR, as entity may have already been populated with role */
            identifier->entity = e;
        }
    }

    return 0;
}

bool ecs_identifier_is_0(
    const char *id)
{
    return id[0] == '0' && !id[1];
}

bool ecs_identifier_is_var(
    const char *id)
{
    if (id[0] == '_') {
        return true;
    }

    if (isdigit(id[0])) {
        return false;
    }

    const char *ptr;
    char ch;
    for (ptr = id; (ch = *ptr); ptr ++) {
        if (!isupper(ch) && ch != '_' && !isdigit(ch)) {
            return false;
        }
    }

    return true;
}

static
int term_resolve_ids(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term)
{
    if (resolve_identifier(world, name, expr, &term->pred)) {
        return -1;
    }
    if (resolve_identifier(world, name, expr, &term->args[0])) {
        return -1;
    }
    if (resolve_identifier(world, name, expr, &term->args[1])) {
        return -1;
    }

    if (term->args[1].entity || term->role == ECS_PAIR) {
        /* Both the relation and object must be set */
        ecs_assert(term->pred.entity != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(term->args[1].entity != 0, ECS_INVALID_PARAMETER, NULL);
        term->id = ecs_pair(term->pred.entity, term->args[1].entity);
    } else {
        term->id = term->pred.entity;
    }

    return 0;
}

bool ecs_id_match(
    ecs_id_t id,
    ecs_id_t pattern)
{
    if (id == pattern) {
        return true;
    }

    if (ECS_HAS_ROLE(pattern, PAIR)) {
        if (!ECS_HAS_ROLE(id, PAIR)) {
            return false;
        }

        ecs_entity_t id_rel = ECS_PAIR_RELATION(id);
        ecs_entity_t id_obj = ECS_PAIR_OBJECT(id);
        ecs_entity_t pattern_rel = ECS_PAIR_RELATION(pattern);
        ecs_entity_t pattern_obj = ECS_PAIR_OBJECT(pattern);

        ecs_assert(id_rel != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(id_obj != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_assert(pattern_rel != 0, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(pattern_obj != 0, ECS_INVALID_PARAMETER, NULL);
        
        if (pattern_rel == EcsWildcard) {
            if (pattern_obj == EcsWildcard || pattern_obj == id_obj) {
                return true;
            }
        } else if (pattern_obj == EcsWildcard) {
            if (pattern_rel == id_rel) {
                return true;
            }
        }
    } else {
        if ((id & ECS_ROLE_MASK) != (pattern & ECS_ROLE_MASK)) {
            return false;
        }

        if ((ECS_COMPONENT_MASK & pattern) == EcsWildcard) {
            return true;
        }
    }

    return false;
}

bool ecs_id_is_wildcard(
    ecs_id_t id)
{
    if (id == EcsWildcard) {
        return true;
    } else if (ECS_HAS_ROLE(id, PAIR)) {
        return ECS_PAIR_RELATION(id) == EcsWildcard || 
               ECS_PAIR_OBJECT(id) == EcsWildcard;
    }

    return false;
}

bool ecs_term_id_is_set(
    const ecs_term_id_t *id)
{
    return id->entity != 0 || id->name != NULL;
}

bool ecs_term_is_initialized(
    const ecs_term_t *term)
{
    return term->id != 0 || ecs_term_id_is_set(&term->pred);
}

bool ecs_term_is_trivial(
    const ecs_term_t *term)
{
    if (term->inout != EcsInOutDefault) {
        return false;
    }

    if (term->args[0].entity != EcsThis) {
        return false;
    }

    if (term->args[0].set.mask != EcsDefaultSet) {
        return false;
    }

    if (term->oper != EcsAnd && term->oper != EcsAndFrom) {
        return false;
    }

    if (term->name != NULL) {
        return false;
    }

    return true;
}

int ecs_term_finalize(
    const ecs_world_t *world,
    const char *name,
    const char *expr,
    ecs_term_t *term)
{
    if (term->id) {
        /* Allow for combining explicit object with id */
        if (term->args[1].name && !term->args[1].entity) {
            if (resolve_identifier(world, name, expr, &term->args[1])) {
                return -1;
            }
        }

        /* If other fields are set, make sure they are consistent with id */
        if (term->args[1].entity) {
            ecs_assert(term->pred.entity != 0, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(term->id == 
                ecs_pair(term->pred.entity, term->args[1].entity), 
                    ECS_INVALID_PARAMETER, NULL);
        } else if (term->pred.entity) {
            /* If only predicate is set (not object) it must match the id
             * without any roles set. */
            ecs_assert(term->pred.entity == (term->id & ECS_COMPONENT_MASK), 
                ECS_INVALID_PARAMETER, NULL);
        }

        /* If id is set, check for pair and derive predicate and object */
        if (ECS_HAS_ROLE(term->id, PAIR)) {
            term->pred.entity = ECS_PAIR_RELATION(term->id);
            term->args[1].entity = ECS_PAIR_OBJECT(term->id);
        } else {
            term->pred.entity = term->id & ECS_COMPONENT_MASK;
        }

        if (!term->role) {
            term->role = term->id & ECS_ROLE_MASK;
        } else {
            /* If id already has a role set it should be equal to the provided
             * role */
            ecs_assert(!(term->id & ECS_ROLE_MASK) || 
                        (term->id & ECS_ROLE_MASK) == term->role, 
                            ECS_INVALID_PARAMETER, NULL);
        }      
    } else {
        if (term_resolve_ids(world, name, expr, term)) {
            /* One or more identifiers could not be resolved */
            return -1;
        }
    }

    /* role field should only set role bits */
    ecs_assert(term->role == (term->role & ECS_ROLE_MASK), 
        ECS_INVALID_PARAMETER, NULL);    

    term->id |= term->role;

    if (!term->args[0].entity && 
        term->args[0].set.mask != EcsNothing && 
        term->args[0].var != EcsVarIsVariable) 
    {
        term->args[0].entity = EcsThis;
    }

    if (term->args[0].set.mask & (EcsSuperSet | EcsSubSet)) {
        if (!term->args[0].set.relation) {
            term->args[0].set.relation = EcsIsA;
        }
    }

    return 0;
}

ecs_term_t ecs_term_copy(
    const ecs_term_t *src)
{
    ecs_term_t dst = *src;
    dst.name = ecs_os_strdup(src->name);
    dst.pred.name = ecs_os_strdup(src->pred.name);
    dst.args[0].name = ecs_os_strdup(src->args[0].name);
    dst.args[1].name = ecs_os_strdup(src->args[1].name);

    return dst;
}

ecs_term_t ecs_term_move(
    ecs_term_t *src)
{
    if (src->move) {
        ecs_term_t dst = *src;
        src->name = NULL;
        src->pred.name = NULL;
        src->args[0].name = NULL;
        src->args[1].name = NULL;
        return dst;
    } else {
        return ecs_term_copy(src);
    }
}

void ecs_term_fini(
    ecs_term_t *term)
{
    ecs_os_free(term->pred.name);
    ecs_os_free(term->args[0].name);
    ecs_os_free(term->args[1].name);
    ecs_os_free(term->name);
}

int ecs_filter_finalize(
    const ecs_world_t *world,
    ecs_filter_t *f)
{
    int32_t i, term_count = f->term_count, actual_count = 0;
    ecs_term_t *terms = f->terms;
    bool is_or = false, prev_or = false;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];

        if (ecs_term_finalize(world, f->name, f->expr, term)) {
            return -1;
        }

        is_or = term->oper == EcsOr;
        actual_count += !(is_or && prev_or);
        term->index = actual_count - 1;
        prev_or = is_or;

        if (term->args[0].entity == EcsThis) {
            f->match_this = true;
            if (term->args[0].set.mask != EcsSelf) {
                f->match_only_this = false;
            }
        } else {
            f->match_only_this = false;
        }          
    }

    f->term_count_actual = actual_count;

    return 0;
}

int ecs_filter_init(
    const ecs_world_t *world,
    ecs_filter_t *filter_out,
    const ecs_filter_desc_t *desc)    
{
    int i, term_count = 0;
    ecs_term_t *terms = desc->terms_buffer;
    const char *name = desc->name;
    const char *expr = desc->expr;

    ecs_filter_t f = {
        /* Temporarily set the fields to the values provided in desc, until the
         * filter has been validated. */
        .name = (char*)name,
        .expr = (char*)expr
    };

    if (terms) {
        terms = desc->terms_buffer;
        term_count = desc->terms_buffer_count;
    } else {
        terms = (ecs_term_t*)desc->terms;
        for (i = 0; i < ECS_TERM_CACHE_SIZE; i ++) {
            if (!ecs_term_is_initialized(&terms[i])) {
                break;
            }

            term_count ++;
        }
    }

    /* Temporarily set array from desc to filter, until the filter has been
     * validated. */
    f.terms = terms;
    f.term_count = term_count;

    if (expr) {
#ifdef FLECS_PARSER
        int32_t buffer_count = 0;

        /* If terms have already been set, copy buffer to allocated one */
        if (terms && term_count) {
            terms = ecs_os_memdup(terms, term_count * ECS_SIZEOF(ecs_term_t));
            buffer_count = term_count;
        } else {
            terms = NULL;
        }

        /* Parse expression into array of terms */
        const char *ptr = desc->expr;
        ecs_term_t term = {0};
        while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))){
            if (!ecs_term_is_initialized(&term)) {
                break;
            }
            
            if (term_count == buffer_count) {
                buffer_count = buffer_count ? buffer_count * 2 : 8;
                terms = ecs_os_realloc(terms, 
                    buffer_count * ECS_SIZEOF(ecs_term_t));
            }

            terms[term_count] = term;
            term_count ++;
        }

        f.terms = terms;
        f.term_count = term_count;

        if (!ptr) {
            goto error;
        }
#else
        ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
#endif
    }

    /* If default substitution is enabled, replace DefaultSet with SuperSet */
    if (desc->substitute_default) {
        for (i = 0; i < term_count; i ++) {
            if (terms[i].args[0].set.mask == EcsDefaultSet) {
                terms[i].args[0].set.mask = EcsSuperSet | EcsSelf;
                terms[i].args[0].set.relation = EcsIsA;
            }            
        }
    }

    /* Ensure all fields are consistent and properly filled out */
    if (ecs_filter_finalize(world, &f)) {
        goto error;
    }

    *filter_out = f;

    /* Copy term resources. */
    if (term_count) {
        if (!filter_out->expr) {
            if (term_count < ECS_TERM_CACHE_SIZE) {
                filter_out->terms = filter_out->term_cache;
            } else {
                filter_out->terms = ecs_os_malloc_n(ecs_term_t, term_count);
            }
        }

        for (i = 0; i < term_count; i ++) {
            filter_out->terms[i] = ecs_term_move(&terms[i]);
        }        
    } else {
        filter_out->terms = NULL;
    }

    filter_out->name = ecs_os_strdup(desc->name);
    filter_out->expr = ecs_os_strdup(desc->expr);

    return 0;
error:
    /* NULL members that point to non-owned resources */
    if (!f.expr) {
        f.terms = NULL;
    }

    f.name = NULL;
    f.expr = NULL;

    ecs_filter_fini(&f);

    return -1;
}

void ecs_filter_move(
    ecs_filter_t *dst,
    ecs_filter_t *src)   
{
    if (src) {
        *dst = *src;

        if (src->terms == src->term_cache) {
            dst->terms = dst->term_cache;
        }
    } else {
        ecs_os_memset_t(dst, 0, ecs_filter_t);
    }
}

void ecs_filter_fini(
    ecs_filter_t *filter) 
{
    if (filter->terms) {
        int i, count = filter->term_count;
        for (i = 0; i < count; i ++) {
            ecs_term_fini(&filter->terms[i]);
        }

        if (filter->terms != filter->term_cache) {
            ecs_os_free(filter->terms);
        }
    }

    ecs_os_free(filter->name);
    ecs_os_free(filter->expr);
}

static
void filter_str_add_id(
    const ecs_world_t *world,
    ecs_strbuf_t *buf,
    ecs_term_id_t *id)
{
    if (id->name) {
        ecs_strbuf_appendstr(buf, id->name);
    } else if (id->entity) {
        char *path = ecs_get_fullpath(world, id->entity);
        ecs_strbuf_appendstr(buf, path);
        ecs_os_free(path);
    } else {
        ecs_strbuf_appendstr(buf, "0");
    }
}

char* ecs_filter_str(
    const ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;
    int32_t or_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];

        if (i) {
            if (terms[i - 1].oper == EcsOr && term->oper == EcsOr) {
                ecs_strbuf_appendstr(&buf, " || ");
            } else {
                ecs_strbuf_appendstr(&buf, ", ");
            }
        }

        if (or_count < 1) {
            if (term->inout == EcsIn) {
                ecs_strbuf_appendstr(&buf, "[in] ");
            } else if (term->inout == EcsInOut) {
                ecs_strbuf_appendstr(&buf, "[inout] ");
            } else if (term->inout == EcsOut) {
                ecs_strbuf_appendstr(&buf, "[out] ");
            }
        }

        if (term->role && term->role != ECS_PAIR) {
            ecs_strbuf_appendstr(&buf, ecs_role_str(term->role));
            ecs_strbuf_appendstr(&buf, " ");
        }

        if (term->oper == EcsOr) {
            or_count ++;
        } else {
            or_count = 0;
        }

        if (term->oper == EcsNot) {
            ecs_strbuf_appendstr(&buf, "!");
        } else if (term->oper == EcsOptional) {
            ecs_strbuf_appendstr(&buf, "?");
        }

        if (term->args[0].entity == EcsThis && 
            ecs_term_id_is_set(&term->args[1])) 
        {
            ecs_strbuf_appendstr(&buf, "(");
        }

        if (!ecs_term_id_is_set(&term->args[1]) && 
            (term->pred.entity != term->args[0].entity)) 
        {
            filter_str_add_id(world, &buf, &term->pred);

            if (!ecs_term_id_is_set(&term->args[0])) {
                ecs_strbuf_appendstr(&buf, "()");
            } else if (term->args[0].entity != EcsThis) {
                ecs_strbuf_appendstr(&buf, "(");
                filter_str_add_id(world, &buf, &term->args[0]);
            }

            if (ecs_term_id_is_set(&term->args[1])) {
                ecs_strbuf_appendstr(&buf, ", ");
                filter_str_add_id(world, &buf, &term->args[1]);
                ecs_strbuf_appendstr(&buf, ")");
            }
        } else if (!ecs_term_id_is_set(&term->args[1])) {
            ecs_strbuf_appendstr(&buf, "$");
            filter_str_add_id(world, &buf, &term->pred);
        } else if (ecs_term_id_is_set(&term->args[1])) {
            filter_str_add_id(world, &buf, &term->pred);
            ecs_strbuf_appendstr(&buf, ", ");
            filter_str_add_id(world, &buf, &term->args[1]);
            ecs_strbuf_appendstr(&buf, ")");
        }
    }

    return ecs_strbuf_get(&buf);
}

static
bool populate_from_column(
    ecs_world_t *world,
    const ecs_table_t *table,
    ecs_id_t id,
    int32_t column,
    ecs_entity_t source,
    ecs_id_t *id_out,
    ecs_type_t *type_out,
    ecs_entity_t *subject_out,
    ecs_size_t *size_out,
    void **ptr_out)
{
    bool has_data = false;

    if (column != -1) {
        /* If source is not This, find table of source */
        if (source) {
            table = ecs_get_table(world, source);
            ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
            column = tr->column;
        }

        ecs_data_t *data = flecs_table_get_data(table);
        ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);

        /* If there is no data, ensure that iterator won't try to get it */
        if (table->column_count > column) {
            ecs_column_t *c = &data->columns[column];
            if (c->size) {
                has_data = true;
                *size_out = c->size;
            }
        }

        if (!has_data) {
            *size_out = 0;
        }

        id = ids[column];

        if (subject_out) {
            *subject_out = source;
        }

        if (ptr_out) {
            if (has_data) {
                if (source) {
                    *ptr_out = (void*)ecs_get_id(world, source, id);
                } else {
                    ecs_column_t *col = &data->columns[column];
                    *ptr_out = ecs_vector_first_t(
                        col->data, col->size, col->alignment);
                }
            } else {
                *ptr_out = NULL;
            }
        }
    }

    *type_out = NULL;
    *id_out = id;

    return has_data;
}

static
void populate_from_table(
    ecs_iter_t *it,
    ecs_table_t *table)
{
    it->table = table;
    it->count = ecs_table_count(table);

    const ecs_data_t *data = flecs_table_get_data(table);
    it->data = (ecs_data_t*)data;

    if (data) {
        it->table_columns = data->columns;
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
    } else {
        it->table_columns = NULL;
        it->entities = NULL;
    }
}

bool flecs_filter_match_table(
    ecs_world_t *world,
    const ecs_filter_t *filter,
    const ecs_table_t *table,
    ecs_type_t type,
    ecs_id_t *ids,
    int32_t *columns,
    ecs_type_t *types,
    ecs_entity_t *subjects,
    ecs_size_t *sizes,
    void **ptrs)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(filter != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    bool is_or = false;
    bool or_result = false;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];
        ecs_oper_kind_t oper = term->oper;
        const ecs_table_t *match_table = table;
        ecs_type_t match_type = type;

        if (!is_or && oper == EcsOr) {
            is_or = true;
            or_result = false;
        } else if (is_or && oper != EcsOr) {
            if (!or_result) {
                return false;
            }

            is_or = false;
        }

        ecs_entity_t subj_entity = subj->entity;
        if (!subj_entity) {
            continue;
        }

        if (subj_entity != EcsThis) {
            match_table = ecs_get_table(world, subj_entity);
            if (match_table) {
                match_type = match_table->type;
            } else {
                match_type = NULL;
            }
        }

        ecs_entity_t source;

        int32_t column = ecs_type_match(world, match_table, match_type,
            0, term->id, subj->set.relation, subj->set.min_depth, 
            subj->set.max_depth, &source);
        bool result = column != -1;

        if (oper == EcsNot) {
            result = !result;
        }

        if (oper == EcsOptional) {
            result = true;
        }

        if (is_or) {
            or_result |= result;
        } else if (!result) {
            return false;
        }

        if (subj_entity != EcsThis) {
            if (!source) {
                source = subj_entity;
            }
        }

        if (columns && result) {
            ecs_assert(ids != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(types != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_assert(subjects != NULL, ECS_INTERNAL_ERROR, NULL);

            int32_t t_i = term->index;

            void **ptr = ptrs ? &ptrs[t_i] : NULL;
            populate_from_column(world, table, term->id, column, 
                source, &ids[t_i], &types[t_i], &subjects[t_i], &sizes[t_i], 
                ptr);

            if (column != -1) {
                columns[t_i] = column + 1;
            } else {
                columns[t_i] = 0;
            }
        }
    }

    return !is_or || or_result;
}

static
void term_iter_init_no_data(
    ecs_term_iter_t *iter)
{
    iter->term = NULL;
    iter->self_index = NULL;
    iter->iter = ecs_map_iter(NULL);
}

static
void term_iter_init_wildcard(
    ecs_world_t *world,
    ecs_term_iter_t *iter)
{
    iter->term = NULL;
    iter->self_index = flecs_get_id_record(world, EcsWildcard);

    if (iter->self_index) {
        iter->iter = ecs_map_iter(iter->self_index->table_index);
    }
}

static
void term_iter_init(
    ecs_world_t *world,
    ecs_term_t *term,
    ecs_term_iter_t *iter)
{    
    const ecs_term_id_t *subj = &term->args[0];

    iter->term = term;

    if (subj->set.mask == EcsDefaultSet || subj->set.mask & EcsSelf) {
        iter->self_index = flecs_get_id_record(world, term->id);
    }

    if (subj->set.mask & EcsSuperSet) {
        iter->set_index = flecs_get_id_record(world, 
            ecs_pair(subj->set.relation, EcsWildcard));
    }

    if (iter->self_index) {
        iter->iter = ecs_map_iter(iter->self_index->table_index);
    } else if (iter->set_index) {
        iter->iter = ecs_map_iter(iter->set_index->table_index);
        iter->iter_set = true;
    }
}

ecs_iter_t ecs_term_iter(
    ecs_world_t *world,
    ecs_term_t *term)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term->id != 0, ECS_INVALID_PARAMETER, NULL);

    if (ecs_term_finalize(world, NULL, NULL, term)) {
        /* Invalid term */
        ecs_abort(ECS_INVALID_PARAMETER, NULL);
    }

    ecs_iter_t it = {
        .world = world,
        .column_count = 1
    };

    term_iter_init(world, term, &it.iter.term);

    return it;
}

static
ecs_table_record_t* term_iter_next(
    ecs_world_t *world,
    ecs_term_iter_t *iter,
    ecs_entity_t *source_out)
{
    ecs_table_t *table = NULL;
    ecs_entity_t source = 0;
    ecs_table_record_t *tr;

    do {
        tr = ecs_map_next(&iter->iter, ecs_table_record_t, NULL);
        if (!tr) {
            if (!iter->iter_set) {
                if (iter->set_index) {
                    iter->iter = ecs_map_iter(iter->set_index->table_index);
                    tr = ecs_map_next(&iter->iter, ecs_table_record_t, NULL);
                    iter->iter_set = true;
                }
            }

            if (!tr) {
                return NULL;
            }
        }

        table = tr->table;

        if (!ecs_table_count(table)) {
            continue;
        }

        if (iter->iter_set) {
            const ecs_term_t *term = iter->term;
            const ecs_term_id_t *subj = &term->args[0];

            if (iter->self_index) {
                if (ecs_map_has(iter->self_index->table_index, table->id)) {
                    /* If the table has the id itself and this term matched Self
                     * we already matched it */
                    continue;
                }
            }

            /* Test if following the relation finds the id */
            int32_t index = ecs_type_match(world, table, table->type, 0, 
                term->id, subj->set.relation, subj->set.min_depth, 
                subj->set.max_depth, &source);
            if (index == -1) {
                continue;
            }

            ecs_assert(source != 0, ECS_INTERNAL_ERROR, NULL);
        }

        break;
    } while (true);

    if (source_out) {
        *source_out = source;
    }

    return tr;
}

bool ecs_term_next(
    ecs_iter_t *it)
{
    ecs_term_iter_t *iter = &it->iter.term;
    ecs_term_t *term = iter->term;
    ecs_world_t *world = it->world;

    ecs_entity_t source;
    ecs_table_record_t *tr = term_iter_next(world, iter, &source);
    if (!tr) {
        it->is_valid = false;
        return false;
    }

    ecs_table_t *table = tr->table;

    /* Source must either be 0 (EcsThis) or nonzero in case of substitution */
    ecs_assert(source || !iter->iter_set, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = flecs_table_get_data(table);

    it->table = table;
    it->data = data;
    it->ids = &iter->id;
    it->types = &iter->type;
    it->columns = &iter->column;
    it->subjects = &iter->subject;
    it->sizes = &iter->size;
    it->ptrs = &iter->ptr;

    it->table_columns = data->columns;
    it->count = ecs_table_count(table);
    it->entities = ecs_vector_first(data->entities, ecs_entity_t);
    it->is_valid = true;

    bool has_data = populate_from_column(world, table, term->id, tr->column, 
        source, &iter->id, &iter->type, &iter->subject, &iter->size, 
        &iter->ptr);

    if (!source) {
        if (has_data) {
            iter->column = tr->column + 1;
        } else {
            iter->column = 0;
        }
    } else {
        iter->column = -1; /* Point to ref */
    }

    return true;
}

ecs_iter_t ecs_filter_iter(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_iter_t it = {
        .world = world
    };

    ecs_filter_iter_t *iter = &it.iter.filter;
    if (filter) {
        iter->filter = *filter;

        if (filter->terms == filter->term_cache) {
            iter->filter.terms = iter->filter.term_cache;
        }

        ecs_filter_finalize(world, &iter->filter);        
    } else {
        ecs_filter_init(world, &iter->filter, &(ecs_filter_desc_t) {
            .terms = {{ .id = EcsWildcard }}
        });

        filter = &iter->filter;
    }

    int32_t i, term_count = filter->term_count;
    ecs_term_t *terms = filter->terms;
    int32_t min_count = -1;
    int32_t min_term_index = -1;

    /* Find term that represents smallest superset */
    if (filter->match_this) {
        iter->kind = EcsFilterIterEvalIndex;

        for (i = 0; i < term_count; i ++) {
            ecs_term_t *term = &terms[i];

            ecs_assert(term != NULL, ECS_INTERNAL_ERROR, NULL);

            if (term->oper != EcsAnd) {
                continue;
            }

            if (term->args[0].entity != EcsThis) {
                continue;
            }

            ecs_id_record_t *idr = flecs_get_id_record(world, term->id);
            if (!idr) {
                /* If one of the terms does not match with any data, iterator 
                 * should not return anything */
                term_iter_init_no_data(&iter->term_iter);
                return it;
            }

            int32_t table_count = ecs_map_count(idr->table_index);
            if (min_count == -1 || table_count < min_count) {
                min_count = table_count;
                min_term_index = i;
            }
        }

        iter->min_term_index = min_term_index;

        if (min_term_index == -1) {
            term_iter_init_wildcard(world, &iter->term_iter);
        } else {
            term_iter_init(world, &terms[min_term_index], &iter->term_iter);
        }
    } else {
        /* If filter has no this terms, no tables need to be evaluated */
        iter->kind = EcsFilterIterEvalNone;
    }

    it.column_count = filter->term_count_actual;

    if (filter->terms == filter->term_cache) {
        /* Because we're returning the iterator by value, the address of the
         * term cache changes. The ecs_filter_next function will set the correct
         * address when it detects that terms is set to NULL */
        iter->filter.terms = NULL;
    }

    return it;
}

bool ecs_filter_next(
    ecs_iter_t *it)
{
    ecs_filter_iter_t *iter = &it->iter.filter;
    ecs_filter_t *filter = &iter->filter;
    ecs_world_t *world = it->world;

    if (!filter->terms) {
        filter->terms = filter->term_cache;
    }

    ecs_iter_init(it);

    if (iter->kind == EcsFilterIterEvalIndex) {
        ecs_term_iter_t *term_iter = &iter->term_iter;
        ecs_table_t *table;
        bool match;

        do {
            ecs_entity_t source;
            ecs_table_record_t *tr = term_iter_next(world, term_iter, &source);
            if (!tr) {
                goto done;
            }

            table = tr->table;
            match = flecs_filter_match_table(world, filter, table, table->type,
                it->ids, it->columns, it->types, it->subjects, it->sizes, 
                it->ptrs);
        } while (!match);

        populate_from_table(it, table);

        goto yield;
    }

done:
    ecs_iter_fini(it);
    return false;

yield:
    it->is_valid = true;
    return true;    
}

static
void observer_callback(ecs_iter_t *it) {
    ecs_observer_t *o = it->ctx;
    ecs_world_t *world = it->world;
    
    ecs_assert(it->table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = it->table;
    ecs_type_t type = table->type;

    ecs_iter_t user_it = *it;
    user_it.column_count = o->filter.term_count_actual,
    user_it.ids = NULL;
    user_it.columns = NULL;
    user_it.types = NULL;
    user_it.subjects = NULL;
    user_it.sizes = NULL;
    user_it.ptrs = NULL;

    ecs_iter_init(&user_it);

    if (flecs_filter_match_table(world, &o->filter, table, type, 
        user_it.ids, user_it.columns, user_it.types, user_it.subjects, 
        user_it.sizes, user_it.ptrs)) 
    {
        ecs_data_t *data = flecs_table_get_data(table);
        
        user_it.ids[it->term_index] = it->event_id;

        user_it.system = o->entity;
        user_it.term_index = it->term_index;
        user_it.self = o->self;
        user_it.ctx = o->ctx;
        user_it.column_count = o->filter.term_count_actual,
        user_it.table_columns = data->columns,
        o->action(&user_it);
    }

    ecs_iter_fini(&user_it);
}

ecs_entity_t ecs_observer_init(
    ecs_world_t *world,
    const ecs_observer_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);

    bool added = false;
    EcsObserver *comp = ecs_get_mut(world, entity, EcsObserver, &added);
    if (added) {
        ecs_observer_t *observer = flecs_sparse_add(
            world->observers, ecs_observer_t);
        ecs_assert(observer != NULL, ECS_INTERNAL_ERROR, NULL);
        observer->id = flecs_sparse_last_id(world->observers);

        /* Make writeable copy of filter desc so that we can set name. This will
         * make debugging easier, as any error messages related to creating the
         * filter will have the name of the observer. */
        ecs_filter_desc_t filter_desc = desc->filter;
        filter_desc.name = desc->entity.name;

        /* Parse filter */
        if (ecs_filter_init(world, &observer->filter, &filter_desc)) {
            flecs_observer_fini(world, observer);
            return 0;
        }

        ecs_filter_t *filter = &observer->filter;

        /* Create a trigger for each term in the filter */
        observer->triggers = ecs_os_malloc(ECS_SIZEOF(ecs_entity_t) * 
            observer->filter.term_count);
        
        int i;
        for (i = 0; i < filter->term_count; i ++) {
            const ecs_term_t *terms = filter->terms;
            const ecs_term_t *t = &terms[i];

            if (t->oper == EcsNot || terms[i].args[0].entity != EcsThis) {
                /* No need to trigger on components that the entity should not
                 * have, or on components that are not defined on the entity */
                observer->triggers[i] = 0;
                continue;
            }

            ecs_trigger_desc_t trigger_desc = {
                .term = *t,
                .callback = observer_callback,
                .ctx = observer,
                .binding_ctx = desc->binding_ctx
            };

            ecs_os_memcpy(trigger_desc.events, desc->events, 
                ECS_SIZEOF(ecs_entity_t) * ECS_TRIGGER_DESC_EVENT_COUNT_MAX);
            observer->triggers[i] = ecs_trigger_init(world, &trigger_desc);
        }

        observer->action = desc->callback;
        observer->self = desc->self;
        observer->ctx = desc->ctx;
        observer->binding_ctx = desc->binding_ctx;
        observer->ctx_free = desc->ctx_free;
        observer->binding_ctx_free = desc->binding_ctx_free;
        observer->event_count = 0;
        ecs_os_memcpy(observer->events, desc->events, 
            observer->event_count * ECS_SIZEOF(ecs_entity_t));
        observer->entity = entity;        

        comp->observer = observer;
    } else {
        ecs_assert(comp->observer != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If existing entity handle was provided, override existing params */
        if (existing) {
            if (desc->callback) {
                ((ecs_observer_t*)comp->observer)->action = desc->callback;
            }
            if (desc->ctx) {
                ((ecs_observer_t*)comp->observer)->ctx = desc->ctx;
            }
            if (desc->binding_ctx) {
                ((ecs_observer_t*)comp->observer)->binding_ctx = 
                    desc->binding_ctx;
            }
        }        
    }

    return entity; 
}

void flecs_observer_fini(
    ecs_world_t *world,
    ecs_observer_t *observer)
{
    int i, count = observer->filter.term_count;
    for (i = 0; i < count; i ++) {
        ecs_entity_t trigger = observer->triggers[i];
        if (trigger) {
            ecs_delete(world, trigger);
        }
    }
    ecs_os_free(observer->triggers);

    ecs_filter_fini(&observer->filter);

    if (observer->ctx_free) {
        observer->ctx_free(observer->ctx);
    }

    if (observer->binding_ctx_free) {
        observer->binding_ctx_free(observer->binding_ctx);
    }

    flecs_sparse_remove(world->observers, observer->id);
}

void* ecs_get_observer_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsObserver *o = ecs_get(world, observer, EcsObserver);
    if (o) {
        return o->observer->ctx;
    } else {
        return NULL;
    }     
}

void* ecs_get_observer_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t observer)
{
    const EcsObserver *o = ecs_get(world, observer, EcsObserver);
    if (o) {
        return o->observer->binding_ctx;
    } else {
        return NULL;
    }      
}


static
void ensure(
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

void flecs_bitset_init(
    ecs_bitset_t* bs)
{
    bs->size = 0;
    bs->count = 0;
    bs->data = NULL;
}

void flecs_bitset_ensure(
    ecs_bitset_t *bs,
    int32_t count)
{
    if (count > bs->count) {
        bs->count = count;
        ensure(bs, count);
    }
}

void flecs_bitset_deinit(
    ecs_bitset_t *bs)
{
    ecs_os_free(bs->data);
}

void flecs_bitset_addn(
    ecs_bitset_t *bs,
    int32_t count)
{
    int32_t elem = bs->count += count;
    ensure(bs, elem);
}

void flecs_bitset_set(
    ecs_bitset_t *bs,
    int32_t elem,
    bool value)
{
    ecs_assert(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    int32_t hi = elem >> 6;
    int32_t lo = elem & 0x3F;
    uint64_t v = bs->data[hi];
    bs->data[hi] = (v & ~((uint64_t)1 << lo)) | ((uint64_t)value << lo);
}

bool flecs_bitset_get(
    const ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_assert(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    return !!(bs->data[elem >> 6] & ((uint64_t)1 << ((uint64_t)elem & 0x3F)));
}

int32_t flecs_bitset_count(
    const ecs_bitset_t *bs)
{
    return bs->count;
}

void flecs_bitset_remove(
    ecs_bitset_t *bs,
    int32_t elem)
{
    ecs_assert(elem < bs->count, ECS_INVALID_PARAMETER, NULL);
    int32_t last = bs->count - 1;
    bool last_value = flecs_bitset_get(bs, last);
    flecs_bitset_set(bs, elem, last_value);
    bs->count --;
}

void flecs_bitset_swap(
    ecs_bitset_t *bs,
    int32_t elem_a,
    int32_t elem_b)
{
    ecs_assert(elem_a < bs->count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem_b < bs->count, ECS_INVALID_PARAMETER, NULL);

    bool a = flecs_bitset_get(bs, elem_a);
    bool b = flecs_bitset_get(bs, elem_b);
    flecs_bitset_set(bs, elem_a, b);
    flecs_bitset_set(bs, elem_b, a);
}

/* Add an extra element to the buffer */
static
void ecs_strbuf_grow(
    ecs_strbuf_t *b)
{
    /* Allocate new element */
    ecs_strbuf_element_embedded *e = ecs_os_malloc(sizeof(ecs_strbuf_element_embedded));
    b->size += b->current->pos;
    b->current->next = (ecs_strbuf_element*)e;
    b->current = (ecs_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = true;
    e->super.buf = e->buf;
    e->super.pos = 0;
    e->super.next = NULL;
}

/* Add an extra dynamic element */
static
void ecs_strbuf_grow_str(
    ecs_strbuf_t *b,
    char *str,
    char *alloc_str,
    int32_t size)
{
    /* Allocate new element */
    ecs_strbuf_element_str *e = ecs_os_malloc(sizeof(ecs_strbuf_element_str));
    b->size += b->current->pos;
    b->current->next = (ecs_strbuf_element*)e;
    b->current = (ecs_strbuf_element*)e;
    b->elementCount ++;
    e->super.buffer_embedded = false;
    e->super.pos = size ? size : (int32_t)ecs_os_strlen(str);
    e->super.next = NULL;
    e->super.buf = str;
    e->alloc_str = alloc_str;
}

static
char* ecs_strbuf_ptr(
    ecs_strbuf_t *b)
{
    if (b->buf) {
        return &b->buf[b->current->pos];
    } else {
        return &b->current->buf[b->current->pos];
    }
}

/* Compute the amount of space left in the current element */
static
int32_t ecs_strbuf_memLeftInCurrentElement(
    ecs_strbuf_t *b)
{
    if (b->current->buffer_embedded) {
        return ECS_STRBUF_ELEMENT_SIZE - b->current->pos;
    } else {
        return 0;
    }
}

/* Compute the amount of space left */
static
int32_t ecs_strbuf_memLeft(
    ecs_strbuf_t *b)
{
    if (b->max) {
        return b->max - b->size - b->current->pos;
    } else {
        return INT_MAX;
    }
}

static
void ecs_strbuf_init(
    ecs_strbuf_t *b)
{
    /* Initialize buffer structure only once */
    if (!b->elementCount) {
        b->size = 0;
        b->firstElement.super.next = NULL;
        b->firstElement.super.pos = 0;
        b->firstElement.super.buffer_embedded = true;
        b->firstElement.super.buf = b->firstElement.buf;
        b->elementCount ++;
        b->current = (ecs_strbuf_element*)&b->firstElement;
    }
}

/* Quick custom function to copy a maxium number of characters and
 * simultaneously determine length of source string. */
static
int32_t fast_strncpy(
    char * dst,
    const char * src,
    int n_cpy,
    int n)
{
    ecs_assert(n_cpy >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(n >= 0, ECS_INTERNAL_ERROR, NULL);

    const char *ptr, *orig = src;
    char ch;

    for (ptr = src; (ptr - orig < n) && (ch = *ptr); ptr ++) {
        if (ptr - orig < n_cpy) {
            *dst = ch;
            dst ++;
        }
    }

    ecs_assert(ptr - orig < INT32_MAX, ECS_INTERNAL_ERROR, NULL);

    return (int32_t)(ptr - orig);
}

/* Append a format string to a buffer */
static
bool ecs_strbuf_vappend_intern(
    ecs_strbuf_t *b,
    const char* str,
    va_list args)
{
    bool result = true;
    va_list arg_cpy;

    if (!str) {
        return result;
    }

    ecs_strbuf_init(b);

    int32_t memLeftInElement = ecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ecs_strbuf_memLeft(b);

    if (!memLeft) {
        return false;
    }

    /* Compute the memory required to add the string to the buffer. If user
     * provided buffer, use space left in buffer, otherwise use space left in
     * current element. */
    int32_t max_copy = b->buf ? memLeft : memLeftInElement;
    int32_t memRequired;

    va_copy(arg_cpy, args);
    memRequired = vsnprintf(
        ecs_strbuf_ptr(b), (size_t)(max_copy + 1), str, args);

    ecs_assert(memRequired != -1, ECS_INTERNAL_ERROR, NULL);

    if (memRequired <= memLeftInElement) {
        /* Element was large enough to fit string */
        b->current->pos += memRequired;
    } else if ((memRequired - memLeftInElement) < memLeft) {
        /* If string is a format string, a new buffer of size memRequired is
         * needed to re-evaluate the format string and only use the part that
         * wasn't already copied to the previous element */
        if (memRequired <= ECS_STRBUF_ELEMENT_SIZE) {
            /* Resulting string fits in standard-size buffer. Note that the
             * entire string needs to fit, not just the remainder, as the
             * format string cannot be partially evaluated */
            ecs_strbuf_grow(b);

            /* Copy entire string to new buffer */
            ecs_os_vsprintf(ecs_strbuf_ptr(b), str, arg_cpy);

            /* Ignore the part of the string that was copied into the
             * previous buffer. The string copied into the new buffer could
             * be memmoved so that only the remainder is left, but that is
             * most likely more expensive than just keeping the entire
             * string. */

            /* Update position in buffer */
            b->current->pos += memRequired;
        } else {
            /* Resulting string does not fit in standard-size buffer.
             * Allocate a new buffer that can hold the entire string. */
            char *dst = ecs_os_malloc(memRequired + 1);
            ecs_os_vsprintf(dst, str, arg_cpy);
            ecs_strbuf_grow_str(b, dst, dst, memRequired);
        }
    } else {
        /* Buffer max has been reached */
        result = false;
    }

    va_end(arg_cpy);

    return result;
}

static
bool ecs_strbuf_append_intern(
    ecs_strbuf_t *b,
    const char* str,
    int n)
{
    bool result = true;

    if (!str) {
        return result;
    }

    ecs_strbuf_init(b);

    int32_t memLeftInElement = ecs_strbuf_memLeftInCurrentElement(b);
    int32_t memLeft = ecs_strbuf_memLeft(b);

    if (memLeft <= 0) {
        return false;
    }

    /* Compute the memory required to add the string to the buffer. If user
     * provided buffer, use space left in buffer, otherwise use space left in
     * current element. */
    int32_t max_copy = b->buf ? memLeft : memLeftInElement;
    int32_t memRequired;

    if (n < 0) n = INT_MAX;

    memRequired = fast_strncpy(ecs_strbuf_ptr(b), str, max_copy, n);

    if (memRequired <= memLeftInElement) {
        /* Element was large enough to fit string */
        b->current->pos += memRequired;
    } else if ((memRequired - memLeftInElement) < memLeft) {
        /* Element was not large enough, but buffer still has space */
        b->current->pos += memLeftInElement;
        memRequired -= memLeftInElement;

        /* Current element was too small, copy remainder into new element */
        if (memRequired < ECS_STRBUF_ELEMENT_SIZE) {
            /* A standard-size buffer is large enough for the new string */
            ecs_strbuf_grow(b);

            /* Copy the remainder to the new buffer */
            if (n) {
                /* If a max number of characters to write is set, only a
                    * subset of the string should be copied to the buffer */
                ecs_os_strncpy(
                    ecs_strbuf_ptr(b),
                    str + memLeftInElement,
                    (size_t)memRequired);
            } else {
                ecs_os_strcpy(ecs_strbuf_ptr(b), str + memLeftInElement);
            }

            /* Update to number of characters copied to new buffer */
            b->current->pos += memRequired;
        } else {
            char *remainder = ecs_os_strdup(str + memLeftInElement);
            ecs_strbuf_grow_str(b, remainder, remainder, memRequired);
        }
    } else {
        /* Buffer max has been reached */
        result = false;
    }

    return result;
}

bool ecs_strbuf_vappend(
    ecs_strbuf_t *b,
    const char* fmt,
    va_list args)
{
    bool result = ecs_strbuf_vappend_intern(
        b, fmt, args
    );

    return result;
}

bool ecs_strbuf_append(
    ecs_strbuf_t *b,
    const char* fmt,
    ...)
{
    va_list args;
    va_start(args, fmt);
    bool result = ecs_strbuf_vappend_intern(
        b, fmt, args
    );
    va_end(args);

    return result;
}

bool ecs_strbuf_appendstrn(
    ecs_strbuf_t *b,
    const char* str,
    int32_t len)
{
    return ecs_strbuf_append_intern(
        b, str, len
    );
}

bool ecs_strbuf_appendstr_zerocpy(
    ecs_strbuf_t *b,
    char* str)
{
    ecs_strbuf_init(b);
    ecs_strbuf_grow_str(b, str, str, 0);
    return true;
}

bool ecs_strbuf_appendstr_zerocpy_const(
    ecs_strbuf_t *b,
    const char* str)
{
    /* Removes const modifier, but logic prevents changing / delete string */
    ecs_strbuf_init(b);
    ecs_strbuf_grow_str(b, (char*)str, NULL, 0);
    return true;
}

bool ecs_strbuf_appendstr(
    ecs_strbuf_t *b,
    const char* str)
{
    return ecs_strbuf_append_intern(
        b, str, -1
    );
}

bool ecs_strbuf_mergebuff(
    ecs_strbuf_t *dst_buffer,
    ecs_strbuf_t *src_buffer)
{
    if (src_buffer->elementCount) {
        if (src_buffer->buf) {
            return ecs_strbuf_appendstr(dst_buffer, src_buffer->buf);
        } else {
            ecs_strbuf_element *e = (ecs_strbuf_element*)&src_buffer->firstElement;

            /* Copy first element as it is inlined in the src buffer */
            ecs_strbuf_appendstrn(dst_buffer, e->buf, e->pos);

            while ((e = e->next)) {
                dst_buffer->current->next = ecs_os_malloc(sizeof(ecs_strbuf_element));
                *dst_buffer->current->next = *e;
            }
        }

        *src_buffer = ECS_STRBUF_INIT;
    }

    return true;
}

char* ecs_strbuf_get(ecs_strbuf_t *b) {
    char* result = NULL;

    if (b->elementCount) {
        if (b->buf) {
            b->buf[b->current->pos] = '\0';
            result = ecs_os_strdup(b->buf);
        } else {
            void *next = NULL;
            int32_t len = b->size + b->current->pos + 1;

            ecs_strbuf_element *e = (ecs_strbuf_element*)&b->firstElement;

            result = ecs_os_malloc(len);
            char* ptr = result;

            do {
                ecs_os_memcpy(ptr, e->buf, e->pos);
                ptr += e->pos;
                next = e->next;
                if (e != &b->firstElement.super) {
                    if (!e->buffer_embedded) {
                        ecs_os_free(((ecs_strbuf_element_str*)e)->alloc_str);
                    }
                    ecs_os_free(e);
                }
            } while ((e = next));

            result[len - 1] = '\0';
        }
    } else {
        result = NULL;
    }

    b->elementCount = 0;

    return result;
}

void ecs_strbuf_reset(ecs_strbuf_t *b) {
    if (b->elementCount && !b->buf) {
        void *next = NULL;
        ecs_strbuf_element *e = (ecs_strbuf_element*)&b->firstElement;
        do {
            next = e->next;
            if (e != (ecs_strbuf_element*)&b->firstElement) {
                ecs_os_free(e);
            }
        } while ((e = next));
    }

    *b = ECS_STRBUF_INIT;
}

void ecs_strbuf_list_push(
    ecs_strbuf_t *buffer,
    const char *list_open,
    const char *separator)
{
    buffer->list_sp ++;
    buffer->list_stack[buffer->list_sp].count = 0;
    buffer->list_stack[buffer->list_sp].separator = separator;

    if (list_open) {
        ecs_strbuf_appendstr(buffer, list_open);
    }
}

void ecs_strbuf_list_pop(
    ecs_strbuf_t *buffer,
    const char *list_close)
{
    buffer->list_sp --;
    
    if (list_close) {
        ecs_strbuf_appendstr(buffer, list_close);
    }
}

void ecs_strbuf_list_next(
    ecs_strbuf_t *buffer)
{
    int32_t list_sp = buffer->list_sp;
    if (buffer->list_stack[list_sp].count != 0) {
        ecs_strbuf_appendstr(buffer, buffer->list_stack[list_sp].separator);
    }
    buffer->list_stack[list_sp].count ++;
}

bool ecs_strbuf_list_append(
    ecs_strbuf_t *buffer,
    const char *fmt,
    ...)
{
    ecs_strbuf_list_next(buffer);

    va_list args;
    va_start(args, fmt);
    bool result = ecs_strbuf_vappend_intern(
        buffer, fmt, args
    );
    va_end(args);

    return result;
}

bool ecs_strbuf_list_appendstr(
    ecs_strbuf_t *buffer,
    const char *str)
{
    ecs_strbuf_list_next(buffer);
    return ecs_strbuf_appendstr(buffer, str);
}

/* -- Private functions -- */

/* O(n) algorithm to check whether type 1 is equal or superset of type 2 */
ecs_entity_t flecs_type_contains(
    const ecs_world_t *world,
    ecs_type_t type_1,
    ecs_type_t type_2,
    bool match_all,
    bool match_prefab)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);
    
    if (!type_1) {
        return 0;
    }

    ecs_assert(type_2 != NULL, ECS_INTERNAL_ERROR, NULL);

    if (type_1 == type_2) {
        return *(ecs_vector_first(type_1, ecs_entity_t));
    }

    if (ecs_vector_count(type_2) == 1) {
        return ecs_type_has_id(world, type_1, 
            ecs_vector_first(type_2, ecs_id_t)[0], !match_prefab);
    }

    int32_t i_2, i_1 = 0;
    ecs_entity_t e1 = 0;
    ecs_entity_t *t1_array = ecs_vector_first(type_1, ecs_entity_t);
    ecs_entity_t *t2_array = ecs_vector_first(type_2, ecs_entity_t);

    ecs_assert(t1_array != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(t2_array != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t t1_count = ecs_vector_count(type_1);
    int32_t t2_count = ecs_vector_count(type_2);

    for (i_2 = 0; i_2 < t2_count; i_2 ++) {
        ecs_entity_t e2 = t2_array[i_2];

        if (i_1 >= t1_count) {
            return 0;
        }

        e1 = t1_array[i_1];

        if (e2 > e1) {
            do {
                i_1 ++;
                if (i_1 >= t1_count) {
                    return 0;
                }
                e1 = t1_array[i_1];
            } while (e2 > e1);
        }

        if (e1 != e2) {
            if (e1 != e2) {
                if (match_all) return 0;
            } else if (!match_all) {
                return e1;
            }
        } else {
            if (!match_all) return e1;
            i_1 ++;
            if (i_1 < t1_count) {
                e1 = t1_array[i_1];
            }
        }
    }

    if (match_all) {
        return e1;
    } else {
        return 0;
    }
}

/* -- Public API -- */

ecs_type_t ecs_type_merge(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t to_add,
    ecs_type_t to_remove)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* This function is allowed while staged, as long as the type already
     * exists. If the type does not exist yet and traversing the table graph
     * results in the creation of a table, an assert will trigger. */    
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);
    
    ecs_table_t *table = ecs_table_from_type(unsafe_world, type);
    ecs_ids_t add_array = flecs_type_to_ids(to_add);
    ecs_ids_t remove_array = flecs_type_to_ids(to_remove);
    
    table = flecs_table_traverse_remove(
        unsafe_world, table, &remove_array, NULL); 

    table = flecs_table_traverse_add(
        unsafe_world, table, &add_array, NULL); 

    if (!table) {
        return NULL;
    } else {
        return table->type;
    }
}

static
bool has_case(
    const ecs_world_t *world,
    ecs_entity_t sw_case,
    ecs_entity_t e)
{
    const EcsType *type_ptr = ecs_get(world, e & ECS_COMPONENT_MASK, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_type_has_id(world, type_ptr->normalized, sw_case, false);
}

static
bool match_id(
    const ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t match_with)
{
    if (ECS_HAS_ROLE(match_with, CASE)) {
        ecs_entity_t sw_case = match_with & ECS_COMPONENT_MASK;
        if (ECS_HAS_ROLE(id, SWITCH) && has_case(world, sw_case, id)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return ecs_id_match(id, match_with);
    }
}

static
int32_t search_type(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    int32_t depth,
    ecs_entity_t *out)
{
    if (!id) {
        return -1;
    }

    if (!type) {
        return -1;
    }

    if (max_depth && depth > max_depth) {
        return -1;
    }

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);

    if (depth >= min_depth) {
        if (table && !offset && !(ECS_HAS_ROLE(id, CASE))) {
            ecs_table_record_t *tr = flecs_get_table_record(world, table, id);
            if (tr) {
                return tr->column;
            }
        } else {
            for (i = offset; i < count; i ++) {
                if (match_id(world, ids[i], id)) {
                    return i;
                }
            }
        }
    }

    if (rel && id != EcsPrefab && id != EcsDisabled && 
        id != ecs_pair(ecs_id(EcsIdentifier), EcsName)) 
    {
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = ids[i];
            if (!ECS_HAS_RELATION(e, rel)) {
                continue;
            }

            ecs_entity_t obj = ecs_pair_object(world, e);
            ecs_assert(obj != 0, ECS_INTERNAL_ERROR, NULL);

            ecs_table_t *obj_table = ecs_get_table(world, obj);
            if (!obj_table) {
                continue;
            }

            if ((search_type(world, obj_table, obj_table->type, 0, id, 
                rel, min_depth, max_depth, depth + 1, out) != -1))
            {
                if (out && !*out) {
                    *out = obj;
                }
                return i;

            /* If the id could not be found on the object and the relationship
             * is not IsA, try substituting the object type with IsA */
            } else if (rel != EcsIsA) {
                if (search_type(world, obj_table, obj_table->type, 0, 
                    id, EcsIsA, 1, 0, 0, out) != -1) 
                {
                    if (out && !*out) {
                        *out = obj;
                    }
                    return i;
                }
            }
        }
    }

    return -1;
}

bool ecs_type_has_id(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_id_t id,
    bool owned)
{
    return search_type(world, NULL, type, 0, id, owned ? 0 : EcsIsA, 0, 0, 0, NULL) != -1;
}

int32_t ecs_type_index_of(
    ecs_type_t type, 
    int32_t offset, 
    ecs_id_t id)
{
    return search_type(NULL, NULL, type, offset, id, 0, 0, 0, 0, NULL);
}

int32_t ecs_type_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_type_t type,
    int32_t offset,
    ecs_id_t id,
    ecs_entity_t rel,
    int32_t min_depth,
    int32_t max_depth,
    ecs_entity_t *out)
{
    if (out) {
        *out = 0;
    }
    return search_type(world, table, type, offset, id, rel, min_depth, max_depth, 0, out);
}

bool ecs_type_has_type(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has)
{
    return flecs_type_contains(world, type, has, true, false) != 0;
}

bool ecs_type_owns_type(
    const ecs_world_t *world,
    ecs_type_t type,
    ecs_type_t has,
    bool owned)
{
    return flecs_type_contains(world, type, has, true, !owned) != 0;
}

ecs_type_t ecs_type_add(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    /* This function is allowed while staged, as long as the type already
     * exists. If the type does not exist yet and traversing the table graph
     * results in the creation of a table, an assert will trigger. */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);

    ecs_table_t *table = ecs_table_from_type(unsafe_world, type);

    ecs_ids_t entities = {
        .array = &e,
        .count = 1
    };

    table = flecs_table_traverse_add(unsafe_world, table, &entities, NULL);

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

ecs_type_t ecs_type_remove(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t e)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    
    /* This function is allowed while staged, as long as the type already
     * exists. If the type does not exist yet and traversing the table graph
     * results in the creation of a table, an assert will trigger. */
    ecs_world_t *unsafe_world = (ecs_world_t*)ecs_get_world(world);
    
    ecs_table_t *table = ecs_table_from_type(unsafe_world, type);

    ecs_ids_t entities = {
        .array = &e,
        .count = 1
    };

    table = flecs_table_traverse_remove(unsafe_world, table, &entities, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;    
}

char* ecs_type_str(
    const ecs_world_t *world,
    ecs_type_t type)
{
    if (!type) {
        return ecs_os_strdup("");
    }

    ecs_vector_t *chbuf = ecs_vector_new(char, 32);
    char *dst;

    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);
    int32_t i, count = ecs_vector_count(type);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        char buffer[256];
        ecs_size_t len;

        if (i) {
            *(char*)ecs_vector_add(&chbuf, char) = ',';
        }

        if (e == 1) {
            ecs_os_strcpy(buffer, "EcsComponent");
            len = ecs_os_strlen("EcsComponent");
        } else {
            len = flecs_from_size_t(ecs_id_str(world, e, buffer, 256));
        }

        dst = ecs_vector_addn(&chbuf, char, len);
        ecs_os_memcpy(dst, buffer, len);
    }

    *(char*)ecs_vector_add(&chbuf, char) = '\0';

    char* result = ecs_os_strdup(ecs_vector_first(chbuf, char));
    ecs_vector_free(chbuf);
    return result;
}

ecs_entity_t ecs_type_get_entity_for_xor(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t xor)
{
    ecs_assert(
        ecs_type_has_id(world, type, ECS_XOR | xor, true),
        ECS_INVALID_PARAMETER, NULL);

    const EcsType *type_ptr = ecs_get(world, xor, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_type_t xor_type = type_ptr->normalized;
    ecs_assert(xor_type != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        if (ecs_type_has_id(world, xor_type, array[i], true)) {
            return array[i];
        }
    }

    return 0;
}

void ecs_os_api_impl(ecs_os_api_t *api);

static bool ecs_os_api_initialized = false;
static int ecs_os_api_init_count = 0;

ecs_os_api_t ecs_os_api;

int64_t ecs_os_api_malloc_count = 0;
int64_t ecs_os_api_realloc_count = 0;
int64_t ecs_os_api_calloc_count = 0;
int64_t ecs_os_api_free_count = 0;

void ecs_os_set_api(
    ecs_os_api_t *os_api)
{
    if (!ecs_os_api_initialized) {
        ecs_os_api = *os_api;
        ecs_os_api_initialized = true;
    }
}

void ecs_os_init(void)
{
    if (!ecs_os_api_initialized) {
        ecs_os_set_api_defaults();
    }
    
    if (!(ecs_os_api_init_count ++)) {
        if (ecs_os_api.init_) {
            ecs_os_api.init_();
        }
    }
}

void ecs_os_fini(void) {
    if (!--ecs_os_api_init_count) {
        if (ecs_os_api.fini_) {
            ecs_os_api.fini_();
        }
    }
}

static
void ecs_log(const char *fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_error(const char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

static
void ecs_log_debug(const char *fmt, va_list args) {
    vfprintf(stdout, fmt, args);
    fprintf(stdout, "\n");
}

static
void ecs_log_warning(const char *fmt, va_list args) {
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}

void ecs_os_dbg(const char *fmt, ...) {
#ifndef NDEBUG
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_debug_) {
        ecs_os_api.log_debug_(fmt, args);
    }
    va_end(args);
#else
    (void)fmt;
#endif
}

void ecs_os_warn(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_warning_) {
        ecs_os_api.log_warning_(fmt, args);
    }
    va_end(args);
}

void ecs_os_log(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_) {
        ecs_os_api.log_(fmt, args);
    }
    va_end(args);
}

void ecs_os_err(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (ecs_os_api.log_error_) {
        ecs_os_api.log_error_(fmt, args);
    }
    va_end(args);
}

static
void ecs_os_gettime(ecs_time_t *time)
{
    uint64_t now = flecs_os_time_now();
    uint64_t sec = now / 1000000000;

    assert(sec < UINT32_MAX);
    assert((now - sec * 1000000000) < UINT32_MAX);

    time->sec = (uint32_t)sec;
    time->nanosec = (uint32_t)(now - sec * 1000000000);
}

static
void* ecs_os_api_malloc(ecs_size_t size) {
    ecs_os_api_malloc_count ++;
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return malloc((size_t)size);
}

static
void* ecs_os_api_calloc(ecs_size_t size) {
    ecs_os_api_calloc_count ++;
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);
    return calloc(1, (size_t)size);
}

static
void* ecs_os_api_realloc(void *ptr, ecs_size_t size) {
    ecs_assert(size > 0, ECS_INVALID_PARAMETER, NULL);

    if (ptr) {
        ecs_os_api_realloc_count ++;
    } else {
        /* If not actually reallocing, treat as malloc */
        ecs_os_api_malloc_count ++; 
    }
    
    return realloc(ptr, (size_t)size);
}

static
void ecs_os_api_free(void *ptr) {
    if (ptr) {
        ecs_os_api_free_count ++;
    }
    free(ptr);
}

static
char* ecs_os_api_strdup(const char *str) {
    if (str) {
        int len = ecs_os_strlen(str);
        char *result = ecs_os_malloc(len + 1);
        ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);
        ecs_os_strcpy(result, str);
        return result;
    } else {
        return NULL;
    }
}

/* Replace dots with underscores */
static
char *module_file_base(const char *module, char sep) {
    char *base = ecs_os_strdup(module);
    ecs_size_t i, len = ecs_os_strlen(base);
    for (i = 0; i < len; i ++) {
        if (base[i] == '.') {
            base[i] = sep;
        }
    }

    return base;
}

static
char* ecs_os_api_module_to_dl(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with underscores + OS library extension */
    char *file_base = module_file_base(module, '_');

#if defined(ECS_OS_LINUX)
    ecs_strbuf_appendstr(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".so");
#elif defined(ECS_OS_DARWIN)
    ecs_strbuf_appendstr(&lib, "lib");
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".dylib");
#elif defined(ECS_OS_WINDOWS)
    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, ".dll");
#endif

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

static
char* ecs_os_api_module_to_etc(const char *module) {
    ecs_strbuf_t lib = ECS_STRBUF_INIT;

    /* Best guess, use module name with dashes + /etc */
    char *file_base = module_file_base(module, '-');

    ecs_strbuf_appendstr(&lib, file_base);
    ecs_strbuf_appendstr(&lib, "/etc");

    ecs_os_free(file_base);

    return ecs_strbuf_get(&lib);
}

void ecs_os_set_api_defaults(void)
{
    /* Don't overwrite if already initialized */
    if (ecs_os_api_initialized != 0) {
        return;
    }

    flecs_os_time_setup();
    
    /* Memory management */
    ecs_os_api.malloc_ = ecs_os_api_malloc;
    ecs_os_api.free_ = ecs_os_api_free;
    ecs_os_api.realloc_ = ecs_os_api_realloc;
    ecs_os_api.calloc_ = ecs_os_api_calloc;

    /* Strings */
    ecs_os_api.strdup_ = ecs_os_api_strdup;

    /* Time */
    ecs_os_api.sleep_ = flecs_os_time_sleep;
    ecs_os_api.get_time_ = ecs_os_gettime;

    /* Logging */
    ecs_os_api.log_ = ecs_log;
    ecs_os_api.log_error_ = ecs_log_error;
    ecs_os_api.log_debug_ = ecs_log_debug;
    ecs_os_api.log_warning_ = ecs_log_warning;

    /* Modules */
    if (!ecs_os_api.module_to_dl_) {
        ecs_os_api.module_to_dl_ = ecs_os_api_module_to_dl;
    }

    if (!ecs_os_api.module_to_etc_) {
        ecs_os_api.module_to_etc_ = ecs_os_api_module_to_etc;
    }

    ecs_os_api.abort_ = abort;
}

bool ecs_os_has_heap(void) {
    return 
        (ecs_os_api.malloc_ != NULL) &&
        (ecs_os_api.calloc_ != NULL) &&
        (ecs_os_api.realloc_ != NULL) &&
        (ecs_os_api.free_ != NULL);
}

bool ecs_os_has_threading(void) {
    return
        (ecs_os_api.mutex_new_ != NULL) &&
        (ecs_os_api.mutex_free_ != NULL) &&
        (ecs_os_api.mutex_lock_ != NULL) &&
        (ecs_os_api.mutex_unlock_ != NULL) &&
        (ecs_os_api.cond_new_ != NULL) &&
        (ecs_os_api.cond_free_ != NULL) &&
        (ecs_os_api.cond_wait_ != NULL) &&
        (ecs_os_api.cond_signal_ != NULL) &&
        (ecs_os_api.cond_broadcast_ != NULL) &&
        (ecs_os_api.thread_new_ != NULL) &&
        (ecs_os_api.thread_join_ != NULL);   
}

bool ecs_os_has_time(void) {
    return 
        (ecs_os_api.get_time_ != NULL) &&
        (ecs_os_api.sleep_ != NULL);
}

bool ecs_os_has_logging(void) {
    return 
        (ecs_os_api.log_ != NULL) &&
        (ecs_os_api.log_error_ != NULL) &&
        (ecs_os_api.log_debug_ != NULL) &&
        (ecs_os_api.log_warning_ != NULL);
}

bool ecs_os_has_dl(void) {
    return 
        (ecs_os_api.dlopen_ != NULL) &&
        (ecs_os_api.dlproc_ != NULL) &&
        (ecs_os_api.dlclose_ != NULL);  
}

bool ecs_os_has_modules(void) {
    return 
        (ecs_os_api.module_to_dl_ != NULL) &&
        (ecs_os_api.module_to_etc_ != NULL);
}

#if defined(_MSC_VER)
static char error_str[255];
#endif

const char* ecs_os_strerror(int err) {
#if defined(_MSC_VER)
    strerror_s(error_str, 255, err);
    return error_str;
#else
    return strerror(err);
#endif
}

#ifdef FLECS_SYSTEMS_H
#endif

static
void activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active);

/* Builtin group_by callback for Cascade terms.
 * This function traces the hierarchy depth of an entity type by following a
 * relation upwards (to its 'parents') for as long as those parents have the
 * specified component id. 
 * The result of the function is the number of parents with the provided 
 * component for a given relation. */
static
int32_t group_by_cascade(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t component,
    void *ctx)
{
    int32_t result = 0;
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
    ecs_term_t *term = ctx;
    ecs_entity_t relation = term->args[0].set.relation;

    /* Cascade needs a relation to calculate depth from */
    ecs_assert(relation != 0, ECS_INVALID_PARAMETER, NULL);

    /* Should only be used with cascade terms */
    ecs_assert(term->args[0].set.mask & EcsCascade, 
        ECS_INVALID_PARAMETER, NULL);

    /* Iterate back to front as relations are more likely to occur near the
     * end of a type. */
    for (i = count - 1; i >= 0; i --) {
        /* Find relation & relation object in entity type */
        if (ECS_HAS_RELATION(array[i], relation)) {
            ecs_type_t obj_type = ecs_get_type(world,     
                ecs_pair_object(world, array[i]));
            int32_t j, c_count = ecs_vector_count(obj_type);
            ecs_entity_t *c_array = ecs_vector_first(obj_type, ecs_entity_t);

            /* Iterate object type, check if it has the specified component */
            for (j = 0; j < c_count; j ++) {
                /* If it has the component, it is part of the tree matched by
                 * the query, increase depth */
                if (c_array[j] == component) {
                    result ++;

                    /* Recurse to test if the object has matching parents */
                    result += group_by_cascade(world, obj_type, component, ctx);
                    break;
                }
            }

            if (j != c_count) {
                break;
            }

        /* If the id doesn't have a role set, we'll find no more relations */
        } else if (!(array[i] & ECS_ROLE_MASK)) {
            break;
        }
    }

    return result;
}

static
int table_compare(
    const void *t1,
    const void *t2)
{
    const ecs_matched_table_t *table_1 = t1;
    const ecs_matched_table_t *table_2 = t2;

    return table_1->rank - table_2->rank;
}

static
bool has_auto_activation(
    ecs_query_t *q)
{
    /* Only a basic query with no additional features does table activation */
    return !(q->flags & EcsQueryNoActivation);
}

static
void order_grouped_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_by) {
        ecs_vector_sort(query->tables, ecs_matched_table_t, table_compare);       

        /* Recompute the table indices by first resetting all indices, and then
         * re-adding them one by one. */
        if (has_auto_activation(query)) { 
            ecs_map_iter_t it = ecs_map_iter(query->table_indices);
            ecs_table_indices_t *ti;
            while ((ti = ecs_map_next(&it, ecs_table_indices_t, NULL))) {
                /* If table is registered, it must have at least one index */
                int32_t count = ti->count;
                ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
                (void)count;

                /* Only active tables are reordered, so don't reset inactive 
                 * tables */
                if (ti->indices[0] >= 0) {
                    ti->count = 0;
                }
            }
        }

        /* Re-register monitors after tables have been reordered. This will update
         * the table administration with the new matched_table ids, so that when a
         * monitor is executed we can quickly find the right matched_table. */
        if (query->flags & EcsQueryMonitor) { 
            ecs_vector_each(query->tables, ecs_matched_table_t, table, {        
                flecs_table_notify(world, table->table, &(ecs_table_event_t){
                    .kind = EcsTableQueryMatch,
                    .query = query,
                    .matched_table_index = table_i
                });
            });
        }

        /* Update table index */
        if (has_auto_activation(query)) {
            ecs_vector_each(query->tables, ecs_matched_table_t, table, {  
                ecs_table_indices_t *ti = ecs_map_get(query->table_indices, 
                    ecs_table_indices_t, table->table->id);

                ecs_assert(ti != NULL, ECS_INTERNAL_ERROR, NULL);
                ti->indices[ti->count] = table_i;
                ti->count ++;
            });
        }
    }
    
    query->match_count ++;
    query->needs_reorder = false;
}

static
void group_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_matched_table_t *table)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (query->group_by) {
        ecs_assert(table->table != NULL, ECS_INTERNAL_ERROR, NULL);
        table->rank = query->group_by(world, table->table->type, 
            query->group_by_id, query->group_by_ctx);
    } else {
        table->rank = 0;
    }
}

/* Rank all tables of query. Only necessary if a new ranking function was
 * provided or if a monitored entity set the component used for ranking. */
static
void group_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    if (query->group_by) {
        ecs_vector_each(query->tables, ecs_matched_table_t, table, {
            group_table(world, query, table);
        });

        ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
            group_table(world, query, table);
        });              
    }
}

#ifndef NDEBUG

static
const char* query_name(
    ecs_world_t *world,
    ecs_query_t *q)
{
    if (q->system) {
        return ecs_get_name(world, q->system);
    } else if (q->filter.name) {
        return q->filter.name;
    } else {
        return q->filter.expr;
    }
}

#endif

static
int get_comp_and_src(
    ecs_world_t *world,
    ecs_query_t *query,
    int32_t t,
    ecs_table_t *table_arg,
    ecs_entity_t *component_out,
    ecs_entity_t *entity_out)
{
    ecs_entity_t component = 0, entity = 0;

    ecs_term_t *terms = query->filter.terms;
    int32_t term_count = query->filter.term_count;
    ecs_term_t *term = &terms[t];
    ecs_term_id_t *subj = &term->args[0];
    ecs_oper_kind_t op = term->oper;

    if (op == EcsNot) {
        entity = subj->entity;
    }

    if (!subj->entity) {
        component = term->id;
    } else {
        ecs_table_t *table = table_arg;
        if (subj->entity != EcsThis) {
            table = ecs_get_table(world, subj->entity);
        }

        ecs_type_t type = NULL;
        if (table) {
            type = table->type;
        }

        if (op == EcsOr) {
            for (; t < term_count; t ++) {
                term = &terms[t];

                /* Keep iterating until the next non-OR expression */
                if (term->oper != EcsOr) {
                    t --;
                    break;
                }

                if (!component) {
                    ecs_entity_t source = 0;
                    int32_t result = ecs_type_match(world, table, type, 
                        0, term->id, subj->set.relation, subj->set.min_depth, 
                        subj->set.max_depth, 
                        &source);

                    if (result != -1) {
                        component = term->id;
                    }

                    if (source) {
                        entity = source;
                    }                    
                }
            }
        } else {
            component = term->id;

            ecs_entity_t source = 0;
            bool result = ecs_type_match(world, table, type, 0, component, 
                subj->set.relation, subj->set.min_depth, subj->set.max_depth, 
                &source) != -1;

            if (op == EcsNot) {
                result = !result;
            }

            /* Optional terms may not have the component. *From terms contain
             * the id of a type of which the contents must match, but the type
             * itself does not need to match. */
            if (op == EcsOptional || op == EcsAndFrom || op == EcsOrFrom || 
                op == EcsNotFrom) 
            {
                result = true;
            }

            /* Table has already been matched, so unless column is optional
             * any components matched from the table must be available. */
            if (table == table_arg) {
                ecs_assert(result == true, ECS_INTERNAL_ERROR, NULL);
            }

            if (source) {
                entity = source;
            }
        }

        if (subj->entity != EcsThis) {
            entity = subj->entity;
        }
    }

    if (entity == EcsThis) {
        entity = 0;
    }

    *component_out = component;
    *entity_out = entity;

    return t;
}

typedef struct pair_offset_t {
    int32_t index;
    int32_t count;
} pair_offset_t;

/* Get index for specified pair. Take into account that a pair can be matched
 * multiple times per table, by keeping an offset of the last found index */
static
int32_t get_pair_index(
    ecs_type_t table_type,
    ecs_id_t pair,
    int32_t column_index,
    pair_offset_t *pair_offsets,
    int32_t count)
{
    int32_t result;

    /* The count variable keeps track of the number of times a pair has been
     * matched with the current table. Compare the count to check if the index
     * was already resolved for this iteration */
    if (pair_offsets[column_index].count == count) {
        /* If it was resolved, return the last stored index. Subtract one as the
         * index is offset by one, to ensure we're not getting stuck on the same
         * index. */
        result = pair_offsets[column_index].index - 1;
    } else {
        /* First time for this iteration that the pair index is resolved, look
         * it up in the type. */
        result = ecs_type_index_of(table_type, 
            pair_offsets[column_index].index, pair);
        pair_offsets[column_index].index = result + 1;
        pair_offsets[column_index].count = count;
    }
    
    return result;
}

static
int32_t get_component_index(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_type_t table_type,
    ecs_entity_t *component_out,
    int32_t column_index,
    ecs_oper_kind_t op,
    pair_offset_t *pair_offsets,
    int32_t count)
{
    int32_t result = 0;
    ecs_entity_t component = *component_out;

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    if (component) {
        /* If requested component is a case, find the corresponding switch to
         * lookup in the table */
        if (ECS_HAS_ROLE(component, CASE)) {
            result = flecs_table_switch_from_case(
                world, table, component);
            ecs_assert(result != -1, ECS_INTERNAL_ERROR, NULL);

            result += table->sw_column_offset;
        } else
        if (ECS_HAS_ROLE(component, PAIR)) { 
            ecs_entity_t rel = ECS_PAIR_RELATION(component);
            ecs_entity_t obj = ECS_PAIR_OBJECT(component);

            /* Both the relationship and the object of the pair must be set */
            ecs_assert(rel != 0, ECS_INVALID_PARAMETER, NULL);
            ecs_assert(obj != 0, ECS_INVALID_PARAMETER, NULL);

            if (rel == EcsWildcard || obj == EcsWildcard) {
                ecs_assert(pair_offsets != NULL, ECS_INTERNAL_ERROR, NULL);

                /* Get index of pair. Start looking from the last pair index
                 * as this may not be the first instance of the pair. */
                result = get_pair_index(
                    table_type, component, column_index, pair_offsets, count);
                
                if (result != -1) {
                    /* If component of current column is a pair, get the actual 
                     * pair type for the table, so the system can see which 
                     * component the pair was applied to */   
                    ecs_entity_t *pair = ecs_vector_get(
                        table_type, ecs_entity_t, result);
                    *component_out = *pair;

                    char buf[256]; ecs_id_str(world, *pair, buf, 256);

                    /* Check if the pair is a tag or whether it has data */
                    if (ecs_get(world, rel, EcsComponent) == NULL) {
                        /* If pair has no data associated with it, use the
                         * component to which the pair has been added */
                        component = ECS_PAIR_OBJECT(*pair);
                    } else {
                        component = rel;
                    }
                }
            } else {

                /* If the low part is a regular entity (component), then
                 * this query exactly matches a single pair instance. In
                 * this case we can simply do a lookup of the pair 
                 * identifier in the table type. */
                result = ecs_type_index_of(table_type, 0, component);
            }
        } else {
            /* Get column index for component */
            result = ecs_type_index_of(table_type, 0, component);
        }

        /* If column is found, add one to the index, as column zero in
        * a table is reserved for entity id's */
        if (result != -1) {
            result ++;
        }     

        /* ecs_table_column_offset may return -1 if the component comes
         * from a prefab. If so, the component will be resolved as a
         * reference (see below) */           
    }

    if (op == EcsAndFrom || op == EcsOrFrom || op == EcsNotFrom) {
        result = 0;
    } else if (op == EcsOptional) {
        /* If table doesn't have the field, mark it as no data */
        if (!ecs_type_has_id(world, table_type, component, false)) {
            result = 0;
        }
    }  

    return result;
}

static
ecs_vector_t* add_ref(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_vector_t *references,
    ecs_term_t *term,
    ecs_entity_t component,
    ecs_entity_t entity)
{    
    ecs_ref_t *ref = ecs_vector_add(&references, ecs_ref_t);
    ecs_term_id_t *subj = &term->args[0];

    if (!(subj->set.mask & EcsCascade)) {
        ecs_assert(entity != 0, ECS_INTERNAL_ERROR, NULL);
    }
    
    *ref = (ecs_ref_t){0};
    ref->entity = entity;
    ref->component = component;

    const EcsComponent *c_info = flecs_component_from_id(world, component);
    if (c_info) {
        if (c_info->size && subj->entity != 0) {
            if (entity) {
                ecs_get_ref_w_id(world, ref, entity, component);
            }

            query->flags |= EcsQueryHasRefs;
        }
    }

    return references;
}

static
int32_t get_pair_count(
    ecs_type_t type,
    ecs_entity_t pair)
{
    int32_t i = -1, result = 0;
    while (-1 != (i = ecs_type_index_of(type, i + 1, pair))) {
        result ++;
    }

    return result;
}

/* For each pair that the query subscribes for, count the occurrences in the
 * table. Cardinality of subscribed for pairs must be the same as in the table
 * or else the table won't match. */
static
int32_t count_pairs(
    const ecs_query_t *query,
    ecs_type_t type)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;
    int32_t first_count = 0, pair_count = 0;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];

        if (!ECS_HAS_ROLE(term->id, PAIR)) {
            continue;
        }

        if (term->args[0].entity != EcsThis) {
            continue;
        }

        if (ecs_id_is_wildcard(term->id)) {
            pair_count = get_pair_count(type, term->id);
            if (!first_count) {
                first_count = pair_count;
            } else {
                if (first_count != pair_count) {
                    /* The pairs that this query subscribed for occur in the
                     * table but don't have the same cardinality. Ignore the
                     * table. This could typically happen for empty tables along
                     * a path in the table graph. */
                    return -1;
                }
            }
        }
    }

    return first_count;
}

static
ecs_type_t get_term_type(
    ecs_world_t *world,
    ecs_term_t *term,
    ecs_entity_t component)
{
    ecs_oper_kind_t oper = term->oper;

    if (oper == EcsAndFrom || oper == EcsOrFrom || oper == EcsNotFrom) {
        const EcsType *type = ecs_get(world, component, EcsType);
        if (type) {
            return type->normalized;
        } else {
            return ecs_get_type(world, component);
        }
    } else {
        return ecs_type_from_id(world, component);
    }    
}

/** Add table to system, compute offsets for system components in table it */
static
void add_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_type_t table_type = NULL;
    ecs_term_t *terms = query->filter.terms;
    int32_t t, c, term_count = query->filter.term_count;

    if (table) {
        table_type = table->type;
    }

    int32_t pair_cur = 0, pair_count = count_pairs(query, table_type);
    
    /* If the query has pairs, we need to account for the fact that a table may
     * have multiple components to which the pair is applied, which means the
     * table has to be registered with the query multiple times, with different
     * table columns. If so, allocate a small array for each pair in which the
     * last added table index of the pair is stored, so that in the next 
     * iteration we can start the search from the correct offset type. */
    pair_offset_t *pair_offsets = NULL;
    if (pair_count) {
        pair_offsets = ecs_os_calloc(
            ECS_SIZEOF(pair_offset_t) * term_count);
    }

    /* From here we recurse */
    int32_t *table_indices = NULL;
    int32_t table_indices_count = 0;
    int32_t matched_table_index = 0;
    ecs_matched_table_t table_data;
    ecs_vector_t *references = NULL;

add_pair:
    table_data = (ecs_matched_table_t){ .table = table };
    if (table) {
        table_type = table->type;
    }

    /* If grouping is enabled for query, assign the group rank to the table */
    group_table(world, query, &table_data);

    if (term_count) {
        /* Array that contains the system column to table column mapping */
        table_data.columns = ecs_os_calloc_n(int32_t, query->filter.term_count_actual);
        ecs_assert(table_data.columns != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Store the components of the matched table. In the case of OR expressions,
         * components may differ per matched table. */
        table_data.ids = ecs_os_calloc_n(ecs_entity_t, query->filter.term_count_actual);
        ecs_assert(table_data.ids != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Also cache types, so no lookup is needed while iterating */
        table_data.types = ecs_os_calloc_n(ecs_type_t, query->filter.term_count_actual);
        ecs_assert(table_data.types != NULL, ECS_OUT_OF_MEMORY, NULL);   

        /* Cache subject (source) entity ids for components */
        table_data.subjects = ecs_os_calloc_n(ecs_entity_t, query->filter.term_count_actual);
        ecs_assert(table_data.subjects != NULL, ECS_OUT_OF_MEMORY, NULL);

        /* Cache subject (source) entity ids for components */
        table_data.sizes = ecs_os_calloc_n(ecs_size_t, query->filter.term_count_actual);
        ecs_assert(table_data.sizes != NULL, ECS_OUT_OF_MEMORY, NULL);                     
    }

    /* Walk columns parsed from the system signature */
    c = 0;
    for (t = 0; t < term_count; t ++) {
        ecs_term_t *term = &terms[t];
        ecs_term_id_t subj = term->args[0];
        ecs_entity_t entity = 0, component = 0;
        ecs_oper_kind_t op = term->oper;

        if (op == EcsNot) {
            subj.entity = 0;
        }

        /* Get actual component and component source for current column */
        t = get_comp_and_src(world, query, t, table, &component, &entity);

        /* This column does not retrieve data from a static entity */
        if (!entity && subj.entity) {
            int32_t index = get_component_index(world, table, table_type, 
                &component, c, op, pair_offsets, pair_cur + 1);

            if (index == -1) {
                if (op == EcsOptional && subj.set.mask == EcsSelf) {
                    index = 0;
                }
            } else {
                if (op == EcsOptional && !(subj.set.mask & EcsSelf)) {
                    index = 0;
                }
            }

            table_data.columns[c] = index;

            /* If the column is a case, we should only iterate the entities in
             * the column for this specific case. Add a sparse column with the
             * case id so we can find the correct entities when iterating */
            if (ECS_HAS_ROLE(component, CASE)) {
                flecs_sparse_column_t *sc = ecs_vector_add(
                    &table_data.sparse_columns, flecs_sparse_column_t);
                sc->signature_column_index = t;
                sc->sw_case = component & ECS_COMPONENT_MASK;
                sc->sw_column = NULL;
            }

            /* If table has a disabled bitmask for components, check if there is
             * a disabled column for the queried for component. If so, cache it
             * in a vector as the iterator will need to skip the entity when the
             * component is disabled. */
            if (index && (table && table->flags & EcsTableHasDisabled)) {
                ecs_entity_t bs_id = 
                    (component & ECS_COMPONENT_MASK) | ECS_DISABLED;
                int32_t bs_index = ecs_type_index_of(table->type, 0, bs_id);
                if (bs_index != -1) {
                    flecs_bitset_column_t *elem = ecs_vector_add(
                        &table_data.bitset_columns, flecs_bitset_column_t);
                    elem->column_index = bs_index;
                    elem->bs_column = NULL;
                }
            }
        }

        ecs_entity_t type_id = ecs_get_typeid(world, component);

        if (entity || table_data.columns[c] == -1 || subj.set.mask & EcsCascade) {
            if (type_id) {
                references = add_ref(world, query, references, term,
                    component, entity);
                table_data.columns[c] = -ecs_vector_count(references);
            }

            table_data.subjects[c] = entity;
            flecs_set_watch(world, entity);
        }

        if (type_id) {
            const EcsComponent *cptr = ecs_get(world, type_id, EcsComponent);
            if (!cptr || !cptr->size) {
                int32_t column = table_data.columns[c];
                if (column < 0) {
                    ecs_ref_t *r = ecs_vector_get(
                        references, ecs_ref_t, -column - 1);
                    r->component = 0;
                }
            }

            if (cptr) {
                table_data.sizes[c] = cptr->size;
            } else {
                table_data.sizes[c] = 0;
            }
        } else {
            table_data.sizes[c] = 0;
        }


        if (ECS_HAS_ROLE(component, SWITCH)) {
            table_data.sizes[c] = ECS_SIZEOF(ecs_entity_t);
        } else if (ECS_HAS_ROLE(component, CASE)) {
            table_data.sizes[c] = ECS_SIZEOF(ecs_entity_t);
        }

        table_data.ids[c] = component;
        table_data.types[c] = get_term_type(world, term, component);

        c ++;
    }

    /* Initially always add table to inactive group. If the system is registered
     * with the table and the table is not empty, the table will send an
     * activate signal to the system. */

    ecs_matched_table_t *table_elem;
    if (table && has_auto_activation(query)) {
        table_elem = ecs_vector_add(&query->empty_tables, 
            ecs_matched_table_t);

        /* Store table index */
        matched_table_index = ecs_vector_count(query->empty_tables);
        table_indices_count ++;
        table_indices = ecs_os_realloc(
            table_indices, table_indices_count * ECS_SIZEOF(int32_t));
        table_indices[table_indices_count - 1] = -matched_table_index;

        #ifndef NDEBUG
        char *type_expr = ecs_type_str(world, table->type);
        ecs_trace_2("query #[green]%s#[reset] matched with table #[green][%s]",
            query_name(world, query), type_expr);
        ecs_os_free(type_expr);
        #endif
    } else {
        /* If no table is provided to function, this is a system that contains
         * no columns that require table matching. In this case, the system will
         * only have one "dummy" table that caches data from the system columns.
         * Always add this dummy table to the list of active tables, since it
         * would never get activated otherwise. */
        table_elem = ecs_vector_add(&query->tables, ecs_matched_table_t);

        /* If query doesn't automatically activates/inactivates tables, we can 
         * get the count to determine the current table index. */
        matched_table_index = ecs_vector_count(query->tables) - 1;
        ecs_assert(matched_table_index >= 0, ECS_INTERNAL_ERROR, NULL);
    }

    if (references) {
        ecs_size_t ref_size = ECS_SIZEOF(ecs_ref_t) * ecs_vector_count(references);
        table_data.references = ecs_os_malloc(ref_size);
        ecs_os_memcpy(table_data.references, 
            ecs_vector_first(references, ecs_ref_t), ref_size);
        ecs_vector_free(references);
        references = NULL;
    }

    *table_elem = table_data;

    /* Use tail recursion when adding table for multiple pairs */
    pair_cur ++;
    if (pair_cur < pair_count) {
        goto add_pair;
    }

    /* Register table indices before sending out the match signal. This signal
     * can cause table activation, and table indices are needed for that. */
    if (table_indices) {
        ecs_table_indices_t *ti = ecs_map_ensure(
            query->table_indices, ecs_table_indices_t, table->id);
        if (ti->indices) {
            ecs_os_free(ti->indices);
        }
        ti->indices = table_indices;
        ti->count = table_indices_count;
    }

    if (table && !(query->flags & EcsQueryIsSubquery)) {
        flecs_table_notify(world, table, &(ecs_table_event_t){
            .kind = EcsTableQueryMatch,
            .query = query,
            .matched_table_index = matched_table_index
        });
    } else if (table && ecs_table_count(table)) {
        activate_table(world, query, table, true);
    }

    if (pair_offsets) {
        ecs_os_free(pair_offsets);
    }
}

static
bool match_term(
    const ecs_world_t *world,
    const ecs_table_t *table,
    ecs_term_t *term,
    ecs_match_failure_t *failure_info)
{
    (void)failure_info;

    ecs_term_id_t *subj = &term->args[0];

    /* If term has no subject, there's nothing to match */
    if (!subj->entity) {
        return true;
    }

    if (term->args[0].entity != EcsThis) {
        table = ecs_get_table(world, subj->entity);
    }

    return ecs_type_match(
        world, table, table->type, 0, term->id, subj->set.relation, 
        subj->set.min_depth, subj->set.max_depth, NULL) != -1;
}

/* Match table with query */
bool flecs_query_match(
    const ecs_world_t *world,
    const ecs_table_t *table,
    const ecs_query_t *query,
    ecs_match_failure_t *failure_info)
{
    /* Prevent having to add if not null checks everywhere */
    ecs_match_failure_t tmp_failure_info;
    if (!failure_info) {
        failure_info = &tmp_failure_info;
    }

    failure_info->reason = EcsMatchOk;
    failure_info->column = 0;

    if (!(query->flags & EcsQueryNeedsTables)) {
        failure_info->reason = EcsMatchSystemIsATask;
        return false;
    }

    ecs_type_t table_type = table->type;

    /* Don't match disabled entities */
    if (!(query->flags & EcsQueryMatchDisabled) && ecs_type_has_id(
        world, table_type, EcsDisabled, true))
    {
        failure_info->reason = EcsMatchEntityIsDisabled;
        return false;
    }

    /* Don't match prefab entities */
    if (!(query->flags & EcsQueryMatchPrefab) && ecs_type_has_id(
        world, table_type, EcsPrefab, true))
    {
        failure_info->reason = EcsMatchEntityIsPrefab;
        return false;
    }

    /* Check if pair cardinality matches pairs in query, if any */
    if (count_pairs(query, table->type) == -1) {
        return false;
    }

    ecs_term_t *terms = query->filter.terms;
    int32_t i, term_count = query->filter.term_count;

    for (i = 0; i < term_count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_oper_kind_t oper = term->oper;

        failure_info->column = i + 1;

        if (oper == EcsAnd) {
            if (!match_term(world, table, term, failure_info)) {
                return false;
            }

        } else if (oper == EcsNot) {
            if (match_term(world, table, term, failure_info)) {
                return false;
            }

        } else if (oper == EcsOr) {
            bool match = false;

            for (; i < term_count; i ++) {
                term = &terms[i];
                if (term->oper != EcsOr) {
                    i --;
                    break;
                }

                if (!match && match_term(
                    world, table, term, failure_info))
                {
                    match = true;
                }
            }

            if (!match) {
                return false;
            }
 
        } else if (oper == EcsAndFrom || oper == EcsOrFrom || oper == EcsNotFrom) {
            ecs_type_t type = get_term_type((ecs_world_t*)world, term, term->id);
            int32_t match_count = 0, j, count = ecs_vector_count(type);
            ecs_entity_t *ids = ecs_vector_first(type, ecs_entity_t);

            for (j = 0; j < count; j ++) {
                ecs_term_t tmp_term = *term;
                tmp_term.oper = EcsAnd;
                tmp_term.id = ids[j];
                tmp_term.pred.entity = ids[j];

                if (match_term(world, table, &tmp_term, failure_info)) {
                    match_count ++;
                }
            }

            if (oper == EcsAndFrom && match_count != count) {
                return false;
            }
            if (oper == EcsOrFrom && match_count == 0) {
                return false;
            }
            if (oper == EcsNotFrom && match_count != 0) {
                return false;
            }
        }
    }

    return true;
}

/** Match existing tables against system (table is created before system) */
static
void match_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    int32_t i, count = flecs_sparse_count(world->store.tables);

    for (i = 0; i < count; i ++) {
        ecs_table_t *table = flecs_sparse_get_dense(
            world->store.tables, ecs_table_t, i);

        if (flecs_query_match(world, table, query, NULL)) {
            add_table(world, query, table);
        }
    }

    order_grouped_tables(world, query);
}

#define ELEM(ptr, size, index) ECS_OFFSET(ptr, size * index)

static
int32_t qsort_partition(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,    
    int32_t elem_size,
    int32_t lo,
    int32_t hi,
    ecs_order_by_action_t compare)
{
    int32_t p = (hi + lo) / 2;
    void *pivot = ELEM(ptr, elem_size, p);
    ecs_entity_t pivot_e = entities[p];
    int32_t i = lo - 1, j = hi + 1;
    void *el;    

repeat:
    {
        do {
            i ++;
            el = ELEM(ptr, elem_size, i);
        } while ( compare(entities[i], el, pivot_e, pivot) < 0);

        do {
            j --;
            el = ELEM(ptr, elem_size, j);
        } while ( compare(entities[j], el, pivot_e, pivot) > 0);

        if (i >= j) {
            return j;
        }

        flecs_table_swap(world, table, data, i, j);

        if (p == i) {
            pivot = ELEM(ptr, elem_size, j);
            pivot_e = entities[j];
        } else if (p == j) {
            pivot = ELEM(ptr, elem_size, i);
            pivot_e = entities[i];
        }

        goto repeat;
    }
}

static
void qsort_array(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_data_t *data,
    ecs_entity_t *entities,
    void *ptr,
    int32_t size,
    int32_t lo,
    int32_t hi,
    ecs_order_by_action_t compare)
{   
    if ((hi - lo) < 1)  {
        return;
    }

    int32_t p = qsort_partition(
        world, table, data, entities, ptr, size, lo, hi, compare);

    qsort_array(world, table, data, entities, ptr, size, lo, p, compare);

    qsort_array(world, table, data, entities, ptr, size, p + 1, hi, compare); 
}

static
void sort_table(
    ecs_world_t *world,
    ecs_table_t *table,
    int32_t column_index,
    ecs_order_by_action_t compare)
{
    ecs_data_t *data = flecs_table_get_data(table);
    if (!data || !data->entities) {
        /* Nothing to sort */
        return;
    }

    int32_t count = flecs_table_data_count(data);
    if (count < 2) {
        return;
    }

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);

    void *ptr = NULL;
    int32_t size = 0;
    if (column_index != -1) {
        ecs_column_t *column = &data->columns[column_index];
        size = column->size;
        ptr = ecs_vector_first_t(column->data, size, column->alignment);
    }

    qsort_array(world, table, data, entities, ptr, size, 0, count - 1, compare);
}

/* Helper struct for building sorted table ranges */
typedef struct sort_helper_t {
    ecs_matched_table_t *table;
    ecs_entity_t *entities;
    const void *ptr;
    int32_t row;
    int32_t elem_size;
    int32_t count;
    bool shared;
} sort_helper_t;

static
const void* ptr_from_helper(
    sort_helper_t *helper)
{
    ecs_assert(helper->row < helper->count, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->elem_size >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(helper->row >= 0, ECS_INTERNAL_ERROR, NULL);
    if (helper->shared) {
        return helper->ptr;
    } else {
        return ELEM(helper->ptr, helper->elem_size, helper->row);
    }
}

static
ecs_entity_t e_from_helper(
    sort_helper_t *helper)
{
    if (helper->row < helper->count) {
        return helper->entities[helper->row];
    } else {
        return 0;
    }
}

static
void build_sorted_table_range(
    ecs_query_t *query,
    int32_t start,
    int32_t end)
{
    ecs_world_t *world = query->world;
    ecs_entity_t component = query->order_by_component;
    ecs_order_by_action_t compare = query->order_by;

    /* Fetch data from all matched tables */
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    sort_helper_t *helper = ecs_os_malloc((end - start) * ECS_SIZEOF(sort_helper_t));

    int i, to_sort = 0;
    for (i = start; i < end; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->table;
        ecs_data_t *data = flecs_table_get_data(table);
        ecs_vector_t *entities;
        if (!data || !(entities = data->entities) || !ecs_table_count(table)) {
            continue;
        }

        int32_t index = ecs_type_index_of(table->type, 0, component);
        if (index != -1) {
            ecs_column_t *column = &data->columns[index];
            int16_t size = column->size;
            int16_t align = column->alignment;
            helper[to_sort].ptr = ecs_vector_first_t(column->data, size, align);
            helper[to_sort].elem_size = size;
            helper[to_sort].shared = false;
        } else if (component) {
            /* Find component in prefab */
            ecs_entity_t base;
            ecs_type_match(world, table, table->type, 0, component, 
                EcsIsA, 1, 0, &base);

            /* If a base was not found, the query should not have allowed using
             * the component for sorting */
            ecs_assert(base != 0, ECS_INTERNAL_ERROR, NULL);

            const EcsComponent *cptr = ecs_get(world, component, EcsComponent);
            ecs_assert(cptr != NULL, ECS_INTERNAL_ERROR, NULL);

            helper[to_sort].ptr = ecs_get_id(world, base, component);
            helper[to_sort].elem_size = cptr->size;
            helper[to_sort].shared = true;
        } else {
            helper[to_sort].ptr = NULL;
            helper[to_sort].elem_size = 0;
            helper[to_sort].shared = false;
        }

        helper[to_sort].table = table_data;
        helper[to_sort].entities = ecs_vector_first(entities, ecs_entity_t);
        helper[to_sort].row = 0;
        helper[to_sort].count = ecs_table_count(table);
        to_sort ++;      
    }

    ecs_table_slice_t *cur = NULL;

    bool proceed;
    do {
        int32_t j, min = 0;
        proceed = true;

        ecs_entity_t e1;
        while (!(e1 = e_from_helper(&helper[min]))) {
            min ++;
            if (min == to_sort) {
                proceed = false;
                break;
            }
        }

        if (!proceed) {
            break;
        }

        for (j = min + 1; j < to_sort; j++) {
            ecs_entity_t e2 = e_from_helper(&helper[j]);
            if (!e2) {
                continue;
            }

            const void *ptr1 = ptr_from_helper(&helper[min]);
            const void *ptr2 = ptr_from_helper(&helper[j]);

            if (compare(e1, ptr1, e2, ptr2) > 0) {
                min = j;
                e1 = e_from_helper(&helper[min]);
            }
        }

        sort_helper_t *cur_helper = &helper[min];
        if (!cur || cur->table != cur_helper->table) {
            cur = ecs_vector_add(&query->table_slices, ecs_table_slice_t);
            ecs_assert(cur != NULL, ECS_INTERNAL_ERROR, NULL);
            cur->table = cur_helper->table;
            cur->start_row = cur_helper->row;
            cur->count = 1;
        } else {
            cur->count ++;
        }

        cur_helper->row ++;
    } while (proceed);

    ecs_os_free(helper);
}

static
void build_sorted_tables(
    ecs_query_t *query)
{
    /* Clean previous sorted tables */
    ecs_vector_free(query->table_slices);
    query->table_slices = NULL;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, ecs_matched_table_t);
    ecs_matched_table_t *table = NULL;

    int32_t start = 0, rank = 0;
    for (i = 0; i < count; i ++) {
        table = &tables[i];
        if (rank != table->rank) {
            if (start != i) {
                build_sorted_table_range(query, start, i);
                start = i;
            }
            rank = table->rank;
        }
    }

    if (start != i) {
        build_sorted_table_range(query, start, i);
    }
}

static
bool tables_dirty(
    ecs_query_t *query)
{
    if (query->needs_reorder) {
        order_grouped_tables(query->world, query);
    }

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(query->tables, 
        ecs_matched_table_t);
    bool is_dirty = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->table;

        if (!table_data->monitor) {
            table_data->monitor = flecs_table_get_monitor(table);
            is_dirty = true;
        }

        int32_t *dirty_state = flecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            is_dirty = is_dirty || (dirty_state[t] != table_data->monitor[t]);
        }
    }

    is_dirty = is_dirty || (query->match_count != query->prev_match_count);

    return is_dirty;
}

static
void tables_reset_dirty(
    ecs_query_t *query)
{
    query->prev_match_count = query->match_count;

    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->table;

        if (!table_data->monitor) {
            /* If one table doesn't have a monitor, none of the tables will have
             * a monitor, so early out. */
            return;
        }

        int32_t *dirty_state = flecs_table_get_dirty_state(table);
        int32_t t, type_count = table->column_count;
        for (t = 0; t < type_count + 1; t ++) {
            table_data->monitor[t] = dirty_state[t];
        }
    }
}

static
void sort_tables(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_order_by_action_t compare = query->order_by;
    if (!compare) {
        return;
    }
    
    ecs_entity_t order_by_component = query->order_by_component;

    /* Iterate over active tables. Don't bother with inactive tables, since
     * they're empty */
    int32_t i, count = ecs_vector_count(query->tables);
    ecs_matched_table_t *tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);
    bool tables_sorted = false;

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table_data = &tables[i];
        ecs_table_t *table = table_data->table;

        /* If no monitor had been created for the table yet, create it now */
        bool is_dirty = false;
        if (!table_data->monitor) {
            table_data->monitor = flecs_table_get_monitor(table);

            /* A new table is always dirty */
            is_dirty = true;
        }

        int32_t *dirty_state = flecs_table_get_dirty_state(table);

        is_dirty = is_dirty || (dirty_state[0] != table_data->monitor[0]);

        int32_t index = -1;
        if (order_by_component) {
            /* Get index of sorted component. We only care if the component we're
            * sorting on has changed or if entities have been added / re(moved) */
            index = ecs_type_index_of(table->type, 0, order_by_component);
            if (index != -1) {
                ecs_assert(index < ecs_vector_count(table->type), ECS_INTERNAL_ERROR, NULL); 
                is_dirty = is_dirty || (dirty_state[index + 1] != table_data->monitor[index + 1]);
            } else {
                /* Table does not contain component which means the sorted
                 * component is shared. Table does not need to be sorted */
                continue;
            }
        }      
        
        /* Check both if entities have moved (element 0) or if the component
         * we're sorting on has changed (index + 1) */
        if (is_dirty) {
            /* Sort the table */
            sort_table(world, table, index, compare);
            tables_sorted = true;
        }
    }

    if (tables_sorted || query->match_count != query->prev_match_count) {
        build_sorted_tables(query);
        query->match_count ++; /* Increase version if tables changed */
    }
}

static
bool has_refs(
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];

        if (term->oper == EcsNot && !subj->entity) {
            /* Special case: if oper kind is Not and the query contained a
             * shared expression, the expression is translated to FromEmpty to
             * prevent resolving the ref */
            return true;
        } else if (subj->entity && (subj->entity != EcsThis || subj->set.mask != EcsSelf)) {
            /* If entity is not this, or if it can be substituted by other
             * entities, the query can have references. */
            return true;
        } 
    }

    return false;
}

static
bool has_pairs(
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        if (ecs_id_is_wildcard(terms[i].id)) {
            return true;
        }
    }

    return false;    
}

static
void register_monitors(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];

        /* If component is requested with EcsCascade register component as a
         * parent monitor. Parent monitors keep track of whether an entity moved
         * in the hierarchy, which potentially requires the query to reorder its
         * tables. 
         * Also register a regular component monitor for EcsCascade columns.
         * This ensures that when the component used in the EcsCascade column
         * is added or removed tables are updated accordingly*/
        if (subj->set.mask & EcsSuperSet && subj->set.mask & EcsCascade && 
            subj->set.relation != EcsIsA) 
        {
            if (term->oper != EcsOr) {
                if (term->args[0].set.relation != EcsIsA) {
                    flecs_monitor_register(
                        world, term->args[0].set.relation, term->id, query);
                }
                flecs_monitor_register(world, 0, term->id, query);
            }

        /* FromAny also requires registering a monitor, as FromAny columns can
         * be matched with prefabs. The only term kinds that do not require
         * registering a monitor are FromOwned and FromEmpty. */
        } else if ((subj->set.mask & EcsSuperSet) || (subj->entity != EcsThis)){
            if (term->oper != EcsOr) {
                flecs_monitor_register(world, 0, term->id, query);
            }
        }
    };
}

static
void process_signature(
    ecs_world_t *world,
    ecs_query_t *query)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *pred = &term->pred;
        ecs_term_id_t *subj = &term->args[0];
        ecs_term_id_t *obj = &term->args[1];
        ecs_oper_kind_t op = term->oper; 
        ecs_inout_kind_t inout = term->inout;

        (void)pred;
        (void)obj;

        /* Queries do not support variables */
        ecs_assert(pred->var != EcsVarIsVariable, 
            ECS_UNSUPPORTED, NULL);
        ecs_assert(subj->var != EcsVarIsVariable, 
            ECS_UNSUPPORTED, NULL);
        ecs_assert(obj->var != EcsVarIsVariable, 
            ECS_UNSUPPORTED, NULL);

        /* Queries do not support subset substitutions */
        ecs_assert(!(pred->set.mask & EcsSubSet), ECS_UNSUPPORTED, NULL);
        ecs_assert(!(subj->set.mask & EcsSubSet), ECS_UNSUPPORTED, NULL);
        ecs_assert(!(obj->set.mask & EcsSubSet), ECS_UNSUPPORTED, NULL);

        /* Superset/subset substitutions aren't supported for pred/obj */
        ecs_assert(pred->set.mask == EcsDefaultSet, ECS_UNSUPPORTED, NULL);
        ecs_assert(obj->set.mask == EcsDefaultSet, ECS_UNSUPPORTED, NULL);

        if (subj->set.mask == EcsDefaultSet) {
            subj->set.mask = EcsSelf;
        }

        /* If self is not included in set, always start from depth 1 */
        if (!subj->set.min_depth && !(subj->set.mask & EcsSelf)) {
            subj->set.min_depth = 1;
        }

        if (inout != EcsIn) {
            query->flags |= EcsQueryHasOutColumns;
        }

        if (op == EcsOptional) {
            query->flags |= EcsQueryHasOptional;
        }

        if (!(query->flags & EcsQueryMatchDisabled)) {
            if (op == EcsAnd || op == EcsOr || op == EcsOptional) {
                if (term->id == EcsDisabled) {
                    query->flags |= EcsQueryMatchDisabled;
                }
            }
        }

        if (!(query->flags & EcsQueryMatchPrefab)) {
            if (op == EcsAnd || op == EcsOr || op == EcsOptional) {
                if (term->id == EcsPrefab) {
                    query->flags |= EcsQueryMatchPrefab;
                }
            }
        }

        if (subj->entity == EcsThis) {
            query->flags |= EcsQueryNeedsTables;
        }

        if (subj->set.mask & EcsCascade && term->oper == EcsOptional) {
            /* Query can only have one cascade column */
            ecs_assert(query->cascade_by == 0, ECS_INVALID_PARAMETER, NULL);
            query->cascade_by = i + 1;
        }

        if (subj->entity && subj->entity != EcsThis && 
            subj->set.mask == EcsSelf) 
        {
            flecs_set_watch(world, term->args[0].entity);
        }
    }

    query->flags |= (ecs_flags32_t)(has_refs(query) * EcsQueryHasRefs);
    query->flags |= (ecs_flags32_t)(has_pairs(query) * EcsQueryHasTraits);

    if (!(query->flags & EcsQueryIsSubquery)) {
        register_monitors(world, query);
    }
}

static
bool match_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    if (flecs_query_match(world, table, query, NULL)) {
        add_table(world, query, table);
        return true;
    }
    return false;
}

/* Move table from empty to non-empty list, or vice versa */
static
int32_t move_table(
    ecs_query_t *query,
    ecs_table_t *table,
    int32_t index,
    ecs_vector_t **dst_array,
    ecs_vector_t *src_array,
    bool activate)
{
    (void)table;

    int32_t new_index = 0;
    int32_t last_src_index = ecs_vector_count(src_array) - 1;
    ecs_assert(last_src_index >= 0, ECS_INTERNAL_ERROR, NULL);

    ecs_matched_table_t *mt = ecs_vector_last(src_array, ecs_matched_table_t);
    
    /* The last table of the source array will be moved to the location of the
     * table to move, do some bookkeeping to keep things consistent. */
    if (last_src_index) {
        ecs_table_indices_t *ti = ecs_map_get(query->table_indices, 
            ecs_table_indices_t, mt->table->id);

        int i, count = ti->count;
        for (i = 0; i < count; i ++) {
            int32_t old_index = ti->indices[i];
            if (activate) {
                if (old_index >= 0) {
                    /* old_index should be negative if activate is true, since
                     * we're moving from the empty list to the non-empty list. 
                     * However, if the last table in the source array is also
                     * the table being moved, this can happen. */
                    ecs_assert(table == mt->table, 
                        ECS_INTERNAL_ERROR, NULL);
                    continue;
                }
                /* If activate is true, src = the empty list, and index should
                 * be negative. */
                old_index = old_index * -1 - 1; /* Normalize */
            }

            /* Ensure to update correct index, as there can be more than one */
            if (old_index == last_src_index) {
                if (activate) {
                    ti->indices[i] = index * -1 - 1;
                } else {
                    ti->indices[i] = index;
                }
                break;
            }
        }

        /* If the src array contains tables, there must be a table that will get
         * moved. */
        ecs_assert(i != count, ECS_INTERNAL_ERROR, NULL);
    } else {
        /* If last_src_index is 0, the table to move was the only table in the
         * src array, so no other administration needs to be updated. */
    }

    /* Actually move the table. Only move from src to dst if we have a
     * dst_array, otherwise just remove it from src. */
    if (dst_array) {
        new_index = ecs_vector_count(*dst_array);
        ecs_vector_move_index(dst_array, src_array, ecs_matched_table_t, index);

        /* Make sure table is where we expect it */
        mt = ecs_vector_last(*dst_array, ecs_matched_table_t);
        ecs_assert(mt->table == table, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(ecs_vector_count(*dst_array) == (new_index + 1), 
            ECS_INTERNAL_ERROR, NULL);  
    } else {
        ecs_vector_remove(src_array, ecs_matched_table_t, index);
    }

    /* Ensure that src array has now one element less */
    ecs_assert(ecs_vector_count(src_array) == last_src_index, 
        ECS_INTERNAL_ERROR, NULL);

    /* Return new index for table */
    if (activate) {
        /* Table is now active, index is positive */
        return new_index;
    } else {
        /* Table is now inactive, index is negative */
        return new_index * -1 - 1;
    }
}

/** Table activation happens when a table was or becomes empty. Deactivated
 * tables are not considered by the system in the main loop. */
static
void activate_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table,
    bool active)
{    
    ecs_vector_t *src_array, *dst_array;
    int32_t activated = 0;
    int32_t prev_dst_count = 0;
    (void)world;
    (void)prev_dst_count; /* Only used when built with systems module */

    if (active) {
        src_array = query->empty_tables;
        dst_array = query->tables;
        prev_dst_count = ecs_vector_count(dst_array);
    } else {
        src_array = query->tables;
        dst_array = query->empty_tables;
    }

    ecs_table_indices_t *ti = ecs_map_get(
        query->table_indices, ecs_table_indices_t, table->id);

    if (ti) {
        int32_t i, count = ti->count;
        for (i = 0; i < count; i ++) {
            int32_t index = ti->indices[i];

            if (index < 0) {
                if (!active) {
                    /* If table is already inactive, no need to move */
                    continue;
                }
                index = index * -1 - 1;
            } else {
                if (active) {
                    /* If table is already active, no need to move */
                    continue;
                }
            }

            ecs_matched_table_t *mt = ecs_vector_get(
                src_array, ecs_matched_table_t, index);
            ecs_assert(mt->table == table, ECS_INTERNAL_ERROR, NULL);
            (void)mt;
            
            activated ++;

            ti->indices[i] = move_table(
                query, table, index, &dst_array, src_array, active);
        }

        if (activated) {
            /* Activate system if registered with query */
#ifdef FLECS_SYSTEMS_H
            if (query->system) {
                int32_t dst_count = ecs_vector_count(dst_array);
                if (active) {
                    if (!prev_dst_count && dst_count) {
                        ecs_system_activate(world, query->system, true, NULL);
                    }
                } else if (ecs_vector_count(src_array) == 0) {
                    ecs_system_activate(world, query->system, false, NULL);
                }
            }
#endif
        }

        if (active)  {
            query->tables = dst_array;
        } else {
            query->empty_tables = dst_array;
        }
    }
    
    if (!activated) {
        /* Received an activate event for a table we're not matched with. This
         * can only happen if this is a subquery */
        ecs_assert((query->flags & EcsQueryIsSubquery) != 0, 
            ECS_INTERNAL_ERROR, NULL);
        return;
    }

    /* Signal query it needs to reorder tables. Doing this in place could slow
     * down scenario's where a large number of tables is matched with an ordered
     * query. Since each table would trigger the activate signal, there would be
     * as many sorts as added tables, vs. only one when ordering happens when an
     * iterator is obtained. */
    query->needs_reorder = true;
}

static
void add_subquery(
    ecs_world_t *world, 
    ecs_query_t *parent, 
    ecs_query_t *subquery) 
{
    ecs_query_t **elem = ecs_vector_add(&parent->subqueries, ecs_query_t*);
    *elem = subquery;

    /* Iterate matched tables, match them with subquery */
    ecs_matched_table_t *tables = ecs_vector_first(parent->tables, ecs_matched_table_t);
    int32_t i, count = ecs_vector_count(parent->tables);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table = &tables[i];
        match_table(world, subquery, table->table);
        activate_table(world, subquery, table->table, true);
    }

    /* Do the same for inactive tables */
    tables = ecs_vector_first(parent->empty_tables, ecs_matched_table_t);
    count = ecs_vector_count(parent->empty_tables);

    for (i = 0; i < count; i ++) {
        ecs_matched_table_t *table = &tables[i];
        match_table(world, subquery, table->table);
    }    
}

static
void notify_subqueries(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event)
{
    if (query->subqueries) {
        ecs_query_t **queries = ecs_vector_first(query->subqueries, ecs_query_t*);
        int32_t i, count = ecs_vector_count(query->subqueries);

        ecs_query_event_t sub_event = *event;
        sub_event.parent_query = query;

        for (i = 0; i < count; i ++) {
            ecs_query_t *sub = queries[i];
            flecs_query_notify(world, sub, &sub_event);
        }
    }
}

static
void free_matched_table(
    ecs_matched_table_t *table)
{
    ecs_os_free(table->columns);
    ecs_os_free(table->ids);
    ecs_os_free(table->subjects);
    ecs_os_free(table->sizes);
    ecs_os_free((ecs_vector_t**)table->types);
    ecs_os_free(table->references);
    ecs_os_free(table->sparse_columns);
    ecs_os_free(table->bitset_columns);
    ecs_os_free(table->monitor);
}

/** Check if a table was matched with the system */
static
ecs_table_indices_t* get_table_indices(
    ecs_query_t *query,
    ecs_table_t *table)
{
    return ecs_map_get(query->table_indices, ecs_table_indices_t, table->id);
}

static
void resolve_cascade_subject(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_indices_t *ti,
    const ecs_table_t *table,
    ecs_type_t table_type)
{
    int32_t term_index = query->cascade_by - 1;
    ecs_term_t *term = &query->filter.terms[term_index];

    /* For each table entry, find the correct subject of a cascade term */
    int32_t i, count = ti->count;
    for (i = 0; i < count; i ++) {
        int32_t table_data_index = ti->indices[i];
        ecs_matched_table_t *table_data;

        if (table_data_index >= 0) {
            table_data = ecs_vector_get(
                query->tables, ecs_matched_table_t, table_data_index);            
        } else {
            table_data = ecs_vector_get(
                query->empty_tables, ecs_matched_table_t, 
                    -1 * table_data_index - 1);
        }
        
        ecs_assert(table_data->references != 0, 
            ECS_INTERNAL_ERROR, NULL);

        /* Obtain reference index */
        int32_t *column_indices = table_data->columns;
        int32_t ref_index = -column_indices[term_index] - 1;

        /* Obtain pointer to the reference data */
        ecs_ref_t *references = table_data->references;

        /* Find source for component */
        ecs_entity_t subject;
        ecs_type_match(world, table, table_type, 0, term->id, 
            term->args[0].set.relation, 1, 0, &subject);

        /* If container was found, update the reference */
        if (subject) {
            ecs_ref_t *ref = &references[ref_index];
            ecs_assert(ref->component == term->id, ECS_INTERNAL_ERROR, NULL);

            references[ref_index].entity = ecs_get_alive(world, subject);
            table_data->subjects[term_index] = subject;
            ecs_get_ref_w_id(world, ref, subject, term->id);
        } else {
            references[ref_index].entity = 0;
            table_data->subjects[term_index] = 0;
        }
    }
}

/* Remove table */
static
void remove_table(
    ecs_query_t *query,
    ecs_table_t *table,
    ecs_vector_t *tables,
    int32_t index,
    bool empty)
{
    ecs_matched_table_t *mt = ecs_vector_get(
        tables, ecs_matched_table_t, index);
    if (!mt) {
        /* Query was notified of a table it doesn't match with, this can only
         * happen if query is a subquery. */
        ecs_assert(query->flags & EcsQueryIsSubquery, ECS_INTERNAL_ERROR, NULL);
        return;
    }
    
    ecs_assert(mt->table == table, ECS_INTERNAL_ERROR, NULL);
    (void)table;

    /* Free table before moving, as the move will cause another table to occupy
     * the memory of mt */
    free_matched_table(mt);  
    move_table(query, mt->table, index, NULL, tables, empty);
}

static
void unmatch_table(
    ecs_query_t *query,
    ecs_table_t *table,
    ecs_table_indices_t *ti)
{
    if (!ti) {
        ti = get_table_indices(query, table);
        if (!ti) {
            return;
        }
    }

    int32_t i, count = ti->count;
    for (i = 0; i < count; i ++) {
        int32_t index = ti->indices[i];
        if (index < 0) {
            index = index * -1 - 1;
            remove_table(query, table, query->empty_tables, index, true);
        } else {
            remove_table(query, table, query->tables, index, false);
        }
    }

    ecs_os_free(ti->indices);
    ecs_map_remove(query->table_indices, table->id);
}

static
void rematch_table(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_table_t *table)
{
    ecs_table_indices_t *match = get_table_indices(query, table);

    if (flecs_query_match(world, table, query, NULL)) {
        /* If the table matches, and it is not currently matched, add */
        if (match == NULL) {
            add_table(world, query, table);

        /* If table still matches and has cascade column, reevaluate the
         * sources of references. This may have changed in case 
         * components were added/removed to container entities */ 
        } else if (query->cascade_by) {
            resolve_cascade_subject(world, query, match, table, table->type);

        /* If query has optional columns, it is possible that a column that
         * previously had data no longer has data, or vice versa. Do a full
         * rematch to make sure data is consistent. */
        } else if (query->flags & EcsQueryHasOptional) {
            unmatch_table(query, table, match);
            if (!(query->flags & EcsQueryIsSubquery)) {
                flecs_table_notify(world, table, &(ecs_table_event_t){
                    .kind = EcsTableQueryUnmatch,
                    .query = query
                }); 
            }
            add_table(world, query, table);
        }
    } else {
        /* Table no longer matches, remove */
        if (match != NULL) {
            unmatch_table(query, table, match);
            if (!(query->flags & EcsQueryIsSubquery)) {
                flecs_table_notify(world, table, &(ecs_table_event_t){
                    .kind = EcsTableQueryUnmatch,
                    .query = query
                });
            }
            notify_subqueries(world, query, &(ecs_query_event_t){
                .kind = EcsQueryTableUnmatch,
                .table = table
            });
        }
    }
}

static
bool satisfy_constraints(
    ecs_world_t *world,
    const ecs_filter_t *filter)
{
    ecs_term_t *terms = filter->terms;
    int32_t i, count = filter->term_count;

    for (i = 0; i < count; i ++) {
        ecs_term_t *term = &terms[i];
        ecs_term_id_t *subj = &term->args[0];
        ecs_oper_kind_t oper = term->oper;

        if (subj->entity != EcsThis && subj->set.mask & EcsSelf) {
            ecs_type_t type = ecs_get_type(world, subj->entity);

            if (ecs_type_has_id(world, type, term->id, false)) {
                if (oper == EcsNot) {
                    return false;
                }
            } else {
                if (oper != EcsNot) {
                    return false;
                }
            }
        }
    }

    return true;
}

/* Rematch system with tables after a change happened to a watched entity */
static
void rematch_tables(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_t *parent_query)
{
    if (parent_query) {
        ecs_matched_table_t *tables = ecs_vector_first(parent_query->tables, ecs_matched_table_t);
        int32_t i, count = ecs_vector_count(parent_query->tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = tables[i].table;
            rematch_table(world, query, table);
        }

        tables = ecs_vector_first(parent_query->empty_tables, ecs_matched_table_t);
        count = ecs_vector_count(parent_query->empty_tables);
        for (i = 0; i < count; i ++) {
            ecs_table_t *table = tables[i].table;
            rematch_table(world, query, table);
        }        
    } else {
        ecs_sparse_t *tables = world->store.tables;
        int32_t i, count = flecs_sparse_count(tables);

        for (i = 0; i < count; i ++) {
            /* Is the system currently matched with the table? */
            ecs_table_t *table = flecs_sparse_get_dense(tables, ecs_table_t, i);
            rematch_table(world, query, table);
        }
    }

    group_tables(world, query);
    order_grouped_tables(world, query);

    /* Enable/disable system if constraints are (not) met. If the system is
     * already dis/enabled this operation has no side effects. */
    query->constraints_satisfied = satisfy_constraints(world, &query->filter);      
}

static
void remove_subquery(
    ecs_query_t *parent, 
    ecs_query_t *sub)
{
    ecs_assert(parent != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(sub != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(parent->subqueries != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = ecs_vector_count(parent->subqueries);
    ecs_query_t **sq = ecs_vector_first(parent->subqueries, ecs_query_t*);

    for (i = 0; i < count; i ++) {
        if (sq[i] == sub) {
            break;
        }
    }

    ecs_vector_remove(parent->subqueries, ecs_query_t*, i);
}

/* -- Private API -- */

void flecs_query_notify(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_query_event_t *event)
{
    bool notify = true;

    switch(event->kind) {
    case EcsQueryTableMatch:
        /* Creation of new table */
        if (match_table(world, query, event->table)) {
            if (query->subqueries) {
                notify_subqueries(world, query, event);
            }
        }
        notify = false;
        break;
    case EcsQueryTableUnmatch:
        /* Deletion of table */
        unmatch_table(query, event->table, NULL);
        break;
    case EcsQueryTableRematch:
        /* Rematch tables of query */
        rematch_tables(world, query, event->parent_query);
        break;        
    case EcsQueryTableEmpty:
        /* Table is empty, deactivate */
        activate_table(world, query, event->table, false);
        break;
    case EcsQueryTableNonEmpty:
        /* Table is non-empty, activate */
        activate_table(world, query, event->table, true);
        break;
    case EcsQueryOrphan:
        ecs_assert(query->flags & EcsQueryIsSubquery, ECS_INTERNAL_ERROR, NULL);
        query->flags |= EcsQueryIsOrphaned;
        query->parent = NULL;
        break;
    }

    if (notify) {
        notify_subqueries(world, query, event);
    }
}

void ecs_query_order_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t order_by_component,
    ecs_order_by_action_t order_by)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->order_by_component = order_by_component;
    query->order_by = order_by;

    ecs_vector_free(query->table_slices);
    query->table_slices = NULL;

    sort_tables(world, query);    

    if (!query->table_slices) {
        build_sorted_tables(query);
    }
}

void ecs_query_group_by(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_entity_t sort_component,
    ecs_group_by_action_t group_by)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);    
    ecs_assert(query->flags & EcsQueryNeedsTables, ECS_INVALID_PARAMETER, NULL);

    query->group_by_id = sort_component;
    query->group_by = group_by;

    group_tables(world, query);

    order_grouped_tables(world, query);

    build_sorted_tables(query);
}


/* -- Public API -- */

ecs_query_t* ecs_query_init(
    ecs_world_t *world,
    const ecs_query_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    ecs_query_t *result = flecs_sparse_add(world->queries, ecs_query_t);
    result->id = flecs_sparse_last_id(world->queries);

    if (ecs_filter_init(world, &result->filter, &desc->filter)) {
        flecs_sparse_remove(world->queries, result->id);
        return NULL;
    }

    result->world = world;
    result->table_indices = ecs_map_new(ecs_table_indices_t, 0);
    result->tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->empty_tables = ecs_vector_new(ecs_matched_table_t, 0);
    result->system = desc->system;
    result->prev_match_count = -1;

    if (desc->parent != NULL) {
        result->flags |= EcsQueryIsSubquery;
    }

    /* If a system is specified, ensure that if there are any subjects in the
     * filter that refer to the system, the component is added */
    if (desc->system)  {
        int32_t t, term_count = result->filter.term_count;
        ecs_term_t *terms = result->filter.terms;

        for (t = 0; t < term_count; t ++) {
            ecs_term_t *term = &terms[t];
            if (term->args[0].entity == desc->system) {
                ecs_add_id(world, desc->system, term->id);
            }
        }        
    }

    process_signature(world, result);

    ecs_trace_2("query #[green]%s#[reset] created with expression #[red]%s", 
        query_name(world, result), result->filter.expr);

    ecs_log_push();

    if (!desc->parent) {
        if (result->flags & EcsQueryNeedsTables) {
            if (desc->system) {
                if (ecs_has_id(world, desc->system, EcsMonitor)) {
                    result->flags |= EcsQueryMonitor;
                }
                
                if (ecs_has_id(world, desc->system, EcsOnSet)) {
                    result->flags |= EcsQueryOnSet;
                }

                if (ecs_has_id(world, desc->system, EcsUnSet)) {
                    result->flags |= EcsQueryUnSet;
                }  
            }      

            match_tables(world, result);
        } else {
            /* Add stub table that resolves references (if any) so everything is
             * preprocessed when the query is evaluated. */
            add_table(world, result, NULL);
        }
    } else {
        add_subquery(world, desc->parent, result);
        result->parent = desc->parent;
    }

    result->constraints_satisfied = satisfy_constraints(world, &result->filter);

    int32_t cascade_by = result->cascade_by;
    if (cascade_by) {
        result->group_by = group_by_cascade;
        result->group_by_id = result->filter.terms[cascade_by - 1].id;
        result->group_by_ctx = &result->filter.terms[cascade_by - 1];
    }

    if (desc->order_by) {
        ecs_query_order_by(
            world, result, desc->order_by_component, desc->order_by);
    }

    if (desc->group_by) {
        /* Can't have a cascade term and group by at the same time, as cascade
         * uses the group_by mechanism */
        ecs_assert(!result->cascade_by, ECS_INVALID_PARAMETER, NULL);
        ecs_query_group_by(world, result, desc->group_by_id, desc->group_by);
        result->group_by_ctx = desc->group_by_ctx;
        result->group_by_ctx_free = desc->group_by_ctx_free;
    }

    ecs_log_pop();

    return result;
}

void ecs_query_fini(
    ecs_query_t *query)
{
    ecs_world_t *world = query->world;
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);

    if (query->group_by_ctx_free) {
        if (query->group_by_ctx) {
            query->group_by_ctx_free(query->group_by_ctx);
        }
    }

    if ((query->flags & EcsQueryIsSubquery) &&
        !(query->flags & EcsQueryIsOrphaned))
    {
        remove_subquery(query->parent, query);
    }

    notify_subqueries(world, query, &(ecs_query_event_t){
        .kind = EcsQueryOrphan
    });

    ecs_vector_each(query->empty_tables, ecs_matched_table_t, table, {
        if (!(query->flags & EcsQueryIsSubquery)) {
            flecs_table_notify(world, table->table, &(ecs_table_event_t){
                .kind = EcsTableQueryUnmatch,
                .query = query
            });
        }    
        free_matched_table(table);
    });

    ecs_vector_each(query->tables, ecs_matched_table_t, table, {
        if (!(query->flags & EcsQueryIsSubquery)) {
            flecs_table_notify(world, table->table, &(ecs_table_event_t){
                .kind = EcsTableQueryUnmatch,
                .query = query
            });
        }
        free_matched_table(table);
    });

    ecs_map_iter_t it = ecs_map_iter(query->table_indices);
    ecs_table_indices_t *ti;
    while ((ti = ecs_map_next(&it, ecs_table_indices_t, NULL))) {
        ecs_os_free(ti->indices);
    }

    ecs_map_free(query->table_indices);
    ecs_vector_free(query->subqueries);
    ecs_vector_free(query->tables);
    ecs_vector_free(query->empty_tables);
    ecs_vector_free(query->table_slices);
    ecs_filter_fini(&query->filter);
    
    /* Remove query from storage */
    flecs_sparse_remove(world->queries, query->id);
}

const ecs_filter_t* ecs_query_get_filter(
    ecs_query_t *query)
{
    return &query->filter;
}

/* Create query iterator */
ecs_iter_t ecs_query_iter_page(
    ecs_query_t *query,
    int32_t offset,
    int32_t limit)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);

    ecs_world_t *world = query->world;

    if (query->needs_reorder) {
        order_grouped_tables(world, query);
    }
    
    sort_tables(world, query);

    if (!world->is_readonly && query->flags & EcsQueryHasRefs) {
        flecs_eval_component_monitors(world);
    }

    tables_reset_dirty(query);

    int32_t table_count;
    if (query->table_slices) {
        table_count = ecs_vector_count(query->table_slices);
    } else {
        table_count = ecs_vector_count(query->tables);
    }

    ecs_query_iter_t it = {
        .page_iter = {
            .offset = offset,
            .limit = limit,
            .remaining = limit
        },
        .index = 0,
    };

    return (ecs_iter_t){
        .world = world,
        .query = query,
        .column_count = query->filter.term_count_actual,
        .table_count = table_count,
        .inactive_table_count = ecs_vector_count(query->empty_tables),
        .iter.query = it
    };
}

ecs_iter_t ecs_query_iter(
    ecs_query_t *query)
{
    return ecs_query_iter_page(query, 0, 0);
}

static
void populate_ptrs(
    ecs_world_t *world,
    ecs_iter_t *it)
{
    ecs_table_t *table = it->table;
    const ecs_data_t *data = NULL;
    ecs_column_t *columns = NULL;

    if (table) {
        data = flecs_table_get_data(table);
    }
    if (data) {
        columns = data->columns;
    }

    int c;
    for (c = 0; c < it->column_count; c ++) {
        int32_t c_index = it->columns[c];
        if (!c_index) {
            it->ptrs[c] = NULL;
            continue;
        }

        if (c_index > 0) {
            c_index --;

            if (!columns) {
                continue;
            }

            if (it->sizes[c] == 0) {
                continue;
            }

            ecs_column_t *col = &columns[c_index];
            it->ptrs[c] = ecs_vector_get_t(
                col->data, col->size, col->alignment, it->offset);
        } else {
            ecs_ref_t *ref = &it->references[-c_index - 1];
            char buf[255]; ecs_id_str(world, ref->component, buf, 255);
            it->ptrs[c] = (void*)ecs_get_ref_w_id(
                world, ref, ref->entity, ref->component);
        }
    }    
}

void flecs_query_set_iter(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_iter_t *it,
    int32_t table_index,
    int32_t row,
    int32_t count)
{
    (void)world;
    
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);
    
    ecs_matched_table_t *table_data = ecs_vector_get(
        query->tables, ecs_matched_table_t, table_index);
    ecs_assert(table_data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_table_t *table = table_data->table;
    ecs_data_t *data = flecs_table_get_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
    
    ecs_entity_t *entity_buffer = ecs_vector_first(data->entities, ecs_entity_t);  
    it->entities = &entity_buffer[row];

    it->world = NULL;
    it->query = query;
    it->column_count = query->filter.term_count_actual;
    it->table_count = 1;
    it->inactive_table_count = 0;
    it->table_columns = data->columns;
    it->table = table;
    it->ids = table_data->ids;
    it->columns = table_data->columns;
    it->types = table_data->types;
    it->subjects = table_data->subjects;
    it->sizes = table_data->sizes;
    it->references = table_data->references;
    it->offset = row;
    it->count = count;
    it->total_count = count;

    ecs_iter_init(it);

    populate_ptrs(world, it);
}

static
int ecs_page_iter_next(
    ecs_page_iter_t *it,
    ecs_page_cursor_t *cur)
{
    int32_t offset = it->offset;
    int32_t limit = it->limit;
    if (!(offset || limit)) {
        return cur->count == 0;
    }

    int32_t count = cur->count;
    int32_t remaining = it->remaining;

    if (offset) {
        if (offset > count) {
            /* No entities to iterate in current table */
            it->offset -= count;
            return 1;
        } else {
            cur->first += offset;
            count = cur->count -= offset;
            it->offset = 0;
        }
    }

    if (remaining) {
        if (remaining > count) {
            it->remaining -= count;
        } else {
            count = cur->count = remaining;
            it->remaining = 0;
        }
    } else if (limit) {
        /* Limit hit: no more entities left to iterate */
        return -1;
    }

    return count == 0;
}

static
int find_smallest_column(
    ecs_table_t *table,
    ecs_matched_table_t *table_data,
    ecs_vector_t *sparse_columns)
{
    flecs_sparse_column_t *sparse_column_array = 
        ecs_vector_first(sparse_columns, flecs_sparse_column_t);
    int32_t i, count = ecs_vector_count(sparse_columns);
    int32_t min = INT_MAX, index = 0;

    for (i = 0; i < count; i ++) {
        /* The array with sparse queries for the matched table */
        flecs_sparse_column_t *sparse_column = &sparse_column_array[i];

        /* Pointer to the switch column struct of the table */
        ecs_sw_column_t *sc = sparse_column->sw_column;

        /* If the sparse column pointer hadn't been retrieved yet, do it now */
        if (!sc) {
            /* Get the table column index from the signature column index */
            int32_t table_column_index = table_data->columns[
                sparse_column->signature_column_index];

            /* Translate the table column index to switch column index */
            table_column_index -= table->sw_column_offset;
            ecs_assert(table_column_index >= 1, ECS_INTERNAL_ERROR, NULL);

            /* Get the sparse column */
            ecs_data_t *data = flecs_table_get_data(table);
            sc = sparse_column->sw_column = 
                &data->sw_columns[table_column_index - 1];
        }

        /* Find the smallest column */
        ecs_switch_t *sw = sc->data;
        int32_t case_count = flecs_switch_case_count(sw, sparse_column->sw_case);
        if (case_count < min) {
            min = case_count;
            index = i + 1;
        }
    }

    return index;
}

static
int sparse_column_next(
    ecs_table_t *table,
    ecs_matched_table_t *matched_table,
    ecs_vector_t *sparse_columns,
    ecs_query_iter_t *iter,
    ecs_page_cursor_t *cur)
{
    bool first_iteration = false;
    int32_t sparse_smallest;

    if (!(sparse_smallest = iter->sparse_smallest)) {
        sparse_smallest = iter->sparse_smallest = find_smallest_column(
            table, matched_table, sparse_columns);
        first_iteration = true;
    }

    sparse_smallest -= 1;

    flecs_sparse_column_t *columns = ecs_vector_first(
        sparse_columns, flecs_sparse_column_t);
    flecs_sparse_column_t *column = &columns[sparse_smallest];
    ecs_switch_t *sw, *sw_smallest = column->sw_column->data;
    ecs_entity_t case_smallest = column->sw_case;

    /* Find next entity to iterate in sparse column */
    int32_t first;
    if (first_iteration) {
        first = flecs_switch_first(sw_smallest, case_smallest);
    } else {
        first = flecs_switch_next(sw_smallest, iter->sparse_first);
    }

    if (first == -1) {
        goto done;
    }    

    /* Check if entity matches with other sparse columns, if any */
    int32_t i, count = ecs_vector_count(sparse_columns);
    do {
        for (i = 0; i < count; i ++) {
            if (i == sparse_smallest) {
                /* Already validated this one */
                continue;
            }

            column = &columns[i];
            sw = column->sw_column->data;

            if (flecs_switch_get(sw, first) != column->sw_case) {
                first = flecs_switch_next(sw_smallest, first);
                if (first == -1) {
                    goto done;
                }
            }
        }
    } while (i != count);

    cur->first = iter->sparse_first = first;
    cur->count = 1;

    return 0;
done:
    /* Iterated all elements in the sparse list, we should move to the
     * next matched table. */
    iter->sparse_smallest = 0;
    iter->sparse_first = 0;

    return -1;
}

#define BS_MAX ((uint64_t)0xFFFFFFFFFFFFFFFF)

static
int bitset_column_next(
    ecs_table_t *table,
    ecs_vector_t *bitset_columns,
    ecs_query_iter_t *iter,
    ecs_page_cursor_t *cur)
{
    /* Precomputed single-bit test */
    static const uint64_t bitmask[64] = {
    (uint64_t)1 << 0, (uint64_t)1 << 1, (uint64_t)1 << 2, (uint64_t)1 << 3,
    (uint64_t)1 << 4, (uint64_t)1 << 5, (uint64_t)1 << 6, (uint64_t)1 << 7,
    (uint64_t)1 << 8, (uint64_t)1 << 9, (uint64_t)1 << 10, (uint64_t)1 << 11,
    (uint64_t)1 << 12, (uint64_t)1 << 13, (uint64_t)1 << 14, (uint64_t)1 << 15,
    (uint64_t)1 << 16, (uint64_t)1 << 17, (uint64_t)1 << 18, (uint64_t)1 << 19,
    (uint64_t)1 << 20, (uint64_t)1 << 21, (uint64_t)1 << 22, (uint64_t)1 << 23,
    (uint64_t)1 << 24, (uint64_t)1 << 25, (uint64_t)1 << 26, (uint64_t)1 << 27,  
    (uint64_t)1 << 28, (uint64_t)1 << 29, (uint64_t)1 << 30, (uint64_t)1 << 31,
    (uint64_t)1 << 32, (uint64_t)1 << 33, (uint64_t)1 << 34, (uint64_t)1 << 35,  
    (uint64_t)1 << 36, (uint64_t)1 << 37, (uint64_t)1 << 38, (uint64_t)1 << 39,
    (uint64_t)1 << 40, (uint64_t)1 << 41, (uint64_t)1 << 42, (uint64_t)1 << 43,
    (uint64_t)1 << 44, (uint64_t)1 << 45, (uint64_t)1 << 46, (uint64_t)1 << 47,  
    (uint64_t)1 << 48, (uint64_t)1 << 49, (uint64_t)1 << 50, (uint64_t)1 << 51,
    (uint64_t)1 << 52, (uint64_t)1 << 53, (uint64_t)1 << 54, (uint64_t)1 << 55,  
    (uint64_t)1 << 56, (uint64_t)1 << 57, (uint64_t)1 << 58, (uint64_t)1 << 59,
    (uint64_t)1 << 60, (uint64_t)1 << 61, (uint64_t)1 << 62, (uint64_t)1 << 63
    };

    /* Precomputed test to verify if remainder of block is set (or not) */
    static const uint64_t bitmask_remain[64] = {
    BS_MAX, BS_MAX - (BS_MAX >> 63), BS_MAX - (BS_MAX >> 62),
    BS_MAX - (BS_MAX >> 61), BS_MAX - (BS_MAX >> 60), BS_MAX - (BS_MAX >> 59),
    BS_MAX - (BS_MAX >> 58), BS_MAX - (BS_MAX >> 57), BS_MAX - (BS_MAX >> 56), 
    BS_MAX - (BS_MAX >> 55), BS_MAX - (BS_MAX >> 54), BS_MAX - (BS_MAX >> 53), 
    BS_MAX - (BS_MAX >> 52), BS_MAX - (BS_MAX >> 51), BS_MAX - (BS_MAX >> 50), 
    BS_MAX - (BS_MAX >> 49), BS_MAX - (BS_MAX >> 48), BS_MAX - (BS_MAX >> 47), 
    BS_MAX - (BS_MAX >> 46), BS_MAX - (BS_MAX >> 45), BS_MAX - (BS_MAX >> 44), 
    BS_MAX - (BS_MAX >> 43), BS_MAX - (BS_MAX >> 42), BS_MAX - (BS_MAX >> 41), 
    BS_MAX - (BS_MAX >> 40), BS_MAX - (BS_MAX >> 39), BS_MAX - (BS_MAX >> 38), 
    BS_MAX - (BS_MAX >> 37), BS_MAX - (BS_MAX >> 36), BS_MAX - (BS_MAX >> 35), 
    BS_MAX - (BS_MAX >> 34), BS_MAX - (BS_MAX >> 33), BS_MAX - (BS_MAX >> 32), 
    BS_MAX - (BS_MAX >> 31), BS_MAX - (BS_MAX >> 30), BS_MAX - (BS_MAX >> 29), 
    BS_MAX - (BS_MAX >> 28), BS_MAX - (BS_MAX >> 27), BS_MAX - (BS_MAX >> 26), 
    BS_MAX - (BS_MAX >> 25), BS_MAX - (BS_MAX >> 24), BS_MAX - (BS_MAX >> 23), 
    BS_MAX - (BS_MAX >> 22), BS_MAX - (BS_MAX >> 21), BS_MAX - (BS_MAX >> 20), 
    BS_MAX - (BS_MAX >> 19), BS_MAX - (BS_MAX >> 18), BS_MAX - (BS_MAX >> 17), 
    BS_MAX - (BS_MAX >> 16), BS_MAX - (BS_MAX >> 15), BS_MAX - (BS_MAX >> 14), 
    BS_MAX - (BS_MAX >> 13), BS_MAX - (BS_MAX >> 12), BS_MAX - (BS_MAX >> 11), 
    BS_MAX - (BS_MAX >> 10), BS_MAX - (BS_MAX >> 9), BS_MAX - (BS_MAX >> 8), 
    BS_MAX - (BS_MAX >> 7), BS_MAX - (BS_MAX >> 6), BS_MAX - (BS_MAX >> 5), 
    BS_MAX - (BS_MAX >> 4), BS_MAX - (BS_MAX >> 3), BS_MAX - (BS_MAX >> 2),
    BS_MAX - (BS_MAX >> 1)
    };

    int32_t i, count = ecs_vector_count(bitset_columns);
    flecs_bitset_column_t *columns = ecs_vector_first(
        bitset_columns, flecs_bitset_column_t);
    int32_t bs_offset = table->bs_column_offset;

    int32_t first = iter->bitset_first;
    int32_t last = 0;

    for (i = 0; i < count; i ++) {
        flecs_bitset_column_t *column = &columns[i];
        ecs_bs_column_t *bs_column = columns[i].bs_column;

        if (!bs_column) {
            ecs_data_t *data = table->data;
            int32_t index = column->column_index;
            ecs_assert((index - bs_offset >= 0), ECS_INTERNAL_ERROR, NULL);
            bs_column = &data->bs_columns[index - bs_offset];
            columns[i].bs_column = bs_column;
        }
        
        ecs_bitset_t *bs = &bs_column->data;
        int32_t bs_elem_count = bs->count;
        int32_t bs_block = first >> 6;
        int32_t bs_block_count = ((bs_elem_count - 1) >> 6) + 1;

        if (bs_block >= bs_block_count) {
            goto done;
        }

        uint64_t *data = bs->data;
        int32_t bs_start = first & 0x3F;

        /* Step 1: find the first non-empty block */
        uint64_t v = data[bs_block];
        uint64_t remain = bitmask_remain[bs_start];
        while (!(v & remain)) {
            /* If no elements are remaining, move to next block */
            if ((++bs_block) >= bs_block_count) {
                /* No non-empty blocks left */
                goto done;
            }

            bs_start = 0;
            remain = BS_MAX; /* Test the full block */
            v = data[bs_block];
        }

        /* Step 2: find the first non-empty element in the block */
        while (!(v & bitmask[bs_start])) {
            bs_start ++;

            /* Block was not empty, so bs_start must be smaller than 64 */
            ecs_assert(bs_start < 64, ECS_INTERNAL_ERROR, NULL);
        }
        
        /* Step 3: Find number of contiguous enabled elements after start */
        int32_t bs_end = bs_start, bs_block_end = bs_block;
        
        remain = bitmask_remain[bs_end];
        while ((v & remain) == remain) {
            bs_end = 0;
            bs_block_end ++;

            if (bs_block_end == bs_block_count) {
                break;
            }

            v = data[bs_block_end];
            remain = BS_MAX; /* Test the full block */
        }

        /* Step 4: find remainder of enabled elements in current block */
        if (bs_block_end != bs_block_count) {
            while ((v & bitmask[bs_end])) {
                bs_end ++;
            }
        }

        /* Block was not 100% occupied, so bs_start must be smaller than 64 */
        ecs_assert(bs_end < 64, ECS_INTERNAL_ERROR, NULL);

        /* Step 5: translate to element start/end and make sure that each column
         * range is a subset of the previous one. */
        first = bs_block * 64 + bs_start;
        int32_t cur_last = bs_block_end * 64 + bs_end;
        
        /* No enabled elements found in table */
        if (first == cur_last) {
            goto done;
        }

        /* If multiple bitsets are evaluated, make sure each subsequent range
         * is equal or a subset of the previous range */
        if (i) {
            /* If the first element of a subsequent bitset is larger than the
             * previous last value, start over. */
            if (first >= last) {
                i = -1;
                continue;
            }

            /* Make sure the last element of the range doesn't exceed the last
             * element of the previous range. */
            if (cur_last > last) {
                cur_last = last;
            }
        }
        
        last = cur_last;
        int32_t elem_count = last - first;

        /* Make sure last element doesn't exceed total number of elements in 
         * the table */
        if (elem_count > bs_elem_count) {
            elem_count = bs_elem_count;
        }
        
        cur->first = first;
        cur->count = elem_count;
        iter->bitset_first = first;
    }
    
    /* Keep track of last processed element for iteration */ 
    iter->bitset_first = last;

    return 0;
done:
    return -1;
}

static
void mark_columns_dirty(
    ecs_query_t *query,
    ecs_matched_table_t *table_data)
{
    ecs_table_t *table = table_data->table;

    if (table && table->dirty_state) {
        ecs_term_t *terms = query->filter.terms;
        int32_t c = 0, i, count = query->filter.term_count;
        for (i = 0; i < count; i ++) {
            ecs_term_t *term = &terms[i];
            ecs_term_id_t *subj = &term->args[0];

            if (term->inout != EcsIn && (term->inout != EcsInOutDefault || 
                (subj->entity == EcsThis && subj->set.mask == EcsSelf)))
            {
                int32_t table_column = table_data->columns[c];
                if (table_column > 0 && table_column < table->column_count) {
                    table->dirty_state[table_column] ++;
                }
            }

            if (terms[i].oper == EcsOr) {
                do {
                    i ++;
                } while ((i < count) && terms[i].oper == EcsOr);
            }

            c ++;
        }
    }
}

/* Return next table */
bool ecs_query_next(
    ecs_iter_t *it)
{
    ecs_assert(it != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_query_iter_t *iter = &it->iter.query;
    ecs_page_iter_t *piter = &iter->page_iter;
    ecs_query_t *query = it->query;
    ecs_world_t *world = query->world;
    (void)world;

    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_iter_init(it);

    if (!query->constraints_satisfied) {
        goto done;
    }

    ecs_table_slice_t *slice = ecs_vector_first(
        query->table_slices, ecs_table_slice_t);
    ecs_matched_table_t *tables = ecs_vector_first(
        query->tables, ecs_matched_table_t);

    ecs_assert(!slice || query->order_by, ECS_INTERNAL_ERROR, NULL);
    
    ecs_page_cursor_t cur;
    int32_t table_count = it->table_count;
    int32_t prev_count = it->total_count;

    int i;
    for (i = iter->index; i < table_count; i ++) {
        ecs_matched_table_t *table_data = slice ? slice[i].table : &tables[i];
        ecs_table_t *table = table_data->table;
        ecs_data_t *data = NULL;

        iter->index = i + 1;

        if (table) {
            ecs_vector_t *bitset_columns = table_data->bitset_columns;
            ecs_vector_t *sparse_columns = table_data->sparse_columns;
            data = flecs_table_get_data(table);
            ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);
            it->table_columns = data->columns;
            
            if (slice) {
                cur.first = slice[i].start_row;
                cur.count = slice[i].count;                
            } else {
                cur.first = 0;
                cur.count = ecs_table_count(table);
            }

            if (cur.count) {
                if (bitset_columns) {
                    if (bitset_column_next(table, bitset_columns, iter, 
                        &cur) == -1) 
                    {
                        /* No more enabled components for table */
                        continue; 
                    } else {
                        iter->index = i;
                    }
                }

                if (sparse_columns) {
                    if (sparse_column_next(table, table_data,
                        sparse_columns, iter, &cur) == -1)
                    {
                        /* No more elements in sparse column */
                        continue;    
                    } else {
                        iter->index = i;
                    }
                }

                int ret = ecs_page_iter_next(piter, &cur);
                if (ret < 0) {
                    goto done;
                } else if (ret > 0) {
                    continue;
                }
            } else {
                continue;
            }

            ecs_entity_t *entity_buffer = ecs_vector_first(
                data->entities, ecs_entity_t); 
            it->entities = &entity_buffer[cur.first];
            it->offset = cur.first;
            it->count = cur.count;
            it->total_count = cur.count;
        }

        it->table = table_data->table;
        it->ids = table_data->ids;
        it->columns = table_data->columns;
        it->types = table_data->types;
        it->subjects = table_data->subjects;
        it->sizes = table_data->sizes;
        it->references = table_data->references;
        it->frame_offset += prev_count;

        populate_ptrs(world, it);

        if (query->flags & EcsQueryHasOutColumns) {
            if (table) {
                mark_columns_dirty(query, table_data);
            }
        }

        goto yield;
    }

done:
    ecs_iter_fini(it);
    return false;
    
yield:
    return true;  
}

bool ecs_query_next_w_filter(
    ecs_iter_t *iter,
    const ecs_filter_t *filter)
{
    ecs_table_t *table;

    do {
        if (!ecs_query_next(iter)) {
            return false;
        }
        table = iter->table;
    } while (filter && !flecs_table_match_filter(iter->world, table, filter));
    
    return true;
}

bool ecs_query_next_worker(
    ecs_iter_t *it,
    int32_t current,
    int32_t total)
{
    int32_t per_worker, first, prev_offset = it->offset;

    do {
        if (!ecs_query_next(it)) {
            return false;
        }

        int32_t count = it->count;
        per_worker = count / total;
        first = per_worker * current;

        count -= per_worker * total;

        if (count) {
            if (current < count) {
                per_worker ++;
                first += current;
            } else {
                first += count;
            }
        }

        if (!per_worker && !(it->query->flags & EcsQueryNeedsTables)) {
            if (current == 0) {
                populate_ptrs(it->world, it);
                return true;
            } else {
                return false;
            }
        }
    } while (!per_worker);

    it->frame_offset -= prev_offset;
    it->count = per_worker;
    it->offset += first;
    it->entities = &it->entities[first];
    it->frame_offset += first;

    populate_ptrs(it->world, it);

    return true;
}

bool ecs_query_changed(
    ecs_query_t *query)
{
    ecs_assert(query != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!(query->flags & EcsQueryIsOrphaned), ECS_INVALID_PARAMETER, NULL);
    return tables_dirty(query);
}

bool ecs_query_orphaned(
    ecs_query_t *query)
{
    return query->flags & EcsQueryIsOrphaned;
}


static
uint64_t ids_hash(const void *ptr) {
    const ecs_ids_t *type = ptr;
    ecs_id_t *ids = type->array;
    int32_t count = type->count;
    uint64_t hash = flecs_hash(ids, count * ECS_SIZEOF(ecs_id_t));
    return hash;
}

static
int ids_compare(const void *ptr_1, const void *ptr_2) {
    const ecs_ids_t *type_1 = ptr_1;
    const ecs_ids_t *type_2 = ptr_2;

    int32_t count_1 = type_1->count;
    int32_t count_2 = type_2->count;

    if (count_1 != count_2) {
        return (count_1 > count_2) - (count_1 < count_2);
    }

    const ecs_id_t *ids_1 = type_1->array;
    const ecs_id_t *ids_2 = type_2->array;
    
    int32_t i;
    for (i = 0; i < count_1; i ++) {
        ecs_id_t id_1 = ids_1[i];
        ecs_id_t id_2 = ids_2[i];

        if (id_1 != id_2) {
            return (id_1 > id_2) - (id_1 < id_2);
        }
    }

    return 0;
}

ecs_hashmap_t flecs_table_hashmap_new(void) {
    return flecs_hashmap_new(ecs_ids_t, ecs_table_t*, ids_hash, ids_compare);
}

const EcsComponent* flecs_component_from_id(
    const ecs_world_t *world,
    ecs_entity_t e)
{
    ecs_entity_t pair = 0;

    /* If this is a pair, get the pair component from the identifier */
    if (ECS_HAS_ROLE(e, PAIR)) {
        pair = e;
        e = ecs_get_alive(world, ECS_PAIR_RELATION(e));

        if (ecs_has_id(world, e, EcsTag)) {
            return NULL;
        }
    }

    if (e & ECS_ROLE_MASK) {
        return NULL;
    }

    const EcsComponent *component = ecs_get(world, e, EcsComponent);
    if ((!component || !component->size) && pair) {
        /* If this is a pair column and the pair is not a component, use
         * the component type of the component the pair is applied to. */
        e = ECS_PAIR_OBJECT(pair);

        /* Because generations are not stored in the pair, get the currently
         * alive id */
        e = ecs_get_alive(world, e);

        /* If a pair is used with a not alive id, the pair is not valid */
        ecs_assert(e != 0, ECS_INTERNAL_ERROR, NULL);

        component = ecs_get(world, e, EcsComponent);
    }

    return component;
}

/* Count number of columns with data (excluding tags) */
static
int32_t data_column_count(
    ecs_world_t * world,
    ecs_table_t * table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        /* Typically all components will be clustered together at the start of
         * the type as components are created from a separate id pool, and type
         * vectors are sorted. 
         * Explicitly check for EcsComponent and EcsName since the ecs_has check
         * doesn't work during bootstrap. */
        if ((component == ecs_id(EcsComponent)) || 
            (component == ecs_pair(ecs_id(EcsIdentifier), EcsName)) || 
            (component == ecs_pair(ecs_id(EcsIdentifier), EcsSymbol)) || 
            flecs_component_from_id(world, component) != NULL) 
        {
            count = c_ptr_i + 1;
        }
    });

    return count;
}

/* Ensure the ids used in the columns exist */
static
int32_t ensure_columns(
    ecs_world_t * world,
    ecs_table_t * table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, PAIR)) {
            ecs_entity_t rel = ECS_PAIR_RELATION(component);
            ecs_entity_t obj = ECS_PAIR_OBJECT(component);
            ecs_ensure(world, rel);
            ecs_ensure(world, obj);
        } else if (component & ECS_ROLE_MASK) {
            ecs_entity_t e = ECS_PAIR_OBJECT(component);
            ecs_ensure(world, e);
        } else {
            ecs_ensure(world, component);
        }
    });

    return count;
}

/* Count number of switch columns */
static
int32_t switch_column_count(
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, SWITCH)) {
            if (!count) {
                table->sw_column_offset = c_ptr_i;
            }
            count ++;
        }
    });

    return count;
}

/* Count number of bitset columns */
static
int32_t bitset_column_count(
    ecs_table_t *table)
{
    int32_t count = 0;
    ecs_vector_each(table->type, ecs_entity_t, c_ptr, {
        ecs_entity_t component = *c_ptr;

        if (ECS_HAS_ROLE(component, DISABLED)) {
            if (!count) {
                table->bs_column_offset = c_ptr_i;
            }
            count ++;
        }
    });

    return count;
}

static
ecs_type_t entities_to_type(
    ecs_ids_t *entities)
{
    if (entities->count) {
        ecs_vector_t *result = NULL;
        ecs_vector_set_count(&result, ecs_entity_t, entities->count);
        ecs_entity_t *array = ecs_vector_first(result, ecs_entity_t);
        ecs_os_memcpy(array, entities->array, ECS_SIZEOF(ecs_entity_t) * entities->count);
        return result;
    } else {
        return NULL;
    }
}

static
ecs_edge_t* get_edge(
    ecs_table_t *node,
    ecs_entity_t e)
{
    if (e < ECS_HI_COMPONENT_ID) {
        if (!node->lo_edges) {
            node->lo_edges = ecs_os_calloc_n(ecs_edge_t, ECS_HI_COMPONENT_ID);
        }
        return &node->lo_edges[e];
    } else {
        if (!node->hi_edges) {
            node->hi_edges = ecs_map_new(ecs_edge_t, 1);
        }
        return ecs_map_ensure(node->hi_edges, ecs_edge_t, e);
    }
}

static
void init_edges(
    ecs_table_t * table)
{
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    int32_t count = ecs_vector_count(table->type);

    table->lo_edges = NULL;
    table->hi_edges = NULL;
    
    /* Iterate components for table, initialize edges that point to self */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        ecs_edge_t *edge = get_edge(table, id);
        ecs_assert(edge != NULL, ECS_INTERNAL_ERROR, NULL);
        edge->add = table;    
    }
}

static
void init_flags(
    ecs_world_t * world,
    ecs_table_t * table)
{
    ecs_id_t *ids = ecs_vector_first(table->type, ecs_id_t);
    int32_t count = ecs_vector_count(table->type);
    
    /* Iterate components to initialize table flags */
    int32_t i;
    for (i = 0; i < count; i ++) {
        ecs_id_t id = ids[i];

        /* As we're iterating over the table components, also set the table
         * flags. These allow us to quickly determine if the table contains
         * data that needs to be handled in a special way, like prefabs or 
         * containers */
        if (id <= EcsLastInternalComponentId) {
            table->flags |= EcsTableHasBuiltins;
        }

        if (id == EcsModule) {
            table->flags |= EcsTableHasBuiltins;
            table->flags |= EcsTableHasModule;
        }

        if (id == EcsPrefab) {
            table->flags |= EcsTableIsPrefab;
            table->flags |= EcsTableIsDisabled;
        }

        /* If table contains disabled entities, mark it as disabled */
        if (id == EcsDisabled) {
            table->flags |= EcsTableIsDisabled;
        }

        /* Does table have exclusive or columns */
        if (ECS_HAS_ROLE(id, XOR)) {
            table->flags |= EcsTableHasXor;
        }

        /* Does table have IsA relations */
        if (ECS_HAS_RELATION(id, EcsIsA)) {
            table->flags |= EcsTableHasIsA;
        }

        /* Does table have switch columns */
        if (ECS_HAS_ROLE(id, SWITCH)) {
            table->flags |= EcsTableHasSwitch;
        }

        /* Does table support component disabling */
        if (ECS_HAS_ROLE(id, DISABLED)) {
            table->flags |= EcsTableHasDisabled;
        }   

        /* Does table have ChildOf relations */
        if (ECS_HAS_RELATION(id, EcsChildOf)) {
            ecs_entity_t obj = ecs_pair_object(world, id);
            if (obj == EcsFlecs || obj == EcsFlecsCore || 
                ecs_has_id(world, obj, EcsModule)) 
            {
                /* If table contains entities that are inside one of the builtin
                 * modules, it contains builtin entities */
                table->flags |= EcsTableHasBuiltins;
                table->flags |= EcsTableHasModule;
            }
        }      
    }
}

static
void init_table(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_ids_t * entities)
{
    table->type = entities_to_type(entities);
    table->c_info = NULL;
    table->data = NULL;
    table->flags = 0;
    table->dirty_state = NULL;
    table->monitors = NULL;
    table->on_set = NULL;
    table->on_set_all = NULL;
    table->on_set_override = NULL;
    table->un_set_all = NULL;
    table->alloc_count = 0;
    table->lock = 0;

    /* Ensure the component ids for the table exist */
    ensure_columns(world, table);

    table->queries = NULL;
    table->column_count = data_column_count(world, table);
    table->sw_column_count = switch_column_count(table);
    table->bs_column_count = bitset_column_count(table);

    init_edges(table);
    init_flags(world, table);

    flecs_register_table(world, table);

    /* Register component info flags for all columns */
    flecs_table_notify(world, table, &(ecs_table_event_t){
        .kind = EcsTableComponentInfo
    });    
}

static
ecs_table_t *create_table(
    ecs_world_t * world,
    ecs_ids_t * entities,
    flecs_hashmap_result_t table_elem)
{
    ecs_table_t *result = flecs_sparse_add(world->store.tables, ecs_table_t);
    result->id = flecs_sparse_last_id(world->store.tables);

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
    init_table(world, result, entities);

#ifndef NDEBUG
    char *expr = ecs_type_str(world, result->type);
    ecs_trace_2("table #[green][%s]#[normal] created [%p]", expr, result);
    ecs_os_free(expr);
#endif
    ecs_log_push();

    /* Store table in table hashmap */
    *(ecs_table_t**)table_elem.value = result;

    /* Set keyvalue to one that has the same lifecycle as the table */
    ecs_ids_t key = {
        .array = ecs_vector_first(result->type, ecs_id_t),
        .count = ecs_vector_count(result->type)
    };
    *(ecs_ids_t*)table_elem.key = key;

    flecs_notify_queries(world, &(ecs_query_event_t) {
        .kind = EcsQueryTableMatch,
        .table = result
    });

    ecs_log_pop();

    return result;
}

static
void add_entity_to_type(
    ecs_type_t type,
    ecs_entity_t add,
    ecs_entity_t replace,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);    
    bool added = false;

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e == replace) {
            continue;
        }

        if (e > add && !added) {
            out->array[el ++] = add;
            added = true;
        }
        
        out->array[el ++] = e;

        ecs_assert(el <= out->count, ECS_INTERNAL_ERROR, NULL);
    }

    if (!added) {
        out->array[el ++] = add;
    }

    out->count = el;

    ecs_assert(out->count != 0, ECS_INTERNAL_ERROR, NULL);
}

static
void remove_entity_from_type(
    ecs_type_t type,
    ecs_entity_t remove,
    ecs_ids_t *out)
{
    int32_t count = ecs_vector_count(type);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    int32_t i, el = 0;
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (e != remove) {
            out->array[el ++] = e;
            ecs_assert(el <= count, ECS_INTERNAL_ERROR, NULL);
        }
    }

    out->count = el;
}

static
void create_backlink_after_add(
    ecs_table_t * next,
    ecs_table_t * prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->remove) {
        edge->remove = prev;
    }
}

static
void create_backlink_after_remove(
    ecs_table_t * next,
    ecs_table_t * prev,
    ecs_entity_t add)
{
    ecs_edge_t *edge = get_edge(next, add);
    if (!edge->add) {
        edge->add = prev;
    }
}

static
ecs_entity_t find_xor_replace(
    ecs_world_t * world,
    ecs_table_t * table,
    ecs_type_t type,
    ecs_entity_t add)
{
    if (table->flags & EcsTableHasXor) {
        ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);
        int32_t i, type_count = ecs_vector_count(type);
        ecs_type_t xor_type = NULL;

        for (i = type_count - 1; i >= 0; i --) {
            ecs_entity_t e = array[i];
            if (ECS_HAS_ROLE(e, XOR)) {
                ecs_entity_t e_type = e & ECS_COMPONENT_MASK;
                const EcsType *type_ptr = ecs_get(world, e_type, EcsType);
                ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

                if (ecs_type_has_id(
                    world, type_ptr->normalized, add, true)) 
                {
                    xor_type = type_ptr->normalized;
                }
            } else if (xor_type) {
                if (ecs_type_has_id(world, xor_type, e, true)) {
                    return e;
                }
            }
        }
    }

    return 0;
}

int32_t flecs_table_switch_from_case(
    const ecs_world_t * world,
    const ecs_table_t * table,
    ecs_entity_t add)
{
    ecs_type_t type = table->type;
    ecs_data_t *data = flecs_table_get_data(table);
    ecs_entity_t *array = ecs_vector_first(type, ecs_entity_t);

    int32_t i, count = table->sw_column_count;
    ecs_assert(count != 0, ECS_INTERNAL_ERROR, NULL);

    add = add & ECS_COMPONENT_MASK;

    ecs_sw_column_t *sw_columns = NULL;

    if (data && (sw_columns = data->sw_columns)) {
        /* Fast path, we can get the switch type from the column data */
        for (i = 0; i < count; i ++) {
            ecs_type_t sw_type = sw_columns[i].type;
            if (ecs_type_has_id(world, sw_type, add, true)) {
                return i;
            }
        }
    } else {
        /* Slow path, table is empty, so we'll have to get the switch types by
         * actually inspecting the switch type entities. */
        for (i = 0; i < count; i ++) {
            ecs_entity_t e = array[i + table->sw_column_offset];
            ecs_assert(ECS_HAS_ROLE(e, SWITCH), ECS_INTERNAL_ERROR, NULL);
            e = e & ECS_COMPONENT_MASK;

            const EcsType *type_ptr = ecs_get(world, e, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

            if (ecs_type_has_id(
                world, type_ptr->normalized, add, true)) 
            {
                return i;
            }
        }
    }

    /* If a table was not found, this is an invalid switch case */
    ecs_abort(ECS_TYPE_INVALID_CASE, NULL);

    return -1;
}

static
ecs_table_t *find_or_create_table_include(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_entity_t add)
{
    /* If table has one or more switches and this is a case, return self */
    if (ECS_HAS_ROLE(add, CASE)) {
        ecs_assert((node->flags & EcsTableHasSwitch) != 0, 
            ECS_TYPE_INVALID_CASE, NULL);
        return node;
    } else {
        ecs_type_t type = node->type;
        int32_t count = ecs_vector_count(type);

        ecs_ids_t entities = {
            .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * (count + 1)),
            .count = count + 1
        };

        /* If table has a XOR column, check if the entity that is being added to
         * the table is part of the XOR type, and if it is, find the current 
         * entity in the table type matching the XOR type. This entity must be 
         * replaced in the new table, to ensure the XOR constraint isn't 
         * violated. */
        ecs_entity_t replace = find_xor_replace(world, node, type, add);

        add_entity_to_type(type, add, replace, &entities);

        ecs_table_t *result = flecs_table_find_or_create(world, &entities);
        
        if (result != node) {
            create_backlink_after_add(result, node, add);
        }

        return result;
    }
}

static
ecs_table_t *find_or_create_table_exclude(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_entity_t remove)
{
    ecs_type_t type = node->type;
    int32_t count = ecs_vector_count(type);

    ecs_ids_t entities = {
        .array = ecs_os_alloca(ECS_SIZEOF(ecs_entity_t) * count),
        .count = count
    };

    remove_entity_from_type(type, remove, &entities);

    ecs_table_t *result = flecs_table_find_or_create(world, &entities);
    if (!result) {
        return NULL;
    }

    if (result != node) {
        create_backlink_after_remove(result, node, remove);
    }

    return result;    
}

ecs_table_t* flecs_table_traverse_remove(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_ids_t * to_remove,
    ecs_ids_t * removed)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);
    
    int32_t i, count = to_remove->count;
    ecs_entity_t *entities = to_remove->array;
    node = node ? node : &world->store.root;

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        /* Removing 0 from an entity is not valid */
        ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_edge_t *edge = get_edge(node, e);
        ecs_table_t *next = edge->remove;

        if (!next) {
            if (edge->add == node) {
                /* Find table with all components of node except 'e' */
                next = find_or_create_table_exclude(world, node, e);
                if (!next) {
                    return NULL;
                }

                edge->remove = next;
            } else {
                /* If the add edge does not point to self, the table
                 * does not have the entity in to_remove. */
                continue;
            }
        }

        bool has_case = ECS_HAS_ROLE(e, CASE);
        if (removed && (node != next || has_case)) {
            removed->array[removed->count ++] = e; 
        }

        node = next;
    }    

    return node;
}

static
void find_owned_components(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_entity_t base,
    ecs_ids_t * owned)
{
    /* If we're adding an IsA relationship, check if the base
     * has OWNED components that need to be added to the instance */
    ecs_type_t t = ecs_get_type(world, base);

    int i, count = ecs_vector_count(t);
    ecs_entity_t *entities = ecs_vector_first(t, ecs_entity_t);
    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];
        if (ECS_HAS_RELATION(e, EcsIsA)) {
            find_owned_components(world, node, ECS_PAIR_OBJECT(e), owned);
        } else
        if (ECS_HAS_ROLE(e, OWNED)) {
            e = e & ECS_COMPONENT_MASK;
            
            /* If entity is a type, add each component in the type */
            const EcsType *t_ptr = ecs_get(world, e, EcsType);
            if (t_ptr) {
                ecs_type_t n = t_ptr->normalized;
                int32_t j, n_count = ecs_vector_count(n);
                ecs_entity_t *n_entities = ecs_vector_first(n, ecs_entity_t);
                for (j = 0; j < n_count; j ++) {
                    owned->array[owned->count ++] = n_entities[j];
                }
            } else {
                owned->array[owned->count ++] = ECS_PAIR_OBJECT(e);
            }
        }
    }
}

ecs_table_t* flecs_table_traverse_add(
    ecs_world_t * world,
    ecs_table_t * node,
    ecs_ids_t * to_add,
    ecs_ids_t * added)    
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    int32_t i, count = to_add->count;
    ecs_entity_t *entities = to_add->array;
    node = node ? node : &world->store.root;

    ecs_entity_t owned_array[ECS_MAX_ADD_REMOVE];
    ecs_ids_t owned = {
        .array = owned_array,
        .count = 0
    };    

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = entities[i];

        /* Adding 0 to an entity is not valid */
        ecs_assert(e != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_edge_t *edge = get_edge(node, e);
        ecs_table_t *next = edge->add;

        if (!next) {
            next = find_or_create_table_include(world, node, e);
            ecs_assert(next != NULL, ECS_INTERNAL_ERROR, NULL);
            edge->add = next;
        }

        bool has_case = ECS_HAS_ROLE(e, CASE);
        if (added && (node != next || has_case)) {
            added->array[added->count ++] = e; 
        }

        if ((node != next) && ECS_HAS_RELATION(e, EcsIsA)) {
            find_owned_components(
                world, next, ecs_pair_object(world, e), &owned);
        }        

        node = next;
    }

    /* In case OWNED components were found, add them as well */
    if (owned.count) {
        node = flecs_table_traverse_add(world, node, &owned, added);
    }

    return node;
}

static
bool ecs_entity_array_is_ordered(
    const ecs_ids_t *entities)
{
    ecs_entity_t prev = 0;
    ecs_entity_t *array = entities->array;
    int32_t i, count = entities->count;

    for (i = 0; i < count; i ++) {
        if (!array[i] && !prev) {
            continue;
        }
        if (array[i] <= prev) {
            return false;
        }
        prev = array[i];
    }    

    return true;
}

static
int32_t ecs_entity_array_dedup(
    ecs_entity_t *array,
    int32_t count)
{
    int32_t j, k;
    ecs_entity_t prev = array[0];

    for (k = j = 1; k < count; j ++, k++) {
        ecs_entity_t e = array[k];
        if (e == prev) {
            k ++;
        }

        array[j] = e;
        prev = e;
    }

    return count - (k - j);
}

#ifndef NDEBUG

static
int32_t count_occurrences(
    ecs_world_t * world,
    ecs_ids_t * entities,
    ecs_entity_t entity,
    int32_t constraint_index)    
{
    const EcsType *type_ptr = ecs_get(world, entity, EcsType);
    ecs_assert(type_ptr != NULL, 
        ECS_INVALID_PARAMETER, "flag must be applied to type");

    ecs_type_t type = type_ptr->normalized;
    int32_t count = 0;
    
    int i;
    for (i = 0; i < constraint_index; i ++) {
        ecs_entity_t e = entities->array[i];
        if (e & ECS_ROLE_MASK) {
            break;
        }

        if (ecs_type_has_id(world, type, e, false)) {
            count ++;
        }
    }

    return count;
}

static
void verify_constraints(
    ecs_world_t * world,
    ecs_ids_t * entities)
{
    int i, count = entities->count;
    for (i = count - 1; i >= 0; i --) {
        ecs_entity_t e = entities->array[i];
        ecs_entity_t mask = e & ECS_ROLE_MASK;
        if (!mask || 
            ((mask != ECS_OR) &&
             (mask != ECS_XOR) &&
             (mask != ECS_NOT)))
        {
            break;
        }

        ecs_entity_t entity = e & ECS_COMPONENT_MASK;
        int32_t matches = count_occurrences(world, entities, entity, i);
        switch(mask) {
        case ECS_OR:
            ecs_assert(matches >= 1, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);
            break;
        case ECS_XOR:
            ecs_assert(matches == 1, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);
            break;
        case ECS_NOT:
            ecs_assert(matches == 0, ECS_TYPE_CONSTRAINT_VIOLATION, NULL);    
            break;
        }
    }
}

#endif

static
ecs_table_t* find_or_create(
    ecs_world_t *world,
    const ecs_ids_t *ids)
{    
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   

    /* Make sure array is ordered and does not contain duplicates */
    int32_t type_count = ids->count;
    ecs_id_t *ordered = NULL;

    if (!type_count) {
        return &world->store.root;
    }

    if (!ecs_entity_array_is_ordered(ids)) {
        ecs_size_t size = ECS_SIZEOF(ecs_entity_t) * type_count;
        ordered = ecs_os_alloca(size);
        ecs_os_memcpy(ordered, ids->array, size);
        qsort(ordered, (size_t)type_count, sizeof(ecs_entity_t), 
            flecs_entity_compare_qsort);
        type_count = ecs_entity_array_dedup(ordered, type_count);        
    } else {
        ordered = ids->array;
    }

    ecs_ids_t ordered_ids = {
        .array = ordered,
        .count = type_count
    };

    ecs_table_t *table;
    flecs_hashmap_result_t elem = flecs_hashmap_ensure(
        world->store.table_map, &ordered_ids, ecs_table_t*);
    if ((table = *(ecs_table_t**)elem.value)) {
        return table;
    }

    /* If we get here, table needs to be created which is only allowed when the
     * application is not currently in progress */
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

#ifndef NDEBUG
    /* Check for constraint violations */
    verify_constraints(world, &ordered_ids);
#endif

    /* If we get here, the table has not been found, so create it. */
    ecs_table_t *result = create_table(world, &ordered_ids, elem);
    
    ecs_assert(ordered_ids.count == ecs_vector_count(result->type), 
        ECS_INTERNAL_ERROR, NULL);

    return result;
}

ecs_table_t* flecs_table_find_or_create(
    ecs_world_t * world,
    const ecs_ids_t * components)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   
    return find_or_create(world, components);
}

ecs_table_t* ecs_table_from_type(
    ecs_world_t *world,
    ecs_type_t type)
{
    ecs_ids_t components = flecs_type_to_ids(type);
    return flecs_table_find_or_create(
        world, &components);
}

void flecs_init_root_table(
    ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   

    ecs_ids_t entities = {
        .array = NULL,
        .count = 0
    };

    init_table(world, &world->store.root, &entities);
}

void flecs_table_clear_edges(
    ecs_world_t *world,
    ecs_table_t *table)
{
    (void)world;
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);   

    uint32_t i;

    if (table->lo_edges) {
        for (i = 0; i < ECS_HI_COMPONENT_ID; i ++) {
            ecs_edge_t *e = &table->lo_edges[i];
            ecs_table_t *add = e->add, *remove = e->remove;

            if (add) {
                add->lo_edges[i].remove = NULL;
            }
            if (remove) {
                remove->lo_edges[i].add = NULL;
            }
        }
    }

    ecs_map_iter_t it = ecs_map_iter(table->hi_edges);
    ecs_edge_t *edge;
    ecs_map_key_t component;
    while ((edge = ecs_map_next(&it, ecs_edge_t, &component))) {
        ecs_table_t *add = edge->add, *remove = edge->remove;
        if (add) {
            ecs_edge_t *e = get_edge(add, component);
            e->remove = NULL;
            if (!e->add) {
                ecs_map_remove(add->hi_edges, component);
            }
        }
        if (remove) {
            ecs_edge_t *e = get_edge(remove, component);
            e->add = NULL;
            if (!e->remove) {
                ecs_map_remove(remove->hi_edges, component);
            }
        }
    }
}

/* Public convenience functions for traversing table graph */
ecs_table_t* ecs_table_add_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_ids_t arr = { .array = &id, .count = 1 };
    return flecs_table_traverse_add(world, table, &arr, NULL);
}

ecs_table_t* ecs_table_remove_id(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_id_t id)
{
    ecs_ids_t arr = { .array = &id, .count = 1 };
    return flecs_table_traverse_remove(world, table, &arr, NULL);
}

/* The ratio used to determine whether the map should rehash. If
 * (element_count * LOAD_FACTOR) > bucket_count, bucket count is increased. */
#define LOAD_FACTOR (1.5f)
#define KEY_SIZE (ECS_SIZEOF(ecs_map_key_t))
#define GET_ELEM(array, elem_size, index) \
    ECS_OFFSET(array, (elem_size) * (index))

typedef struct ecs_bucket_t {
    ecs_map_key_t *keys;    /* Array with keys */
    void *payload;          /* Payload array */
    int32_t count;          /* Number of elements in bucket */
} ecs_bucket_t;

struct ecs_map_t {
    ecs_bucket_t *buckets;
    int32_t elem_size;
    int32_t bucket_count;
    int32_t count;
};

/* Get bucket count for number of elements */
static
int32_t get_bucket_count(
    int32_t element_count)
{
    return flecs_next_pow_of_2((int32_t)((float)element_count * LOAD_FACTOR));
}

/* Get bucket index for provided map key */
static
int32_t get_bucket_id(
    int32_t bucket_count,
    ecs_map_key_t key) 
{
    ecs_assert(bucket_count > 0, ECS_INTERNAL_ERROR, NULL);
    int32_t result = (int32_t)(key & ((uint64_t)bucket_count - 1));
    ecs_assert(result < INT32_MAX, ECS_INTERNAL_ERROR, NULL);
    return result;
}

/* Get bucket for key */
static
ecs_bucket_t* get_bucket(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    int32_t bucket_count = map->bucket_count;
    if (!bucket_count) {
        return NULL;
    }

    int32_t bucket_id = get_bucket_id(bucket_count, key);
    ecs_assert(bucket_id < bucket_count, ECS_INTERNAL_ERROR, NULL);

    return &map->buckets[bucket_id];
}

/* Ensure that map has at least new_count buckets */
static
void ensure_buckets(
    ecs_map_t *map,
    int32_t new_count)
{
    int32_t bucket_count = map->bucket_count;
    new_count = flecs_next_pow_of_2(new_count);
    if (new_count && new_count > bucket_count) {
        map->buckets = ecs_os_realloc(map->buckets, new_count * ECS_SIZEOF(ecs_bucket_t));
        map->bucket_count = new_count;

        ecs_os_memset(
            ECS_OFFSET(map->buckets, bucket_count * ECS_SIZEOF(ecs_bucket_t)), 
            0, (new_count - bucket_count) * ECS_SIZEOF(ecs_bucket_t));
    }
}

/* Free contents of bucket */
static
void clear_bucket(
    ecs_bucket_t *bucket)
{
    ecs_os_free(bucket->keys);
    ecs_os_free(bucket->payload);
    bucket->keys = NULL;
    bucket->payload = NULL;
    bucket->count = 0;
}

/* Clear all buckets */
static
void clear_buckets(
    ecs_map_t *map)
{
    ecs_bucket_t *buckets = map->buckets;
    int32_t i, count = map->bucket_count;
    for (i = 0; i < count; i ++) {
        clear_bucket(&buckets[i]);
    }
    ecs_os_free(buckets);
    map->buckets = NULL;
    map->bucket_count = 0;
}

/* Find or create bucket for specified key */
static
ecs_bucket_t* ensure_bucket(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    if (!map->bucket_count) {
        ensure_buckets(map, 2);
    }

    int32_t bucket_id = get_bucket_id(map->bucket_count, key);
    ecs_assert(bucket_id >= 0, ECS_INTERNAL_ERROR, NULL);
    return &map->buckets[bucket_id];
}

/* Add element to bucket */
static
int32_t add_to_bucket(
    ecs_bucket_t *bucket,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    const void *payload)
{
    int32_t index = bucket->count ++;
    int32_t bucket_count = index + 1;

    bucket->keys = ecs_os_realloc(bucket->keys, KEY_SIZE * bucket_count);
    bucket->payload = ecs_os_realloc(bucket->payload, elem_size * bucket_count);
    bucket->keys[index] = key;

    if (payload) {
        void *elem = GET_ELEM(bucket->payload, elem_size, index);
        ecs_os_memcpy(elem, payload, elem_size);
    }

    return index;
}

/*  Remove element from bucket */
static
void remove_from_bucket(
    ecs_bucket_t *bucket,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    int32_t index)
{
    (void)key;

    ecs_assert(bucket->count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(index < bucket->count, ECS_INTERNAL_ERROR, NULL);
    
    int32_t bucket_count = -- bucket->count;

    if (index != bucket->count) {
        ecs_assert(key == bucket->keys[index], ECS_INTERNAL_ERROR, NULL);
        bucket->keys[index] = bucket->keys[bucket_count];

        ecs_map_key_t *elem = GET_ELEM(bucket->payload, elem_size, index);
        ecs_map_key_t *last_elem = GET_ELEM(bucket->payload, elem_size, bucket->count);

        ecs_os_memcpy(elem, last_elem, elem_size);
    }
}

/* Get payload pointer for key from bucket */
static
void* get_from_bucket(
    ecs_bucket_t *bucket,
    ecs_map_key_t key,
    ecs_size_t elem_size)
{
    ecs_map_key_t *keys = bucket->keys;
    int32_t i, count = bucket->count;

    for (i = 0; i < count; i ++) {
        if (keys[i] == key) {
            return GET_ELEM(bucket->payload, elem_size, i);
        }
    }
    return NULL;
}

/* Grow number of buckets */
static
void rehash(
    ecs_map_t *map,
    int32_t bucket_count)
{
    ecs_assert(bucket_count != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(bucket_count > map->bucket_count, ECS_INTERNAL_ERROR, NULL);

    ecs_size_t elem_size = map->elem_size;

    ensure_buckets(map, bucket_count);

    ecs_bucket_t *buckets = map->buckets;
    ecs_assert(buckets != NULL, ECS_INTERNAL_ERROR, NULL);
    
    int32_t bucket_id;

    /* Iterate backwards as elements could otherwise be moved to existing
        * buckets which could temporarily cause the number of elements in a
        * bucket to exceed BUCKET_COUNT. */
    for (bucket_id = bucket_count - 1; bucket_id >= 0; bucket_id --) {
        ecs_bucket_t *bucket = &buckets[bucket_id];

        int i, count = bucket->count;
        ecs_map_key_t *key_array = bucket->keys;
        void *payload_array = bucket->payload;

        for (i = 0; i < count; i ++) {
            ecs_map_key_t key = key_array[i];
            void *elem = GET_ELEM(payload_array, elem_size, i);
            int32_t new_bucket_id = get_bucket_id(bucket_count, key);

            if (new_bucket_id != bucket_id) {
                ecs_bucket_t *new_bucket = &buckets[new_bucket_id];

                add_to_bucket(new_bucket, elem_size, key, elem);
                remove_from_bucket(bucket, elem_size, key, i);

                count --;
                i --;
            }
        }

        if (!bucket->count) {
            clear_bucket(bucket);
        }
    }
}

ecs_map_t* _ecs_map_new(
    ecs_size_t elem_size,
    ecs_size_t alignment, 
    int32_t element_count)
{
    (void)alignment;

    ecs_map_t *result = ecs_os_calloc(ECS_SIZEOF(ecs_map_t) * 1);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    int32_t bucket_count = get_bucket_count(element_count);

    result->count = 0;
    result->elem_size = elem_size;

    ensure_buckets(result, bucket_count);

    return result;
}

void ecs_map_free(
    ecs_map_t *map)
{
    if (map) {
        clear_buckets(map);
        ecs_os_free(map);
    }
}

void* _ecs_map_get(
    const ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    (void)elem_size;

    if (!map) {
        return NULL;
    }

    ecs_assert(elem_size == map->elem_size, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = get_bucket(map, key);
    if (!bucket) {
        return NULL;
    }

    return get_from_bucket(bucket, key, elem_size);
}

void* _ecs_map_get_ptr(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    void * ptr_ptr = _ecs_map_get(map, ECS_SIZEOF(void*), key);

    if (ptr_ptr) {
        return *(void**)ptr_ptr;
    } else {
        return NULL;
    }
}

bool ecs_map_has(
    const ecs_map_t *map,
    ecs_map_key_t key)
{
    if (!map) {
        return false;
    }

    ecs_bucket_t * bucket = get_bucket(map, key);
    if (!bucket) {
        return false;
    }

    return get_from_bucket(bucket, key, 0) != NULL;
}

void * _ecs_map_ensure(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key)
{
    void *result = _ecs_map_get(map, elem_size, key);
    if (!result) {
        result = _ecs_map_set(map, elem_size, key, NULL);
        ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_os_memset(result, 0, elem_size);
    }

    return result;
}

void* _ecs_map_set(
    ecs_map_t *map,
    ecs_size_t elem_size,
    ecs_map_key_t key,
    const void *payload)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(elem_size == map->elem_size, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t *bucket = ensure_bucket(map, key);
    ecs_assert(bucket != NULL, ECS_INTERNAL_ERROR, NULL);

    void *elem = get_from_bucket(bucket, key, elem_size);
    if (!elem) {
        int32_t index = add_to_bucket(bucket, elem_size, key, payload);
        
        int32_t map_count = ++map->count;
        int32_t target_bucket_count = get_bucket_count(map_count);
        int32_t map_bucket_count = map->bucket_count;

        if (target_bucket_count > map_bucket_count) {
            rehash(map, target_bucket_count);
            bucket = ensure_bucket(map, key);
            return get_from_bucket(bucket, key, elem_size);
        } else {
            return GET_ELEM(bucket->payload, elem_size, index);
        }       
    } else {
        if (payload) {
            ecs_os_memcpy(elem, payload, elem_size);
        }
        return elem;
    }
}

void ecs_map_remove(
    ecs_map_t *map,
    ecs_map_key_t key)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_bucket_t * bucket = get_bucket(map, key);
    if (!bucket) {
        return;
    }

    int32_t i, bucket_count = bucket->count;
    for (i = 0; i < bucket_count; i ++) {
        if (bucket->keys[i] == key) {
            remove_from_bucket(bucket, map->elem_size, key, i);
            map->count --;
        }
    } 
}

int32_t ecs_map_count(
    const ecs_map_t *map)
{
    return map ? map->count : 0;
}

int32_t ecs_map_bucket_count(
    const ecs_map_t *map)
{
    return map ? map->bucket_count : 0;
}

void ecs_map_clear(
    ecs_map_t *map)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    clear_buckets(map);
    map->count = 0;
}

ecs_map_iter_t ecs_map_iter(
    const ecs_map_t *map)
{
    return (ecs_map_iter_t){
        .map = map,
        .bucket = NULL,
        .bucket_index = 0,
        .element_index = 0
    };
}

void* _ecs_map_next(
    ecs_map_iter_t *iter,
    ecs_size_t elem_size,
    ecs_map_key_t *key_out)
{
    const ecs_map_t *map = iter->map;
    if (!map) {
        return NULL;
    }
    
    ecs_assert(!elem_size || elem_size == map->elem_size, ECS_INVALID_PARAMETER, NULL);
 
    ecs_bucket_t *bucket = iter->bucket;
    int32_t element_index = iter->element_index;
    elem_size = map->elem_size;

    do {
        if (!bucket) {
            int32_t bucket_index = iter->bucket_index;
            ecs_bucket_t *buckets = map->buckets;
            if (bucket_index < map->bucket_count) {
                bucket = &buckets[bucket_index];
                iter->bucket = bucket;

                element_index = 0;
                iter->element_index = 0;
            } else {
                return NULL;
            }
        }

        if (element_index < bucket->count) {
            iter->element_index = element_index + 1;
            break;
        } else {
            bucket = NULL;
            iter->bucket_index ++;
        }
    } while (true);
    
    if (key_out) {
        *key_out = bucket->keys[element_index];
    }

    return GET_ELEM(bucket->payload, elem_size, element_index);
}

void* _ecs_map_next_ptr(
    ecs_map_iter_t *iter,
    ecs_map_key_t *key_out)
{
    void *result = _ecs_map_next(iter, ECS_SIZEOF(void*), key_out);
    if (result) {
        return *(void**)result;
    } else {
        return NULL;
    }
}

void ecs_map_grow(
    ecs_map_t *map, 
    int32_t element_count)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t target_count = map->count + element_count;
    int32_t bucket_count = get_bucket_count(target_count);

    if (bucket_count > map->bucket_count) {
        rehash(map, bucket_count);
    }
}

void ecs_map_set_size(
    ecs_map_t *map, 
    int32_t element_count)
{    
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);
    int32_t bucket_count = get_bucket_count(element_count);

    if (bucket_count) {
        rehash(map, bucket_count);
    }
}

void ecs_map_memory(
    ecs_map_t *map, 
    int32_t *allocd,
    int32_t *used)
{
    ecs_assert(map != NULL, ECS_INVALID_PARAMETER, NULL);

    if (used) {
        *used = map->count * map->elem_size;
    }

    if (allocd) {
        *allocd += ECS_SIZEOF(ecs_map_t);

        int i, bucket_count = map->bucket_count;
        for (i = 0; i < bucket_count; i ++) {
            ecs_bucket_t *bucket = &map->buckets[i];
            *allocd += KEY_SIZE * bucket->count;
            *allocd += map->elem_size * bucket->count;
        }

        *allocd += ECS_SIZEOF(ecs_bucket_t) * bucket_count;
    }
}

#define INIT_CACHE(it, f, term_count)\
    if (!it->f && term_count) {\
        if (term_count < ECS_TERM_CACHE_SIZE) {\
            it->f = it->cache.f;\
            it->cache.f##_alloc = false;\
        } else {\
            it->f = ecs_os_malloc(ECS_SIZEOF(*(it->f)) * term_count);\
            it->cache.f##_alloc = true;\
        }\
    } else {\
        it->cache.f##_alloc = false;\
    }

#define FINI_CACHE(it, f)\
    if (it->f) {\
        if (it->cache.f##_alloc) {\
            ecs_os_free((void*)it->f);\
        }\
    }   

void ecs_iter_init(
    ecs_iter_t *it)
{
    INIT_CACHE(it, ids, it->column_count);
    INIT_CACHE(it, types, it->column_count);
    INIT_CACHE(it, columns, it->column_count);
    INIT_CACHE(it, subjects, it->column_count);
    INIT_CACHE(it, sizes, it->column_count);
    INIT_CACHE(it, ptrs, it->column_count);

    it->is_valid = true;
}

void ecs_iter_fini(
    ecs_iter_t *it)
{
    ecs_assert(it->is_valid == true, ECS_INVALID_PARAMETER, NULL);
    it->is_valid = false;

    FINI_CACHE(it, ids);
    FINI_CACHE(it, types);
    FINI_CACHE(it, columns);
    FINI_CACHE(it, subjects);
    FINI_CACHE(it, sizes);
    FINI_CACHE(it, ptrs);
}

/* --- Public API --- */

void* ecs_term_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!size || ecs_term_size(it, term) == size, 
        ECS_INVALID_PARAMETER, NULL);

    (void)size;

    if (!term) {
        return it->entities;
    }
    
    if (!it->ptrs) {
        return NULL;
    }

    return it->ptrs[term - 1];
}

bool ecs_term_is_owned(
    const ecs_iter_t *it,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    return it->subjects == NULL || it->subjects[term - 1] == 0;
}

bool ecs_term_is_readonly(
    const ecs_iter_t *it,
    int32_t term_index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);

    ecs_query_t *query = it->query;

    /* If this is not a query iterator, readonly is meaningless */
    ecs_assert(query != NULL, ECS_INVALID_OPERATION, NULL);
    (void)query;

    ecs_term_t *term = &it->query->filter.terms[term_index - 1];
    ecs_assert(term != NULL, ECS_INVALID_PARAMETER, NULL);
    
    if (term->inout == EcsIn) {
        return true;
    } else {
        ecs_term_id_t *subj = &term->args[0];

        if (term->inout == EcsInOutDefault) {
            if (subj->entity != EcsThis) {
                return true;
            }

            if ((subj->set.mask != EcsSelf) && 
                (subj->set.mask != EcsDefaultSet)) 
            {
                return true;
            }
        }
    }

    return false;
}

bool ecs_term_is_set(
    const ecs_iter_t *it,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->columns != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!term) {
        return true;
    }

    return it->columns[term - 1] != 0;
}

ecs_entity_t ecs_term_source(
    const ecs_iter_t *it,
    int32_t term)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(term > 0, ECS_INVALID_PARAMETER, NULL);
    
    if (!it->subjects) {
        return 0;
    } else {
        return it->subjects[term - 1];
    }
}

ecs_id_t ecs_term_id(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index > 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->ids != NULL, ECS_INTERNAL_ERROR, NULL);
    return it->ids[index - 1];
}

size_t ecs_term_size(
    const ecs_iter_t *it,
    int32_t index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index <= it->column_count, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(index >= 0, ECS_INVALID_PARAMETER, NULL);

    if (index == 0) {
        return sizeof(ecs_entity_t);
    }

    return flecs_to_size_t(it->sizes[index - 1]);
}

ecs_table_t* ecs_iter_table(
    const ecs_iter_t *it)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    return it->table;
}

ecs_type_t ecs_iter_type(
    const ecs_iter_t *it)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);

    /* If no table is set it means that the iterator isn't pointing to anything
     * yet. The most likely cause for this is that the operation is invoked on
     * a new iterator for which "next" hasn't been invoked yet, or on an
     * iterator that is out of elements. */
    ecs_table_t *table = ecs_iter_table(it);
    ecs_assert(table != NULL, ECS_INVALID_PARAMETER, NULL);
    return table->type;
}

int32_t ecs_iter_find_column(
    const ecs_iter_t *it,
    ecs_entity_t component)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    return ecs_type_index_of(it->table->type, 0, component);
}

void* ecs_iter_column_w_size(
    const ecs_iter_t *it,
    size_t size,
    int32_t column_index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    (void)size;
    
    ecs_table_t *table = it->table;
    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);
    
    if (table->column_count <= column_index) {
        return NULL;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    ecs_assert(!size || (ecs_size_t)size == column->size, ECS_INVALID_PARAMETER, NULL);

    return ecs_vector_first_t(column->data, column->size, column->alignment);
}

size_t ecs_iter_column_size(
    const ecs_iter_t *it,
    int32_t column_index)
{
    ecs_assert(it->is_valid, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(it->table != NULL, ECS_INVALID_PARAMETER, NULL);
    
    ecs_table_t *table = it->table;
    ecs_assert(column_index < ecs_vector_count(table->type), 
        ECS_INVALID_PARAMETER, NULL);

    if (table->column_count <= column_index) {
        return 0;
    }

    ecs_column_t *columns = it->table_columns;
    ecs_column_t *column = &columns[column_index];
    
    return flecs_to_size_t(column->size);
}

static
int32_t count_events(
    const ecs_entity_t *events) 
{
    int32_t i;

    for (i = 0; i < ECS_TRIGGER_DESC_EVENT_COUNT_MAX; i ++) {
        if (!events[i]) {
            break;
        }
    }

    return i;
} 

static
void register_id_trigger(
    ecs_map_t *set,
    ecs_trigger_t *trigger)
{
    ecs_trigger_t **t = ecs_map_ensure(set, ecs_trigger_t*, trigger->id);
    ecs_assert(t != NULL, ECS_INTERNAL_ERROR, NULL);
    *t = trigger;
}

static
ecs_map_t* unregister_id_trigger(
    ecs_map_t *set,
    ecs_trigger_t *trigger)
{
    ecs_map_remove(set, trigger->id);

    if (!ecs_map_count(set)) {
        ecs_map_free(set);
        return NULL;
    }

    return set;
}

static
void register_trigger(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(trigger != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_ensure(triggers, 
        ecs_id_trigger_t, trigger->term.id);
    ecs_assert(idt != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        ecs_map_t **set = NULL;
        if (trigger->events[i] == EcsOnAdd) {
            set = &idt->on_add_triggers;
        } else if (trigger->events[i] == EcsOnRemove) {
            set = &idt->on_remove_triggers;
        } else if (trigger->events[i] == EcsOnSet) {
            set = &idt->on_set_triggers;
        } else if (trigger->events[i] == EcsUnSet) {
            set = &idt->un_set_triggers;            
        } else {
            /* Invalid event provided */
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        ecs_assert(set != NULL, ECS_INTERNAL_ERROR, NULL);

        if (!*set) {
            *set = ecs_map_new(ecs_trigger_t*, 1);

            // First trigger of its kind, send table notification
            flecs_notify_tables(world, trigger->term.id, &(ecs_table_event_t){
                .kind = EcsTableTriggerMatch,
                .event = trigger->events[i]
            });            
        }

        register_id_trigger(*set, trigger);
    }
}

static
void unregister_trigger(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(trigger != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_get(
        triggers, ecs_id_trigger_t, trigger->term.id);
    if (!idt) {
        return;
    }

    int i;
    for (i = 0; i < trigger->event_count; i ++) {
        ecs_map_t **set = NULL;
        if (trigger->events[i] == EcsOnAdd) {
            set = &idt->on_add_triggers;
        } else if (trigger->events[i] == EcsOnRemove) {
            set = &idt->on_remove_triggers;
        } else if (trigger->events[i] == EcsOnSet) {
            set = &idt->on_set_triggers;
        } else if (trigger->events[i] == EcsUnSet) {
            set = &idt->un_set_triggers;            
        } else {
            /* Invalid event provided */
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }
        if (!*set) {
            return;
        }

        *set = unregister_id_trigger(*set, trigger);                
    }

    ecs_map_remove(triggers, trigger->id);
}

ecs_map_t* flecs_triggers_get(
    const ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(id != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_map_t *triggers = world->id_triggers;
    ecs_assert(triggers != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_id_trigger_t *idt = ecs_map_get(triggers, ecs_id_trigger_t, id);
    if (!idt) {
        return NULL;
    }

    ecs_map_t *set = NULL;

    if (event == EcsOnAdd) {
        set = idt->on_add_triggers;
    } else if (event == EcsOnRemove) {
        set = idt->on_remove_triggers;
    } else if (event == EcsOnSet) {
        set = idt->on_set_triggers;
    } else if (event == EcsUnSet) {
        set = idt->un_set_triggers;
    }

    if (ecs_map_count(set)) {
        return set;
    } else {
        return NULL;
    }
}

static
void notify_trigger_set(
    ecs_world_t *world,
    ecs_entity_t id,
    ecs_entity_t event,
    const ecs_map_t *triggers,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    if (!triggers) {
        return;
    }

    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t *entities = ecs_vector_first(data->entities, ecs_entity_t);        
    ecs_assert(entities != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(count > 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(row < ecs_vector_count(data->entities), ECS_INTERNAL_ERROR, NULL);
    ecs_assert((row + count) <= ecs_vector_count(data->entities), 
        ECS_INTERNAL_ERROR, NULL);
    entities = ECS_OFFSET(entities, ECS_SIZEOF(ecs_entity_t) * row);

    int32_t index = ecs_type_index_of(table->type, 0, id);
    ecs_assert(index >= 0, ECS_INTERNAL_ERROR, NULL);
    index ++;

    ecs_entity_t ids[1] = { id };
    int32_t columns[1] = { index };
    ecs_size_t sizes[1] = { 0 };

    /* If there is no data, ensure that system won't try to get it */
    if (table->column_count < index) {
        columns[0] = 0;
    } else {
        ecs_column_t *column = &data->columns[index - 1];
        if (!column->size) {
            columns[0] = 0;
        }
    }

    void *ptr = NULL;

    if (columns[0] && data && data->columns) {
        ecs_column_t *col = &data->columns[index - 1];
        ptr = ecs_vector_get_t(col->data, col->size, col->alignment, row);
        sizes[0] = col->size;
    }

    ecs_type_t types[1] = { ecs_type_from_id(world, id) };

    ecs_iter_t it = {
        .world = world,
        .event = event,
        .event_id = id,
        .table = table,
        .columns = columns,
        .ids = ids,
        .types = types,
        .sizes = sizes,
        .ptrs = &ptr,
        .table_count = 1,
        .inactive_table_count = 0,
        .column_count = 1,
        .table_columns = data->columns,
        .entities = entities,
        .offset = row,
        .count = count,
        .is_valid = true
    }; 

    ecs_map_iter_t mit = ecs_map_iter(triggers);
    ecs_trigger_t *t;
    while ((t = ecs_map_next_ptr(&mit, ecs_trigger_t*, NULL))) {
        it.system = t->entity;
        it.self = t->self;
        it.ctx = t->ctx;
        it.binding_ctx = t->binding_ctx;
        it.term_index = t->term.index;
        t->action(&it);                   
    }
}

void flecs_triggers_notify(
    ecs_world_t *world,
    ecs_id_t id,
    ecs_entity_t event,
    ecs_table_t *table,
    ecs_data_t *data,
    int32_t row,
    int32_t count)
{
    notify_trigger_set(world, id, event,
        flecs_triggers_get(world, id, event), 
            table, data, row, count);

    if (ECS_HAS_ROLE(id, PAIR)) {
        ecs_entity_t pred = ECS_PAIR_RELATION(id);
        ecs_entity_t obj = ECS_PAIR_OBJECT(id);

        notify_trigger_set(world, id, event,
            flecs_triggers_get(world, ecs_pair(pred, EcsWildcard), event), 
                table, data, row, count);

        notify_trigger_set(world, id, event, 
            flecs_triggers_get(world, ecs_pair(EcsWildcard, obj), event), 
                table, data, row, count);

        notify_trigger_set(world, id, event, 
            flecs_triggers_get(world, ecs_pair(EcsWildcard, EcsWildcard), event), 
                table, data, row, count);                                
    } else {
        notify_trigger_set(world, id, event, 
            flecs_triggers_get(world, EcsWildcard, event), 
                table, data, row, count);
    }
}

ecs_entity_t ecs_trigger_init(
    ecs_world_t *world,
    const ecs_trigger_desc_t *desc)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(desc != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_fini, ECS_INVALID_OPERATION, NULL);

    char *name = NULL;
    const char *expr = desc->expr;

    /* If entity is provided, create it */
    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t entity = ecs_entity_init(world, &desc->entity);

    bool added = false;
    EcsTrigger *comp = ecs_get_mut(world, entity, EcsTrigger, &added);
    if (added) {
        ecs_assert(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);
        
        /* Something went wrong with the construction of the entity */
        ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);
        name = ecs_get_fullpath(world, entity);

        ecs_term_t term;
        if (expr) {
    #ifdef FLECS_PARSER
            const char *ptr = ecs_parse_term(world, name, expr, expr, &term);
            if (!ptr) {
                goto error;
            }

            if (!ecs_term_is_initialized(&term)) {
                ecs_parser_error(
                    name, expr, 0, "invalid empty trigger expression");
                goto error;
            }

            if (ptr[0]) {
                ecs_parser_error(name, expr, 0, 
                    "too many terms in trigger expression (expected 1)");
                goto error;
            }
    #else
            ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
    #endif
        } else {
            term = ecs_term_copy(&desc->term);
        }

        if (ecs_term_finalize(world, name, expr, &term)) {
            goto error;
        }

        /* Currently triggers are not supported for specific entities */
        ecs_assert(term.args[0].entity == EcsThis, ECS_UNSUPPORTED, NULL);

        ecs_trigger_t *trigger = flecs_sparse_add(world->triggers, ecs_trigger_t);
        trigger->id = flecs_sparse_last_id(world->triggers);
        trigger->term = ecs_term_move(&term);
        trigger->action = desc->callback;
        trigger->ctx = desc->ctx;
        trigger->binding_ctx = desc->binding_ctx;
        trigger->ctx_free = desc->ctx_free;
        trigger->binding_ctx_free = desc->binding_ctx_free;
        trigger->event_count = count_events(desc->events);
        ecs_os_memcpy(trigger->events, desc->events, 
            trigger->event_count * ECS_SIZEOF(ecs_entity_t));
        trigger->entity = entity;
        trigger->self = desc->self;

        comp->trigger = trigger;

        /* Trigger must have at least one event */
        ecs_assert(trigger->event_count != 0, ECS_INVALID_PARAMETER, NULL);

        register_trigger(world, trigger);

        ecs_term_fini(&term);        
    } else {
        ecs_assert(comp->trigger != NULL, ECS_INTERNAL_ERROR, NULL);

        /* If existing entity handle was provided, override existing params */
        if (existing) {
            if (desc->callback) {
                ((ecs_trigger_t*)comp->trigger)->action = desc->callback;
            }
            if (desc->ctx) {
                ((ecs_trigger_t*)comp->trigger)->ctx = desc->ctx;
            }
            if (desc->binding_ctx) {
                ((ecs_trigger_t*)comp->trigger)->binding_ctx = desc->binding_ctx;
            }
        }
    }

    ecs_os_free(name);
    return entity;
error:
    ecs_os_free(name);
    return 0;
}

void* ecs_get_trigger_ctx(
    const ecs_world_t *world,
    ecs_entity_t trigger)
{
    const EcsTrigger *t = ecs_get(world, trigger, EcsTrigger);
    if (t) {
        return t->trigger->ctx;
    } else {
        return NULL;
    }     
}

void* ecs_get_trigger_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t trigger)
{
    const EcsTrigger *t = ecs_get(world, trigger, EcsTrigger);
    if (t) {
        return t->trigger->binding_ctx;
    } else {
        return NULL;
    }      
}

void flecs_trigger_fini(
    ecs_world_t *world,
    ecs_trigger_t *trigger)
{
    unregister_trigger(world, trigger);
    ecs_term_fini(&trigger->term);

    if (trigger->ctx_free) {
        trigger->ctx_free(trigger->ctx);
    }

    if (trigger->binding_ctx_free) {
        trigger->binding_ctx_free(trigger->binding_ctx);
    }

    flecs_sparse_remove(world->triggers, trigger->id);
}

int8_t flflecs_to_i8(
    int64_t v)
{
    ecs_assert(v < INT8_MAX, ECS_INTERNAL_ERROR, NULL);
    return (int8_t)v;
}

int16_t flecs_to_i16(
    int64_t v)
{
    ecs_assert(v < INT16_MAX, ECS_INTERNAL_ERROR, NULL);
    return (int16_t)v;
}

uint32_t flecs_to_u32(
    uint64_t v)
{
    ecs_assert(v < UINT32_MAX, ECS_INTERNAL_ERROR, NULL);
    return (uint32_t)v;    
}

size_t flecs_to_size_t(
    int64_t size)
{
    ecs_assert(size >= 0, ECS_INTERNAL_ERROR, NULL);
    return (size_t)size;
}

ecs_size_t flecs_from_size_t(
    size_t size)
{
   ecs_assert(size < INT32_MAX, ECS_INTERNAL_ERROR, NULL); 
   return (ecs_size_t)size;
}

int32_t flecs_next_pow_of_2(
    int32_t n)
{
    n --;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n ++;

    return n;
}

/** Convert time to double */
double ecs_time_to_double(
    ecs_time_t t)
{
    double result;
    result = t.sec;
    return result + (double)t.nanosec / (double)1000000000;
}

ecs_time_t ecs_time_sub(
    ecs_time_t t1,
    ecs_time_t t2)
{
    ecs_time_t result;

    if (t1.nanosec >= t2.nanosec) {
        result.nanosec = t1.nanosec - t2.nanosec;
        result.sec = t1.sec - t2.sec;
    } else {
        result.nanosec = t1.nanosec - t2.nanosec + 1000000000;
        result.sec = t1.sec - t2.sec - 1;
    }

    return result;
}

void ecs_sleepf(
    double t)
{
    if (t > 0) {
        int sec = (int)t;
        int nsec = (int)((t - sec) * 1000000000);
        ecs_os_sleep(sec, nsec);
    }
}

double ecs_time_measure(
    ecs_time_t *start)
{
    ecs_time_t stop, temp;
    ecs_os_get_time(&stop);
    temp = stop;
    stop = ecs_time_sub(stop, *start);
    *start = temp;
    return ecs_time_to_double(stop);
}

void* ecs_os_memdup(
    const void *src, 
    ecs_size_t size) 
{
    if (!src) {
        return NULL;
    }
    
    void *dst = ecs_os_malloc(size);
    ecs_assert(dst != NULL, ECS_OUT_OF_MEMORY, NULL);
    ecs_os_memcpy(dst, src, size);  
    return dst;  
}

int flecs_entity_compare(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2) 
{
    (void)ptr1;
    (void)ptr2;
    return (e1 > e2) - (e1 < e2);
}

int flecs_entity_compare_qsort(
    const void *e1,
    const void *e2)
{
    ecs_entity_t v1 = *(ecs_entity_t*)e1;
    ecs_entity_t v2 = *(ecs_entity_t*)e2;
    return flecs_entity_compare(v1, NULL, v2, NULL);
}

uint64_t flecs_string_hash(
    const void *ptr)
{
    const ecs_string_t *str = ptr;
    ecs_assert(str->hash != 0, ECS_INTERNAL_ERROR, NULL);
    return str->hash;
}

/*
    This code was taken from sokol_time.h 
    
    zlib/libpng license
    Copyright (c) 2018 Andre Weissflog
    This software is provided 'as-is', without any express or implied warranty.
    In no event will the authors be held liable for any damages arising from the
    use of this software.
    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:
        1. The origin of this software must not be misrepresented; you must not
        claim that you wrote the original software. If you use this software in a
        product, an acknowledgment in the product documentation would be
        appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not
        be misrepresented as being the original software.
        3. This notice may not be removed or altered from any source
        distribution.
*/


static int ecs_os_time_initialized;

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
static double _ecs_os_time_win_freq;
static LARGE_INTEGER _ecs_os_time_win_start;
#elif defined(__APPLE__) && defined(__MACH__)
#include <mach/mach_time.h>
static mach_timebase_info_data_t _ecs_os_time_osx_timebase;
static uint64_t _ecs_os_time_osx_start;
#else /* anything else, this will need more care for non-Linux platforms */
#include <time.h>
static uint64_t _ecs_os_time_posix_start;
#endif

/* prevent 64-bit overflow when computing relative timestamp
    see https://gist.github.com/jspohr/3dc4f00033d79ec5bdaf67bc46c813e3
*/
#if defined(_WIN32) || (defined(__APPLE__) && defined(__MACH__))
int64_t int64_muldiv(int64_t value, int64_t numer, int64_t denom) {
    int64_t q = value / denom;
    int64_t r = value % denom;
    return q * numer + r * numer / denom;
}
#endif

void flecs_os_time_setup(void) {
    if ( ecs_os_time_initialized) {
        return;
    }
    
    ecs_os_time_initialized = 1;
    #if defined(_WIN32)
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&_ecs_os_time_win_start);
        _ecs_os_time_win_freq = (double)freq.QuadPart / 1000000000.0;
    #elif defined(__APPLE__) && defined(__MACH__)
        mach_timebase_info(&_ecs_os_time_osx_timebase);
        _ecs_os_time_osx_start = mach_absolute_time();
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        _ecs_os_time_posix_start = (uint64_t)ts.tv_sec*1000000000 + (uint64_t)ts.tv_nsec; 
    #endif
}

uint64_t flecs_os_time_now(void) {
    ecs_assert(ecs_os_time_initialized != 0, ECS_INTERNAL_ERROR, NULL);

    uint64_t now;

    #if defined(_WIN32)
        LARGE_INTEGER qpc_t;
        QueryPerformanceCounter(&qpc_t);
        now = (uint64_t)(qpc_t.QuadPart / _ecs_os_time_win_freq);
    #elif defined(__APPLE__) && defined(__MACH__)
        now = (uint64_t) int64_muldiv((int64_t)mach_absolute_time(), (int64_t)_ecs_os_time_osx_timebase.numer, (int64_t)_ecs_os_time_osx_timebase.denom);
    #else
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        now = ((uint64_t)ts.tv_sec * 1000000000 + (uint64_t)ts.tv_nsec);
    #endif

    return now;
}

void flecs_os_time_sleep(
    int32_t sec, 
    int32_t nanosec) 
{
#ifndef _WIN32
    struct timespec sleepTime;
    ecs_assert(sec >= 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(nanosec >= 0, ECS_INTERNAL_ERROR, NULL);

    sleepTime.tv_sec = sec;
    sleepTime.tv_nsec = nanosec;
    if (nanosleep(&sleepTime, NULL)) {
        ecs_os_err("nanosleep failed");
    }
#else
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -((int64_t)sec * 10000000 + (int64_t)nanosec / 100);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
#endif
}


#if defined(_WIN32)

static ULONG win32_current_resolution;

void flecs_increase_timer_resolution(bool enable)
{
    HMODULE hntdll = GetModuleHandle((LPCTSTR)"ntdll.dll");
    if (!hntdll) {
        return;
    }

    LONG (__stdcall *pNtSetTimerResolution)(
        ULONG desired, BOOLEAN set, ULONG * current);

    pNtSetTimerResolution = (LONG(__stdcall*)(ULONG, BOOLEAN, ULONG*))
        GetProcAddress(hntdll, "NtSetTimerResolution");

    if(!pNtSetTimerResolution) {
        return;
    }

    ULONG current, resolution = 10000; /* 1 ms */

    if (!enable && win32_current_resolution) {
        pNtSetTimerResolution(win32_current_resolution, 0, &current);
        win32_current_resolution = 0;
        return;
    } else if (!enable) {
        return;
    }

    if (resolution == win32_current_resolution) {
        return;
    }

    if (win32_current_resolution) {
        pNtSetTimerResolution(win32_current_resolution, 0, &current);
    }

    if (pNtSetTimerResolution(resolution, 1, &current)) {
        /* Try setting a lower resolution */
        resolution *= 2;
        if(pNtSetTimerResolution(resolution, 1, &current)) return;
    }

    win32_current_resolution = resolution;
}

#else
void flecs_increase_timer_resolution(bool enable)
{
    (void)enable;
    return;
}
#endif

#ifdef FLECS_PIPELINE


/* Worker thread */
static
void* worker(void *arg) {
    ecs_stage_t *stage = arg;
    ecs_world_t *world = stage->world;

    /* Start worker thread, increase counter so main thread knows how many
     * workers are ready */
    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running ++;

    if (!world->quit_workers) {
        ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    }

    ecs_os_mutex_unlock(world->sync_mutex);

    while (!world->quit_workers) {
        ecs_entity_t old_scope = ecs_set_scope((ecs_world_t*)stage, 0);

        ecs_pipeline_run(
            (ecs_world_t*)stage, 
            world->pipeline, 
            world->stats.delta_time);

        ecs_set_scope((ecs_world_t*)stage, old_scope);
    }

    ecs_os_mutex_lock(world->sync_mutex);
    world->workers_running --;
    ecs_os_mutex_unlock(world->sync_mutex);

    return NULL;
}

/* Start threads */
static
void start_workers(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_set_stages(world, threads);

    ecs_assert(ecs_get_stage_count(world) == threads, ECS_INTERNAL_ERROR, NULL);

    int32_t i;
    for (i = 0; i < threads; i ++) {
        ecs_stage_t *stage = (ecs_stage_t*)ecs_get_stage(world, i);
        ecs_assert(stage != NULL, ECS_INTERNAL_ERROR, NULL);
        ecs_assert(stage->magic == ECS_STAGE_MAGIC, ECS_INTERNAL_ERROR, NULL);

        ecs_vector_get(world->worker_stages, ecs_stage_t, i);
        stage->thread = ecs_os_thread_new(worker, stage);
        ecs_assert(stage->thread != 0, ECS_THREAD_ERROR, NULL);
    }
}

/* Wait until all workers are running */
static
void wait_for_workers(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);
    bool wait = true;

    do {
        ecs_os_mutex_lock(world->sync_mutex);
        if (world->workers_running == stage_count) {
            wait = false;
        }
        ecs_os_mutex_unlock(world->sync_mutex);
    } while (wait);
}

/* Synchronize worker threads */
static
void sync_worker(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);

    /* Signal that thread is waiting */
    ecs_os_mutex_lock(world->sync_mutex);
    if (++ world->workers_waiting == stage_count) {
        /* Only signal main thread when all threads are waiting */
        ecs_os_cond_signal(world->sync_cond);
    }

    /* Wait until main thread signals that thread can continue */
    ecs_os_cond_wait(world->worker_cond, world->sync_mutex);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Wait until all threads are waiting on sync point */
static
void wait_for_sync(
    ecs_world_t *world)
{
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_os_mutex_lock(world->sync_mutex);
    if (world->workers_waiting != stage_count) {
        ecs_os_cond_wait(world->sync_cond, world->sync_mutex);
    }
    
    /* We should have been signalled unless all workers are waiting on sync */
    ecs_assert(world->workers_waiting == stage_count, 
        ECS_INTERNAL_ERROR, NULL);

    ecs_os_mutex_unlock(world->sync_mutex);
}

/* Signal workers that they can start/resume work */
static
void signal_workers(
    ecs_world_t *world)
{
    ecs_os_mutex_lock(world->sync_mutex);
    ecs_os_cond_broadcast(world->worker_cond);
    ecs_os_mutex_unlock(world->sync_mutex);
}

/** Stop worker threads */
static
bool ecs_stop_threads(
    ecs_world_t *world)
{
    bool threads_active = false;

    /* Test if threads are created. Cannot use workers_running, since this is
     * a potential race if threads haven't spun up yet. */
    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        if (stage->thread) {
            threads_active = true;
            break;
        }
        stage->thread = 0;
    });

    /* If no threads are active, just return */
    if (!threads_active) {
        return false;
    }

    /* Make sure all threads are running, to ensure they catch the signal */
    wait_for_workers(world);

    /* Signal threads should quit */
    world->quit_workers = true;
    signal_workers(world);

    /* Join all threads with main */
    ecs_vector_each(world->worker_stages, ecs_stage_t, stage, {
        ecs_os_thread_join(stage->thread);
        stage->thread = 0;
    });

    world->quit_workers = false;
    ecs_assert(world->workers_running == 0, ECS_INTERNAL_ERROR, NULL);

    /* Deinitialize stages */
    ecs_set_stages(world, 0);

    return true;
}

/* -- Private functions -- */

void ecs_worker_begin(
    ecs_world_t *world)
{
    flecs_stage_from_world(&world);
    int32_t stage_count = ecs_get_stage_count(world);
    ecs_assert(stage_count != 0, ECS_INTERNAL_ERROR, NULL);
    
    if (stage_count == 1) {
        ecs_staging_begin(world);
    }
}

bool ecs_worker_sync(
    ecs_world_t *world)
{
    flecs_stage_from_world(&world);

    int32_t build_count = world->stats.pipeline_build_count_total;
    int32_t stage_count = ecs_get_stage_count(world);
    ecs_assert(stage_count != 0, ECS_INTERNAL_ERROR, NULL);

    /* If there are no threads, merge in place */
    if (stage_count == 1) {
        ecs_staging_end(world);
        ecs_pipeline_update(world, world->pipeline, false);
        ecs_staging_begin(world);

    /* Synchronize all workers. The last worker to reach the sync point will
     * signal the main thread, which will perform the merge. */
    } else {
        sync_worker(world);
    }

    return world->stats.pipeline_build_count_total != build_count;
}

void ecs_worker_end(
    ecs_world_t *world)
{
    flecs_stage_from_world(&world);

    int32_t stage_count = ecs_get_stage_count(world);
    ecs_assert(stage_count != 0, ECS_INTERNAL_ERROR, NULL);

    /* If there are no threads, merge in place */
    if (stage_count == 1) {
        ecs_staging_end(world);

    /* Synchronize all workers. The last worker to reach the sync point will
     * signal the main thread, which will perform the merge. */
    } else {
        sync_worker(world);
    }
}

void ecs_workers_progress(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_time_t start = {0};
    if (world->measure_frame_time) {
        ecs_time_measure(&start);
    }

    if (stage_count == 1) {
        ecs_pipeline_update(world, pipeline, true);
        ecs_entity_t old_scope = ecs_set_scope(world, 0);
        ecs_world_t *stage = ecs_get_stage(world, 0);

        ecs_pipeline_run(stage, pipeline, delta_time);
        ecs_set_scope(world, old_scope);
    } else {
        int32_t i, sync_count = ecs_pipeline_update(world, pipeline, true);

        /* Make sure workers are running and ready */
        wait_for_workers(world);

        /* Synchronize n times for each op in the pipeline */
        for (i = 0; i < sync_count; i ++) {
            ecs_staging_begin(world);

            /* Signal workers that they should start running systems */
            world->workers_waiting = 0;
            signal_workers(world);

            /* Wait until all workers are waiting on sync point */
            wait_for_sync(world);

            /* Merge */
            ecs_staging_end(world);

            int32_t update_count;
            if ((update_count = ecs_pipeline_update(world, pipeline, false))) {
                /* The number of operations in the pipeline could have changed
                 * as result of the merge */
                sync_count = update_count;
            }
        }
    }

    if (world->measure_frame_time) {
        world->stats.system_time_total += (FLECS_FLOAT)ecs_time_measure(&start);
    }    
}


/* -- Public functions -- */

void ecs_set_threads(
    ecs_world_t *world,
    int32_t threads)
{
    ecs_assert(threads <= 1 || ecs_os_has_threading(), ECS_MISSING_OS_API, NULL);

    int32_t stage_count = ecs_get_stage_count(world);

    if (!world->arg_threads && stage_count != threads) {
        /* Stop existing threads */
        if (stage_count > 1) {
            if (ecs_stop_threads(world)) {
                ecs_os_cond_free(world->worker_cond);
                ecs_os_cond_free(world->sync_cond);
                ecs_os_mutex_free(world->sync_mutex);
            }
        }

        /* Start threads if number of threads > 1 */
        if (threads > 1) {
            world->worker_cond = ecs_os_cond_new();
            world->sync_cond = ecs_os_cond_new();
            world->sync_mutex = ecs_os_mutex_new();
            start_workers(world, threads);
        }
    }
}

#endif

#ifdef FLECS_PIPELINE


ECS_TYPE_DECL(EcsPipelineQuery);

static ECS_CTOR(EcsPipelineQuery, ptr, {
    memset(ptr, 0, _size);
})

static ECS_DTOR(EcsPipelineQuery, ptr, {
    ecs_vector_free(ptr->ops);
})

static
int compare_entity(
    ecs_entity_t e1, 
    const void *ptr1, 
    ecs_entity_t e2, 
    const void *ptr2) 
{
    (void)ptr1;
    (void)ptr2;
    return (e1 > e2) - (e1 < e2);
}

static
int group_by_phase(
    ecs_world_t *world,
    ecs_type_t type,
    ecs_entity_t pipeline,
    void *ctx) 
{
    (void)ctx;
    
    const EcsType *pipeline_type = ecs_get(world, pipeline, EcsType);
    ecs_assert(pipeline_type != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Find tag in system that belongs to pipeline */
    ecs_entity_t *sys_comps = ecs_vector_first(type, ecs_entity_t);
    int32_t c, t, count = ecs_vector_count(type);

    ecs_entity_t *tags = ecs_vector_first(pipeline_type->normalized, ecs_entity_t);
    int32_t tag_count = ecs_vector_count(pipeline_type->normalized);

    ecs_entity_t result = 0;

    for (c = 0; c < count; c ++) {
        ecs_entity_t comp = sys_comps[c];
        for (t = 0; t < tag_count; t ++) {
            if (comp == tags[t]) {
                result = comp;
                break;
            }
        }
        if (result) {
            break;
        }
    }

    ecs_assert(result != 0, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(result < INT_MAX, ECS_INTERNAL_ERROR, NULL);

    return (int)result;
}

typedef enum ComponentWriteState {
    NotWritten = 0,
    WriteToMain,
    WriteToStage
} ComponentWriteState;

typedef struct write_state_t {
    ecs_map_t *components;
    bool wildcard;
} write_state_t;

static
int32_t get_write_state(
    ecs_map_t *write_state,
    ecs_entity_t component)
{
    int32_t *ptr = ecs_map_get(write_state, int32_t, component);
    if (ptr) {
        return *ptr;
    } else {
        return 0;
    }
}

static
void set_write_state(
    write_state_t *write_state,
    ecs_entity_t component,
    int32_t value)
{
    if (component == EcsWildcard) {
        ecs_assert(value == WriteToStage, ECS_INTERNAL_ERROR, NULL);
        write_state->wildcard = true;
    } else {
        ecs_map_set(write_state->components, component, &value);
    }
}

static
void reset_write_state(
    write_state_t *write_state)
{
    ecs_map_clear(write_state->components);
    write_state->wildcard = false;
}

static
int32_t get_any_write_state(
    write_state_t *write_state)
{
    if (write_state->wildcard) {
        return WriteToStage;
    }

    ecs_map_iter_t it = ecs_map_iter(write_state->components);
    int32_t *elem;
    while ((elem = ecs_map_next(&it, int32_t, NULL))) {
        if (*elem == WriteToStage) {
            return WriteToStage;
        }
    }

    return 0;
}

static
bool check_term_component(
    ecs_term_t *term,
    bool is_active,
    ecs_entity_t component,
    write_state_t *write_state)    
{
    int32_t state = get_write_state(write_state->components, component);

    ecs_term_id_t *subj = &term->args[0];

    if ((subj->set.mask & EcsSelf) && subj->entity == EcsThis && term->oper != EcsNot) {
        switch(term->inout) {
        case EcsInOutDefault:
        case EcsInOut:
        case EcsIn:
            if (state == WriteToStage || write_state->wildcard) {
                return true;
            }
            // fall through
        case EcsOut:
            if (is_active && term->inout != EcsIn) {
                set_write_state(write_state, component, WriteToMain);
            }
        };
    } else if (!subj->entity || term->oper == EcsNot) {
        bool needs_merge = false;

        switch(term->inout) {
        case EcsInOutDefault:
        case EcsIn:
        case EcsInOut:
            if (state == WriteToStage) {
                needs_merge = true;
            }
            if (component == EcsWildcard) {
                if (get_any_write_state(write_state) == WriteToStage) {
                    needs_merge = true;
                }
            }
            break;
        default:
            break;
        };

        switch(term->inout) {
        case EcsInOutDefault:
            if (!(subj->set.mask & EcsSelf) || !subj->entity || 
                subj->entity != EcsThis) 
            {
                break;
            }
            // fall through
        case EcsInOut:
        case EcsOut:
            if (is_active) {
                set_write_state(write_state, component, WriteToStage);
            }
            break;
        default:
            break;
        };   

        if (needs_merge) {
            return true;
        }
    }

    return false;
}

static
bool check_term(
    ecs_term_t *term,
    bool is_active,
    write_state_t *write_state)
{
    if (term->oper != EcsOr) {
        return check_term_component(
            term, is_active, term->id, write_state);
    }  

    return false;
}

static
bool build_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    EcsPipelineQuery *pq)
{
    (void)pipeline;

    ecs_query_iter(pq->query);

    if (pq->match_count == pq->query->match_count) {
        /* No need to rebuild the pipeline */
        return false;
    }

    ecs_trace_2("rebuilding pipeline #[green]%s", 
        ecs_get_name(world, pipeline));

    world->stats.pipeline_build_count_total ++;

    write_state_t ws = {
        .components = ecs_map_new(int32_t, ECS_HI_COMPONENT_ID),
        .wildcard = false
    };

    ecs_pipeline_op_t *op = NULL;
    ecs_vector_t *ops = NULL;
    ecs_query_t *query = pq->build_query;

    if (pq->ops) {
        ecs_vector_free(pq->ops);
    }

    /* Iterate systems in pipeline, add ops for running / merging */
    ecs_iter_t it = ecs_query_iter(query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int i;
        for (i = 0; i < it.count; i ++) {      
            ecs_query_t *q = sys[i].query;
            if (!q) {
                continue;
            }

            bool needs_merge = false;
            bool is_active = !ecs_has_id(
                world, it.entities[i], EcsInactive);
            
            ecs_term_t *terms = q->filter.terms;
            int32_t t, term_count = q->filter.term_count;
            for (t = 0; t < term_count; t ++) {
                needs_merge |= check_term(&terms[t], is_active, &ws);
            }

            if (needs_merge) {
                /* After merge all components will be merged, so reset state */
                reset_write_state(&ws);
                op = NULL;

                /* Re-evaluate columns to set write flags if system is active.
                 * If system is inactive, it can't write anything and so it
                 * should not insert unnecessary merges.  */
                needs_merge = false;
                if (is_active) {
                    for (t = 0; t < term_count; t ++) {
                        needs_merge |= check_term(&terms[t], true, &ws);
                    }
                }

                /* The component states were just reset, so if we conclude that
                 * another merge is needed something is wrong. */
                ecs_assert(needs_merge == false, ECS_INTERNAL_ERROR, NULL);        
            }

            if (!op) {
                op = ecs_vector_add(&ops, ecs_pipeline_op_t);
                op->count = 0;
            }

            /* Don't increase count for inactive systems, as they are ignored by
             * the query used to run the pipeline. */
            if (is_active) {
                op->count ++;
            }
        }
    }

    ecs_map_free(ws.components);

    /* Force sort of query as this could increase the match_count */
    pq->match_count = pq->query->match_count;
    pq->ops = ops;

    return true;
}

static
int32_t iter_reset(
    const EcsPipelineQuery *pq,
    ecs_iter_t *iter_out,
    ecs_pipeline_op_t **op_out,
    ecs_entity_t move_to)
{
    ecs_pipeline_op_t *op = ecs_vector_first(pq->ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    *iter_out = ecs_query_iter(pq->query);
    while (ecs_query_next(iter_out)) {
        int32_t i;
        for(i = 0; i < iter_out->count; i ++) {
            ecs_entity_t e = iter_out->entities[i];

            ran_since_merge ++;
            if (ran_since_merge == op->count) {
                ran_since_merge = 0;
                op ++;
            }

            if (e == move_to) {
                *op_out = op;
                return i;
            }
        }
    }

    ecs_abort(ECS_UNSUPPORTED, NULL);

    return -1;
}

int32_t ecs_pipeline_update(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    bool start_of_frame)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);
    ecs_assert(pipeline != 0, ECS_INTERNAL_ERROR, NULL);

    /* If any entity mutations happened that could have affected query matching
     * notify appropriate queries so caches are up to date. This includes the
     * pipeline query. */
    if (start_of_frame) {
        flecs_eval_component_monitors(world);
    }

    bool added = false;
    EcsPipelineQuery *pq = ecs_get_mut(world, pipeline, EcsPipelineQuery, &added);
    ecs_assert(added == false, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    build_pipeline(world, pipeline, pq);

    return ecs_vector_count(pq->ops);
}

void ecs_pipeline_run(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    FLECS_FLOAT delta_time)
{
    ecs_assert(world != NULL, ECS_INVALID_OPERATION, NULL);

    if (!pipeline) {
        pipeline = world->pipeline;
    }    

    /* If the world is passed to ecs_pipeline_run, the function will take care
     * of staging, so the world should not be in staged mode when called. */
    if (world->magic == ECS_WORLD_MAGIC) {
        ecs_assert(!world->is_readonly, ECS_INVALID_OPERATION, NULL);

        /* Forward to worker_progress. This function handles staging, threading
         * and synchronization across workers. */
        ecs_workers_progress(world, pipeline, delta_time);
        return;

    /* If a stage is passed, the function could be ran from a worker thread. In
     * that case the main thread should manage staging, and staging should be
     * enabled. */
    } else {
        ecs_assert(world->magic == ECS_STAGE_MAGIC, ECS_INVALID_PARAMETER, NULL);
    }

    ecs_stage_t *stage = flecs_stage_from_world(&world);  
    
    const EcsPipelineQuery *pq = ecs_get(world, pipeline, EcsPipelineQuery);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(pq->query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_vector_t *ops = pq->ops;
    ecs_pipeline_op_t *op = ecs_vector_first(ops, ecs_pipeline_op_t);
    ecs_pipeline_op_t *op_last = ecs_vector_last(ops, ecs_pipeline_op_t);
    int32_t ran_since_merge = 0;

    int32_t stage_index = ecs_get_stage_id(stage->thread_ctx);
    int32_t stage_count = ecs_get_stage_count(world);

    ecs_worker_begin(stage->thread_ctx);
    
    ecs_iter_t it = ecs_query_iter(pq->query);
    while (ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int32_t i;
        for(i = 0; i < it.count; i ++) {
            ecs_entity_t e = it.entities[i];

            ecs_run_intern(world, stage, e, &sys[i], stage_index, stage_count, 
                delta_time, 0, 0, NULL, NULL);

            ran_since_merge ++;
            world->stats.systems_ran_frame ++;

            if (op != op_last && ran_since_merge == op->count) {
                ran_since_merge = 0;
                op++;

                /* If the set of matched systems changed as a result of the
                 * merge, we have to reset the iterator and move it to our
                 * current position (system). If there are a lot of systems
                 * in the pipeline this can be an expensive operation, but
                 * should happen infrequently. */
                if (ecs_worker_sync(stage->thread_ctx)) {
                    i = iter_reset(pq, &it, &op, e);
                    op_last = ecs_vector_last(pq->ops, ecs_pipeline_op_t);
                    sys = ecs_term(&it, EcsSystem, 1);
                }
            }
        }
    }

    ecs_worker_end(stage->thread_ctx);
}

static
void add_pipeline_tags_to_sig(
    ecs_world_t *world,
    ecs_term_t *terms,
    ecs_type_t type)
{
    (void)world;
    
    int32_t i, count = ecs_vector_count(type);
    ecs_entity_t *entities = ecs_vector_first(type, ecs_entity_t);

    for (i = 0; i < count; i ++) {
        terms[i] = (ecs_term_t){
            .inout = EcsIn,
            .oper = EcsOr,
            .pred.entity = entities[i],
            .args[0] = {
                .entity = EcsThis,
                .set.mask = EcsSelf | EcsSuperSet
            }
        };
    }
}

static
ecs_query_t* build_pipeline_query(
    ecs_world_t *world,
    ecs_entity_t pipeline,
    const char *name,
    bool with_inactive)
{
    const EcsType *type_ptr = ecs_get(world, pipeline, EcsType);
    ecs_assert(type_ptr != NULL, ECS_INTERNAL_ERROR, NULL);

    int32_t type_count = ecs_vector_count(type_ptr->normalized);
    int32_t term_count = 2;

    if (with_inactive) {
        term_count ++;
    }

    ecs_term_t *terms = ecs_os_malloc(
        (type_count + term_count) * ECS_SIZEOF(ecs_term_t));

    terms[0] = (ecs_term_t){
        .inout = EcsIn,
        .oper = EcsAnd,
        .pred.entity = ecs_id(EcsSystem),
        .args[0] = {
            .entity = EcsThis,
            .set.mask = EcsSelf | EcsSuperSet
        }
    };

    terms[1] = (ecs_term_t){
        .inout = EcsIn,
        .oper = EcsNot,
        .pred.entity = EcsDisabledIntern,
        .args[0] = {
            .entity = EcsThis,
            .set.mask = EcsSelf | EcsSuperSet
        }
    };

    if (with_inactive) {
        terms[2] = (ecs_term_t){
            .inout = EcsIn,
            .oper = EcsNot,
            .pred.entity = EcsInactive,
            .args[0] = {
                .entity = EcsThis,
                .set.mask = EcsSelf | EcsSuperSet
            }
        };
    }

    add_pipeline_tags_to_sig(world, &terms[term_count], type_ptr->normalized);    

    ecs_query_t *result = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .name = name,
            .terms_buffer = terms,
            .terms_buffer_count = term_count + type_count
        },
        .order_by = compare_entity,
        .group_by = group_by_phase,
        .group_by_id = pipeline
    });

    ecs_assert(result != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_os_free(terms);

    return result;
}

static 
void EcsOnUpdatePipeline(
    ecs_iter_t *it)
{
    ecs_world_t *world = it->world;
    ecs_entity_t *entities = it->entities;

    int32_t i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_entity_t pipeline = entities[i];
        
#ifndef NDEBUG
        ecs_trace_1("pipeline #[green]%s#[normal] created",
            ecs_get_name(world, pipeline));
#endif
        ecs_log_push();

        /* Build signature for pipeline quey that matches EcsSystems, has the
         * pipeline phases as OR columns, and ignores systems with EcsInactive 
         * and EcsDisabledIntern. Note that EcsDisabled is automatically ignored 
         * by the regular query matching */
        ecs_query_t *query = build_pipeline_query(
            world, pipeline, "BuiltinPipelineQuery", true);
        ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

        /* Build signature for pipeline build query. The build query includes
         * systems that are inactive, as an inactive system may become active as
         * a result of another system, and as a result the correct merge 
         * operations need to be put in place. */
        ecs_query_t *build_query = build_pipeline_query(
            world, pipeline, "BuiltinPipelineBuildQuery", false);
        ecs_assert(build_query != NULL, ECS_INTERNAL_ERROR, NULL);

        bool added = false;
        EcsPipelineQuery *pq = ecs_get_mut(
            world, pipeline, EcsPipelineQuery, &added);
        ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

        if (added) {
            /* Should not modify pipeline after it has been used */
            ecs_assert(pq->ops == NULL, ECS_INVALID_OPERATION, NULL);

            if (pq->query) {
                ecs_query_fini(pq->query);
            }
            if (pq->build_query) {
                ecs_query_fini(pq->build_query);
            }
        }

        pq->query = query;
        pq->build_query = build_query;
        pq->match_count = -1;
        pq->ops = NULL;

        ecs_log_pop();
    }
}

/* -- Public API -- */

bool ecs_progress(
    ecs_world_t *world,
    FLECS_FLOAT user_delta_time)
{
    float delta_time = ecs_frame_begin(world, user_delta_time);

    ecs_pipeline_run(world, 0, delta_time);

    ecs_frame_end(world);

    return !world->should_quit;
}

void ecs_set_time_scale(
    ecs_world_t *world,
    FLECS_FLOAT scale)
{
    world->stats.time_scale = scale;
}

void ecs_reset_clock(
    ecs_world_t *world)
{
    world->stats.world_time_total = 0;
    world->stats.world_time_total_raw = 0;
}

void ecs_deactivate_systems(
    ecs_world_t *world)
{
    ecs_assert(!world->is_readonly, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t pipeline = world->pipeline;
    const EcsPipelineQuery *pq = ecs_get( world, pipeline, EcsPipelineQuery);
    ecs_assert(pq != NULL, ECS_INTERNAL_ERROR, NULL);

    /* Iterate over all systems, add EcsInvalid tag if queries aren't matched
     * with any tables */
    ecs_iter_t it = ecs_query_iter(pq->build_query);

    /* Make sure that we defer adding the inactive tags until after iterating
     * the query */
    flecs_defer_none(world, &world->stage);

    while( ecs_query_next(&it)) {
        EcsSystem *sys = ecs_term(&it, EcsSystem, 1);

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            ecs_query_t *query = sys[i].query;
            if (query) {
                if (!ecs_vector_count(query->tables)) {
                    ecs_add_id(world, it.entities[i], EcsInactive);
                }
            }
        }
    }

    flecs_defer_flush(world, &world->stage);
}

void ecs_set_pipeline(
    ecs_world_t *world,
    ecs_entity_t pipeline)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);
    ecs_assert( ecs_get(world, pipeline, EcsPipelineQuery) != NULL, 
        ECS_INVALID_PARAMETER, NULL);

    world->pipeline = pipeline;
}

ecs_entity_t ecs_get_pipeline(
    const ecs_world_t *world)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    world = ecs_get_world(world);
    return world->pipeline;
}

/* -- Module implementation -- */

static
void FlecsPipelineFini(
    ecs_world_t *world,
    void *ctx)
{
    (void)ctx;
    if (ecs_get_stage_count(world)) {
        ecs_set_threads(world, 0);
    }
}

void FlecsPipelineImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsPipeline);

    ECS_IMPORT(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_tag(world, EcsPipeline);
    flecs_bootstrap_component(world, EcsPipelineQuery);

    /* Phases of the builtin pipeline are regular entities. Names are set so
     * they can be resolved by type expressions. */
    flecs_bootstrap_tag(world, EcsPreFrame);
    flecs_bootstrap_tag(world, EcsOnLoad);
    flecs_bootstrap_tag(world, EcsPostLoad);
    flecs_bootstrap_tag(world, EcsPreUpdate);
    flecs_bootstrap_tag(world, EcsOnUpdate);
    flecs_bootstrap_tag(world, EcsOnValidate);
    flecs_bootstrap_tag(world, EcsPostUpdate);
    flecs_bootstrap_tag(world, EcsPreStore);
    flecs_bootstrap_tag(world, EcsOnStore);
    flecs_bootstrap_tag(world, EcsPostFrame);

    ECS_TYPE_IMPL(EcsPipelineQuery);

    /* Set ctor and dtor for PipelineQuery */
    ecs_set(world, ecs_id(EcsPipelineQuery), EcsComponentLifecycle, {
        .ctor = ecs_ctor(EcsPipelineQuery),
        .dtor = ecs_dtor(EcsPipelineQuery)
    });

    /* When the Pipeline tag is added a pipeline will be created */
    ECS_SYSTEM(world, EcsOnUpdatePipeline, EcsOnSet, Pipeline, Type);

    /* Create the builtin pipeline */
    world->pipeline = ecs_type_init(world, &(ecs_type_desc_t){
        .entity = {
            .name = "BuiltinPipeline",
            .add = {EcsPipeline}
        },
        .ids = {
            EcsPreFrame, EcsOnLoad, EcsPostLoad, EcsPreUpdate, EcsOnUpdate,
            EcsOnValidate, EcsPostUpdate, EcsPreStore, EcsOnStore, EcsPostFrame
         }
    });

    /* Cleanup thread administration when world is destroyed */
    ecs_atfini(world, FlecsPipelineFini, NULL);
}

#endif

#ifdef FLECS_TIMER


ecs_type_t ecs_type(EcsTimer);
ecs_type_t ecs_type(EcsRateFilter);

static
void AddTickSource(ecs_iter_t *it) {
    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], EcsTickSource, {0});
    }
}

static
void ProgressTimers(ecs_iter_t *it) {
    EcsTimer *timer = ecs_term(it, EcsTimer, 1);
    EcsTickSource *tick_source = ecs_term(it, EcsTickSource, 2);

    ecs_assert(timer != NULL, ECS_INTERNAL_ERROR, NULL);

    int i;
    for (i = 0; i < it->count; i ++) {
        tick_source[i].tick = false;

        if (!timer[i].active) {
            continue;
        }

        const ecs_world_info_t *info = ecs_get_world_info(it->world);
        FLECS_FLOAT time_elapsed = timer[i].time + info->delta_time_raw;
        FLECS_FLOAT timeout = timer[i].timeout;
        
        if (time_elapsed >= timeout) {
            FLECS_FLOAT t = time_elapsed - timeout;
            if (t > timeout) {
                t = 0;
            }

            timer[i].time = t; /* Initialize with remainder */            
            tick_source[i].tick = true;
            tick_source[i].time_elapsed = time_elapsed;

            if (timer[i].single_shot) {
                timer[i].active = false;
            }
        } else {
            timer[i].time = time_elapsed;
        }  
    }
}

static
void ProgressRateFilters(ecs_iter_t *it) {
    EcsRateFilter *filter = ecs_term(it, EcsRateFilter, 1);
    EcsTickSource *tick_dst = ecs_term(it, EcsTickSource, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t src = filter[i].src;
        bool inc = false;

        filter[i].time_elapsed += it->delta_time;

        if (src) {
            const EcsTickSource *tick_src = ecs_get(it->world, src, EcsTickSource);
            if (tick_src) {
                inc = tick_src->tick;
            } else {
                inc = true;
            }
        } else {
            inc = true;
        }

        if (inc) {
            filter[i].tick_count ++;
            bool triggered = !(filter[i].tick_count % filter[i].rate);
            tick_dst[i].tick = triggered;
            tick_dst[i].time_elapsed = filter[i].time_elapsed;

            if (triggered) {
                filter[i].time_elapsed = 0;
            }            
        } else {
            tick_dst[i].tick = false;
        }
    }
}

static
void ProgressTickSource(ecs_iter_t *it) {
    EcsTickSource *tick_src = ecs_term(it, EcsTickSource, 1);

    /* If tick source has no filters, tick unconditionally */
    int i;
    for (i = 0; i < it->count; i ++) {
        tick_src[i].tick = true;
        tick_src[i].time_elapsed = it->delta_time;
    }
}

ecs_entity_t ecs_set_timeout(
    ecs_world_t *world,
    ecs_entity_t timer,
    FLECS_FLOAT timeout)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    timer = ecs_set(world, timer, EcsTimer, {
        .timeout = timeout,
        .single_shot = true,
        .active = true
    });

    EcsSystem *system_data = ecs_get_mut(world, timer, EcsSystem, NULL);
    if (system_data) {
        system_data->tick_source = timer;
    }

    return timer;
}

FLECS_FLOAT ecs_get_timeout(
    const ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(timer != 0, ECS_INVALID_PARAMETER, NULL);

    const EcsTimer *value = ecs_get(world, timer, EcsTimer);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

ecs_entity_t ecs_set_interval(
    ecs_world_t *world,
    ecs_entity_t timer,
    FLECS_FLOAT interval)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    timer = ecs_set(world, timer, EcsTimer, {
        .timeout = interval,
        .active = true
    });

    EcsSystem *system_data = ecs_get_mut(world, timer, EcsSystem, NULL);
    if (system_data) {
        system_data->tick_source = timer;
    }  

    return timer;  
}

FLECS_FLOAT ecs_get_interval(
    const ecs_world_t *world,
    ecs_entity_t timer)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    if (!timer) {
        return 0;
    }

    const EcsTimer *value = ecs_get(world, timer, EcsTimer);
    if (value) {
        return value->timeout;
    } else {
        return 0;
    }
}

void ecs_start_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_mut(world, timer, EcsTimer, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = true;
    ptr->time = 0;
}

void ecs_stop_timer(
    ecs_world_t *world,
    ecs_entity_t timer)
{
    EcsTimer *ptr = ecs_get_mut(world, timer, EcsTimer, NULL);
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, NULL);

    ptr->active = false;
}

ecs_entity_t ecs_set_rate(
    ecs_world_t *world,
    ecs_entity_t filter,
    int32_t rate,
    ecs_entity_t source)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);

    filter = ecs_set(world, filter, EcsRateFilter, {
        .rate = rate,
        .src = source
    });

    EcsSystem *system_data = ecs_get_mut(world, filter, EcsSystem, NULL);
    if (system_data) {
        system_data->tick_source = filter;
    }  

    return filter;     
}

/* Deprecated */
ecs_entity_t ecs_set_rate_filter(
    ecs_world_t *world,
    ecs_entity_t filter,
    int32_t rate,
    ecs_entity_t source)
{
    return ecs_set_rate(world, filter, rate, source);
}

void ecs_set_tick_source(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_entity_t tick_source)
{
    ecs_assert(world != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(system != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(tick_source != 0, ECS_INVALID_PARAMETER, NULL);

    EcsSystem *system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    ecs_assert(system_data != NULL, ECS_INVALID_PARAMETER, NULL);

    system_data->tick_source = tick_source;
}

void FlecsTimerImport(
    ecs_world_t *world)
{    
    ECS_MODULE(world, FlecsTimer);

    ECS_IMPORT(world, FlecsPipeline);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsTimer);
    flecs_bootstrap_component(world, EcsRateFilter);

    /* Add EcsTickSource to timers and rate filters */
    ECS_SYSTEM(world, AddTickSource, EcsPreFrame, [in] Timer || RateFilter, [out] !flecs.system.TickSource);

    /* Timer handling */
    ECS_SYSTEM(world, ProgressTimers, EcsPreFrame, Timer, flecs.system.TickSource);

    /* Rate filter handling */
    ECS_SYSTEM(world, ProgressRateFilters, EcsPreFrame, [in] RateFilter, [out] flecs.system.TickSource);

    /* TickSource without a timer or rate filter just increases each frame */
    ECS_SYSTEM(world, ProgressTickSource, EcsPreFrame, [out] flecs.system.TickSource, !RateFilter, !Timer);
}

#endif

#ifdef FLECS_SYSTEM


/* Global type variables */
ECS_TYPE_DECL(EcsComponentLifecycle);
ECS_TYPE_DECL(EcsSystem);
ECS_TYPE_DECL(EcsTickSource);

static
ecs_on_demand_in_t* get_in_component(
    ecs_map_t *component_map,
    ecs_entity_t component)
{
    ecs_on_demand_in_t *in = ecs_map_get(
        component_map, ecs_on_demand_in_t, component);
    if (!in) {
        ecs_on_demand_in_t in_value = {0};
        ecs_map_set(component_map, component, &in_value);
        in = ecs_map_get(component_map, ecs_on_demand_in_t, component);
        ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);
    }

    return in;
}

static
void activate_in_columns(
    ecs_world_t *world,
    ecs_query_t *query,
    ecs_map_t *component_map,
    bool activate)
{
    ecs_term_t *terms = query->filter.terms;
    int32_t i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        if (terms[i].inout == EcsIn) {
            ecs_on_demand_in_t *in = get_in_component(
                component_map, terms[i].id);
            ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

            in->count += activate ? 1 : -1;

            ecs_assert(in->count >= 0, ECS_INTERNAL_ERROR, NULL);

            /* If this is the first system that registers the in component, walk
             * over all already registered systems to enable them */
            if (in->systems && 
               ((activate && in->count == 1) || 
                (!activate && !in->count))) 
            {
                ecs_on_demand_out_t **out = ecs_vector_first(
                    in->systems, ecs_on_demand_out_t*);
                int32_t s, in_count = ecs_vector_count(in->systems);

                for (s = 0; s < in_count; s ++) {
                    /* Increase the count of the system with the out params */
                    out[s]->count += activate ? 1 : -1;
                    
                    /* If this is the first out column that is requested from
                     * the OnDemand system, enable it */
                    if (activate && out[s]->count == 1) {
                        ecs_remove_id(world, out[s]->system, EcsDisabledIntern);
                    } else if (!activate && !out[s]->count) {
                        ecs_add_id(world, out[s]->system, EcsDisabledIntern);             
                    }
                }
            }
        }
    }    
}

static
void register_out_column(
    ecs_map_t *component_map,
    ecs_entity_t component,
    ecs_on_demand_out_t *on_demand_out)
{
    ecs_on_demand_in_t *in = get_in_component(component_map, component);
    ecs_assert(in != NULL, ECS_INTERNAL_ERROR, NULL);

    on_demand_out->count += in->count;
    ecs_on_demand_out_t **elem = ecs_vector_add(&in->systems, ecs_on_demand_out_t*);
    *elem = on_demand_out;
}

static
void register_out_columns(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystem *system_data)
{
    ecs_query_t *query = system_data->query;
    ecs_term_t *terms = query->filter.terms;
    int32_t out_count = 0, i, count = query->filter.term_count;

    for (i = 0; i < count; i ++) {
        if (terms[i].inout == EcsOut) {
            if (!system_data->on_demand) {
                system_data->on_demand = ecs_os_malloc(sizeof(ecs_on_demand_out_t));
                ecs_assert(system_data->on_demand != NULL, ECS_OUT_OF_MEMORY, NULL);

                system_data->on_demand->system = system;
                system_data->on_demand->count = 0;
            }

            /* If column operator is NOT and the inout kind is [out], the system
             * explicitly states that it will create the component (it is not
             * there, yet it is an out column). In this case it doesn't make
             * sense to wait until [in] terms get activated (matched with
             * entities) since the component is not there yet. Therefore add it
             * to the on_enable_components list, so this system will be enabled
             * when a [in] column is enabled, rather than activated */
            ecs_map_t *component_map;
            if (terms[i].oper == EcsNot) {
                component_map = world->on_enable_components;
            } else {
                component_map = world->on_activate_components;
            }

            register_out_column(
                component_map, terms[i].id, 
                system_data->on_demand);

            out_count ++;
        }
    }

    /* If there are no out terms in the on-demand system, the system will
     * never be enabled */
    ecs_assert(out_count != 0, ECS_NO_OUT_COLUMNS, ecs_get_name(world, system));
}

static
void invoke_status_action(
    ecs_world_t *world,
    ecs_entity_t system,
    const EcsSystem *system_data,
    ecs_system_status_t status)
{
    ecs_system_status_action_t action = system_data->status_action;
    if (action) {
        action(world, system, status, system_data->status_ctx);
    }
}

/* Invoked when system becomes active or inactive */
void ecs_system_activate(
    ecs_world_t *world,
    ecs_entity_t system,
    bool activate,
    const EcsSystem *system_data)
{
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

    if (activate) {
        /* If activating system, ensure that it doesn't have the Inactive tag.
         * Systems are implicitly activated so they are kept out of the main
         * loop as long as they aren't used. They are not implicitly deactivated
         * to prevent overhead in case of oscillating app behavior. 
         * After activation, systems that aren't matched with anything can be
         * deactivated again by explicitly calling ecs_deactivate_systems.
         */
        ecs_remove_id(world, system, EcsInactive);
    }

    if (!system_data) {
        system_data = ecs_get(world, system, EcsSystem);
    }
    if (!system_data || !system_data->query) {
        return;
    }

    if (!activate) {
        if (ecs_has_id(world, system, EcsDisabled) || 
            ecs_has_id(world, system, EcsDisabledIntern)) 
        {
            if (!ecs_vector_count(system_data->query->tables)) {
                /* If deactivating a disabled system that isn't matched with
                 * any active tables, there is nothing to deactivate. */
                return;
            }
        }            
    }

    /* If system contains in columns, signal that they are now in use */
    activate_in_columns(
        world, system_data->query, world->on_activate_components, activate);

    /* Invoke system status action */
    invoke_status_action(world, system, system_data, 
        activate ? EcsSystemActivated : EcsSystemDeactivated);

    ecs_trace_2("system #[green]%s#[reset] %s", 
        ecs_get_name(world, system), 
        activate ? "activated" : "deactivated");
}

/* Actually enable or disable system */
static
void ecs_enable_system(
    ecs_world_t *world,
    ecs_entity_t system,
    EcsSystem *system_data,
    bool enabled)
{
    ecs_assert(!world->is_readonly, ECS_INTERNAL_ERROR, NULL);

    ecs_query_t *query = system_data->query;
    if (!query) {
        return;
    }

    if (ecs_vector_count(query->tables)) {
        /* Only (de)activate system if it has non-empty tables. */
        ecs_system_activate(world, system, enabled, system_data);
        system_data = ecs_get_mut(world, system, EcsSystem, NULL);
    }

    /* Enable/disable systems that trigger on [in] enablement */
    activate_in_columns(
        world, 
        query, 
        world->on_enable_components, 
        enabled);
    
    /* Invoke action for enable/disable status */
    invoke_status_action(
        world, system, system_data,
        enabled ? EcsSystemEnabled : EcsSystemDisabled);
}

/* -- Public API -- */

void ecs_enable(
    ecs_world_t *world,
    ecs_entity_t entity,
    bool enabled)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_PARAMETER, NULL);

    const EcsType *type_ptr = ecs_get( world, entity, EcsType);
    if (type_ptr) {
        /* If entity is a type, disable all entities in the type */
        ecs_vector_each(type_ptr->normalized, ecs_entity_t, e, {
            ecs_enable(world, *e, enabled);
        });
    } else {
        if (enabled) {
            ecs_remove_id(world, entity, EcsDisabled);
        } else {
            ecs_add_id(world, entity, EcsDisabled);
        }
    }
}

ecs_entity_t ecs_run_intern(
    ecs_world_t *world,
    ecs_stage_t *stage,
    ecs_entity_t system,
    EcsSystem *system_data,
    int32_t stage_current,
    int32_t stage_count,    
    FLECS_FLOAT delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param) 
{
    FLECS_FLOAT time_elapsed = delta_time;
    ecs_entity_t tick_source = system_data->tick_source;

    /* Support legacy behavior */
    if (!param) {
        param = system_data->ctx;
    }

    if (tick_source) {
        const EcsTickSource *tick = ecs_get(
            world, tick_source, EcsTickSource);

        if (tick) {
            time_elapsed = tick->time_elapsed;

            /* If timer hasn't fired we shouldn't run the system */
            if (!tick->tick) {
                return 0;
            }
        } else {
            /* If a timer has been set but the timer entity does not have the
             * EcsTimer component, don't run the system. This can be the result
             * of a single-shot timer that has fired already. Not resetting the
             * timer field of the system will ensure that the system won't be
             * ran after the timer has fired. */
            return 0;
        }
    }

    ecs_time_t time_start;
    bool measure_time = world->measure_system_time;
    if (measure_time) {
        ecs_os_get_time(&time_start);
    }

    ecs_defer_begin(stage->thread_ctx);

    /* Prepare the query iterator */
    ecs_iter_t it = ecs_query_iter_page(system_data->query, offset, limit);
    it.world = stage->thread_ctx;
    it.system = system;
    it.self = system_data->self;
    it.delta_time = delta_time;
    it.delta_system_time = time_elapsed;
    it.world_time = world->stats.world_time_total;
    it.frame_offset = offset;
    it.param = param;
    it.ctx = system_data->ctx;
    it.binding_ctx = system_data->binding_ctx;

    ecs_iter_action_t action = system_data->action;

    /* If no filter is provided, just iterate tables & invoke action */
    if (stage_count <= 1) {
        while (ecs_query_next_w_filter(&it, filter)) {
            action(&it);
        }
    } else {
        while (ecs_query_next_worker(&it, stage_current, stage_count)) {
            action(&it);               
        }
    }

    ecs_defer_end(stage->thread_ctx);

    if (measure_time) {
        system_data->time_spent += (FLECS_FLOAT)ecs_time_measure(&time_start);
    }

    system_data->invoke_count ++;

    return it.interrupted_by;
}

/* -- Public API -- */

ecs_entity_t ecs_run_w_filter(
    ecs_world_t *world,
    ecs_entity_t system,
    FLECS_FLOAT delta_time,
    int32_t offset,
    int32_t limit,
    const ecs_filter_t *filter,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    EcsSystem *system_data = (EcsSystem*)ecs_get(
        world, system, EcsSystem);
    assert(system_data != NULL);

    return ecs_run_intern(
        world, stage, system, system_data, 0, 0, delta_time, offset, limit, 
        filter, param);
}

ecs_entity_t ecs_run_worker(
    ecs_world_t *world,
    ecs_entity_t system,
    int32_t stage_current,
    int32_t stage_count,
    FLECS_FLOAT delta_time,
    void *param)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    EcsSystem *system_data = (EcsSystem*)ecs_get(
        world, system, EcsSystem);
    assert(system_data != NULL);

    return ecs_run_intern(
        world, stage, system, system_data, stage_current, stage_count, 
        delta_time, 0, 0, NULL, param);
}

ecs_entity_t ecs_run(
    ecs_world_t *world,
    ecs_entity_t system,
    FLECS_FLOAT delta_time,
    void *param)
{
    return ecs_run_w_filter(world, system, delta_time, 0, 0, NULL, param);
}

void flecs_run_monitor(
    ecs_world_t *world,
    ecs_matched_query_t *monitor,
    ecs_ids_t *components,
    int32_t row,
    int32_t count,
    ecs_entity_t *entities)
{
    ecs_query_t *query = monitor->query;
    ecs_assert(query != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t system = query->system;
    const EcsSystem *system_data = ecs_get(world, system, EcsSystem);
    ecs_assert(system_data != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!system_data->action) {
        return;
    }

    ecs_iter_t it = {0};
    flecs_query_set_iter( world, query, &it, 
        monitor->matched_table_index, row, count);

    it.world = world;
    it.triggered_by = components;
    it.is_valid = true;
    it.ctx = system_data->ctx;
    it.binding_ctx = system_data->binding_ctx;

    if (entities) {
        it.entities = entities;
    }

    it.system = system;
    system_data->action(&it);
}

ecs_query_t* ecs_get_system_query(
    const ecs_world_t *world,
    ecs_entity_t system)
{
    const EcsQuery *q = ecs_get(world, system, EcsQuery);
    if (q) {
        return q->query;
    } else {
        const EcsSystem *s = ecs_get(world, system, EcsSystem);
        if (s) {
            return s->query;
        } else {
            return NULL;
        }
    }
}

void* ecs_get_system_ctx(
    const ecs_world_t *world,
    ecs_entity_t system)
{
    const EcsSystem *s = ecs_get(world, system, EcsSystem);
    if (s) {
        return s->ctx;
    } else {
        return NULL;
    }   
}

void* ecs_get_system_binding_ctx(
    const ecs_world_t *world,
    ecs_entity_t system)
{
    const EcsSystem *s = ecs_get(world, system, EcsSystem);
    if (s) {
        return s->binding_ctx;
    } else {
        return NULL;
    }   
}

/* Generic constructor to initialize a component to 0 */
static
void sys_ctor_init_zero(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)world;
    (void)component;
    (void)entities;
    (void)ctx;
    memset(ptr, 0, size * (size_t)count);
}

/* System destructor */
static
void ecs_colsystem_dtor(
    ecs_world_t *world,
    ecs_entity_t component,
    const ecs_entity_t *entities,
    void *ptr,
    size_t size,
    int32_t count,
    void *ctx)
{
    (void)component;
    (void)ctx;
    (void)size;

    EcsSystem *system_data = ptr;

    int i;
    for (i = 0; i < count; i ++) {
        EcsSystem *system = &system_data[i];
        ecs_entity_t e = entities[i];

        /* Invoke Deactivated action for active systems */
        if (system->query && ecs_vector_count(system->query->tables)) {
            invoke_status_action(world, e, ptr, EcsSystemDeactivated);
        }

        /* Invoke Disabled action for enabled systems */
        if (!ecs_has_id(world, e, EcsDisabled) && 
            !ecs_has_id(world, e, EcsDisabledIntern)) 
        {
            invoke_status_action(world, e, ptr, EcsSystemDisabled);
        }

        ecs_os_free(system->on_demand);

        if (system->ctx_free) {
            system->ctx_free(system->ctx);
        }

        if (system->status_ctx_free) {
            system->status_ctx_free(system->status_ctx);
        }

        if (system->binding_ctx_free) {
            system->binding_ctx_free(system->binding_ctx);
        }  

        if (system->query) {
            ecs_query_fini(system->query);
        }
    }
}

/* Disable system when EcsDisabled is added */
static 
void DisableSystem(
    ecs_iter_t *it)
{
    EcsSystem *system_data = ecs_term(it, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_enable_system(
            it->world, it->entities[i], &system_data[i], false);
    }
}

/* Enable system when EcsDisabled is removed */
static
void EnableSystem(
    ecs_iter_t *it)
{
    EcsSystem *system_data = ecs_term(it, EcsSystem, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        ecs_enable_system(
            it->world, it->entities[i], &system_data[i], true);
    }
}

ecs_entity_t ecs_system_init(
    ecs_world_t *world,
    const ecs_system_desc_t *desc)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_FROM_WORKER, NULL);
    ecs_assert(!world->is_readonly, ECS_INVALID_WHILE_ITERATING, NULL);

    ecs_entity_t existing = desc->entity.entity;
    ecs_entity_t result = ecs_entity_init(world, &desc->entity);
    if (!result) {
        return 0;
    }

    bool added = false;
    EcsSystem *system = ecs_get_mut(world, result, EcsSystem, &added);
    if (added) {
        ecs_assert(desc->callback != NULL, ECS_INVALID_PARAMETER, NULL);

        memset(system, 0, sizeof(EcsSystem));

        ecs_query_desc_t query_desc = desc->query;
        query_desc.filter.name = desc->entity.name;
        query_desc.system = result;

        ecs_query_t *query = ecs_query_init(world, &query_desc);
        if (!query) {
            ecs_delete(world, result);
            return 0;
        }

        /* Re-obtain pointer, as query may have added components */
        system = ecs_get_mut(world, result, EcsSystem, &added);
        ecs_assert(added == false, ECS_INTERNAL_ERROR, NULL);

        /* Prevent the system from moving while we're initializing */
        ecs_defer_begin(world);

        system->entity = result;
        system->query = query;

        system->action = desc->callback;
        system->status_action = desc->status_callback;

        system->self = desc->self;
        system->ctx = desc->ctx;
        system->status_ctx = desc->status_ctx;
        system->binding_ctx = desc->binding_ctx;

        system->ctx_free = desc->ctx_free;
        system->status_ctx_free = desc->status_ctx_free;
        system->binding_ctx_free = desc->binding_ctx_free;

        system->tick_source = desc->tick_source;

        /* If tables have been matched with this system it is active, and we
         * should activate the in terms, if any. This will ensure that any
         * OnDemand systems get enabled. */
        if (ecs_vector_count(query->tables)) {
            ecs_system_activate(world, result, true, system);
        } else {
            /* If system isn't matched with any tables, mark it as inactive. This
             * causes it to be ignored by the main loop. When the system matches
             * with a table it will be activated. */
            ecs_add_id(world, result, EcsInactive);
        }

        /* If system is enabled, trigger enable components */
        activate_in_columns(world, query, world->on_enable_components, true);

        /* If the query has a OnDemand system tag, register its [out] terms */
        if (ecs_has_id(world, result, EcsOnDemand)) {
            register_out_columns(world, result, system);
            ecs_assert(system->on_demand != NULL, ECS_INTERNAL_ERROR, NULL);

            /* If there are no systems currently interested in any of the [out]
             * terms of the on demand system, disable it */
            if (!system->on_demand->count) {
                ecs_add_id(world, result, EcsDisabledIntern);
            }        
        }

        if (!ecs_has_id(world, result, EcsDisabled)) {
            /* If system is already enabled, generate enable status. The API 
            * should guarantee that it exactly matches enable-disable 
            * notifications and activate-deactivate notifications. */
            invoke_status_action(world, result, system, EcsSystemEnabled);

            /* If column system has active (non-empty) tables, also generate the
            * activate status. */
            if (ecs_vector_count(system->query->tables)) {
                invoke_status_action(world, result, system, EcsSystemActivated);
            }
        }

        if (desc->interval != 0 || desc->rate != 0 || desc->tick_source != 0) {
#ifdef FLECS_TIMER
            if (desc->interval != 0) {
                ecs_set_interval(world, result, desc->interval);
            }

            if (desc->rate) {
                ecs_set_rate(world, result, desc->rate, desc->tick_source);
            } else if (desc->tick_source) {
                ecs_set_tick_source(world, result, desc->tick_source);
            }
#else
            ecs_abort(ECS_UNSUPPORTED, "timer module not available");
#endif
        }

        ecs_modified(world, result, EcsSystem);

        ecs_trace_1("system #[green]%s#[reset] created with #[red]%s", 
            ecs_get_name(world, result), query->filter.expr);

        ecs_defer_end(world);            
    } else {
        const char *expr_desc = desc->query.filter.expr;
        const char *expr_sys = system->query->filter.expr;

        /* Only check expression if it's set */
        if (expr_desc) {
            if (expr_sys && !strcmp(expr_sys, "0")) expr_sys = NULL;
            if (expr_desc && !strcmp(expr_desc, "0")) expr_desc = NULL;

            if (expr_sys && expr_desc) {
                if (strcmp(expr_sys, expr_desc)) {
                    ecs_abort(ECS_ALREADY_DEFINED, desc->entity.name);
                }
            } else {
                if (expr_sys != expr_desc) {
                    ecs_abort(ECS_ALREADY_DEFINED, desc->entity.name);
                }
            }

        /* If expr_desc is not set, and this is an existing system, don't throw
         * an error because we could be updating existing parameters of the
         * system such as the context or system callback. However, if no
         * entity handle was provided, we have to assume that the application is
         * trying to redeclare the system. */
        } else if (!existing) {
            if (expr_sys) {
                ecs_abort(ECS_ALREADY_DEFINED, desc->entity.name);
            }
        }

        /* Override the existing callback or context */
        if (desc->callback) {
            system->action = desc->callback;
        }
        if (desc->ctx) {
            system->ctx = desc->ctx;
        }
        if (desc->binding_ctx) {
            system->binding_ctx = desc->binding_ctx;
        }
    }

    return result;
}

void FlecsSystemImport(
    ecs_world_t *world)
{
    ECS_MODULE(world, FlecsSystem);

    ecs_set_name_prefix(world, "Ecs");

    flecs_bootstrap_component(world, EcsSystem);
    flecs_bootstrap_component(world, EcsTickSource);

    flecs_bootstrap_tag(world, EcsOnAdd);
    flecs_bootstrap_tag(world, EcsOnRemove);
    flecs_bootstrap_tag(world, EcsOnSet);
    flecs_bootstrap_tag(world, EcsUnSet);

    /* Put following tags in flecs.core so they can be looked up
     * without using the flecs.systems prefix. */
    ecs_entity_t old_scope = ecs_set_scope(world, EcsFlecsCore);
    flecs_bootstrap_tag(world, EcsDisabledIntern);
    flecs_bootstrap_tag(world, EcsInactive);
    flecs_bootstrap_tag(world, EcsOnDemand);
    flecs_bootstrap_tag(world, EcsMonitor);
    ecs_set_scope(world, old_scope);

    ECS_TYPE_IMPL(EcsComponentLifecycle);
    ECS_TYPE_IMPL(EcsSystem);
    ECS_TYPE_IMPL(EcsTickSource);

    /* Bootstrap ctor and dtor for EcsSystem */
    ecs_set_component_actions_w_id(world, ecs_id(EcsSystem), 
        &(EcsComponentLifecycle) {
            .ctor = sys_ctor_init_zero,
            .dtor = ecs_colsystem_dtor
        });

    /* Monitors that trigger when a system is enabled or disabled */
    ECS_SYSTEM(world, DisableSystem, EcsMonitor, 
        System, Disabled || DisabledIntern, SYSTEM:Hidden);

    ECS_SYSTEM(world, EnableSystem, EcsMonitor, 
        System, !Disabled, !DisabledIntern, SYSTEM:Hidden);
}

#endif

static
ecs_vector_t* sort_and_dedup(
    ecs_vector_t *result)
{
    /* Sort vector */
    ecs_vector_sort(result, ecs_id_t, flecs_entity_compare_qsort);

    /* Ensure vector doesn't contain duplicates */
    ecs_id_t *ids = ecs_vector_first(result, ecs_id_t);
    int32_t i, offset = 0, count = ecs_vector_count(result);

    for (i = 0; i < count; i ++) {
        if (i && ids[i] == ids[i - 1]) {
            offset ++;
        }

        if (i + offset >= count) {
            break;
        }

        ids[i] = ids[i + offset];
    }

    ecs_vector_set_count(&result, ecs_id_t, i - offset);

    return result;
}

/** Parse callback that adds type to type identifier */
static
ecs_vector_t* expr_to_ids(
    ecs_world_t *world,
    const char *name,
    const char *expr)
{
#ifdef FLECS_PARSER    
    ecs_vector_t *result = NULL;
    const char *ptr = expr;
    ecs_term_t term = {0};

    if (!ptr) {
        return NULL;
    }

    while (ptr[0] && (ptr = ecs_parse_term(world, name, expr, ptr, &term))) {
        if (term.name) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "column names not supported in type expression");
            goto error;
        }

        if (term.oper != EcsAnd && term.oper != EcsAndFrom) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "operator other than AND not supported in type expression");
            goto error;
        }

        if (ecs_term_finalize(world, name, expr, &term)) {
            goto error;
        }

        if (term.args[0].entity == 0) {
            /* Empty term */
            goto done;
        }

        if (term.args[0].set.mask != EcsDefaultSet) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "source modifiers not supported for type expressions");
            goto error;
        }

        if (term.args[0].entity != EcsThis) {
            ecs_parser_error(name, expr, (ptr - expr), 
                "subject other than this not supported in type expression");
            goto error;
        }

        if (term.oper == EcsAndFrom) {
            term.role = ECS_AND;
        }

        ecs_id_t* elem = ecs_vector_add(&result, ecs_id_t);
        *elem = term.id | term.role;

        ecs_term_fini(&term);
    }

    result = sort_and_dedup(result);

done:
    return result;
error:
    ecs_term_fini(&term);
    ecs_vector_free(result);
    return NULL;
#else
    (void)world;
    (void)name;
    (void)expr;
    ecs_abort(ECS_UNSUPPORTED, "parser addon is not available");
    return NULL;
#endif    
}

/* Create normalized type. A normalized type resolves all elements with an
 * AND flag and appends them to the resulting type, where the default type
 * maintains the original type hierarchy. */
static
ecs_vector_t* ids_to_normalized_ids(
    ecs_world_t *world,
    ecs_vector_t *ids)
{
    ecs_vector_t *result = NULL;

    ecs_entity_t *array = ecs_vector_first(ids, ecs_id_t);
    int32_t i, count = ecs_vector_count(ids);

    for (i = 0; i < count; i ++) {
        ecs_entity_t e = array[i];
        if (ECS_HAS_ROLE(e, AND)) {
            ecs_entity_t entity = ECS_PAIR_OBJECT(e);

            const EcsType *type_ptr = ecs_get(world, entity, EcsType);
            ecs_assert(type_ptr != NULL, ECS_INVALID_PARAMETER, 
                "flag must be applied to type");

            ecs_vector_each(type_ptr->normalized, ecs_id_t, c_ptr, {
                ecs_entity_t *el = ecs_vector_add(&result, ecs_id_t);
                *el = *c_ptr;
            })
        } else {
            ecs_entity_t *el = ecs_vector_add(&result, ecs_id_t);
            *el = e;
        }   
    }

    return sort_and_dedup(result);
}

static
ecs_table_t* table_from_ids(
    ecs_world_t *world,
    ecs_vector_t *ids)
{
    ecs_ids_t ids_array = flecs_type_to_ids(ids);
    ecs_table_t *result = flecs_table_find_or_create(world, &ids_array);
    return result;
}

/* If a name prefix is set with ecs_set_name_prefix, check if the entity name
 * has the prefix, and if so remove it. This enables using prefixed names in C
 * for components / systems while storing a canonical / language independent 
 * identifier. */
const char* flecs_name_from_symbol(
    ecs_world_t *world,
    const char *type_name)
{
    const char *prefix = world->name_prefix;
    if (type_name && prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(type_name, prefix, len) && 
           (isupper(type_name[len]) || type_name[len] == '_')) 
        {
            if (type_name[len] == '_') {
                return type_name + len + 1;
            } else {
                return type_name + len;
            }
        }
    }

    return type_name;
}

/* -- Public functions -- */

ecs_type_t ecs_type_from_str(
    ecs_world_t *world,
    const char *expr)
{
    ecs_vector_t *ids = expr_to_ids(world, NULL, expr);
    if (!ids) {
        return NULL;
    }

    ecs_vector_t *normalized_ids = ids_to_normalized_ids(world, ids);
    ecs_vector_free(ids);

    ecs_table_t *table = table_from_ids(world, normalized_ids);
    ecs_vector_free(normalized_ids);

    return table->type;
}

ecs_table_t* ecs_table_from_str(
    ecs_world_t *world,
    const char *expr)
{
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INVALID_OPERATION, NULL);

    ecs_vector_t *ids = expr_to_ids(world, NULL, expr);
    if (!ids) {
        return NULL;
    }

    ecs_table_t *result = table_from_ids(world, ids);
    ecs_vector_free(ids);

    return result;
}

typedef struct ecs_hm_bucket_t {
    ecs_vector_t *keys;
    ecs_vector_t *values;
} ecs_hm_bucket_t;

static
int32_t find_key(
    ecs_hashmap_t map,
    ecs_vector_t *keys,
    ecs_size_t key_size, 
    const void *key)
{
    int32_t i, count = ecs_vector_count(keys);
    void *key_array = ecs_vector_first_t(keys, key_size, 8);
    for (i = 0; i < count; i ++) {
        void *key_ptr = ECS_OFFSET(key_array, key_size * i);
        if (map.compare(key_ptr, key) == 0) {
            return i;
        }
    }
    return -1;
}

ecs_hashmap_t _flecs_hashmap_new(
    ecs_size_t key_size,
    ecs_size_t value_size,
    ecs_hash_value_action_t hash,
    ecs_compare_action_t compare)
{
    return (ecs_hashmap_t){
        .key_size = key_size,
        .value_size = value_size,
        .compare = compare,
        .hash = hash,
        .impl = ecs_map_new(ecs_hm_bucket_t, 0)
    };
}

void flecs_hashmap_free(
    ecs_hashmap_t map)
{
    ecs_map_iter_t it = ecs_map_iter(map.impl);
    ecs_hm_bucket_t *bucket;
    while ((bucket = ecs_map_next(&it, ecs_hm_bucket_t, NULL))) {
        ecs_vector_free(bucket->keys);
        ecs_vector_free(bucket->values);
    }

    ecs_map_free(map.impl);
}

void* _flecs_hashmap_get(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map.key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map.value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map.hash(key);
    ecs_hm_bucket_t *bucket = ecs_map_get(map.impl, ecs_hm_bucket_t, hash);
    if (!bucket) {
        return NULL;
    }

    int32_t index = find_key(map, bucket->keys, key_size, key);
    if (index == -1) {
        return NULL;
    }

    return ecs_vector_get_t(bucket->values, value_size, 8, index);
}

flecs_hashmap_result_t _flecs_hashmap_ensure(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size)
{
    ecs_assert(map.key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map.value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map.hash(key);
    ecs_hm_bucket_t *bucket = ecs_map_ensure(map.impl, ecs_hm_bucket_t, hash);
    ecs_assert(bucket != NULL, ECS_INTERNAL_ERROR, NULL);

    void *value_ptr, *key_ptr;

    ecs_vector_t *keys = bucket->keys;
    if (!keys) {
        bucket->keys = ecs_vector_new_t(key_size, 8, 1);
        bucket->values = ecs_vector_new_t(value_size, 8, 1);
        key_ptr = ecs_vector_add_t(&bucket->keys, key_size, 8);
        ecs_os_memcpy(key_ptr, key, key_size);
        value_ptr = ecs_vector_add_t(&bucket->values, value_size, 8);
        ecs_os_memset(value_ptr, 0, value_size);
    } else {
        int32_t index = find_key(map, keys, key_size, key);
        if (index == -1) {
            key_ptr = ecs_vector_add_t(&bucket->keys, key_size, 8);
            ecs_os_memcpy(key_ptr, key, key_size);
            value_ptr = ecs_vector_add_t(&bucket->values, value_size, 8);
            ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
            ecs_os_memset(value_ptr, 0, value_size);
        } else {
            key_ptr = ecs_vector_get_t(bucket->keys, key_size, 8, index);
            value_ptr = ecs_vector_get_t(bucket->values, value_size, 8, index);
        }
    }

    return (flecs_hashmap_result_t){
        .key = key_ptr,
        .value = value_ptr,
        .hash = hash
    };
}

void _flecs_hashmap_set(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    void *key,
    ecs_size_t value_size,
    const void *value)
{
    void *value_ptr = _flecs_hashmap_ensure(map, key_size, key, value_size).value;
    ecs_assert(value_ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_os_memcpy(value_ptr, value, value_size);
}

void _flecs_hashmap_remove_w_hash(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size,
    uint64_t hash)
{
    ecs_hm_bucket_t *bucket = ecs_map_get(map.impl, ecs_hm_bucket_t, hash);
    if (!bucket) {
        return;
    }

    int32_t index = find_key(map, bucket->keys, key_size, key);
    if (index == -1) {
        return;
    }

    ecs_vector_remove_t(bucket->keys, key_size, 8, index);
    ecs_vector_remove_t(bucket->values, value_size, 8, index);

    if (!ecs_vector_count(bucket->keys)) {
        ecs_vector_free(bucket->keys);
        ecs_vector_free(bucket->values);
        ecs_map_remove(map.impl, hash);
    }
}

void _flecs_hashmap_remove(
    const ecs_hashmap_t map,
    ecs_size_t key_size,
    const void *key,
    ecs_size_t value_size)
{
    ecs_assert(map.key_size == key_size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(map.value_size == value_size, ECS_INVALID_PARAMETER, NULL);

    uint64_t hash = map.hash(key);
    _flecs_hashmap_remove_w_hash(map, key_size, key, value_size, hash);
}

flecs_hashmap_iter_t flecs_hashmap_iter(
    ecs_hashmap_t map)
{
    return (flecs_hashmap_iter_t){
        .it = ecs_map_iter(map.impl)
    };
}

void* _flecs_hashmap_next(
    flecs_hashmap_iter_t *it,
    ecs_size_t key_size,
    void *key_out,
    ecs_size_t value_size)
{
    int32_t index = ++ it->index;
    ecs_hm_bucket_t *bucket = it->bucket;
    while (!bucket || it->index >= ecs_vector_count(bucket->keys)) {
        bucket = it->bucket = ecs_map_next(&it->it, ecs_hm_bucket_t, NULL);
        if (!bucket) {
            return NULL;
        }
        index = it->index = 0;
    }

    if (key_out) {
        *(void**)key_out = ecs_vector_get_t(bucket->keys, key_size, 8, index);
    }
    
    return ecs_vector_get_t(bucket->values, value_size, 8, index);
}

/* Global type variables */
ecs_type_t ecs_type(EcsComponent);
ecs_type_t ecs_type(EcsType);
ecs_type_t ecs_type(EcsIdentifier);
ecs_type_t ecs_type(EcsQuery);
ecs_type_t ecs_type(EcsTrigger);
ecs_type_t ecs_type(EcsObserver);
ecs_type_t ecs_type(EcsPrefab);

/* Component lifecycle actions for EcsIdentifier */
static ECS_CTOR(EcsIdentifier, ptr, {
    ptr->value = NULL;
    ptr->hash = 0;
    ptr->length = 0;
})

static ECS_DTOR(EcsIdentifier, ptr, {
    ecs_os_strset(&ptr->value, NULL);    
})

static ECS_COPY(EcsIdentifier, dst, src, {
    ecs_os_strset(&dst->value, src->value);
    dst->hash = src->hash;
    dst->length = src->length;
})

static ECS_MOVE(EcsIdentifier, dst, src, {
    ecs_os_strset(&dst->value, NULL);
    dst->value = src->value;
    dst->hash = src->hash;
    dst->length = src->length;

    src->value = NULL;
    src->hash = 0;
    src->length = 0;

})

static ECS_ON_SET(EcsIdentifier, ptr, {
    if (ptr->value) {
        ptr->length = ecs_os_strlen(ptr->value);
        ptr->hash = flecs_hash(ptr->value, ptr->length);
    } else {
        ptr->length = 0;
        ptr->hash = 0;
    }
})

/* Component lifecycle actions for EcsTrigger */
static ECS_CTOR(EcsTrigger, ptr, {
    ptr->trigger = NULL;
})

static ECS_DTOR(EcsTrigger, ptr, {
    flecs_trigger_fini(world, (ecs_trigger_t*)ptr->trigger);
})

static ECS_COPY(EcsTrigger, dst, src, {
    ecs_abort(ECS_INVALID_OPERATION, "Trigger component cannot be copied");
})

static ECS_MOVE(EcsTrigger, dst, src, {
    if (dst->trigger) {
        flecs_trigger_fini(world, (ecs_trigger_t*)dst->trigger);
    }
    dst->trigger = src->trigger;
    src->trigger = NULL;
})

/* Component lifecycle actions for EcsObserver */
static ECS_CTOR(EcsObserver, ptr, {
    ptr->observer = NULL;
})

static ECS_DTOR(EcsObserver, ptr, {
    flecs_observer_fini(world, (ecs_observer_t*)ptr->observer);
})

static ECS_COPY(EcsObserver, dst, src, {
    ecs_abort(ECS_INVALID_OPERATION, "Observer component cannot be copied");
})

static ECS_MOVE(EcsObserver, dst, src, {
    if (dst->observer) {
        flecs_observer_fini(world, (ecs_observer_t*)dst->observer);
    }
    dst->observer = src->observer;
    src->observer = NULL;
})

static
void register_on_delete(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_id_record_t *r = flecs_ensure_id_record(it->world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete = ECS_PAIR_OBJECT(id);

        r = flecs_ensure_id_record(it->world, ecs_pair(e, EcsWildcard));
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete = ECS_PAIR_OBJECT(id);

        flecs_set_watch(it->world, e);
    }
}

static
void register_on_delete_object(ecs_iter_t *it) {
    ecs_id_t id = ecs_term_id(it, 1);
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_id_record_t *r = flecs_ensure_id_record(it->world, e);
        ecs_assert(r != NULL, ECS_INTERNAL_ERROR, NULL);
        r->on_delete_object = ECS_PAIR_OBJECT(id);

        flecs_set_watch(it->world, e);
    }    
}

static
void on_set_component_lifecycle( ecs_iter_t *it) {
    EcsComponentLifecycle *cl = ecs_term(it, EcsComponentLifecycle, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        ecs_set_component_actions_w_id(world, e, &cl[i]);   
    }
}

/* -- Bootstrapping -- */

#define bootstrap_component(world, table, name)\
    _bootstrap_component(world, table, ecs_id(name), #name, sizeof(name),\
        ECS_ALIGNOF(name))

static
void _bootstrap_component(
    ecs_world_t *world,
    ecs_table_t *table,
    ecs_entity_t entity,
    const char *symbol,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_data_t *data = flecs_table_get_or_create_data(table);
    ecs_assert(data != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_column_t *columns = data->columns;
    ecs_assert(columns != NULL, ECS_INTERNAL_ERROR, NULL);

    ecs_record_t *record = ecs_eis_ensure(world, entity);
    record->table = table;

    int32_t index = flecs_table_append(world, table, data, entity, record, false);
    record->row = index + 1;

    EcsComponent *component = ecs_vector_first(columns[0].data, EcsComponent);
    component[index].size = size;
    component[index].alignment = alignment;

    const char *name = &symbol[3]; /* Strip 'Ecs' */
    ecs_size_t symbol_length = ecs_os_strlen(symbol);
    ecs_size_t name_length = symbol_length - 3;

    EcsIdentifier *name_col = ecs_vector_first(columns[1].data, EcsIdentifier);
    name_col[index].value = ecs_os_strdup(name);
    name_col[index].length = name_length;
    name_col[index].hash = flecs_hash(name, name_length);

    EcsIdentifier *symbol_col = ecs_vector_first(columns[2].data, EcsIdentifier);
    symbol_col[index].value = ecs_os_strdup(symbol);
    symbol_col[index].length = symbol_length;
    symbol_col[index].hash = flecs_hash(symbol, symbol_length);    
}

/** Create type for component */
ecs_type_t flecs_bootstrap_type(
    ecs_world_t *world,
    ecs_entity_t entity)
{
    ecs_table_t *table = flecs_table_find_or_create(world, &(ecs_ids_t){
        .array = (ecs_entity_t[]){entity},
        .count = 1
    });

    ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(table->type != NULL, ECS_INTERNAL_ERROR, NULL);

    return table->type;
}

/** Bootstrap types for builtin components and tags */
static
void bootstrap_types(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = flecs_bootstrap_type(world, ecs_id(EcsComponent));
    ecs_type(EcsType) = flecs_bootstrap_type(world, ecs_id(EcsType));
    ecs_type(EcsIdentifier) = flecs_bootstrap_type(world, ecs_id(EcsIdentifier));
}

/** Initialize component table. This table is manually constructed to bootstrap
 * flecs. After this function has been called, the builtin components can be
 * created. 
 * The reason this table is constructed manually is because it requires the size
 * and alignment of the EcsComponent and EcsIdentifier components, which haven't
 * been created yet */
static
ecs_table_t* bootstrap_component_table(
    ecs_world_t *world)
{
    ecs_entity_t entities[] = {
        ecs_id(EcsComponent), 
        ecs_pair(ecs_id(EcsIdentifier), EcsName),
        ecs_pair(ecs_id(EcsIdentifier), EcsSymbol),
        ecs_pair(EcsChildOf, EcsFlecsCore)
    };
    
    ecs_ids_t array = {
        .array = entities,
        .count = 4
    };

    ecs_table_t *result = flecs_table_find_or_create(world, &array);
    ecs_data_t *data = flecs_table_get_or_create_data(result);

    /* Preallocate enough memory for initial components */
    data->entities = ecs_vector_new(ecs_entity_t, EcsFirstUserComponentId);
    data->record_ptrs = ecs_vector_new(ecs_record_t*, EcsFirstUserComponentId);

    data->columns = ecs_os_malloc_n(ecs_column_t, 3);
    ecs_assert(data->columns != NULL, ECS_OUT_OF_MEMORY, NULL);

    data->columns[0].data = ecs_vector_new(EcsComponent, EcsFirstUserComponentId);
    data->columns[0].size = ECS_SIZEOF(EcsComponent);
    data->columns[0].alignment = ECS_ALIGNOF(EcsComponent);
    data->columns[1].data = ecs_vector_new(EcsIdentifier, EcsFirstUserComponentId);
    data->columns[1].size = ECS_SIZEOF(EcsIdentifier);
    data->columns[1].alignment = ECS_ALIGNOF(EcsIdentifier);
    data->columns[2].data = ecs_vector_new(EcsIdentifier, EcsFirstUserComponentId);
    data->columns[2].size = ECS_SIZEOF(EcsIdentifier);
    data->columns[2].alignment = ECS_ALIGNOF(EcsIdentifier);

    result->column_count = 3;
    
    return result;
}

static
void bootstrap_entity(
    ecs_world_t *world,
    ecs_entity_t id,
    const char *name,
    ecs_entity_t parent)
{
    char symbol[256];
    ecs_os_strcpy(symbol, "flecs.core.");
    ecs_os_strcat(symbol, name);

    ecs_set_name(world, id, name);
    ecs_set_symbol(world, id, symbol);

    ecs_assert(ecs_get_name(world, id) != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_add_pair(world, id, EcsChildOf, parent);

    if (!parent || parent == EcsFlecsCore) {
        ecs_assert(ecs_lookup_fullpath(world, name) == id, 
            ECS_INTERNAL_ERROR, NULL);
    }
}

void flecs_bootstrap(
    ecs_world_t *world)
{
    ecs_type(EcsComponent) = NULL;

    ecs_trace_1("bootstrap core components");
    ecs_log_push();

    /* Create table for initial components */
    ecs_table_t *table = bootstrap_component_table(world);
    assert(table != NULL);

    bootstrap_component(world, table, EcsIdentifier);
    bootstrap_component(world, table, EcsComponent);
    bootstrap_component(world, table, EcsComponentLifecycle);
    bootstrap_component(world, table, EcsType);
    bootstrap_component(world, table, EcsQuery);
    bootstrap_component(world, table, EcsTrigger);
    bootstrap_component(world, table, EcsObserver);

    ecs_set_component_actions(world, EcsIdentifier, {
        .ctor = ecs_ctor(EcsIdentifier),
        .dtor = ecs_dtor(EcsIdentifier),
        .copy = ecs_copy(EcsIdentifier),
        .move = ecs_move(EcsIdentifier),
        .on_set = ecs_on_set(EcsIdentifier)
    });

    ecs_set_component_actions(world, EcsTrigger, {
        .ctor = ecs_ctor(EcsTrigger),
        .dtor = ecs_dtor(EcsTrigger),
        .copy = ecs_copy(EcsTrigger),
        .move = ecs_move(EcsTrigger)
    }); 

    ecs_set_component_actions(world, EcsObserver, {
        .ctor = ecs_ctor(EcsObserver),
        .dtor = ecs_dtor(EcsObserver),
        .copy = ecs_copy(EcsObserver),
        .move = ecs_move(EcsObserver)
    });            

    world->stats.last_component_id = EcsFirstUserComponentId;
    world->stats.last_id = EcsFirstUserEntityId;
    world->stats.min_id = 0;
    world->stats.max_id = 0;

    bootstrap_types(world);

    ecs_set_scope(world, EcsFlecsCore);

    flecs_bootstrap_tag(world, EcsName);
    flecs_bootstrap_tag(world, EcsSymbol);

    flecs_bootstrap_tag(world, EcsModule);
    flecs_bootstrap_tag(world, EcsPrefab);
    flecs_bootstrap_tag(world, EcsHidden);
    flecs_bootstrap_tag(world, EcsDisabled);

    /* Initialize scopes */
    ecs_set_name(world, EcsFlecs, "flecs");
    ecs_add_id(world, EcsFlecs, EcsModule);
    ecs_set_name(world, EcsFlecsCore, "core");
    ecs_add_id(world, EcsFlecsCore, EcsModule);
    ecs_add_pair(world, EcsFlecsCore, EcsChildOf, EcsFlecs);

    /* Initialize builtin entities */
    bootstrap_entity(world, EcsWorld, "World", EcsFlecsCore);
    bootstrap_entity(world, EcsThis, "This", EcsFlecsCore);
    bootstrap_entity(world, EcsWildcard, "*", EcsFlecsCore);
    bootstrap_entity(world, EcsTransitive, "Transitive", EcsFlecsCore);
    bootstrap_entity(world, EcsFinal, "Final", EcsFlecsCore);
    bootstrap_entity(world, EcsTag, "Tag", EcsFlecsCore);

    bootstrap_entity(world, EcsIsA, "IsA", EcsFlecsCore);
    bootstrap_entity(world, EcsChildOf, "ChildOf", EcsFlecsCore);

    bootstrap_entity(world, EcsOnAdd, "OnAdd", EcsFlecsCore);
    bootstrap_entity(world, EcsOnRemove, "OnRemove", EcsFlecsCore);
    bootstrap_entity(world, EcsOnSet, "OnSet", EcsFlecsCore);
    bootstrap_entity(world, EcsUnSet, "UnSet", EcsFlecsCore);

    bootstrap_entity(world, EcsOnDelete, "OnDelete", EcsFlecsCore);

    // bootstrap_entity(world, EcsOnCreateTable, "OnCreateTable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteTable, "OnDeleteTable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnTableEmpty, "OnTableEmpty", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnTableNonEmpty, "OnTableNonEmpty", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnCreateTrigger, "OnCreateTrigger", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteTrigger, "OnDeleteTrigger", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnDeleteObservable, "OnDeleteObservable", EcsFlecsCore);
    // bootstrap_entity(world, EcsOnComponentLifecycle, "OnComponentLifecycle", EcsFlecsCore);
    
    bootstrap_entity(world, EcsOnDeleteObject, "OnDeleteObject", EcsFlecsCore);

    bootstrap_entity(world, EcsRemove, "Remove", EcsFlecsCore);
    bootstrap_entity(world, EcsDelete, "Delete", EcsFlecsCore);
    bootstrap_entity(world, EcsThrow, "Throw", EcsFlecsCore);


    /* Transitive relations */
    ecs_add_id(world, EcsIsA, EcsTransitive);

    /* Tag relations (relations that cannot have data) */
    ecs_add_id(world, EcsIsA, EcsTag);
    ecs_add_id(world, EcsChildOf, EcsTag);

    /* Final components/relations */
    ecs_add_id(world, ecs_id(EcsComponent), EcsFinal);
    ecs_add_id(world, ecs_id(EcsIdentifier), EcsFinal);
    ecs_add_id(world, EcsTransitive, EcsFinal);
    ecs_add_id(world, EcsFinal, EcsFinal);
    ecs_add_id(world, EcsIsA, EcsFinal);
    ecs_add_id(world, EcsOnDelete, EcsFinal);
    ecs_add_id(world, EcsOnDeleteObject, EcsFinal);


    /* Define triggers for when relationship cleanup rules are assigned */
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDelete, EcsWildcard)},
        .callback = register_on_delete,
        .events = {EcsOnAdd}
    });

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(EcsOnDeleteObject, EcsWildcard)},
        .callback = register_on_delete_object,
        .events = {EcsOnAdd}
    });

    /* Define trigger for when component lifecycle is set for component */
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_id(EcsComponentLifecycle)},
        .callback = on_set_component_lifecycle,
        .events = {EcsOnSet}
    });  

    /* Removal of ChildOf objects (parents) deletes the subject (child) */
    ecs_add_pair(world, EcsChildOf, EcsOnDeleteObject, EcsDelete);  

    /* Run bootstrap functions for other parts of the code */
    flecs_bootstrap_hierarchy(world);

    ecs_set_scope(world, 0);

    ecs_log_pop();
}


#define ECS_NAME_BUFFER_LENGTH (64)

static
bool path_append(
    const ecs_world_t *world, 
    ecs_entity_t parent, 
    ecs_entity_t child, 
    const char *sep,
    const char *prefix,
    ecs_strbuf_t *buf)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    ecs_entity_t cur = 0;
    char buff[22];
    const char *name;

    if (ecs_is_valid(world, child)) {
        cur = ecs_get_object(world, child, EcsChildOf, 0);
        if (cur) {
            if (cur != parent && cur != EcsFlecsCore) {
                path_append(world, parent, cur, sep, prefix, buf);
                ecs_strbuf_appendstr(buf, sep);
            }
        } else if (prefix) {
            ecs_strbuf_appendstr(buf, prefix);
        }

        name = ecs_get_name(world, child);
        if (!name) {
            ecs_os_sprintf(buff, "%u", (uint32_t)child);
            name = buff;
        }        
    } else {
        ecs_os_sprintf(buff, "%u", (uint32_t)child);
        name = buff;
    }

    ecs_strbuf_appendstr(buf, name);

    return cur != 0;
}

static
ecs_string_t get_string_key(
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_assert(!length || length == ecs_os_strlen(name), 
        ECS_INTERNAL_ERROR, NULL);

    if (!length) {
        length = ecs_os_strlen(name);
    }

    ecs_assert(!hash || hash == flecs_hash(name, length),
        ECS_INTERNAL_ERROR, NULL);

    if (!hash) {
        hash = flecs_hash(name, length);
    }

    return  (ecs_string_t) {
        .value = (char*)name,
        .length = length,
        .hash = hash
    };
}

static
ecs_entity_t find_by_name(
    const ecs_hashmap_t *map,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_string_t key = get_string_key(name, length, hash);

    ecs_entity_t *e = flecs_hashmap_get(*map, &key, ecs_entity_t);

    if (!e) {
        return 0;
    }

    return *e;
}

static
void register_by_name(
    ecs_hashmap_t *map,
    ecs_entity_t entity,
    const char *name,
    ecs_size_t length,
    uint64_t hash)
{
    ecs_assert(entity != 0, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);

    ecs_string_t key = get_string_key(name, length, hash);
    
    ecs_entity_t existing = find_by_name(map, name, key.length, key.hash);
    if (existing) {
        if (existing != entity) {
            ecs_abort(ECS_ALREADY_DEFINED, 
                "conflicting entity registered with name '%s'", name);
        }
    }

    key.value = ecs_os_strdup(key.value);

    flecs_hashmap_result_t hmr = flecs_hashmap_ensure(
        *map, &key, ecs_entity_t);

    *((ecs_entity_t*)hmr.value) = entity;
}

static
bool is_number(
    const char *name)
{
    ecs_assert(name != NULL, ECS_INTERNAL_ERROR, NULL);
    
    if (!isdigit(name[0])) {
        return false;
    }

    ecs_size_t i, length = ecs_os_strlen(name);
    for (i = 1; i < length; i ++) {
        char ch = name[i];

        if (!isdigit(ch)) {
            break;
        }
    }

    return i >= length;
}

static 
ecs_entity_t name_to_id(
    const char *name)
{
    long int result = atol(name);
    ecs_assert(result >= 0, ECS_INTERNAL_ERROR, NULL);
    return (ecs_entity_t)result;
}

static
ecs_entity_t get_builtin(
    const char *name)
{
    if (name[0] == '.' && name[1] == '\0') {
        return EcsThis;
    } else if (name[0] == '*' && name[1] == '\0') {
        return EcsWildcard;
    }

    return 0;
}

static
ecs_entity_t find_child_in_table(
    const ecs_table_t *table,
    const char *name)
{
    /* If table doesn't have names, then don't bother */
    int32_t name_index = ecs_type_index_of(table->type, 0,
        ecs_pair(ecs_id(EcsIdentifier), EcsName));
    if (name_index == -1) {
        return 0;
    }

    ecs_data_t *data = flecs_table_get_data(table);
    if (!data || !data->columns) {
        return 0;
    }

    int32_t i, count = ecs_vector_count(data->entities);
    if (!count) {
        return 0;
    }

    ecs_column_t *column = &data->columns[name_index];
    EcsIdentifier *names = ecs_vector_first(column->data, EcsIdentifier);

    if (is_number(name)) {
        return name_to_id(name);
    }

    for (i = 0; i < count; i ++) {
        const char *cur_name = names[i].value;
        if (cur_name && !strcmp(cur_name, name)) {
            return *ecs_vector_get(data->entities, ecs_entity_t, i);
        }
    }

    return 0;
}

static
bool is_sep(
    const char **ptr,
    const char *sep)
{
    ecs_size_t len = ecs_os_strlen(sep);

    if (!ecs_os_strncmp(*ptr, sep, len)) {
        *ptr += len;
        return true;
    } else {
        return false;
    }
}

static
const char* path_elem(
    const char *path,
    const char *sep,
    int32_t *len)
{
    const char *ptr;
    char ch;
    int32_t template_nesting = 0;
    int32_t count = 0;

    for (ptr = path; (ch = *ptr); ptr ++) {
        if (ch == '<') {
            template_nesting ++;
        } else if (ch == '>') {
            template_nesting --;
        }

        ecs_assert(template_nesting >= 0, ECS_INVALID_PARAMETER, path);

        if (!template_nesting && is_sep(&ptr, sep)) {
            break;
        }

        count ++;
    }

    if (len) {
        *len = count;
    }

    if (count) {
        return ptr;
    } else {
        return NULL;
    }
}

static
ecs_entity_t get_parent_from_path(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char **path_ptr,
    const char *prefix,
    bool new_entity)
{
    bool start_from_root = false;
    const char *path = *path_ptr;
   
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    if (prefix) {
        ecs_size_t len = ecs_os_strlen(prefix);
        if (!ecs_os_strncmp(path, prefix, len)) {
            path += len;
            parent = 0;
            start_from_root = true;
        }
    }

    if (!start_from_root && !parent && new_entity) {
        parent = ecs_get_scope(world);
    }

    *path_ptr = path;

    return parent;
}

static
void on_set_symbol(ecs_iter_t *it) {
    EcsIdentifier *n = ecs_term(it, EcsIdentifier, 1);
    ecs_world_t *world = it->world;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        register_by_name(
            &world->symbols, e, n[i].value, n[i].length, n[i].hash);
    }
}

static
uint64_t string_hash(
    const void *ptr)
{
    const ecs_string_t *str = ptr;
    ecs_assert(str->hash != 0, ECS_INVALID_PARAMETER, NULL);
    return str->hash;
}

static
int string_compare(
    const void *ptr1, 
    const void *ptr2)
{
    const ecs_string_t *str1 = ptr1;
    const ecs_string_t *str2 = ptr2;
    ecs_size_t len1 = str1->length;
    ecs_size_t len2 = str2->length;
    if (len1 != len2) {
        return (len1 > len2) - (len1 < len2);
    }

    return ecs_os_memcmp(str1->value, str2->value, len1);
}

ecs_hashmap_t flecs_string_hashmap_new(void) {
    return flecs_hashmap_new(ecs_string_t, ecs_entity_t, 
        string_hash, 
        string_compare);
}

void flecs_bootstrap_hierarchy(ecs_world_t *world) {
    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .term = {.id = ecs_pair(ecs_id(EcsIdentifier), EcsSymbol)},
        .callback = on_set_symbol,
        .events = {EcsOnSet}
    });
}


/* Public functions */

char* ecs_get_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    ecs_entity_t child,
    const char *sep,
    const char *prefix)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    if (!sep) {
        sep = ".";
    }
        
    ecs_strbuf_t buf = ECS_STRBUF_INIT;

    if (parent != child) {
        path_append(world, parent, child, sep, prefix, &buf);
    } else {
        ecs_strbuf_appendstr(&buf, "");
    }

    return ecs_strbuf_get(&buf);
}

ecs_entity_t ecs_lookup_child(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *name)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);
    ecs_entity_t result = 0;

    ecs_id_record_t *r = flecs_get_id_record(world, ecs_pair(EcsChildOf, parent));
    if (r && r->table_index) {        
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        ecs_table_record_t *tr;
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            result = find_child_in_table(tr->table, name);
            if (result) {
                return result;
            }            
        }
    }

    return result;
}

ecs_entity_t ecs_lookup(
    const ecs_world_t *world,
    const char *name)
{   
    if (!name) {
        return 0;
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = get_builtin(name);
    if (e) {
        return e;
    }

    if (is_number(name)) {
        return name_to_id(name);
    }

    e = find_by_name(&world->aliases, name, 0, 0);
    if (e) {
        return e;
    }    
    
    return ecs_lookup_child(world, 0, name);
}

ecs_entity_t ecs_lookup_symbol(
    const ecs_world_t *world,
    const char *name,
    bool lookup_as_path)
{   
    if (!name) {
        return 0;
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = find_by_name(&world->symbols, name, 0, 0);
    if (e) {
        return e;
    }

    if (lookup_as_path) {
        return ecs_lookup_fullpath(world, name);
    }

    return 0;
}

ecs_entity_t ecs_lookup_path_w_sep(
    const ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix,
    bool recursive)
{
    if (!path) {
        return 0;
    }

    if (!sep) {
        sep = ".";
    }

    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    ecs_entity_t e = get_builtin(path);
    if (e) {
        return e;
    }

    e = find_by_name(&world->aliases, path, 0, 0);
    if (e) {
        return e;
    }      

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr, *ptr_start;
    char *elem = buff;
    int32_t len, size = ECS_NAME_BUFFER_LENGTH;
    ecs_entity_t cur;
    bool core_searched = false;

    if (!sep) {
        sep = ".";
    }

    parent = get_parent_from_path(world, parent, &path, prefix, true);

retry:
    cur = parent;
    ptr_start = ptr = path;

    while ((ptr = path_elem(ptr, sep, &len))) {
        if (len < size) {
            ecs_os_memcpy(elem, ptr_start, len);
        } else {
            if (size == ECS_NAME_BUFFER_LENGTH) {
                elem = NULL;
            }

            elem = ecs_os_realloc(elem, len + 1);
            ecs_os_memcpy(elem, ptr_start, len);
            size = len + 1;
        }

        elem[len] = '\0';
        ptr_start = ptr;

        cur = ecs_lookup_child(world, cur, elem);
        if (!cur) {
            goto tail;
        }
    }

tail:
    if (!cur && recursive) {
        if (!core_searched) {
            if (parent) {
                parent = ecs_get_object(world, parent, EcsChildOf, 0);
            } else {
                parent = EcsFlecsCore;
                core_searched = true;
            }
            goto retry;
        }
    }

    if (elem != buff) {
        ecs_os_free(elem);
    }

    return cur;
}

ecs_entity_t ecs_set_scope(
    ecs_world_t *world,
    ecs_entity_t scope)
{
    ecs_stage_t *stage = flecs_stage_from_world(&world);

    ecs_entity_t e = ecs_pair(EcsChildOf, scope);
    ecs_ids_t to_add = {
        .array = &e,
        .count = 1
    };

    ecs_entity_t cur = stage->scope;
    stage->scope = scope;

    if (scope) {
        stage->scope_table = flecs_table_traverse_add(
            world, &world->store.root, &to_add, NULL);
    } else {
        stage->scope_table = &world->store.root;
    }

    return cur;
}

ecs_entity_t ecs_get_scope(
    const ecs_world_t *world)
{
    const ecs_stage_t *stage = flecs_stage_from_readonly_world(world);
    return stage->scope;
}

int32_t ecs_get_child_count(
    const ecs_world_t *world,
    ecs_entity_t parent)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    world = ecs_get_world(world);

    int32_t count = 0;

    ecs_id_record_t *r = flecs_get_id_record(world, ecs_pair(EcsChildOf, parent));
    if (r && r->table_index) {
        ecs_map_iter_t it = ecs_map_iter(r->table_index);
        ecs_table_record_t *tr;
        while ((tr = ecs_map_next(&it, ecs_table_record_t, NULL))) {
            count += ecs_table_count(tr->table);
        }
    }

    return count;
}

ecs_iter_t ecs_scope_iter_w_filter(
    ecs_world_t *iter_world,
    ecs_entity_t parent,
    ecs_filter_t *filter)
{
    ecs_assert(iter_world != NULL, ECS_INTERNAL_ERROR, NULL);
    const ecs_world_t *world = (ecs_world_t*)ecs_get_world(iter_world);
    ecs_iter_t it = {
        .world = iter_world
    };

    ecs_id_record_t *r = flecs_get_id_record(
        world, ecs_pair(EcsChildOf, parent));
    if (r && r->table_index) {
        it.iter.parent.tables = ecs_map_iter(r->table_index);
        it.table_count = ecs_map_count(r->table_index);
        if (filter) {
            it.iter.parent.filter = *filter;
        }
    }

    return it;
}

ecs_iter_t ecs_scope_iter(
    ecs_world_t *iter_world,
    ecs_entity_t parent)
{
    return ecs_scope_iter_w_filter(iter_world, parent, NULL);
}

bool ecs_scope_next(
    ecs_iter_t *it)
{
    ecs_scope_iter_t *iter = &it->iter.parent;
    ecs_map_iter_t *tables = &iter->tables;
    ecs_filter_t filter = iter->filter;
    ecs_table_record_t *tr;

    while ((tr = ecs_map_next(tables, ecs_table_record_t, NULL))) {
        ecs_table_t *table = tr->table;
        ecs_assert(table != NULL, ECS_INTERNAL_ERROR, NULL);

        iter->index ++;

        ecs_data_t *data = flecs_table_get_data(table);
        if (!data) {
            continue;
        }

        it->count = ecs_table_count(table);
        if (!it->count) {
            continue;
        }

        if (filter.include || filter.exclude) {
            if (!flecs_table_match_filter(it->world, table, &filter)) {
                continue;
            }
        }

        it->table = table;
        it->table_columns = data->columns;
        it->count = ecs_table_count(table);
        it->entities = ecs_vector_first(data->entities, ecs_entity_t);
        it->is_valid = true;

        goto yield;
    }

    it->is_valid = false;
    return false;

yield:
    it->is_valid = true;
    return true;   
}

const char* ecs_set_name_prefix(
    ecs_world_t *world,
    const char *prefix)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_assert(world->magic == ECS_WORLD_MAGIC, ECS_INTERNAL_ERROR, NULL);

    const char *old_prefix = world->name_prefix;
    world->name_prefix = prefix;
    return old_prefix;
}

ecs_entity_t ecs_add_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t entity,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    ecs_assert(world != NULL, ECS_INTERNAL_ERROR, NULL);

    if (!sep) {
        sep = ".";
    }    

    if (!path) {
        if (!entity) {
            entity = ecs_new_id(world);
        }

        if (parent) {
            ecs_add_pair(world, entity, EcsChildOf, entity);
        }

        return entity;
    }

    char buff[ECS_NAME_BUFFER_LENGTH];
    const char *ptr = path;
    const char *ptr_start = path;
    char *elem = buff;
    int32_t len, size = ECS_NAME_BUFFER_LENGTH;

    parent = get_parent_from_path(world, parent, &path, prefix, entity == 0);

    ecs_entity_t cur = parent;

    char *name = NULL;

    while ((ptr = path_elem(ptr, sep, &len))) {
        if (len < size) {
            ecs_os_memcpy(elem, ptr_start, len);
        } else {
            if (size == ECS_NAME_BUFFER_LENGTH) {
                elem = NULL;
            }

            elem = ecs_os_realloc(elem, len + 1);
            ecs_os_memcpy(elem, ptr_start, len);
            size = len + 1;          
        }

        elem[len] = '\0';
        ptr_start = ptr;

        ecs_entity_t e = ecs_lookup_child(world, cur, elem);
        if (!e) {
            if (name) {
                ecs_os_free(name);
            }

            name = ecs_os_strdup(elem);

            /* If this is the last entity in the path, use the provided id */
            if (entity && !path_elem(ptr, sep, NULL)) {
                e = entity;
            }

            if (!e) {
                e = ecs_new_id(world);
            }

            ecs_set_name(world, e, name);

            if (cur) {
                ecs_add_pair(world, e, EcsChildOf, cur);
            }
        }

        cur = e;
    }

    if (entity && (cur != entity)) {
        if (name) {
            ecs_os_free(name);
        }

        name = ecs_os_strdup(elem);

        ecs_set_name(world, entity, name);
    }

    if (name) {
        ecs_os_free(name);
    }

    if (elem != buff) {
        ecs_os_free(elem);
    }

    return cur;
}

ecs_entity_t ecs_new_from_path_w_sep(
    ecs_world_t *world,
    ecs_entity_t parent,
    const char *path,
    const char *sep,
    const char *prefix)
{
    if (!sep) {
        sep = ".";
    }

    return ecs_add_path_w_sep(world, 0, parent, path, sep, prefix);
}

void ecs_use(
    ecs_world_t *world,
    ecs_entity_t entity,
    const char *name)
{
    register_by_name(&world->aliases, entity, name, 0, 0);
}
