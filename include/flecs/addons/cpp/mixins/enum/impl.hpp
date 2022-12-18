/**
 * @file addons/cpp/mixins/enum/impl.hpp
 * @brief Enum implementation.
 */

#pragma once

namespace flecs {

template <typename E>
inline E entity_view::to_constant() const {
    const E* ptr = this->get<E>();
    ecs_assert(ptr != NULL, ECS_INVALID_PARAMETER, "entity is not a constant");
    return ptr[0];
}

template <typename E, if_t< is_enum<E>::value >>
inline flecs::entity world::to_entity(E constant) const {
    const auto& et = enum_type<E>(m_world);
    return flecs::entity(m_world, et.entity(constant));
}

}