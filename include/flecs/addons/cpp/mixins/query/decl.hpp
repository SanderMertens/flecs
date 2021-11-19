#pragma once

namespace flecs {

class query_base;

template<typename ... Components>
class query;

template<typename ... Components>
class query_builder;

template <typename T>
struct query_m : mixin<T> { };

/** Query mixin for flecs::world */
template <>
struct query_m<flecs::world> : mixin<flecs::world> {
  void init() { }

    /** Create a query.
     * @see ecs_query_init
     */
    template <typename... Comps, typename... Args>
    flecs::query<Comps...> query(Args &&... args) const;

    /** Create a subquery.
     * @see ecs_query_init
     */
    template <typename... Comps, typename... Args>
    flecs::query<Comps...> query(flecs::query_base& parent, Args &&... args) const;

    /** Create a query builder.
     * @see ecs_query_init
     */
    template <typename... Comps, typename... Args>
    flecs::query_builder<Comps...> query_builder(Args &&... args) const;
};

using query_m_world = query_m<flecs::world>;

}
