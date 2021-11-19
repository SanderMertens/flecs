#pragma once

namespace flecs {

#define me_ this->me()

// Component mixin implementation
template <typename T, typename... Args>
inline flecs::entity component_m_world::component(Args &&... args) const {
    return flecs::component<T>(me_, std::forward<Args>(args)...);
}

#undef me_

} // namespace flecs
