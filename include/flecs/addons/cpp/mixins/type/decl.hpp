#pragma once

namespace flecs {

struct type;

template <typename T>
struct type_m : mixin<T> { };

/** Type mixin for flecs::world */
template <>
struct type_m<flecs::world> : mixin<flecs::world> {
  void init() { }

  /** Create a type.
   */
  template <typename... Args>
  flecs::type type(Args &&... args) const;      
};

using type_m_world = type_m<flecs::world>;

}
