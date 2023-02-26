/**
 * @file addons/cpp/mixins/meta/impl.hpp
 * @brief Meta implementation.
 */

#pragma once

FLECS_ENUM_LAST(flecs::meta::type_kind_t, flecs::meta::TypeKindLast)
FLECS_ENUM_LAST(flecs::meta::primitive_kind_t, flecs::meta::PrimitiveKindLast)

namespace flecs {
namespace meta {
namespace _ {

/* Type support for entity wrappers */
template <typename EntityType>
inline flecs::opaque<EntityType> flecs_entity_support(flecs::world&) {
    return flecs::opaque<EntityType>()
        .as_type(flecs::Entity)
        .serialize([](const flecs::serializer *ser, const EntityType *data) {
            flecs::entity_t id = data->id();
            return ser->value(flecs::Entity, &id);
        })
        .assign_entity(
            [](EntityType *dst, flecs::world_t *world, flecs::entity_t e) {
                *dst = EntityType(world, e);
            });
}

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
        flecs::_::cpp_type<iptr_t>::init(flecs::Iptr, true);
        ecs_assert(flecs::type_id<iptr_t>() == flecs::Iptr, 
            ECS_INTERNAL_ERROR, NULL);
        // Remove symbol to prevent validation errors, as it doesn't match with 
        // the typename
        ecs_remove_pair(world, flecs::Iptr, ecs_id(EcsIdentifier), EcsSymbol);
    }

    if (!flecs::is_same<u32_t, uptr_t>() && !flecs::is_same<u64_t, uptr_t>()) {
        flecs::_::cpp_type<uptr_t>::init(flecs::Uptr, true);
        ecs_assert(flecs::type_id<uptr_t>() == flecs::Uptr, 
            ECS_INTERNAL_ERROR, NULL);
        // Remove symbol to prevent validation errors, as it doesn't match with 
        // the typename
        ecs_remove_pair(world, flecs::Uptr, ecs_id(EcsIdentifier), EcsSymbol);
    }

    // Register opaque type support for C++ entity wrappers
    world.component<flecs::entity_view>()
        .opaque(flecs_entity_support<flecs::entity_view>);

    world.component<flecs::entity>()
        .opaque(flecs_entity_support<flecs::entity>);
}

} // namespace _

} // namespace meta


inline flecs::entity cursor::get_type() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_type(&m_cursor));
}

inline flecs::entity cursor::get_unit() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_unit(&m_cursor));
}

inline flecs::entity cursor::get_entity() const {
    return flecs::entity(m_cursor.world, ecs_meta_get_entity(&m_cursor));
}

/** Create primitive type */
inline flecs::entity world::primitive(flecs::meta::primitive_kind_t kind) {
    ecs_primitive_desc_t desc = {};
    desc.kind = kind;
    flecs::entity_t eid = ecs_primitive_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, eid);
}

/** Create array type. */
inline flecs::entity world::array(flecs::entity_t elem_id, int32_t array_count) {
    ecs_array_desc_t desc = {};
    desc.type = elem_id;
    desc.count = array_count;
    flecs::entity_t eid = ecs_array_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, eid);
}

/** Create array type. */
template <typename T>
inline flecs::entity world::array(int32_t array_count) {
    return this->array(_::cpp_type<T>::id(m_world), array_count);
}

inline flecs::entity world::vector(flecs::entity_t elem_id) {
    ecs_vector_desc_t desc = {};
    desc.type = elem_id;
    flecs::entity_t eid = ecs_vector_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INVALID_OPERATION, NULL);
    return flecs::entity(m_world, eid);
}

template <typename T>
inline flecs::entity world::vector() {
    return this->vector(_::cpp_type<T>::id(m_world));
}

} // namespace flecs

inline int ecs_serializer_t::value(ecs_entity_t type, const void *v) const {
    return this->value_(this, type, v);
}

template <typename T>
inline int ecs_serializer_t::value(const T& v) const {
    return this->value(flecs::_::cpp_type<T>::id(
        const_cast<flecs::world_t*>(this->world)), &v);
}

inline int ecs_serializer_t::member(const char *name) const {
    return this->member_(this, name);
}
