#pragma once

namespace flecs {

#define flecs_me_ this->me()

// Type mixin implementation
template <typename... Args>
inline flecs::type type_m_world::type(Args &&... args) const {
    return flecs::type(flecs_me_, std::forward<Args>(args)...);
}

#undef flecs_me_

}
