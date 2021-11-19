#pragma once

namespace flecs {

struct entity_view;
struct entity;
struct prefab;

template <typename T>
struct entity_m : mixin<T> { };

/** Query mixin for flecs::world */
template <>
struct entity_m<flecs::world> : mixin<flecs::world> {
  void init() { }

  /** Create an entity.
   */
  template <typename... Args>
  flecs::entity entity(Args &&... args) const;

  /** Create a prefab.
   */
  template <typename... Args>
  flecs::entity prefab(Args &&... args) const;
};

using entity_m_world = entity_m<flecs::world>;

}
