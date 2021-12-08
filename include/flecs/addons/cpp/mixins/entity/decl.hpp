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

  /** Create an entity that's associated with a type.
   */
  template <typename T>
  flecs::entity entity(const char *name = nullptr) const;

  /** Create a prefab that's associated with a type.
   */
  template <typename T>
  flecs::entity prefab(const char *name = nullptr) const;
};

using entity_m_world = entity_m<flecs::world>;

}
