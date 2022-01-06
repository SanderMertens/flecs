#pragma once

namespace flecs {

// Component mixin implementation
template <typename T, typename... Args>
inline flecs::entity world::component(Args &&... args) const {
    return flecs::component<T>(m_world, std::forward<Args>(args)...);
}

} // namespace flecs
