/**
 * @file api_support.h
 * @brief Support functions and constants.
 *
 * Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests, but that may change between minor / patch 
 * releases. 
 */

#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

#include "api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** This reserves entity ids for components. Regular entity ids will start after
 * this constant. This affects performance of table traversal, as edges with ids 
 * lower than this constant are looked up in an array, whereas constants higher
 * than this id are looked up in a map. Increasing this value can improve
 * performance at the cost of (significantly) higher memory usage. */
#define ECS_HI_COMPONENT_ID (256) /* Maximum number of components */

/** The maximum number of nested function calls before the core will throw a
 * cycle detected error */
#define ECS_MAX_RECURSION (512)

/** Maximum length of a parser token (used by parser-related addons) */
#define ECS_MAX_TOKEN_SIZE (256)

////////////////////////////////////////////////////////////////////////////////
//// Global type handles
////////////////////////////////////////////////////////////////////////////////

/** This allows passing 0 as type to functions that accept ids */
#define FLECS__E0 0

////////////////////////////////////////////////////////////////////////////////
//// Functions used in declarative (macro) API
////////////////////////////////////////////////////////////////////////////////

FLECS_API
char* ecs_module_path_from_c(
    const char *c_name);

////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

bool ecs_identifier_is_0(
    const char *id);

////////////////////////////////////////////////////////////////////////////////
//// Ctor that initializes component to 0
////////////////////////////////////////////////////////////////////////////////

/* Constructor that zeromem's a component value */
FLECS_API
void ecs_default_ctor(
    void *ptr, 
    int32_t count, 
    const ecs_type_info_t *ctx);

/* Increase refcount of table (prevents deletion) */
FLECS_API
void flecs_table_claim(
    ecs_world_t *world, 
    ecs_table_t *table);

/* Decreases refcount of table (may delete) */
FLECS_API
bool flecs_table_release(
    ecs_world_t *world, 
    ecs_table_t *table);

/** Calculate offset from address */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif
#define ECS_OFFSET_T(o, T) ECS_OFFSET(o, ECS_SIZEOF(T))

#define ECS_ELEM(ptr, size, index) ECS_OFFSET(ptr, (size) * (index))
#define ECS_ELEM_T(o, T, index) ECS_ELEM(o, ECS_SIZEOF(T), index)

/** Enable/disable bitsets */
#define ECS_BIT_SET(flags, bit) (flags) |= (bit)
#define ECS_BIT_CLEAR(flags, bit) (flags) &= ~(bit) 
#define ECS_BIT_COND(flags, bit, cond) ((cond) \
    ? (ECS_BIT_SET(flags, bit)) \
    : (ECS_BIT_CLEAR(flags, bit)))
#define ECS_BIT_IS_SET(flags, bit) ((flags) & (bit))

#ifdef __cplusplus
}
#endif

#endif
