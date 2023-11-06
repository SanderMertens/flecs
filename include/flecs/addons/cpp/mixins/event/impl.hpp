/**
 * @file addons/cpp/mixins/event/impl.hpp
 * @brief Event implementation.
 */

#pragma once

#include "builder.hpp"

namespace flecs 
{

// Mixin implementation

inline flecs::event_builder world::event(flecs::entity_t evt) const {
    return flecs::event_builder(m_world, evt);
}

template <typename E>
inline flecs::event_builder_typed<E> world::event() const {
    return flecs::event_builder_typed<E>(m_world, _::cpp_type<E>().id(m_world));
}

template <typename Self>
template <typename Func>
inline Self& entity_builder<Self>::observe(flecs::entity_t evt, Func&& f) {
    flecs::world(m_world).observer()
      .event(evt)
      .with(flecs::Any).src(m_id)
      .iter(f)
      .child_of(m_id);

    return to_base();
}

template <typename Self>
template <typename Evt, typename Func>
inline Self& entity_builder<Self>::observe(Func&& f) {
    return this->observe(_::cpp_type<Evt>::id(m_world), f);
}

} // namespace flecs
