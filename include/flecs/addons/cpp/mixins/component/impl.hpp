#pragma once

namespace flecs {

#define flecs_me_ this->me()

// Component mixin implementation
template <typename T, typename... Args>
inline flecs::entity component_m_world::component(Args &&... args) const {
    return flecs::component<T>(flecs_me_, std::forward<Args>(args)...);
}

#undef flecs_me_

} // namespace flecs
