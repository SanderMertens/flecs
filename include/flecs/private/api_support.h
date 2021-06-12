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

/** Type handles to builtin components */
FLECS_API
extern ecs_type_t 
    ecs_type(EcsComponent),
    ecs_type(EcsComponentLifecycle),
    ecs_type(EcsType),
    ecs_type(EcsName);

/** This allows passing 0 as type to functions that accept types */
#define FLECS__TNULL 0
#define FLECS__T0 0
#define FLECS__E0 0

////////////////////////////////////////////////////////////////////////////////
//// Functions used in declarative (macro) API
////////////////////////////////////////////////////////////////////////////////

FLECS_API
ecs_entity_t ecs_new_module(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment);

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

#define ECS_INVALID_OPERATION (1)
#define ECS_INVALID_PARAMETER (2)
#define ECS_INVALID_DELETE (3)
#define ECS_OUT_OF_MEMORY (4)
#define ECS_OUT_OF_RANGE (5)
#define ECS_UNSUPPORTED (6)
#define ECS_INTERNAL_ERROR (7)
#define ECS_ALREADY_DEFINED (8)
#define ECS_MISSING_OS_API (9)
#define ECS_THREAD_ERROR (10)
#define ECS_CYCLE_DETECTED (11)

#define ECS_INCONSISTENT_NAME (20)
#define ECS_NAME_IN_USE (21)
#define ECS_NOT_A_COMPONENT (22)
#define ECS_INVALID_COMPONENT_SIZE (23)
#define ECS_INVALID_COMPONENT_ALIGNMENT (24)
#define ECS_COMPONENT_NOT_REGISTERED (25)
#define ECS_INCONSISTENT_COMPONENT_ID (26)
#define ECS_INCONSISTENT_COMPONENT_ACTION (27)
#define ECS_MODULE_UNDEFINED (28)

#define ECS_COLUMN_ACCESS_VIOLATION (40)
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (41)
#define ECS_COLUMN_IS_NOT_SHARED (42)
#define ECS_COLUMN_IS_SHARED (43)
#define ECS_COLUMN_HAS_NO_DATA (44)
#define ECS_COLUMN_TYPE_MISMATCH (45)
#define ECS_NO_OUT_COLUMNS (46)

#define ECS_TYPE_NOT_AN_ENTITY (60)
#define ECS_TYPE_CONSTRAINT_VIOLATION (61)
#define ECS_TYPE_INVALID_CASE (62)

#define ECS_INVALID_WHILE_ITERATING (70)
#define ECS_LOCKED_STORAGE (71)
#define ECS_INVALID_FROM_WORKER (72)

#define ECS_DESERIALIZE_FORMAT_ERROR (80)


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
