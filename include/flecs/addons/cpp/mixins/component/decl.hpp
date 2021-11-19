#pragma once

namespace flecs {

template <typename T>
struct component_m : mixin<T> { };

/** Component mixin for flecs::world */
template <>
struct component_m<flecs::world> : mixin<flecs::world> {
  void init() { }

  /** Register a component.
   */
  template <typename T, typename... Args>
  flecs::entity component(Args &&... args) const;
};

using component_m_world = component_m<flecs::world>;

}
