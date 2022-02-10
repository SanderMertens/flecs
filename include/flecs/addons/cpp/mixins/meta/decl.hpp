#pragma once

namespace flecs {

using bool_t = ecs_bool_t;
using char_t = ecs_char_t;
using u8_t = ecs_u8_t;
using u16_t = ecs_u16_t;
using u32_t = ecs_u32_t;
using u64_t = ecs_u64_t;
using i8_t = ecs_i8_t;
using i16_t = ecs_i16_t;
using i32_t = ecs_i32_t;
using i64_t = ecs_i64_t;
using f32_t = ecs_f32_t;
using f64_t = ecs_f64_t;

using type_kind_t = ecs_type_kind_t;
using primitive_kind_t = ecs_primitive_kind_t;
using member_t = ecs_member_t;
using enum_constant_t = ecs_enum_constant_t;
using bitmask_constant_t = ecs_bitmask_constant_t;

using MetaType = EcsMetaType;
using MetaTypeSerialized = EcsMetaTypeSerialized;
using Primitive = EcsPrimitive;
using Enum = EcsEnum;
using Bitmask = EcsBitmask;
using Member = EcsMember;
using Struct = EcsStruct;
using Array = EcsArray;
using Vector = EcsVector;

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

namespace meta {

struct cursor {
    cursor(flecs::world_t *world, flecs::entity_t type_id, void *ptr) {
        m_cursor = ecs_meta_cursor(world, type_id, ptr);
    }

    int push() {
        return ecs_meta_push(&m_cursor);
    }

    int pop() {
        return ecs_meta_pop(&m_cursor);
    }

    int next() {
        return ecs_meta_next(&m_cursor);
    }

    int member(const char *name) {
        return ecs_meta_member(&m_cursor, name);
    }

    bool is_collection() {
        return ecs_meta_is_collection(&m_cursor);
    }

    flecs::entity_t get_type() {
        return ecs_meta_get_type(&m_cursor);
    }

    void* get_ptr() {
        return ecs_meta_get_ptr(&m_cursor);
    }

    int set_bool(bool value) {
        return ecs_meta_set_bool(&m_cursor, value);
    }

    int set_char(char value) {
        return ecs_meta_set_char(&m_cursor, value);
    }

    int set_int(int64_t value) {
        return ecs_meta_set_int(&m_cursor, value);
    }

    int set_uint(uint64_t value) {
        return ecs_meta_set_uint(&m_cursor, value);
    }

    int set_float(double value) {
        return ecs_meta_set_float(&m_cursor, value);
    }

    int set_string(const char *value) {
        return ecs_meta_set_string(&m_cursor, value);
    }

    int set_string_literal(const char *value) {
        return ecs_meta_set_string_literal(&m_cursor, value);
    }

    int set_entity(flecs::entity_t value) {
        return ecs_meta_set_entity(&m_cursor, value);
    }

    int set_null() {
        return ecs_meta_set_null(&m_cursor);
    }

    ecs_meta_cursor_t m_cursor;
};

namespace _ {

void init(flecs::world& world);

} // namespace _
} // namespace meta
} // namespace flecs
