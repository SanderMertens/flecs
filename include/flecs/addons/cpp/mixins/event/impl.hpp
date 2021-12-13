#pragma once

#include "builder.hpp"

namespace flecs 
{

// Mixin implementation

inline flecs::event_builder event_m_world::event(flecs::entity_t evt) const {
    return flecs::event_builder(this->me(), evt);
}

template <typename E>
inline flecs::event_builder event_m_world::event() const {
    return flecs::event_builder(this->me(), _::cpp_type<E>().id(this->me()));
}

} // namespace flecs
