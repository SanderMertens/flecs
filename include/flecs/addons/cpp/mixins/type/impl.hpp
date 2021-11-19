#pragma once

namespace flecs {

#define me_ this->me()

// Type mixin implementation
template <typename... Args>
inline flecs::type type_m_world::type(Args &&... args) const {
    return flecs::type(me_, std::forward<Args>(args)...);
}

#undef me_

}
