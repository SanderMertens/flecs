#pragma once

#include "builder.hpp"

namespace flecs {

struct event_builder;

template <typename E>
struct event_builder_typed;

template<typename T>
struct event_m : mixin<T> { };

/** Event mixin for flecs::world. */
template<>
struct event_m<flecs::world> : mixin<flecs::world> {
  void init() { };

  /** Create a new event.
   * 
   * @param evt The event id.
   * @return Event builder.
   */
  flecs::event_builder event(flecs::entity_t evt) const;

  /** Create a new event.
   * 
   * @tparam E The event type.
   * @return Event builder.
   */
  template <typename E>
  flecs::event_builder_typed<E> event() const;
};

using event_m_world = event_m<flecs::world>;

} // namespace flecs
