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

FLECS_API
bool ecs_component_has_actions(
    const ecs_world_t *world,
    ecs_entity_t component);

FLECS_API
void ecs_add_module_tag(
    ecs_world_t *world,
    ecs_entity_t module);

////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

bool ecs_identifier_is_0(
    const char *id);

bool ecs_identifier_is_var(
    const char *id);

/** Calculate offset from address */
#ifdef __cplusplus
#define ECS_OFFSET(o, offset) reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(o)) + (static_cast<uintptr_t>(offset)))
#else
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))
#endif

#ifdef __cplusplus
}
#endif

#endif
