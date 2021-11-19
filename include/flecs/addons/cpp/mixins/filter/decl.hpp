#pragma once

namespace flecs {

struct filter_base;

template<typename ... Components>
struct filter;

template<typename ... Components>
struct filter_builder;

template <typename T>
struct filter_m : mixin<T> { };

/** Filter mixin for flecs::world */
template <>
struct filter_m<flecs::world> : mixin<flecs::world> {
  void init() { }

    /** Create a filter.
     * @see ecs_filter_init
     */
    template <typename... Comps, typename... Args>
    flecs::filter<Comps...> filter(Args &&... args) const;

    /** Create a filter builder.
     * @see ecs_filter_init
     */
    template <typename... Comps, typename... Args>
    flecs::filter_builder<Comps...> filter_builder(Args &&... args) const;

    /** Iterate over all entities with components in argument list of function.
     * The function parameter must match the following signature:
     *   void(*)(T&, U&, ...) or
     *   void(*)(flecs::entity, T&, U&, ...)
     */
    template <typename Func>
    void each(Func&& func) const;

    /** Iterate over all entities with provided component.
     * The function parameter must match the following signature:
     *   void(*)(T&) or
     *   void(*)(flecs::entity, T&)
     */
    template <typename T, typename Func>
    void each(Func&& func) const;

    /** Iterate over all entities with provided (component) id.
     */
    template <typename Func>
    void each(flecs::id_t term_id, Func&& func) const;
};

/** Filter mixin for flecs::entity_view */
template <>
struct filter_m<flecs::entity_view> : mixin<flecs::entity_view> {
  /** Return iterator to entity children.
   * Enables depth-first iteration over entity children.
   * 
   * The operation accepts function with the signature:
   *   void(*)(flecs::entity e)
   *
   * @param func Function used to iterate over children.
   */
  template <typename Func>
  inline void children(Func&& func) const;
};

using filter_m_world = filter_m<flecs::world>;
using filter_m_entity_view = filter_m<flecs::entity_view>;

}
