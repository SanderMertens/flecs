#pragma once

namespace flecs {

struct term;
struct term_builder;

template <typename T>
struct term_m : mixin<T> { };

/** Term mixin for flecs::world */
template <>
struct term_m<flecs::world> : mixin<flecs::world> {
  void init() { }

  /** Create a term.
   * 
   */
  template<typename... Args>
  flecs::term term(Args &&... args) const;

  /** Create a term for a (component) type.
   */
  template<typename T, typename... Args>
  flecs::term term(Args &&... args) const;  

  /** Create a term for a pair.
   */
  template<typename R, typename O, typename... Args>
  flecs::term term(Args &&... args) const;  
};

using term_m_world = term_m<flecs::world>;

}
