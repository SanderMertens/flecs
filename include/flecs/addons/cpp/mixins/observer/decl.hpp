#pragma once

namespace flecs {

struct observer;

template<typename ... Components>
class observer_builder;

template<typename T>
struct observer_m : mixin<T> { };

/** Observer mixin for flecs::world. */
template<>
struct observer_m<flecs::world> : mixin<flecs::world> {
  /** Initialize mixin. */
  void init();

  /** Create a new observer.
   * 
   * @tparam Components The components to match on.
   * @tparam Args Arguments passed to the constructor of flecs::observer_builder.
   * @return Observer builder.
   */
  template <typename... Components, typename... Args>
  flecs::observer_builder<Components...> observer(Args &&... args) const;
};

using observer_m_world = observer_m<flecs::world>;

}
