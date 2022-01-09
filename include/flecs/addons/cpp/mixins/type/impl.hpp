#pragma once

namespace flecs {

// Type mixin implementation
template <typename... Args>
inline flecs::type world::type(Args &&... args) const {
    return flecs::type(m_world, FLECS_FWD(args)...);
}

template <typename T>
inline flecs::type world::type(const char *name) const {
    flecs::entity result = flecs::component<T>(m_world, name, true);
    return flecs::type(m_world, result);
}

}
