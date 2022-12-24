/**
 * @file addons/cpp/mixins/component/impl.hpp
 * @brief Component mixin implementation
 */

#pragma once

namespace flecs {

template <typename T, typename... Args>
inline flecs::component<T> world::component(Args &&... args) const {
    return flecs::component<T>(m_world, FLECS_FWD(args)...);
}

template <typename... Args>
inline flecs::untyped_component world::component(Args &&... args) const {
    return flecs::untyped_component(m_world, FLECS_FWD(args)...);
}

} // namespace flecs
