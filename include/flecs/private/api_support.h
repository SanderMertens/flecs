#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

#include "api_types.h"

/** Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests, but that may change between minor / patch 
 * releases. */

#ifdef __cplusplus
extern "C" {
#endif

/** This reserves entity ids for components. Regular entity ids will start after
 * this constant. This affects performance of table traversal, as edges with ids 
 * lower than this constant are looked up in an array, whereas constants higher
 * than this id are looked up in a map. Increasing this value can improve
 * performance at the cost of (significantly) higher memory usage. */
#define ECS_HI_COMPONENT_ID (256) /* Maximum number of components */


////////////////////////////////////////////////////////////////////////////////
//// Global type handles
////////////////////////////////////////////////////////////////////////////////

/** Type handles to builtin components */
FLECS_EXPORT
extern ecs_type_t 
    ecs_type(EcsComponent),
    ecs_type(EcsComponentLifecycle),
    ecs_type(EcsType),
    ecs_type(EcsName);

/** This allows passing 0 as type to functions that accept types */
#define FLECS__TNULL 0
#define FLECS__T0 0


////////////////////////////////////////////////////////////////////////////////
//// Functions used in declarative (macro) API
////////////////////////////////////////////////////////////////////////////////

FLECS_EXPORT
ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *id,
    const char *components);

FLECS_EXPORT
ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *id,
    size_t size,
    size_t alignment);

FLECS_EXPORT
ecs_entity_t ecs_new_module(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    size_t size,
    size_t alignment);

FLECS_EXPORT
ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *id,
    const char *components);

FLECS_EXPORT
ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *id,
    const char *sig);

FLECS_EXPORT
ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t phase,
    const char *signature,
    ecs_iter_action_t action);

FLECS_EXPORT
ecs_entity_t ecs_new_trigger(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t kind,
    const char *component,
    ecs_iter_action_t action);

FLECS_EXPORT
ecs_entity_t ecs_new_pipeline(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr);

FLECS_EXPORT
char* ecs_module_path_from_c(
    const char *c_name);


////////////////////////////////////////////////////////////////////////////////
//// Signature API
////////////////////////////////////////////////////////////////////////////////

typedef enum ecs_sig_inout_kind_t {
    EcsInOut,
    EcsIn,
    EcsOut
} ecs_sig_inout_kind_t;

/** Type that is used by systems to indicate where to fetch a component from */
typedef enum ecs_sig_from_kind_t {
    EcsFromAny,            /* Get component from self (default) */
    EcsFromOwned,           /* Get owned component from self */
    EcsFromShared,          /* Get shared component from self */
    EcsFromParent,          /* Get component from container */
    EcsFromSystem,          /* Get component from system */
    EcsFromEmpty,           /* Get entity handle by id */
    EcsFromEntity,          /* Get component from other entity */
    EcsCascade              /* Walk component in cascading (hierarchy) order */
} ecs_sig_from_kind_t;

/** Type describing an operator used in an signature of a system signature */
typedef enum ecs_sig_oper_kind_t {
    EcsOperAnd = 0,
    EcsOperOr = 1,
    EcsOperNot = 2,
    EcsOperOptional = 3,
    EcsOperLast = 4
} ecs_sig_oper_kind_t;

/** Type that describes a single column in the system signature */
typedef struct ecs_sig_column_t {
    ecs_sig_from_kind_t from_kind;        /* Element kind (Entity, Component) */
    ecs_sig_oper_kind_t oper_kind;   /* Operator kind (AND, OR, NOT) */
    ecs_sig_inout_kind_t inout_kind; /* Is component read or written */
    union {
        ecs_vector_t *type;          /* Used for OR operator */
        ecs_entity_t component;      /* Used for AND operator */
    } is;
    ecs_entity_t source;             /* Source entity (used with FromEntity) */
} ecs_sig_column_t;

/** Type that stores a parsed signature */
typedef struct ecs_sig_t {
    const char *name;           /* Optional name used for debugging */
    char *expr;                 /* Original expression string */
    ecs_vector_t *columns;      /* Columns that contain parsed data */
} ecs_sig_t;

/** Add column to signature. */
FLECS_EXPORT
int ecs_sig_add(
    ecs_sig_t *sig,
    ecs_sig_from_kind_t from_kind,
    ecs_sig_oper_kind_t oper_kind,
    ecs_sig_inout_kind_t access_kind,
    ecs_entity_t component,
    ecs_entity_t source);

/** Create query based on signature object. */
FLECS_EXPORT
ecs_query_t* ecs_query_new_w_sig(
    ecs_world_t *world,
    ecs_entity_t system,
    ecs_sig_t *sig);


#define ECS_INVALID_HANDLE (1)
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

/** Declare type variable */
#define ECS_TYPE_VAR(type)\
    ecs_type_t ecs_type(type)

/** Declare entity variable */
#define ECS_ENTITY_VAR(type)\
    ecs_entity_t ecs_entity(type)

/** Utility macro for setting a component in a module function */
#define ECS_SET_COMPONENT(type)\
    if (handles) handles->ecs_entity(type) = ecs_entity(type);\
    if (handles) handles->ecs_type(type) = ecs_type(type)

/** Utility macro for setting a system in a module function */
#define ECS_SET_ENTITY(entity)\
    if (handles) handles->entity = entity;

/** Calculate offset from address */
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))

#ifdef __cplusplus
}
#endif

#endif
