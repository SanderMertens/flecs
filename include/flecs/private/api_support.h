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
ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t phase,
    const char *signature,
    ecs_iter_action_t action);

FLECS_API
ecs_entity_t ecs_new_pipeline(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr);

FLECS_API
char* ecs_module_path_from_c(
    const char *c_name);

FLECS_API
bool ecs_component_has_actions(
    const ecs_world_t *world,
    ecs_entity_t component);

////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

bool ecs_identifier_is_0(
    const char *id);

bool ecs_identifier_is_var(
    const char *id);

/** Get filter from query */
FLECS_API
ecs_filter_t* ecs_query_get_filter(
    ecs_query_t *query);


#define ECS_INVALID_ENTITY (1)
#define ECS_INVALID_PARAMETER (2)
#define ECS_INVALID_COMPONENT_ID (3)
#define ECS_INVALID_EXPRESSION (4)
#define ECS_INVALID_TYPE_EXPRESSION (5)
#define ECS_INVALID_SIGNATURE (6)
#define ECS_UNKNOWN_COMPONENT_ID (7)
#define ECS_UNKNOWN_TYPE_ID (8)
#define ECS_TYPE_NOT_AN_ENTITY (9)
#define ECS_MISSING_SYSTEM_CONTEXT (10)
#define ECS_NOT_A_COMPONENT (11)
#define ECS_INTERNAL_ERROR (12)
#define ECS_MORE_THAN_ONE_PREFAB (13)
#define ECS_ALREADY_DEFINED (14)
#define ECS_INVALID_COMPONENT_SIZE (15)
#define ECS_INVALID_COMPONENT_ALIGNMENT (16)
#define ECS_OUT_OF_MEMORY (17)
#define ECS_MODULE_UNDEFINED (18)
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (19)
#define ECS_COLUMN_IS_NOT_SHARED (20)
#define ECS_COLUMN_IS_SHARED (21)
#define ECS_COLUMN_HAS_NO_DATA (22)
#define ECS_COLUMN_TYPE_MISMATCH (23)
#define ECS_INVALID_WHILE_MERGING (24)
#define ECS_INVALID_WHILE_ITERATING (25)
#define ECS_INVALID_FROM_WORKER (26)
#define ECS_UNRESOLVED_IDENTIFIER (27)
#define ECS_OUT_OF_RANGE (28)
#define ECS_COLUMN_IS_NOT_SET (29)
#define ECS_UNRESOLVED_REFERENCE (30)
#define ECS_THREAD_ERROR (31)
#define ECS_MISSING_OS_API (32)
#define ECS_TYPE_TOO_LARGE (33)
#define ECS_INVALID_PREFAB_CHILD_TYPE (34)
#define ECS_UNSUPPORTED (35)
#define ECS_NO_OUT_COLUMNS (36)
#define ECS_COLUMN_ACCESS_VIOLATION (37)
#define ECS_DESERIALIZE_COMPONENT_ID_CONFLICT (38)
#define ECS_DESERIALIZE_COMPONENT_SIZE_CONFLICT (39)
#define ECS_DESERIALIZE_FORMAT_ERROR (40)
#define ECS_INVALID_REACTIVE_SIGNATURE (41)
#define ECS_INCONSISTENT_COMPONENT_NAME (42)
#define ECS_TYPE_CONSTRAINT_VIOLATION (43)
#define ECS_COMPONENT_NOT_REGISTERED (44)
#define ECS_INCONSISTENT_COMPONENT_ID (45)
#define ECS_INVALID_CASE (46)
#define ECS_COMPONENT_NAME_IN_USE (47)
#define ECS_INCONSISTENT_NAME (48)
#define ECS_INCONSISTENT_COMPONENT_ACTION (49)
#define ECS_INVALID_OPERATION (50)
#define ECS_INVALID_DELETE (51)
#define ECS_CYCLE_DETECTED (52)

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
