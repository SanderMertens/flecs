#ifndef FLECS_API_SUPPORT_H
#define FLECS_API_SUPPORT_H

/** Supporting types and functions that need to be exposed either in support of 
 * the public API or for unit tests, but that may change between minor / patch 
 * releases. */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_table_t ecs_table_t;
typedef struct ecs_table_column_t ecs_table_column_t;

////////////////////////////////////////////////////////////////////////////////
//// Private datatypes
////////////////////////////////////////////////////////////////////////////////

typedef enum ecs_blob_header_kind_t {
    EcsStreamHeader,

    /* Stream states */
    EcsComponentSegment,
    EcsTableSegment,
    EcsFooterSegment,

    /* Component segment */
    EcsComponentHeader,
    EcsComponentId,
    EcsComponentSize,
    EcsComponentNameLength,
    EcsComponentName,

    /* Table segment */
    EcsTableHeader,
    EcsTableTypeSize,
    EcsTableType,
    EcsTableSize,
    EcsTableColumn,
    EcsTableColumnHeader,
    EcsTableColumnSize,
    EcsTableColumnData,

    /* Name column (EcsId) */
    EcsTableColumnNameHeader,
    EcsTableColumnNameLength,
    EcsTableColumnName,

    EcsStreamFooter  
} ecs_blob_header_kind_t;

typedef struct ecs_component_reader_t {
    ecs_blob_header_kind_t state;

    /* Component data fetched from the world */
    ecs_entity_t *id_column;
    EcsComponent *data_column;
    EcsId *name_column;

    /* Current component & total number of components */
    int32_t index;
    int32_t count;

    /* Keep track how much of the component name has been written */
    const char *name;
    size_t len;
    size_t written;
} ecs_component_reader_t;

typedef struct ecs_table_reader_t {
    ecs_blob_header_kind_t state;

    uint32_t table_index;
    ecs_table_t *table;
    ecs_table_column_t *columns;

    /* Current index in type */
    int32_t type_written;
    ecs_type_t type;

    /* Current column */
    ecs_table_column_t *column;
    int32_t column_index;
    int32_t total_columns;

    /* Keep track of how much of the component column has been written */
    void *column_data;
    size_t column_size;
    size_t column_written;

    /* Keep track of row when writing non-blittable data */
    int32_t row_index;
    uint32_t row_count;

    /* Keep track of how much of an entity name has been written */
    const char *name;
    size_t name_len;
    size_t name_written;
} ecs_table_reader_t;

typedef struct ecs_reader_t {
    ecs_world_t *world;
    ecs_blob_header_kind_t state;
    ecs_chunked_t *tables;
    ecs_component_reader_t component;
    ecs_table_reader_t table;
} ecs_reader_t;

typedef struct ecs_name_writer_t {
    char *name;
    int32_t written;
    int32_t len;
    int32_t max_len;
} ecs_name_writer_t;

typedef struct ecs_component_writer_t {
    ecs_blob_header_kind_t state;

    int32_t id;
    size_t size;
    ecs_name_writer_t name;
} ecs_component_writer_t;

typedef struct ecs_table_writer_t {
    ecs_blob_header_kind_t state;

    ecs_table_t *table;
    ecs_table_column_t *column;

    /* Keep state for parsing type */
    uint32_t type_count;
    uint32_t type_max_count;
    uint32_t type_written;
    ecs_entity_t *type_array;
    
    uint32_t column_index;
    uint32_t column_size;
    uint32_t column_written;
    void *column_data;

    uint32_t row_count;
    int32_t row_index;
    ecs_name_writer_t name; 
} ecs_table_writer_t;

typedef struct ecs_writer_t {
    ecs_world_t *world;
    ecs_blob_header_kind_t state;
    ecs_component_writer_t component;
    ecs_table_writer_t table;
    int error;
} ecs_writer_t;

////////////////////////////////////////////////////////////////////////////////
//// Error API
////////////////////////////////////////////////////////////////////////////////

#define ECS_ENTITY_FLAGS_MASK ((ecs_entity_t)(ECS_INSTANCEOF | ECS_CHILDOF))
#define ECS_ENTITY_MASK ((ecs_entity_t)~ECS_ENTITY_FLAGS_MASK)


////////////////////////////////////////////////////////////////////////////////
//// Deprecated names
////////////////////////////////////////////////////////////////////////////////

#define ECS_SINGLETON ((ecs_entity_t)(ECS_ENTITY_MASK) - 1)
#define ECS_INVALID_ENTITY (0)

struct ecs_filter_t;
typedef struct ecs_filter_t ecs_type_filter_t;


////////////////////////////////////////////////////////////////////////////////
//// Deprecated functions / function wrappers
////////////////////////////////////////////////////////////////////////////////

#define ecs_get_singleton(world, type)\
    (*(type*)_ecs_get_ptr(world, EcsSingleton, T##type))

#define ecs_get_singleton_ptr(world, type)\
    (type*)_ecs_get_ptr(world, EcsSingleton, T##type)

#ifndef __BAKE_LEGACY__
#define ecs_set_singleton(world, component, ...)\
    _ecs_set_ptr(world, EcsSingleton, ecs_entity(component), sizeof(component), &(component)__VA_ARGS__)
#endif

#define ecs_set_singleton_ptr(world, component, ptr)\
    _ecs_set_ptr(world, EcsSingleton, ecs_entity(component), sizeof(component), ptr)

#define ecs_is_empty(world, entity) (ecs_get_type(world, entity) == NULL)


////////////////////////////////////////////////////////////////////////////////
//// Functions used in declarative (macro) API
////////////////////////////////////////////////////////////////////////////////

FLECS_EXPORT
ecs_entity_t ecs_new_entity(
    ecs_world_t *world,
    const char *id,
    const char *components);

FLECS_EXPORT
ecs_entity_t ecs_new_component(
    ecs_world_t *world,
    const char *id,
    size_t size);

FLECS_EXPORT
ecs_entity_t ecs_new_system(
    ecs_world_t *world,
    const char *id,
    EcsSystemKind kind,
    const char *sig,
    ecs_system_action_t action);

FLECS_EXPORT
ecs_entity_t ecs_new_type(
    ecs_world_t *world,
    const char *id,
    const char *components);

FLECS_EXPORT
ecs_entity_t ecs_new_prefab(
    ecs_world_t *world,
    const char *id,
    const char *sig);


////////////////////////////////////////////////////////////////////////////////
//// Error API
////////////////////////////////////////////////////////////////////////////////

/** Get description for error code */
FLECS_EXPORT
const char* ecs_strerror(
    uint32_t error_code);

/** Abort */
FLECS_EXPORT
void _ecs_abort(
    uint32_t error_code,
    const char *param,
    const char *file,
    uint32_t line);

/** Assert */
FLECS_EXPORT
void _ecs_assert(
    bool condition,
    uint32_t error_code,
    const char *param,
    const char *condition_str,
    const char *file,
    uint32_t line);

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
#define ECS_OUT_OF_MEMORY (16)
#define ECS_MODULE_UNDEFINED (17)
#define ECS_COLUMN_INDEX_OUT_OF_RANGE (18)
#define ECS_COLUMN_IS_NOT_SHARED (19)
#define ECS_COLUMN_IS_SHARED (20)
#define ECS_COLUMN_HAS_NO_DATA (21)
#define ECS_COLUMN_TYPE_MISMATCH (22)
#define ECS_INVALID_WHILE_MERGING (23)
#define ECS_INVALID_WHILE_ITERATING (24)
#define ECS_INVALID_FROM_WORKER (25)
#define ECS_UNRESOLVED_IDENTIFIER (26)
#define ECS_OUT_OF_RANGE (27)
#define ECS_COLUMN_IS_NOT_SET (28)
#define ECS_UNRESOLVED_REFERENCE (29)
#define ECS_THREAD_ERROR (30)
#define ECS_MISSING_OS_API (31)
#define ECS_TYPE_TOO_LARGE (32)
#define ECS_INVALID_PREFAB_CHILD_TYPE (33)
#define ECS_UNSUPPORTED (34)
#define ECS_NO_OUT_COLUMNS (35)
#define ECS_COLUMN_ACCESS_VIOLATION (36)
#define ECS_DESERIALIZE_COMPONENT_ID_CONFLICT (37)
#define ECS_DESERIALIZE_COMPONENT_SIZE_CONFLICT (38)
#define ECS_DESERIALIZE_FORMAT_ERROR (39)

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
    if (handles) handles->entity = entity;\
    if (handles) handles->ecs_type(entity) = ecs_type(entity)

/** Calculate offset from address */
#define ECS_OFFSET(o, offset) (void*)(((uintptr_t)(o)) + ((uintptr_t)(offset)))

#ifdef __cplusplus
}
#endif

#endif
