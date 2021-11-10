#pragma once

namespace flecs {

using TickSource = EcsTickSource;

class system;

template<typename ... Components>
class system_builder;

template<typename T>
struct system_m : mixin<T> { };

/** System mixin for flecs::world. */
template<>
struct system_m<flecs::world> : mixin<flecs::world> {
  /** Initialize mixin. */
  void init();

  /** Upcast entity to a system.
   * The provided entity must be a system.
   * 
   * @param e The entity.
   * @return A system object.
   */
  flecs::system system(flecs::entity e) const;

  /** Create a new system.
   * 
   * @tparam Components The components to match on.
   * @tparam Args Arguments to pass into the constructor of flecs::system.
   * @return System builder.
   */
  template <typename... Components, typename... Args>
  flecs::system_builder<Components...> system(Args &&... args) const;
};

using system_m_world = system_m<flecs::world>;

}
