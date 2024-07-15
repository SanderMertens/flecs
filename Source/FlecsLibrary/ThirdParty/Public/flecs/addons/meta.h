/**
 * @file addons/meta.h
 * @brief Meta addon.
 *
 * The meta addon enables reflecting on component data. Types are stored as
 * entities, with components that store the reflection data. A type has at least
 * two components:
 *
 * - EcsComponent: core component, contains size & alignment
 * - EcsType:  component that indicates what kind of type the entity is
 *
 * Additionally the type may have an additional component that contains the
 * reflection data for the type. For example, structs have these components:
 *
 * - EcsComponent
 * - EcsType
 * - EcsStruct
 *
 * Structs can be populated by adding child entities with the EcsMember
 * component. Adding a child with a Member component to an entity will
 * automatically add the EcsStruct component to the parent.
 *
 * Enums/bitmasks can be populated by adding child entities with the Constant
 * tag. By default constants are automatically assigned values when they are
 * added to the enum/bitmask. The parent entity must have the EcsEnum or
 * EcsBitmask component before adding the constants.
 *
 * To create enum constants with a manual value, set (Constant, i32) to the
 * desired value. To create bitmask constants with a manual value, set
 * (Constant, u32) to the desired value. Constants with manual values should not
 * conflict with other constants.
 *
 * The _init APIs are convenience wrappers around creating the entities and
 * components for the types.
 *
 * When a type is created it automatically receives the EcsComponent and
 * EcsType components. The former means that the resulting type can be
 * used as a regular component:
 *
 * @code
 * // Create Position type
 * ecs_entity_t pos = ecs_struct_init(world, &(ecs_struct_desc_t){
 *  .entity.name = "Position",
 *  .members = {
 *       {"x", ecs_id(ecs_f32_t)},
 *       {"y", ecs_id(ecs_f32_t)}
 *   }
 * });
 *
 * // Create entity with Position component
 * ecs_entity_t e = ecs_new_w_id(world, pos);
 * @endcode
 *
 * Type entities do not have to be named.
 */

#ifdef FLECS_META

/**
 * @defgroup c_addons_meta Meta
 * @ingroup c_addons
 * Flecs reflection framework.
 *
 * @{
 */

#include <stddef.h>

#ifndef FLECS_MODULE
#define FLECS_MODULE
#endif

#ifndef FLECS_META_H
#define FLECS_META_H

#ifdef __cplusplus
extern "C" {
#endif

/** Max number of constants/members that can be specified in desc structs. */
#define ECS_MEMBER_DESC_CACHE_SIZE (32)

/** Primitive type definitions.
 * These typedefs allow the builtin primitives to be used as regular components:
 *
 * @code
 * ecs_set(world, e, ecs_i32_t, {10});
 * @endcode
 *
 * Or a more useful example (create an enum constant with a manual value):
 *
 * @code
 * ecs_set_pair_second(world, e, EcsConstant, ecs_i32_t, {10});
 * @endcode
 */

typedef bool ecs_bool_t;                                        /**< Builtin bool type */
typedef char ecs_char_t;                                        /**< Builtin char type */
typedef unsigned char ecs_byte_t;                               /**< Builtin  ecs_byte type */
typedef uint8_t ecs_u8_t;                                       /**< Builtin u8 type */
typedef uint16_t ecs_u16_t;                                     /**< Builtin u16 type */
typedef uint32_t ecs_u32_t;                                     /**< Builtin u32 type */
typedef uint64_t ecs_u64_t;                                     /**< Builtin u64 type */
typedef uintptr_t ecs_uptr_t;                                   /**< Builtin uptr type */
typedef int8_t ecs_i8_t;                                        /**< Builtin i8 type */
typedef int16_t ecs_i16_t;                                      /**< Builtin i16 type */
typedef int32_t ecs_i32_t;                                      /**< Builtin i32 type */
typedef int64_t ecs_i64_t;                                      /**< Builtin i64 type */
typedef intptr_t ecs_iptr_t;                                    /**< Builtin iptr type */
typedef float ecs_f32_t;                                        /**< Builtin f32 type */
typedef double ecs_f64_t;                                       /**< Builtin f64 type */
typedef char* ecs_string_t;                                     /**< Builtin string type */

/* Meta module component ids */
FLECS_API extern const ecs_entity_t ecs_id(EcsType);            /**< Id for component added to all types with reflection data. */
FLECS_API extern const ecs_entity_t ecs_id(EcsTypeSerializer);  /**< Id for component that stores a type specific serializer. */
FLECS_API extern const ecs_entity_t ecs_id(EcsPrimitive);       /**< Id for component that stores reflection data for a primitive type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsEnum);            /**< Id for component that stores reflection data for an enum type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsBitmask);         /**< Id for component that stores reflection data for a bitmask type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsMember);          /**< Id for component that stores reflection data for struct members. */
FLECS_API extern const ecs_entity_t ecs_id(EcsMemberRanges);    /**< Id for component that stores min/max ranges for member values. */
FLECS_API extern const ecs_entity_t ecs_id(EcsStruct);          /**< Id for component that stores reflection data for a struct type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsArray);           /**< Id for component that stores reflection data for an array type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsVector);          /**< Id for component that stores reflection data for a vector type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsOpaque);          /**< Id for component that stores reflection data for an opaque type. */
FLECS_API extern const ecs_entity_t ecs_id(EcsUnit);            /**< Id for component that stores unit data. */
FLECS_API extern const ecs_entity_t ecs_id(EcsUnitPrefix);      /**< Id for component that stores unit prefix data. */
FLECS_API extern const ecs_entity_t EcsConstant;                /**< Tag added to enum/bitmask constants. */
FLECS_API extern const ecs_entity_t EcsQuantity;                /**< Tag added to unit quantities. */

/* Primitive type component ids */

FLECS_API extern const ecs_entity_t ecs_id(ecs_bool_t);         /**< Builtin boolean type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_char_t);         /**< Builtin char type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_byte_t);         /**< Builtin byte type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_u8_t);           /**< Builtin 8 bit unsigned int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_u16_t);          /**< Builtin 16 bit unsigned int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_u32_t);          /**< Builtin 32 bit unsigned int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_u64_t);          /**< Builtin 64 bit unsigned int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_uptr_t);         /**< Builtin pointer sized unsigned int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_i8_t);           /**< Builtin 8 bit signed int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_i16_t);          /**< Builtin 16 bit signed int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_i32_t);          /**< Builtin 32 bit signed int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_i64_t);          /**< Builtin 64 bit signed int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_iptr_t);         /**< Builtin pointer sized signed int type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_f32_t);          /**< Builtin 32 bit floating point type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_f64_t);          /**< Builtin 64 bit floating point type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_string_t);       /**< Builtin string type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_entity_t);       /**< Builtin entity type. */
FLECS_API extern const ecs_entity_t ecs_id(ecs_id_t);           /**< Builtin (component) id type. */

/** Type kinds supported by meta addon */
typedef enum ecs_type_kind_t {
    EcsPrimitiveType,
    EcsBitmaskType,
    EcsEnumType,
    EcsStructType,
    EcsArrayType,
    EcsVectorType,
    EcsOpaqueType,
    EcsTypeKindLast = EcsOpaqueType
} ecs_type_kind_t;

/** Component that is automatically added to every type with the right kind. */
typedef struct EcsType {
    ecs_type_kind_t kind;  /**< Type kind. */
    bool existing;         /**< Did the type exist or is it populated from reflection */
    bool partial;          /**< Is the reflection data a partial type description */
} EcsType;

/** Primitive type kinds supported by meta addon */
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
    EcsEntity,
    EcsId,
    EcsPrimitiveKindLast = EcsId
} ecs_primitive_kind_t;

/** Component added to primitive types */
typedef struct EcsPrimitive {
    ecs_primitive_kind_t kind;                     /**< Primitive type kind. */
} EcsPrimitive;

/** Component added to member entities */
typedef struct EcsMember {
    ecs_entity_t type;                             /**< Member type. */
    int32_t count;                                 /**< Number of elements (for inline arrays). */
    ecs_entity_t unit;                             /**< Member unit. */
    int32_t offset;                                /**< Member offset. */
} EcsMember;

/** Type expressing a range for a member value */
typedef struct ecs_member_value_range_t {
    double min;                                    /**< Min member value. */
    double max;                                    /**< Max member value. */
} ecs_member_value_range_t;

/** Component added to member entities to express valid value ranges */
typedef struct EcsMemberRanges {
    ecs_member_value_range_t value;                /**< Member value range. */
    ecs_member_value_range_t warning;              /**< Member value warning range. */
    ecs_member_value_range_t error;                /**< Member value error range. */
} EcsMemberRanges;

/** Element type of members vector in EcsStruct */
typedef struct ecs_member_t {
    /** Must be set when used with ecs_struct_desc_t */
    const char *name;

    /** Member type. */
    ecs_entity_t type;

    /** Element count (for inline arrays). May be set when used with ecs_struct_desc_t */
    int32_t count;

    /** May be set when used with ecs_struct_desc_t. Member offset. */
    int32_t offset;

    /** May be set when used with ecs_struct_desc_t, will be auto-populated if
     * type entity is also a unit */
    ecs_entity_t unit;

    /** Numerical range that specifies which values member can assume. This
     * range may be used by UI elements such as a progress bar or slider. The
     * value of a member should not exceed this range. */
    ecs_member_value_range_t range;

    /** Numerical range outside of which the value represents an error. This
     * range may be used by UI elements to style a value. */
    ecs_member_value_range_t error_range;

    /** Numerical range outside of which the value represents an warning. This
     * range may be used by UI elements to style a value. */
    ecs_member_value_range_t warning_range;

    /** Should not be set by ecs_struct_desc_t */
    ecs_size_t size;

    /** Should not be set by ecs_struct_desc_t */
    ecs_entity_t member;
} ecs_member_t;

/** Component added to struct type entities */
typedef struct EcsStruct {
    /** Populated from child entities with Member component */
    ecs_vec_t members; /* vector<ecs_member_t> */
} EcsStruct;

/** Type that describes an enum constant */
typedef struct ecs_enum_constant_t {
    /** Must be set when used with ecs_enum_desc_t */
    const char *name;

    /** May be set when used with ecs_enum_desc_t */
    int32_t value;

    /** Should not be set by ecs_enum_desc_t */
    ecs_entity_t constant;
} ecs_enum_constant_t;

/** Component added to enum type entities */
typedef struct EcsEnum {
    /** Populated from child entities with Constant component */
    ecs_map_t constants; /**< map<i32_t, ecs_enum_constant_t> */
} EcsEnum;

/** Type that describes an bitmask constant */
typedef struct ecs_bitmask_constant_t {
    /** Must be set when used with ecs_bitmask_desc_t */
    const char *name;

    /** May be set when used with ecs_bitmask_desc_t */
    ecs_flags32_t value;

    /** Should not be set by ecs_bitmask_desc_t */
    ecs_entity_t constant;
} ecs_bitmask_constant_t;

/** Component added to bitmask type entities */
typedef struct EcsBitmask {
    /* Populated from child entities with Constant component */
    ecs_map_t constants; /**< map<u32_t, ecs_bitmask_constant_t> */
} EcsBitmask;

/** Component added to array type entities */
typedef struct EcsArray {
    ecs_entity_t type; /**< Element type */
    int32_t count;     /**< Number of elements */
} EcsArray;

/** Component added to vector type entities */
typedef struct EcsVector {
    ecs_entity_t type; /**< Element type */
} EcsVector;


/* Opaque type support */

#if !defined(__cplusplus) || !defined(FLECS_CPP)

/** Serializer interface */
typedef struct ecs_serializer_t {
    /* Serialize value */
    int (*value)(
        const struct ecs_serializer_t *ser, /**< Serializer */
        ecs_entity_t type,                  /**< Type of the value to serialize */
        const void *value);                 /**< Pointer to the value to serialize */

    /* Serialize member */
    int (*member)(
        const struct ecs_serializer_t *ser, /**< Serializer */
        const char *member);                /**< Member name */

    const ecs_world_t *world;               /**< The world. */
    void *ctx;                              /**< Serializer context. */
} ecs_serializer_t;

#elif defined(__cplusplus)

} /* extern "C" { */

/** Serializer interface (same layout as C, but with convenience methods) */
typedef struct ecs_serializer_t {
    /* Serialize value */
    int (*value_)(
        const struct ecs_serializer_t *ser,
        ecs_entity_t type,
        const void *value);

    /* Serialize member */
    int (*member_)(
        const struct ecs_serializer_t *ser,
        const char *name);

    /* Serialize value */
    int value(ecs_entity_t type, const void *value) const;

    /* Serialize value */
    template <typename T>
    int value(const T& value) const;

    /* Serialize member */
    int member(const char *name) const;

    const ecs_world_t *world;
    void *ctx;
} ecs_serializer_t;

extern "C" {
#endif

/** Callback invoked serializing an opaque type. */
typedef int (*ecs_meta_serialize_t)(
    const ecs_serializer_t *ser,
    const void *src);                  /**< Pointer to value to serialize */

/** Opaque type reflection data. 
 * An opaque type is a type with an unknown layout that can be mapped to a type
 * known to the reflection framework. See the opaque type reflection examples.
 */
typedef struct EcsOpaque {
    ecs_entity_t as_type;              /**< Type that describes the serialized output */
    ecs_meta_serialize_t serialize;    /**< Serialize action */

    /* Deserializer interface
     * Only override the callbacks that are valid for the opaque type. If a
     * deserializer attempts to assign a value type that is not supported by the
     * interface, a conversion error is thrown.
     */

    /** Assign bool value */
    void (*assign_bool)(
        void *dst,
        bool value);

    /** Assign char value */
    void (*assign_char)(
        void *dst,
        char value);

    /** Assign int value */
    void (*assign_int)(
        void *dst,
        int64_t value);

    /** Assign unsigned int value */
    void (*assign_uint)(
        void *dst,
        uint64_t value);

    /** Assign float value */
    void (*assign_float)(
        void *dst,
        double value);

    /** Assign string value */
    void (*assign_string)(
        void *dst,
        const char *value);

    /** Assign entity value */
    void (*assign_entity)(
        void *dst,
        ecs_world_t *world,
        ecs_entity_t entity);

    /** Assign (component) id value */
    void (*assign_id)(
        void *dst,
        ecs_world_t *world,
        ecs_id_t id);

    /** Assign null value */
    void (*assign_null)(
        void *dst);

    /** Clear collection elements */
    void (*clear)(
        void *dst);

    /** Ensure & get collection element */
    void* (*ensure_element)(
        void *dst,
        size_t elem);

    /** Ensure & get element */
    void* (*ensure_member)(
        void *dst,
        const char *member);

    /** Return number of elements */
    size_t (*count)(
        const void *dst);

    /** Resize to number of elements */
    void (*resize)(
        void *dst,
        size_t count);
} EcsOpaque;


/* Units */

/** Helper type to describe translation between two units. Note that this
 * is not intended as a generic approach to unit conversions (e.g. from celsius
 * to fahrenheit) but to translate between units that derive from the same base
 * (e.g. meters to kilometers).
 *
 * Note that power is applied to the factor. When describing a translation of
 * 1000, either use {factor = 1000, power = 1} or {factor = 1, power = 3}. */
typedef struct ecs_unit_translation_t {
    int32_t factor; /**< Factor to apply (e.g. "1000", "1000000", "1024") */
    int32_t power;  /**< Power to apply to factor (e.g. "1", "3", "-9") */
} ecs_unit_translation_t;

/** Component that stores unit data. */
typedef struct EcsUnit {
    char *symbol;                                  /**< Unit symbol. */
    ecs_entity_t prefix;                           /**< Order of magnitude prefix relative to derived */
    ecs_entity_t base;                             /**< Base unit (e.g. "meters") */
    ecs_entity_t over;                             /**< Over unit (e.g. "per second") */
    ecs_unit_translation_t translation;            /**< Translation for derived unit */
} EcsUnit;

/** Component that stores unit prefix data. */
typedef struct EcsUnitPrefix {
    char *symbol;                                 /**< Symbol of prefix (e.g. "K", "M", "Ki") */
    ecs_unit_translation_t translation;           /**< Translation of prefix */
} EcsUnitPrefix;


/* Serializer utilities */

/** Serializer instruction opcodes. 
 * The meta type serializer works by generating a flattened array with 
 * instructions that tells a serializer what kind of fields can be found in a
 * type at which offsets.
*/
typedef enum ecs_meta_type_op_kind_t {
    EcsOpArray,
    EcsOpVector,
    EcsOpOpaque,
    EcsOpPush,
    EcsOpPop,

    EcsOpScope, /**< Marks last constant that can open/close a scope */

    EcsOpEnum,
    EcsOpBitmask,

    EcsOpPrimitive, /**< Marks first constant that's a primitive */

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
    EcsOpEntity,
    EcsOpId,
    EcsMetaTypeOpKindLast = EcsOpId
} ecs_meta_type_op_kind_t;

/** Meta type serializer instruction data. */
typedef struct ecs_meta_type_op_t {
    ecs_meta_type_op_kind_t kind;                  /**< Instruction opcode. */
    ecs_size_t offset;                             /**< Offset of current field */
    int32_t count;                                 /**< Number of elements (for inline arrays). */
    const char *name;                              /**< Name of value (only used for struct members) */
    int32_t op_count;                              /**< Number of operations until next field or end */
    ecs_size_t size;                               /**< Size of type of operation */
    ecs_entity_t type;                             /**< Type entity */
    int32_t member_index;                          /**< Index of member in struct */
    ecs_hashmap_t *members;                        /**< string -> member index (structs only) */
} ecs_meta_type_op_t;

/** Component that stores the type serializer.
 * Added to all types with reflection data.
 */
typedef struct EcsTypeSerializer {
    ecs_vec_t ops;      /**< vector<ecs_meta_type_op_t> */
} EcsTypeSerializer;


/* Deserializer utilities */

/** Maximum level of type nesting. 
 * >32 levels of nesting is not sane.
 */
#define ECS_META_MAX_SCOPE_DEPTH (32)

/** Type with information about currently serialized scope. */
typedef struct ecs_meta_scope_t {
    ecs_entity_t type;                             /**< The type being iterated */
    ecs_meta_type_op_t *ops;                       /**< The type operations (see ecs_meta_type_op_t) */
    int32_t op_count;                              /**< Number of operations in ops array to process */
    int32_t op_cur;                                /**< Current operation */
    int32_t elem_cur;                              /**< Current element (for collections) */
    int32_t prev_depth;                            /**< Depth to restore, in case dotmember was used */
    void *ptr;                                     /**< Pointer to the value being iterated */
    const EcsComponent *comp;                      /**< Pointer to component, in case size/alignment is needed */
    const EcsOpaque *opaque;                       /**< Opaque type interface */
    ecs_vec_t *vector;                             /**< Current vector, in case a vector is iterated */
    ecs_hashmap_t *members;                        /**< string -> member index */
    bool is_collection;                            /**< Is the scope iterating elements? */
    bool is_inline_array;                          /**< Is the scope iterating an inline array? */
    bool is_empty_scope;                           /**< Was scope populated (for collections) */
} ecs_meta_scope_t;

/** Type that enables iterating/populating a value using reflection data. */
typedef struct ecs_meta_cursor_t {
    const ecs_world_t *world;                      /**< The world. */
    ecs_meta_scope_t scope[ECS_META_MAX_SCOPE_DEPTH]; /**< Cursor scope stack. */
    int32_t depth;                                 /**< Current scope depth. */
    bool valid;                                    /**< Does the cursor point to a valid field. */
    bool is_primitive_scope;                       /**< If in root scope, this allows for a push for primitive types */

    /** Custom entity lookup action for overriding default ecs_lookup */
    ecs_entity_t (*lookup_action)(const ecs_world_t*, const char*, void*);
    void *lookup_ctx;                              /**< Context for lookup_action */
} ecs_meta_cursor_t;

/** Create meta cursor.
 * A meta cursor allows for walking over, reading and writing a value without
 * having to know its type at compile time.
 * 
 * When a value is assigned through the cursor API, it will get converted to
 * the actual value of the underlying type. This allows the underlying type to
 * change without having to update the serialized data. For example, an integer
 * field can be set by a string, a floating point can be set as integer etc.
 * 
 * @param world The world.
 * @param type The type of the value.
 * @param ptr Pointer to the value.
 * @return A meta cursor for the value.
 */
FLECS_API
ecs_meta_cursor_t ecs_meta_cursor(
    const ecs_world_t *world,
    ecs_entity_t type,
    void *ptr);

/** Get pointer to current field.
 * 
 * @param cursor The cursor.
 * @return A pointer to the current field.
 */
FLECS_API
void* ecs_meta_get_ptr(
    ecs_meta_cursor_t *cursor);

/** Move cursor to next field.
 * 
 * @param cursor The cursor.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_next(
    ecs_meta_cursor_t *cursor);

/** Move cursor to a field.
 * 
 * @param cursor The cursor.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_elem(
    ecs_meta_cursor_t *cursor,
    int32_t elem);

/** Move cursor to member.
 * 
 * @param cursor The cursor.
 * @param name The name of the member.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_member(
    ecs_meta_cursor_t *cursor,
    const char *name);

/** Move cursor to member.
 * Same as ecs_meta_member(), but with support for "foo.bar" syntax.
 * 
 * @param cursor The cursor.
 * @param name The name of the member.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_dotmember(
    ecs_meta_cursor_t *cursor,
    const char *name);

/** Push a scope (required/only valid for structs & collections).
 * 
 * @param cursor The cursor.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_push(
    ecs_meta_cursor_t *cursor);

/** Pop a struct or collection scope (must follow a push).
 * 
 * @param cursor The cursor.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_pop(
    ecs_meta_cursor_t *cursor);

/** Is the current scope a collection?.
 * 
 * @param cursor The cursor.
 * @return True if current scope is a collection, false if not.
 */
FLECS_API
bool ecs_meta_is_collection(
    const ecs_meta_cursor_t *cursor);

/** Get type of current field.
 * 
 * @param cursor The cursor.
 * @return The type of the current field.
 */
FLECS_API
ecs_entity_t ecs_meta_get_type(
    const ecs_meta_cursor_t *cursor);

/** Get unit of current field.
 * 
 * @param cursor The cursor.
 * @return The unit of the current field.
 */
FLECS_API
ecs_entity_t ecs_meta_get_unit(
    const ecs_meta_cursor_t *cursor);

/** Get member name of current field.
 * 
 * @param cursor The cursor.
 * @return The member name of the current field.
 */
FLECS_API
const char* ecs_meta_get_member(
    const ecs_meta_cursor_t *cursor);

/** Get member entity of current field.
 * 
 * @param cursor The cursor.
 * @return The member entity of the current field.
 */
FLECS_API
ecs_entity_t ecs_meta_get_member_id(
    const ecs_meta_cursor_t *cursor);

/* The set functions assign the field with the specified value. If the value
 * does not have the same type as the field, it will be cased to the field type.
 * If no valid conversion is available, the operation will fail. */

/** Set field with boolean value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_bool(
    ecs_meta_cursor_t *cursor,
    bool value);

/** Set field with char value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_char(
    ecs_meta_cursor_t *cursor,
    char value);

/** Set field with int value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_int(
    ecs_meta_cursor_t *cursor,
    int64_t value);

/** Set field with uint value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_uint(
    ecs_meta_cursor_t *cursor,
    uint64_t value);

/** Set field with float value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_float(
    ecs_meta_cursor_t *cursor,
    double value);

/** Set field with string value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_string(
    ecs_meta_cursor_t *cursor,
    const char *value);

/** Set field with string literal value (has enclosing "").
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_string_literal(
    ecs_meta_cursor_t *cursor,
    const char *value);

/** Set field with entity value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_entity(
    ecs_meta_cursor_t *cursor,
    ecs_entity_t value);

/** Set field with (component) id value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_id(
    ecs_meta_cursor_t *cursor,
    ecs_id_t value);

/** Set field with null value.
 * 
 * @param cursor The cursor.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_null(
    ecs_meta_cursor_t *cursor);

/** Set field with dynamic value.
 * 
 * @param cursor The cursor.
 * @param value The value to set.
 * @return Zero if success, non-zero if failed.
 */
FLECS_API
int ecs_meta_set_value(
    ecs_meta_cursor_t *cursor,
    const ecs_value_t *value);

/* Functions for getting members. */

/** Get field value as boolean.
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
bool ecs_meta_get_bool(
    const ecs_meta_cursor_t *cursor);

/** Get field value as char.
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
char ecs_meta_get_char(
    const ecs_meta_cursor_t *cursor);

/** Get field value as signed integer.
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
int64_t ecs_meta_get_int(
    const ecs_meta_cursor_t *cursor);

/** Get field value as unsigned integer.
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
uint64_t ecs_meta_get_uint(
    const ecs_meta_cursor_t *cursor);

/** Get field value as float.
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
double ecs_meta_get_float(
    const ecs_meta_cursor_t *cursor);

/** Get field value as string.
 * This operation does not perform conversions. If the field is not a string,
 * this operation will fail.
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
const char* ecs_meta_get_string(
    const ecs_meta_cursor_t *cursor);

/** Get field value as entity.
 * This operation does not perform conversions. 
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
FLECS_API
ecs_entity_t ecs_meta_get_entity(
    const ecs_meta_cursor_t *cursor);

/** Get field value as (component) id.
 * This operation can convert from an entity. 
 * 
 * @param cursor The cursor.
 * @return The value of the current field.
 */
ecs_id_t ecs_meta_get_id(
    const ecs_meta_cursor_t *cursor);

/** Convert pointer of primitive kind to float. 
 * 
 * @param type_kind The primitive type kind of the value.
 * @param ptr Pointer to a value of a primitive type.
 * @return The value in floating point format.
 */
FLECS_API
double ecs_meta_ptr_to_float(
    ecs_primitive_kind_t type_kind,
    const void *ptr);

/* API functions for creating meta types */

/** Used with ecs_primitive_init(). */
typedef struct ecs_primitive_desc_t {
    ecs_entity_t entity;       /**< Existing entity to use for type (optional). */
    ecs_primitive_kind_t kind; /**< Primitive type kind. */
} ecs_primitive_desc_t;

/** Create a new primitive type. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_primitive_init(
    ecs_world_t *world,
    const ecs_primitive_desc_t *desc);


/** Used with ecs_enum_init(). */
typedef struct ecs_enum_desc_t {
    ecs_entity_t entity;       /**< Existing entity to use for type (optional). */
    ecs_enum_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE]; /**< Enum constants. */
} ecs_enum_desc_t;

/** Create a new enum type. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_enum_init(
    ecs_world_t *world,
    const ecs_enum_desc_t *desc);


/** Used with ecs_bitmask_init(). */
typedef struct ecs_bitmask_desc_t {
    ecs_entity_t entity;       /**< Existing entity to use for type (optional). */
    ecs_bitmask_constant_t constants[ECS_MEMBER_DESC_CACHE_SIZE]; /**< Bitmask constants. */
} ecs_bitmask_desc_t;

/** Create a new bitmask type. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_bitmask_init(
    ecs_world_t *world,
    const ecs_bitmask_desc_t *desc);


/** Used with ecs_array_init(). */
typedef struct ecs_array_desc_t {
    ecs_entity_t entity;  /**< Existing entity to use for type (optional). */
    ecs_entity_t type;    /**< Element type. */
    int32_t count;        /**< Number of elements. */
} ecs_array_desc_t;

/** Create a new array type. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_array_init(
    ecs_world_t *world,
    const ecs_array_desc_t *desc);


/** Used with ecs_vector_init(). */
typedef struct ecs_vector_desc_t {
    ecs_entity_t entity;  /**< Existing entity to use for type (optional). */
    ecs_entity_t type;    /**< Element type. */
} ecs_vector_desc_t;

/** Create a new vector type. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_vector_init(
    ecs_world_t *world,
    const ecs_vector_desc_t *desc);


/** Used with ecs_struct_init(). */
typedef struct ecs_struct_desc_t {
    ecs_entity_t entity; /**< Existing entity to use for type (optional). */
    ecs_member_t members[ECS_MEMBER_DESC_CACHE_SIZE]; /**< Struct members. */
} ecs_struct_desc_t;

/** Create a new struct type. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_struct_init(
    ecs_world_t *world,
    const ecs_struct_desc_t *desc);


/** Used with ecs_opaque_init(). */
typedef struct ecs_opaque_desc_t {
    ecs_entity_t entity;  /**< Existing entity to use for type (optional). */
    EcsOpaque type;       /**< Type that the opaque type maps to. */
} ecs_opaque_desc_t;

/** Create a new opaque type.
 * Opaque types are types of which the layout doesn't match what can be modelled
 * with the primitives of the meta framework, but which have a structure
 * that can be described with meta primitives. Typical examples are STL types
 * such as std::string or std::vector, types with a nontrivial layout, and types
 * that only expose getter/setter methods.
 *
 * An opaque type is a combination of a serialization function, and a handle to
 * a meta type which describes the structure of the serialized output. For
 * example, an opaque type for std::string would have a serializer function that
 * accesses .c_str(), and with type ecs_string_t.
 *
 * The serializer callback accepts a serializer object and a pointer to the
 * value of the opaque type to be serialized. The serializer has two methods:
 *
 * - value, which serializes a value (such as .c_str())
 * - member, which specifies a member to be serialized (in the case of a struct)
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new type, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_opaque_init(
    ecs_world_t *world,
    const ecs_opaque_desc_t *desc);


/** Used with ecs_unit_init(). */
typedef struct ecs_unit_desc_t {
    /** Existing entity to associate with unit (optional). */
    ecs_entity_t entity;

    /** Unit symbol, e.g. "m", "%", "g". (optional). */
    const char *symbol;

    /** Unit quantity, e.g. distance, percentage, weight. (optional). */
    ecs_entity_t quantity;

    /** Base unit, e.g. "meters" (optional). */
    ecs_entity_t base;

    /** Over unit, e.g. "per second" (optional). */
    ecs_entity_t over;

    /** Translation to apply to derived unit (optional). */
    ecs_unit_translation_t translation;

    /** Prefix indicating order of magnitude relative to the derived unit. If set
     * together with "translation", the values must match. If translation is not
     * set, setting prefix will auto-populate it.
     * Additionally, setting the prefix will enforce that the symbol (if set)
     * is consistent with the prefix symbol + symbol of the derived unit. If the
     * symbol is not set, it will be auto populated. */
    ecs_entity_t prefix;
} ecs_unit_desc_t;

/** Create a new unit. 
 * 
 * @param world The world.
 * @param desc The unit descriptor.
 * @return The new unit, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_unit_init(
    ecs_world_t *world,
    const ecs_unit_desc_t *desc);


/** Used with ecs_unit_prefix_init(). */
typedef struct ecs_unit_prefix_desc_t {
    /** Existing entity to associate with unit prefix (optional). */
    ecs_entity_t entity;

    /** Unit symbol, e.g. "m", "%", "g". (optional). */
    const char *symbol;

    /** Translation to apply to derived unit (optional). */
    ecs_unit_translation_t translation;
} ecs_unit_prefix_desc_t;

/** Create a new unit prefix. 
 * 
 * @param world The world.
 * @param desc The type descriptor.
 * @return The new unit prefix, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_unit_prefix_init(
    ecs_world_t *world,
    const ecs_unit_prefix_desc_t *desc);


/** Create a new quantity. 
 * 
 * @param world The world.
 * @param desc The quantity descriptor.
 * @return The new quantity, 0 if failed.
 */
FLECS_API
ecs_entity_t ecs_quantity_init(
    ecs_world_t *world,
    const ecs_entity_desc_t *desc);

/* Convenience macros */

/** Create a primitive type. */
#define ecs_primitive(world, ...)\
    ecs_primitive_init(world, &(ecs_primitive_desc_t) __VA_ARGS__ )

/** Create an enum type. */
#define ecs_enum(world, ...)\
    ecs_enum_init(world, &(ecs_enum_desc_t) __VA_ARGS__ )

/** Create a bitmask type. */
#define ecs_bitmask(world, ...)\
    ecs_bitmask_init(world, &(ecs_bitmask_desc_t) __VA_ARGS__ )

/** Create an array type. */
#define ecs_array(world, ...)\
    ecs_array_init(world, &(ecs_array_desc_t) __VA_ARGS__ )

/** Create a vector type. */
#define ecs_vector(world, ...)\
    ecs_vector_init(world, &(ecs_vector_desc_t) __VA_ARGS__ )

/** Create an opaque type. */
#define ecs_opaque(world, ...)\
    ecs_opaque_init(world, &(ecs_opaque_desc_t) __VA_ARGS__ )

/** Create a struct type. */
#define ecs_struct(world, ...)\
    ecs_struct_init(world, &(ecs_struct_desc_t) __VA_ARGS__ )

/** Create a unit. */
#define ecs_unit(world, ...)\
    ecs_unit_init(world, &(ecs_unit_desc_t) __VA_ARGS__ )

/** Create a unit prefix. */
#define ecs_unit_prefix(world, ...)\
    ecs_unit_prefix_init(world, &(ecs_unit_prefix_desc_t) __VA_ARGS__ )

/** Create a unit quantity. */
#define ecs_quantity(world, ...)\
    ecs_quantity_init(world, &(ecs_entity_desc_t) __VA_ARGS__ )


/** Meta module import function.
 * Usage:
 * @code
 * ECS_IMPORT(world, FlecsMeta)
 * @endcode
 * 
 * @param world The world.
 */
FLECS_API
void FlecsMetaImport(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#include "meta_c.h"

#endif

/** @} */

#endif
