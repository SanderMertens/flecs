#pragma once

namespace flecs {

#define flecs_me_ this->me()

// Type mixin implementation
template <typename... Args>
inline flecs::type type_m_world::type(Args &&... args) const {
    return flecs::type(flecs_me_, std::forward<Args>(args)...);
}

template <typename T>
inline flecs::type type_m_world::type(const char *name) const {
    flecs::entity result = flecs::component<T>(flecs_me_, name, true);
    return flecs::type(flecs_me_, result);
}

#undef flecs_me_

}
