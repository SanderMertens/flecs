#pragma once

namespace flecs {

struct trigger;

template<typename ... Components>
class trigger_builder;

template<typename T>
struct trigger_m : mixin<T> { };

/** Observer mixin for flecs::world. */
template<>
struct trigger_m<flecs::world> : mixin<flecs::world> {
  /** Initialize mixin. */
  void init();

  /** Create a new trigger.
   * 
   * @tparam Components The components to match on.
   * @tparam Args Arguments passed to the constructor of flecs::trigger_builder.
   * @return Trigger builder.
   */
  template <typename... Components, typename... Args>
  flecs::trigger_builder<Components...> trigger(Args &&... args) const;
};

using trigger_m_world = trigger_m<flecs::world>;

}
