#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

/** Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests, but that may change between minor / patch 
 * releases. */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_column_t ecs_column_t;
typedef struct ecs_data_t ecs_data_t;

/** This reserves entity ids for components. Regular entity ids will start after
 * this constant. This affects performance of table traversal, as edges with ids 
 * lower than this constant are looked up in an array, whereas constants higher
 * than this id are looked up in a map. Increasing this value can improve
 * performance at the cost of (significantly) higher memory usage. */
#define ECS_HI_COMPONENT_ID (256) /* Maximum number of components */

////////////////////////////////////////////////////////////////////////////////
//// Private datatypes
////////////////////////////////////////////////////////////////////////////////

typedef enum ecs_blob_header_kind_t {
    EcsStreamHeader,

    /* Stream states */
    EcsTableSegment,
    EcsFooterSegment,

    /* Table segment */
    EcsTableHeader,
    EcsTableTypeSize,
    EcsTableType,
    EcsTableSize,
    EcsTableColumn,
    EcsTableColumnHeader,
    EcsTableColumnSize,
    EcsTableColumnData,

    /* Name column (EcsName) */
    EcsTableColumnNameHeader,
    EcsTableColumnNameLength,
    EcsTableColumnName,

    EcsStreamFooter  
} ecs_blob_header_kind_t;

typedef struct ecs_table_reader_t {
    ecs_blob_header_kind_t state;

    int32_t table_index;
    ecs_table_t *table;
    ecs_data_t *data;

    /* Current index in type */
    size_t type_written;
    ecs_type_t type;

    /* Current column */
    ecs_vector_t *column_vector;
    int32_t column_index;
    int32_t total_columns;

    /* Keep track of how much of the component column has been written */
    void *column_data;
    size_t column_size;
    size_t column_alignment;
    size_t column_written;

    /* Keep track of row when writing non-blittable data */
    int32_t row_index;
    int32_t row_count;

    /* Keep track of how much of an entity name has been written */
    const char *name;
    size_t name_len;
    size_t name_written;
} ecs_table_reader_t;

typedef struct ecs_reader_t {
    ecs_world_t *world;
    ecs_blob_header_kind_t state;
    ecs_sparse_t *tables;
    ecs_table_reader_t table;
} ecs_reader_t;

typedef struct ecs_name_writer_t {
    char *name;
    int32_t written;
    int32_t len;
    int32_t max_len;
} ecs_name_writer_t;

typedef struct ecs_table_writer_t {
    ecs_blob_header_kind_t state;

    ecs_table_t *table;
    ecs_vector_t *column_vector;

    /* Keep state for parsing type */
    int32_t type_count;
    int32_t type_max_count;
    size_t type_written;
    ecs_entity_t *type_array;
    
    int32_t column_index;
    size_t column_size;
    size_t column_alignment;
    size_t column_written;
    void *column_data;

    int32_t row_count;
    int32_t row_index;
    ecs_name_writer_t name; 
} ecs_table_writer_t;

typedef struct ecs_writer_t {
    ecs_world_t *world;
    ecs_blob_header_kind_t state;
    ecs_table_writer_t table;
    int error;
} ecs_writer_t;

////////////////////////////////////////////////////////////////////////////////
//// Error API
////////////////////////////////////////////////////////////////////////////////

#define ECS_TYPE_FLAG_MASK ((ecs_entity_t)(ECS_INSTANCEOF | ECS_CHILDOF | ECS_AND | ECS_OR | ECS_XOR | ECS_NOT))
#define ECS_ENTITY_MASK ((ecs_entity_t)~ECS_TYPE_FLAG_MASK)
#define ECS_TYPE_FLAG_START ECS_CHILDOF


////////////////////////////////////////////////////////////////////////////////
//// Deprecated names
////////////////////////////////////////////////////////////////////////////////

struct ecs_filter_t;
typedef struct ecs_filter_t ecs_type_filter_t;

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
    ecs_view_action_t action);

FLECS_EXPORT
ecs_entity_t ecs_new_trigger(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    ecs_entity_t kind,
    const char *component,
    ecs_view_action_t action,
    const void *ctx);

FLECS_EXPORT
ecs_entity_t ecs_new_pipeline(
    ecs_world_t *world,
    ecs_entity_t e,
    const char *name,
    const char *expr);

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
    EcsFromSelf,            /* Get component from self (default) */
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


////////////////////////////////////////////////////////////////////////////////
//// Error API
////////////////////////////////////////////////////////////////////////////////

/** Get description for error code */
FLECS_EXPORT
const char* ecs_strerror(
    int32_t error_code);

/** Abort */
FLECS_EXPORT
void _ecs_abort(
    int32_t error_code,
    const char *param,
    const char *file,
    int32_t line);

/** Assert */
FLECS_EXPORT
void _ecs_assert(
    bool condition,
    int32_t error_code,
    const char *param,
    const char *condition_str,
    const char *file,
    int32_t line);

/** Parse error */
void _ecs_parser_error(
    const char *name,
    const char *expr, 
    int column,
    const char *fmt,
    ...);

#define ecs_abort(error_code, param) _ecs_abort(error_code, param, __FILE__, __LINE__); abort()
#define ecs_parser_error(name, expr, column, ...) _ecs_parser_error(name, expr, column, __VA_ARGS__); abort()

#ifdef NDEBUG
#define ecs_assert(condition, error_code, param)
#else
#define ecs_assert(condition, error_code, param) _ecs_assert(condition, error_code, param, #condition, __FILE__, __LINE__); assert(condition)
#endif

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
