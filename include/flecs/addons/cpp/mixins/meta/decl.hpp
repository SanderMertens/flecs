/**
 * @file addons/cpp/mixins/meta/decl.hpp
 * @brief Meta declarations.
 */

#pragma once

FLECS_API_NAMESPACE namespace flecs {

/**
 * @defgroup cpp_addons_meta Meta
 * @ingroup cpp_addons
 * Flecs reflection framework.
 *
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
using Type = EcsType;
using TypeSerializer = EcsTypeSerializer;
using Primitive = EcsPrimitive;
using Enum = EcsEnum;
using Bitmask = EcsBitmask;
using Member = EcsMember;
using MemberRanges = EcsMemberRanges;
using Struct = EcsStruct;
using Array = EcsArray;
using Vector = EcsVector;
using Unit = EcsUnit;

/** Base type for bitmasks */
struct bitmask {
    uint32_t value;
};

/* Handles to builtin reflection types */
FLECS_STATIC_IN_HEADER const flecs::entity_t Bool = ecs_id(ecs_bool_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t Char = ecs_id(ecs_char_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t Byte = ecs_id(ecs_byte_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t U8 = ecs_id(ecs_u8_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t U16 = ecs_id(ecs_u16_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t U32 = ecs_id(ecs_u32_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t U64 = ecs_id(ecs_u64_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t Uptr = ecs_id(ecs_uptr_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t I8 = ecs_id(ecs_i8_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t I16 = ecs_id(ecs_i16_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t I32 = ecs_id(ecs_i32_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t I64 = ecs_id(ecs_i64_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t Iptr = ecs_id(ecs_iptr_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t F32 = ecs_id(ecs_f32_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t F64 = ecs_id(ecs_f64_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t String = ecs_id(ecs_string_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t Entity = ecs_id(ecs_entity_t);
FLECS_STATIC_IN_HEADER const flecs::entity_t Constant = EcsConstant;
FLECS_STATIC_IN_HEADER const flecs::entity_t Quantity = EcsQuantity;

namespace meta {

/* Type kinds supported by reflection system */
using type_kind_t = ecs_type_kind_t;
FLECS_STATIC_IN_HEADER const type_kind_t PrimitiveType = EcsPrimitiveType;
FLECS_STATIC_IN_HEADER const type_kind_t BitmaskType = EcsBitmaskType;
FLECS_STATIC_IN_HEADER const type_kind_t EnumType = EcsEnumType;
FLECS_STATIC_IN_HEADER const type_kind_t StructType = EcsStructType;
FLECS_STATIC_IN_HEADER const type_kind_t ArrayType = EcsArrayType;
FLECS_STATIC_IN_HEADER const type_kind_t VectorType = EcsVectorType;
FLECS_STATIC_IN_HEADER const type_kind_t CustomType = EcsOpaqueType;
FLECS_STATIC_IN_HEADER const type_kind_t TypeKindLast = EcsTypeKindLast;

/* Primitive type kinds supported by reflection system */
using primitive_kind_t = ecs_primitive_kind_t;
FLECS_STATIC_IN_HEADER const primitive_kind_t Bool = EcsBool;
FLECS_STATIC_IN_HEADER const primitive_kind_t Char = EcsChar;
FLECS_STATIC_IN_HEADER const primitive_kind_t Byte = EcsByte;
FLECS_STATIC_IN_HEADER const primitive_kind_t U8 = EcsU8;
FLECS_STATIC_IN_HEADER const primitive_kind_t U16 = EcsU16;
FLECS_STATIC_IN_HEADER const primitive_kind_t U32 = EcsU32;
FLECS_STATIC_IN_HEADER const primitive_kind_t U64 = EcsU64;
FLECS_STATIC_IN_HEADER const primitive_kind_t I8 = EcsI8;
FLECS_STATIC_IN_HEADER const primitive_kind_t I16 = EcsI16;
FLECS_STATIC_IN_HEADER const primitive_kind_t I32 = EcsI32;
FLECS_STATIC_IN_HEADER const primitive_kind_t I64 = EcsI64;
FLECS_STATIC_IN_HEADER const primitive_kind_t F32 = EcsF32;
FLECS_STATIC_IN_HEADER const primitive_kind_t F64 = EcsF64;
FLECS_STATIC_IN_HEADER const primitive_kind_t UPtr = EcsUPtr;
FLECS_STATIC_IN_HEADER const primitive_kind_t IPtr = EcsIPtr;
FLECS_STATIC_IN_HEADER const primitive_kind_t String = EcsString;
FLECS_STATIC_IN_HEADER const primitive_kind_t Entity = EcsEntity;
FLECS_STATIC_IN_HEADER const primitive_kind_t PrimitiveKindLast = EcsPrimitiveKindLast;

/** @} */
} // namespace meta
} // namespace flecs

namespace flecs {
namespace meta {
namespace _ {

void init(flecs::world& world);

} // namespace _
} // namespace meta
} // namespace flecs

#include "cursor.hpp"
#include "opaque.hpp"
