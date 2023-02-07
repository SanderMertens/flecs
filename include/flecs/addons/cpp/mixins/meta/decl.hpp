/**
 * @file addons/cpp/mixins/meta/decl.hpp
 * @brief Meta declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @brief Flecs reflection framework.
 * 
 * \ingroup cpp_addons
 * @{
 */

/* Primitive type aliases */
using bool_t = ecs_bool_t;
using char_t = ecs_char_t;
using u8_t = ecs_u8_t;
using u16_t = ecs_u16_t;
using u32_t = ecs_u32_t;
using u64_t = ecs_u64_t;
using uptr_t = ecs_uptr_t;
using i8_t = ecs_i8_t;
using i16_t = ecs_i16_t;
using i32_t = ecs_i32_t;
using i64_t = ecs_i64_t;
using iptr_t = ecs_iptr_t;
using f32_t = ecs_f32_t;
using f64_t = ecs_f64_t;

/* Embedded type aliases */
using member_t = ecs_member_t;
using enum_constant_t = ecs_enum_constant_t;
using bitmask_constant_t = ecs_bitmask_constant_t;

/* Components */
using MetaType = EcsMetaType;
using MetaTypeSerialized = EcsMetaTypeSerialized;
using Primitive = EcsPrimitive;
using Enum = EcsEnum;
using Bitmask = EcsBitmask;
using Member = EcsMember;
using Struct = EcsStruct;
using Array = EcsArray;
using Vector = EcsVector;
using Unit = EcsUnit;

/** Base type for bitmasks */
struct bitmask {
    uint32_t value;
};

/* Handles to builtin reflection types */
static const flecs::entity_t Bool = ecs_id(ecs_bool_t);
static const flecs::entity_t Char = ecs_id(ecs_char_t);
static const flecs::entity_t Byte = ecs_id(ecs_byte_t);
static const flecs::entity_t U8 = ecs_id(ecs_u8_t);
static const flecs::entity_t U16 = ecs_id(ecs_u16_t);
static const flecs::entity_t U32 = ecs_id(ecs_u32_t);
static const flecs::entity_t U64 = ecs_id(ecs_u64_t);
static const flecs::entity_t Uptr = ecs_id(ecs_uptr_t);
static const flecs::entity_t I8 = ecs_id(ecs_i8_t);
static const flecs::entity_t I16 = ecs_id(ecs_i16_t);
static const flecs::entity_t I32 = ecs_id(ecs_i32_t);
static const flecs::entity_t I64 = ecs_id(ecs_i64_t);
static const flecs::entity_t Iptr = ecs_id(ecs_iptr_t);
static const flecs::entity_t F32 = ecs_id(ecs_f32_t);
static const flecs::entity_t F64 = ecs_id(ecs_f64_t);
static const flecs::entity_t String = ecs_id(ecs_string_t);
static const flecs::entity_t Entity = ecs_id(ecs_entity_t);
static const flecs::entity_t Constant = EcsConstant;
static const flecs::entity_t Quantity = EcsQuantity;

/** Serializer object, used for serializing opaque types */
using serializer = ecs_meta_serializer_t;

/** Serializer function, used to serialize opaque types */
using serialize_t = ecs_meta_serialize_t;

/** Type safe variant of serializer function */
template <typename T>
using serialize = int(*)(const serializer *, const T*);

/** Type safe interface for opaque types */
template <typename T, typename ElemType = void>
struct opaque {
    /** Type that describes the type kind/structure of the opaque type */
    flecs::id_t as_type = 0;

    /** Serialize function */
    flecs::serialize<T> serialize = nullptr;

    /* Deserializer interface */

    /** Assign bool value */
    void (*assign_bool)(
        T *dst, 
        bool value) = nullptr;

    /** Assign char value */
    void (*assign_char)(
        T *dst, 
        char value) = nullptr;

    /** Assign int value */
    void (*assign_int)(
        T *dst, 
        int64_t value) = nullptr;

    /** Assign unsigned int value */
    void (*assign_uint)(
        T *dst, 
        uint64_t value) = nullptr;

    /** Assign float value */
    void (*assign_float)(
        T *dst, 
        double value) = nullptr;

    /** Assign string value */
    void (*assign_string)(
        T *dst, 
        const char *value) = nullptr;

    /** Assign entity value */
    void (*assign_entity)(
        T *dst,
        ecs_entity_t entity) = nullptr;

    /** Assign null value */
    void (*assign_null)(
        T *dst) = nullptr;

    /** Clear collection elements */
    void (*clear)(
        T *dst) = nullptr;

    /** Ensure & get collection element */
    ElemType* (*ensure_element)(
        T *dst, 
        size_t elem) = nullptr;

    /** Ensure & get element */
    void* (*ensure_member)(
        T *dst, 
        const char *member) = nullptr;

    /** Return number of elements */
    size_t (*count)(
        T *dst) = nullptr;
    
    /** Resize to number of elements */
    void (*resize)(
        T *dst, 
        size_t count) = nullptr;
};

namespace meta {

/* Type kinds supported by reflection system */
using type_kind_t = ecs_type_kind_t;
static const type_kind_t PrimitiveType = EcsPrimitiveType;
static const type_kind_t BitmaskType = EcsBitmaskType;
static const type_kind_t EnumType = EcsEnumType;
static const type_kind_t StructType = EcsStructType;
static const type_kind_t ArrayType = EcsArrayType;
static const type_kind_t VectorType = EcsVectorType;
static const type_kind_t CustomType = EcsOpaqueType;
static const type_kind_t TypeKindLast = EcsTypeKindLast;

/* Primitive type kinds supported by reflection system */
using primitive_kind_t = ecs_primitive_kind_t;
static const primitive_kind_t Bool = EcsBool;
static const primitive_kind_t Char = EcsChar;
static const primitive_kind_t Byte = EcsByte;
static const primitive_kind_t U8 = EcsU8;
static const primitive_kind_t U16 = EcsU16;
static const primitive_kind_t U32 = EcsU32;
static const primitive_kind_t U64 = EcsU64;
static const primitive_kind_t I8 = EcsI8;
static const primitive_kind_t I16 = EcsI16;
static const primitive_kind_t I32 = EcsI32;
static const primitive_kind_t I64 = EcsI64;
static const primitive_kind_t F32 = EcsF32;
static const primitive_kind_t F64 = EcsF64;
static const primitive_kind_t UPtr = EcsUPtr;
static const primitive_kind_t IPtr = EcsIPtr;
static const primitive_kind_t String = EcsString;
static const primitive_kind_t Entity = EcsEntity;
static const primitive_kind_t PrimitiveKindLast = EcsPrimitiveKindLast;

/** Class for reading/writing dynamic values.
 * 
 * \ingroup cpp_addons_meta
 */
struct cursor {
    cursor(flecs::world_t *world, flecs::entity_t type_id, void *ptr) {
        m_cursor = ecs_meta_cursor(world, type_id, ptr);
    }

    /** Push value scope (such as a nested struct) */
    int push() {
        return ecs_meta_push(&m_cursor);
    }

    /** Pop value scope */
    int pop() {
        return ecs_meta_pop(&m_cursor);
    }

    /** Move to next member/element */
    int next() {
        return ecs_meta_next(&m_cursor);
    }

    /** Move to member by name */
    int member(const char *name) {
        return ecs_meta_member(&m_cursor, name);
    }

    /** Move to element by index */
    int elem(int32_t elem) {
        return ecs_meta_elem(&m_cursor, elem);
    }

    /** Test if current scope is a collection type */
    bool is_collection() {
        return ecs_meta_is_collection(&m_cursor);
    }

    /** Get member name */
    flecs::string_view get_member() const {
        return flecs::string_view(ecs_meta_get_member(&m_cursor));
    }

    /** Get type of value */
    flecs::entity get_type() const;

    /** Get unit of value */
    flecs::entity get_unit() const;

    /** Get untyped pointer to value */
    void* get_ptr() {
        return ecs_meta_get_ptr(&m_cursor);
    }

    /** Set boolean value */
    int set_bool(bool value) {
        return ecs_meta_set_bool(&m_cursor, value);
    }

    /** Set char value */
    int set_char(char value) {
        return ecs_meta_set_char(&m_cursor, value);
    }

    /** Set signed int value */
    int set_int(int64_t value) {
        return ecs_meta_set_int(&m_cursor, value);
    }

    /** Set unsigned int value */
    int set_uint(uint64_t value) {
        return ecs_meta_set_uint(&m_cursor, value);
    }

    /** Set float value */
    int set_float(double value) {
        return ecs_meta_set_float(&m_cursor, value);
    }

    /** Set string value */
    int set_string(const char *value) {
        return ecs_meta_set_string(&m_cursor, value);
    }

    /** Set string literal value */
    int set_string_literal(const char *value) {
        return ecs_meta_set_string_literal(&m_cursor, value);
    }

    /** Set entity value */
    int set_entity(flecs::entity_t value) {
        return ecs_meta_set_entity(&m_cursor, value);
    }

    /** Set null value */
    int set_null() {
        return ecs_meta_set_null(&m_cursor);
    }

    /** Get boolean value */
    bool get_bool() const {
        return ecs_meta_get_bool(&m_cursor);
    }

    /** Get char value */
    char get_char() const {
        return ecs_meta_get_char(&m_cursor);
    }

    /** Get signed int value */
    int64_t get_int() const {
        return ecs_meta_get_int(&m_cursor);
    }

    /** Get unsigned int value */
    uint64_t get_uint() const {
        return ecs_meta_get_uint(&m_cursor);
    }

    /** Get float value */
    double get_float() const {
        return ecs_meta_get_float(&m_cursor);
    }

    /** Get string value */
    const char *get_string() const {
        return ecs_meta_get_string(&m_cursor);
    }

    /** Get entity value */
    flecs::entity get_entity() const;

    /** Cursor object */
    ecs_meta_cursor_t m_cursor;
};

/** @} */

namespace _ {

void init(flecs::world& world);

} // namespace _
} // namespace meta
} // namespace flecs
