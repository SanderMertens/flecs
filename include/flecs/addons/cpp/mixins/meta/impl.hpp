/**
 * @file addons/cpp/mixins/meta/impl.hpp
 * @brief Meta implementation.
 */

#pragma once

FLECS_ENUM_LAST(flecs::type_kind_t, EcsTypeKindLast)
FLECS_ENUM_LAST(flecs::primitive_kind_t, EcsPrimitiveKindLast)

namespace flecs {
namespace meta {
namespace _ {

inline void init(flecs::world& world) {
    world.component<bool_t>("flecs::meta::bool");
    world.component<char_t>("flecs::meta::char");
    world.component<u8_t>("flecs::meta::u8");
    world.component<u16_t>("flecs::meta::u16");
    world.component<u32_t>("flecs::meta::u32");
    world.component<u64_t>("flecs::meta::u64");
    world.component<i8_t>("flecs::meta::i8");
    world.component<i16_t>("flecs::meta::i16");
    world.component<i32_t>("flecs::meta::i32");
    world.component<i64_t>("flecs::meta::i64");
    world.component<f32_t>("flecs::meta::f32");
    world.component<f64_t>("flecs::meta::f64");

    world.component<type_kind_t>("flecs::meta::type_kind");
    world.component<primitive_kind_t>("flecs::meta::primitive_kind");
    world.component<member_t>("flecs::meta::member");
    world.component<enum_constant_t>("flecs::meta::enum_constant");
    world.component<bitmask_constant_t>("flecs::meta::bitmask_constant");

    world.component<MetaType>("flecs::meta::MetaType");
    world.component<MetaTypeSerialized>("flecs::meta::MetaTypeSerialized");
    world.component<Primitive>("flecs::meta::Primitive");
    world.component<Enum>("flecs::meta::Enum");
    world.component<Bitmask>("flecs::meta::Bitmask");
    world.component<Member>("flecs::meta::Member");
    world.component<Struct>("flecs::meta::Struct");
    world.component<Array>("flecs::meta::Array");
    world.component<Vector>("flecs::meta::Vector");

    world.component<Unit>("flecs::meta::Unit");

    // To support member<uintptr_t> and member<intptr_t> register components
    // (that do not have conflicting symbols with builtin ones) for platform
    // specific types.

    if (!flecs::is_same<i32_t, iptr_t>() && !flecs::is_same<i64_t, iptr_t>()) {
        flecs::_::cpp_type<iptr_t>::init(world, flecs::Iptr, true);
        ecs_assert(flecs::type_id<iptr_t>() == flecs::Iptr, 
            ECS_INTERNAL_ERROR, NULL);
        // Remove symbol to prevent validation errors, as it doesn't match with 
        // the typename
        ecs_remove_pair(world, flecs::Iptr, ecs_id(EcsIdentifier), EcsSymbol);
    }

    if (!flecs::is_same<u32_t, uptr_t>() && !flecs::is_same<u64_t, uptr_t>()) {
        flecs::_::cpp_type<uptr_t>::init(world, flecs::Uptr, true);
        ecs_assert(flecs::type_id<uptr_t>() == flecs::Uptr, 
            ECS_INTERNAL_ERROR, NULL);
        // Remove symbol to prevent validation errors, as it doesn't match with 
        // the typename
        ecs_remove_pair(world, flecs::Uptr, ecs_id(EcsIdentifier), EcsSymbol);
    }
}

} // namespace _

inline flecs::entity cursor::get_type() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_type(&m_cursor));
}

inline flecs::entity cursor::get_unit() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_unit(&m_cursor));
}

inline flecs::entity cursor::get_entity() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_entity(&m_cursor));
}

} // namespace meta
} // namespace flecs
