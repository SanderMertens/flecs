/**
 * @file addons/cpp/mixins/prefab/impl.hpp
 * @brief Prefab implementation.
 */

#pragma once

namespace flecs {

template <typename... Args>
inline flecs::entity world::prefab(Args &&... args) const {
    flecs::entity result = flecs::entity(world_, FLECS_FWD(args)...);
    result.add(flecs::Prefab);
    return result;
}

template <typename T>
inline flecs::entity world::prefab(const char *name) const {
    flecs::entity result = this->entity<T>(name);
    result.add(flecs::Prefab);
    return result;
}

}
