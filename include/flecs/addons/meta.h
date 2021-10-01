/**
 * @file meta.h
 * @brief Meta addon.
 *
 * The meta addon enables reflecting on component data.
 */

#ifdef FLECS_META

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#include "../addons/module.h"

#ifndef FLECS_META_H
#define FLECS_META_H

#ifdef __cplusplus
extern "C" {
#endif

#define ECS_MEMBER_DESC_CACHE_SIZE (16)

/** Primitive type definitions */
typedef bool ecs_bool_t;
typedef char ecs_char_t;
typedef unsigned char ecs_byte_t;
typedef uint8_t ecs_u8_t;
typedef uint16_t ecs_u16_t;
typedef uint32_t ecs_u32_t;
typedef uint64_t ecs_u64_t;
typedef uintptr_t ecs_uptr_t;
typedef int8_t ecs_i8_t;
typedef int16_t ecs_i16_t;
typedef int32_t ecs_i32_t;
typedef int64_t ecs_i64_t;
typedef intptr_t ecs_iptr_t;
typedef float ecs_f32_t;
typedef double ecs_f64_t;
typedef char* ecs_string_t;

/** Meta module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsMetaType);
FLECS_API extern const ecs_entity_t ecs_id(EcsMetaTypeSerialized);
FLECS_API extern const ecs_entity_t ecs_id(EcsPrimitive);
FLECS_API extern const ecs_entity_t ecs_id(EcsEnum);
FLECS_API extern const ecs_entity_t ecs_id(EcsBitmask);
FLECS_API extern const ecs_entity_t ecs_id(EcsMember);
FLECS_API extern const ecs_entity_t ecs_id(EcsStruct);
FLECS_API extern const ecs_entity_t ecs_id(EcsArray);
FLECS_API extern const ecs_entity_t ecs_id(EcsVector);
FLECS_API extern const ecs_entity_t EcsConstant;

/** Primitive type component ids */
FLECS_API extern const ecs_entity_t ecs_id(ecs_bool_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_char_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_byte_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u8_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u16_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u32_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_u64_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_uptr_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i8_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i16_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i32_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_i64_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_iptr_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_f32_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_f64_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_string_t);
FLECS_API extern const ecs_entity_t ecs_id(ecs_entity_t);

typedef enum ecs_type_kind_t {
    EcsPrimitiveType,
    EcsBitmaskType,
    EcsEnumType,
    EcsStructType,
    EcsArrayType,
    EcsVectorType
} ecs_type_kind_t;

typedef struct EcsMetaType {
    ecs_type_kind_t kind;
} EcsMetaType;

typedef enum ecs_primitive_kind_t {
    EcsBool = 1,
    EcsChar,
    EcsByte,
    EcsU8,
    EcsU16,
    EcsU32,
    EcsU64,
    EcsI8,
    EcsI16,
    EcsI32,
    EcsI64,
    EcsF32,
    EcsF64,
    EcsUPtr,
    EcsIPtr,
    EcsString,
    EcsEntity
} ecs_primitive_kind_t;

typedef struct EcsPrimitive {
    ecs_primitive_kind_t kind;
} EcsPrimitive;

typedef struct EcsMember {
    ecs_entity_t type;
    int32_t count;
} EcsMember;

/* Element type of members vector in EcsStruct */
typedef struct ecs_member_t {
    /* Must be set when used with ecs_struct_desc_t */
    char *name;
    ecs_entity_t type;

    /* May be set when used with ecs_struct_desc_t */
    int32_t count;
    int32_t offset;

    /* Should not be set by ecs_struct_desc_t */
    ecs_size_t size;
    ecs_entity_t member;
} ecs_member_t;

typedef struct EcsStruct {
    /* Populated from child entities with Member component */
    ecs_vector_t *members; /* vector<ecs_member_t> */
} EcsStruct;

typedef struct ecs_enum_constant_t {
    /* Must be set when used with ecs_enum_desc_t */
    char *name;

    /* May be set when used with ecs_enum_desc_t */
    int32_t value;

    /* Should not be set by ecs_enum_desc_t */
    ecs_entity_t constant;
} ecs_enum_constant_t;

typedef struct EcsEnum {
    /* Populated from child entities with Constant component */
    ecs_map_t *constants;
} EcsEnum;

typedef struct ecs_bitmask_constant_t {
    /* Must be set when used with ecs_bitmask_desc_t */
    char *name;

    /* May be set when used with ecs_bitmask_desc_t */
    ecs_flags32_t value;

    /* Should not be set by ecs_bitmask_desc_t */
    ecs_entity_t constant;
} ecs_bitmask_constant_t;

typedef struct EcsBitmask {
    /* Populated from child entities with Constant component */
    ecs_map_t *constants;
} EcsBitmask;

typedef struct EcsArray {
    ecs_entity_t type;
    int32_t count;
} EcsArray;

typedef struct EcsVector {
    ecs_entity_t type;
} EcsVector;


/** Serializer utilities */

typedef enum ecs_meta_type_op_kind_t {
    EcsOpEnum,
    EcsOpBitmask,
    EcsOpArray,
    EcsOpVector,
    EcsOpPush,
    EcsOpPop,

    EcsOpPrimitive,

    EcsOpBool,
    EcsOpChar,
    EcsOpByte,
    EcsOpU8,
    EcsOpU16,
    EcsOpU32,
    EcsOpU64,
    EcsOpI8,
    EcsOpI16,
    EcsOpI32,
    EcsOpI64,
    EcsOpF32,
    EcsOpF64,
    EcsOpUPtr,
    EcsOpIPtr,
    EcsOpString,
    EcsOpEntity
} ecs_meta_type_op_kind_t;

typedef struct ecs_meta_type_op_t {
    ecs_meta_type_op_kind_t kind;
    ecs_size_t offset;    /* Offset of current field */
    int32_t count;        
    int32_t op_count;     /* Number of operations until next field or end */
    const char *name;     /* Name of value (only used for struct members) */
    ecs_entity_t type;
} ecs_meta_type_op_t;

typedef struct EcsMetaTypeSerialized {
    ecs_vector_t* ops;
} EcsMetaTypeSerialized;


/** Deserializer utilities */

#define ECS_META_MAX_SCOPE_DEPTH (32) /* >32 levels of nesting is not sane */

typedef struct ecs_meta_scope_t {
    ecs_entity_t type;        /* The type being iterated */
    ecs_meta_type_op_t *ops;  /* The type operations (see ecs_meta_type_op_t) */
    int32_t op_count;         /* Number of operations in ops array to process */
    int32_t op_cur;           /* Current operation */
    int32_t elem_cur;         /* Current element (for collections) */
    void *ptr;                /* Pointer to the value being iterated */

    const EcsComponent *comp; /* Pointer to component, in case size/alignment is needed */
    ecs_vector_t **vector;    /* Current vector, in case a vector is iterated */
    bool is_collection;       /* Is the scope iterating elements? */
    bool is_inline_array;     /* Is the scope iterating an inline array? */
} ecs_meta_scope_t;

/** Type that enables iterating/populateing a value using reflection data */
typedef struct ecs_meta_cursor_t {
    const ecs_world_t *world;
    ecs_meta_scope_t scope[ECS_META_MAX_SCOPE_DEPTH];
    int32_t depth;
} ecs_meta_cursor_t;

FLECS_API
ecs_meta_cursor_t ecs_meta_cursor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr);

FLECS_API
void* ecs_meta_get_ptr(
    ecs_meta_cursor_t *cursor);

FLECS_API
int ecs_meta_next(
    ecs_meta_cursor_t *cursor);

FLECS_API
int ecs_meta_move(
    ecs_meta_cursor_t *cursor,
    int32_t pos);

FLECS_API
int ecs_meta_move_name(
    ecs_meta_cursor_t *cursor,
    const char *name);

FLECS_API
int ecs_meta_push(
    ecs_meta_cursor_t *cursor);

FLECS_API
int ecs_meta_pop(
    ecs_meta_cursor_t *cursor);

FLECS_API
int ecs_meta_set_bool(
    ecs_meta_cursor_t *cursor,
    bool value);

FLECS_API
int ecs_meta_set_char(
    ecs_meta_cursor_t *cursor,
    char value);

FLECS_API
int ecs_meta_set_int(
    ecs_meta_cursor_t *cursor,
    int64_t value);

FLECS_API
int ecs_meta_set_uint(
    ecs_meta_cursor_t *cursor,
    uint64_t value);

FLECS_API
int ecs_meta_set_float(
    ecs_meta_cursor_t *cursor,
    double value);

FLECS_API
int ecs_meta_set_string(
    ecs_meta_cursor_t *cursor,
    const char *value);

FLECS_API
int ecs_meta_set_entity(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value);

FLECS_API
int ecs_meta_set_null(
    ecs_meta_cursor_t *cursor);


/** API functions for creating meta types */

/** Used with ecs_enum_init. */
typedef struct ecs_enum_desc_t {
    ecs_entity_desc_t entity;
    ecs_enum_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_enum_desc_t;

/** Create a new enum type */
FLECS_API
ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc);


/** Used with ecs_bitmask_init. */
typedef struct ecs_bitmask_desc_t {
    ecs_entity_desc_t entity;
    ecs_bitmask_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_bitmask_desc_t;

/** Create a new enum type */
FLECS_API
ecs_entity_t ecs_bitmask_init(
    ecs_world_t *world,
    const ecs_bitmask_desc_t *desc);


/** Used with ecs_array_init. */
typedef struct ecs_array_desc_t {
    ecs_entity_desc_t entity;
    ecs_entity_t type;
    int32_t count;
} ecs_array_desc_t;

/** Create a new array type */
FLECS_API
ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc);


/** Used with ecs_vector_init. */
typedef struct ecs_vector_desc_t {
    ecs_entity_desc_t entity;
    ecs_entity_t type;
} ecs_vector_desc_t;

/** Create a new vector type */
FLECS_API
ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc);


/** Used with ecs_struct_init. */
typedef struct ecs_struct_desc_t {
    ecs_entity_desc_t entity;
    ecs_member_t members[ECS_MEMBER_DESC_CACHE_SIZE];
} ecs_struct_desc_t;

/** Create a new struct type */
FLECS_API
ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc);


/* Module import boilerplate */

typedef struct FlecsMeta {
    int32_t dummy; 
} FlecsMeta;

FLECS_API
void FlecsMetaImport(
    ecs_world_t *world);

#define FlecsMetaImportHandles(handles)

#ifdef __cplusplus
}
#endif

#endif

#endif
